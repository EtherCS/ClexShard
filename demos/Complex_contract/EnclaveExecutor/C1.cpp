#include <cstring>
#include <string>
#include <iostream>
#include "EnclaveExecutor.h"
#include "EnclaveExecutor_t.h"

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
    int PackageState(char* buf, int out_buf_size) { // 状态打包（只简单地打包存储信息）
        char out_buf[BUFSIZ];
        int len = mysprintf(buf,"contract:%s\nversion:%d\nkv_begin\n",this->name,this->version());
        for (auto iter=this->sm.storage.begin(); iter!=this->sm.storage.end(); iter++)
        {
            len += mysprintf(buf + len,"%s=%s\n", iter->first, iter->second);
            
        }
        len += mysprintf(buf + len,"kv_end\n");
        int ret;
        ocall_write(&ret,"","");
        ocall_read(&ret,"",out_buf);
        if(0 == encrypt_message(buf, len, out_buf)){  
            return (len*sizeof(char)/32*32+32);
        }
        return 0;
    };
	int UpdateState(std::string package_state) { // 根据打包的状态更新
        char temp[BUFSIZ];
        decrypt_message((char*)package_state.c_str(), package_state.length(), temp);
        int len = (int)strlen(temp);
        for (int i = 0; i < len; i++) {
            switch(temp[i]){
                default: break;
            }
        }
        int ret;
        ocall_write(&ret,"","");
        ocall_read(&ret,"",temp);
        return 1;
    };

    static C1* instance;
};

C1* C1::instance = NULL;
Contract* C1_instance(){
    if(C1::instance == NULL){
        C1::instance = new C1();
        
    }
    return C1::instance;
}
