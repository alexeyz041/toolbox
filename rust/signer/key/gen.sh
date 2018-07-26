openssl genrsa -out private.pem 2048
openssl rsa -in private.pem -outform PEM -pubout -out public.pem
openssl rsa -pubin -in public.pem -inform PEM -RSAPublicKey_out -outform DER -out pubk.der
openssl rsa -in private.pem -out pk.der -outform DER
