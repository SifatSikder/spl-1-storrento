//This is going to parse 3 things from an url 
//1.protocol-such as http or udp 
//2.hostname - www.google.com
//2.port-port number of the connected server(80 for http)

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>

void url_protocol(char* url, char* protocol)
{
    int length_of_url = strlen(url);
    int protocol_end=0;

    for (int i = 0; i < length_of_url; i++)
    {
        if (url[i] == ':')
        {
            protocol_end = i;
            break;
        }
    }

    for (int i = 0,k=0; i < protocol_end; i++) protocol[k++] = url[i];
    
    //if protocol omitted then set default protocol to http
    if (protocol_end == 0|| protocol_end > 4 )
    {
        strcpy(protocol, "http");
    }
}

void url_hostname(char* url, char* host)
{

    int length_of_url = strlen(url);
    int domain_start = 0;
    int domain_end = length_of_url;

    for (int i = 0; i < length_of_url; i++)
    {
        if (i > 6 && (url[i] == '/' || url[i] == ':'))
        {
            domain_end = i;
            break;
        }

        if (url[i] == ':' && domain_start == 0)
        {
            domain_start = i+3;
            i += 3;
        }
    }

    for (int i = domain_start,k=0; i < domain_end; i++) host[k++] = url[i]; 
    
}

void url_port(char* url, int *port)
{
    int length_of_url = strlen(url);
    int port_start=0;
    int port_end = length_of_url;

    //this loop finds the starting and ending of the port in url
    for (int i = length_of_url; i > 0; i--)
    {
        if (url[i] == ':')
        {
            port_start = i;
            break;
        }

        if (url[i] < 48 || url[i] > 57)
        {
            port_end = i-1;
        }
    }

    //this loop converts the port string to port number
    int positional_value = 1;
    *port=0;
    for (int i = port_end; i > port_start; i--)
    {
        *port += (url[i]-48) *positional_value;
        positional_value *= 10;
    }

}

void testing(char* URL)
{
    int length_of_url = strlen(URL);
    printf("URL: %s\n", URL);

    //parsing the hostname
    char* hostname;
    hostname = (char*) malloc(length_of_url);
    url_hostname(URL, hostname);
    printf("hostname: %s\n", hostname);

    //parsing the protocol(if omitted then default is http)
    char* protocol;
    protocol = (char*) malloc(length_of_url);
    url_protocol(URL, protocol);
    printf("protocol: %s\n", protocol);

    ////parsing the portnumber
    int port ;
    url_port(URL, &port);
    printf("port: %d\n", port);
}

int main(int argc, char ** argv)
{

    testing("udp://www.siena.crazyhd.com:53/annonce");
    return 0;
}