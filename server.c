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
    int data[2];
};

void *calculation_thread(void *arg);

int main(int argc, char **argv) {
    int connect_shmid = shmget(CONNECT_CHANNEL_ID, MAX_SHM_SIZE, 0666 | IPC_CREAT);
    if (connect_shmid < 0) {
        perror("shmget (connect channel)");
        exit(1);
    }

    int communication_shmid = shmget(COMMUNICATION_CHANNEL_ID, MAX_SHM_SIZE, 0666 | IPC_CREAT);
    if (communication_shmid < 0) {
        perror("shmget (communication channel)");
        exit(1);
    }

    struct connect_channel *connect_channel_ptr = (struct connect_channel *) shmat(connect_shmid, NULL, 0);
    struct communication_channel *communication_channel_ptr = (struct communication_channel *) shmat(communication_shmid, NULL, 0);

    memset(connect_channel_ptr, 0, MAX_SHM_SIZE);
    memset(communication_channel_ptr, 0, MAX_SHM_SIZE);

    printf("Server started\n");

    while (1) {
        if (connect_channel_ptr->client_id > 0) {
            int client_id = connect_channel_ptr->client_id;

            if (communication_channel_ptr->request_type == CALCULATION_REQUEST) {
                int num1 = communication_channel_ptr->data[0];
                int num2 = communication_channel_ptr->data[1];
                int result;
                int operation = communication_channel_ptr->data[2];

                if (operation == 1) {
                    result = num1 + num2;
                } else if (operation == 2) {
                    result = num1 - num2;
                } else if (operation == 3) {
                    result = num1 * num2;
                } else if (operation == 4) {
                    if (num2 == 0) {
                        result = -1; // Division by zero error
                    } else {
                        result = num1 / num2;
                    }
                } else {
                    result = -2; // Invalid operation error
                }

                printf("Client %d requested calculation: %d %d %d = %d\n", client_id, num1, num2, operation, result);

                communication_channel_ptr->request_type = 0;
                communication_channel_ptr->data[2] = result;
            }
        }

        usleep(100);
    }

    shmdt(connect_channel_ptr);
    shmdt(communication_channel_ptr);

    return 0;
}
