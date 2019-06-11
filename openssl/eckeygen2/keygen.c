// view key
//  openssl ec -in key.pem -noout -text

// extract public key
//  openssl ec -in key.pem -pubout -out pubk.pem

// view public key
//  openssl ec -in key-pub2.pem -pubin -noout -text

#include <openssl/obj_mac.h>
#include <openssl/ec.h>
#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/err.h>


char *curve = \
"-----BEGIN EC PARAMETERS-----\n\
MIIBogIBATBMBgcqhkjOPQEBAkEAqt2duNvpxIs/1OauM8n8B8swjbOzydIO1mOc\n\
ynAzCHF9TZsAm8ZoQq7NoSrmo4DmKIH/Ly2CxoUoqmBWWDpI8zCBhARAqt2duNvp\n\
xIs/1OauM8n8B8swjbOzydIO1mOcynAzCHF9TZsAm8ZoQq7NoSrmo4DmKIH/Ly2C\n\
xoUoqmBWWDpI8ARAfLu8+UQc+rduGJDkaITq4yH3DAvLSYFSeJdQS+w+NqYrzfoj\n\
BJdlQPZFAIXy2uFFwiVTtGV2NokYDqJXGGdCPgSBgQRkDs5cEniHF7nBugbLwqb+\n\
uoWEJFjFbd6dsXWNOcAxPYK6UXNc2z6kmap3p9aUOmT3o/Jf4m8GtRuqJpb6kDXa\n\
W1NL1ZX1rw+iyJI3bISs4btOMBm3FjTAETEVnK4DzunZkyGEvu8ha9cd8trfhqYn\n\
MG7P+W27i6zhmLYeAPizMgJBAKrdnbjb6cSLP9TmrjPJ/AfLMI2zs8nSDtZjnMpw\n\
MwhwVT5cQUypJhlBhmEZf6wQRx2x04EIXdrdtYeWgpypAGkCAQE=\n\
-----END EC PARAMETERS-----";


void newkey(char *curve, BUF_MEM **pubk, BUF_MEM **privk)
{
EC_GROUP *ecgroup = NULL;
int rc = 0;
int rc1 = 0;
    BIO *f = BIO_new_mem_buf(curve, -1);
    if(!PEM_read_bio_ECPKParameters(f,&ecgroup,NULL,NULL)) {
	printf("fail 1\n");
	return;
    }
    BIO_free(f);

    EC_KEY *eckey = EC_KEY_new();
    EC_KEY_set_group(eckey, ecgroup);
    EC_KEY_generate_key(eckey);

    BIO *out = BIO_new(BIO_s_mem());
    if(out) {
        rc = PEM_write_bio_ECPrivateKey(out, eckey, NULL, NULL, 0, NULL, NULL);
	BIO_get_mem_ptr(out, privk);
	BIO_set_close(out, BIO_NOCLOSE);
	BIO_free(out);
    } else {
	printf("can\'t save key2");
    }

    BIO *out1 = BIO_new(BIO_s_mem());
    if(out1) {
        rc1 = PEM_write_bio_EC_PUBKEY(out1, eckey);
	BIO_get_mem_ptr(out, pubk);
	BIO_set_close(out, BIO_NOCLOSE);
	BIO_free(out);
    } else {
	printf("can\'t save keypub");
    }
    EC_KEY_free(eckey);

    if(!rc) {
        printf("rc = %d\n",rc);
    }
    if(!rc1) {
        printf("rc1 = %d\n",rc1);
    }
}


EC_KEY *load_key(char *k)
{
EC_KEY *key = NULL;

    BIO *fp = BIO_new_mem_buf(k, -1);
    if(!fp) {
        printf("Error reading private key file\n");
	exit(-1);
    }

    if(!PEM_read_bio_ECPrivateKey(fp, &key, NULL, NULL)) {
        printf("Error importing key content from file\n");
	exit(-1);
    }
    BIO_free(fp);
    return key;
}


EC_KEY *load_pubkey(char *k)
{
EC_KEY *key = NULL;

    BIO *fp = BIO_new_mem_buf(k, -1);
    if(!fp) {
        printf("Error reading public key file\n");
	exit(-1);
    }

    if(!PEM_read_bio_EC_PUBKEY(fp, &key, NULL, NULL)) {
        printf("Error importing public key content from file\n");
	exit(-1);
    }
    BIO_free(fp);
    return key;
}


int main(int argc, char *argv[])
{ 
  /* Load the human readable error strings for libcrypto */
  ERR_load_crypto_strings();

  /* Load all digest and cipher algorithms */
  OpenSSL_add_all_algorithms();

    BUF_MEM *pubk = NULL;
    BUF_MEM *privk = NULL;
    newkey(curve, &pubk, &privk);

    printf("%s\n",pubk->data);
    printf("%s\n",privk->data);

    EC_KEY *k1 = load_key(privk->data);
    EC_KEY *k2 = load_pubkey(pubk->data);

    BUF_MEM_free(privk);
    BUF_MEM_free(pubk);

 EC_KEY_free(k1);
 EC_KEY_free(k2);


  /* Clean up */
  /* Removes all digests and ciphers */
  EVP_cleanup();

  /* if you omit the next, a small leak may be left when you make use of the BIO (low level API) for e.g. base64 transformations */
  CRYPTO_cleanup_all_ex_data();

  /* Remove error strings */
  ERR_free_strings();

  return 0;
}
