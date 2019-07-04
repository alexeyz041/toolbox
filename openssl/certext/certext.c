/* ------------------------------------------------------------ *
 * file:        certextensions.c                                *
 * purpose:     Example code for OpenSSL certificate extensions *
 * author:      09/12/2012 Frank4DD                             *
 * based on:    openssl-[x.x.x]/crypto/asn1/t_x509.c            *
 *                                                              *
 * gcc -o certextensions certextensions.c -lssl -lcrypto        *
 * ------------------------------------------------------------ */

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/asn1t.h>


struct x_pubkey_st {
    ASN1_INTEGER *num_keys;
    ASN1_BIT_STRING *public_key;
};

typedef struct x_pubkey_st X_PUBKEY;

ASN1_SEQUENCE(X_PUBKEY) = {
        ASN1_SIMPLE(X_PUBKEY, num_keys, ASN1_INTEGER),
        ASN1_SIMPLE(X_PUBKEY, public_key, ASN1_BIT_STRING)
} ASN1_SEQUENCE_END(X_PUBKEY)

IMPLEMENT_ASN1_FUNCTIONS(X_PUBKEY)


int i2d_xpubkey(unsigned char **pp)
{
int ret;
X_PUBKEY *xpk;
    
    if ((xpk = X_PUBKEY_new()) == NULL)
        return -1;

    ASN1_INTEGER_set(xpk->num_keys, 2);
    unsigned char bits[] = { 22, 33 };
    ASN1_BIT_STRING_set(xpk->public_key, bits, 13);

    ret = i2d_X_PUBKEY(xpk, pp);
        
    return ret;
}

X_PUBKEY *d2i_xpubkey(const unsigned char **pp, long length)
{
    const unsigned char *q;
    q = *pp;
    X_PUBKEY *xpk = d2i_X_PUBKEY(NULL, &q, length);
    if (!xpk)
        return NULL;

    return xpk;
}



int main()
{
  const char cert_filestr[] = "./cert.pem";
  BIO              *certbio = NULL;
  BIO               *outbio = NULL;
  X509                *cert = NULL;
  X509_CINF       *cert_inf = NULL;
//  STACK_OF(X509_EXTENSION) *ext_list;
  int ret, i;

  /* ---------------------------------------------------------- *
   * These function calls initialize openssl for correct work.  *
   * ---------------------------------------------------------- */
  OpenSSL_add_all_algorithms();
  ERR_load_BIO_strings();
  ERR_load_crypto_strings();

  /* ---------------------------------------------------------- *
   * Create the Input/Output BIO's.                             *
   * ---------------------------------------------------------- */
  certbio = BIO_new(BIO_s_file());
  outbio  = BIO_new_fp(stdout, BIO_NOCLOSE);

  /* ---------------------------------------------------------- *
   * Load the certificate from file (PEM).                      *
   * ---------------------------------------------------------- */
  ret = BIO_read_filename(certbio, cert_filestr);
  if (! (cert = PEM_read_bio_X509(certbio, NULL, 0, NULL))) {
    BIO_printf(outbio, "Error loading cert into memory\n");
    exit(-1);
  }

  printf("---\n");

  /* ---------------------------------------------------------- *
   * Extract the certificate's extensions                       *
   * ---------------------------------------------------------- */

    int n_ext = X509_get_ext_count(cert);
    for(int i=0; i < n_ext; i++) {
        X509_EXTENSION *ext = X509_get_ext(cert, i);

        ASN1_OBJECT *obj = X509_EXTENSION_get_object(ext);
	BIO_printf(outbio, "\n");
	BIO_printf(outbio, "Object %.2d: ", i);
	i2a_ASN1_OBJECT(outbio, obj);
	BIO_printf(outbio, "\n");

	if(i == 3) {
	    ASN1_STRING* d = X509_EXTENSION_get_data(ext);
	    X_PUBKEY *xpk = d2i_xpubkey(&d->data,d->length);
	    unsigned char *bits = xpk->public_key->data;
	    printf("n = %ld %d %d %d\n", ASN1_INTEGER_get(xpk->num_keys), bits[0], bits[1], xpk->public_key->length);
	    continue;
	}

	if(X509V3_EXT_print(outbio, ext, 0, 0)) {
    	    BIO_printf(outbio, "\n");
	} else {
	    ASN1_STRING* str = X509_EXTENSION_get_data(ext);
	    char *p = (char*)ASN1_STRING_get0_data(str);
	    int len = ASN1_STRING_length(str);
	    printf("len1 = %d type %d",len,ASN1_STRING_type(str));
	    for(int i=0; i < len; i++) {
	       printf("%c %02x ",p[i]&0xff,p[i]&0xff);
	    }
	    printf("\n");
	}
    }

  X509_free(cert);
  BIO_free_all(certbio);
  BIO_free_all(outbio);
  exit(0);
}
