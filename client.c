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
#define SIZE 4096

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

void clearBuf(char *b)
{
    for (int i = 0; i < SIZE; i++)
        b[i] = '\0';
}

void printlist()
{

    char filename[1000][100];
    int file_num;

    printf("Loading List from the Server. \nPlease Wait..\n");

    // if (recv(client_socket, &handshake, sizeof(handshake), 0)<0)
    // {
    //     printf("Handshaking Failed\n");
    // }

    // if (recv(client_socket, &c, sizeof(int), 0) < 0)
    // {
    //     printf("Total Client Not recieved\n");
    // }

    //  if (recv(client_socket, &clients, sizeof(struct client), 0) < 0)
    // {
    //     printf("Total Client Not recieved\n");
    // }

    recv(client_socket, &file_num, sizeof(file_num), 0);
    printf("%d\n", file_num);

    for (int i = 1; i <= file_num; i++)
    {
        recv(client_socket, &filename[i], sizeof(filename[i]), 0);
        printf("%d. %s\n", i, filename[i]);
    }
}

void create_file_and_download(char *buffer)
{
    send(client_socket, buffer, SIZE, 0);

    ssize_t filesize;
    recv(client_socket, &filesize, sizeof(filesize), 0);

    if ((fp = fopen(buffer, "wb")) != NULL)
    {
        printf("File Creation successful");
    }
    else
    {
        printf("File Creation Unsuccessful");
    }

    printf("\n---------Data Received---------\n");

    ssize_t n, total = 0;
    char buff[SIZE] = {0};
    while (filesize != total)
    {
        n = recv(client_socket, &buff, SIZE, 0);
        total += n;
        if (n == -1)
        {
            perror("Receive File Error");
            exit(1);
        }

        if (fwrite(buff, sizeof(char), n, fp) != n)
        {
            perror("Write File Error");
            exit(1);
        }
        memset(buff, 0, SIZE);
        printf("\n-------------------------------\n");
    }
    fclose(fp);
}

ssize_t size_of_a_file(char *filename)
{
    FILE *fp1 = fopen(filename, "rb");
    ssize_t size = 0;

    fseek(fp1, 0, SEEK_END);
    size = ftell(fp1);
    fclose(fp1);

    return size;
}

void upload_file()
{
    // printf("\nWaiting for file name...\n");
    // clearBuf(buffer);
    send(client_socket, buffer, SIZE, 0);

    ssize_t filesize = size_of_a_file(buffer);
    send(client_socket, &filesize, sizeof(filesize), 0);

    fp = fopen(buffer, "rb");
    // printf("\nFile Name Received: %s\n", buffer);

    int n;
    ssize_t total = 0;
    char sendline[SIZE] = {0};
    while (((n = fread(sendline, sizeof(char), SIZE, fp)) > 0) && filesize != total)
    {
        total += n;
        if (n != SIZE && ferror(fp))
        {
            perror("Read File Error");
            exit(1);
        }

        if (send(client_socket, sendline, n, 0) == -1)
        {
            perror("Can't send file");
            exit(1);
        }
        memset(sendline, 0, SIZE);
    }
    fclose(fp);
}

int main(int argc, char *argv[])
{
    system("clear");
    client_creation(AF_INET, SOCK_STREAM, IPPROTO_TCP, PORT, inet_addr("192.168.0.7"));
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
            printlist();
            printf("Enter the file name you want to download:- ");
            scanf("%s", buffer);
            getchar();
            create_file_and_download(buffer);
        }
        else if (strncmp(command, "u", 1) == 0)
        {

            printf("Enter the file name you want to upload:- ");
            scanf("%s", buffer);
            getchar();
            upload_file();
        }
        else if (strncmp(command, "q", 1) == 0)
        {
            close(client_socket);
            break;
        }
    }
    return 0;
}
