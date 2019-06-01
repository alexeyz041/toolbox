// https://security.stackexchange.com/questions/184845/how-to-generate-csrcertificate-signing-request-using-c-and-openssl
// view certificate request
// openssl req -noout -text -in req.pem

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>

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


int gen_X509Req()
{
int ret = 0;
int             nVersion = 0;
X509_REQ        *x509_req = NULL;
X509_NAME       *x509_name = NULL;
EVP_PKEY        *pKey = NULL;
RSA             *tem = NULL;
BIO             *out = NULL, *bio_err = NULL;
const char      *szCountry = "FI";
const char      *szProvince = "Pm";
const char      *szCity = "Tre";
const char      *szOrganization = "SlowHacking Co Ltd";
const char      *szCommon = "localhost";
const char      *szPath = "req.pem";

    // 1. Load key
    EC_KEY *r = load_key("key.pem");

    // 2. set version of x509 req
    x509_req = X509_REQ_new();
    ret = X509_REQ_set_version(x509_req, nVersion);
    if (ret != 1){
	goto free_all;
    }

    // 3. set subject of x509 req
    x509_name = X509_REQ_get_subject_name(x509_req);

    ret = X509_NAME_add_entry_by_txt(x509_name,"C", MBSTRING_ASC, (const unsigned char*)szCountry, -1, -1, 0);
    if (ret != 1) {
	goto free_all;
    }

    ret = X509_NAME_add_entry_by_txt(x509_name,"ST", MBSTRING_ASC, (const unsigned char*)szProvince, -1, -1, 0);
    if (ret != 1) {
	goto free_all;
    }

    ret = X509_NAME_add_entry_by_txt(x509_name,"L", MBSTRING_ASC, (const unsigned char*)szCity, -1, -1, 0);
    if (ret != 1) {
	goto free_all;
    }   

    ret = X509_NAME_add_entry_by_txt(x509_name,"O", MBSTRING_ASC, (const unsigned char*)szOrganization, -1, -1, 0);
    if (ret != 1) {
	goto free_all;
    }

    ret = X509_NAME_add_entry_by_txt(x509_name,"CN", MBSTRING_ASC, (const unsigned char*)szCommon, -1, -1, 0);
    if (ret != 1) {
	goto free_all;
    }

    // Add custom extensions
    // text
    struct stack_st_X509_EXTENSION *exts = NULL;
    X509_EXTENSION *ex = NULL;
    
    exts = sk_X509_EXTENSION_new_null();
    ASN1_OCTET_STRING *os = ASN1_OCTET_STRING_new();
    int nid = OBJ_create("1.2.3.4.5.6", "Custom Text", "Custom Text");
    ASN1_OCTET_STRING_set(os, "ABC Corp", 8);
    ex = X509_EXTENSION_create_by_NID(NULL, nid, 0, os);
    sk_X509_EXTENSION_push(exts, ex);

    // binary
    unsigned char data[20] = {1,2,3,4,5,6,7,8,9};

    // build big number from our bytes
    BIGNUM* bn = BN_new();
    BN_bin2bn(data, sizeof(data), bn);
    // build the ASN1_INTEGER from our BIGNUM
    ASN1_INTEGER* asnInt = ASN1_INTEGER_new();
    BN_to_ASN1_INTEGER(bn, asnInt);

    nid = OBJ_create("1.2.3.4.5.7", "Bin data", "Bin data");
    ex = X509_EXTENSION_create_by_NID( NULL, nid, 0, asnInt);
    sk_X509_EXTENSION_push(exts, ex);

    X509_REQ_add_extensions(x509_req, exts);

    //----

    // 4. set public key of x509 req
    pKey = EVP_PKEY_new();
    EVP_PKEY_assign_EC_KEY(pKey, r);
    r = NULL;   // will be free rsa when EVP_PKEY_free(pKey)

    ret = X509_REQ_set_pubkey(x509_req, pKey);
    if (ret != 1) {
	goto free_all;
    }

    // 5. set sign key of x509 req
    ret = X509_REQ_sign(x509_req, pKey, EVP_sha256());    // return x509_req->signature->length
    if (ret <= 0) {
	goto free_all;
    }

    out = BIO_new_file(szPath,"w");
    ret = PEM_write_bio_X509_REQ(out, x509_req);

    // 6. free
free_all:
    X509_REQ_free(x509_req);
    BIO_free_all(out);
    EVP_PKEY_free(pKey);

    ASN1_INTEGER_free(asnInt);
    BN_free(bn);

    return (ret == 1); 
}



int main()
{
int rc = gen_X509Req();
    printf("r = %d\n",rc);
    return 0;
}
