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
#include <dirent.h>

#define PORT 4444
#define SIZE 4096
#define MAX_CLIENT 31


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

void clearBuf(char *b)
{
    for (int i = 0; i < SIZE; i++) b[i] = '\0';
}

ssize_t size_of_a_file(char* filename)
{
    FILE *fp1=fopen(filename,"rb");
    ssize_t size=0;

    fseek(fp1,0,SEEK_END);
    size=ftell(fp1);
    fclose(fp1);

    return size;

}

void sendlist()
{

    /* setting up the path */
    char path[30];
    strcpy(path, "/home/");
    strcpy(path, strcat(path, getenv("USER")));
    strcpy(path, strcat(path, "/My_Torrent/Server"));

    puts(path);
    struct dirent *dt;
    DIR *dir = opendir(path);
    if (dir != NULL)
    {
        printf("Directory opened.\n");

        char filename[1000][100];
        strcpy(filename[0], "STARTOFFILES");
        int file_num = 1;

        
        while ((dt = readdir(dir)) != NULL)
        {
            strcpy(filename[file_num], dt->d_name);
            if (strcmp(filename[file_num], ".") != 0 && strcmp(filename[file_num], "..") != 0 && filename[file_num][0]!='.')
            {
              
                file_num++;
            }
        }
        printf("\n");
        strcpy(filename[file_num], "ENDOFFILE");
        
        file_num--;
        send(client_socket, &file_num , sizeof(file_num), 0);

        for (int i = 1; i <= file_num; i++)
        {
            printf("filename[%d] is %s\n",i,filename[i]);
             send(client_socket, &filename[i], sizeof(filename[i]), 0);
        }
    }
}

void open_file_and_send()
{
    printf("\nWaiting for file name...\n");
    clearBuf(buffer);
    recv(client_socket, buffer, SIZE, 0);

    ssize_t filesize=size_of_a_file(buffer);
    send(client_socket,&filesize,sizeof(filesize),0);

    fp = fopen(buffer, "rb");
    printf("\nFile Name Received: %s\n", buffer);

    int n;
    ssize_t total = 0;
    char sendline[SIZE] = {0};
    while (((n = fread(sendline, sizeof(char), SIZE, fp)) > 0) && filesize!=total)
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

void download_file()
{
    recv(client_socket, buffer, SIZE, 0);
    ssize_t filesize;
    recv(client_socket,&filesize,sizeof(filesize),0);

    if ((fp = fopen(buffer, "wb")) != NULL)
    {
        printf("File Creation successful");
    }
    else
    {
        printf("File Creation Unsuccessful");
    }

    ssize_t n, total=0;
    char buff[SIZE] = {0};
    while (filesize!=total)
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

int main(int argc,char* argv[])
{
    system("clear");
    server_creation(AF_INET, SOCK_STREAM, IPPROTO_TCP, PORT, inet_addr("192.168.0.7"), MAX_CLIENT);
    client_socket = accept(server_socket, (struct sockaddr *)&server_address, &address_length);

     while (1)
    {
        printf("Server ready to recieve data:-\n");
        char command[2];
        recv(client_socket, &command, sizeof(command), 0);
        if (strncmp(command, "d", 1) == 0)
        {
            sendlist();
            open_file_and_send();
        }
        else if (strncmp(command, "u", 1) == 0)
        {
            download_file();
        }
        else  if (strncmp(command, "q", 1) == 0) break;
    }
    close(server_socket);   
    return 0;
}

