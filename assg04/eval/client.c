#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

void func_commun(int sockfd) {
	char name[512];
	printf("Enter your name: ");
	fgets(name, 512, stdin);
	name[strlen(name) - 1] = ':';
	char buff[512];
	int n;
	char password[512];
	printf("enter password: ");
	fgets(password, 512, stdin);
	write(sockfd, password, sizeof(password));
	n = read(sockfd, buff, sizeof(buff));
	if(strcmp(buff, "Wrong password") == 0) {
		printf("Wrong password\n");
		exit(0);
	}
	write(sockfd, name, sizeof(name));
	while(1) {
		bzero(buff, 512);
		printf("[above][@client]Enter your message: ");
		fgets(buff, 512, stdin);
		//make a string to send that concatinates name and buff
		char send_buff[512];
		bzero(send_buff, 512);
		strcpy(send_buff, name);
		strcat(send_buff, buff);
		n = write(sockfd, send_buff,512);
		if(strcmp(buff, "bye\n") == 0) {
			//send the name of the person who is exiting
			bzero(buff, 512);
			strcpy(send_buff, name);
			write(sockfd, send_buff, 512);
			break;
		}
		else if (strcmp(buff, "show\n") == 0) {
			//we will have to read the messages from the server and print them till we get a string "end_of_reading_file"
			printf("--------WELCOME TO COBRA--------\n");
			char c;
			while(1) {
				bzero(buff, 512);
				n = read(sockfd, buff, 512);
				if(n < 0) {
					printf("Error reading from socket\n");
					exit(1);
				}
				if(strcmp(buff, "end_of_reading_file") == 0) {
					break;
				}
				else {
					printf("%s", buff);
				}
			}
			printf("--------end of messages-----------\n");
			bzero(buff, 512);
		}
		else {
			//we will have to read the messages from the server and print them till we get a string "end_of_reading_file"
			printf("--------WELCOME TO COBRA--------\n");
			char c;
			while(1) {
				bzero(buff, 512);
				n = read(sockfd, buff, 512);
				if(n < 0) {
					printf("Error reading from socket\n");
					exit(1);
				}
				if(strcmp(buff, "end_of_reading_file") == 0) {
					break;
				}
				else {
					printf("%s", buff);
				}
			}
			printf("--------end of messages-----------\n");
			bzero(buff, 512);
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


