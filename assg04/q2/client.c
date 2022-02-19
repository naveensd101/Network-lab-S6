#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void func_commun(int sockfd) {
	char buff[256];
	int n;
	while(1) {
		bzero(buff, 256);
		printf("[above][@client]Enter the math binary expression: ");
		fgets(buff, 255, stdin);
		n = write(sockfd, buff, strlen(buff));
		if(strcmp(buff, "exit\n") == 0) {
			break;
		}
		else {
			int result;
			n = read(sockfd, &result, sizeof(int));
			printf("[below][@client]The result is: %d\n", result);
		}
	}
}
int main() {
	int sockfd, connfd;
	struct sockaddr_in servaddr, cliaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) {
		printf("socket creation error\n");
		exit(1);
	}
	else {
		printf("socket successfully created\n");
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(5000);

	if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		printf("connection with the server failed\n");
		exit(1);
	}
	else {
		printf("connected to the server\n");
	}

	func_commun(sockfd);
	close(sockfd);
	close(connfd);
	return 0;
}
