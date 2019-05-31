// https://security.stackexchange.com/questions/184845/how-to-generate-csrcertificate-signing-request-using-c-and-openssl

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>

EC_KEY *genkey()
{
    EC_KEY *eckey = EC_KEY_new();
    EC_GROUP *ecgroup = EC_GROUP_new_by_curve_name(NID_secp256k1);
    EC_KEY_set_group(eckey, ecgroup);
    EC_KEY_generate_key(eckey);

    FILE *out = fopen("key.pem", "w");
    int i = PEM_write_ECPrivateKey(out, eckey, NULL, NULL, 0, NULL, NULL);
    fclose(out);

    return eckey;
}


int gen_X509Req()
{
int ret = 0;
//RSA *r = NULL;
//BIGNUM *bne = NULL;

int             nVersion = 0;
//int             bits = 2048;
//unsigned long   e = RSA_F4;

X509_REQ        *x509_req = NULL;
X509_NAME       *x509_name = NULL;
EVP_PKEY        *pKey = NULL;
RSA             *tem = NULL;
BIO             *out = NULL, *bio_err = NULL;

const char      *szCountry = "CA";
const char      *szProvince = "BC";
const char      *szCity = "Vancouver";
const char      *szOrganization = "SlowHacking Co Ltd";
const char      *szCommon = "localhost";

const char      *szPath = "x509Req.pem";

/*
    // 1. generate rsa key
    bne = BN_new();
    ret = BN_set_word(bne,e);
    if(ret != 1){
	goto free_all;
    }

    r = RSA_new();
    ret = RSA_generate_key_ex(r, bits, bne, NULL);
    if(ret != 1){
	goto free_all;
    }
*/

    EC_KEY *r = genkey();

    // 2. set version of x509 req
    x509_req = X509_REQ_new();
    ret = X509_REQ_set_version(x509_req, nVersion);
    if (ret != 1){
	goto free_all;
    }

    // 3. set subject of x509 req
    x509_name = X509_REQ_get_subject_name(x509_req);

    ret = X509_NAME_add_entry_by_txt(x509_name,"C", MBSTRING_ASC, (const unsigned char*)szCountry, -1, -1, 0);
    if (ret != 1){
	goto free_all;
    }

    ret = X509_NAME_add_entry_by_txt(x509_name,"ST", MBSTRING_ASC, (const unsigned char*)szProvince, -1, -1, 0);
    if (ret != 1){
	goto free_all;
    }

    ret = X509_NAME_add_entry_by_txt(x509_name,"L", MBSTRING_ASC, (const unsigned char*)szCity, -1, -1, 0);
    if (ret != 1){
	goto free_all;
    }   

    ret = X509_NAME_add_entry_by_txt(x509_name,"O", MBSTRING_ASC, (const unsigned char*)szOrganization, -1, -1, 0);
    if (ret != 1){
	goto free_all;
    }

    ret = X509_NAME_add_entry_by_txt(x509_name,"CN", MBSTRING_ASC, (const unsigned char*)szCommon, -1, -1, 0);
    if (ret != 1){
	goto free_all;
    }

//----------
    struct stack_st_X509_EXTENSION *exts = NULL;
    X509_EXTENSION *ex;
    
    exts = sk_X509_EXTENSION_new_null();
    ASN1_OCTET_STRING *os = ASN1_OCTET_STRING_new();
    int nid = OBJ_create("2.5.29.41", "CompanyName", "Company Name");
    ASN1_OCTET_STRING_set(os,"ABC Corp",8);   
    ex = X509_EXTENSION_create_by_NID( NULL, nid, 0, os );
    sk_X509_EXTENSION_push(exts, ex);

    unsigned char data[20] = {1,2,3,4,5,6,7,8,9};
    //RAND_bytes(data, sizeof(data));
    //data[0] &= 0x7F;

    // build big number from our bytes
    BIGNUM* bn = BN_new();
    BN_bin2bn(data, sizeof(data), bn);

    // build the ASN1_INTEGER from our BIGNUM
    ASN1_INTEGER* asnInt = ASN1_INTEGER_new();
    BN_to_ASN1_INTEGER(bn, asnInt);

    // cleanup
    //ASN1_INTEGER_free(asnInt);
    //BN_free(bn);

    nid = OBJ_create("2.5.29.42", "Bin data", "Bin data");
    ex = X509_EXTENSION_create_by_NID( NULL, nid, 0, asnInt );
    sk_X509_EXTENSION_push(exts, ex);

    X509_REQ_add_extensions(x509_req, exts);
//----

    // 4. set public key of x509 req
    pKey = EVP_PKEY_new();
    EVP_PKEY_assign_EC_KEY(pKey, r);
    r = NULL;   // will be free rsa when EVP_PKEY_free(pKey)

    ret = X509_REQ_set_pubkey(x509_req, pKey);
    if (ret != 1){
	goto free_all;
    }

    // 5. set sign key of x509 req
    ret = X509_REQ_sign(x509_req, pKey, EVP_sha256());    // return x509_req->signature->length
    if (ret <= 0){
	goto free_all;
    }

    out = BIO_new_file(szPath,"w");
    ret = PEM_write_bio_X509_REQ(out, x509_req);

    // 6. free
free_all:
    X509_REQ_free(x509_req);
    BIO_free_all(out);
    EVP_PKEY_free(pKey);
//    BN_free(bne);
    return (ret == 1); 
}



int main()
{
int r = gen_X509Req();
    printf("r = %d\n",r);

    return 0;
}
