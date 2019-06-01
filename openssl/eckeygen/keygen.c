// view key
// openssl ec -in key.pem -noout -text
// extract public key
// openssl ec -in key.pem -pubout -out pubk.pem
// view public key
// openssl ec -in key-pub2.pem -pubin -noout -text

#include <openssl/obj_mac.h>
#include <openssl/ec.h>
#include <openssl/pem.h>

int newkey()
{
    EC_KEY *eckey = EC_KEY_new();
    EC_GROUP *ecgroup = EC_GROUP_new_by_curve_name(NID_secp256k1);
    EC_KEY_set_group(eckey, ecgroup);
    EC_KEY_generate_key(eckey);

    FILE *out = fopen("key.pem", "w");
    int i = PEM_write_ECPrivateKey(out, eckey, NULL, NULL, 0, NULL, NULL);
    fclose(out);
    EC_KEY_free(eckey);
    printf("i = %d\n",i);
    return 0;
}


int newkey2()
{
    FILE *f = fopen("curve.pem","r");
    EC_GROUP *ecgroup = NULL;
    PEM_read_ECPKParameters(f,&ecgroup,NULL,NULL);
    fclose(f);

    EC_KEY *eckey = EC_KEY_new();
    EC_KEY_set_group(eckey, ecgroup);
    EC_KEY_generate_key(eckey);

    FILE *out = fopen("key2.pem", "w");
    int i = PEM_write_ECPrivateKey(out, eckey, NULL, NULL, 0, NULL, NULL);
    fclose(out);

    FILE *out1 = fopen("key-pub2.pem", "w");
    int i1 = PEM_write_EC_PUBKEY(out1, eckey);
    fclose(out1);

    EC_KEY_free(eckey);
    printf("i = %d i1 = %d\n",i,i1);
}


int main()
{
    newkey();
    newkey2();
    return 0;
}
