# The impact of different number of accounts of Token contract in throughput and confirmation latency
## Throughput test
### 1. Running TEE provider
step 1: compile the code
```
$ cd $(DEV_PATH)/ShardCon/demos/Token/multiple_accounts
$ occlum-g++ Token.cpp -o Token -lcrypto
$ occlum-g++ create_account.cpp -o create -lcrypto
```
step 2: initialize the enclave
```
$ mkdir occlum_instance && cd occlum_instance
$ occlum init
$ cp ../Token ./image/bin
$ cp ../create ./image/bin
$ cp ../libcrypto.so.1.1 ./image/lib
$ SGX_MODE=SIM occlum build
```
step 3: running TEE providers
```
$ occlum run /bin/create ./$(account number)_state 20 $(account number)
$ occlum run /bin/Token ./$(account number)_state test
```
### 2. Running client
step 1: comile the code
```
$ cd $(DEV_PATH)/ShardCon/Client/TPS_test
$ g++ client_tps.cpp -o tps -lpthread
```
step 2: run client
```
$ ./tps ./tps.csv 100000 "createAc 12345678901234567890 1000" // test create function
$ ./tps ./tps.csv 100000 "getBalance 12345678901234567890"  // test get function
$ /tps ./tps.csv 100000 "transfer 12345678901234567890 12345678901234567890 0"  // test transfer function
```

## Confirmation latency test
### 1. Running TEE provider
step 1: compile the code
```
$ cd $(DEV_PATH)/ShardCon/demos/Token/multiple_accounts
$ occlum-g++ Token.cpp -o Token -lcrypto
$ occlum-g++ create_account.cpp -o create -lcrypto
```
step 2: initialize the enclave
```
$ mkdir occlum_instance && cd occlum_instance
$ occlum init
$ cp ../Token ./image/bin
$ cp ../create ./image/bin
$ cp ../libcrypto.so.1.1 ./image/lib
$ SGX_MODE=SIM occlum build
```
step 3: running TEE providers
```
$ occlum run /bin/create ./$(account number)_state 20 $(account number)
$ occlum run /bin/Token ./$(account number)_state test
```
### 2. Running client
step 1: comile the code
```
$ cd $(DEV_PATH)/ShardCon/Client/Latency_test
$ g++ client_latency.cpp -o latency -lpthread
```
step 2: run client
```
$ ./latency ./latency.csv 1 "createAc 12345678901234567890 1000" // test create function
$ ./latency ./latency.csv 1 "getBalance 12345678901234567890"  // test get function
$ ./latency ./latency.csv 1 "transfer 12345678901234567890 12345678901234567890 0"  // test transfer function
```