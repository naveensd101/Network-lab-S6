//create a client that can send and receive files using TCP
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

const int SIZE = 1024;
const int PORT = 4035;
int start = 0;

void command_handler(int sockfd, char *command) {
	//we have send command string to the server and now we have to handle the response
	char buffer[SIZE];
	if(strcmp(command, "START\n") == 0) {
		bzero(buffer, SIZE);
		read(sockfd, buffer, SIZE);
		printf("%s", buffer);
		if(strcmp(buffer, "START\n") == 0) {
			printf("Server is ready to receive file\n");
			start = 1;
		}
	}
	else if(strcmp(command, "USERN\n") == 0) {
		//ask for username
		printf("[@Client]Enter username: ");
		bzero(buffer, SIZE);
		fgets(buffer, SIZE, stdin);
		write(sockfd, buffer, SIZE);
		printf("[@Client]Enter PASSWD: ");
		bzero(buffer, SIZE);
		fgets(buffer, SIZE, stdin);
		write(sockfd, buffer, SIZE);
		bzero(buffer, SIZE);
		read(sockfd, buffer, SIZE);
		printf("%s", buffer);
	}
	else if(strcmp(command, "CreateFile\n") == 0) {
		bzero(buffer, SIZE);
		read(sockfd, buffer, SIZE);
		if(strcmp(buffer, "First get authenticated\n") == 0) {
			printf("You have to user USERN command first to get authenticated\n");
		}
		else {
			printf("%s", buffer);
			printf("[@client]Enter file name: ");
			bzero(buffer, SIZE);
			fgets(buffer, SIZE, stdin);
			write(sockfd, buffer, SIZE);
			bzero(buffer, SIZE);
			read(sockfd, buffer, SIZE);
			printf("%s", buffer);
		}
	}
	else if(strcmp(command, "ListDir\n") == 0) {
		bzero(buffer, SIZE);
		read(sockfd, buffer, SIZE);
		if(strcmp(buffer, "First get authenticated\n") == 0) {
			printf("You have to user USERN command first to get authenticated\n");
		}
		else {
			printf("%s", buffer);
			printf("Following files are available in the server:\n\n");
			//read from socket till you get the string "end of directory\n"
			do {
				bzero(buffer, SIZE);
				read(sockfd, buffer, SIZE);
				printf("%s\n", buffer);
			} while(strcmp(buffer, "end of directory\n") != 0);
		}
	}
	else if(strcmp(command, "StoreFile\n") == 0) {
		bzero(buffer, SIZE);
		read(sockfd, buffer, SIZE);
		if(strcmp(buffer, "First get authenticated\n") == 0) {
			printf("You have to user USERN command first to get authenticated\n");
		}
		else {
			printf("%s", buffer);
			//send a file present in the client to the server
			printf("[@client]Enter file name: ");
			bzero(buffer, SIZE);
			fgets(buffer, SIZE, stdin);
			write(sockfd, buffer, SIZE);
			buffer[strlen(buffer) - 1] = '\0';
			//send the file to the server
			FILE *fp = fopen(buffer, "rb");
			double time_taken = 0.0;
			clock_t start_time = clock();
			bzero(buffer, SIZE);
			while(feof(fp) == 0) {
				fread(buffer, sizeof(char), SIZE, fp);
				write(sockfd, buffer, SIZE);
				bzero(buffer, SIZE);
			}
			clock_t end_time = clock();
			fclose(fp);
			bzero(buffer, SIZE);
			strcpy(buffer, "end of file\n");
			write(sockfd, buffer, SIZE);
			bzero(buffer, SIZE);
			printf("File sent successfully\n");
			time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;
			printf("Time taken to send file: %f\n", time_taken);
		}
	}
	else if(strcmp(command, "Del\n") == 0) {
		bzero(buffer, SIZE);
		read(sockfd, buffer, SIZE);
		printf("%s", buffer);
		if(strcmp(buffer, "First get authenticated\n") == 0) {
			printf("You have to user USERN command first to get authenticated\n");
		}
		else {
			printf("[@client]Enter file name: ");
			bzero(buffer, SIZE);
			fgets(buffer, SIZE, stdin);
			write(sockfd, buffer, SIZE);
			bzero(buffer, SIZE);
			read(sockfd, buffer, SIZE);
			printf("%s", buffer);
		}
	}
	else if(strcmp(command, "GetFile\n") == 0) {
		bzero(buffer, SIZE);
		read(sockfd, buffer, SIZE);
		if(strcmp(buffer, "First get authenticated\n") == 0) {
			printf("You have to user USERN command first to get authenticated\n");
		}
		else {
			printf("%s", buffer);
			//send the file name to the server
			printf("[@client]Enter file name: ");
			char file_name[SIZE];
			bzero(buffer, SIZE);
			bzero(file_name, SIZE);
			fgets(file_name, SIZE, stdin);
			write(sockfd, file_name, SIZE);
			file_name[strlen(file_name) - 1] = '\0';
			//receive the file from the server
			FILE *fp = fopen(file_name, "wb");
			double time_taken = 0.0;
			clock_t start_time = clock();
			bzero(buffer, SIZE);
			while(strcmp(buffer, "end of file\n") != 0) {
				bzero(buffer, SIZE);
				read(sockfd, buffer, SIZE);
				if(strcmp(buffer, "end of file\n") != 0) {
					fwrite(buffer, sizeof(char), SIZE, fp);
				}
			}
			clock_t end_time = clock();
			fclose(fp);
			bzero(buffer, SIZE);
			printf("File received successfully\n");
			time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;
			printf("Time taken to receive file: %f\n", time_taken);
		}
	}
	else {
		bzero(buffer, SIZE);
		read(sockfd, buffer, SIZE);
		printf("%s", buffer);
	}
}
void communicate_with_server(int sockfd) {
	char buffer[SIZE];
	int n;
	while (1) {
		bzero(buffer, SIZE);
		printf("[@Client]Enter a command: ");
		fgets(buffer, SIZE, stdin);
		n = write(sockfd, buffer, 1024);
		if(strcmp(buffer, "exit\n") == 0) {
			printf("Exiting...\n");
			break;
		}
		command_handler(sockfd, buffer);
	}
}

int main() {
	int sockfd;
	struct sockaddr_in serv_addr;

	//create a socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		printf("Error opening socket\n");
		exit(1);
	}
	else {
		printf("Socket opened\n");
	}

	//set up the server address
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	//connect to the server
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		printf("Error connecting to server\n");
		exit(1);
	}
	else {
		printf("Connected to server\n");
	}

	communicate_with_server(sockfd);
	close(sockfd);
	return 0;
}
