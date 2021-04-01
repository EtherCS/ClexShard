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

const uint USER_LENGTH = 20;
const uint CRYPT_LENGTH = 24;
uint FUN_NOMINATE_SIZE = 8;
uint FUN_VOTE_SIZE = 4;

/* state cache */
char manager[USER_LENGTH + 1];   // contract creator
uint numNominators;
std::map<std::string, uint> nominators;    // nominator name -> received vote number
std::map<std::string, uint> voters;     // voter name -> vote number

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
    nominators.clear();
    voters.clear();
    std::pair<char[USER_LENGTH + 1], uint> tempNominators;
    std::pair<char[USER_LENGTH + 1], uint> tempVoters;
    std::ifstream infp(_path, std::ios::in | std::ios::binary);
    if (!infp.is_open()) {
        printf("Open file failed! \n");
        exit(0);
    }
    char read_temp[CRYPT_LENGTH];
    char read_map_temp[CRYPT_LENGTH+CRYPT_LENGTH/3];
    std::string temp_decrpyt_text;
    std::string temp_encrpyt_text;
    /* Resolve manager */
    infp.read(reinterpret_cast<char *>(read_temp), sizeof(read_temp));
    temp_encrpyt_text = read_temp;
    temp_decrpyt_text = des_decrypt(temp_encrpyt_text, desKey);
    strncpy(manager, temp_decrpyt_text.c_str(), USER_LENGTH);

    /* Resolve numNominators */
    infp.read(reinterpret_cast<char *>(read_temp), sizeof(read_temp));
    temp_encrpyt_text = read_temp;
    temp_decrpyt_text = des_decrypt(temp_encrpyt_text, desKey);
    numNominators = (uint)(atoi(temp_decrpyt_text.c_str()));

    /* Resolve Nominators */
    for (int i = 0; i < numNominators; i++) {       // get all nominators
        infp.read(reinterpret_cast<char *>(read_map_temp), sizeof(read_map_temp));
        std::string temp_enc_nominator(read_map_temp, read_map_temp + CRYPT_LENGTH);
        std::string temp_enc_nominator_num(read_map_temp + CRYPT_LENGTH, strlen(read_map_temp));
        std::string temp_decrpyt_nominator = des_decrypt(temp_enc_nominator, desKey);
        strncpy(tempNominators.first, temp_decrpyt_nominator.c_str(), USER_LENGTH);
        std::string temp_decrpyt_nominator_num = des_decrypt(temp_enc_nominator_num, desKey);
        tempNominators.second = (uint)(atoi(temp_decrpyt_nominator_num.c_str()));
        nominators[tempNominators.first] = tempNominators.second;
    }

    /* Resolve Voters */
    while(infp.peek() != EOF) {        // get all voters
        infp.read(reinterpret_cast<char *>(read_map_temp), sizeof(read_map_temp));
        std::string temp_enc_voter(read_map_temp, read_map_temp + CRYPT_LENGTH);
        std::string temp_enc_voter_num(read_map_temp + CRYPT_LENGTH, strlen(read_map_temp));
        // temp_encrpyt_text = read_temp;
        std::string temp_decrpyt_voter = des_decrypt(temp_enc_voter, desKey);
        strncpy(tempVoters.first, temp_decrpyt_voter.c_str(), USER_LENGTH);
        std::string temp_decrpyt_voter_num = des_decrypt(temp_enc_voter_num, desKey);
        tempVoters.second = (uint)(atoi(temp_decrpyt_voter_num.c_str()));
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
    std::string temp_decrpyt_text;
    std::string temp_encrpyt_text;
    std::string temp_map_text;
    /* encrypt manager */
    temp_decrpyt_text = manager;
    temp_encrpyt_text = des_encrypt(temp_decrpyt_text, desKey);
    outfp.write(reinterpret_cast<char *>((char *)temp_encrpyt_text.c_str()), CRYPT_LENGTH);

    /* encrypt numNominators */
    temp_decrpyt_text = std::to_string(numNominators);
    temp_encrpyt_text = des_encrypt(temp_decrpyt_text, desKey);
    outfp.write(reinterpret_cast<char *>((char *)temp_encrpyt_text.c_str()), CRYPT_LENGTH);

    /* encrypt Nominators */
    std::map<std::string, uint>::reverse_iterator nomIter;
    for (nomIter = nominators.rbegin(); nomIter != nominators.rend(); nomIter++) {
        temp_map_text = "";
        temp_decrpyt_text = nomIter->first;
        temp_encrpyt_text = des_encrypt(temp_decrpyt_text, desKey);
        temp_map_text += temp_encrpyt_text;
        temp_decrpyt_text = std::to_string(nomIter->second);
        temp_encrpyt_text = des_encrypt(temp_decrpyt_text, desKey);
        temp_map_text += temp_encrpyt_text;
        outfp.write(reinterpret_cast<char *>((char *)temp_map_text.c_str()), CRYPT_LENGTH + CRYPT_LENGTH/3);
    }

    /* encrypt Voters */
    std::map<std::string, uint>::reverse_iterator voterIter;
    for (voterIter = voters.rbegin(); voterIter != voters.rend(); voterIter++) {
        temp_map_text = "";
        temp_decrpyt_text = voterIter->first;
        temp_encrpyt_text = des_encrypt(temp_decrpyt_text, desKey);
        temp_map_text += temp_encrpyt_text;
        temp_decrpyt_text = std::to_string(voterIter->second);
        temp_encrpyt_text = des_encrypt(temp_decrpyt_text, desKey);
        temp_map_text += temp_encrpyt_text;
        outfp.write(reinterpret_cast<char *>((char *)temp_map_text.c_str()), CRYPT_LENGTH + CRYPT_LENGTH/3);
    }
    outfp.close();
}

bool initial(const char *_path, const char *_manager) {
    struct stat buffer;
    std::ofstream outfp(_path, std::ios::out | std::ios::binary);
    char temp_manager[USER_LENGTH + 1];
    strncpy(temp_manager, _manager, USER_LENGTH + 1);
    if (!outfp.is_open()) {
        printf("Open file failed! \n");
        return false;
    }
    uint temp_num = 0;
    std::string temp_decrpyt_text;
    std::string temp_encrpyt_text;
    /* encrypt manager */
    temp_decrpyt_text = temp_manager;
    temp_encrpyt_text = des_encrypt(temp_decrpyt_text, desKey);
    outfp.write(reinterpret_cast<char *>((char *)temp_encrpyt_text.c_str()), CRYPT_LENGTH);

    /* encrypt numNominators */
    temp_decrpyt_text = std::to_string(temp_num);
    temp_encrpyt_text = des_encrypt(temp_decrpyt_text, desKey);
    outfp.write(reinterpret_cast<char *>((char *)temp_encrpyt_text.c_str()), CRYPT_LENGTH);
    outfp.close();
    return true;
}

bool nominate(const char *_path, const char *_mode, const char *_caller, std::vector<std::string> &_nominators, std::vector<std::string> &_voters) {
    loadState(_path);
    if (strcmp(manager, _caller) == 0) {
        numNominators = (uint)(_nominators.size());
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
        }
        for (int i = 0; i < _voters.size(); i++) {
            char tempVoter[USER_LENGTH + 1];
            for (int j = 0; j < _voters[i].length(); j++) {
                tempVoter[j] = _voters[i][j];
            }
            voters[tempVoter] = 1;
        }
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
    _nominators.clear();
    _voters.clear();
    _caller = input_data.substr(FUN_NOMINATE_SIZE + 1, USER_LENGTH);
    uint numNominators;
    int index_1 = input_data.find(" ", FUN_NOMINATE_SIZE + 1);
    int index_2 = input_data.find(" ", index_1 + 1);
    std::string str_temp = input_data.substr(index_1 + 1, index_2 - index_1);
    numNominators = (uint)(atoi(str_temp.c_str()));
    for (int i = 0; i < numNominators; i++) {
        index_1 = index_2;
        index_2 = input_data.find(" ", index_1 + 1);
        str_temp = input_data.substr(index_1 + 1, index_2 - index_1);
        _nominators.push_back(str_temp);
    }
    while (input_data.find(" ", index_2 + 1) != input_data.npos) {
        index_1 = index_2;
        index_2 = input_data.find(" ", index_1 + 1);
        str_temp = input_data.substr(index_1 + 1, index_2 - index_1);
        _voters.push_back(str_temp);
    }
    str_temp = input_data.substr(index_2 + 1);
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
	stLocalAddr.sin_port   = htons(10081);
	stLocalAddr.sin_addr.s_addr = 0;
 
	/* 绑定地址 */
	if(0 > bind(iSocketFD, (struct sockaddr *)&stLocalAddr, sizeof(stLocalAddr)))
	{
		printf("绑定地址失败!\n");
		close(iSocketFD);
		return 0;
	}
    if(!initial(file_path, creator)) {
        exit(0);
    }
    std::vector<std::string> _nominators, _voters;
    while((iRecvLen = recvfrom(iSocketFD, acBuf, sizeof(acBuf), 0, (struct sockaddr *)&stRemoteAddr, &iRemoteAddrLen))>0)     //实现了循环监听
	{
        // TODO: Resolve ip and port from user
        stRemoteAddr.sin_family = AF_INET;
        stRemoteAddr.sin_port = htons(10082);
        stRemoteAddr.sin_addr.s_addr = htonl(INADDR_ANY); // localhost
        std::string input_data = acBuf;
        /* Nominate Function: nominate 12345678901234567890 2 12345678901234567890 01234567890123456789 12345678901234567890 01234567890123456789*/
        if (input_data.find("nominate") != input_data.npos) {     
            std::string _caller;
            parseNominate(input_data, _caller, _nominators, _voters);
            if (nominate(file_path, mode, _caller.c_str(), _nominators, _voters)) {
                sendto(send_SocketFD, "successfully!", strlen("successfully!"), 0, (struct sockaddr *)&stRemoteAddr, sizeof(stRemoteAddr));
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
	}
	close(iSocketFD);
    close(send_SocketFD);
}