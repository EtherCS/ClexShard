#include <iostream>
#include <map>
#include <vector>
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
#include <sys/stat.h>
#include <openssl/des.h>
#include <vector>

const uint CRYPT_LENGTH = 24;
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

void initial() {
	printf("Load into the enclave and initialize successfully! \n");
}

int main(int argc, char **argv)
{
	char *test_type = argv[1];
	printf("instruction is: %s \n", test_type);
	if(strcmp(test_type, "deploy") == 0) {
		initial();
		return 0;
	}
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
	stLocalAddr.sin_port   = htons(10011);
	stLocalAddr.sin_addr.s_addr = 0;
 
	/* 绑定地址 */
	if(0 > bind(iSocketFD, (struct sockaddr *)&stLocalAddr, sizeof(stLocalAddr)))
	{
		printf("绑定地址失败!\n");
		close(iSocketFD);
		return 0;
	}
	// pthread_create(&stPid, NULL, test, NULL);   //实现了多线程
	
	while((iRecvLen = recvfrom(iSocketFD, acBuf, sizeof(acBuf), 0, (struct sockaddr *)&stRemoteAddr, &iRemoteAddrLen))>0)     //实现了循环监听
	{
		// printf("iRecvLen: %d\n", iRecvLen);
		// printf("acBuf:%s\n", acBuf);
		std::string encrpt_text = "hello World!";
		std::string decrypt_text = des_decrypt(encrpt_text,desKey);
        stRemoteAddr.sin_family = AF_INET;
        stRemoteAddr.sin_port = htons(10012);
        stRemoteAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        sendto(send_SocketFD, decrypt_text.c_str(), strlen(decrypt_text.c_str()), 0, (struct sockaddr *)&stRemoteAddr, sizeof(stRemoteAddr));
        // printf("Hello world! \n");
	}
	close(iSocketFD);
    close(send_SocketFD);
 
	return 0;
}