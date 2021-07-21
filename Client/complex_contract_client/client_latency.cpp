#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
using namespace std;


struct List_para {
    int send_SocketFD;
    struct sockaddr_in stRemoteAddr;
    string reqMsg;
};

/* test tps */
struct timeval start;

unsigned long lunow = 0;

/* test latency */
int send_num;

void * sendReq(void * arg) {
    int iRecvLen = 0;
    struct List_para *para;
    para = (struct List_para*) arg;
    printf("Send request is: %s \n", para->reqMsg.c_str());
    sleep(3);
	gettimeofday(&start, NULL);
    while(send_num--) {
        // gettimeofday(&start, NULL);
        iRecvLen = sendto(para->send_SocketFD, para->reqMsg.c_str(), strlen(para->reqMsg.c_str()), 0, (struct sockaddr *)&para->stRemoteAddr, sizeof(para->stRemoteAddr));

    }
    // iRecvLen = sendto(para->send_SocketFD, para->reqMsg.c_str(), strlen(para->reqMsg.c_str()), 0, (struct sockaddr *)&para->stRemoteAddr, sizeof(para->stRemoteAddr));
    return NULL;
}

/* run: function "para(string)" */
int main(int argc, char **argv)
{
	string file_path = argv[1];
	ofstream outfile(file_path, ios::out);
    int totalUdp = atoi(argv[2]);
    send_num = totalUdp;
	pthread_t stPid = 0; 
	int iRecvLen = 0;
	int iSocketFD = 0;
    int send_SocketFD = 0;
    // int len;
	char acBuf[4096] = {0};
	struct sockaddr_in stLocalAddr = {0};
 
	struct sockaddr_in stRemoteAddr = {0};
	socklen_t iRemoteAddrLen = 0;
 
	
	iSocketFD = socket(AF_INET, SOCK_DGRAM, 0);
    send_SocketFD = socket(AF_INET, SOCK_DGRAM, 0);
	if(iSocketFD < 0 || send_SocketFD < 0)
	{
		printf("fail to create socket!\n");
		return 0;
	}
 
	
	stLocalAddr.sin_family = AF_INET;
	stLocalAddr.sin_port   = htons(11012);
	stLocalAddr.sin_addr.s_addr = 0;

 
	
	if(0 > bind(iSocketFD, (struct sockaddr *)&stLocalAddr, sizeof(stLocalAddr)))
	{
		printf("fail to bind address!\n");
		close(iSocketFD);
		return 0;
	}

    struct List_para para;
    para.send_SocketFD = send_SocketFD;
    para.stRemoteAddr.sin_family = AF_INET;
	para.stRemoteAddr.sin_port   = htons(11011);
	para.stRemoteAddr.sin_addr.s_addr = 0;
    para.reqMsg = argv[3];
    // printf("In msg is: %s\n", argv[2]);

    if (pthread_create(&stPid, NULL, sendReq, &para) != 0) {
        printf("thread creation failed\n");
    }
    
    // int maxRate = atoi(argv[2]); // caps
    printf("totalUdp: %d\n", totalUdp);
	// printf("maxRate: %d\n", maxRate);
	int sum = 0;
    double rate = 0.0;
	struct timeval now;
    memset(&now, 0, sizeof(struct timeval));

	while(sum < totalUdp)     
	{   
        // rate = sum/(unow - ustart + 0.0);
		if (iRecvLen = recvfrom(iSocketFD, acBuf, sizeof(acBuf), 0, (struct sockaddr *)&stRemoteAddr, &iRemoteAddrLen) > 0) {
			// printf("Hello world! \n");

			printf("Receive message is: %s \n", acBuf);
		}
        sum++;
        
		// gettimeofday(&now, NULL);
        // cout << "Latency is: " << (now.tv_sec*1000000 + now.tv_usec) - (start.tv_sec*1000000 + start.tv_usec) << endl;
		// printf("Latency is: %lu", (now.tv_sec*1000000 + now.tv_usec) - (start.tv_sec*1000000 + start.tv_usec));
		// double temp_tps = 1000000.0/(now.tv_sec*1000000 + now.tv_usec - lunow);
		// printf("TPS is: %.3f \n", temp_tps);
		// outfile << temp_tps << endl;
		// lunow = now.tv_sec*1000000 + now.tv_usec;
		// printf("last time is: %lu", lunow);
	}
    gettimeofday(&now, NULL);
    cout << "Latency is: " << (now.tv_sec*1000000 + now.tv_usec) - (start.tv_sec*1000000 + start.tv_usec) << endl;
	outfile.close();
	close(iSocketFD);
    close(send_SocketFD);
 
	return 0;
}