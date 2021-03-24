#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
// #include <netinet/ip.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
using namespace std;

const int TEE_NUM = 2;
bool received_sig[TEE_NUM];
struct Send_para {
    int send_SocketFD[TEE_NUM];
    struct sockaddr_in stRemoteAddr[TEE_NUM];
    string reqMsg[TEE_NUM];
};

struct Rec_para {
	int socketFD;
	struct sockaddr_in stRemoteAddr;
	struct sockaddr_in stLocalAddr;
};

void * sendReq(void * arg) {
    int iRecvLen = 0;
    struct Send_para *para;
    para = (struct Send_para*) arg;
    printf("Send request is: %s \n", para->reqMsg[0].c_str());
    printf("Send request is: %s \n", para->reqMsg[1].c_str());
    while(1) {
        // sleep(10);
        iRecvLen = sendto(para->send_SocketFD[0], para->reqMsg[0].c_str(), strlen(para->reqMsg[0].c_str()), 0, (struct sockaddr *)&para->stRemoteAddr[0], sizeof(para->stRemoteAddr[0]));
        iRecvLen = sendto(para->send_SocketFD[1], para->reqMsg[1].c_str(), strlen(para->reqMsg[1].c_str()), 0, (struct sockaddr *)&para->stRemoteAddr[1], sizeof(para->stRemoteAddr[1]));
        // printf("Hello World!\n");

    }
    return NULL;
}

void * recSig1(void * arg) {
	int iRecvLen = 0;
	char acBuf[4096];
	socklen_t iRemoteAddrLen = {0};
	struct Rec_para *para;
	para = (struct Rec_para*) arg;
	printf("Start listening: %s \n", inet_ntoa(para->stLocalAddr.sin_addr));
	if (0 > bind(para->socketFD, (struct sockaddr *)&para->stLocalAddr, sizeof(para->stLocalAddr))) {
		printf("绑定地址失败!\n");
		close(para->socketFD);
		exit(0);
	}
	while (1)
	{
		if (iRecvLen = recvfrom(para->socketFD, acBuf, sizeof(acBuf), 0, (struct sockaddr *)&para->stRemoteAddr, &iRemoteAddrLen) > 0) {
			// printf("Receive message from %s is: %s \n", inet_ntoa(para->stLocalAddr.sin_addr), acBuf);
			// printf("1\n");
			received_sig[0] = true;
		}
	}
	
} 

void * recSig2(void * arg) {
	int iRecvLen = 0;
	char acBuf[4096];
	socklen_t iRemoteAddrLen = {0};
	struct Rec_para *para;
	para = (struct Rec_para*) arg;
	printf("Start listening: %s \n", inet_ntoa(para->stLocalAddr.sin_addr));
	if (0 > bind(para->socketFD, (struct sockaddr *)&para->stLocalAddr, sizeof(para->stLocalAddr))) {
		printf("绑定地址失败!\n");
		close(para->socketFD);
		exit(0);
	}
	while (1)
	{
		// iRecvLen = recvfrom(para->socketFD, acBuf, sizeof(acBuf), 0, (struct sockaddr *)&para->stRemoteAddr, &iRemoteAddrLen);
		if (iRecvLen = recvfrom(para->socketFD, acBuf, sizeof(acBuf), 0, (struct sockaddr *)&para->stRemoteAddr, &iRemoteAddrLen) > 0) {
			// printf("2\n");
			received_sig[1] = true;
		}
	}
	
}

/* run: ./tps ./tps.csv 10000 function "para(string)" */
int main(int argc, char **argv)
{
	string file_path = argv[1];
	ofstream outfile(file_path, ios::out);
    int totalUdp = atoi(argv[2]);
	pthread_t stPid[3] = {0, 0, 0}; 
	int iRecvLen[TEE_NUM] = {0, 0};
	int iSocketFD[TEE_NUM] = {0, 0};
    int send_SocketFD[TEE_NUM] = {0, 0};
    // int len;
	char acBuf1[4096] = {0};
    char acBuf2[4096] = {0};
	struct sockaddr_in stLocalAddr[TEE_NUM] = {0};
 
	struct sockaddr_in stRemoteAddr[TEE_NUM] = {0};
	socklen_t iRemoteAddrLen[TEE_NUM] = {0, 0};
 
	/* 创建socket */
	iSocketFD[0] = socket(AF_INET, SOCK_DGRAM, 0);
    send_SocketFD[0] = socket(AF_INET, SOCK_DGRAM, 0);
    iSocketFD[1] = socket(AF_INET, SOCK_DGRAM, 0);
    send_SocketFD[1] = socket(AF_INET, SOCK_DGRAM, 0);
	if(iSocketFD < 0 || send_SocketFD < 0)
	{
		printf("创建socket失败!\n");
		return 0;
	}
 
	/* 填写地址 */
	stLocalAddr[0].sin_family = AF_INET;
	stLocalAddr[0].sin_port   = htons(10002);
	stLocalAddr[0].sin_addr.s_addr = 0;
    stLocalAddr[1].sin_family = AF_INET;
	stLocalAddr[1].sin_port   = htons(20002);
	stLocalAddr[1].sin_addr.s_addr = 0;
	struct Rec_para rec_para[2];
	rec_para[0].socketFD = iSocketFD[0];
	rec_para[0].stLocalAddr = stLocalAddr[0];
	rec_para[0].stRemoteAddr = stRemoteAddr[0];
	rec_para[1].socketFD = iSocketFD[1];
	rec_para[1].stLocalAddr = stLocalAddr[1];
	rec_para[1].stRemoteAddr = stRemoteAddr[1];
	if (pthread_create(&stPid[0], NULL, recSig1, &rec_para[0]) != 0) {
        printf("thread creation failed\n");
    }
	if (pthread_create(&stPid[1], NULL, recSig2, &rec_para[1]) != 0) {
        printf("thread creation failed\n");
    }
	
	/* 绑定地址 */
	// if(0 > bind(iSocketFD[0], (struct sockaddr *)&stLocalAddr[0], sizeof(stLocalAddr[0])) )
	// {
	// 	printf("绑定地址失败!\n");
	// 	close(iSocketFD[0]);
    //     close(iSocketFD[1]);
	// 	return 0;
	// }
	// sleep(3);
    struct Send_para send_para;
    send_para.send_SocketFD[0] = send_SocketFD[0];
    send_para.stRemoteAddr[0].sin_family = AF_INET;
	send_para.stRemoteAddr[0].sin_port   = htons(10001);
	send_para.stRemoteAddr[0].sin_addr.s_addr = 0;
    send_para.reqMsg[0] = argv[3];
    send_para.send_SocketFD[1] = send_SocketFD[1];
    send_para.stRemoteAddr[1].sin_family = AF_INET;
	send_para.stRemoteAddr[1].sin_port   = htons(20001);
	send_para.stRemoteAddr[1].sin_addr.s_addr = 0;
    send_para.reqMsg[1] = argv[3];
    // printf("In msg is: %s\n", argv[2]);

    if (pthread_create(&stPid[2], NULL, sendReq, &send_para) != 0) {
        printf("thread creation failed\n");
    }

	/* test tps */
	struct timeval start;
    gettimeofday(&start, NULL);
    unsigned long ustart = start.tv_sec*1000000 + start.tv_usec;

    usleep(1);

    struct timeval now;
    memset(&now, 0, sizeof(struct timeval));
    unsigned long lunow = 0;

    
    // int maxRate = atoi(argv[2]); // caps
    printf("totalUdp: %d\n", totalUdp);
	// printf("maxRate: %d\n", maxRate);
	int sum = 0;
    double rate = 0.0;
	received_sig[0] = false;
	received_sig[1] = false;
	while(sum < totalUdp)     //实现了循环监听
	{   
        // rate = sum/(unow - ustart + 0.0);
		// iRecvLen[0] = recvfrom(iSocketFD[0], acBuf1, sizeof(acBuf1), 0, (struct sockaddr *)&stRemoteAddr[0], &iRemoteAddrLen[0]);
		if (received_sig[0] && received_sig[1]) {
			sum++;
			gettimeofday(&now, NULL);
			// printf("now time is: %lu", now.tv_sec*1000000 + now.tv_usec);
			double temp_tps = 1000000.0/(now.tv_sec*1000000 + now.tv_usec - lunow);
			printf("TPS is: %.3f \n", temp_tps);
			outfile << temp_tps << endl;
			lunow = now.tv_sec*1000000 + now.tv_usec;
			received_sig[0] = false;
			received_sig[1] = false;
		}

	}
	outfile.close();
	close(iSocketFD[0]);
    close(send_SocketFD[0]);
    close(iSocketFD[1]);
    close(send_SocketFD[1]);
 
	return 0;
}