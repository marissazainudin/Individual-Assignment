#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#define MAX 1000
#define PORT 1717
#define STAD struct sockaddr

void func(int sock_desc)
{
	char buff[MAX] ;
	int a ;
	bzero(buff, sizeof(buff)) ;
	printf("Send your message to server: ") ;
	a = 0 ;
	if((buff[a++] = getchar()) != '\n')
	{
		bzero(buff, sizeof(buff)) ;
		read(sock_desc, buff, sizeof(buff)) ;
		printf("\nFrom server : %s\n", buff) ;
	}
}

int main()
{
	int sock_desc, connfd ;
	struct sockaddr_in servaddr, client ;

	sock_desc = socket(AF_INET, SOCK_STREAM, 0) ;
	if (sock_desc == -1) 
	{
		printf("Fail to create socket \n") ;
		exit(0) ;
	}
        //Assigning IP and PORT
	servaddr.sin_family = AF_INET ;
	servaddr.sin_addr.s_addr = inet_addr("192.168.56.105") ;
	servaddr.sin_port = htons(PORT) ;
        
        //to connect client socket to server socket
	if (connect(sock_desc, (STAD*)&servaddr, sizeof(servaddr)) != 0) 
	{
		printf("Connection to server: failed \n") ;
		exit(0) ;
	}
        else
	{
		printf("Connection to server: successful \n") ;
	}

        func(sock_desc) ;
	close(sock_desc) ; // socket close
}
