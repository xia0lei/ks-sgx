#include <map>
#include <string>
#include <iostream>

#include "ks_enclave_ssl_funcs.h"
#include "ks_enclave_util.h"
#include "Enclave_KS_t.h"
#include "tSgxSSL_api.h"

#include "sgx_trts.h"
#include "sgx_tseal.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#define ADD_ENTROPY_SIZE 32


EVP_PKEY *evp_pkey = NULL;
RSA *keypair = NULL;
std::string base64PublicKey;
int nPublicLength = 0;
std::map<std::string, std::string> recoveryMap;

/*
void printf(const char *fmt, ...)
{
    char buf[BUFSIZ] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    oc_print(buf);
}
*/

void rsa_key_gen()
{
	BIGNUM *bn = BN_new();
	if (bn == NULL) {
		printf("BN_new failure: %ld\n", ERR_get_error());
	    return;
	}
	int ret = BN_set_word(bn, RSA_F4);
    if (!ret) {
       	printf("BN_set_word failure\n");
	    return;
	}

	keypair = RSA_new();
	if (keypair == NULL) {
		printf("RSA_new failure: %ld\n", ERR_get_error());
	    return;
	}
	ret = RSA_generate_key_ex(keypair, 3027, bn, NULL);
	if (!ret) {
        printf("RSA_generate_key_ex failure: %ld\n", ERR_get_error());
	    return;
	}

	evp_pkey = EVP_PKEY_new();
	if (evp_pkey == NULL) {
		printf("EVP_PKEY_new failure: %ld\n", ERR_get_error());
		return;
	}
	EVP_PKEY_assign_RSA(evp_pkey, keypair);

	BN_free(bn);
}

void deliver_public_key()
{
    std::string base64;
    FormatPubToPem(keypair, base64);
    oc_deliver_public_key(base64.c_str());
}

char aad_mac_text[BUFSIZ] = "aad mac text";

sgx_status_t ec_gen_key()
{
    rsa_key_gen();
    return static_cast<sgx_status_t>(0);
}

sgx_status_t ec_deliver_public_key()
{
    deliver_public_key();
    return static_cast<sgx_status_t>(0);
}

sgx_status_t ec_rsa_encrypt(const char* from)
{
    char* out = (char*)encrypt(evp_pkey, from);
    std::string outStr;
    outStr.append(out);
    free(out);
    oc_encrypted_string(outStr.c_str());
    return static_cast<sgx_status_t>(0);
}

sgx_status_t ec_ks_exchange_pair_key(const char* str)
{
    deliver_public_key();
    return static_cast<sgx_status_t>(0);
}

sgx_status_t ec_ks_exchange(char* str)
{
    std::string base64;
    FormatPubToPem(keypair, base64);
    memcpy(str, base64.c_str(), 1024);
    return static_cast<sgx_status_t>(0);
}

sgx_status_t ec_ks_seal(const char *str, char* sealedStr)
{
    int len = strlen(str);
    unsigned char* in = (unsigned char*)malloc(len);
    memcpy(in, str, len);
    unsigned char* decrypt_data = decrypt(evp_pkey, in, len);
    printf("ks_seal | decrypt pice \n", decrypt_data);
    uint32_t sealed_data_size = sgx_calc_sealed_data_size((uint32_t)strlen(aad_mac_text), len);

    char* temp_sealed_buff = (char*)malloc(sealed_data_size);
    if(temp_sealed_buff == NULL)
    {
        free(decrypt_data);
        free(in);
        return SGX_ERROR_OUT_OF_MEMORY;

    }
    sgx_status_t err = sgx_seal_data((uint32_t)strlen(aad_mac_text),
                                    (const uint8_t*)aad_mac_text,
                                    len,
                                    (uint8_t*)decrypt_data,
                                    sealed_data_size,
                                    (sgx_sealed_data_t*)temp_sealed_buff);

    memcpy(sealedStr, temp_sealed_buff, sealed_data_size);
    oc_deliver_sealed_string(temp_sealed_buff);

    free(decrypt_data);
    free(in);
    free(temp_sealed_buff);

    return err;
}

sgx_status_t ec_ks_unseal(char* pkey, const char* str, uint8_t array[6])
{
    uint32_t mac_text_len = sgx_get_add_mac_txt_len((const sgx_sealed_data_t*)str);
    uint32_t decrypt_data_len = sgx_get_encrypt_txt_len((const sgx_sealed_data_t*)str);

    uint8_t* de_mac_text =(uint8_t*)malloc(mac_text_len);
    if(de_mac_text == NULL)
    {
        return SGX_ERROR_OUT_OF_MEMORY;
    }

    uint8_t* decrypt_data = (uint8_t*)malloc(decrypt_data_len);
    if(decrypt_data == NULL)
    {
        free(de_mac_text);
        return SGX_ERROR_OUT_OF_MEMORY;
    }

    sgx_status_t ret = sgx_unseal_data((const sgx_sealed_data_t*)str,
                                        de_mac_text,
                                        &mac_text_len,
                                        decrypt_data,
                                        &decrypt_data_len);
    if(ret != SGX_SUCCESS)
    {
        free(de_mac_text);
        free(decrypt_data);
        return ret;
    }


    unsigned char alpha[6];
    sgx_read_rand(alpha, 6);
    std::string k;
    for(int i = 0;i<6;++i)
    {
        array[i] = (uint8_t)alpha[i];
        k.append(std::to_string(array[i]));
    }


    std::string v;
    v.append((const char*)decrypt_data, decrypt_data_len);
    recoveryMap[k] = v;
    /*
    std::string encryptText = rsa_pub_encrypt(pkey, (const char*)decrypt_data);
    oc_deliver_unseal_string(encryptText.c_str());
    */

    free(de_mac_text);
    free(decrypt_data);
    return ret;
}

sgx_status_t ec_prove_me(uint8_t array[6], char* sealedStr)
{
    std::string k;
    for(int i = 0;i<6;++i)
    {
        k.append(std::to_string(array[i]));
    }
    auto it = recoveryMap.find(k);
    if(it != recoveryMap.end())
    {
        std::string v = it->second;
        memcpy(sealedStr, v.c_str(), 1024);
        oc_deliver_unseal_string(v.c_str());
        recoveryMap.erase(k);
    }
    return static_cast<sgx_status_t>(0);
}


sgx_status_t ec_unseal_data(const uint8_t *sealed_blob, size_t data_size)
{
    uint32_t mac_text_len = sgx_get_add_mac_txt_len((const sgx_sealed_data_t*)sealed_blob);
    uint32_t decrypt_data_len = sgx_get_encrypt_txt_len((const sgx_sealed_data_t*)sealed_blob);

    if(mac_text_len==UINT32_MAX || decrypt_data_len == UINT32_MAX)
        return SGX_ERROR_UNEXPECTED;

    if(mac_text_len > data_size || decrypt_data_len > data_size)
        return SGX_ERROR_INVALID_PARAMETER;

    uint8_t *de_mac_text = (uint8_t*)malloc(mac_text_len);
    if(de_mac_text == NULL)
    {
        return SGX_ERROR_OUT_OF_MEMORY;
    }

    uint8_t *decrypt_data = (uint8_t*)malloc(decrypt_data_len);
    if(decrypt_data == NULL)
    {
        free(de_mac_text);
        return SGX_ERROR_OUT_OF_MEMORY;
    }

    sgx_status_t ret = sgx_unseal_data((const sgx_sealed_data_t*)sealed_blob, de_mac_text, &mac_text_len, decrypt_data, &decrypt_data_len);
    if(ret != SGX_SUCCESS)
    {
        free(de_mac_text);
        free(decrypt_data);
        return ret;
    }

    free(de_mac_text);
    free(decrypt_data);
    return ret;
}

sgx_status_t ec_rsa_decrypt(const char *str)
{
    std::string source(str);
    size_t outlen = source.length() + 1;
    decrypt(evp_pkey, (unsigned char*)source.c_str(), outlen);

    return static_cast<sgx_status_t>(0);
}

