# Confirmation latency for different contract functions

## Chainspace
please see *$(DEV_PATH)/ShardCon/chainspace/for_testing/README.md*

## ShardCon-no-encrypt
### 1. Running TEE provider
step 1: compile the code
```
$ cd $(DEV_PATH)/ShardCon/demos/$(contract name)/no_encrypt
$ occlum-g++ $(contract name).cpp -o $(contract name)
$ (optional)occlum-g++ create_account.cpp -o create // used in Token contract
```
step 2: initialize the enclave
```
$ mkdir occlum_instance && cd occlum_instance
$ occlum init
$ cp ../$(contract name) ./image/bin
$ (optional)cp ../create ./image/bin // used in Token contract
$ SGX_MODE=SIM occlum build
```
step 3: running TEE providers
```
$ (optional)occlum run /bin/create $(state_dir) $(length_of_address) $(number_of_accounts)
$ occlum run /bin/$(contract name) $(parameter) // the parameter depends on different contract
```

### 2. Running client
step 1: comile the code
```
$ cd $(DEV_PATH)/ShardCon/Client/Latency_test
$ g++ client_latency.cpp -o latency -lpthread 
```
step 2: run client
```
$ ./latency ./latency.csv 1 $(parameter)
```

## ShardCon
### 1. Running TEE provider
step 1: compile the code
```
$ cd $(DEV_PATH)/ShardCon/demos/$(contract name)/encrypt
$ occlum-g++ $(contract name).cpp -o $(contract name)
$ (optional)occlum-g++ create_account.cpp -o create // used in Token contract
```
step 2: initialize the enclave
```
$ mkdir occlum_instance && cd occlum_instance
$ occlum init
$ cp ../$(contract name) ./image/bin
$ cp ../libcrypto.so.1.1 ./image/lib
$ (optional)cp ../create ./image/bin // used in Token contract
$ SGX_MODE=SIM occlum build
```
step 3: running TEE providers
```
$ (optional)occlum run /bin/create $(state_dir) $(length_of_address) $(number_of_accounts)
$ occlum run /bin/$(contract name) $(parameter) // the parameter depends on different contract
```

### 2. Running client
step 1: comile the code
```
$ cd $(DEV_PATH)/ShardCon/Client/Latency_test
$ g++ client_latency.cpp -o latency -lpthread 
```
step 2: run client
```
$ ./latency ./latency.csv 1 $(parameter)
```