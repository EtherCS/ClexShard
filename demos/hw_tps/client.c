#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <arpa/inet.h>

struct List_para {
    int list_SocketFD;
    char acBuf[4096];
    struct sockaddr_in stRemoteAddr;
    socklen_t iRemoteAddrLen;
};

void * listening(struct List_para para) {
    int iRecvLen = 0;
    printf("client listennig! \n");
    while((iRecvLen = recvfrom(para.list_SocketFD, para.acBuf, sizeof(para.acBuf), 0, (void *)&para.stRemoteAddr, &para.iRemoteAddrLen))>0)     //实现了循环监听
	{
        printf("Hello world! \n");
	}
}

void * sendReq(struct List_para para) {
    while(1) {
        sleep(10);
        // printf("Hello World!\n");
    }
    return NULL;
}

int main(void)
{
    pthread_t stPid = 0; 
	int iRecvLen = 0;
	int iSocketFD = 0;
    int list_SocketFD = 0;
	int iRemotAddr = 0;
	char acBuf[4096] = {0};
	struct sockaddr_in stLocalAddr = {0};
    struct sockaddr_in listentLocalAddr = {0};

    struct List_para list_para; 
    // list_para.iSocketFD = 0;
    // list_para.acBuf = {0};

	struct sockaddr_in stRemoteAddr = {0};
	socklen_t iRemoteAddrLen = 0;
 
	/* 创建socket */
	iSocketFD = socket(AF_INET, SOCK_DGRAM, 0);
    list_SocketFD = socket(AF_INET, SOCK_DGRAM, 0);
	if(iSocketFD < 0 || list_SocketFD < 0)
	{
		printf("创建socket失败!\n");
		return 0;
	}
 
	/* 填写服务端地址 */
	stLocalAddr.sin_family = AF_INET;
	stLocalAddr.sin_port = htons(12345);
	// inet_pton(AF_INET, "192.168.1.230", (void *)&iRemotAddr);
    stLocalAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* 填写客户端地址 */
    listentLocalAddr.sin_family = AF_INET;
	listentLocalAddr.sin_port = htons(22345);
	listentLocalAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    if(0 > bind(list_SocketFD, (void *)&listentLocalAddr, sizeof(listentLocalAddr)))
	{
		printf("绑定地址失败!\n");
		close(list_SocketFD);
		return 0;
	}

    list_para.list_SocketFD = list_SocketFD;
    pthread_create(&stPid, NULL, sendReq, NULL);   //实现了多线程

    // while (1) {
    //     iRecvLen = sendto(iSocketFD, "这是一个测试字符串", strlen("这是一个测试字符串"), 0, (void *)&stLocalAddr, sizeof(stLocalAddr));
    // }
	
    printf("client listennig! \n");
    while((iRecvLen = recvfrom(list_SocketFD, list_para.acBuf, sizeof(list_para.acBuf), 0, (void *)&list_para.stRemoteAddr, &list_para.iRemoteAddrLen))>0)     //实现了循环监听
	{
        printf("Hello world! \n");
	}
 
 
	close(iSocketFD);
    close(list_SocketFD);
 
	return 0;
}