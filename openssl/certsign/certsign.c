// http://fm4dd.com/openssl/certcreate.htm

/* ------------------------------------------------------------ *
 * file:        certcreate.c                                    *
 * purpose:     Example code for creating OpenSSL certificates  *
 * author:      10/06/2012 Frank4DD                             *
 *                                                              *
 * gcc -o certcreate certcreate.c -lssl -lcrypto                *
 * ------------------------------------------------------------ */

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509v3.h>
#include <openssl/asn1t.h>


void handleErrors()
{
    if (ERR_peek_last_error()) {
    fprintf(stderr, "generated errors:\n");
        ERR_print_errors_fp(stderr);
    }
    exit(-1);
}

/*********** where is the ca certificate .pem file ****************************/
#define CACERT          "./clca.pem"
/*********** where is the ca's private key file *******************************/
#define CAKEY           "./clca-key.pem"

/*********** The password for the ca's private key ****************************/
//#define PASS            "ca-secret"

BIO               *reqbio = NULL;
BIO               *outbio = NULL;
X509                *cert = NULL;
X509_REQ         *certreq = NULL;


struct x_pubkey_st {
    ASN1_INTEGER *num_keys;
    ASN1_BIT_STRING *public_key;
};

typedef struct x_pubkey_st X_PUBKEY;

ASN1_SEQUENCE(X_PUBKEY) = {
        ASN1_SIMPLE(X_PUBKEY, num_keys, ASN1_INTEGER),
        ASN1_SIMPLE(X_PUBKEY, public_key, ASN1_BIT_STRING)
} ASN1_SEQUENCE_END(X_PUBKEY)

IMPLEMENT_ASN1_FUNCTIONS(X_PUBKEY)


int i2d_xpubkey(unsigned char **pp)
{
int ret;
X_PUBKEY *xpk;
    
    if ((xpk = X_PUBKEY_new()) == NULL)
        return -1;

    ASN1_INTEGER_set(xpk->num_keys, 2);
    unsigned char bits[] = { 22, 33 };
    ASN1_BIT_STRING_set(xpk->public_key, bits, 13);

    ret = i2d_X_PUBKEY(xpk, pp);
        
    return ret;
}

X_PUBKEY *d2i_xpubkey(const unsigned char **pp, long length)
{
    const unsigned char *q;
    q = *pp;
    X_PUBKEY *xpk = d2i_X_PUBKEY(NULL, &q, length);
    if (!xpk)
        return NULL;

    return xpk;
}



char request_str[4096] = {0};

int addExtension(X509 *cert, char *name, char *value)
{
    X509_EXTENSION *ex = NULL;
    X509V3_CTX ctx;

    // This sets the 'context' of the extensions. No configuration database
    X509V3_set_ctx_nodb(&ctx);

    // Issuer and subject certs: both the target since it is self signed, no request and no CRL
    X509V3_set_ctx(&ctx, cert, cert, NULL, NULL, 0);
//    ex = X509V3_EXT_conf_nid(NULL, &ctx, nid, value);
    ex = X509V3_EXT_conf(NULL, &ctx, name, value);
    if (!ex) {
	printf("X509V3_EXT_conf failed 1\n");
	handleErrors();
        return 0;
    }

    int result = X509_add_ext(cert, ex, -1);
    if(!result) {
	printf("X509_add_ext failed\n");
	handleErrors();
    }
    X509_EXTENSION_free(ex);

    return (result == 0) ? 1 : 0;
}


int addExtension1(X509 *cert, int nid, char *value)
{
    X509_EXTENSION *ex = NULL;
    X509V3_CTX ctx;

    // This sets the 'context' of the extensions. No configuration database
    X509V3_set_ctx_nodb(&ctx);

    // Issuer and subject certs: both the target since it is self signed, no request and no CRL
    X509V3_set_ctx(&ctx, cert, cert, NULL, NULL, 0);
    ex = X509V3_EXT_conf_nid(NULL, &ctx, nid, value);
//    ex = X509V3_EXT_conf(NULL, &ctx, name, value);
    if (!ex) {
	printf("X509V3_EXT_conf failed 1\n");
	handleErrors();
        return 0;
    }

    int result = X509_add_ext(cert, ex, -1);
    if(!result) {
	printf("X509_add_ext failed\n");
	handleErrors();
    }
    X509_EXTENSION_free(ex);

    return (result == 0) ? 1 : 0;
}


int main()
{
  ASN1_INTEGER                 *aserial = NULL;
  EVP_PKEY                     *ca_privkey, *req_pubkey;
  EVP_MD                       const *digest = NULL;
  X509                         *newcert, *cacert;
  X509_NAME                    *name;
  X509V3_CTX                   ctx;
  FILE                         *fp;
  long                         valid_secs = 31536000;

  /* ---------------------------------------------------------- *
   * These function calls initialize openssl for correct work.  *
   * ---------------------------------------------------------- */
  OpenSSL_add_all_algorithms();
  ERR_load_BIO_strings();
  ERR_load_crypto_strings();

  if (! (fp=fopen("req.pem", "r"))) {
    BIO_printf(outbio, "Error opening req file\n");
    exit -1;
   }

  int l = fread(request_str, 1, sizeof(request_str), fp);
  if(l <= 0) {
    BIO_printf(outbio, "Error reading req file\n");
    exit -1;
  }
  request_str[l] = '\0';
  fclose(fp);

  /* ---------------------------------------------------------- *
   * Create the Input/Output BIO's.                             *
   * ---------------------------------------------------------- */
  outbio  = BIO_new(BIO_s_file());
  outbio = BIO_new_fp(stdout, BIO_NOCLOSE);

  /* ---------------------------------------------------------- *
   * Load the request data in a BIO, then in a x509_REQ struct. *
   * ---------------------------------------------------------- */
  reqbio = BIO_new_mem_buf(request_str, -1);

  if (! (certreq = PEM_read_bio_X509_REQ(reqbio, NULL, NULL, NULL))) {
    BIO_printf(outbio, "Error can't read X509 request data into memory\n");
    exit -1;
   }

  /* -------------------------------------------------------- *
   * Load ithe signing CA Certificate file                    *
   * ---------------------------------------------------------*/
  if (! (fp=fopen(CACERT, "r"))) {
    BIO_printf(outbio, "Error reading CA cert file\n");
    exit -1;
   }

  if(! (cacert = PEM_read_X509(fp,NULL,NULL,NULL))) {
    BIO_printf(outbio, "Error loading CA cert into memory\n");
    exit -1;
   }

  fclose(fp);

  /* -------------------------------------------------------- *
   * Import CA private key file for signing                   *
   * ---------------------------------------------------------*/
  ca_privkey = EVP_PKEY_new();

  if (! (fp = fopen (CAKEY, "r"))) {
    BIO_printf(outbio, "Error reading CA private key file\n");
    exit -1;
   }

  if (! (ca_privkey = PEM_read_PrivateKey( fp, NULL, NULL, NULL /*PASS*/))) {
    BIO_printf(outbio, "Error importing key content from file\n");
    exit -1;
   }

  fclose(fp);

  /* --------------------------------------------------------- *
   * Build Certificate with data from request                  *
   * ----------------------------------------------------------*/
  if (! (newcert=X509_new())) {
    BIO_printf(outbio, "Error creating new X509 object\n");
    exit -1;
   }

  if (X509_set_version(newcert, 2) != 1) {
    BIO_printf(outbio, "Error setting certificate version\n");
    exit -1;
   }

  /* --------------------------------------------------------- *
   * set the certificate serial number here                    *
   * If there is a problem, the value defaults to '0'          *
   * ----------------------------------------------------------*/
  aserial=ASN1_INTEGER_new();
  ASN1_INTEGER_set(aserial, 0);
  if (! X509_set_serialNumber(newcert, aserial)) {
    BIO_printf(outbio, "Error setting serial number of the certificate\n");
    exit -1;
   }

  /* --------------------------------------------------------- *
   * Extract the subject name from the request                 *
   * ----------------------------------------------------------*/
  if (! (name = X509_REQ_get_subject_name(certreq)))
    BIO_printf(outbio, "Error getting subject from cert request\n");

  /* --------------------------------------------------------- *
   * Set the new certificate subject name                      *
   * ----------------------------------------------------------*/
  if (X509_set_subject_name(newcert, name) != 1) {
    BIO_printf(outbio, "Error setting subject name of certificate\n");
    exit -1;
   }

  /* --------------------------------------------------------- *
   * Extract the subject name from the signing CA cert         *
   * ----------------------------------------------------------*/
  if (! (name = X509_get_subject_name(cacert))) {
    BIO_printf(outbio, "Error getting subject from CA certificate\n");
    exit -1;
   }

  /* --------------------------------------------------------- *
   * Set the new certificate issuer name                       *
   * ----------------------------------------------------------*/
  if (X509_set_issuer_name(newcert, name) != 1) {
    BIO_printf(outbio, "Error setting issuer name of certificate\n");
    exit -1;
   }

  /* --------------------------------------------------------- *
   * Extract the public key data from the request              *
   * ----------------------------------------------------------*/
  if (! (req_pubkey=X509_REQ_get_pubkey(certreq))) {
    BIO_printf(outbio, "Error unpacking public key from request\n");
    exit -1;
   }

  /* --------------------------------------------------------- *
   * Optionally: Use the public key to verify the signature    *
   * ----------------------------------------------------------*/
  if (X509_REQ_verify(certreq, req_pubkey) != 1) {
    BIO_printf(outbio, "Error verifying signature on request\n");
    exit -1;
   }

  /* --------------------------------------------------------- *
   * Set the new certificate public key                        *
   * ----------------------------------------------------------*/
  if (X509_set_pubkey(newcert, req_pubkey) != 1) {
    BIO_printf(outbio, "Error setting public key of certificate\n");
    exit -1;
   }

  /* ---------------------------------------------------------- *
   * Set X509V3 start date (now) and expiration date (+365 days)*
   * -----------------------------------------------------------*/
   if (! (X509_gmtime_adj(X509_get_notBefore(newcert),0))) {
      BIO_printf(outbio, "Error setting start time\n");
    exit -1;
   }

   if(! (X509_gmtime_adj(X509_get_notAfter(newcert), valid_secs))) {
      BIO_printf(outbio, "Error setting expiration time\n");
    exit -1;
   }

  /* ----------------------------------------------------------- *
   * Add X509V3 extensions                                       *
   * ------------------------------------------------------------*/

    struct stack_st_X509_EXTENSION *exts = X509_REQ_get_extensions(certreq);
    int n = sk_X509_EXTENSION_num(exts);
    printf("number of ext %d\n",n);

    for(int i=0; i < n; i++) {
        X509_EXTENSION *ext = sk_X509_EXTENSION_value(exts, i);
	ASN1_OBJECT *obj = X509_EXTENSION_get_object(ext);
        char buf[100] = {0};
	OBJ_obj2txt(buf, 100, obj, 1);
        printf("%s\n",buf);
    }

    X509_EXTENSION *ext = sk_X509_EXTENSION_value(exts, 0);

    ASN1_OCTET_STRING *d = X509_EXTENSION_get_data(ext);

    X_PUBKEY *xpk = d2i_xpubkey(&d->data,d->length);
    unsigned char *bits = xpk->public_key->data;
    printf("n = %ld %d %d %d\n", ASN1_INTEGER_get(xpk->num_keys), bits[0], bits[1], xpk->public_key->length);

    addExtension1(newcert,NID_basic_constraints, "CA:FALSE");
    addExtension1(newcert,NID_key_usage, "nonRepudiation, digitalSignature, keyEncipherment");
    addExtension1(newcert,NID_netscape_comment, "Comment");

    addExtension(newcert, "1.2.3.4.5.6", "ASN1:UTF8String:1111");
    addExtension(newcert, "1.2.3.4.5.7", "DER:010203");

  /* ----------------------------------------------------------- *
   * Set digest type, sign new certificate with CA's private key *
   * ------------------------------------------------------------*/
  digest = EVP_sha256();

  if (! X509_sign(newcert, ca_privkey, digest)) {
    BIO_printf(outbio, "Error signing the new certificate\n");
    exit -1;
   }

  /* ------------------------------------------------------------ *
   *  print the certificate                                       *
   * -------------------------------------------------------------*/
  if (! PEM_write_bio_X509(outbio, newcert)) {
    BIO_printf(outbio, "Error printing the signed certificate\n");
    exit -1;
   }

  /* ---------------------------------------------------------- *
   * Free up all structures                                     *
   * ---------------------------------------------------------- */

  EVP_PKEY_free(req_pubkey);
  EVP_PKEY_free(ca_privkey);
  X509_REQ_free(certreq);
  X509_free(newcert);
  BIO_free_all(reqbio);
  BIO_free_all(outbio);
  exit(0);
}
