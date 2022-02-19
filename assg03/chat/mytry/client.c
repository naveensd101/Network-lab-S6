#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

void func_commun(int sockfd)
{
	char buff[256];
	int n;
	while(1)
	{
		bzero(buff,256);
		printf("Enter the message to send to the sever : ");
		n = 0;
		while ((buff[n++] = getchar()) != '\n');
		write(sockfd,buff,n);
		bzero(buff,256);
		n = read(sockfd,buff,255);
		printf("Message from server : %s",buff);
		break;
		if(strcmp(buff,"exit") == 0)
		{
			printf("\nClient exiting...\n");
			break;
		}
	}
}
int main()
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cliaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		printf("socket creation error\n");
		exit(1);
	}
	else
	{
		printf("socket successfully created\n");
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(5000);

	if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("connection with the server failed\n");
		exit(1);
	}
	else
	{
		printf("connected to the server\n");
	}

	func_commun(sockfd);
	close(sockfd);
	return 0;
}
