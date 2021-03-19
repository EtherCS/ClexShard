#ifndef _ENCLAVE_H_
#define _ENCLAVE_H_

#include <assert.h>
#include <stdlib.h>

#include "contract.h"

#if defined(__cplusplus)
extern "C" {
#endif

int mysprintf(char *buf, const char *fmt, ...);
char* mystrcpy(char* dst,const char*src);
int storage_set_key(KVStorage *storage, const char *key, const char *val);
int storage_get_key(KVStorage *storage, const char *key, char *val, size_t len);
int encrypt_message(char* input, size_t len_in, char* cipher);
int decrypt_message(char* input, size_t len_in, char* output);

Contract* C1_instance();
Contract* C2_instance();

#if defined(__cplusplus)
}
#endif

#endif /* !_ENCLAVE_H_ */
