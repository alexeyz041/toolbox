//
// original version taken from http://fm4dd.com/openssl/certfprint.htm
//

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>



int main(int argc,char *argv[])
{
  BIO              *certbio = NULL;
  BIO               *outbio = NULL;
  X509                *cert = NULL;
  const EVP_MD *fprint_type = NULL;
  int ret, j, fprint_size;
  unsigned char fprint[EVP_MAX_MD_SIZE];

    if(argc < 2) {
	printf("Usage:\ncertfprint cert.pem\n");
	return -1;
    }

  OpenSSL_add_all_algorithms();
  ERR_load_BIO_strings();
  ERR_load_crypto_strings();

  certbio = BIO_new(BIO_s_file());
  outbio  = BIO_new_fp(stdout, BIO_NOCLOSE);

  ret = BIO_read_filename(certbio, argv[1]);
  if (!ret ) {
    BIO_printf(outbio, "Error loading cert into memory\n");
    exit(-1);
  }

  cert = PEM_read_bio_X509(certbio, NULL, 0, NULL);
  if (!cert ) {
    BIO_printf(outbio, "Error loading cert into memory 2\n");
    if (ERR_peek_last_error()) {
	fprintf(stderr, "generated errors:\n");
	ERR_print_errors_fp(stderr);
    }
    exit(-1);
  }

  fprint_type = EVP_sha1();

  if (!X509_digest(cert, fprint_type, fprint, &fprint_size))
    BIO_printf(outbio,"Error creating the certificate fingerprint.\n");

  BIO_printf(outbio,"Fingerprint Method: %s\n",
    OBJ_nid2sn(EVP_MD_type(fprint_type)));

  BIO_printf(outbio,"Fingerprint Length: %d\n", fprint_size);

  /* Microsoft Thumbprint-style: lowercase hex bytes with space */
  BIO_printf(outbio,"Fingerprint String: ");
  for (j=0; j<fprint_size; ++j) BIO_printf(outbio, "%02x ", fprint[j]);
  BIO_printf(outbio,"\n");

  /* OpenSSL fingerprint-style: uppercase hex bytes with colon */
  for (j=0; j<fprint_size; j++) {
    BIO_printf(outbio,"%02X%c", fprint[j], (j+1 == fprint_size) ?'\n':':');
  }

  X509_free(cert);
  BIO_free_all(certbio);
  BIO_free_all(outbio);
  return 0;
}
