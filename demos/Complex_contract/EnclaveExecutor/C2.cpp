#include <cstring>
#include <string>
#include <iostream>
#include "EnclaveExecutor.h"
#include "EnclaveExecutor_t.h"
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
        if(check_version("C1",C1->version()) == SUCCESS){
            int temp_num = 9;
            while(temp_num-- > 0) {
                char buf[BUFSIZ];
                int size = C1->PackageState(buf,BUFSIZ);
                char *c1_state2 = NULL;
                if(message_exchange(STATE_SYNC, buf, size, &c1_state2) == SUCCESS)
                    C1->UpdateState(c1_state2);
            }
            
        }
        if(check_version("C2",C2->version()) == SUCCESS){
            // char buf[BUFSIZ];
            // int size = C1->PackageState(buf,BUFSIZ);
            int temp_num = 9;
            while(temp_num-- > 0) {
                char buf[BUFSIZ];
                int size = C1->PackageState(buf,BUFSIZ);
                char *c2_state2 = NULL;
                if(message_exchange(STATE_SYNC, buf, size, &c2_state2) == SUCCESS){
                    C2->UpdateState(c2_state2);
                };
            }
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

    static C2* instance;
};

C2* C2::instance = NULL;
Contract* C2_instance(){
    if(C2::instance == NULL){
        C2::instance = new C2();
    }
    return C2::instance;
}
