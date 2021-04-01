#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sys/types.h>
#include <cstring>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <openssl/des.h>
using namespace std;

const uint USER_LENGTH = 20;
const uint CRYPT_LENGTH = 24;
string desKey = "12345";

string des_encrypt(const string &clearText, const string &key)
{
	string cipherText; 
 
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
	vector<unsigned char> vecCiphertext;
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
 
string des_decrypt(const string &cipherText, const string &key)
{
	string clearText; 
 
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
	vector<unsigned char> vecCleartext;
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

void createRandom(const char *_path, const int &length, const int &number) {
    ofstream outfp;
    outfp.open(_path, ios::out | ios::binary);
    vector<char> temp;
    pair<char[USER_LENGTH + 1], uint> element;
    string temp_decrpyt_text;
    string temp_encrpyt_text;
    string temp_map_text = "";
    string initName = "12345678901234567890";
    temp_encrpyt_text = des_encrypt(initName, desKey);
    temp_map_text += temp_encrpyt_text;
    temp_decrpyt_text = to_string(1000);
    temp_encrpyt_text = des_encrypt(temp_decrpyt_text, desKey);
    temp_map_text += temp_encrpyt_text;
    outfp.write(reinterpret_cast<char *>((char *)temp_map_text.c_str()), CRYPT_LENGTH + CRYPT_LENGTH/3);

    for (int i = 0; i < number; i++) {
        temp.clear();
        temp_map_text = "";
        for (int j = 0; j < length; j++) {
            char c = static_cast<char>(rand() % 90 + 33);
            temp.push_back(c);
        }
        // printf("char is: %c \n", c);
        random_shuffle(temp.begin(), temp.end());
        for (int k = 0; k < temp.size(); k++) {
            element.first[k] = temp[k];
        }
        temp_decrpyt_text = element.first;
        temp_encrpyt_text = des_encrypt(temp_decrpyt_text, desKey);
        temp_map_text += temp_encrpyt_text;
        temp_decrpyt_text = to_string(100);
        temp_encrpyt_text = des_encrypt(temp_decrpyt_text, desKey);
        temp_map_text += temp_encrpyt_text;
        // printf("new account is: %s \n", element.first);
        outfp.write(reinterpret_cast<char *>((char *)temp_map_text.c_str()), CRYPT_LENGTH + CRYPT_LENGTH/3);
        // outfp << element.first << " " << element.second << endl;
    }
    outfp.close();
}

int main(int argc, char *argv[]) {
    char * file_path = argv[1];
    int length = atoi(argv[2]);
    int number = atoi(argv[3]);
    createRandom(file_path, length, number);
    return 0;
}