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


void newkey()
{
int rc = 0;
    EC_KEY *eckey = EC_KEY_new();
    EC_GROUP *ecgroup = EC_GROUP_new_by_curve_name(NID_secp256k1);
    EC_KEY_set_group(eckey, ecgroup);
    EC_KEY_generate_key(eckey);

    FILE *out = fopen("key.pem", "w");
    if(out) {
	rc = PEM_write_ECPrivateKey(out, eckey, NULL, NULL, 0, NULL, NULL);
	fclose(out);
    } else {
	printf("can\'t save key");
    }
    EC_KEY_free(eckey);
    if(!rc) {
        printf("rc = %d\n",rc);
    }
}


void newkey2()
{
EC_GROUP *ecgroup = NULL;
int rc = 0;
int rc1 = 0;
    FILE *f = fopen("curve.pem","r");
    if(!f) {
	printf("can\'t load curve");
	return;
    }
    PEM_read_ECPKParameters(f,&ecgroup,NULL,NULL);
    fclose(f);

    EC_KEY *eckey = EC_KEY_new();
    EC_KEY_set_group(eckey, ecgroup);
    EC_KEY_generate_key(eckey);

    FILE *out = fopen("key2.pem", "w");
    if(out) {
        rc = PEM_write_ECPrivateKey(out, eckey, NULL, NULL, 0, NULL, NULL);
	fclose(out);
    } else {
	printf("can\'t save key2");
    }

    FILE *out1 = fopen("key-pub2.pem", "w");
    if(out1) {
        rc1 = PEM_write_EC_PUBKEY(out1, eckey);
        fclose(out1);
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



int main(int argc, char *argv[])
{ 
  /* Load the human readable error strings for libcrypto */
  ERR_load_crypto_strings();

  /* Load all digest and cipher algorithms */
  OpenSSL_add_all_algorithms();

  newkey();
  newkey2();


  /* Clean up */
  /* Removes all digests and ciphers */
  EVP_cleanup();

  /* if you omit the next, a small leak may be left when you make use of the BIO (low level API) for e.g. base64 transformations */
  CRYPTO_cleanup_all_ex_data();

  /* Remove error strings */
  ERR_free_strings();

  return 0;
}
