# 测试复杂跨分片智能合约性能
*说明：用于测试不同复杂程度跨分片智能合约的吞吐量和确认时延，对比实验为复杂跨分片智能合约所涉及的跨分片智能合约数量（2个或者3个，在假定复杂跨分片智能合约不存在合约状态的情况下，分别需要同步1个和2个合约状态），横坐标为同步合约状态的大小（单位为字节），纵坐标为吞吐量和确认时延*

## 吞吐量测试
### 一、服务端（TEE provider）
1、进入$(DEV_PATH)/ClexShard/demos/Complex_contract，修改所需同步的合约状态大小**BUFSIZ**，测试2合约则注释掉“if(check_version("C2",C2->version()) == SUCCESS)”这一段，测试3合约则无需注释

2、在./complex_sc目录下，执行source /opt/intel/sgxsdk/environment && make

3、进入./bin，分别执行"./appteeprovider1 1"和"./appteeprovider1 2"

结果：appteeprovider1和appteeprovider2之间通过local attestation建立可信连接，分别监听“11011和11021”端口，等待客户端发送调用指令，并通过建立起来的可信连接同步指定大小的合约状态

### 二、客户端（client）
1、进入$(DEV_PATH)/ClexShard/demos/Client/complex_contract_client目录，执行"g++ client_tps.cpp -o tps -lpthread"，生成用于测试复杂跨分片智能合约的可执行文件

2、运行“./tps ./tps.csv 100000 "Call complex contract request for tps" ”

结果：客户端监听11012端口，并给11011端口发送请求，计算**100000**次tps

## 确认时延测试
### 一、服务端（TEE provider）
1、进入$(DEV_PATH)/ClexShard/demos/Complex_contract，修改所需同步的合约状态大小**BUFSIZ**，测试2合约则注释掉“if(check_version("C2",C2->version()) == SUCCESS)”这一段，测试3合约则无需注释

2、在./complex_sc目录下，执行source /opt/intel/sgxsdk/environment && make

3、进入./bin，分别执行"./appteeprovider1 1"和"./appteeprovider1 2"

结果：appteeprovider1和appteeprovider2之间通过local attestation建立可信连接，分别监听“11011和11021”端口，等待客户端发送调用指令，并通过建立起来的可信连接同步指定大小的合约状态

### 二、客户端（client）
1、进入$(DEV_PATH)/ClexShard/demos/Client/complex_contract_client目录，执行"g++ client_latency.cpp -o latency -lpthread"，生成用于测试复杂跨分片智能合约的可执行文件

2、运行“./latency ./latency.csv 1 "Call complex contract request for latency" ”

结果：客户端监听11012端口，并给11011端口发送请求，计算合约调用确认时延