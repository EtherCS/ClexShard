# Deployment latency test
## 1. Running TEE provider
step 1: running TEE providers
```
$ cd $(DEV_PATH)/ClexShard/demos/$(contract name)/deploy_test
$ bash run.sh x
// x is the size of executing set, i.e., how many TEE providers are appointed to maintain the cross-shard contract
```

## 2. Running client
step 1: comile the code
```
$ cd $(DEV_PATH)/ClexShard/demos/Client/deploy_test
change the variable x in client_latency_xtee.cpp as the number of TEE providers 
$ g++ client_latency_xtee.cpp -o latency -lpthread  
```
step 2: run client
```
$ ./latency ./latency.csv 1 "Test for deployment latency"
```


