#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

struct List_para {
    int send_SocketFD;
    // char acBuf[4096];
    struct sockaddr_in stRemoteAddr;
    // socklen_t iRemoteAddrLen;
};

void * sendReq(void * arg) {
    int iRecvLen = 0;
    struct List_para *para;
    para = (struct List_para*) arg;
    while(1) {
        // sleep(10);
        
        iRecvLen = sendto(para->send_SocketFD, "这是一个测试字符串", strlen("这是一个测试字符串"), 0, (void *)&para->stRemoteAddr, sizeof(para->stRemoteAddr));
        // printf("Hello World!\n");

    }
    return NULL;
}

int main(int argc, char **argv)
{
	pthread_t stPid = 0; 
	int iRecvLen = 0;
	int iSocketFD = 0;
    int send_SocketFD = 0;
    // int len;
	char acBuf[4096] = {0};
	struct sockaddr_in stLocalAddr = {0};
 
	struct sockaddr_in stRemoteAddr = {0};
	socklen_t iRemoteAddrLen = 0;
 
	/* 创建socket */
	iSocketFD = socket(AF_INET, SOCK_DGRAM, 0);
    send_SocketFD = socket(AF_INET, SOCK_DGRAM, 0);
	if(iSocketFD < 0 || send_SocketFD < 0)
	{
		printf("创建socket失败!\n");
		return 0;
	}
 
	/* 填写地址 */
	stLocalAddr.sin_family = AF_INET;
	stLocalAddr.sin_port   = htons(22345);
	stLocalAddr.sin_addr.s_addr = 0;

 
	/* 绑定地址 */
	if(0 > bind(iSocketFD, (void *)&stLocalAddr, sizeof(stLocalAddr)))
	{
		printf("绑定地址失败!\n");
		close(iSocketFD);
		return 0;
	}

    struct List_para para;
    para.send_SocketFD = send_SocketFD;
    para.stRemoteAddr.sin_family = AF_INET;
	para.stRemoteAddr.sin_port   = htons(12345);
	para.stRemoteAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (pthread_create(&stPid, NULL, sendReq, &para) != 0) {
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

    int totalUdp = atoi(argv[1]);
    // int maxRate = atoi(argv[2]); // caps
    printf("totalUdp: %d\n", totalUdp);
	// printf("maxRate: %d\n", maxRate);
	int sum = 0;
    double rate = 0.0;
	
	while(sum < totalUdp)     //实现了循环监听
	{   
        // rate = sum/(unow - ustart + 0.0);
		if (iRecvLen = recvfrom(iSocketFD, acBuf, sizeof(acBuf), 0, (void *)&stRemoteAddr, &iRemoteAddrLen) > 0) {
			// printf("Hello world! \n");
			printf("%s \n", acBuf);
		}
        sum++;
		gettimeofday(&now, NULL);
		// printf("now time is: %lu", now.tv_sec*1000000 + now.tv_usec);
		printf("TPS is: %.3f \n", 1000000.0/(now.tv_sec*1000000 + now.tv_usec - lunow));
		lunow = now.tv_sec*1000000 + now.tv_usec;
		// printf("last time is: %lu", lunow);
	}
	close(iSocketFD);
    close(send_SocketFD);
 
	return 0;
}