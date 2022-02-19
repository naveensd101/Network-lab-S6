#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

void func_commun(int connfd)
{
	char buffer[256];
	int n;
	while(1)
	{
		bzero(buffer,256);
		n = read(connfd,buffer,255);
		if(n < 0)
		{
			perror("ERROR reading from socket");
			exit(1);
		}
		printf("From client: %s",buffer);
		bzero(buffer,256);
		fgets(buffer,255,stdin);
		n = write(connfd,buffer,strlen(buffer));
		break;
		if(n < 0)
		{
			perror("ERROR writing to socket");
			exit(1);
		}
		if(strncmp(buffer,"exit",4) == 0)
		{
			printf("Client disconnected\n");
			break;
		}
	}
}
int main()
{
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		printf("Socket creation failed\n");
		exit(1);
	}
	else
		printf("Socket successfully created\n");
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(5000);

	if(bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0)
	{
		printf("Socket bind failed\n");
		exit(1);
	}
	else
		printf("Socket successfully binded\n");

	if(listen(sockfd, 5) != 0)
	{
		printf("Listen failed\n");
		exit(1);
	}
	else
		printf("Server listening\n");
	len = sizeof(cli);
	connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
	if(connfd < 0)
	{
		printf("Accept failed\n");
		exit(1);
	}
	else
		printf("Client connected\n");
	func_commun(connfd);
	close(sockfd);
}
