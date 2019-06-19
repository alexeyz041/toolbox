// add CRL to crls folder like this
// ln -s crl.pem $(openssl crl -hash -noout -in crl.pem).r0

#include "crlcertverify.h"


#define LOG_SSL_WARNING(custommsg) \
     ERR_load_crypto_strings();  \
     unsigned long errQueue = 0; \
     stringstream error; \
     error << custommsg; \
     while ((errQueue = ERR_get_error())) \
         error << " - " << ERR_reason_error_string(errQueue)<<"["<<errQueue<<"]"; \
     cout << error.str();

CertificateStore::CertificateStore()
{
    //create store and store ctx
    _store    = X509_STORE_new();
    _storeCtx = X509_STORE_CTX_new();
}

CertificateStore::~CertificateStore()
{
    X509_STORE_free(_store);
    X509_STORE_CTX_free(_storeCtx);
}

void CertificateStore::setCRLVerificationFlag(bool crlCheckAll)
{
    //verify complete chain or only one certificate
    X509_STORE_set_flags(_store, crlCheckAll ? (X509_V_FLAG_CRL_CHECK_ALL | X509_V_FLAG_CRL_CHECK) : X509_V_FLAG_CRL_CHECK);
}

void CertificateStore::addLookupHashDir(const char* path)
{
    /*Adding CRL directory for lookup. During CRL verify, openssl will check for CRLs from this path.
    CRL files should be present in this dir as <issuer_hash>.rN 
    More details : https://www.openssl.org/docs/man1.1.0/crypto/X509_LOOKUP_file.html 
    
    If you have specic CRL files to add you can loaded them using "X509_load_crl_file" API as well*/
        
    X509_LOOKUP* lookup = X509_STORE_add_lookup(_store, X509_LOOKUP_hash_dir());
    if (lookup == NULL)
    {
        cout << "CRL path initialization error: X509 lookup initialization failed.";
//        throw CRL_LOOKUP_FAILURE;
    }

    if(not X509_LOOKUP_add_dir(lookup, path, X509_FILETYPE_PEM))
    {
        cout << "CRL path initialization error: path addition failed.";
//        throw CRL_LOOKUP_FAILURE;
    }
}

void CertificateStore::addCertToStore(X509* cert)
{
    /*Add certificate to the store*/
    if (not X509_STORE_add_cert(_store, cert))
    {
        LOG_SSL_WARNING("Failed to add certificate to store.");
//        throw OPERATION_FAILED;
    }
}

int CertificateStore::verifyCert(X509* cert, string& msg)
{
    /*Initialize store_ctx with global store, add the last EE certificate to the store ctx.
    NOTE: store ctx cab be used only once to verify chain. */

    X509_STORE_CTX_init(_storeCtx, _store, NULL, NULL);
    X509_STORE_CTX_set_cert(_storeCtx, cert);
    X509_verify_cert(_storeCtx);

    int sslRet = X509_STORE_CTX_get_error(_storeCtx);
    if (sslRet != X509_V_OK)
    {
        const char* err = X509_verify_cert_error_string(sslRet);
        if (err)
            msg = string(err);
    }
    return sslRet;
}

X509* CertificateStore::getCurrentCertDetails(long& serialNum, string& issuer)
{
    /*get the certificate details for which verify failed*/
    X509* cert = X509_STORE_CTX_get_current_cert(_storeCtx);
    serialNum = ASN1_INTEGER_get(X509_get_serialNumber(cert));
    char* issuerId = X509_NAME_oneline(X509_get_issuer_name(cert), NULL, 0);
    if (issuerId)
        issuer = string(issuerId);
    return cert;
}

X509 *load_cert(const char *file)
{
X509 *cert=NULL;
FILE *fp;
    if (! (fp=fopen(file, "r"))) {
	printf("Error reading %s file\n",file);
        exit(-1);
    }

    if(! (cert = PEM_read_X509(fp,NULL,NULL,NULL))) {
	printf("Error loading %s into memory\n",file);
        exit(-1);
    }
    fclose(fp);
    return cert;
}

X509 *load_cert_tr(const char *file)
{
X509 *cert=NULL;
FILE *fp;
    if (! (fp=fopen(file, "r"))) {
	printf("Error reading %s file\n",file);
        exit(-1);
    }

    if(! (cert = PEM_read_X509_AUX(fp,NULL,NULL,NULL))) {
	printf("Error loading %s into memory\n",file);
        exit(-1);
    }
    fclose(fp);
    return cert;
}


int main()
{
     string msg;
     X509* rca_cert = load_cert_tr("dmca.pem");
     X509* ica_cert = load_cert("clca.pem");
     X509* ee = load_cert("user.pem");
     X509* ee2 = load_cert("user2.pem");
     
     CertificateStore store;
     store.setCRLVerificationFlag(true);
     store.addLookupHashDir("crls");
     //get certificates in X509* format and add them to trusted store.
     store.addCertToStore(rca_cert);
     store.addCertToStore(ica_cert);
     //verify the required certificate. if required ica_cert can also be verified using below.
     //But it may not be required indeed, as we verify whole chain.

     int ret = store.verifyCert(ee, msg);
     if (ret == X509_V_ERR_CERT_REVOKED)
     {
          long sr;
          string issuer;
          store.getCurrentCertDetails(sr, issuer);
          cout << "Certificate " << std::hex << sr << " issued by "<<issuer<<" is revoked " << msg <<  "\n";
     } else {
          long sr;
          string issuer;
          store.getCurrentCertDetails(sr, issuer);
          cout << "Certificate " << std::hex << sr << " issued by "<<issuer<<" ok " << msg <<  "\n";
     }

     ret = store.verifyCert(ee2, msg);
     if (ret == X509_V_ERR_CERT_REVOKED)
     {
          long sr;
          string issuer;
          store.getCurrentCertDetails(sr, issuer);
          cout << "Certificate " << std::hex << sr << " issued by "<<issuer<<" is revoked \n";
     } else {
          long sr;
          string issuer;
          store.getCurrentCertDetails(sr, issuer);
          cout << "Certificate " << std::hex << sr << " issued by "<<issuer<<" ok " << msg << ret << "\n";
     }
     return 0;
}
