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


/* g++ test.cpp -o test -lcryptopp */
int main(int argc, char *argv[]) {
    // char *inputTx = argv[1];
    // string str = inputTx;
    // // "This is a encry text";
    // cout << "before encry: " << str << endl;
    // string encry_str;
    // encry_str = des_encrypt(str, desKey);
    // cout << "after encry: " << encry_str << endl;
    // cout << "encry text size is " << encry_str.length() << endl;
    // string decry_str;
    // decry_str = des_decrypt(encry_str, desKey);
    // cout << "after decry: " << decry_str << endl;
    char *exchange_ch = new char[10*1024*1024];
    // memset(exchange_ch, '1', sizeof(exchange_ch));
    // cout << "data size is: " << exchange_ch << endl;
    cout << "(sizeof)Exchange data size is: " << sizeof(exchange_ch) << endl;
    string c1_state = exchange_ch;
    cout << "(length)Exchange data size is: " << c1_state.length() << endl;
}