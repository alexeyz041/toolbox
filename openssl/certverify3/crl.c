
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/asn1.h>
#include <openssl/err.h>

#include <stdio.h>

// add CRL to crls folder like this
// ln -s crl.pem $(openssl crl -hash -noout -in crl.pem).r0


void print_error(char *custommsg)
{
    ERR_load_crypto_strings();
    unsigned long errQueue = 0;
     
    printf("%s\n", custommsg);
    while ((errQueue = ERR_get_error())) {
	printf("%s - [%ld]\n", ERR_reason_error_string(errQueue), errQueue);
    }
}


int crl_check(char *crls_path, X509 *ca_cert[], int n_ca, X509 *cert)
{
    X509_STORE* _store    = X509_STORE_new();
    X509_STORE_CTX *_storeCtx = X509_STORE_CTX_new();

    X509_STORE_set_flags(_store, /*X509_V_FLAG_CRL_CHECK_ALL | */ X509_V_FLAG_CRL_CHECK);

    // Adding CRL directory for lookup. During CRL verify, openssl will check for CRLs from this path.
    // CRL files should be present in this dir as <issuer_hash>.rN 
    // More details : https://www.openssl.org/docs/man1.1.0/crypto/X509_LOOKUP_file.html 
        
    X509_LOOKUP* lookup = X509_STORE_add_lookup(_store, X509_LOOKUP_hash_dir());
    if(!lookup) {
        print_error("CRL path initialization error: X509 lookup initialization failed.\n");
        return -1;
    }
    if(!X509_LOOKUP_add_dir(lookup, crls_path, X509_FILETYPE_PEM)) {
        print_error("CRL path initialization error: path addition failed.\n");
        return -1;
    }

// Add CA certificates to the store
//for(int i=0; i < n_ca; i++) {
//    if(!X509_STORE_add_cert(_store, ca_cert[i])) {
//	    print_error("Failed to add certificate to store.\n");
//    	    return -1;
//	}
//    }

    int ret = X509_STORE_load_locations(_store, "./ca.pem", NULL);
    if (ret != 1) {
	printf("Error loading CA cert or chain file\n");
    }

    X509_STORE_CTX_init(_storeCtx, _store, NULL, NULL);
    X509_STORE_CTX_set_cert(_storeCtx, cert);
    ret = X509_verify_cert(_storeCtx);
    if(ret != 1) {
        int sslRet = X509_STORE_CTX_get_error(_storeCtx);
	if (sslRet != X509_V_OK) {
    	    const char *err = X509_verify_cert_error_string(sslRet);
            if (err) {
    		X509* cert = X509_STORE_CTX_get_current_cert(_storeCtx);
		long serialNum = ASN1_INTEGER_get(X509_get_serialNumber(cert));
		char* issuerId = X509_NAME_oneline(X509_get_issuer_name(cert), NULL, 0);
        	printf("Certificate %ld issued by %s is revoked %s\n", serialNum, issuerId, err);
	    }
	}
    }
    X509_STORE_free(_store);
    X509_STORE_CTX_free(_storeCtx);

    return ret;
}


X509 *load_cert(const char *file, int trusted)
{
X509 *cert=NULL;
FILE *fp;
    if (! (fp=fopen(file, "r"))) {
	printf("Error reading %s file\n",file);
        exit(-1);
    }

    if(trusted) {
	if(! (cert = PEM_read_X509_AUX(fp,NULL,NULL,NULL))) {
	    printf("Error loading %s into memory\n",file);
    	    exit(-1);
	}
    } else {
        if(! (cert = PEM_read_X509(fp,NULL,NULL,NULL))) {
    	    printf("Error loading %s into memory\n",file);
            exit(-1);
	}
    }

    fclose(fp);
    return cert;
}



#define NELEM(X)	sizeof(X)/sizeof(X[0])

int main()
{
    X509 *rca_cert = load_cert("dmca.pem",1);
    X509 *ica_cert = load_cert("clca.pem",0);
    X509 *ca[] = { rca_cert, ica_cert };

     X509* ee = load_cert("user.pem",0);
     X509* ee2 = load_cert("user2.pem",0);
     
     int ret = crl_check("crls", ca, NELEM(ca), ee);
     if(ret) {
	printf("Certificate verified Ok\n");
     }

     ret = crl_check("crls", ca, NELEM(ca), ee2);
     if(ret) {
	printf("Certificate verified Ok\n");
     }

    return 0;
}
