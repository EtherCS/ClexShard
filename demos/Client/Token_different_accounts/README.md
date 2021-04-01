# Token合约测试
*说明：用于测试不同账户数量下，token合约三个函数（create、get、transfer）的吞吐量和时延，对比实验为不同Token合约函数，横坐标为合约账户数量，纵坐标为吞吐量或时延*

## TPS测试
### 一、服务端（TEE provider）
1. 进入$(DEV_PATH)/ClexShard/demos/Token/multiple_accounts，分别编译Token合约“occlum-g++ Token.cpp -o Token -lcrypto"和账户创建函数“occlum-g++ create_account.cpp -o create -lcrypto”

2. 创建occlum环境：

- mkdir occlum_instance && cd occlum_instance
- occlum init
- cp ../Token ./image/bin
- cp ../create ./image/bin
- cp ../libcrypto.so.1.1 ./image/lib
- SGX_MODE=SIM occlum build

3. 执行"occlum run /bin/create ./$(account number)_state 20 $(account number)"，随机生成$(account number)个账户，包括用于测试的账户“12345678901234567890”，其中账户名为20字节（与以太坊相同），如 occlum run /bin/create ./100_state 20 100
4. 执行“occlum run /bin/Token ./$(account number)_state test”，其中$(account number)与第3步中保持一致，如 occlum run /bin/Token ./100_state test

结果：在1个TEE provider执行包含了$(account number)个账户的Token合约，监听“10011”端口

### 二、客户端
*目录：$(DEV_PATH)/ClexShard/demos/Client/TPS_test*
1. 执行编译指令“g++ client_tps.cpp -o tps -lpthread”，生成可执行文件*tps*

2. 测试相应函数的TPS
- Create函数tps测试：运行“./tps ./tps.csv 100000 "createAc 12345678901234567890 1000" ”
- Get函数tps测试：运行“./tps ./tps.csv 100000 "getBalance 12345678901234567890" ”
- Transfer函数tps测试：运行“./tps ./tps.csv 100000 "transfer 12345678901234567890 12345678901234567890 0" ”

结果：客户端监听“10012”端口，计算100000条TPS，保存在同一目录的tps.csv文件下


## Latency测试
### 一、服务端（TEE provider）（同上）
1. 进入$(DEV_PATH)/ClexShard/demos/Token/multiple_accounts，分别编译Token合约“occlum-g++ Token.cpp -o Token"和账户创建函数“occlum-g++ create_account.cpp -o create_account”

2. 创建occlum环境：

- mkdir occlum_instance && cd occlum_instance
- occlum init
- cp ../Token ./image/bin
- cp ../create_account ./image/bin
- cp ../libcrypto.so.1.1 ./image/lib
- SGX_MODE=SIM occlum build

3. 执行"occlum run /bin/create_account ./$(account number)_state 20 $(account number)"，随机生成$(account number)个账户，包括用于测试的账户“12345678901234567890”，其中账户名为20字节（与以太坊相同）
4. 执行“occlum run /bin/Token ./$(account number)_state test”，其中$(account number)与第3步中保持一致

结果：在1个TEE provider执行包含了$(account number)个账户的Token合约，监听“10011”端口

### 二、客户端
*目录：$(DEV_PATH)/ClexShard/demos/Client/Latency_test*
1、执行编译指令“g++ client_latency.cpp -o latency -lpthread”，生成可执行文件*latency*

2、测试相应函数的Latency
- Create函数latency测试：运行“./latency ./latency.csv 1 "createAc 12345678901234567890 1000" ”
- Get函数latency测试：运行“./latency ./latency.csv 1 "getBalance 12345678901234567890" ”
- Transfer函数latency测试：运行“./latency ./latency.csv 1 "transfer 12345678901234567890 12345678901234567890 0" ”

结果：客户端监听“10012”端口，计算并输出确认时延，保存在同一目录的latency.csv文件下