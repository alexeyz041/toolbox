
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/asn1.h>
#include <openssl/err.h>

#include <string>
#include <iostream>
#include <sstream>

using namespace std;

class CertificateStore
{
        public:
            CertificateStore();
            ~CertificateStore();

            void setCRLVerificationFlag(bool crlCheckAll);
            void addLookupHashDir(const char* path);
            void addCertToStore(X509* cert);
            int  verifyCert(X509* cert, string& msg);
            X509* getCurrentCertDetails(long& serialNum, string& issuer);

        private:
            X509_STORE* _store;
            X509_STORE_CTX *_storeCtx;
};
