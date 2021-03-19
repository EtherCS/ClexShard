/* Contract types */

#ifndef __CONTRACT_H__
#define __CONTRACT_H__

#include <string>
#include <cstring>
#include <list>
#include <map>
#include "storage.h"
#include "user_types.h"

// 状态
typedef int State;
// 状态转移函数=(旧状态state+输入trace->输出new_state(+中间操作obj))
typedef State (*Action) (KVStorage* storage, struct Trace *trace, struct Object *obj);
// 合约NFA状态机
typedef struct {
	std::map<State, Action> table; // 状态转移函数表
	KVStorage storage; // 合约存储内容
	std::list<Trace*> txs; // 调用交易记录
	std::list<State> old_states; // 旧合约状态
	State state; // 当前合约状态
} StateMechine;

// 合约
class Contract{
    size_t _version = 0;
public:
    size_t version(){
        return _version;
    }
    std::string name; // 合约名字
    StateMechine sm; // 合约状态机
	int Handle(struct Trace *trace,struct Object *obj){ // 更新操作
		int new_state = this->sm.table[this->sm.state](&(this->sm.storage), trace, obj);
		if(strcmp(obj->Status,"success")==0){
			_version++;
			// this->sm.txs.push_back(trace); //存储交易信息用于验证
			this->sm.old_states.push_back(this->sm.state);
			this->sm.state = new_state;
			return 1;
		}else
			return 0;
	};
	virtual ~Contract(){};
	virtual std::string PackageState() = 0; // 状态打包
	virtual int UpdateState(std::string package_state) = 0; // 根据打包的状态更新
};

#endif