//create a server that can be used to send and receive files using TCP
//also there should be user authentication while connection starts
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

const int SIZE = 1024;
const int PORT = 4035;

//states of the client
int AUTHENTICATED = 0;

void communicate_with_client(int sockfd) {
	char buffer[SIZE];
	int n;
	while (1) {
		bzero(buffer, SIZE);
		read(sockfd, buffer, SIZE);
		printf("[From Client]%s", buffer);
		if(strcmp(buffer, "exit\n") == 0) {
			printf("Client disconnected\n");
			break;
		}
		else if(strcmp(buffer, "START\n") == 0) {
			printf("Client connected\n");
			//send 200 OK to client
			strcpy(buffer, "200 OK\n");
			write(sockfd, buffer, SIZE);
		}
		else if(strcmp(buffer, "USERN\n") == 0) {
			//get username from the client and then the password then check with logincred.txt
			char username[SIZE];
			char password[SIZE];
			bzero(username, SIZE);
			bzero(password, SIZE);
			read(sockfd, username, SIZE);
			read(sockfd, password, SIZE);
			username[strlen(username)-1] = '\0';
			password[strlen(password)-1] = '\0';
			int correct_user = 0;
			int correct_pass = 0;
			//check with logincred.txt
			char line[SIZE];
			char name[SIZE], pass[SIZE];
			FILE *fp = fopen("logincred.txt", "r");
			while(fscanf(fp, "%s", line) != EOF) {
				int comma = 0;
				for(int i = 0; i < strlen(line); i++) {
					name[i] = line[i];
					if(line[i] == ',') {
						comma = i;
						name[i] = '\0';
						break;
					}
				}
				for(int i = comma + 1; i < strlen(line); i++) {
					pass[i - comma - 1] = line[i];
				}
				pass[strlen(line) - comma - 1] = '\0';
				if(strcmp(name, username) == 0) {
					correct_user = 1;
					if(strcmp(pass, password) == 0) {
						correct_pass = 1;
					}
					break;
				}
			}
			fclose(fp);
			if(correct_user == 1 && correct_pass == 1) {
				strcpy(buffer, "305 user authentication done\n");
				strcat(buffer, "Welcome ");
				strcat(buffer, username);
				strcat(buffer, "\n");
				printf("user authentication done\n");
				write(sockfd, buffer, SIZE);
				AUTHENTICATED = 1;
			}
			else if(correct_user == 1 && correct_pass == 0) {
				strcpy(buffer, "300 and 310 correct username but incorrect password need password\n");
				printf("correct username but incorrect password need password\n");
				write(sockfd, buffer, SIZE);
			}
			else if(correct_user == 0) {
				strcpy(buffer, "301 incorrect username\n");
				printf("incorrect username\n");
				write(sockfd, buffer, SIZE);
			}
		}
		else if(strcmp(buffer, "CreateFile\n") == 0) {
			if(AUTHENTICATED == 0) {
				bzero(buffer, SIZE);
				strcpy(buffer, "First get authenticated\n");
				write(sockfd, buffer, SIZE);
			}
			else {
				bzero(buffer, SIZE);
				strcpy(buffer, "ok we can move on to create the file\n");
				printf("ok we can move on to create the file\n");
				write(sockfd, buffer, SIZE);
				//get the file name from the client
				bzero(buffer, SIZE);
				read(sockfd, buffer, SIZE);
				buffer[strlen(buffer)-1] = '\0';
				//create the file
				FILE *fp = fopen(buffer, "w");
				fclose(fp);
				//append the file name to the filesystem.txt
				fp = fopen("filesystem.txt", "a");
				fprintf(fp, "%s\n", buffer);
				fclose(fp);
				//send message to client that file is created
				bzero(buffer, SIZE);
				strcpy(buffer, "File created\n");
				write(sockfd, buffer, SIZE);
				printf("File created\n");
			}
		}
		else if(strcmp(buffer, "ListDir\n") == 0) {
			if(AUTHENTICATED == 0) {
				bzero(buffer, SIZE);
				strcpy(buffer, "First get authenticated\n");
				printf("First get authenticated\n");
				write(sockfd, buffer, SIZE);
			}
			else {
				bzero(buffer, SIZE);
				strcpy(buffer, "ok we can move on to list the directory\n");
				printf("ok we can move on to list the directory\n");
				write(sockfd, buffer, SIZE);
				//print the contenst of the file filesystem.txt
				FILE *fp = fopen("filesystem.txt", "r");
				char line[SIZE];
				while(fscanf(fp, "%s", line) != EOF) {
					bzero(buffer, SIZE);
					strcpy(buffer, line);
					write(sockfd, buffer, SIZE);
				}
				fclose(fp);
				bzero(buffer, SIZE);
				strcpy(buffer, "end of directory\n");
				write(sockfd, buffer, SIZE);
			}
		}
		else if(strcmp(buffer, "StoreFile\n") == 0) {
			if(AUTHENTICATED == 0) {
				bzero(buffer, SIZE);
				strcpy(buffer, "First get authenticated\n");
				printf("First get authenticated\n");
				write(sockfd, buffer, SIZE);
			}
			else {
				bzero(buffer, SIZE);
				strcpy(buffer, "ok we can move on to store the file\n");
				printf("ok we can move on to store the file\n");
				write(sockfd, buffer, SIZE);
				//get the file name from the client
				bzero(buffer, SIZE);
				read(sockfd, buffer, SIZE);
				buffer[strlen(buffer)-1] = '\0';
				//add this file name to the filesystem.txt
				FILE *fp = fopen("filesystem.txt", "a");
				fprintf(fp, "%s\n", buffer);
				fclose(fp);
				//open a file with the file name
				fp = fopen(buffer, "wb");
				//get the file contents from the client
				bzero(buffer, SIZE);
				read(sockfd, buffer, SIZE);
				while(strcmp(buffer, "end of file\n") != 0) {
					fwrite(buffer, 1, SIZE, fp);
					bzero(buffer, SIZE);
					read(sockfd, buffer, SIZE);
				}
				fclose(fp);
				printf("file stored\n");
			}
		}
		else if(strcmp(buffer, "Del\n") == 0) {
			if(AUTHENTICATED == 0) {
				bzero(buffer, SIZE);
				strcpy(buffer, "First get authenticated\n");
				printf("First get authenticated\n");
				write(sockfd, buffer, SIZE);
			}
			else {
				bzero(buffer, SIZE);
				strcpy(buffer, "ok we can move on to delete the file\n");
				printf("ok we can move on to delete the file\n");
				write(sockfd, buffer, SIZE);
				//get the file name from the client
				bzero(buffer, SIZE);
				read(sockfd, buffer, SIZE);
				buffer[strlen(buffer)-1] = '\0';
				//delete the file name from the filesystem.txt
				FILE *fp = fopen("filesystem.txt", "r");
				FILE *tmp = fopen("tmp.txt", "w");
				char line[SIZE];
				while(fscanf(fp, "%s", line) != EOF) {
					if(strcmp(line, buffer) != 0) {
						fprintf(tmp, "%s\n", line);
					}
				}
				fclose(fp);
				fclose(tmp);
				remove("filesystem.txt");
				rename("tmp.txt", "filesystem.txt");
				printf("file deleted\n");
				bzero(buffer, SIZE);
				strcpy(buffer, "file deleted\n");
				write(sockfd, buffer, SIZE);
			}
		}
		else if(strcmp(buffer, "GetFile\n") == 0) {
			if(AUTHENTICATED == 0) {
				bzero(buffer, SIZE);
				strcpy(buffer, "First get authenticated\n");
				printf("First get authenticated\n");
				write(sockfd, buffer, SIZE);
			}
			else {
				bzero(buffer, SIZE);
				strcpy(buffer, "ok we can move on to get the file\n");
				printf("ok we can move on to get the file\n");
				write(sockfd, buffer, SIZE);

				//get the file name from the client
				bzero(buffer, SIZE);
				read(sockfd, buffer, SIZE);
				buffer[strlen(buffer)-1] = '\0';
				printf("file name is %s\n", buffer);
				//open the file
				FILE *fp = fopen(buffer, "rb");
				//send the file contents to the client
				bzero(buffer, SIZE);
				while(fread(buffer, 1, SIZE, fp) > 0) {
					write(sockfd, buffer, SIZE);
					bzero(buffer, SIZE);
				}
				fclose(fp);
				bzero(buffer, SIZE);
				strcpy(buffer, "end of file\n");
				write(sockfd, buffer, SIZE);
			}
		}
		else {
			bzero(buffer, SIZE);
			strcpy(buffer, "505 Command not found\n");
			write(sockfd, buffer, SIZE);
		}
	}
}

int main() {
	int sockfd, connfd, n;
	struct sockaddr_in servaddr, cliaddr;

	//create a socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1) {
		printf("Error in creating socket\n");
		exit(0);
	}
	else {
		printf("Socket created successfully\n");
	}

	//initialize the server address
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	//bind the socket to the server address
	if(bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
		printf("Error in binding the socket\n");
		close(sockfd);
		exit(0);
	}
	else {
		printf("Socket binded successfully\n");
	}

	//listen for connections
	if(listen(sockfd, 5) == -1) {
		printf("Error in listening\n");
		close(sockfd);
		exit(0);
	}
	else {
		printf("Listening for connections\n");
	}

	//accept connections
	int len = sizeof(cliaddr);
	connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
	if(connfd == -1) {
		printf("Error in accepting the connection\n");
		close(sockfd);
		exit(0);
	}
	else {
		printf("Connection accepted\n");
	}

	communicate_with_client(connfd);
	close(connfd);
	close(sockfd);
	return 0;
}


