#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * mode = {"AddCandidate", "Vote"}
 * parameter{name, "AddCandidate", user, path, times}
 * parameter{name, "Vote", candidate, path, times}
 */

void addCandidate(const int user, const char *path, int times)
{
	int candidates[3] = {0, 0, 0};
	FILE *fp = NULL;
	
	while(times-- > 0) {
		if((fp = fopen(path, "a+")) == NULL) {
			printf("\nFirst open file");
		}
		for (int i = 0; i < 3; i++) {
			fscanf(fp, "%d\n", &candidates[i]);
		}
		fclose(fp);
		
		candidates[user] = 1;
		
		if((fp = fopen(path, "w")) == NULL) {
			printf("\nCannot open write file");
			return;
		}
		for (int i = 0; i < 3; i++) {
			fprintf(fp, "%d\n", candidates[i]);
		}
		fclose(fp);
	}
}


void vote(const int candidate, const char *path, int times)
{
	FILE *fp = NULL;
	while(times-- > 0) {
		int candidates[3] = {0, 0, 0};
		if((fp = fopen(path, "a+")) == NULL) {
			printf("\nFirst open file");
		}
		for (int i = 0; i < 3; i++) {
			fscanf(fp, "%d\n", &candidates[i]);
		}
		fclose(fp);
		
		candidates[candidate]++;
		
		if((fp = fopen(path, "w")) == NULL) {
			printf("\nCannot open write file");
			return;
		}
		for (int i = 0; i < 3; i++) {
			fprintf(fp, "%d\n", candidates[i]);
		}
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
	
	if(strcmp(mode, "addCandidate") == 0) {
		if (argc != 5) {
			printf("AddCandidate with error parameter...\n");
			return -1;
		}
		int user, times;
		char *path = argv[3];
		sscanf(argv[2], "%d", &user);
		sscanf(argv[4], "%d", &times);
		addCandidate(user, path, times);
	}
	else if(strcmp(mode, "vote") == 0) {
		if (argc != 5) {
			printf("Vote with error parameter...\n");
			return -1;
		}
		int candidate, times;
		char *path = argv[3];
		sscanf(argv[2], "%d", &candidate);
		sscanf(argv[4], "%d", &times);
		vote(candidate, path, times);
	}
	else {
		printf("Error function.\n");
		return -1;
	}
}
