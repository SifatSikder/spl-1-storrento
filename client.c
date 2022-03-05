#include "unp.h"
#include <errno.h>
#include <dirent.h>

// storing the id number, ip address, number of files and the list of files of a client
struct client
{
    int id;
    char ip_address[20];
    int file_num;
    char *name;
    char list[1000][100];
};
/* sending the list of files that the client can share to the server when it is connected */
void sendlist(int *fd)
{

    /* sending the client ip to the server */
    int myfd = *fd;
    char s[17];
    FILE *f = fopen("./ipaddress", "r");
    fscanf(f, "%s", s);
    printf("Client's IP Address is %s\n", s);
    send(myfd, s, sizeof(s), 0);

    /* setting up the path */
    char path[30];
    strcpy(path, "/home/");
    strcpy(path, strcat(path, getenv("USER")));
    strcpy(path, strcat(path, "/Desktop/"));

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
            if (strcmp(filename[file_num], ".") != 0 && strcmp(filename[file_num], "..") != 0)
            {
              
                file_num++;
            }
        }
        printf("\n");
        strcpy(filename[file_num], "ENDOFFILE");
        send(myfd, &file_num, sizeof(file_num), 0);

        for (int i = 1; i < file_num; i++)
        {
            send(myfd, &filename[i], sizeof(filename[i]), 0);
        }
    }
}

void printlist(int *fd)
{
    int c;
    int process_socket = *fd;
    char handshake[15];
    struct client clients;

    printf("Loading List from the Server. \nPlease Wait..\n");

    if ( recv(process_socket, &handshake, sizeof(handshake), 0)<0)
    {
        printf("Handshaking Failed\n");
    }

    if (recv(process_socket, &c, sizeof(int), 0) < 0)
    {
        printf("Total Client Not recieved\n");
    }

     if (recv(process_socket, &clients, sizeof(struct client), 0) < 0)
    {
        printf("Total Client Not recieved\n");
    }
    for (int  i = 0; i < 6; i++)
    {
        printf("%s\n",clients.list[i]);
    }
}

int main(void)
{
    /* initializing server address */
    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8080);

    /* creating a client socket */
    int client_socket;
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        printf("Socket creation Unsuccessful");
    }
    /* connecting to the server  */
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        printf("Connection Failed");
        exit(-1);
    }
    printf("\nConnection Established !!\n");

    sendlist(&client_socket);
    char answer;
    int dreturn;
    while (1)
    {
        printf("Enter choice:\n'l' to get list of files\n'd' to download file\n'q' to quit\n>");
        scanf("%c", &answer);
        getchar();

        if (answer == 'l')
        {
            printf("\nList Request Sent to the Server.\n");
            send(client_socket,&answer,sizeof(answer),0);
            printlist(&client_socket);
        }
        else if (answer == 'd')
        {
            printf("\nDownload Request Sent to the Server.\n");
            /* dreturn=download(&client_socket);  */
        }
        else if (answer == 'q')
        {
            char c[2];
            c[0] = 'q';
            c[1] = '\0';
            write(client_socket, (const void *)c, 2);
            exit(0);
        }

        else
        {
            printf("Invalid choice. Please enter your choice again\n");
            fflush(stdin);
        }
    }

    return 0;
}