#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4

/* the available amount of each resource */
int available[NUMBER_OF_RESOURCES];

/* the maximum demand of each customer */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the amount currently allocated to each customer */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the remaining need of each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

int is_safe_state() {
    int i, j, k, finish[NUMBER_OF_CUSTOMERS], work[NUMBER_OF_RESOURCES];
    for (i=0; i < NUMBER_OF_CUSTOMERS; ++i) {
        finish[i] = 0;
    }
    for (i=0; i < NUMBER_OF_RESOURCES; ++i) {
        work[i] = available[i];
    }
    for (i=0; i < NUMBER_OF_CUSTOMERS; ++i) {
        /* find j s.t. finish[j] = 0 && need[j] < work */
        for (j=0; j < NUMBER_OF_CUSTOMERS; ++j) {
            if (finish[j] == 1)
                continue;
            for (k=0; k < NUMBER_OF_RESOURCES; ++k) {
                if (need[j][k] < work[k])
                    break;
            }
            if (k == NUMBER_OF_RESOURCES)
                break;
        }
        /* not found */
        if (j == NUMBER_OF_CUSTOMERS)
            return -1;
        
        finish[j] = 1;
        for (k = 0; k < NUMBER_OF_RESOURCES; ++k)
            work[k] += allocation[j][k];
    }
    return 0;
}

int request_resources(int customer_num, int request[]) {
    int i, ret;
    for (i = 0; i < NUMBER_OF_RESOURCES; ++i) {
        if (request[i] > need[customer_num][i] || request[i] > available[i]) { 
            return -1;
        }
    }
    for (i = 0; i < NUMBER_OF_RESOURCES; ++i) {
        allocation[customer_num][i] += request[i];
        need[customer_num][i] -= request[i];
        available[i] -= request[i];
    }
    ret = is_safe_state();
    for (i = 0; i < NUMBER_OF_RESOURCES; ++i) {
        allocation[customer_num][i] -= request[i];
        need[customer_num][i] += request[i];
        available[i] += request[i];
    }
    return ret;
}

void release_resources(int customer_num, int release[]) {
    int i;
    for (i=0; i < NUMBER_OF_RESOURCES; ++i) {
        if (allocation[customer_num][i] < release[i]) {
            printf("ERROR: not enough resources to release\n");
            return;
        }
    }
    for (i=0; i < NUMBER_OF_RESOURCES; ++i) {
        allocation[customer_num][i] -= release[i];
        need[customer_num][i] += release[i];
        available[i] += release[i];
    }
}

void init() {
    FILE *fd;
    int i,j;
    fd = fopen("maximum.txt", "r");
    for (i = 0; i < NUMBER_OF_CUSTOMERS; ++i) {
        for (j = 0; j < NUMBER_OF_RESOURCES - 1; ++j) {
            fscanf(fd, "%d,", &maximum[i][j]);
            allocation[i][j] = 0;
            need[i][j] = maximum[i][j];
        }
        fscanf(fd, "%d", &maximum[i][j]);
        need[i][j] = maximum[i][j];
    }
}

int main(int argc, char *argv[]) {
    int i, j;
    char cmd[3];
    int cmd_customer;
    int cmd_resources[NUMBER_OF_RESOURCES];
    
    init();

    for (i = 0; i < NUMBER_OF_RESOURCES; ++i) {
        available[i] = atoi(argv[i+1]);
    }
   
    while (scanf("%s", cmd) != EOF) {
        if (strcmp(cmd, "*") == 0) {
            printf("available:\n");
            printf("  ");
            for (i = 0; i < NUMBER_OF_RESOURCES; ++i)
                printf("%d ", available[i]);
            printf("\n");
            printf("maximum:\n");
            for (i = 0; i < NUMBER_OF_CUSTOMERS; ++i) {
                printf("  ");
                for (j = 0; j < NUMBER_OF_RESOURCES; ++j)
                    printf("%d ", maximum[i][j]);
                printf("\n");
            }
            printf("allocation:\n");
            for (i = 0; i < NUMBER_OF_CUSTOMERS; ++i) {
                printf("  ");
                for (j = 0; j < NUMBER_OF_RESOURCES; ++j)
                    printf("%d ", allocation[i][j]);
                printf("\n");
            } 
            printf("need:\n");
            for (i = 0; i < NUMBER_OF_CUSTOMERS; ++i) {
                printf("  ");
                for (j = 0; j < NUMBER_OF_RESOURCES; ++j)
                    printf("%d ", need[i][j]);
                printf("\n");
            }
        }            
            
        else {
            scanf("%d", &cmd_customer);
            for (i = 0; i < NUMBER_OF_RESOURCES; ++i)
                scanf("%d", cmd_resources+i);
            if (strcmp(cmd, "RQ") == 0) {
                if (request_resources(cmd_customer, cmd_resources) == 0)
                    printf("request success\n");
                else
                    printf("request denied\n");
            }
            else
                release_resources(cmd_customer, cmd_resources);
        }
    }
}
