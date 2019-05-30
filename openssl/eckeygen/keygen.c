// extract public key
// openssl ec -in key.pem -pubout -out pubk.pem

#include <openssl/obj_mac.h>
#include <openssl/ec.h>
#include <openssl/pem.h>

int main()
{
    EC_KEY *eckey = EC_KEY_new();
    EC_GROUP *ecgroup = EC_GROUP_new_by_curve_name(NID_secp256k1);
    EC_KEY_set_group(eckey, ecgroup);
    EC_KEY_generate_key(eckey);

    FILE *out = fopen("key.pem", "w");
    int i = PEM_write_ECPrivateKey(out, eckey, NULL, NULL, 0, NULL, NULL);
    fclose(out);
    EC_KEY_free(eckey);
    
    printf("i = %d",i);

    return 0;
}
