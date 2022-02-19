#include <stdio.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void func_commun(int connfd) {
	char buffer[256];
	int n;
	while(1) {
		bzero(buffer,256);
		n = read(connfd,buffer,256);
		if(n < 0) {
			perror("ERROR reading from socket");
			exit(1);
		}
		printf("%s",buffer);
		if(strcmp(buffer,"exit\n") == 0) {
			break;
		}
		else {
			printf("%s", buffer);
			int operator_pos = 0;
			char operator;
			for(int i = 1; i < sizeof(buffer); i++) {
				if(buffer[i] == '+' || buffer[i] == '-' || buffer[i] == '*' || buffer[i] == '/') {
					operator_pos = i;
					operator = buffer[i];
					break;
				}
			}
			int num1, num2;
			char num_char[20];
			int j = 0;
			for(int i = 0; i < operator_pos; i++) num_char[j++] = buffer[i];
			num_char[j] = '\0';
			num1 = atoi(num_char);
			j = 0;
			for(int i = operator_pos + 1; i < sizeof(buffer); i++) num_char[j++] = buffer[i];
			num_char[j] = '\0';
			num2 = atoi(num_char);
			printf("%d %d\n", num1, num2);
			int result;
			switch(operator) {
				case '+':
					result = num1 + num2;
					break;
				case '-':
					result = num1 - num2;
					break;
				case '*':
					result = num1 * num2;
					break;
				case '/':
					if(num2 != 0)
					result = num1 / num2;
					else
					result = INT_MAX;
					break;
				default:
					printf("Invalid operator\n");
					result = INT_MIN;
			}
			printf("result: %d\n", result);
			write(connfd, &result, sizeof(result));
		}
	}
}
int main() {
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1) {
		printf("Socket creation failed\n");
		exit(1);
	}
	else
		printf("Socket successfully created\n");
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(5000);

	if(bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
		printf("Socket bind failed\n");
		exit(1);
	}
	else
		printf("Socket successfully binded\n");

	if(listen(sockfd, 5) != 0) {
		printf("Listen failed\n");
		exit(1);
	}
	else
		printf("Server listening\n");
	len = sizeof(cli);

	//connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
	//if(connfd < 0) {
	//	printf("Accept failed\n");
	//	exit(1);
	//}
	//else
	//	printf("Client connected\n");
	//func_commun(connfd);
	//close(sockfd);
	//close(connfd);
	while(1) {
		connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
		if(connfd < 0) {
			printf("Accept failed\n");
			exit(1);
		}
		printf("Client connected\n");
		printf("Client is connected at port %d\n", ntohs(cli.sin_port));

		if(fork() == 0) {
			func_commun(connfd);
			close(sockfd);
			close(connfd);
			exit(0);
		}
		close(connfd);
	}
	return 0;
}

