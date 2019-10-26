/*
 * TinySSH client based on libssh2 example code
 *
 * make RSA key
 *  openssl genrsa -out key.pem 2048
 *
 * make EC key
 *  openssl ecparam -name secp521r1 -genkey -noout -out key.pem
 *
 * check key
 *  openssl pkey -in key.pem -text
 *
 * convert key to ssh format
 *  ssh-keygen -f key.pem -y > key.pub
 *
 */

#include "libssh2_config.h"
#include <libssh2.h>
#include <libssh2_sftp.h>

#ifdef HAVE_WINDOWS_H
# include <windows.h>
#endif
#ifdef HAVE_WINSOCK2_H
# include <winsock2.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif
# ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
# ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifdef WIN32
typedef DWORD u_int32_t;
#endif

#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

#include <openssl/engine.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/ec.h>
#include <openssl/pem.h>

#include "libssh2_priv.h"
#include "userauth.h"
#include "session.h"


int error_cb(const char *str, size_t len, void *u)
{
    (void)len;
    (void)u;
    printf("%s\n",str);
    return 0;
}

void print_error()
{
    if(ERR_peek_last_error()) {
        ERR_print_errors_cb(error_cb, NULL);
    }
}


EVP_PKEY *load_key_file(char *file)
{
FILE *fp = NULL;
RSA *rsa = NULL;
    fp = fopen(file, "r");
    if(fp == NULL) {
        printf("can\'t load key from %s (%s)\n", file, strerror(errno));
        return NULL;
    }
    rsa = PEM_read_RSAPrivateKey(fp, NULL, NULL, NULL);
    if(rsa) {
		EVP_PKEY *key = EVP_PKEY_new();
		EVP_PKEY_assign_RSA(key, rsa);
		rsa = NULL;
		return key;
    }
    fseek(fp, 0, SEEK_SET);
    EC_KEY *ec = PEM_read_ECPrivateKey(fp, NULL, NULL, NULL);
    fclose(fp);
    if(!ec) {
    	print_error();
    	return NULL;
    }
    EVP_PKEY *key = EVP_PKEY_new();
    EVP_PKEY_assign_EC_KEY(key, ec);
    ec = NULL;
    return key;
}


EVP_PKEY *load_key(ENGINE **e, char *keyName, char *pin)
{
    OPENSSL_add_all_algorithms_noconf();
    ENGINE_load_dynamic();

    // Setup OpenSSL engine
    ENGINE* engine = ENGINE_by_id("dynamic");
#ifdef WIN32
    char *enginePath = "pkcs11.dll";
    char *modulePath = "opensc-pkcs11.dll";
#else
    char *enginePath = "/usr/lib/x86_64-linux-gnu/engines-1.1/pkcs11.so";
    char *modulePath = "/usr/local/lib/opensc-pkcs11.so";
#endif
    ENGINE_ctrl_cmd_string(engine, "SO_PATH", enginePath, 0);
    ENGINE_ctrl_cmd_string(engine, "LIST_ADD", "1", 0);
    ENGINE_ctrl_cmd_string(engine, "LOAD", NULL, 0);
    ENGINE_ctrl_cmd_string(engine, "MODULE_PATH", modulePath, 0);

    ENGINE_ctrl_cmd_string(engine, "PIN", pin, 0);
    ENGINE_ctrl_cmd_string(engine, "VERBOSE", NULL, 0);
    ENGINE_init(engine);
    ENGINE_set_default(engine, ENGINE_METHOD_ALL);

    EVP_PKEY *evp = ENGINE_load_private_key(engine, keyName, NULL, NULL);
    if(!evp) {
        print_error();
    }
    *e = engine;
    return evp;
}

//=============================================================================

int read_privatekey(LIBSSH2_SESSION * session,
                     const LIBSSH2_HOSTKEY_METHOD ** hostkey_method,
                     void **hostkey_abstract,
                     unsigned char *method, int method_len,
                     EVP_PKEY *key)
{
    const LIBSSH2_HOSTKEY_METHOD **hostkey_methods_avail = libssh2_hostkey_methods();
    
//    char tmp[64] = {0};
//    strncpy(tmp, method, method_len);
//    printf("read_privatekey %s\n", tmp);
    
    *hostkey_method = NULL;
    *hostkey_abstract = NULL;
    while(*hostkey_methods_avail && (*hostkey_methods_avail)->name) {
        if((*hostkey_methods_avail)->initPEM && strncmp((*hostkey_methods_avail)->name, (const char *) method, method_len) == 0) {
            *hostkey_method = *hostkey_methods_avail;
            break;
        }
        hostkey_methods_avail++;
    }
    if(!*hostkey_method) {
        printf("No handler for specified private key\n");
        return -1;
    }

    switch (EVP_PKEY_id(key)) {
          case EVP_PKEY_RSA: {
            RSA *rsa = EVP_PKEY_get1_RSA(key);
            *hostkey_abstract = rsa;
            printf("Found RSA private key\n");
            break;
          }
          case EVP_PKEY_EC: {
            EC_KEY *ec = EVP_PKEY_get1_EC_KEY(key);
            *hostkey_abstract = ec;
            printf("Found EC private key\n");
            break;
          }
          case EVP_PKEY_DSA:
            printf("DSA private key, unsupported\n");
            return -1;
          default:
              printf("Unsupported private key\n");
            return -1;
    }
    
    return 0;
}

static int sign_cb(LIBSSH2_SESSION *session, unsigned char **sig, size_t *sig_len,
              const unsigned char *data, size_t data_len, void **abstract)
{
    EVP_PKEY *privkey = (EVP_PKEY *)(*abstract);
    const LIBSSH2_HOSTKEY_METHOD *privkeyobj = NULL;
    void *hostkey_abstract = NULL;
    struct iovec datavec;
    int rc = 0;

//    printf("sign_cb %p %p %d\n", session, session->userauth_pblc_method, sizeof(*session));
    rc = read_privatekey(session, &privkeyobj, &hostkey_abstract,
                              session->userauth_pblc_method,
                              session->userauth_pblc_method_len,
                              privkey);
    if(rc) {
        printf("read_privatekey failed\n");
        return rc;
    }
    libssh2_prepare_iovec(&datavec, 1);
    datavec.iov_base = (void *)data;
    datavec.iov_len  = data_len;

    if(privkeyobj->signv(session, sig, sig_len, 1, &datavec, &hostkey_abstract)) {
        if(privkeyobj->dtor) {
            privkeyobj->dtor(session, &hostkey_abstract);
        }
        printf("privkeyobj->signv failed\n");
        return -1;
    }

    if(privkeyobj->dtor) {
        privkeyobj->dtor(session, &hostkey_abstract);
    }
//    printf("sign_cb successfull\n");
    return 0;
}


#define POKE_U32(p, v) \
    do { \
	const u_int32_t __v = (v); \
	((u_char *)(p))[0] = (__v >> 24) & 0xff; \
	((u_char *)(p))[1] = (__v >> 16) & 0xff; \
	((u_char *)(p))[2] = (__v >> 8) & 0xff; \
	((u_char *)(p))[3] = __v & 0xff; \
    } while (0)


unsigned char *sshbuf_put_cstring(unsigned char *d, unsigned char *v, size_t len, size_t *buflen)
{
    if(len + 4 >= *buflen) {
    	printf("buffer too small\n");
    	return NULL;
    }
    POKE_U32(d, len);
    if (len != 0)
        memcpy(d + 4, v, len);
    (*buflen) -= (4 + len);
    return d + 4 + len;
}


#define SSHBUF_MAX_BIGNUM 4096

unsigned char *sshbuf_put_bignum2(unsigned char *buf, const BIGNUM *v, size_t *size)
{
    u_char d[SSHBUF_MAX_BIGNUM + 1] = {0};
    int len = BN_num_bytes(v), prepend = 0;

    if (len < 0 || len > SSHBUF_MAX_BIGNUM)
            return NULL;

    *d = '\0';
    if (BN_bn2bin(v, d + 1) != len)
            return NULL;

    /* If MSB is set, prepend a \0 */
    if (len > 0 && (d[1] & 0x80) != 0)
        prepend = 1;

    return sshbuf_put_cstring(buf, d + 1 - prepend, len + prepend, size);
}


int get_rsa_public_key(RSA *rsa, unsigned char *buf, size_t *buflen)
{
const BIGNUM *rsa_n = NULL;
const BIGNUM *rsa_e = NULL;
    RSA_get0_key(rsa, &rsa_n, &rsa_e, NULL);
    buf = sshbuf_put_cstring(buf, "ssh-rsa", 7, buflen);
    buf = sshbuf_put_bignum2(buf, rsa_e, buflen);
    if(!buf)
    	return -1;
    buf = sshbuf_put_bignum2(buf, rsa_n, buflen);
    if(!buf)
    	return -1;
    return 0;
}


const char *sshkey_curve_nid_to_name(int nid)
{
    switch (nid) {
//      case NID_X9_62_prime256v1: return "nistp256";
//      case NID_secp384r1:        return "nistp384";
        case NID_secp521r1:        return "nistp521";
        default:                   return NULL;
    }
}

#define SSHBUF_MAX_ECPOINT (528*2/8+1)

unsigned char *sshbuf_put_ec(unsigned char *buf, const EC_POINT *v, const EC_GROUP *g, size_t *buflen)
{
    u_char d[SSHBUF_MAX_ECPOINT] = {0};
    BN_CTX *bn_ctx = NULL;
    size_t len;
    int ret;

    if ((bn_ctx = BN_CTX_new()) == NULL)
        return NULL;
    if ((len = EC_POINT_point2oct(g, v, POINT_CONVERSION_UNCOMPRESSED, NULL, 0, bn_ctx)) > SSHBUF_MAX_ECPOINT) {
        BN_CTX_free(bn_ctx);
        return NULL;
    }
    if (EC_POINT_point2oct(g, v, POINT_CONVERSION_UNCOMPRESSED,  d, len, bn_ctx) != len) {
        BN_CTX_free(bn_ctx);
        return NULL;
    }
    BN_CTX_free(bn_ctx);
    return sshbuf_put_cstring(buf, d, len, buflen);
}

unsigned char *sshbuf_put_eckey(unsigned char *buf, const EC_KEY *v, size_t *buflen)
{
    return sshbuf_put_ec(buf, EC_KEY_get0_public_key(v), EC_KEY_get0_group(v), buflen);
}


int get_ecdsa_public_key(EC_KEY *key, unsigned char *buf, size_t *buflen)
{
    buf = sshbuf_put_cstring(buf, "ecdsa-sha2-nistp521", 19, buflen);
    if(!buf)
    	return -1;
    const char *curve = sshkey_curve_nid_to_name(EC_GROUP_get_curve_name(EC_KEY_get0_group(key)));
    if(!curve)
    	return -1;
    buf = sshbuf_put_cstring(buf, (unsigned char *)curve, strlen(curve), buflen);
    if(!buf)
    	return -1;
    buf = sshbuf_put_eckey(buf, key, buflen);
    if(!buf)
    	return -1;
    return 0;
}


int read_publickey(LIBSSH2_SESSION * session, unsigned char **method,
                    size_t *method_len,
                    unsigned char **pubkeydata,
                    size_t *pubkeydata_len,
                    EVP_PKEY *pkey)
{
    switch (EVP_PKEY_id(pkey)) {
          case EVP_PKEY_RSA: {
			RSA *rsa = EVP_PKEY_get1_RSA(pkey);
			size_t buflen = SSHBUF_MAX_BIGNUM;
			unsigned char *buf = malloc(buflen);
			if(get_rsa_public_key(rsa, buf, &buflen))
				return -1;
			RSA_free(rsa);
			*method = strdup("ssh-rsa");
			*method_len = 7;
			*pubkeydata = buf;
			*pubkeydata_len = SSHBUF_MAX_BIGNUM-buflen;
            break;
          }
          case EVP_PKEY_EC: {
			EC_KEY *ec = EVP_PKEY_get1_EC_KEY(pkey);
			size_t buflen = SSHBUF_MAX_BIGNUM;
			unsigned char *buf = malloc(buflen);
			if(get_ecdsa_public_key(ec, buf, &buflen))
				return -1;
			EC_KEY_free(ec);
	
			*method = strdup("ecdsa-sha2-nistp521");
			*method_len = 19;
			*pubkeydata = buf;
			*pubkeydata_len = SSHBUF_MAX_BIGNUM-buflen;

//    FILE *f = fopen("key.bin","wb");
//    fwrite(buf, *pubkeydata_len, 1, f);
//    fclose(f);

            break;
          }
          case EVP_PKEY_DSA:
            printf("DSA private key, unsupported\n");
            return -1;
          default:
            printf("Unsupported private key\n");
            return -1;
    }
	
    printf("public key loaded ok\n");
    return 0;
}


int userauth_publickey_token(LIBSSH2_SESSION *session, const char *username, size_t username_len, EVP_PKEY *key)
{
    unsigned char *pubkeydata = NULL;
    size_t pubkeydata_len = 0;
    void *abstract = key;
    int rc = 0;

    if(session->userauth_pblc_state == libssh2_NB_state_idle) {
        rc = read_publickey(session, &session->userauth_pblc_method,
                                 &session->userauth_pblc_method_len,
                                 &pubkeydata, &pubkeydata_len, key);
        if(rc) {
            printf("read_publickey failed\n");
            return rc;
        }
    }

    BLOCK_ADJUST(rc, session, _libssh2_userauth_publickey(session, username, username_len,
                                     pubkeydata, pubkeydata_len,
                                     sign_cb, &abstract));
    if(rc) {
        printf("_libssh2_userauth_publickey failed rc=%d\n", rc);
    }
    
    if(pubkeydata) {
        LIBSSH2_FREE(session, pubkeydata);
    }

    return rc;
}

//==============================================================================

int scp_write(LIBSSH2_SESSION *session, char *loclfile, char *scppath)
{
    char mem[4096] = {0};
    char *ptr = NULL;
    LIBSSH2_CHANNEL *channel = NULL;
    struct stat fileinfo;
    FILE *local = fopen(loclfile, "rb");
    if(!local) {
        printf("Can't open local file %s\n", loclfile);
        return -1;
    }

    stat(loclfile, &fileinfo);

   /* Send a file via scp. The mode parameter must only have permissions! */
    channel = libssh2_scp_send(session, scppath, fileinfo.st_mode & 0777, (unsigned long)fileinfo.st_size);

    if(!channel) {
        char *errmsg;
        int errlen;
        int err = libssh2_session_last_error(session, &errmsg, &errlen, 0);
        printf("Unable to open a session: (%d) %s\n", err, errmsg);
        return -1;
    }

    printf("SCP session waiting to send file\n");
    do {
        size_t nread = fread(mem, 1, sizeof(mem), local);
        if(nread <= 0) {
            /* end of file */
            break;
        }
        ptr = mem;

        do {
            /* write the same data over and over, until error or completion */
            int rc = libssh2_channel_write(channel, ptr, nread);
            if(rc < 0) {
                printf("libssh2_channel_write failed, rc=%d\n", rc);
                break;
            }
            else {
                /* rc indicates how many bytes were written this time */
                ptr += rc;
                nread -= rc;
            }
        } while(nread);

    } while(1);

    fprintf(stderr, "Sending EOF\n");
    libssh2_channel_send_eof(channel);

    fprintf(stderr, "Waiting for EOF\n");
    libssh2_channel_wait_eof(channel);

    fprintf(stderr, "Waiting for channel to close\n");
    libssh2_channel_wait_closed(channel);

    libssh2_channel_free(channel);
    channel = NULL;
    return 0;
}

//==============================================================================

int scp_read(LIBSSH2_SESSION *session, char *scppath, char *loclfile)
{
    char mem[4096] = {0};
    char *ptr = NULL;
    LIBSSH2_CHANNEL *channel = NULL;

    libssh2_struct_stat sb;
    channel = libssh2_scp_recv2(session, scppath, &sb);

    if(!channel) {
        char *errmsg;
        int errlen;
        int err = libssh2_session_last_error(session, &errmsg, &errlen, 0);
        printf("Unable to open a session: (%d) %s\n", err, errmsg);
        return -1;
    }

    FILE *local = fopen(loclfile, "wb");
    if(!local) {
        printf("Can't open local file %s\n", loclfile);
        return -1;
    }

    printf("SCP session waiting to receive file\n");
    size_t nread = 0;
    do {
        int rc = libssh2_channel_read(channel, mem, sizeof(mem));
        if(rc < 0) {
            printf("libssh2_channel_read failed, rc=%d\n", rc);
            break;
        }
    	if(rc == 0) break;
	
    	size_t nwrite = fwrite(mem, 1, rc, local);
        if(nwrite != rc) {
            printf("fwrite failed rc=%d nwrite=%ld\n", rc, nwrite);
            break;
        }
        nread += rc;
        if(nread >= sb.st_size) break;
    } while(1);


    printf("Sending EOF\n");
    libssh2_channel_send_eof(channel);

    printf("Waiting for EOF\n");
    libssh2_channel_wait_eof(channel);


    printf("Waiting for channel to close\n");
    libssh2_channel_wait_closed(channel);

    libssh2_channel_free(channel);
    channel = NULL;
    return 0;
}

//==============================================================================

int shell(LIBSSH2_SESSION *session, char *cmd, char *fno)
{
    LIBSSH2_CHANNEL *channel;
    
    /* Request a shell */
    channel = libssh2_channel_open_session(session);
    if(!channel) {
        printf("Unable to open a session\n");
        return -1;
    }

    /* Some environment variables may be set,
     * It's up to the server which ones it'll allow though
     */
    //libssh2_channel_setenv(channel, "FOO", "bar");

    /* Request a terminal with 'vanilla' terminal emulation
     * See /etc/termcap for more options
     */
    if(libssh2_channel_request_pty(channel, "vanilla")) {
        printf("Failed requesting pty\n");
        goto skip_shell;
    }

    /* Open a SHELL on that pty */
    if(libssh2_channel_shell(channel)) {
        printf("Unable to request shell on allocated pty\n");
        goto skip_shell;
    }

    char str[1024] = {0};
    snprintf(str, sizeof(str), "%s\n", cmd);

    libssh2_channel_write(channel, str, strlen(str));

    FILE *out = NULL;
    if(fno) {
	out = fopen(fno,"w");
	if(!out) {
	    printf("can't create file %s\n", fno);
	    goto skip_shell;
	}
    }
    for( ; ; ) {
        char buf[1024] = {0};
        int rc = libssh2_channel_read(channel, buf, 1024);
        if(rc == 0) break;

        printf("%s\n", buf);
	if(out) {
	    fprintf(out, "%s\n", buf);
	}
        if(buf[strlen(buf)-2] == '#') break;
    }

skip_shell:
    if(out) fclose(out);

    if(channel) {
        libssh2_channel_free(channel);
        channel = NULL;
    }

    return 0;
}


void usage()
{
    printf("Usage:\n");
    printf("\n");
    printf(" ssh2 host -c command [-o file]   execute command, store output to file\n");
    printf(" ssh2 host -u file url            upload file\n");
    printf(" ssh2 host -d url file            download file\n");
    printf("\n");
    printf("Optional paramters:\n");
    printf("  -p pin                          use pin for token (default: 123123)\n");
    printf("  -l label                        use key with label (default: 00)\n");
    printf("  -n username                     login with username (default: root)\n");
    printf("  -h fingerprint                  check server key fingerprint (default: do not check)\n");
    printf("  -v verbose                      print debug information (default: off)\n");
    printf("\n");
    exit(1);
}

//==============================================================================

int main(int argc, char *argv[])
{
unsigned long hostaddr = 0;
int rc = 0, sock = 0, i = 0, auth_pw = 0;
struct sockaddr_in sin;
const char *fingerprint = NULL;
char *userauthlist = NULL;
LIBSSH2_SESSION *session = NULL;
char *username = NULL;
char *pin = NULL;
char *label = NULL;
char *fp = NULL;
char *out = NULL;
int verbose = 0;

#ifdef WIN32
    WSADATA wsadata;
    int err;

    err = WSAStartup(MAKEWORD(2, 0), &wsadata);
    if(err != 0) {
        printf("WSAStartup failed with error: %d\n", err);
        return 1;
    }
#endif

    if(argc < 3) usage();

    hostaddr = inet_addr(argv[1]);
    for(i=2; i < argc; ) {
        if(strcmp("-p", argv[i]) == 0) {
        	if(i+1 >= argc) usage();
        	pin = argv[i+1];
        	i += 2;
        } else if(strcmp("-l", argv[i]) == 0) {
        	if(i+1 >= argc) usage();
        	label = argv[i+1];
        	i += 2;
        } else if(strcmp("-n", argv[i]) == 0) {
        	if(i+1 >= argc) usage();
        	username = argv[i+1];
        	i += 2;
        } else if(strcmp("-h", argv[i]) == 0) {
        	if(i+1 >= argc) usage();
        	fp = argv[i+1];
        	i += 2;
        } else if(strcmp("-o", argv[i]) == 0) {
        	if(i+1 >= argc) usage();
        	out = argv[i+1];
        	i += 2;
        } else if(strcmp("-v", argv[i]) == 0) {
        	verbose = 1;
        	i++;
        } else {
        	i++;
        }
    }
   
    ENGINE *e = NULL;
    EVP_PKEY *key = load_key(&e, label ? label : "00", pin ? pin : "123123");
    if(!key) {
        printf("can't load private key from token\n");
        key = load_key_file("key.pem");
        if(!key) {
        	printf("can't load private key from file\n");
            return 1;
        }
    }

    rc = libssh2_init(0);
    if(rc != 0) {
        printf("libssh2 initialization failed (%d)\n", rc);
        return 1;
    }

    /* Ultra basic "connect to port 22 on localhost".  Your code is
     * responsible for creating the socket establishing the connection
     */
    sock = socket(AF_INET, SOCK_STREAM, 0);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(22);
    sin.sin_addr.s_addr = hostaddr;
    if(connect(sock, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in)) != 0) {
        printf("failed to connect!\n");
        return -1;
    }

    /* Create a session instance and start it up. This will trade welcome
     * banners, exchange keys, and setup crypto, compression, and MAC layers
     */
    session = libssh2_session_init();
    rc = libssh2_session_handshake(session, sock);
    if(rc) {
        printf("Failure establishing SSH session rc=%d\n",rc);
        return -1;
    }

    if(verbose) {
    	libssh2_trace(session, ~0); 
    }
    
    /* At this point we havn't authenticated. The first thing to do is check
     * the hostkey's fingerprint against our known hosts Your app may have it
     * hard coded, may go to a file, may present it to the user, that's your
     * call
     */
    fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
    printf("Server key fingerprint: ");
    for(i = 0; i < 20; i++) {
        printf("%02X%s", (unsigned char)fingerprint[i], (i == 20-1) ? "" : ":");
    }
    printf("\n");
    
    if(fp) {
    	int ok = 1;
    	for(i = 0; i < 20; i++) {
    		char tmp[3] = {0};
            snprintf(tmp, sizeof(tmp), "%02X", (unsigned char)fingerprint[i]);
            if(tmp[0] != fp[i*3] || tmp[1] != fp[i*3+1]) {
            	ok = 0;
            	break;
            }
        }
    	if(!ok) {
    		printf("Wrong server finger print\n");
    		goto shutdown;
    	}
    }

    // check what authentication methods are available
    username = username ? username : "root";
    printf("Username: %s\n", username);
    userauthlist = libssh2_userauth_list(session, username, strlen(username));
    printf("Authentication methods: %s\n", userauthlist);

    rc = userauth_publickey_token(session, username, strlen(username), key);
    if(rc) {
        printf("Authentication by public key failed rc=%d\n",rc);
        goto shutdown;
    }
    else {
        printf("Authentication by public key succeeded.\n");
    }

    rc = 0;
    for(i=2; i < argc; ) {
		if(strcmp("-c", argv[i]) == 0) {
			if(i+1 >= argc) usage();
			rc = shell(session, argv[i+1], out);
		    if(rc) {
		    	printf("operation failed, rc=%d", rc);
		    	break;
		    }
			i += 2;
		} else if(strcmp("-u",argv[i]) == 0) {
			if(i+1 >= argc) usage();
			rc = scp_write(session, argv[i], argv[i+1]);
		    if(rc) {
		    	printf("operation failed, rc=%d", rc);
		    	break;
		    }
			i += 2;
		} else if(strcmp("-d",argv[2]) == 0) {
			if(argc < 4) usage();
			rc = scp_read(session, argv[3], argv[4]);
		    if(rc) {
		    	printf("operation failed, rc=%d", rc);
		    	break;
		    }
			i += 2;
		} else if(strcmp("-p",argv[i]) == 0) {
			i+=2;
		} else if(strcmp("-n",argv[i]) == 0) {
			i+=2;
		} else if(strcmp("-l",argv[i]) == 0) {
			i+=2;
		} else if(strcmp("-v",argv[i]) == 0) {
			i++;
		} else if(strcmp("-h",argv[i]) == 0) {
			i+=2;
	        } else if(strcmp("-o", argv[i]) == 0) {
			i+=2;
		} else {
			printf("Unrecognised option %s\n", argv[i]);
			break;
		}
    }
shutdown:
    EVP_PKEY_free(key);

    libssh2_session_disconnect(session,"Normal Shutdown, Thank you for playing");
    libssh2_session_free(session);

#ifdef WIN32
    closesocket(sock);
#else
    close(sock);
#endif
    printf("done\n");
    if(e) {
    	ENGINE_free(e);
    	e = NULL;
    }
    libssh2_exit();

    return 0;
}
