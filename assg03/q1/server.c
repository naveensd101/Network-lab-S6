#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void func_commun(int connfd) {
	//storing fruits in a array {apple, mango, banana, chikoo, papaya}
	int fruits_buffer[] = {5, 5, 5, 5, 5};
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
		else if(strcmp(buffer, "SendInventory\n") == 0) {
			write(connfd, fruits_buffer, sizeof(fruits_buffer));
		}
		else if(strcmp(buffer, "Fruits\n") == 0) {
			strcpy(buffer, "[@server is asking]Enter the name of the fruit : ");
			write(connfd, buffer, sizeof(buffer));
			read(connfd, buffer, sizeof(buffer));
			printf("%s", buffer);
			int space_idx = 0;
			for(int i = 0; i < sizeof(buffer); i++) if(buffer[i] == ' ') {
				space_idx = i;break;
			}
			//fruit number\0
			//     i
			//[0,i-1] has the frutis name
			//[i+1, sizeof-1] has the number
			char fruit_name[256], number[10];
			for(int i = 0; i < space_idx; ++i) {
				fruit_name[i] = buffer[i];
			}
			fruit_name[space_idx] = '\0';
			int j = 0;
			for(int i = space_idx+1; i < sizeof(buffer); i++) {
				number[j++] = buffer[i];
			}
			number[j] = '\0';
			int number_as_int = atoi(number);
			printf("[After analysis of client message] fruit = %s and number = %d\n", fruit_name, number_as_int);

			if(strcmp(fruit_name, "apple") == 0) {
				if(fruits_buffer[0] < number_as_int) {
					strcpy(buffer, "[@sever message]Not available\n");
					write(connfd, buffer, sizeof(buffer));
				}
				else {
					fruits_buffer[0] -= number_as_int;
					strcpy(buffer, "[@sever message]Reduction successfull\n");
					write(connfd, buffer, sizeof(buffer));
				}
			}
			else if(strcmp(fruit_name, "mango") == 0) {
				if(fruits_buffer[1] < number_as_int) {
					strcpy(buffer, "[@sever message]Not available\n");
					write(connfd, buffer, sizeof(buffer));
				}
				else {
					fruits_buffer[1] -= number_as_int;
					strcpy(buffer, "[@sever message]Reduction successfull\n");
					write(connfd, buffer, sizeof(buffer));
				}
			}
			else if(strcmp(fruit_name, "banana") == 0) {
				if(fruits_buffer[2] < number_as_int) {
					strcpy(buffer, "[@sever message]Not available\n");
					write(connfd, buffer, sizeof(buffer));
				}
				else {
					fruits_buffer[2] -= number_as_int;
					strcpy(buffer, "[@sever message]Reduction successfull\n");
					write(connfd, buffer, sizeof(buffer));
				}
			}
			else if(strcmp(fruit_name, "chikoo") == 0) {
				if(fruits_buffer[3] < number_as_int) {
					strcpy(buffer, "[@sever message]Not available\n");
					write(connfd, buffer, sizeof(buffer));
				}
				else {
					fruits_buffer[3] -= number_as_int;
					strcpy(buffer, "[@sever message]Reduction successfull\n");
					write(connfd, buffer, sizeof(buffer));
				}
			}
			else if(strcmp(fruit_name, "papaya") == 0) {
				if(fruits_buffer[4] < number_as_int) {
					strcpy(buffer, "[@sever message]Not available\n");
					write(connfd, buffer, sizeof(buffer));
				}
				else {
					fruits_buffer[4] -= number_as_int;
					strcpy(buffer, "[@sever message]Reduction successfull\n");
					write(connfd, buffer, sizeof(buffer));
				}
			}
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
	connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
	if(connfd < 0) {
		printf("Accept failed\n");
		exit(1);
	}
	else
		printf("Client connected\n");
	func_commun(connfd);
	connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
	if(connfd < 0) {
		printf("2ndAccept failed\n");
		exit(1);
	}
	else {
		printf("2ndClient connected\n");
		func_commun(connfd);
	}
	close(sockfd);
}

