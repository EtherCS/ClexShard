#include <stdio.h>

// para(name, amount)

int main(int argc, char *argv[]) {
    int amount = 0;
    if (argc < 1) {
        printf("error parameter...\n");
        return 0;
    }
    sscanf(argv[1], "%d", &amount);
    while (amount--) {
    	printf("Hello World\n");
    }
    return 0;
}
