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
using namespace std;

void createRandom(const char *_path, const int &length, const int &number) {
    ofstream outfp;
    outfp.open(_path, ios::out | ios::binary);
    vector<char> temp;
    pair<char[21], uint> element;
    char ini_name[21] = "12345678901234567890";
    strcpy(element.first, ini_name);
    element.second = 1000;
    outfp.write(reinterpret_cast<char *>(&element), sizeof(element));
    for (int i = 0; i < number; i++) {
        temp.clear();
        for (int j = 0; j < length; j++) {
            char c = static_cast<char>(rand() % 90 + 33);
            temp.push_back(c);
        }
        // printf("char is: %c \n", c);
        random_shuffle(temp.begin(), temp.end());
        for (int k = 0; k < temp.size(); k++) {
            element.first[k] = temp[k];
        }
        // printf("new account is: %s \n", element.first);
        element.second = 100;
        outfp.write(reinterpret_cast<char *>(&element), sizeof(element));
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