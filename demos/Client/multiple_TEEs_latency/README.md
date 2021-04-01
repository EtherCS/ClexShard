# 合约函数确认时延测试
*说明：用于测试不同数量TEE Provider下，不同合约函数确认时延。对比实验为不同合约函数，横坐标为TEE Provider数量，纵坐标为确认时延*

## 一、服务端（TEE provider）
1. 分别进入$(DEV_PATH)/ClexShard/demos/$(contract name)/multiple_TEE_Providers，执行"bash run.sh x"，其中*x*为TEE provider数量
*注意：执行Token合约时默认创建100个账户*

结果：enclave中运行*x*个TEE provider，分别监听“10011 - 100x1”端口，

## 二、客户端
*目录：$(DEV_PATH)/ClexShard/demos/Client/multiple_TEEs_latency*
1. 执行编译指令“g++ client_latency_xtee.cpp -o latencyx -lpthread”，生成可执行文件*latencyx*，其中*x*需和服务端的*x*一致

2. 运行“./latencyx ./latencyx.csv 1 "根据所调用的函数确定" ”

3. 测试完后记得执行“pkill -9 occlum”结束所有occlum进程

结果：客户端分别向服务端所监听的端口发送一条交易请求，计算收到所有TEE provider反馈后的时延