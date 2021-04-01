#include "EnclaveExecutor.h"
#include "sgx_trts.h"
#include "sgx_tcrypto.h"

sgx_aes_ctr_128bit_key_t AEK[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6 };

/*
 * Encrypts a message under a symmetric key
 */
int encrypt_message(char* input, size_t len_in, char* cipher) {
	// encrypt using AES CTR mode
	uint8_t ctr[16] = {0};
	return sgx_aes_ctr_encrypt(AEK, (uint8_t*)input, (uint32_t)len_in, ctr, 1, (uint8_t*)cipher);
}

/*
 * Decrypts a message under a symmetric key
 */
int decrypt_message(char* input, size_t len_in, char* output) {
	uint8_t ctr[16] = {0};
	return sgx_aes_ctr_decrypt(AEK, (uint8_t*)input, (uint32_t)len_in, ctr, 1, (uint8_t*)output);
}