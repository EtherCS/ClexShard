#include <cstring>
#include <string>
#include <iostream>
#include "EnclaveExecutor.h"
#include "EnclaveExecutor_t.h"
#include "stringbuilder.h"

namespace C1_internal{
    const char* Procedures[10]={"func1","sync",NULL};
    /* ECALL */
    int ok_count = 0;
    int ko_count = 0;

    int func1(const char *obj){
        if(strcmp(obj,"ok")==0){
            ok_count++;
            return 1;
        }
        else{
            ko_count++;
            return 0;
        }
    }
    // 解析交易请求
    int findProcedures(const char* Procedure){
        for (int i = 0; i < 2; i++)
            if(strcmp(Procedure,Procedures[i])==0)
                return i;
        return -1;
    }
    int C1_handler(KVStorage* storage, struct Trace * trace,struct Object *obj) {
        char temp[BUFSIZ];
        int ret;
        ocall_write(&ret,"","");
        ocall_read(&ret,"",temp);
        int i = findProcedures(trace->Procedure);
        if (storage==NULL || i==-1) return 0;
        switch (i){
        case 0:{
            if(func1(trace->OutputObjects[0]->Object)){
                mystrcpy(obj->Status,"success");
            }else
                mystrcpy(obj->Status,"fail");
            break;
        }case 1:{
            
        }default:
            return 0;
        }
        return 0;
    }
}

class C1: public Contract {
public:
    C1(){
        this->name = "C1";
        this->sm.state = 0;
        this->sm.table[0] = C1_internal::C1_handler;
    }
    ~C1(){
    }
    std::string PackageState() { // 状态打包（只简单地打包存储信息）
        StringBuilder<char> *result = new StringBuilder<char>();
        result->Append("contract:C2\n");
        char buf[BUFSIZ];
        mysprintf(buf,"version:%d\n",this->version());
        result->Append(buf);
        result->Append("kv_begin\n");
        for (auto iter=this->sm.storage.begin(); iter!=this->sm.storage.end(); iter++)
        {
            mysprintf(buf,"%s=%s\n", iter->first, iter->second);
            result->Append(buf);
        }
        result->Append("kv_end\n");
        std::string str = result->ToString();
        char temp[BUFSIZ*BUFSIZ];
        if(0 == encrypt_message((char*)str.c_str(), str.length(), temp)){    
            return std::string(temp);
        }
        int ret;
        ocall_write(&ret,"","");
        ocall_read(&ret,"",temp);
        return "encrypt: failed.";
    }
	int UpdateState(std::string package_state) { // 根据打包的状态更新
        char temp[BUFSIZ*BUFSIZ];
        decrypt_message((char*)package_state.c_str(), package_state.length(), temp);
        for (int i = 0; i < strlen(temp); i++) {
            switch(package_state[i]){
                default: break;
            }
        }
        int ret;
        ocall_write(&ret,"","");
        ocall_read(&ret,"",temp);
        return 1;
    }
    static C1* instance;
};

C1* C1::instance = NULL;
Contract* C1_instance(){
    if(C1::instance == NULL){
        C1::instance = new C1();
        
    }
    return C1::instance;
}
