#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
using namespace std;

const int TEE_NUM = 3;
bool received_sig[TEE_NUM];

/* test tps */
struct timeval start;

/* test latency */
int send_num;

struct Send_para {
    int send_SocketFD[TEE_NUM];
    struct sockaddr_in stRemoteAddr[TEE_NUM];
    string reqMsg[TEE_NUM];
};

struct Rec_para {
	int tee_num;
	int socketFD;
	struct sockaddr_in stRemoteAddr;
	struct sockaddr_in stLocalAddr;
};

void * sendReq(void * arg) {
    int iRecvLen = 0;
    struct Send_para *para;
    para = (struct Send_para*) arg;
    printf("Send request is: %s \n", para->reqMsg[0].c_str());
    // printf("Send request is: %s \n", para->reqMsg[1].c_str());
	sleep(3);
	gettimeofday(&start, NULL);
    while(send_num--) {
		for (int i = 0; i < TEE_NUM; i++) {
			iRecvLen = sendto(para->send_SocketFD[i], para->reqMsg[i].c_str(), strlen(para->reqMsg[i].c_str()), 0, (struct sockaddr *)&para->stRemoteAddr[i], sizeof(para->stRemoteAddr[i]));
		}
	}
    return NULL;
}

void * recSig(void * arg) {
	int iRecvLen = 0;
	char acBuf[4096];
	socklen_t iRemoteAddrLen = {0};
	struct Rec_para *para;
	para = (struct Rec_para*) arg;
	if (0 > bind(para->socketFD, (struct sockaddr *)&para->stLocalAddr, sizeof(para->stLocalAddr))) {
		printf("Bind address failed!\n");
		close(para->socketFD);
		exit(0);
	}
	while (1)
	{
		if (iRecvLen = recvfrom(para->socketFD, acBuf, sizeof(acBuf), 0, (struct sockaddr *)&para->stRemoteAddr, &iRemoteAddrLen) > 0) {
			printf("Receive message from %d\n", para->tee_num);
			printf("Receive message is: %s \n", acBuf);
			// printf("1\n");
			received_sig[para->tee_num] = true;
		}
	}
	
} 

/* run: ./tps ./tps.csv 10000 function "para(string)" */
int main(int argc, char **argv)
{
	string file_path = argv[1];
	ofstream outfile(file_path, ios::out);
    int totalUdp = atoi(argv[2]);
	string send_msg = argv[3];
	send_num = totalUdp;
	pthread_t stPid[TEE_NUM+1] = {0}; 
	int iRecvLen[TEE_NUM] = {0};
	int iSocketFD[TEE_NUM] = {0};
    int send_SocketFD[TEE_NUM] = {0};
	struct sockaddr_in stLocalAddr[TEE_NUM] = {0};
	struct sockaddr_in stRemoteAddr[TEE_NUM] = {0};
	socklen_t iRemoteAddrLen[TEE_NUM] = {0};
 
	/* create socket */
	string str_loc_temp_port;
	string str_rem_temp_port;
	struct Rec_para rec_para[TEE_NUM];
	struct Send_para send_para;
	for (int i = 0; i < TEE_NUM; i++) {
		/* initialize signal */
		received_sig[i] = false;
		str_loc_temp_port = "";
		str_loc_temp_port = "";
		iSocketFD[i] = socket(AF_INET, SOCK_DGRAM, 0);
    	send_SocketFD[i] = socket(AF_INET, SOCK_DGRAM, 0);
		if(iSocketFD[i] < 0 || send_SocketFD[i] < 0)
		{
			printf("create socket failed!\n");
			return 0;
		}
		/* define socket */
		int temp_TEE_num = i+1;
		str_loc_temp_port = "100" + to_string(temp_TEE_num) + "2";
		// str_temp_port += "2";
		printf("listening port is: %s\n", str_loc_temp_port.c_str());
		// uint16_t temp_port = (uint16_t)(atoi(str_loc_temp_port.c_str()));
		stLocalAddr[i].sin_family = AF_INET;
		stLocalAddr[i].sin_port   = htons((uint16_t)(atoi(str_loc_temp_port.c_str())));
		stLocalAddr[i].sin_addr.s_addr = 0;

		/* define para */
		rec_para[i].tee_num = i;
		rec_para[i].socketFD = iSocketFD[i];
		rec_para[i].stLocalAddr = stLocalAddr[i];
		rec_para[i].stRemoteAddr = stRemoteAddr[i];
		if (pthread_create(&stPid[i], NULL, recSig, &rec_para[i]) != 0) {
			printf("thread creation failed\n");
		}

		/* send thread */
		str_rem_temp_port = "100" + to_string(temp_TEE_num) + "1";
		send_para.send_SocketFD[i] = send_SocketFD[i];
		send_para.stRemoteAddr[i].sin_family = AF_INET;
		send_para.stRemoteAddr[i].sin_port   = htons((uint16_t)(atoi(str_rem_temp_port.c_str())));
		send_para.stRemoteAddr[i].sin_addr.s_addr = 0;
		send_para.reqMsg[i] = send_msg;
	}

    if (pthread_create(&stPid[TEE_NUM], NULL, sendReq, &send_para) != 0) {
        printf("thread creation failed\n");
    }
    
    // int maxRate = atoi(argv[2]); // caps
    printf("totalUdp: %d\n", totalUdp);
	// printf("maxRate: %d\n", maxRate);
	int sum = 0;
    double rate = 0.0;
	struct timeval now;
    memset(&now, 0, sizeof(struct timeval));

	while(sum < totalUdp)     //实现了循环监听
	{   
		for (int i = 0; i < TEE_NUM; i++) {
			while (1) {
				if (received_sig[i]) {
					break;
				}
			}
			
		}
		sum++;
	}
	gettimeofday(&now, NULL);
    cout << "Latency is: " << (now.tv_sec*1000000 + now.tv_usec) - (start.tv_sec*1000000 + start.tv_usec) << endl;
	outfile.close();
	/* close socket */
	for (int i = 0; i < TEE_NUM; i++) {
		close(iSocketFD[i]);
    	close(send_SocketFD[i]);
	}
	return 0;
}