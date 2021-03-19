#include "EnclaveExecutor.h"

#include "user_types.h"
int storage_set_key(KVStorage *storage, const char *key, const char *val){
    if(storage == NULL) return 0;
    storage->operator[](key) = val;
    return 1;
}
int storage_get_key(KVStorage *storage, const char *key, char *val, size_t len){
    if(storage == NULL) return 0;
    try{
        std::string v = storage->at(key);
        strlcpy(val,v.c_str(),len);
    }catch(std::out_of_range err){
        return 0;
    };
    return 1;
}