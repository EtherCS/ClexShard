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
#include <openssl/des.h>
#include <vector>
// using namespace std;

const uint USER_LENGTH = 20;
const uint CRYPT_LENGTH = 24;
uint FUN_CREATE_SIZE = 8;
uint FUN_BALANCE_SIZE = 10;
uint FUN_TRANSFER_SIZE = 8;

std::map<std::string, uint> balance;    // state cache

std::string desKey = "12345";

std::string des_encrypt(const std::string &clearText, const std::string &key)
{
	std::string cipherText; 
 
	DES_cblock keyEncrypt;
	memset(keyEncrypt, 0, 8);
 
	if (key.length() <= 8)
		memcpy(keyEncrypt, key.c_str(), key.length());
	else
		memcpy(keyEncrypt, key.c_str(), 8);
  
	DES_key_schedule keySchedule;
	DES_set_key_unchecked(&keyEncrypt, &keySchedule);
 
	const_DES_cblock inputText;
	DES_cblock outputText;
	std::vector<unsigned char> vecCiphertext;
	unsigned char tmp[8];
 
	for (int i = 0; i < clearText.length() / 8; i++)
	{
		memcpy(inputText, clearText.c_str() + i * 8, 8);
		DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_ENCRYPT);
		memcpy(tmp, outputText, 8);
 
		for (int j = 0; j < 8; j++)
			vecCiphertext.push_back(tmp[j]);
	}
 
	if (clearText.length() % 8 != 0)
	{
		int tmp1 = clearText.length() / 8 * 8;
		int tmp2 = clearText.length() - tmp1;
		memset(inputText, 0, 8);
		memcpy(inputText, clearText.c_str() + tmp1, tmp2); 
		DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_ENCRYPT);
		memcpy(tmp, outputText, 8);
 
		for (int j = 0; j < 8; j++)
			vecCiphertext.push_back(tmp[j]);
	}
 
	cipherText.clear();
	cipherText.assign(vecCiphertext.begin(), vecCiphertext.end());
 
	return cipherText;
}
 
std::string des_decrypt(const std::string &cipherText, const std::string &key)
{
	std::string clearText; 
 
	DES_cblock keyEncrypt;
	memset(keyEncrypt, 0, 8);
 
	if (key.length() <= 8)
		memcpy(keyEncrypt, key.c_str(), key.length());
	else
		memcpy(keyEncrypt, key.c_str(), 8);
 
	DES_key_schedule keySchedule;
	DES_set_key_unchecked(&keyEncrypt, &keySchedule);
 
	const_DES_cblock inputText;
	DES_cblock outputText;
	std::vector<unsigned char> vecCleartext;
	unsigned char tmp[8];
 
	for (int i = 0; i < cipherText.length() / 8; i++)
	{
		memcpy(inputText, cipherText.c_str() + i * 8, 8);
		DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_DECRYPT);
		memcpy(tmp, outputText, 8);
 
		for (int j = 0; j < 8; j++)
			vecCleartext.push_back(tmp[j]);
	}
 
	if (cipherText.length() % 8 != 0)
	{
		int tmp1 = cipherText.length() / 8 * 8;
		int tmp2 = cipherText.length() - tmp1;
		memset(inputText, 0, 8);
		memcpy(inputText, cipherText.c_str() + tmp1, tmp2);
		DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_DECRYPT);
		memcpy(tmp, outputText, 8);
 
		for (int j = 0; j < 8; j++)
			vecCleartext.push_back(tmp[j]);
	}
 
	clearText.clear();
	clearText.assign(vecCleartext.begin(), vecCleartext.end());
 
	return clearText;
}

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
    // char read_temp[CRYPT_LENGTH];
    char read_map_temp[CRYPT_LENGTH+CRYPT_LENGTH/3];
    std::string temp_decrpyt_text;
    std::string temp_encrpyt_text;
    std::pair<char[USER_LENGTH + 1], uint> tempBalance;
    while(infp.peek() != EOF) {
        infp.read(reinterpret_cast<char *>(read_map_temp), sizeof(read_map_temp));
        std::string temp_enc_user(read_map_temp, read_map_temp + CRYPT_LENGTH);
        std::string temp_enc_balance(read_map_temp + CRYPT_LENGTH, strlen(read_map_temp));
        std::string temp_dec_user = des_decrypt(temp_enc_user, desKey);
        strncpy(tempBalance.first, temp_dec_user.c_str(), USER_LENGTH);
        std::string temp_dec_balance = des_decrypt(temp_enc_balance, desKey);
        tempBalance.second = (uint)(atoi(temp_dec_balance.c_str()));
        balance[tempBalance.first] = tempBalance.second;
        // printf("Load decrypt name is: %s \n", tempBalance.first);
        // printf("Load decrypt balance is: %d \n", tempBalance.second);
    }
    infp.close();
}

void updateState(const char *_path) {
    std::ofstream outfp(_path, std::ios::out | std::ios::binary);
    if (!outfp.is_open()) {
        printf("Open file failed! \n");
        exit(0);
    }
    std::string temp_decrpyt_text;
    std::string temp_encrpyt_text;
    std::string temp_map_text;
    std::pair<char[USER_LENGTH + 1], uint> element;
    std::map<std::string, uint>::reverse_iterator balIter;
    // printf("Update balance element num is: %d\n", balance.size());
    for (balIter = balance.rbegin(); balIter != balance.rend(); balIter++) {
        temp_map_text = "";
        temp_decrpyt_text = balIter->first;
        // printf("(char)update decrypt name is: %s \n", balIter->first);
        // printf("(str)update decrypt name is: %s \n", temp_decrpyt_text.c_str());
        temp_encrpyt_text = des_encrypt(temp_decrpyt_text, desKey);
        temp_map_text += temp_encrpyt_text;
        temp_decrpyt_text = std::to_string(balIter->second);
        // printf("(char)update decrypt balance is: %d \n", balIter->second);
        // printf("(str)update decrypt balance is: %s \n", temp_decrpyt_text.c_str());
        temp_encrpyt_text = des_encrypt(temp_decrpyt_text, desKey);
        temp_map_text += temp_encrpyt_text;
        // printf("After encrypt, nominator map size is: %d\n", strlen(temp_map_text.c_str()));
        outfp.write(reinterpret_cast<char *>((char *)temp_map_text.c_str()), CRYPT_LENGTH + CRYPT_LENGTH/3);
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

void initial() {
	printf("Load into the enclave and initialize successfully! \n");
}

// mode = test or non-test
/* run: ./Token ./state test deploy */ 
int main(int argc, char *argv[]) {
    char *file_path, *mode, *test_type;
    file_path = argv[1];
    // function = argv[2];
    mode = argv[2];
    test_type = argv[3];
    if(strcmp(test_type, "deploy") == 0) {
		initial();
		return 0;
	}
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