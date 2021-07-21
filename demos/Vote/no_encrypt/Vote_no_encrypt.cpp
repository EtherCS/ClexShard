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
// using namespace std;
const uint USER_LENGTH = 20;
uint FUN_NOMINATE_SIZE = 8;
uint FUN_VOTE_SIZE = 4;

/* state cache */
char manager[USER_LENGTH + 1];   // contract creator
uint numNominators;
std::map<std::string, uint> nominators;    // nominator name -> received vote number
std::map<std::string, uint> voters;     // voter name -> vote number

void loadState(const char *_path) {
    // printf("nominators size is: %d \n", nominators.size());
    // printf("voters size is: %d \n", voters.size());
    nominators.clear();
    voters.clear();
    std::pair<char[USER_LENGTH + 1], uint> tempNominators;
    std::pair<char[USER_LENGTH + 1], uint> tempVoters;
    std::ifstream infp(_path, std::ios::in | std::ios::binary);
    if (!infp.is_open()) {
        printf("Open file failed! \n");
        exit(0);
    }
    infp.read(reinterpret_cast<char *>(manager), sizeof(manager));
    uint temp_bid;
    infp.read(reinterpret_cast<char *>(&temp_bid), sizeof(temp_bid));
    numNominators = temp_bid;
    for (int i = 0; i < numNominators; i++) {       // get all nominators
        infp.read(reinterpret_cast<char *>(&tempNominators), sizeof(tempNominators));
        nominators[tempNominators.first] = tempNominators.second;
    }
    while(infp.peek() != EOF) {        // get all voters
        infp.read((char*)(&tempVoters), sizeof(tempVoters));
        // strcpy(uname, element.first);
        voters[tempVoters.first] = tempVoters.second;
    }
    infp.close();
}

void updateState(const char *_path) {
    std::ofstream outfp(_path, std::ios::out | std::ios::binary);
    if (!outfp.is_open()) {
        printf("Open file failed! \n");
        exit(0);
    }
    outfp.write(reinterpret_cast<char *>(manager), sizeof(manager));
    outfp.write(reinterpret_cast<char *>(&numNominators), sizeof(numNominators));
    std::map<std::string, uint>::reverse_iterator nomIter;
    for (nomIter = nominators.rbegin(); nomIter != nominators.rend(); nomIter++) {
        std::pair<char[USER_LENGTH + 1], uint> tempNominator;
        const char *temp_nominator = nomIter->first.c_str();
        strncpy(tempNominator.first, temp_nominator, USER_LENGTH);
        tempNominator.second = nomIter->second;
        outfp.write(reinterpret_cast<char *>(&tempNominator), sizeof(tempNominator));
        // delete temp_nominator;
    }
    std::map<std::string, uint>::reverse_iterator voterIter;
    for (voterIter = voters.rbegin(); voterIter != voters.rend(); voterIter++) {
        std::pair<char[USER_LENGTH + 1], uint> tempVoter;
        const char *temp_voter = voterIter->first.c_str();
        strncpy(tempVoter.first, temp_voter, USER_LENGTH);
        tempVoter.second = voterIter->second;
        outfp.write(reinterpret_cast<char *>(&tempVoter), sizeof(tempVoter));
        // delete temp_voter;
    }
    outfp.close();
}

bool initial(const char *_path, const char *_manager) {
    struct stat buffer;
    if (stat (_path, &buffer) == 0) {   // file has existed
        return true;
    }
    std::ofstream outfp(_path, std::ios::out | std::ios::binary);
    char temp_manager[USER_LENGTH + 1];
    strncpy(temp_manager, _manager, USER_LENGTH + 1);
    if (!outfp.is_open()) {
        printf("Open file failed! \n");
        return false;
    }
    uint temp_num = 0;
    outfp.write(reinterpret_cast<char *>(temp_manager), sizeof(temp_manager));
    outfp.write(reinterpret_cast<char *>(&temp_num), sizeof(temp_num));
    outfp.close();
    // printf("The creator is: %s \n", temp_manager);
    return true;
}

bool nominate(const char *_path, const char *_mode, const char *_caller, std::vector<std::string> &_nominators, std::vector<std::string> &_voters) {
    // char beneficiary[USER_LENGTH + 1];
    loadState(_path);
    // printf("The creator is: %s \n", manager);
    // printf("The caller is: %s \n", _caller);
    if (strcmp(manager, _caller) == 0) {
        // std::ofstream outfp(_path, std::ios::out | std::ios::binary);
        // if (!outfp.is_open()) {
        //     printf("Open file failed! \n");
        //     return false;
        // }
        // outfp.write(reinterpret_cast<char *>(manager), sizeof(manager));
        numNominators = (uint)(_nominators.size());
        // outfp.write(reinterpret_cast<char *>(numNominators), sizeof(numNominators));
        // printf("mode is: %s \n", _mode);
        if (strcmp(_mode, "test") == 0 ) {
            nominators.clear();
            voters.clear();
        }
        for (int i = 0; i < _nominators.size(); i++) {
            char tempNominator[USER_LENGTH + 1];
            for (int j = 0; j < _nominators[i].length(); j++) {
                tempNominator[j] = _nominators[i][j];
            }
            nominators[tempNominator] = 1;
            // std::pair<char*, uint> tempNominators(tempNominator, 1);
            // outfp.write(reinterpret_cast<char *>(&tempNominators), sizeof(tempNominators));
        }
        for (int i = 0; i < _voters.size(); i++) {
            char tempVoter[USER_LENGTH + 1];
            for (int j = 0; j < _voters[i].length(); j++) {
                tempVoter[j] = _voters[i][j];
            }
            voters[tempVoter] = 1;
            // std::pair<char*, uint> tempVoters(tempVoter, 1);
            // outfp.write(reinterpret_cast<char *>(&tempVoters), sizeof(tempVoters));
        }
        // outfp.close();
        updateState(_path);
        return true;
    }
    else {
        return false;
    }
}

bool vote(const char *_path, const char *_mode, const char *_voter, const char *_nominator) {
    loadState(_path);
    char tempVoter[USER_LENGTH + 1], tempNominator[USER_LENGTH + 1];
    strncpy(tempVoter, _voter, USER_LENGTH + 1);
    strncpy(tempNominator, _nominator, USER_LENGTH + 1);
    if (voters[tempVoter] > 0) {
        nominators[tempNominator] += 1;
        if (strcmp(_mode, "test") != 0) { // non-test
            voters[tempVoter] -= 1;
        }
        // test mode, no need voters[_voter]--
        updateState(_path);
        return true;
    }
    else {
        return false;
    }
}

// input data format: nominate(func) abcdeabcdeabcdeabcde(caller) 1(numberOfNom) abcdeabcdeabcdeabcde(Nom) abcdeabcdeabcdeabcde(Voter) 
void parseNominate(std::string input_data, std::string &_caller, std::vector<std::string> &_nominators, std::vector<std::string> &_voters) {
    // const char *temp_caller = input_data.substr(FUN_NOMINATE_SIZE + 1, USER_LENGTH).c_str();
    _nominators.clear();
    _voters.clear();
    _caller = input_data.substr(FUN_NOMINATE_SIZE + 1, USER_LENGTH);
    // strncpy(_caller, temp_caller, USER_LENGTH + 1);
    uint numNominators;
    int index_1 = input_data.find(" ", FUN_NOMINATE_SIZE + 1);
    int index_2 = input_data.find(" ", index_1 + 1);
    std::string str_temp = input_data.substr(index_1 + 1, index_2 - index_1);
    numNominators = (uint)(atoi(str_temp.c_str()));
    for (int i = 0; i < numNominators; i++) {
        // index_1 = input_data.find(" ", index_2 + 1);
        index_1 = index_2;
        index_2 = input_data.find(" ", index_1 + 1);
        str_temp = input_data.substr(index_1 + 1, index_2 - index_1);
        // printf("The %dth nominate is: %s \n", i, str_temp.c_str());
        _nominators.push_back(str_temp);
    }
    while (input_data.find(" ", index_2 + 1) != input_data.npos) {
        index_1 = index_2;
        index_2 = input_data.find(" ", index_1 + 1);
        str_temp = input_data.substr(index_1 + 1, index_2 - index_1);
        // printf("The voter is: %s \n", str_temp.c_str());
        _voters.push_back(str_temp);
    }
    str_temp = input_data.substr(index_2 + 1);
    // printf("The voter is: %s \n", str_temp.c_str());
    _voters.push_back(str_temp);
}

void parseVote(std::string input_data, std::string &_voter, std::string &_nominator) {
    _voter = input_data.substr(FUN_VOTE_SIZE + 1, USER_LENGTH);
    _nominator = input_data.substr(FUN_VOTE_SIZE + USER_LENGTH + 2, USER_LENGTH);
}
// mode = test or non-test
/* run: ./Vote ./state test 12345678901234567890 */
int main(int argc, char *argv[]) {
    char *file_path, *mode, *creator;
    file_path = argv[1];
    // function = argv[2];
    mode = argv[2];
    creator = argv[3];
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
    if(!initial(file_path, creator)) {
        exit(0);
    }
    printf("Initialize successfully! \n");
    std::vector<std::string> _nominators, _voters;
    while((iRecvLen = recvfrom(iSocketFD, acBuf, sizeof(acBuf), 0, (struct sockaddr *)&stRemoteAddr, &iRemoteAddrLen))>0)     
	{
        // TODO: Resolve ip and port from user
        stRemoteAddr.sin_family = AF_INET;
        stRemoteAddr.sin_port = htons(10012);
        stRemoteAddr.sin_addr.s_addr = htonl(INADDR_ANY); // localhost
        std::string input_data = acBuf;
        /* Nominate Function: nominate 12345678901234567890 2 12345678901234567890 01234567890123456789 12345678901234567890 01234567890123456789*/
        if (input_data.find("nominate") != input_data.npos) {     
            std::string _caller;
            parseNominate(input_data, _caller, _nominators, _voters);
            // parseCreate(input_data, inName, inCoins);
            if (nominate(file_path, mode, _caller.c_str(), _nominators, _voters)) {
                sendto(send_SocketFD, "successfully!", strlen("successfully!"), 0, (struct sockaddr *)&stRemoteAddr, sizeof(stRemoteAddr));
                // sendto(send_SocketFD, "Create a new account successfully!", strlen("Create a new account successfully!"), 0, (struct sockaddr *)&stRemoteAddr, sizeof(stRemoteAddr));
            }
            else {
                sendto(send_SocketFD, "Failed!", strlen("Failed!"), 0, (struct sockaddr *)&stRemoteAddr, sizeof(stRemoteAddr));
            }
        }
        /* Vote Function: vote 12345678901234567890 12345678901234567890*/
        else if (input_data.find("vote") != input_data.npos) {    
            std::string _voter, _nominator;
            parseVote(input_data, _voter, _nominator);
            if (vote(file_path, mode, _voter.c_str(), _nominator.c_str())) {
                sendto(send_SocketFD, "Vote successfully!", strlen("Vote successfully!"), 0, (struct sockaddr *)&stRemoteAddr, sizeof(stRemoteAddr));
            }
            else {
                sendto(send_SocketFD, "Vote failed!", strlen("Vote failed!"), 0, (struct sockaddr *)&stRemoteAddr, sizeof(stRemoteAddr));
            }
        }
        else {
            printf("error call! \n");
        }
        memset(acBuf, 0, sizeof(acBuf));
        // sendto(send_SocketFD, "hello World!", strlen("hello World!"), 0, (struct sockaddr *)&stRemoteAddr, sizeof(stRemoteAddr));

	}
	close(iSocketFD);
    close(send_SocketFD);
}