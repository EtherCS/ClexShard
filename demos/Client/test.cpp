#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <cstring>
#include <string>
#include <map>
using namespace std;

void test(char * &_a, string b = "abc"){
    _a = (char*)b.substr(0,2).c_str();
}
int main(int argc, char *argv[]) {
    char *a;
    // char b[7];
    // strncpy(b, a, 7);
    test(a);
    cout << a << endl;
}