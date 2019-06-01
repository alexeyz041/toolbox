#include <openssl/evp.h>
#include <openssl/objects.h>
#include <stdio.h>

void my_callback(const OBJ_NAME *obj, void *arg)
{
  printf("Digest: %s\n", obj->name);
}

int main(int argc, char *argv)
{
  void *my_arg;
  OpenSSL_add_all_digests(); //make sure they're loaded

  my_arg = NULL;
  OBJ_NAME_do_all(OBJ_NAME_TYPE_MD_METH, my_callback, my_arg);
}
