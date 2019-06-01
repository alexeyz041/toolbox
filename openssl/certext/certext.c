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

	if(X509V3_EXT_print(outbio, ext, 0, 0)) {
    	    BIO_printf(outbio, "\n");
	} else {
	    ASN1_OCTET_STRING* str = X509_EXTENSION_get_data(ext);
	    const unsigned char *out = ASN1_STRING_get0_data(str);
	    BIO_printf(outbio, "%s\n",out);
	}
    }

  X509_free(cert);
  BIO_free_all(certbio);
  BIO_free_all(outbio);
  exit(0);
}
