#include "datatypes.h"
#include "stdio.h"

uint32_t check_version(const char* contractID, size_t version);
uint32_t message_exchange(uint32_t msg_type, const char *send_msg, size_t send_msg_len, char **receive_msg);