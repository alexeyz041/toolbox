// verify signature
// openssl dgst -sha256 -verify key-pub.pem -signature signature.bin sign.c

#include <string.h>

#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/evp.h>


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


int sign(char *path)
{
EVP_MD_CTX *mdctx = NULL;
int ret = 0;
unsigned char *s = NULL;
unsigned char **sig = &s;

    EC_KEY *eckey = load_key("key.pem");
    if(!eckey) {
        printf("Failed to load private key\n");
	return -1;
    }
 
    EVP_PKEY *key = EVP_PKEY_new();
    EVP_PKEY_assign_EC_KEY(key, eckey);
    eckey = NULL;

    /* Create the Message Digest Context */
    if(!(mdctx = EVP_MD_CTX_create())) goto err;
 
    /* Initialise the DigestSign operation - SHA-256 has been selected as the message digest function in this example */
     if(1 != EVP_DigestSignInit(mdctx, NULL, EVP_sha256(), NULL, key)) goto err;
 
    /* Call update with the message */

    FILE *file = fopen(path, "rb");
    if(!file) return -1;
    char buffer[4096] = {0};
    for( ; ; ) {
	int bytesRead = fread(buffer, 1, 4096, file);
	if(bytesRead <= 0) break;
        if(1 != EVP_DigestSignUpdate(mdctx, buffer, bytesRead)) goto err;
    }

    fclose(file);

    /* Finalise the DigestSign operation */
    /* First call EVP_DigestSignFinal with a NULL sig parameter to obtain the length of the
      * signature. Length is returned in slen */
    size_t slen = 0;
    if(1 != EVP_DigestSignFinal(mdctx, NULL, &slen)) goto err;

    /* Allocate memory for the signature based on size in slen */
    if(!(*sig = OPENSSL_malloc(sizeof(unsigned char) * slen))) goto err;

    /* Obtain the signature */
    if(1 != EVP_DigestSignFinal(mdctx, *sig, &slen)) goto err;

    printf("sig len %ld\n",slen);

    FILE *f = fopen("signature.bin","wb");
    fwrite(*sig, slen, 1, f);
    fclose(f);

     /* Success */
     ret = 1;
 
err:
     if(ret != 1)  {
	/* Do some error handling */
	printf("signing failed");
    }
 
    /* Clean up */
    if(*sig && !ret) OPENSSL_free(*sig);
    if(mdctx) EVP_MD_CTX_destroy(mdctx);

    return ret;
}


int verify(/*char *hash, int hashlen*/ char *path)
{
unsigned char p[1024] = {0};
const unsigned char *pp = p;
unsigned char *val = p;

    FILE *f = fopen("signature.bin","rb");
    if(!f) return -1;
    size_t vallen = fread(p, 1, sizeof(p), f);
    fclose(f);

    EVP_MD_CTX *mdctx = NULL;
    int ret = 0;

    EC_KEY *eckey = load_pubkey("key-pub.pem");
    if(!eckey) {
        printf("Failed to load public key\n");
	return -1;
    }

    EVP_PKEY *key = EVP_PKEY_new();
    EVP_PKEY_assign_EC_KEY(key, eckey);
    eckey = NULL;

    if(!(mdctx = EVP_MD_CTX_create())) goto err;

    /* Calculate the MAC for the received message */
    if(1 != EVP_DigestVerifyInit(mdctx, NULL, EVP_sha256(), NULL, key)) goto err;

    FILE *file = fopen(path, "rb");
    if(!file) return -1;
    char buffer[4096] = {0};
    for( ; ; ) {
	int bytesRead = fread(buffer, 1, 4096, file);
	if(bytesRead <= 0) break;
        if(1 != EVP_DigestVerifyUpdate(mdctx, buffer, bytesRead)) goto err;
    }
    fclose(file);

    ret = EVP_DigestVerifyFinal(mdctx, val, vallen);
    if(ret == 1)
	printf("verify ok\n");
err:
 
    /* Clean up */
    if(mdctx) EVP_MD_CTX_destroy(mdctx);
    return ret;
}

//------------------------------------------------------------------------------

void digest(char *path)
{
    BIO* bio = BIO_new_file(path, "rb");
    if (bio != NULL) {
	OpenSSL_add_all_digests();
	const EVP_MD* md = EVP_sha256();

	if (md != NULL) {
    	    EVP_MD_CTX* ctx = EVP_MD_CTX_create();

    	    if (EVP_DigestInit_ex(ctx, md, NULL)) {
        	const int bufLength = 4096;
		unsigned char buf[bufLength];
    	        unsigned int len;

		int readlen;
        	while ( (readlen = BIO_read(bio, buf, bufLength)) > 0)
        	{
            	    EVP_DigestUpdate(ctx, buf, readlen);
        	}

        	unsigned char digest[EVP_MAX_MD_SIZE];
        	int ok = EVP_DigestFinal_ex(ctx, digest, &len);
	    
		char str[EVP_MAX_MD_SIZE*2+1];
		printf("sha 256 ");
		for(int i=0; i < len; i++) {
		    printf("%02x",digest[i] & 0xff);
		}
		printf("\n");
	    }
	    EVP_MD_CTX_destroy(ctx);
	}
        BIO_free_all(bio);
    }
}


int main(int argc, char *argv[])
{
int rc = 0;
    digest(argv[1]);

    rc = sign(argv[1]);
    if(!rc) {
	printf("sign failed\n");
	exit(-1);
    }

    rc = verify(argv[1]);
    if(!rc) {
	printf("verify failed\n");
	exit(-1);
    }

    return 0;
}
