#include <cstring>
#include <string>
#include <map>
#include "EnclaveExecutor.h"
#include "EnclaveExecutor_t.h"

std::map<std::string, Contract*> contracts={
    std::pair<std::string,Contract*>("C1",C1_instance()),
    std::pair<std::string,Contract*>("C2",C2_instance())
};
void ecall_handle(struct Transaction * tx, struct Object *obj){
    if (tx != NULL){
        // for (int i = 0; i < 100; i++){
            Trace *trace = tx->Traces[0];
            if (trace != NULL){
                // TODO: 按照chainspace应该是要查map来检查状态是否一致的
                Contract* contract = contracts[std::string(trace->ContractID)];
                if(contract == NULL) return;
                contract->Handle(trace,obj);
            }
        // }
    }
    return;
}

int mysprintf(char *buf, const char *fmt, ...)
{
    va_list ap;
    int ret;
    va_start(ap, fmt);
    ret = vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    return ret;
}

char* mystrcpy(char* dst,const char*src){
    char* t=dst;
    while((*dst++=*src++)!='\0');
    return t;
}