# Confirmation latency for different size of executing set
## 1. Running TEE provider
step 1: compile the code
```
$ cd $(DEV_PATH)/ClexShard/demos/$(contract name)/multiple_TEE_Providers
// repeat: from x=1 to x=8
    $ occlum-g++ $x_(contract name).cpp -o x 
$ (optional)occlum-g++ create_account.cpp -o create // used in Token contract
```
step 2: initialize the enclave
```
// repeat: from x=1 to x=8
    $ mkdir occlum_instance && cd occlum_instance
    $ occlum init
    $ cp ../x ./image/bin
    $ (optional)cp ../create ./image/bin // used in Token contract
    $ SGX_MODE=SIM occlum build
```
step 3: running TEE providers
```
$ (optional)occlum run /bin/create $(state_dir) $(length_of_address) $(number_of_accounts)
$ bash run.sh x
// x denotes the size of executing set for test
```

## 2. Running client
step 1: comile the code
```
$ cd $(DEV_PATH)/ClexShard/demos/Client/multiple_TEEs_latency
$ g++ client_latency_xtee.cpp -o latencyx -lpthread
// x is corresponding to the above x
```
step 2: run client
```
$ ./latency ./latency.csv 1 $(parameter)
```
step 3: release the resources
```
$ pkill -9 occlum
```
