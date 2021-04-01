#include <stdio.h>
#include <stdlib.h>
// mode = {"create", "balance", "transfer"}
// parameter(name, "create", user, initial coin, path, execute amount): create an account
// parameter(name, "balance", user, path, execute amount): get balance
// parameter(name, "transfer", A, B, amount, path, execute amount) means A transfer ammount coins to B 
struct {
    char name[20];
    uint balance;
} Users;

void create(const int user, const int ini_coins, const char *path, int times) {
	int balance[3] = {0, 0, 0};
	FILE *fp = NULL;
	FILE *wfp = NULL;
	while (times-- > 0) {
		if ((fp = fopen(path, "a+")) == NULL) {
			printf("\nFirst open file");
		}
		for (int i = 0; i < 3; i++) {
			fscanf(fp, "%d\n", &balance[i]);
		}
		balance[user] = ini_coins;
		fclose(fp);
		if ((wfp = fopen(path, "w")) == NULL) {
			printf("\nCannot open write file");
		}
		for (int i = 0; i < 3; i++) {
			fprintf(wfp, "%d\n", balance[i]);
		}
		fclose(wfp);
		
	}
	// free(fp);
	// free(wfp);
}

void getBalance(const int user, const char *path, int times) {
	int balance[3] = {0, 0, 0};
	FILE *fp = NULL;
	while (times-- > 0) {
		if ((fp = fopen(path, "a+")) == NULL) {
			printf("\nFirst open file");
		}
		for (int i = 0; i < 3; i++) {
			fscanf(fp, "%d\n", &balance[i]);
		}
		printf("The balance of user %d is %d \n", user, balance[user]);
		fclose(fp);
		// free(fp);
	}
}

void transfer(const int user_a, const int user_b, const int value, const char *path, int times) {
	FILE *fp = NULL;
	FILE *wfp = NULL;
	while (times-- > 0) {
		int balance[3] = {0, 0, 0};
		if ((fp = fopen(path, "a+")) == NULL) {
			printf("\nFirst open file");
		}
		for (int i = 0; i < 3; i++) {
			fscanf(fp, "%d\n", &balance[i]);
		}
		fclose(fp);
		if ((wfp = fopen(path, "w")) == NULL) {
			printf("\nCannot open write file");
		}
		if (balance[user_a] < value) {
			printf("user has not enough coins!\n");
		}
		else {
			balance[user_a] -= value;
			balance[user_b] += value;
		}
	
		for (int i = 0; i < 3; i++) {
			fprintf(wfp, "%d\n", balance[i]);
		}
		fclose(wfp);
		// free(fp);
		// free(wfp);
	}
}

int main(int argc, char *argv[]) {
	char *mode;
    char *file_path;
    file_path = argv[1];

    char *user;
    user = argv[2];
    int ini_coins, times;
    // sscanf(argv[2], "%s", &user);
    sscanf(argv[3], "%d", &ini_coins);
    sscanf(argv[4], "%d", &times);
		// free(file_path);
    FILE *wfp = NULL;
    if ((wfp = fopen(file_path, "w")) == NULL) {
        printf("\nCannot open write file");
        return -1;
    }
    // printf("user is: %s", *user);
    for (int i = 0; i < times; i++) {
        fprintf(wfp, "%-20.20s %d\n", user, ini_coins);
        // fprintf(wfp, "%s\n", user);
    }
    fclose(wfp);
    
    return 0;
}
