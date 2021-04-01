/*
 * Copyright (C) 2011-2020 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


// App.cpp : Defines the entry point for the console application.
#include <stdio.h>
#include <unistd.h>
#include <map>
#include <sys/types.h>
#include <sys/stat.h>
#include <sched.h>
#include <sys/sysinfo.h>
#include <signal.h>
// #include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "EnclaveExecutor_u.h"
#include "sgx_eid.h"
#include "sgx_urts.h"

#include "fifo_def.h"
#include "datatypes.h"
#include "example.h"

#include "CPTask.h"
#include "CPServer.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>


#define UNUSED(val) (void)(val)
#define TCHAR   char
#define _TCHAR  char
#define _T(str) str
#define scanf_s scanf
#define _tmain  main

CPTask * g_cptask = NULL;
CPServer * g_cpserver = NULL;

struct Rec_para {
	// int tee_num;
	int socketFD;
	struct sockaddr_in stRemoteAddr;
	struct sockaddr_in stLocalAddr;
};

void signal_handler(int sig)
{
    switch(sig)
    {
        case SIGINT:
        case SIGTERM:
        {
            if (g_cpserver)
                g_cpserver->shutDown();
        }
        break;
    default:
        break;
    }

    exit(1);
}

void cleanup()
{
    if(g_cptask != NULL)
        delete g_cptask;
    if(g_cpserver != NULL)
        delete g_cpserver;
}

void* invoke_test(void* args)
{
    sleep(5);
    // 5s后执行合约C2的函数func2
    struct Object obj;
    sgx_status_t status = SGX_SUCCESS;
    printf("Begin to run func2...\n");
    status = ecall_handle(get_enclave_id(), &fun2_tx, &obj);
    if (status != SGX_SUCCESS || strcmp(obj.Status,"success") != 0)
        printf("C2 Ecall failed: ECALL return 0x%x, error message is %s.\n", status, obj.Value);
    else
        printf("Succeed to run func2...\n");

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
    sleep(5);
    struct Object obj;
    sgx_status_t status = SGX_SUCCESS;
	while (1)
	{
		if (iRecvLen = recvfrom(para->socketFD, acBuf, sizeof(acBuf), 0, (struct sockaddr *)&para->stRemoteAddr, &iRemoteAddrLen) > 0) {
			// printf("Receive message from %d\n", para->tee_num);
			// printf("Receive message is: %s \n", acBuf);
            para->stRemoteAddr.sin_family = AF_INET;
            para->stRemoteAddr.sin_port = htons(11012);
            para->stRemoteAddr.sin_addr.s_addr = htonl(INADDR_ANY); // localhost
            // std::string output_data;
            
            // printf("Begin to run func2...\n");
            status = ecall_handle(get_enclave_id(), &fun2_tx, &obj);
            if (status != SGX_SUCCESS || strcmp(obj.Status,"success") != 0) {
                printf("C2 Ecall failed: ECALL return 0x%x, error message is %s.\n", status, obj.Value);
            } 
            else {
                // printf("Succeed to run func2...\n");
            }
            sendto(para->socketFD, "Complex contract is executed successfully", strlen("Complex contract is executed successfully"), 0, (struct sockaddr *)&para->stRemoteAddr, sizeof(para->stRemoteAddr));
            // sendto(para->socketFD, obj.Value, strlen(obj.Value), 0, (struct sockaddr *)&para->stRemoteAddr, sizeof(para->stRemoteAddr));
		}
	}
	
} 

void * startServer(void * arg) {
    g_cpserver->doWork();
    printf("Server is ON...\n");
    printf("Press Ctrl+C to exit...\n");
}
/* run: ./appteeprovider1 1 */
/* run: ./appteeprovider2 2 */
int  main(int argc, char* argv[])
{
    char *tee_num = argv[1];

    // create server instance, it would listen on sockets and proceeds client's requests
    g_cptask = new (std::nothrow) CPTask;
    g_cpserver = new (std::nothrow) CPServer(g_cptask);

    if (!g_cptask || !g_cpserver)
         return -1;

    atexit(cleanup);

    // register signal handler so to respond to user interception
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    g_cptask->start();

    if (g_cpserver->init() != 0)
    {
         printf("fail to init server\n");       
    } 
    else                                           // 改成循环监听（服务器，argv为不同参数）（合约）
    {
        int iRecvLen = 0;
        int iSocketFD = 0;
        int send_SocketFD = 0;
        // int len;
        char acBuf[4096] = {0};
        // struct sockaddr_in stLocalAddr = {0};
    
        struct sockaddr_in stRemoteAddr = {0};
        socklen_t iRemoteAddrLen = 0;
    
        /* 创建socket */
        iSocketFD = socket(AF_INET, SOCK_DGRAM, 0);
        send_SocketFD = socket(AF_INET, SOCK_DGRAM, 0);
        if(iSocketFD < 0 || send_SocketFD < 0)
        {
            printf("创建socket失败!\n");
            // return 0;
        }
        pthread_t m_thread;         
        int rc = pthread_create(&m_thread, NULL, startServer, NULL);

        struct Rec_para rec_para;
        rec_para.socketFD = iSocketFD;
        /* local address */
        std::string str_loc_temp_port = tee_num;
        str_loc_temp_port = "110" + str_loc_temp_port + "1";
        rec_para.stLocalAddr.sin_family = AF_INET;
        rec_para.stLocalAddr.sin_port   = htons((uint16_t)(atoi(str_loc_temp_port.c_str())));
        rec_para.stLocalAddr.sin_addr.s_addr = 0;

        rec_para.stRemoteAddr = stRemoteAddr;

        // para = (struct Rec_para*) arg;
        if (0 > bind(rec_para.socketFD, (struct sockaddr *)&rec_para.stLocalAddr, sizeof(rec_para.stLocalAddr))) {
            printf("Bind address failed!\n");
            close(rec_para.socketFD);
            exit(0);
        }
        sleep(5);
        struct Object obj;
        sgx_status_t status = SGX_SUCCESS;
        while (1)
        {
            if (iRecvLen = recvfrom(rec_para.socketFD, acBuf, sizeof(acBuf), 0, (struct sockaddr *)&rec_para.stRemoteAddr, &iRemoteAddrLen) > 0) {
                // printf("Receive message from %d\n", rec_para.tee_num);
                // printf("Receive message is: %s \n", acBuf);
                rec_para.stRemoteAddr.sin_family = AF_INET;
                rec_para.stRemoteAddr.sin_port = htons(11012);
                rec_para.stRemoteAddr.sin_addr.s_addr = htonl(INADDR_ANY); // localhost
                // std::string output_data;
                
                // printf("Begin to run func2...\n");
                status = ecall_handle(get_enclave_id(), &fun2_tx, &obj);
                if (status != SGX_SUCCESS || strcmp(obj.Status,"success") != 0) {
                    printf("C2 Ecall failed: ECALL return 0x%x, error message is %s.\n", status, obj.Value);
                    sendto(rec_para.socketFD, "Failed", strlen("Failed"), 0, (struct sockaddr *)&rec_para.stRemoteAddr, sizeof(rec_para.stRemoteAddr));
                } 
                else {
                    sendto(rec_para.socketFD, "Complex contract is executed successfully", strlen("Complex contract is executed successfully"), 0, (struct sockaddr *)&rec_para.stRemoteAddr, sizeof(rec_para.stRemoteAddr));
                    // printf("Succeed to run func2...\n");
                }
                // sendto(rec_para.socketFD, "Complex contract is executed successfully", strlen("Complex contract is executed successfully"), 0, (struct sockaddr *)&rec_para.stRemoteAddr, sizeof(rec_para.stRemoteAddr));
                // sendto(para->socketFD, obj.Value, strlen(obj.Value), 0, (struct sockaddr *)&para->stRemoteAddr, sizeof(para->stRemoteAddr));
            }
        }

        
        // printf("Server is ON...\n");
        // printf("Press Ctrl+C to exit...\n");
        // g_cpserver->doWork();
    }
    return 0;
}
