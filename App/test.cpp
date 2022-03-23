#include <string>
#include <vector>

#include "test.h"
#include "global.h"
#include "Enclave_KS_u.h"
#include "ErrorSupport.h"


char* test_out_public_key(sgx_enclave_id_t eid_t, char* userpkHex)
{
    sgx_status_t ret, ret_val;
    char* str = (char*)malloc(256);
    char* sharedStr = (char*)malloc(256);
    ret = ec_ks_exchange(eid_t,&ret_val, userpkHex, str, sharedStr);
    printf("%s %d %d\n", str, ret, ret_val);
    free(sharedStr);
    //ret = ec_rand_num(eid_t, &ret_val);
    if(ret != SGX_SUCCESS)
    {
        ret_error_support(ret);
        return NULL;
    }
    else if(ret_val != SGX_SUCCESS)
    {
        ret_error_support(ret_val);
        return NULL;
    }

    return str;

}


void test_gen_rand_num(sgx_enclave_id_t eid_t)
{
    sgx_status_t ret, ret_val;
    //ret = ec_rand_num(eid_t, &ret_val);
    if(ret != SGX_SUCCESS)
    {
        ret_error_support(ret);
        return;
    }
    else if(ret_val != SGX_SUCCESS)
    {
        ret_error_support(ret_val);
        return;
    }
}

void test_gen_key(sgx_enclave_id_t eid_t)
{
    sgx_status_t ret, ret_val;
    ret = ec_gen_key(eid_t, &ret_val);
    if(ret != SGX_SUCCESS)
    {
        ret_error_support(ret);
        return;
    }
    else if(ret_val != SGX_SUCCESS)
    {
        ret_error_support(ret_val);
        return;
    }
}


void test_encrypt(sgx_enclave_id_t eid_t, const char* str)
{
    sgx_status_t ret, ret_val;
    ret = ec_rsa_encrypt(eid_t, &ret_val, str);
    if(ret != SGX_SUCCESS)
    {
        ret_error_support(ret);
        return;
    }
    else if(ret_val != SGX_SUCCESS)
    {
        ret_error_support(ret_val);
        return;
    }
}

void test_rsa_decrypt(sgx_enclave_id_t eid_t)
{
    std::string strSource = "source text,hello world";
    sgx_status_t ret, ret_val;
    ret = ec_rsa_decrypt(eid_t, &ret_val, (const char*)strSource.c_str());
    if(ret != SGX_SUCCESS)
    {
        ret_error_support(ret);
        return;
    }
    else if(ret_val != SGX_SUCCESS)
    {
        ret_error_support(ret_val);
        return;
    }
}

void test_aes_decrypt(sgx_enclave_id_t eid_t, char* str)
{
    sgx_status_t ret, ret_val;
    printf("To decrypt str\n", str);
    ret = ec_aes_decrypt(eid_t, &ret_val, str);
    if(ret != SGX_SUCCESS)
    {
        ret_error_support(ret);
        return;
    }
    else if(ret_val != SGX_SUCCESS)
    {
        ret_error_support(ret_val);
        return;
    }
}

uint8_t* test_seal_and_save_data(sgx_enclave_id_t eid_t, uint32_t* sz)
{
    uint8_t array[] ={
        119, 216, 200, 17, 26,
        14, 17, 56, 239, 214,
        181, 208, 142, 87, 232, 28, 91, 225, 236, 138, 0, 15, 245, 171, 108, 100, 37, 96, 157, 155, 18, 182, 158, 37, 224, 67, 32, 43, 103, 100, 136, 133, 38, 30, 121, 232, 137, 34, 68, 50, 117, 217, 63, 227, 223, 157, 14, 194, 11, 201, 201, 146, 60, 39, 171, 206, 199, 140, 239, 103, 4, 105, 241, 94, 41, 45, 182, 126, 160, 207, 255, 223, 180, 124, 89, 96, 27, 196, 184, 191, 53, 27, 105, 109, 193, 220, 100, 214, 113, 8, 254, 203, 49, 203, 214, 13, 82, 180, 30, 80, 244, 56, 183, 57, 127, 208, 107, 18, 64, 200, 46, 201, 185, 31, 240, 44, 248, 23, 215, 24, 34, 99, 233, 162, 27, 192, 131, 67, 198, 36, 23, 83, 241, 36, 98, 113, 41, 184, 176, 159, 139, 104, 212, 147, 147, 74, 106, 242, 66, 177, 140, 39, 95, 10, 51, 216, 126, 243, 201, 110, 27, 252, 126, 138, 195, 17, 59, 163, 185, 187, 225, 37, 0, 200, 77, 69, 47, 220, 248, 163, 91, 53, 166, 209, 252, 40, 84, 8, 41, 94, 152, 47, 23, 79, 115, 162, 186, 38, 22, 118, 207, 11, 190, 47, 140, 219, 106, 7, 149, 134, 78, 49, 170, 5, 189, 196, 55, 223, 105, 120, 207, 121, 192, 17, 119, 140, 245, 58, 200, 89, 81, 178, 117, 245, 63, 12, 146, 205, 145, 146, 4, 144, 244, 76, 5, 12, 234, 194, 57, 224, 3, 240, 219, 130, 80, 198, 25, 211, 47, 109, 69, 59, 98, 21, 243, 23, 125, 205, 10, 240, 233, 32, 12, 93, 30, 11, 195, 192, 37, 138, 36, 46, 62, 39, 217, 60, 179, 39, 6, 219, 23, 3, 106, 145, 129, 37, 154, 163, 235, 137, 109, 253, 88, 133, 212, 233, 188, 65, 208, 71, 149, 217, 178, 145, 51, 190, 254, 87, 95, 101, 140, 111, 2, 197, 221, 130, 78, 225, 13, 107, 128, 219, 12, 37, 199, 218, 31, 9, 183, 153, 230, 70, 240, 150, 176, 140, 247, 0, 183, 130, 189, 90, 246, 127, 160, 66, 197, 253, 156, 227, 22, 165, 77, 84, 17, 171, 105, 208, 43, 67, 86, 92, 249, 78, 240, 173, 37, 255, 151, 123, 164, 125, 201, 11, 125, 111, 127, 28, 8, 183, 234, 133, 4, 248, 198, 56, 167, 12, 103, 238, 158, 102, 144, 29, 229, 190, 245, 232, 229, 56, 227, 197, 90, 45, 143, 196, 42, 120, 103, 204, 59, 197, 210, 180, 183, 174, 237, 20, 197, 173, 102, 9, 58, 134, 229, 248, 91, 232, 25, 67, 66, 59, 2, 8, 206, 143, 62, 174, 249, 135, 147, 105, 172, 6, 245, 47, 234, 112, 39, 112, 171, 35, 28, 155, 103, 156, 27, 42, 139, 223, 113, 251, 31, 103, 163, 5, 94, 39, 165, 140, 172, 223, 249, 212, 2, 253, 88, 178, 138, 32, 41, 242, 88, 167, 189, 156, 69, 166, 12, 49, 15, 103, 86, 149, 226, 126, 62, 112, 174, 59, 50, 143, 161, 96, 158, 36, 30, 34, 72, 48, 56, 131, 10, 69, 34, 1, 97, 144, 211, 74, 59, 97, 89, 112, 222, 133, 1, 173, 129, 106, 194, 21, 187, 41, 234, 190, 172, 29, 45, 82, 177, 9, 236, 212, 31, 50, 123, 70, 123, 170, 52, 122, 99, 200, 187, 234, 2, 224, 166, 126, 141, 95, 236, 55, 240, 38, 24, 157, 91, 141, 135, 62, 68, 52, 142, 76, 255, 63, 24, 158, 31, 159, 28, 143, 164, 162, 212, 1, 26, 113, 231, 142, 78, 67, 21, 146, 151, 125, 120, 142, 51, 206, 247, 119, 170, 140, 94, 55, 245, 86, 221, 29, 41, 218, 179, 49, 177, 72, 137, 136, 221, 247, 197, 128, 255, 16, 115, 19, 210, 185, 110, 8, 74, 239, 9, 11, 85, 202, 76, 56, 103, 197, 220, 233, 114, 156, 108, 198, 110, 34, 54, 139, 131, 20, 136, 47, 246, 218, 81, 46, 234, 186, 202, 102, 7, 210, 141, 111, 107, 132, 22, 14, 62, 138, 223, 83, 50, 134, 245, 39, 15, 91, 76, 144, 151, 104, 240, 241, 165, 24, 220, 132, 198, 47, 109, 32, 172, 236, 108, 120, 12, 98, 133, 36, 212, 145, 1, 17, 148, 157, 4, 124, 203, 74, 19, 183, 30, 200, 145, 130, 198, 74, 228, 58, 198, 249, 103, 68, 228, 38, 219, 7, 54, 63, 32, 130, 144, 210, 5, 129, 70, 115, 254, 175, 114, 31, 55, 149, 86, 139, 93, 177, 111, 13, 188, 239, 182, 143, 90, 89, 4, 50, 2, 178, 137, 120, 199, 254, 236, 200, 173, 121, 56, 155, 0, 127, 94, 240, 31, 1, 154, 175, 57, 181, 30, 68, 90, 142, 91, 96, 133, 230, 143, 225, 219, 139, 130, 194, 70, 100, 252, 67, 83, 51, 214, 168, 143, 171, 71, 183, 236, 170, 236, 190, 188, 112, 93, 70, 22, 126, 236, 188, 185, 41, 199, 74, 158, 145, 117, 103, 235, 30, 121, 63, 221, 108, 9, 85, 200, 133, 202, 150, 117, 101, 160, 237, 196, 223, 252, 5, 162, 28, 244, 137, 74, 72, 129, 221, 0, 38, 3, 123, 84, 146, 152, 10, 55, 92, 180, 228, 150, 181, 176, 250, 161, 247, 205, 139, 234, 124, 209, 207, 203, 11, 176, 196, 6, 4, 95, 220, 15, 168, 171, 6, 112, 19, 142, 103, 240, 183, 133, 53, 94, 246, 56, 153, 204, 198, 7, 240, 181, 247, 119, 191, 207, 51, 36, 212, 166, 55, 93, 32, 208, 56, 140, 121, 143, 90, 172, 104, 2, 62, 138, 63, 36, 224, 136, 224, 199, 172, 119, 0, 177, 160, 121, 171, 68, 40, 51, 255, 183, 92, 85, 69, 83, 104, 36, 169, 114, 112, 225, 54, 231, 197, 96, 161, 164, 216, 114, 92, 139, 136, 229, 23, 132, 19, 47, 165, 190, 169, 61, 98, 110, 176, 143, 105, 152, 75, 151, 23, 114, 226, 94, 210, 40, 56, 132, 118, 157, 214, 212, 191, 217, 5, 166, 117, 131, 112, 7, 222, 98, 96, 205, 130, 87, 173, 51, 99, 98, 227, 160, 225, 38, 75, 169, 54, 62, 146, 231, 210, 5, 22, 140, 71, 117, 65, 206, 211, 117, 185, 35, 218, 5, 65, 195, 63, 137, 186, 32, 16, 63, 26, 21, 109, 118, 200, 52, 96, 44, 180, 26, 115, 54, 35, 30, 55, 149, 66, 11, 206, 106
    };

    printf("array size %d\n", sizeof(array));
    sgx_status_t ret, ret_val;
    std::string str2="hello world";
    uint32_t sealedSize = 0;
    ec_calc_sealed_size(eid_t, &sealedSize, str2.length());
    *sz = sealedSize;
    printf("calc sealed size %d\n", sealedSize);
    uint8_t * str = (uint8_t*)malloc(sealedSize);
    ret = ec_ks_seal(eid_t, &ret_val,(const char*)array, sizeof(array),
                                str2.c_str(), str2.length(),str, sealedSize);
    if(ret != SGX_SUCCESS)
    {
        ret_error_support(ret);
        free(str);
        return NULL;
    }
    else if(ret_val != SGX_SUCCESS)
    {
        ret_error_support(ret_val);
        free(str);
        return NULL;
    }
    printf("sealed %s\n", str);
    return str;
}


void test_read_unseal_data(sgx_enclave_id_t eid_unseal, uint8_t* sealedBlob, uint32_t data_size)
{
    sgx_status_t ret, ret_val;
    uint32_t randVal =0;
    std::string str = "hello";
    printf("test 2\n");
    ret = ec_ks_unseal(eid_unseal, &randVal, str.c_str(), sealedBlob, data_size);
    printf("%d", randVal);
    printf("\n");
    if(ret != SGX_SUCCESS)
    {
        printf("test 1\n");
        ret_error_support(ret);
        return;
    }
    char* unsealStr = (char*)malloc(4096);
    ret = ec_prove_me(eid_unseal, &ret_val, randVal , unsealStr);
    if (ret != SGX_SUCCESS)
    {
        ret_error_support(ret);
        free(unsealStr);
        return;
    }
    else if(ret_val != SGX_SUCCESS)
    {
        ret_error_support(ret_val);
        free(unsealStr);
        return;
    }
    printf("prove me %s\n", unsealStr);
    free(unsealStr);
}

void test_get_public_key(sgx_enclave_id_t eid_t)
{
    sgx_status_t ret, ret_val;
    ret = ec_deliver_public_key(eid_t, &ret_val);
    if(ret != SGX_SUCCESS)
    {
        ret_error_support(ret);
        return;
    }
    else if(ret_val != SGX_SUCCESS)
    {
        ret_error_support(ret_val);
        return;
    }
}
