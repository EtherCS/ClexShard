#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <map>
#include <vector>
#include <openssl/des.h>
#include <unistd.h>


using namespace std;


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

/* g++ test.cpp -o test -lcryptopp */
int main(int argc, char *argv[]) {
    char *inputTx = argv[1];
    string str = inputTx;
    // "This is a encry text";
    cout << "before encry: " << str << endl;
    string encry_str;
    encry_str = des_encrypt(str, desKey);
    cout << "after encry: " << encry_str << endl;
    cout << "encry text size is " << encry_str.length() << endl;
    string decry_str;
    decry_str = des_decrypt(encry_str, desKey);
    cout << "after decry: " << decry_str << endl;

}