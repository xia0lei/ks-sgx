#include "ks_enclave_ssl_funcs.h"
#include "ks_enclave_util.h"

char * Base64Encode(const char * input, int length, bool with_new_line)
{
    BIO * bmem = NULL;
    BIO * b64 = NULL;
    BUF_MEM * bptr = NULL;

    b64 = BIO_new(BIO_f_base64());
    if(!with_new_line) {
            BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
        }
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, input, length);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);

    char * buff = (char *)malloc(bptr->length + 1);
    memcpy(buff, bptr->data, bptr->length);
    buff[bptr->length] = 0;

    BIO_free_all(b64);

    return buff;
}

char * Base64Decode(char * input, int length, bool with_new_line)
{
    BIO * b64 = NULL;
    BIO * bmem = NULL;
    char * buffer = (char *)malloc(length);
    memset(buffer, 0, length);

    b64 = BIO_new(BIO_f_base64());
    if(!with_new_line) {
            BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
        }
    bmem = BIO_new_mem_buf(input, length);
    bmem = BIO_push(b64, bmem);
    BIO_read(bmem, buffer, length);

    BIO_free_all(bmem);

    return buffer;
}

unsigned char* decrypt(EVP_PKEY* evp_pkey, unsigned char* in, size_t inlen)
{
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(evp_pkey, evp_pkey->engine);
    EVP_PKEY_decrypt_init(ctx);
    EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING);
    size_t outlen;
    EVP_PKEY_decrypt(ctx, NULL, &outlen, in, inlen);
    unsigned char* out = (unsigned char*)OPENSSL_malloc(outlen);
    EVP_PKEY_decrypt(ctx, out, &outlen, in, inlen);
    printf("decrypt success : %s\n", out);
    return out;
}






