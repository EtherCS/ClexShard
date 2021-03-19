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

int  main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

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
    }else
    {
        pthread_t m_thread;
        int rc = pthread_create(&m_thread, NULL, invoke_test, NULL);
        printf("Server is ON...\n");
        printf("Press Ctrl+C to exit...\n");
        g_cpserver->doWork();
    }
    
    return 0;
}
