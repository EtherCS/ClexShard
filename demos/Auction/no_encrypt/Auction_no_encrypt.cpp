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
// using namespace std;
const uint USER_LENGTH = 20;
uint FUN_ADDITEM_SIZE = 7;
uint FUN_BID_SIZE = 3;
   
/* state cache */
char beneficiary[USER_LENGTH + 1];   
char item[USER_LENGTH + 1];
char highestBidder[USER_LENGTH + 1];     
uint highestBid;        

void loadState(const char *_path) {
    std::ifstream infp(_path, std::ios::in);
    // char *
    if (!infp.is_open()) {
        printf("Open file failed! \n");
        exit(0);
    }
    infp.read(reinterpret_cast<char *>(beneficiary), sizeof(beneficiary));
    infp.read(reinterpret_cast<char *>(item), sizeof(item));
    infp.read(reinterpret_cast<char *>(highestBidder), sizeof(highestBidder));
    char temp_bid[USER_LENGTH + 1];
    infp.read(reinterpret_cast<char *>(temp_bid), sizeof(temp_bid));
    highestBid = (uint)(atoi(temp_bid));
    // while(!infp.eof()) {
    //     infp.read((char*)(&element), sizeof(element));
    //     // strcpy(uname, element.first);
    //     balance[element.first] = element.second;
    // }
    infp.close();
}

void updateState(const char *_path) {
    std::ofstream outfp(_path, std::ios::out | std::ios::binary);
    if (!outfp.is_open()) {
        printf("Open file failed! \n");
        exit(0);
    }
    outfp.write(reinterpret_cast<char *>(beneficiary), sizeof(beneficiary));
    outfp.write(reinterpret_cast<char *>(item), sizeof(item));
    outfp.write(reinterpret_cast<char *>(highestBidder), sizeof(highestBidder));  // initialize beneficiary as the highestBidder
    outfp.write(reinterpret_cast<char *>(&highestBid), sizeof(highestBid));
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
 
	
	iSocketFD = socket(AF_INET, SOCK_DGRAM, 0);
    send_SocketFD = socket(AF_INET, SOCK_DGRAM, 0);
	if(iSocketFD < 0 || send_SocketFD < 0)
	{
		printf("fail to create socket!\n");
		// return 0;
	}
 
	
	stLocalAddr.sin_family = AF_INET;
	stLocalAddr.sin_port   = htons(10011);
	stLocalAddr.sin_addr.s_addr = 0;
 
	
	if(0 > bind(iSocketFD, (struct sockaddr *)&stLocalAddr, sizeof(stLocalAddr)))
	{
		printf("fail to bind address!\n");
		close(iSocketFD);
		return 0;
	}
    while((iRecvLen = recvfrom(iSocketFD, acBuf, sizeof(acBuf), 0, (struct sockaddr *)&stRemoteAddr, &iRemoteAddrLen))>0)     
	{
        // TODO: Resolve ip and port from user
        stRemoteAddr.sin_family = AF_INET;
        stRemoteAddr.sin_port = htons(10012);
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