#include <iostream>
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
uint FUN_ADDITEM_SIZE = 7;
uint FUN_BID_SIZE = 3;
   
/* state cache */
char beneficiary[USER_LENGTH + 1];   // 受益人
char item[USER_LENGTH + 1];
char highestBidder[USER_LENGTH + 1];     // 最高竞价者
uint highestBid;        // 最高竞价

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
    std::ifstream infp(_path, std::ios::in);
    // char *
    if (!infp.is_open()) {
        printf("Open file failed! \n");
        exit(0);
    }
    char read_temp[CRYPT_LENGTH];
    std::string temp_decrpyt_text;
    std::string temp_encrpyt_text;
    /* Resolve beneficiary*/
    infp.read(reinterpret_cast<char *>(read_temp), sizeof(read_temp));
    temp_encrpyt_text = read_temp;
    printf("(char)Encrypt beneficiary is: %s \n", read_temp);
    printf("(str)Encrypt beneficiary is: %s \n", temp_encrpyt_text.c_str());
    temp_decrpyt_text = des_decrypt(temp_encrpyt_text, desKey);
    printf("(str)Decrypt beneficiary is: %s \n", temp_decrpyt_text.c_str());
    strncpy(beneficiary, temp_decrpyt_text.c_str(), USER_LENGTH);
    printf("(char[])Decrypt beneficiary is: %s \n", beneficiary);

    /* Resolve item*/
    infp.read(reinterpret_cast<char *>(read_temp), sizeof(read_temp));
    temp_encrpyt_text = read_temp;
    printf("(char)Encrypt item is: %s \n", read_temp);
    printf("(str)Encrypt item is: %s \n", temp_encrpyt_text.c_str());
    temp_decrpyt_text = des_decrypt(temp_encrpyt_text, desKey);
    printf("(str)Decrypt item is: %s \n", temp_decrpyt_text.c_str());
    strncpy(item, temp_decrpyt_text.c_str(), USER_LENGTH);
    printf("(char[])Decrypt item is: %s \n", item);

    /* Resolve highestBidder*/
    infp.read(reinterpret_cast<char *>(read_temp), sizeof(read_temp));
    temp_encrpyt_text = read_temp;
    printf("(char)Encrypt highestBidder is: %s \n", read_temp);
    printf("(str)Encrypt highestBidder is: %s \n", temp_encrpyt_text.c_str());
    temp_decrpyt_text = des_decrypt(temp_encrpyt_text, desKey);
    printf("(str)Decrypt highestBidder is: %s \n", temp_decrpyt_text.c_str());
    strncpy(highestBidder, temp_decrpyt_text.c_str(), USER_LENGTH);
    printf("(char[])Decrypt highestBidder is: %s \n", highestBidder);

    /* Resolve highestBid*/
    infp.read(reinterpret_cast<char *>(read_temp), sizeof(read_temp));
    temp_encrpyt_text = read_temp;
    printf("(char)Encrypt highestBid is: %s \n", read_temp);
    printf("(str)Encrypt highestBid is: %s \n", temp_encrpyt_text.c_str());
    temp_decrpyt_text = des_decrypt(temp_encrpyt_text, desKey);
    printf("(str)Decrypt highestBid is: %s \n", temp_decrpyt_text.c_str());
    highestBid = (uint)(atoi(temp_decrpyt_text.c_str()));
    printf("(int)Decrypt highestBid is: %lu \n", highestBid);
    // infp.read(reinterpret_cast<char *>(beneficiary), sizeof(beneficiary));
    // infp.read(reinterpret_cast<char *>(item), sizeof(item));
    // infp.read(reinterpret_cast<char *>(highestBidder), sizeof(highestBidder));
    // char temp_bid[USER_LENGTH + 1];
    // infp.read(reinterpret_cast<char *>(temp_bid), sizeof(temp_bid));
    // highestBid = (uint)(atoi(temp_bid));
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
    /* encrypt beneficiary */
    temp_decrpyt_text = beneficiary;
    printf("(char[])Before encrypt, beneficiary is: %s", beneficiary);
    printf("(str)Before encrypt, beneficiary is: %s", temp_decrpyt_text.c_str());
    temp_encrpyt_text = des_encrypt(temp_decrpyt_text, desKey);
    printf("(str)After encrypt, beneficiary is: %s", temp_encrpyt_text.c_str());
    outfp.write(reinterpret_cast<char *>((char *)temp_encrpyt_text.c_str()), sizeof(temp_encrpyt_text.c_str()));

    /* encrypt item */
    temp_decrpyt_text = item;
    printf("(char[])Before encrypt, item is: %s", item);
    printf("(str)Before encrypt, item is: %s", temp_decrpyt_text.c_str());
    temp_encrpyt_text = des_encrypt(temp_decrpyt_text, desKey);
    printf("(str)After encrypt, item is: %s", temp_encrpyt_text.c_str());
    outfp.write(reinterpret_cast<char *>((char *)temp_encrpyt_text.c_str()), sizeof(temp_encrpyt_text.c_str()));

    /* encrypt highestBidder */
    temp_decrpyt_text = highestBidder;
    printf("(char[])Before encrypt, highestBidder is: %s", highestBidder);
    printf("(str)Before encrypt, highestBidder is: %s", temp_decrpyt_text.c_str());
    temp_encrpyt_text = des_encrypt(temp_decrpyt_text, desKey);
    printf("(str)After encrypt, highestBidder is: %s", temp_encrpyt_text.c_str());
    outfp.write(reinterpret_cast<char *>((char *)temp_encrpyt_text.c_str()), sizeof(temp_encrpyt_text.c_str()));  // initialize beneficiary as the highestBidder

    /* encrypt highestBid */
    temp_decrpyt_text = std::to_string(highestBid);
    printf("(char[])Before encrypt, highestBid is: %lu", highestBid);
    printf("(str)Before encrypt, highestBid is: %s", temp_decrpyt_text.c_str());
    temp_encrpyt_text = des_encrypt(temp_decrpyt_text, desKey);
    printf("(str)After encrypt, highestBid is: %s", temp_encrpyt_text.c_str());
    outfp.write(reinterpret_cast<char *>((char *)temp_encrpyt_text.c_str()), sizeof(temp_encrpyt_text.c_str()));
    outfp.close();
}

bool addItem(const char *_path, const char *_beneficiary, const char *_newItem) {
    // char beneficiary[USER_LENGTH + 1];
    // uint initial_bid = 0;
    strncpy(beneficiary, _beneficiary, USER_LENGTH);
    strncpy(item, _newItem, USER_LENGTH + 1);
    strncpy(highestBidder, beneficiary, USER_LENGTH);
    highestBid = 0;
    updateState(_path);
    return true;
}

bool bid(const char *_path, const char *_mode, const char *_bidder, const uint _bidPrice) {
    loadState(_path);
    if (strcmp(_mode, "test")) {
        strncpy(highestBidder, _bidder, USER_LENGTH);
        highestBid = _bidPrice;
        updateState(_path);
        return true;
    }
    else {
        if (highestBid < _bidPrice) {
            strncpy(highestBidder, _bidder, USER_LENGTH);
            highestBid = _bidPrice;
            updateState(_path);
            return true;
        }
        else {
            return false;
        }
    }
    
}

void parseAddItem(std::string input_data, std::string &_beneficiary, std::string &_newItem) {
    _beneficiary = input_data.substr(FUN_ADDITEM_SIZE + 1, USER_LENGTH);
    _newItem = input_data.substr(FUN_ADDITEM_SIZE + USER_LENGTH + 2, USER_LENGTH);
}

void parseBid(std::string input_data, std::string &_bidder, uint &_bidPrice) {
    _bidder = input_data.substr(FUN_BID_SIZE + 1, USER_LENGTH);
    std::string str_value = input_data.substr(FUN_BID_SIZE + USER_LENGTH + 2);
    _bidPrice = (uint)(atoi(str_value.c_str()));
}

// mode = test or non-test
/* run: ./Auction ./state test */
int main(int argc, char *argv[]) {
    char *file_path, *function, *mode;
    file_path = argv[1];
    // function = argv[2];
    mode = argv[2];
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
	stLocalAddr.sin_port   = htons(10001);
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
        stRemoteAddr.sin_port = htons(10002);
        stRemoteAddr.sin_addr.s_addr = htonl(INADDR_ANY); // localhost
        std::string input_data = acBuf;
        /* addItem Function: addItem 12345678901234567890 aaaaaaaaaaaaaaaaaaaa*/
        if (input_data.find("addItem") != input_data.npos) {      
            std::string _beneficiary, _newItem;
            parseAddItem(input_data, _beneficiary, _newItem);
            // parseCreate(input_data, inName, inCoins);
            if (addItem(file_path, _beneficiary.c_str(), _newItem.c_str())) {
                sendto(send_SocketFD, "Release a new item successfully!", strlen("Release a new item successfully!"), 0, (struct sockaddr *)&stRemoteAddr, sizeof(stRemoteAddr));
            }
            else {
                sendto(send_SocketFD, "Failed to release a new item!", strlen("Failed to release a new item!"), 0, (struct sockaddr *)&stRemoteAddr, sizeof(stRemoteAddr));
            }
        }
        /* bid Function: bid 01234567890123456789 100*/
        else if (input_data.find("bid") != input_data.npos) {       
            std::string _bidder;
            uint _bidPrice;
            parseBid(input_data, _bidder, _bidPrice);
            if (bid(file_path, mode, _bidder.c_str(), _bidPrice)) {
                sendto(send_SocketFD, "Congratulation!", strlen("Congratulation!"), 0, (struct sockaddr *)&stRemoteAddr, sizeof(stRemoteAddr));
            }
            else {
                sendto(send_SocketFD, "Failed!", strlen("Failed!"), 0, (struct sockaddr *)&stRemoteAddr, sizeof(stRemoteAddr));
            }
        }
        else {
            printf("error call! \n");
        }
        // sendto(send_SocketFD, "hello World!", strlen("hello World!"), 0, (struct sockaddr *)&stRemoteAddr, sizeof(stRemoteAddr));

	}
	close(iSocketFD);
    close(send_SocketFD);
}