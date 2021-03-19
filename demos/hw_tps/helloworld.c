#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <string.h>
 
int main(void)
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
	stLocalAddr.sin_port   = htons(12345);
	stLocalAddr.sin_addr.s_addr = 0;
 
	/* 绑定地址 */
	if(0 > bind(iSocketFD, (void *)&stLocalAddr, sizeof(stLocalAddr)))
	{
		printf("绑定地址失败!\n");
		close(iSocketFD);
		return 0;
	}
	// pthread_create(&stPid, NULL, test, NULL);   //实现了多线程
	
	while((iRecvLen = recvfrom(iSocketFD, acBuf, sizeof(acBuf), 0, (void *)&stRemoteAddr, &iRemoteAddrLen))>0)     //实现了循环监听
	{
		// printf("iRecvLen: %d\n", iRecvLen);
		// printf("acBuf:%s\n", acBuf);
        stRemoteAddr.sin_family = AF_INET;
        stRemoteAddr.sin_port = htons(22345);
        stRemoteAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        sendto(send_SocketFD, "hello World!", strlen("hello World!"), 0, (void *)&stRemoteAddr, sizeof(stRemoteAddr));
        // printf("Hello world! \n");
	}
	close(iSocketFD);
    close(send_SocketFD);
 
	return 0;
}