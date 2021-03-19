#include <cstring>
#include <string>
#include <iostream>
#include "EnclaveExecutor.h"
#include "EnclaveExecutor_t.h"
#include "stringbuilder.h"
#include "EnclaveMessage.h"
#include "example.h"

#define SUCCESS 0
#define FAIL    1

namespace C2_internal{
    const char* Procedures[10]={"func2",NULL};
    int ok_count = 0;
    int ko_count = 0;
    /* ECALL */
    int func2(struct Trace * trace){
        Contract* C1 = C1_instance();
        Contract* C2 = C2_instance();
        // 1. local attestation 验证身份
        if(test_create_session() != SUCCESS){
            return FAIL;
        };
        if(test_message_exchange() != SUCCESS){
            test_close_session();
            return FAIL;
        }
        // 2. 检查状态并交换状态
        if(check_version("C1",C1->version()) == FAIL){
            std::string c1_state = C1->PackageState();
            char *c1_state2 = NULL;
            if(message_exchange(STATE_SYNC, c1_state.c_str(), c1_state.length(), &c1_state2) == SUCCESS)
                C1->UpdateState(c1_state2);
            else
                return FAIL;
        }
        if(check_version("C2",C2->version()) == FAIL){
            std::string c2_state = C2->PackageState();
            char *c2_state2 = NULL;
            if(message_exchange(STATE_SYNC, c2_state.c_str(), c2_state.length(), &c2_state2) == SUCCESS){
                C2->UpdateState(c2_state2);
            };
        }
        
        test_close_session();

        // 3. 嵌套调用
        struct Object obj;
        C1->Handle(&fun1_trace,&obj);
        
        // 后面随意

        return SUCCESS;
    }
    // 解析交易请求
    int findProcedures(const char* Procedure){
        for (int i = 0; i < 1; i++)
            if(strcmp(Procedure,Procedures[i])==0)
                return i;
        return -1;
    }
    int C2_handler(KVStorage* storage, struct Trace * trace,struct Object *obj) {
        char temp[BUFSIZ];
        int ret;
        ocall_write(&ret,"","");
        ocall_read(&ret,"",temp);
        int i = findProcedures(trace->Procedure);
        if (i==-1) return 0;
        switch (i){
        case 0:{
            if(func2(trace) == SUCCESS){
                mystrcpy(obj->Status,"success");
            }else
                mystrcpy(obj->Status,"fail");
            break;
        }default:
            return 0;
        }
        return 0;
    }
}

class C2: public Contract {
public:
    C2(){
        this->name = "C2";
        this->sm.state = 0;
        this->sm.table[0] = C2_internal::C2_handler;
    }
    ~C2(){
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
    static C2* instance;
};

C2* C2::instance = NULL;
Contract* C2_instance(){
    if(C2::instance == NULL){
        C2::instance = new C2();
    }
    return C2::instance;
}
