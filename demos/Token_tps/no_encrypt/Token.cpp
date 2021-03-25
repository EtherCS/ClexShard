#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
// using namespace std;

const uint USER_LENGTH = 20;
uint FUN_CREATE_SIZE = 8;
uint FUN_BALANCE_SIZE = 10;
uint FUN_TRANSFER_SIZE = 8;

std::map<std::string, uint> balance;    // state cache


void loadState(const char *_path) {
    balance.clear();
    // char uname[USER_LENGTH + 1];
    // uint ubalance;
    std::pair<char[USER_LENGTH + 1], uint> element;
    std::ifstream infp(_path, std::ios::in | std::ios::binary);
    if (!infp.is_open()) {
        printf("Open file failed! \n");
        exit(0);
    }
    while(infp.peek() != EOF) {
        infp.read(reinterpret_cast<char *>(&element), sizeof(element));
        // strcpy(uname, element.first);
        balance[element.first] = element.second;
    }
    infp.close();
}

void updateState(const char *_path) {
    std::ofstream outfp(_path, std::ios::out | std::ios::binary);
    if (!outfp.is_open()) {
        printf("Open file failed! \n");
        exit(0);
    }
    std::pair<char[USER_LENGTH + 1], uint> element;
    std::map<std::string, uint>::reverse_iterator balIter;
    for (balIter = balance.rbegin(); balIter != balance.rend(); balIter++) {
        const char *name = balIter->first.c_str();
        strncpy(element.first, name, USER_LENGTH);
        element.second = balIter->second;
        outfp.write(reinterpret_cast<char *>(&element), sizeof(element));
        // delete name;
    }
    outfp.close();
}

bool createAc(const char *_path, const char *_mode, const char* _name, const uint _coins) {
    loadState(_path);
    char name[USER_LENGTH + 1];
    strncpy(name, _name, USER_LENGTH + 1);
    if (strcmp(_mode, "test") == 0 || balance.count(name) <= 0) {  // test mode: no consider repeated accounts. or non-test and new account
        std::ofstream outfp(_path, std::ios::app | std::ios::binary);
        if (!outfp.is_open()) {
            printf("Open file failed! \n");
            return false;
        }
        std::pair<char[USER_LENGTH+1], uint> newElement;
        strcpy(newElement.first, name);
        newElement.second = _coins;
        balance[name] = _coins;     // update cache
        // outfp.write(reinterpret_cast<char *>(&newElement), sizeof(newElement));  // remain the number of accounts
        outfp.close();
        return true;
    }
    else {      // account has existed
        printf("Account %s has existed! \n", name);
        return false;
    }
}

void parseCreate(std::string input_data, std::string &_name, uint &_coins) {
    _name = input_data.substr(FUN_CREATE_SIZE + 1, USER_LENGTH);
    std::string str_coins = input_data.substr(FUN_CREATE_SIZE + USER_LENGTH + 2);
    _coins = (uint)(atoi(str_coins.c_str()));
}

uint getBalance(const char *_path, const char* _name) {
    char name[USER_LENGTH + 1];
    strncpy(name, _name, USER_LENGTH + 1);
    loadState(_path);
    if (balance.count(name) > 0) {
        return balance[name];
    }
    else {
        return 0;
    }
}

void parseBalance(std::string input_data, std::string &_name) {
    _name = input_data.substr(FUN_BALANCE_SIZE + 1, USER_LENGTH);
    // strcpy(_name, input_data.substr(FUN_BALANCE_SIZE + 1, USER_LENGTH).c_str());
}

bool transfer(const char *_path, const char *_from, const char *_to, const uint _value) {
    char from[USER_LENGTH+1], to[USER_LENGTH+1];
    strncpy(from, _from, USER_LENGTH+1);
    strncpy(to, _to, USER_LENGTH+1);
    loadState(_path);
    if (balance.count(from) > 0 && balance.count(to) > 0) {
        if (balance[from] >= _value) {
            balance[from] -= _value;
            balance[to] += _value;
            // TODO: how to write efficiently?
            updateState(_path);
            return true;
        }
    }
    return false;
}

void parseTransfer(std::string input_data, std::string &_from, std::string &_to, uint &_value) {
    _from = input_data.substr(FUN_TRANSFER_SIZE + 1, USER_LENGTH);
    _to = input_data.substr(FUN_TRANSFER_SIZE + USER_LENGTH + 2, USER_LENGTH);
    std::string str_value = input_data.substr(FUN_TRANSFER_SIZE + USER_LENGTH + USER_LENGTH + 3);
    _value = (uint)(atoi(str_value.c_str()));
}

// mode = test or non-test
/* run: ./Token ./state test */ 
int main(int argc, char *argv[]) {
    char *file_path, *function, *mode;
    file_path = argv[1];
    // function = argv[2];
    mode = argv[2];
    printf("path is: %s \n", file_path);
    printf("mode is: %s \n", mode);
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
		// return 0;
	}
 
	/* 填写地址 */
	stLocalAddr.sin_family = AF_INET;
	stLocalAddr.sin_port   = htons(10011);
	stLocalAddr.sin_addr.s_addr = 0;
 
	/* 绑定地址 */
	if(0 > bind(iSocketFD, (struct sockaddr *)&stLocalAddr, sizeof(stLocalAddr)))
	{
		printf("绑定地址失败!\n");
		close(iSocketFD);
		return 0;
	}
    while((iRecvLen = recvfrom(iSocketFD, acBuf, sizeof(acBuf), 0, (struct sockaddr *)&stRemoteAddr, &iRemoteAddrLen))>0)     //实现了循环监听
	{
        // TODO: Resolve ip and port from user
        stRemoteAddr.sin_family = AF_INET;
        stRemoteAddr.sin_port = htons(10012);
        stRemoteAddr.sin_addr.s_addr = htonl(INADDR_ANY); // localhost
        // printf("Received data is: %s \n", acBuf);
        std::string input_data = acBuf;
        /* Create Function: createAc 12345678901234567890 1000 */
        if (input_data.find("createAc") != input_data.npos) {       // create account function
            // printf("Call createAc function, input is: %s \n", input_data.c_str());
            std::string inName;
            uint inCoins;
            parseCreate(input_data, inName, inCoins);
            // printf("After parse, the name is: %s \n", inName.c_str());
            // printf("After parse, the coin is: %u \n", inCoins);
            if (createAc(file_path, mode, inName.c_str(), inCoins)) {
                sendto(send_SocketFD, "Create a new account successfully!", strlen("Create a new account successfully!"), 0, (struct sockaddr *)&stRemoteAddr, sizeof(stRemoteAddr));
            }
            else {
                sendto(send_SocketFD, "Failed to create a new account!", strlen("Failed to create a new account!"), 0, (struct sockaddr *)&stRemoteAddr, sizeof(stRemoteAddr));
            }
            // delete inName;
        }
        /* Balance Function: getBalance 12345678901234567890 */
        else if (input_data.find("getBalance") != input_data.npos) {        // getbalance function
            std::string inName;
            parseBalance(input_data, inName);
            // printf("After parse, the name is: %s \n", inName.c_str());
            uint tempBalance = getBalance(file_path, inName.c_str());
            char *outMsg = (char*)(std::to_string(tempBalance).c_str());
            // strcpy(outMsg, std::to_string(tempBalance).c_str());
            sendto(send_SocketFD, outMsg, strlen(outMsg), 0, (struct sockaddr *)&stRemoteAddr, sizeof(stRemoteAddr));
        }
        /* Transfer Function: transfer 12345678901234567890 12345678901234567890 0*/
        else if (input_data.find("transfer") != input_data.npos) {      // transfer function
            std::string _from, _to;
            uint _value;
            parseTransfer(input_data, _from, _to, _value);
            if (transfer(file_path, _from.c_str(), _to.c_str(), _value)) {
                sendto(send_SocketFD, "Transfer successfully!", strlen("Transfer successfully!"), 0, (struct sockaddr *)&stRemoteAddr, sizeof(stRemoteAddr));
            }
            else {
                sendto(send_SocketFD, "Failed to transfer!", strlen("Failed to transfer!"), 0, (struct sockaddr *)&stRemoteAddr, sizeof(stRemoteAddr));
            }
        }
        else {
            printf("error call! \n");
        }
        memset(acBuf, 0, sizeof(acBuf));
        // printf("After clear, the buffer is: %s \n", acBuf);
        // sendto(send_SocketFD, "hello World!", strlen("hello World!"), 0, (struct sockaddr *)&stRemoteAddr, sizeof(stRemoteAddr));

	}
	close(iSocketFD);
    close(send_SocketFD);
}