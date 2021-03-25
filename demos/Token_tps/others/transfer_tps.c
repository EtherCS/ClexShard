#include <stdio.h>
#include <stdlib.h>
// mode = {"create", "balance", "transfer"}
// parameter(name, "create", user, initial coin, path, execute amount): create an account
// parameter(name, "balance", user, path, execute amount): get balance
// parameter(name, "transfer", A, B, amount, path, execute amount) means A transfer ammount coins to B 

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
			return -1;
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
			return -1;
		}
		if (balance[user_a] < value) {
			printf("user has not enough coins!\n");
			return 0; // execute failed
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
	if (argc < 5) {
		printf("error parameter...\n");
		return 0;
	}
	char *mode;
	mode = argv[1];
	
	if (strcmp(mode, "create") == 0) {	// create account function
		if (argc < 6) {
			printf("error parameter...\n");
			return 0;
		}
		char *file_path;
		file_path = argv[4];
		int user, ini_coins, times;
		sscanf(argv[2], "%d", &user);
		sscanf(argv[3], "%d", &ini_coins);
		sscanf(argv[5], "%d", &times);
		create(user, ini_coins, file_path, times);
		// free(file_path);
	}
	else if (strcmp(mode, "balance") == 0) {	// getbalance function
		if (argc < 5) {
			printf("error parameter...\n");
			return 0;
		}
		char *file_path;
		file_path = argv[3];
		int user, times;
		sscanf(argv[2], "%d", &user);
		sscanf(argv[4], "%d", &times);
		getBalance(user, file_path, times);
	}
	else if (strcmp(mode, "transfer") == 0) {  // transfer function
		if (argc < 7) {
			printf("error parameter...\n");
			return 0;
		}
		char *file_path;
		file_path = argv[5];
		int user_a, user_b, value, times;
		sscanf(argv[2], "%d", &user_a);
		sscanf(argv[3], "%d", &user_b);
		sscanf(argv[4], "%d", &value);
		sscanf(argv[6], "%d", &times);
		transfer(user_a, user_b, value, file_path, times);
	}
	else {
		printf("error mode\n");
		return 0;
	}
    
    return 0;
}
