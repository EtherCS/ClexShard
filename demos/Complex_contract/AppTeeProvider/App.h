#include "EnclaveExecutor_u.h"
#include "sgx_eid.h"
#include "sgx_urts.h"

#define ENCLAVE_EXECUTOR_NAME "libenclave_executor.signed.so"

sgx_enclave_id_t enclave_id = 0;
/* Function Description: load responder enclave
 * */
int load_enclave()
{
    sgx_status_t ret = SGX_SUCCESS;
    sgx_launch_token_t token = {0};
    int update = 0;

    ret = sgx_create_enclave(ENCLAVE_EXECUTOR_NAME, SGX_DEBUG_FLAG, &token, &update, &enclave_id, NULL);
    if (ret != SGX_SUCCESS) 
    {
        printf("failed to load enclave %s, error code is 0x%x.\n", ENCLAVE_EXECUTOR_NAME, ret);
        return -1;
    }

    return 0;
}
int destroy_enclave(){
    if(enclave_id!=0)
        sgx_destroy_enclave(enclave_id);
    enclave_id = 0;
    return 0;
}
sgx_enclave_id_t get_enclave_id(){
    if(enclave_id!=0)
        return enclave_id;
    load_enclave();
    return enclave_id;
}