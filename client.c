#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#define PORT 4444
#define SIZE 1024
#define cipherKey 'S'

int client_socket;
struct sockaddr_in server_address, client_address;
int address_length = sizeof(struct sockaddr_in);
char buffer[SIZE];
FILE *fp;

void client_creation(int domain, int type, int protocol, int port, u_int32_t internet_address)
{

    // int option_value = 1;

    /* initializing a server socket */
    client_socket = socket(domain, type, protocol);
    if (client_socket < 0)
    {
        printf("Socket creation Unsuccessful");
    }

    /* initializing server address */
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = domain;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = internet_address;
}

// function to clear buffer
void clearBuf(char *b)
{
    for (int i = 0; i < SIZE; i++)
        b[i] = '\0';
}

// function for decryption
char Cipher(char ch)
{
    return ch ^ cipherKey;
}

// function to receive file
int recvFile(char *buf, int s)
{
    char ch;
    for (int i = 0; i < s; i++)
    {
        ch = buf[i];
        ch = Cipher(ch);
        if (ch == EOF)
            return 1;
        else
            fprintf(fp,"%c", ch);
    }
    return 0;
}

void download(char *filename)
{

    if ((fp = fopen(filename, "w")) == NULL)
    {
        printf("Error in downloading the file");
        exit(EXIT_SUCCESS);
    }
    char data[SIZE] = {0};
    while (1)
    {

        if (recv(client_socket, data, sizeof(data), 0) <= 0)
            break;
        fprintf(fp, "%s", data);
        bzero(data, SIZE);
    }
    return;
}

void upload(char *filename)
{

    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("Error in opening the file");
        exit(EXIT_SUCCESS);
    }

    char data[SIZE] = {0};
    while (fgets(data, SIZE, fp) != NULL)
    {

        if (send(client_socket, data, sizeof(data), 0) == -1)
        {
            perror("Error in sending data");
            exit(1);
        }
        bzero(data, SIZE);
    }
}

void create_file_and_download(char *buffer)
{
    if ((fp = fopen(buffer, "w")) != NULL)
    {
        printf("File Creation successful");
    }
    else
    {
        printf("File Creation Unsuccessful");
    }

    send(client_socket, buffer, SIZE, 0);

    printf("\n---------Data Received---------\n");

    while (1)
    {
        clearBuf(buffer);
        recv(client_socket, buffer, SIZE, 0);

        if (recvFile(buffer, SIZE))
        {
            break;
        }
        printf("\n-------------------------------\n");
    }
    if (fp != NULL)
        fclose(fp);
}
int main()
{

    system("clear");
    client_creation(AF_INET, SOCK_STREAM, IPPROTO_TCP, PORT, inet_addr("127.0.0.1"));
    int ret = connect(client_socket, (struct sockaddr *)&server_address, address_length);

    while (1)
    {
        printf("Press one of the following commands:-\n> d: download a file\n> u: upload a file\n> q: quit\n");

        char command[2];
        fgets(command, sizeof(command), stdin);
        getchar();
        send(client_socket, &command, sizeof(command), 0);


        if (strncmp(command, "d", 1) == 0)
        {
            printf("Enter the file name you want to download:- ");
            scanf("%s", buffer);
            getchar();
            create_file_and_download(buffer); 
        }
    }
    return 0;
}