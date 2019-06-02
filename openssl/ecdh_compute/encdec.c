// https://stackoverflow.com/questions/1152555/encrypting-decrypting-text-strings-using-openssl-ecc/3449701#3449701
// https://wiki.openssl.org/index.php/Elliptic_Curve_Diffie_Hellman

#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/pem.h>


EC_KEY *load_key(char *path)
{
EC_KEY *key = NULL;

    FILE *fp = fopen(path, "r");
    if(!fp) {
	printf("Error reading private key file\n");
	exit(-1);
    }

    if(!PEM_read_ECPrivateKey(fp, &key, NULL, NULL)) {
	printf("Error importing key content from file\n");
	exit(-1);
    }
    fclose(fp);
    return key;
}


EC_KEY *load_pubkey(char *path)
{
EC_KEY *key = NULL;

    FILE *fp = fopen(path, "r");
    if(!fp) {
	printf("Error reading public key file\n");
	exit(-1);
    }

    if(!PEM_read_EC_PUBKEY(fp, &key, NULL, NULL)) {
	printf("Error importing public key content from file\n");
	exit(-1);
    }
    fclose(fp);
    return key;
}


void newkey(char *curve,char *key,char *pubk)
{
EC_GROUP *ecgroup = NULL;
int rc = 0;
int rc1 = 0;
    FILE *f = fopen(curve,"r");
    if(!f) {
	printf("can\'t load curve");
	exit(-1);
    }
    PEM_read_ECPKParameters(f,&ecgroup,NULL,NULL);
    fclose(f);

    EC_KEY *eckey = EC_KEY_new();
    EC_KEY_set_group(eckey, ecgroup);
    EC_KEY_generate_key(eckey);

    FILE *out = fopen(key, "w");
    if(out) {
        rc = PEM_write_ECPrivateKey(out, eckey, NULL, NULL, 0, NULL, NULL);
	fclose(out);
    } else {
	printf("can\'t save key %s",key);
	exit(-1);
    }

    FILE *out1 = fopen(pubk, "w");
    if(out1) {
        rc1 = PEM_write_EC_PUBKEY(out1, eckey);
        fclose(out1);
    } else {
	printf("can\'t save key %s",pubk);
	exit(-1);
    }
    EC_KEY_free(eckey);

    if(!rc) {
        printf("rc = %d\n",rc);
	exit(-1);
    }
    if(!rc1) {
        printf("rc1 = %d\n",rc1);
	exit(-1);
    }
    
}


void handleErrors()
{
    if (ERR_peek_last_error()) {
    fprintf(stderr, "generated errors:\n");
        ERR_print_errors_fp(stderr);
    }
    exit(-1);
}


unsigned char *ecdh(EC_KEY *ephkey, EC_KEY *peerkey1, size_t *secret_len)
{
EVP_PKEY_CTX *ctx;
unsigned char *secret;
EVP_PKEY *pkey = NULL, *peerkey = NULL;

    pkey = EVP_PKEY_new();
    EVP_PKEY_assign_EC_KEY(pkey, ephkey);

    peerkey = EVP_PKEY_new();
    EVP_PKEY_assign_EC_KEY(peerkey, peerkey1);

    /* Create the context for the shared secret derivation */
    if(NULL == (ctx = EVP_PKEY_CTX_new(pkey, NULL)))
	handleErrors();

    /* Initialise */
    if(1 != EVP_PKEY_derive_init(ctx))
	handleErrors();

    /* Provide the peer public key */
    if(1 != EVP_PKEY_derive_set_peer(ctx, peerkey))
	handleErrors();

    /* Determine buffer length for shared secret */
    if(1 != EVP_PKEY_derive(ctx, NULL, secret_len))
	handleErrors();

    /* Create the buffer */
    if(NULL == (secret = OPENSSL_malloc(*secret_len)))
	handleErrors();

    /* Derive the shared secret */
    if(1 != (EVP_PKEY_derive(ctx, secret, secret_len)))
	handleErrors();

    EVP_PKEY_CTX_free(ctx);

    /* Never use a derived secret directly. Typically it is passed
     * through some hash function to produce a key */
    return secret;
}


void test()
{
    newkey("curve.pem","e.pem","epub.pem");
    newkey("curve.pem","k.pem","kpub.pem");

    EC_KEY *key = load_pubkey("kpub.pem");
    EC_KEY *ekey = load_key("e.pem");

    size_t secret_len = 0;
    unsigned char *p = ecdh(ekey, key, &secret_len);
    printf("%ld:",secret_len);
    for(int i=0; i < secret_len; i++) {
	printf("%02x",p[i] & 0xff);
    }
    printf("\n");

    OPENSSL_free(p);

    key = load_key("k.pem");
    ekey = load_pubkey("epub.pem");

    secret_len = 0;
    p = ecdh(key, ekey, &secret_len);
    printf("%ld:",secret_len);
    for(int i=0; i < secret_len; i++) {
	printf("%02x",p[i] & 0xff);
    }
    printf("\n");

    OPENSSL_free(p);
}


int main(int argc, char *argv[])
{ 
  /* Load the human readable error strings for libcrypto */
  ERR_load_crypto_strings();

  /* Load all digest and cipher algorithms */
  OpenSSL_add_all_algorithms();

  test();

  /* Clean up */

  /* Removes all digests and ciphers */
  EVP_cleanup();

  /* if you omit the next, a small leak may be left when you make use of the BIO (low level API) for e.g. base64 transformations */
  CRYPTO_cleanup_all_ex_data();

  /* Remove error strings */
  ERR_free_strings();

  return 0;
}
