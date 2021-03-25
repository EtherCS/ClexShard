#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * mode = {"releaseItem", "bidItem"}
 * parameter{name, "releaseItem", item, path, times}
 * parameter{name, "bidItem", user, money, path, times}
 */

void releaseItem(const char* item, const char *path, int times)
{
	FILE *fp = NULL;
	
	while(times-- > 0) {
		/*
		if((fp = fopen(path, "a+")) == NULL) {
			printf("\nFirst open file");
		}
		for (int i = 0; i < 3; i++) {
			fscanf(fp, "%d\n", &candidates[i]);
		}
		fclose(fp);
		
		candidates[user] = 1;
		*/
		
		if((fp = fopen(path, "w")) == NULL) {
			printf("\nCannot open write file");
			return;
		}
		fprintf(fp, "%s\n", item);
		fprintf(fp, "NULL\n");
		fprintf(fp, "0\n");
		fclose(fp);
	}
}


void bidItem(const char* user, int money, const char *path, int times)
{
	FILE *fp = NULL;
	while(times-- > 0) {
		char item[20];
		char pre_user[20];
		int pre_money;
		if((fp = fopen(path, "a+")) == NULL) {
			printf("First open file\n");
		}
		fscanf(fp, "%s\n", item);
		fscanf(fp, "%s\n", pre_user);
		fscanf(fp, "%d\n", &pre_money);
		fclose(fp);
		
		if(strcmp(user, pre_user) == 0) {
			return;
		}
		
		if(pre_money > money) {
			printf("Not Enough.\n");
			return;
		}
		
		if((fp = fopen(path, "w")) == NULL) {
			printf("Cannot open write file\n");
			return;
		}
		fprintf(fp, "%s\n", item);
		fprintf(fp, "%s\n", user);
		fprintf(fp, "%d\n", money);
		fclose(fp);
	}
}


int main(int argc, char *argv[])
{
	if (argc < 5) {
		printf("error parameter...\n");
		return -1;
	}
	
	char *mode;
	mode = argv[1];
	
	if(strcmp(mode, "releaseItem") == 0) {
		if (argc != 5) {
			printf("releaseItem with error parameter...\n");
			return -1;
		}
		int times;
		char *path = argv[3];
		char *item = argv[2];
		sscanf(argv[4], "%d", &times);
		releaseItem(item, path, times);
	}
	else if(strcmp(mode, "bidItem") == 0) {
		if (argc != 6) {
			printf("bidItem with error parameter...\n");
			return -1;
		}
		int money, times;
		char *user = argv[2];
		char *path = argv[4];
		sscanf(argv[3], "%d", &money);
		sscanf(argv[5], "%d", &times);
		bidItem(user, money, path, times);
	}
	else {
		printf("Error function.\n");
		return -1;
	}
}
