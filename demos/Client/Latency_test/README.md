# 合约函数latency测试
*说明：用于测试不同模型下（chainspace、ClexShard-no-encrypt、ClexShard），单个TEE provider维护下，不同合约函数确认延时。对比实验为不同模型，横坐标为不同合约函数，纵坐标为确认延时*

## Chainspace
*TODO*

## ClexShard-no-encrypt
### 一、服务端（TEE provider）
1. 分别进入$(DEV_PATH)/ClexShard/demos/$(contract name)/no_encrypt，编译无加密的合约：“occlum-g++ 合约名.cpp -o 合约名”

2. 创建occlum环境：

- mkdir occlum_instance && cd occlum_instance
- occlum init
- cp ../合约名 ./image/bin
- SGX_MODE=SIM occlum build

3. 执行"occlum run /bin/$(contract name) 参数根据不同合约而定"
*注意：测试Token合约时，需要用create_account在enclave中生成100个账户*

结果：在1个TEE provider执行智能合约，监听“10011”端口

### 二、客户端
*目录：$(DEV_PATH)/ClexShard/demos/Client/Latency_test*
1、执行编译指令“g++ client_latency.cpp -o latency -lpthread”，生成可执行文件*latency*

2、运行“./latency ./latency.csv 1 "根据所调用的函数确定" ”

结果：客户端监听“10012”端口，计算并输出相关函数确认时延，保存在同一目录的latency.csv文件下

## ClexShard
### 一、服务端（TEE provider）
1. 分别进入$(DEV_PATH)/ClexShard/demos/$(contract name)/no_encrypt，编译加密的合约：“occlum-g++ 合约名.cpp -o 合约名 -lcrypto”

2. 创建occlum环境：

- mkdir occlum_instance && cd occlum_instance
- occlum init
- cp ../合约名 ./image/bin
- cp ../libcrypto.so.1.1 ./image/lib
- SGX_MODE=SIM occlum build

3. 执行"occlum run /bin/$(contract name) 参数根据不同合约而定"
*注意：测试Token合约时，需要用create_account在enclave中生成100个账户*

结果：在1个TEE provider执行智能合约，监听“10011”端口

### 二、客户端
*目录：$(DEV_PATH)/ClexShard/demos/Client/Latency_test*
1、执行编译指令“g++ client_latency.cpp -o latency -lpthread”，生成可执行文件*latency*

2、运行“./latency ./latency.csv 1 "根据所调用的函数确定" ”

结果：客户端监听“10012”端口，计算并输出相关函数确认时延，保存在同一目录的latency.csv文件下