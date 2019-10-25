/*
 * Sample showing how to do SSH2 connect.
 *
 * The sample code has default values for host name, user name, password
 * and path to copy, but you can specify them on the command line like:
 *
 * ssh2 host -c command -u|-d file
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
FILE *fp;
RSA *rsa;
    fp = fopen(file, "r");
    if(fp == NULL) {
        printf("can\'t load key from %s (%s)\n", file, strerror(errno));
        return NULL;
    }
    rsa = PEM_read_RSAPrivateKey(fp, NULL, NULL, NULL);
    fclose(fp);
    if(!rsa) {
    	print_error();
    	return NULL;
    }
    EVP_PKEY *key = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(key, rsa);
    rsa = NULL;
    return key;
}


EVP_PKEY *load_key(ENGINE **e)
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

    char *pin = "123123";
    ENGINE_ctrl_cmd_string(engine, "PIN", pin, 0);
    ENGINE_ctrl_cmd_string(engine, "VERBOSE", NULL, 0);
    ENGINE_init(engine);
    ENGINE_set_default(engine, ENGINE_METHOD_ALL);

    char *keyName = "00";
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

    //method = "ssh-rsa";    
    //method ="ecdsa-sha2-nistp521";
    //method_len = strlen(method);
    
    char tmp[64] = {0};
    strncpy(tmp, method, method_len);
    printf("read_privatekey %s\n", tmp);
    
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

    printf("sign_cb\n");
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
    printf("sign_cb successfull\n");
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


int get_public_key(RSA *rsa, unsigned char *buf, size_t *buflen)
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


int read_publickey(LIBSSH2_SESSION * session, unsigned char **method,
                    size_t *method_len,
                    unsigned char **pubkeydata,
                    size_t *pubkeydata_len,
                    EVP_PKEY *pkey)
{
    RSA *rsa = EVP_PKEY_get1_RSA(pkey);
    
    size_t buflen = SSHBUF_MAX_BIGNUM;
    unsigned char *buf = malloc(buflen);
    if(get_public_key(rsa, buf, &buflen))
	return -1;

    *method = strdup("ssh-rsa");
    *method_len = 7;

    *pubkeydata = buf;
    *pubkeydata_len = SSHBUF_MAX_BIGNUM-buflen;

//    FILE *f = fopen("key.bin","wb");
//    fwrite(buf, *pubkeydata_len, 1, f);
//    fclose(f);
	
    printf("public key loaded ok\n");
    RSA_free(rsa);
    return 0;
}


int userauth_publickey_token(LIBSSH2_SESSION *session, const char *username, size_t username_len, EVP_PKEY *key)
{
    unsigned char *pubkeydata = NULL;
    size_t pubkeydata_len = 0;
    void *abstract = key;
    int rc;

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
        fprintf(stderr, "Can't open local file %s\n", loclfile);
        return -1;
    }

    stat(loclfile, &fileinfo);

   /* Send a file via scp. The mode parameter must only have permissions! */
    channel = libssh2_scp_send(session, scppath, fileinfo.st_mode & 0777, (unsigned long)fileinfo.st_size);

    if(!channel) {
        char *errmsg;
        int errlen;
        int err = libssh2_session_last_error(session, &errmsg, &errlen, 0);
        fprintf(stderr, "Unable to open a session: (%d) %s\n", err, errmsg);
        return -1;
    }

    fprintf(stderr, "SCP session waiting to send file\n");
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
                fprintf(stderr, "ERROR %d\n", rc);
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
        fprintf(stderr, "Unable to open a session: (%d) %s\n", err, errmsg);
        return -1;
    }

    FILE *local = fopen(loclfile, "wb");
    if(!local) {
        fprintf(stderr, "Can't open local file %s\n", loclfile);
        return -1;
    }

    fprintf(stderr, "SCP session waiting to receive file\n");
    size_t nread = 0;
    do {
        int rc = libssh2_channel_read(channel, mem, sizeof(mem));
	printf("rc = %d\n", rc);
        if(rc < 0) {
            fprintf(stderr, "ERROR %d\n", rc);
            break;
        }
    	if(rc == 0) break;
	
	size_t nwrite = fwrite(mem, 1, rc, local);
        if(nwrite != rc) {
            fprintf(stderr, "ERROR %d\n", rc);
            break;
        }
	nread += rc;
	if(nread >= sb.st_size) break;
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


int shell(LIBSSH2_SESSION *session, char *cmd)
{
    LIBSSH2_CHANNEL *channel;
    /* Request a shell */
    channel = libssh2_channel_open_session(session);
    if(!channel) {
        fprintf(stderr, "Unable to open a session\n");
        return -1;
    }

    /* Some environment variables may be set,
     * It's up to the server which ones it'll allow though
     */
    libssh2_channel_setenv(channel, "FOO", "bar");

    /* Request a terminal with 'vanilla' terminal emulation
     * See /etc/termcap for more options
     */
    if(libssh2_channel_request_pty(channel, "vanilla")) {
        fprintf(stderr, "Failed requesting pty\n");
        return -1;
    }

    /* Open a SHELL on that pty */
    if(libssh2_channel_shell(channel)) {
        fprintf(stderr, "Unable to request shell on allocated pty\n");
        return -1;
    }

//    char *cmd = "show version\nexit\n";
    char str[80] = {0};
    snprintf(str, sizeof(str), "%s\n", cmd);

    libssh2_channel_write(channel, str, strlen(str));
//    sleep(1);
    for(;;) {
        char buf[1024] = {0};
	int rc = libssh2_channel_read(channel, buf, 1024);
        if(rc == 0) break;
	printf("%s\n", buf);
	if(buf[strlen(buf)-2] == '#') break;
    }

    /* At this point the shell can be interacted with using
     * libssh2_channel_read()
     * libssh2_channel_read_stderr()
     * libssh2_channel_write()
     * libssh2_channel_write_stderr()
     *
     * Blocking mode may be (en|dis)abled with: libssh2_channel_set_blocking()
     * If the server send EOF, libssh2_channel_eof() will return non-0
     * To send EOF to the server use: libssh2_channel_send_eof()
     * A channel can be closed with: libssh2_channel_close()
     * A channel can be freed with: libssh2_channel_free()
     */

skip_shell:
    if(channel) {
        libssh2_channel_free(channel);
        channel = NULL;
    }

    /* Other channel types are supported via:
     * libssh2_scp_send()
     * libssh2_scp_recv2()
     * libssh2_channel_direct_tcpip()
     */
    return 0;
}

void usage()
{
    printf("Usage:\n");
    printf("ssh2 IP -cud file|cmd [url] \n");
    printf("\n");
}

int main(int argc, char *argv[])
{
    unsigned long hostaddr;
    int rc, sock, i, auth_pw = 0;
    struct sockaddr_in sin;
    const char *fingerprint;
    char *userauthlist;
    LIBSSH2_SESSION *session;


#ifdef WIN32
    WSADATA wsadata;
    int err;

    err = WSAStartup(MAKEWORD(2, 0), &wsadata);
    if(err != 0) {
        fprintf(stderr, "WSAStartup failed with error: %d\n", err);
        return 1;
    }
#endif

    if(argc < 3) {
	usage();
	return 1;
    }
    hostaddr = inet_addr(argv[1]);

    ENGINE *e = NULL;
    EVP_PKEY *key = load_key(&e);
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
        fprintf(stderr, "libssh2 initialization failed (%d)\n", rc);
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
        fprintf(stderr, "failed to connect!\n");
        return -1;
    }

    /* Create a session instance and start it up. This will trade welcome
     * banners, exchange keys, and setup crypto, compression, and MAC layers
     */
    session = libssh2_session_init();
    rc = libssh2_session_handshake(session, sock);
    if(rc) {
        fprintf(stderr, "Failure establishing SSH session rc=%d\n",rc);
        return -1;
    }

    /* At this point we havn't authenticated. The first thing to do is check
     * the hostkey's fingerprint against our known hosts Your app may have it
     * hard coded, may go to a file, may present it to the user, that's your
     * call
     */
    fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
    fprintf(stderr, "Server key fingerprint: ");
    for(i = 0; i < 20; i++) {
        fprintf(stderr, "%02X ", (unsigned char)fingerprint[i]);
    }
    fprintf(stderr, "\n");

    /* check what authentication methods are available */
    char *username = "user";
    userauthlist = libssh2_userauth_list(session, username, strlen(username));
    fprintf(stderr, "Authentication methods: %s\n", userauthlist);

/*
    if(libssh2_userauth_publickey_fromfile(session, "user", NULL, "key.pem", NULL)) {
        fprintf(stderr, "\tAuthentication by public key failed!\n");
        goto shutdown;
    }
    else {
        fprintf(stderr, "\tAuthentication by public key succeeded.\n");
    }
*/
    rc = userauth_publickey_token(session, username, strlen(username), key);
    if(rc) {
        fprintf(stderr, "\tAuthentication by public key failed rc=%d\n",rc);
        goto shutdown;
    }
    else {
        fprintf(stderr, "\tAuthentication by public key succeeded.\n");
    }

    if(strcmp("-c",argv[2]) == 0) {
	shell(session, argv[3]);
    } else if(strcmp("-u",argv[2]) == 0) {
        if(argc < 4) {
    	    usage();
	    return 1;
	}
	scp_write(session, argv[3], argv[4]);
    } else if(strcmp("-d",argv[2]) == 0) {
        if(argc < 4) {
    	    usage();
	    return 1;
	}
	scp_read(session, argv[3], argv[4]);
    } else {
	printf("Unrecognised option %s\n", argv[2]);
    }

  shutdown:
    EVP_PKEY_free(key);

    libssh2_session_disconnect(session,
                               "Normal Shutdown, Thank you for playing");
    libssh2_session_free(session);

#ifdef WIN32
    closesocket(sock);
#else
    close(sock);
#endif
    fprintf(stderr, "all done!\n");
    if(e) ENGINE_free(e);
    libssh2_exit();

    return 0;
}
