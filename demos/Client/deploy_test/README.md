# 合约部署时延测试
*说明：用于测试不同数量TEE Providers下，不同合约部署时延，对比实验为不同合约，横坐标为TEE provider数量，纵坐标为部署时延*

## 一、服务端（TEE provider）

1、分别进入$(DEV_PATH)/ClexShard/demos/$(contract name)_tps/deploy_test，执行"bash run.sh x"，其中x为TEE provider数量

结果：启动**x**个TEE Providers，分别监听"10111 - 101x1“端口

## 二、客户端
*目录：$(DEV_PATH)/ClexShard/demos/Client/deploy_test*
1、更改client_latency_xtee.cpp下的变量TEE_NUM，表示用于测试的TEE Provider数量

2、编译：g++ client_latency_xtee.cpp -o latency -lpthread

3、执行指令：./latency ./latency.csv 1 "Test for deployment latency"

结果：客户端会监听”10112 - 101x2“，并分别向”10111 - 101x1“端口发送请求，输出相关合约在**x**个TEE Providers下的部署时延

