# The performance test for complex cross-shard contracts
## Throughput test
### 1. Running TEE provider
step 1: compile the code
```
$ cd $(DEV_PATH)/ClexShard/demos/Complex_contract
$ source /opt/intel/sgxsdk/environment  
$ make
```
step 2: running TEE providers
```
$ cd ./bin  
$ ./appteeprovider1  
$ ./appteeprovider2
```
### 2. Running client
step 1: comile the code
```
$ cd $(DEV_PATH)/ClexShard/Client/complex_contract_client
$ g++ client_tps.cpp -o tps -lpthread  
```
step 2: run client
```
$ ./tps ./tps.csv 100000 "Call complex contract request for tps"
```

## Confirmation latency test
### 1. Running TEE provider
step 1: compile the code
```
$ cd $(DEV_PATH)/ClexShard/demos/Complex_contract
$ source /opt/intel/sgxsdk/environment  
$ make
```
step 2: running TEE providers
```
$ cd ./bin  
$ ./appteeprovider1  
$ ./appteeprovider2
```
### 2. Running client
step 1: comile the code
```
$ cd $(DEV_PATH)/ClexShard/Client/complex_contract_client
$ g++ client_latency.cpp -o latency -lpthread
```
step 2: run client
```
$ ./latency ./latency.csv 1 "Call complex contract request for latency"
```