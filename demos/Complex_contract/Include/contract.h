/* Contract types */

#ifndef __CONTRACT_H__
#define __CONTRACT_H__

#include <string>
#include <cstring>
#include <list>
#include <map>
#include "storage.h"
#include "user_types.h"

typedef int State;
typedef State (*Action) (KVStorage* storage, struct Trace *trace, struct Object *obj);
typedef struct {
	std::map<State, Action> table; 
	KVStorage storage; 
	std::list<Trace*> txs; 
	std::list<State> old_states;
	State state;
} StateMechine;

class Contract{
    size_t _version = 0;
public:
    size_t version(){
        return _version;
    }
    std::string name; 
    StateMechine sm; 
	int Handle(struct Trace *trace,struct Object *obj){ 
		int new_state = this->sm.table[this->sm.state](&(this->sm.storage), trace, obj);
		if(strcmp(obj->Status,"success")==0){
			_version++;
			this->sm.old_states.push_back(this->sm.state);
			this->sm.state = new_state;
			return 1;
		}else
			return 0;
	};
	virtual ~Contract(){};
	virtual int PackageState(char* out_buf, int out_buf_size) = 0;
	virtual int UpdateState(std::string package_state) = 0 ;
};

#endif