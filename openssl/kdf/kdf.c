#include <openssl/obj_mac.h>
#include <openssl/ec.h>
#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/kdf.h>


int main()
{
    EVP_PKEY_CTX *pctx = NULL;
    unsigned char out[32] = {0};
    size_t outlen = sizeof(out);

    pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, NULL);

    if (EVP_PKEY_derive_init(pctx) <= 0) {
	printf("error 1");
	exit(-1);
    }
	
    if (EVP_PKEY_CTX_set_hkdf_md(pctx, EVP_sha256()) <= 0) {
	printf("error 2");
	exit(-1);
    }

    if (EVP_PKEY_CTX_set1_hkdf_salt(pctx, "salt", 4) <= 0) {
	printf("error 3");
	exit(-1);
    }
    
    if (EVP_PKEY_CTX_set1_hkdf_key(pctx, "secret", 6) <= 0) {
	printf("error 4");
	exit(-1);
    }
    
    if (EVP_PKEY_CTX_add1_hkdf_info(pctx, "label", 6) <= 0) {
	printf("error 5");
	exit(-1);
    }
    
    if (EVP_PKEY_derive(pctx, out, &outlen) <= 0) {
	printf("error 1");
	exit(-1);
    }


    for(int i=0; i < outlen; i++) {
	printf("%02x",out[i]);
    }
    printf("\n");

    return 0;
}
