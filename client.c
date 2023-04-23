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

struct connect_channel
{
    int client_id;
};

struct communication_channel
{
    int request_type;
    int client_id;
    int data[3];
};

int main(int argc, char **argv)
{
    int connect_shmid = shmget(CONNECT_CHANNEL_ID, MAX_SHM_SIZE, 0666 | IPC_CREAT);
    if (connect_shmid < 0)
    {
        perror("shmget (connect channel)");
        exit(1);
    }

    int communication_shmid = shmget(COMMUNICATION_CHANNEL_ID, MAX_SHM_SIZE, 0666 | IPC_CREAT);
    if (communication_shmid < 0)
    {
        perror("shmget (communication channel)");
        exit(1);
    }

    struct connect_channel *connect_channel_ptr = (struct connect_channel *)shmat(connect_shmid, NULL, 0);
    struct communication_channel *communication_channel_ptr = (struct communication_channel *)shmat(communication_shmid, NULL, 0);

    int client_id = getpid();
    connect_channel_ptr->client_id = client_id;

    printf("Client %d registered\n", client_id);

    while (1)
    {
        int request_type;
        printf("Enter request type (1=register, 2=unregister, 3=calculation): ");
        scanf("%d", &request_type);

        if (request_type == REGISTER_REQUEST)
        {
            connect_channel_ptr->client_id = client_id;
            printf("Client %d registered\n", client_id);
        }
        else if (request_type == UNREGISTER_REQUEST)
        {
            connect_channel_ptr->client_id = 0;
            printf("Client %d unregistered\n", client_id);
        }
        else if (request_type == CALCULATION_REQUEST)
        {
            int num1, num2, operation;

            printf("Enter operation type (1=add, 2=subtract, 3=multiply, 4=divide , 5=even/odd , 6=isprime , 7=isnegative): ");
            scanf("%d", &operation);
            if (operation >4)
            {
                printf("Enter number: ");

                scanf("%d", &num1);
                printf("0 for even 1 for odd");
                num2 = __INT_MAX__;
            }

            else if(operation ==5){
                
            }
            else{

                printf("Enter two numbers: ");
                scanf("%d %d", &num1, &num2);
            }

            communication_channel_ptr->request_type = CALCULATION_REQUEST;
            communication_channel_ptr->client_id = client_id;
            communication_channel_ptr->data[0] = num1;
            communication_channel_ptr->data[1] = num2;
            communication_channel_ptr->data[2] = operation;

            printf("Sending calculation request to server...\n");
            sleep(1);

            if (connect_channel_ptr->client_id == 0)
            {
                printf("Server is not available\n");
                continue;
            }

            printf("Received result from server: %d\n", communication_channel_ptr->data[0]);
        }
        else
        {
            printf("Invalid request type\n");
        }
    }

    shmdt(connect_channel_ptr);
    shmdt(communication_channel_ptr);

    return 0;
}
