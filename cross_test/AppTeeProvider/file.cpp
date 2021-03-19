#include <stdio.h>
#include <stdlib.h>
#include "EnclaveExecutor_u.h"

int ocall_write(const char *filename, const char *content){
    FILE *fp = NULL;
    if ((fp = fopen("/tmp/temp.db", "w")) == NULL) {
        printf("\nFirst open file");
    }
    fprintf(fp, "%s\n", content);
    fclose(fp);
}
int ocall_read(const char *filename, char *content){
    FILE *fp = NULL;
    if ((fp = fopen("/tmp/temp.db", "a+")) == NULL) {
        printf("\nFirst open file");
    }
    fscanf(fp, "%s\n", content);
    fclose(fp);
}