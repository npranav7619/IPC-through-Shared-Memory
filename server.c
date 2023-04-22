#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>

#define CONNECT_CHANNEL_ID 100
#define COMMUNICATION_CHANNEL_ID 200

#define REGISTER_REQUEST 1
#define UNREGISTER_REQUEST 2
#define CALCULATION_REQUEST 3

#define MAX_SHM_SIZE 1024

struct connect_channel {
    int client_id;
};

struct communication_channel {
    int request_type;
    int client_id;
    int data[3]; // Added a third element for operation type
};

void *calculation_thread(void *arg);

int add_thread(num1,num2){
    int c = num1+num2;
    return c;
}
int sub_thread(num1,num2){
    return num1-num2;
}
int mul_thread(num1,num2){
    return num1*num2;
}
int div_thread(num1,num2){
    return num1/num2;
}
int main(int argc, char **argv) {
    int connect_shmid = shmget(CONNECT_CHANNEL_ID, MAX_SHM_SIZE, 0666 | IPC_CREAT);
    int count;
    if (connect_shmid < 0) {
        perror("shmget (connect channel)");
        exit(1);
    }
    int client_id_ifsame;
    int communication_shmid = shmget(COMMUNICATION_CHANNEL_ID, MAX_SHM_SIZE, 0666 | IPC_CREAT);
    if (communication_shmid < 0) {
        perror("shmget (communication channel)");
        exit(1);
    }

    struct connect_channel *connect_channel_ptr = (struct connect_channel *) shmat(connect_shmid, NULL, 0);
    struct communication_channel *communication_channel_ptr = (struct communication_channel *) shmat(communication_shmid, NULL, 0);

    int client_id = 0;
    //int array=[]; // add client id here
    while (1) {
        if (connect_channel_ptr->client_id != client_id) {
            client_id = connect_channel_ptr->client_id;
            printf("Client %d registered\n", client_id);
            count = 0;
        }
        if (communication_channel_ptr->request_type == CALCULATION_REQUEST) {
            int num1 = communication_channel_ptr->data[0];
            int num2 = communication_channel_ptr->data[1];
            int operation = communication_channel_ptr->data[2];
            int result;
            if (operation == 1) {
                //create threads here for every result
                result = add_thread(num1,num2);
                //result = num1 + num2;
            } else if (operation == 2) {
                result = sub_thread(num1,num2);
                //result = num1 - num2;
            } else if (operation == 3) {
                result = mul_thread(num1,num2);
                //result = num1 * num2;
            } else if (operation == 4) {
                result = div_thread(num1,num2);
                //result = num1 / num2;
            } else {
                printf("Invalid operation type\n");
                continue;
            }

            printf("Client %d requested calculation of %d %d\n", client_id, num1, num2);
            
            communication_channel_ptr->request_type = 0; // Reset request type
            communication_channel_ptr->data[0] = result;

            printf("Sending result %d to client %d\n", result, client_id);
            
            if (client_id_ifsame = client_id){
                count = count + 1;
                printf("Count is :");
                printf("%d\n",count);
            }
        }
    }

    shmdt(connect_channel_ptr);
    shmdt(communication_channel_ptr);

    return 0;
}
