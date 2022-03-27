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
#define MAX_CLIENT 31
#define cipherKey 'S'
#define nofile "File Not Found!"


int server_socket, client_socket;
struct sockaddr_in server_address, client_address;
int address_length = sizeof(struct sockaddr_in);
char buffer[SIZE];
FILE *fp;

void server_creation(int domain, int type, int protocol, int port, u_int32_t internet_address, int max_client)
{

    int option_value = 1;

    /* initializing a server socket */
    server_socket = socket(domain, type, protocol);
    if (server_socket < 0)
    {
        printf("Socket creation Unsuccessful");
    }

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(option_value)) < 0)
    {
        perror("setsockoption_value");
        exit(EXIT_FAILURE);
    }

    /* initializing server address */
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = domain;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = internet_address;

    /* binding the address to the socket */
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        printf("Binding Unsuccessful");
    }

    /* server in the listen mode */
    if (listen(server_socket, max_client) < 0)
    {
        printf("Listen Unsuccessful");
    }
    printf("\nServer Ready\n");
}

// function to clear buffer
void clearBuf(char *b)
{

    for (int i = 0; i < SIZE; i++)
        b[i] = '\0';
}

// function to encrypt
char Cipher(char ch)
{
    return ch ^ cipherKey;
}

// function sending file
int sendFile(char *buf, int s)
{
    int i, len;
    if (fp == NULL)
    {
        strcpy(buf, nofile);
        len = strlen(nofile);
        buf[len] = EOF;
        for (i = 0; i <= len; i++)
            buf[i] = Cipher(buf[i]);
        return 1;
    }

    char ch, ch2;
    for (i = 0; i < s; i++)
    {
        ch = fgetc(fp);
        ch2 = Cipher(ch);
        buf[i] = ch2;
        if (ch == EOF)
            return 1;
    }
    return 0;
}

void open_file_and_send()
{
    printf("\nWaiting for file name...\n");
        clearBuf(buffer);
        recv(client_socket,buffer,SIZE,0);

        fp = fopen(buffer, "r");
        printf("\nFile Name Received: %s\n", buffer);

         while (1) {
             if (sendFile(buffer,SIZE))
             {
                 send(client_socket,buffer,SIZE,0);
                 break;
             }
             send(client_socket,buffer,SIZE,0);
             clearBuf(buffer);   
         }
        if (fp != NULL)
            fclose(fp);

}

void download_response(char *buffer)
{

    fp = fopen(buffer, "r");
    printf("\nFile Name Received: %s\n", buffer);
    if (fp == NULL)
    {
        printf("Error in opening the file");
        exit(EXIT_SUCCESS);
    }

    while (1)
    {

        // process
        if (sendFile(buffer, SIZE))
        {
            send(client_socket, buffer, SIZE, 0);
            break;
        }
        send(client_socket, buffer, SIZE, 0);
        clearBuf(buffer);
    }
    if (fp != NULL)
        fclose(fp);
}

void upload_response(char *buffer)
{

    if ((fp = fopen(buffer, "w")) == NULL)
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

int main(int argc,char* argv[])
{


    system("clear");
    server_creation(AF_INET, SOCK_STREAM, IPPROTO_TCP, PORT, inet_addr("127.0.0.1"), MAX_CLIENT);
    client_socket = accept(server_socket, (struct sockaddr *)&server_address, &address_length);

    while (1)
    {
        printf("Server ready to recieve data:-\n");
        char command[2];
        recv(client_socket, &command, sizeof(command), 0);
        if (strncmp(command, "d", 1) == 0) open_file_and_send();
    }
    close(server_socket);
}
