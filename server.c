#include<sys/shm.h>
#include"unp.h"
#define MAX_CLIENT 5

//storing the id number, ip address, number of files and the list of files of a client
struct client{
    int id;
    char ip_address[20];
    int file_num;
    char *name;
    char list[1000][100];
};

int *download_status;
int client_number=0; 
struct client clients[MAX_CLIENT];
//Function to send the list of files shared by all the clients connected to the server.
void sendlist(int *fd,int total_client)
{
    int client_socket= *fd;
    char handshake[]="CLIENT_START";

    if (send(client_socket, &handshake, sizeof(handshake), 0)<0)
    {
        printf("Handshaking Failed\n");
    }
    
 
    if (send(client_socket, &total_client, sizeof(int), 0)<0)
    {
        printf("Total Client Number Not sent\n");
    }

    if (send(client_socket, &clients[0], sizeof(struct client), 0)<0)
    {
       printf("Total Client Not sent\n");
    }
    
  
    


    

  
    printf("List sent.\n");

}


int server_socket;
struct sockaddr_in server_address;
void server_creation(int domain, int type, int protocol,int port, u_int32_t internet_address, int max_client){
    /* initializing server socket */
    server_socket=socket(domain,type,protocol);
    if (server_socket<0)
    {
        printf("Socket creation Unsuccessful");
    }

    /* initializing server address */
    bzero(&server_address,sizeof(server_address));
    server_address.sin_family=domain;
    server_address.sin_port=htons(port);
    server_address.sin_addr.s_addr=internet_address;

    /* binding the address to the socket */
    if (bind(server_socket,(struct sockaddr *)&server_address,sizeof(server_address))<0)
    {
        printf("Binding Unsuccessful");
    }

    /* server in the listen mode */
    if (listen(server_socket,max_client)<0)
    {
        printf("Listen Unsuccessful");
    }
    printf("\nServer Ready\n");
}

int main(void){
    
    server_creation(AF_INET,SOCK_STREAM,0,8080,INADDR_ANY,MAX_CLIENT);
    while (1)
    {
        /* establishing a connection via accept funtion */
        int client_socket;
        printf("Waiting for clients ..\n\n");
        client_socket=accept(server_socket,NULL,NULL);
        printf("Connection Established.\n");
        client_number++;


        /* storing the client information such as clients id,file number, list of the shared files */
        clients[client_number-1].id=client_number;
    
        char ip[17];
        int return_value=recv(client_socket,&ip,sizeof(ip),0);
        if(return_value==-1) {printf("read error.\n");} 
        strncpy(clients[client_number-1].ip_address,ip,20); 
        printf("IP Address of Client %d connected is %s\n",clients[client_number-1].id,clients[client_number-1].ip_address);

        /* recieve list of files that a client want to share */
        recv(client_socket,&clients[client_number-1].file_num,sizeof(clients[client_number-1].file_num),0);
        printf("List of files present in client %d are:-\n",clients[client_number-1].id);
         for (int i = 1; i < clients[client_number-1].file_num; i++)
         {
            recv(client_socket,&clients[client_number-1].list[i-1],sizeof(clients[client_number-1].list[i-1]),0);
            printf("%s\n",clients[client_number-1].list[i-1]);
         }

    
        /* recieve a request of a particular client  */
        char request;
        recv(client_socket,&request,sizeof(request),0);
        switch (request)
        {
        case 'l':
            sendlist(&client_socket,client_number);
            break;
        
        default:
            break;
        }
        
    }

    
    

    
}