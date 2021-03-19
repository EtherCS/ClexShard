#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <cstring>
#include <string>
#include <map>
using namespace std;
struct element{
    char a[21];
    uint b;
};
int main(int argc, char *argv[]) {
    string file_path = argv[1];
    uint times = 10;
    string str = "12345678901234567890";
    uint coin = 1000;
    pair<char[21], uint> ele;
    map<string, uint> balance;
    // const char *name = str.c_str();
    // strcpy(ele.first, name);
    // ele.second = coin;
    // cout << "write name is: " << ele.first << " , write coin is: " << ele.second << endl;
    // ofstream outfp(file_path, ios::out | ios::binary); 
    // for (int i = 0; i < times; i++) {
    //     outfp.write(reinterpret_cast<char *>(&ele), sizeof(ele));
    // }
    // outfp.close();

    ifstream infp(file_path,  ios::in | ios::binary);
    pair<char[21], uint> oele;
    while(infp.peek() != EOF) {
        infp.read(reinterpret_cast<char *>(&oele), sizeof(oele));
        // strcpy(uname, element.first);
        balance[oele.first] = oele.second;
        cout << balance[oele.first] << endl;
        cout << "name is: " << oele.first << " , coin is: " << oele.second << endl;
    }
    infp.close();

    // cout << a << endl;
}