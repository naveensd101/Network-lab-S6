//create a SMTP server

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

//states of the client
int AUTHENTICATED = 0;

void communicate_with_client(int sockfd) {
	char buffer[SIZE];
	int n;
	while(1) {
		bzero(buffer, SIZE);
		read(sockfd, buffer, SIZE);
		printf("[From Client]%s", buffer);
		if(strcmp(buffer, "quit\n") == 0) {
			bzero(buffer, SIZE);
			strcpy(buffer, "Good Bye from SMTP!!\n");
			write(sockfd, buffer, SIZE);
			printf("Client has quit\n");
			return;
		}
		else if(strcmp(buffer, "auth\n") == 0) {
			//get username and password from client
			//check if username and password are correct
			//cross check with the userlogincred.txt file
			//format of the file is username password
			char username[SIZE];
			char password[SIZE];
			bzero(username, SIZE);
			bzero(password, SIZE);
			read(sockfd, username, SIZE);
			read(sockfd, password, SIZE);
			username[strlen(username)-1] = '\0';
			password[strlen(password)-1] = '\0';

			char name[SIZE], pass[SIZE];
			FILE *fp = fopen("userlogincred.txt", "r");
			while(fscanf(fp, "%s", name) != EOF) {
				fscanf(fp, "%s", pass);
				if(strcmp(name, username) == 0 && strcmp(pass, password) == 0) {
					AUTHENTICATED = 1;
				}
				bzero(name, SIZE);
				bzero(pass, SIZE);
			}
			fclose(fp);
			if(AUTHENTICATED) {
				bzero(buffer, SIZE);
				strcpy(buffer, "Authenticated\n");
				printf("Authenticated by the user %s\n", username);
				write(sockfd, buffer, SIZE);
			}
			else {
				bzero(buffer, SIZE);
				strcpy(buffer, "Not Authenticated\n");
				printf("Not Authenticated by the user %s\n", username);
				write(sockfd, buffer, SIZE);
			}
		}
		else if(strcmp(buffer, "SendMail\n") == 0) {
			//ask for user id and password
			char username[SIZE];
			char password[SIZE];
			bzero(username, SIZE);
			bzero(password, SIZE);
			read(sockfd, username, SIZE);
			read(sockfd, password, SIZE);
			username[strlen(username)-1] = '\0';
			password[strlen(password)-1] = '\0';

			char name[SIZE], pass[SIZE];
			FILE *fp = fopen("userlogincred.txt", "r");
			int auth = 0;
			while(fscanf(fp, "%s", name) != EOF) {
				fscanf(fp, "%s", pass);
				if(strcmp(name, username) == 0 && strcmp(pass, password) == 0) {
					auth = 1;
				}
				bzero(name, SIZE);
				bzero(pass, SIZE);
			}
			fclose(fp);
			if(auth) {
				bzero(buffer, SIZE);
				strcpy(buffer, "Authenticated\n");
				printf("Authenticated by the user %s\n", username);
				write(sockfd, buffer, SIZE);
			}
			else {
				bzero(buffer, SIZE);
				strcpy(buffer, "Not Authenticated\n");
				printf("Not Authenticated by the user %s\n", username);
				write(sockfd, buffer, SIZE);
				continue;
			}
			char toaddr[SIZE];
			bzero(toaddr, SIZE);
			read(sockfd, toaddr, SIZE);
			toaddr[strlen(toaddr)-1] = '\0';
			char tousr[SIZE];
			for(int i = 0; i < strlen(toaddr); i++) {
				if(toaddr[i] == '@') {
					tousr[i] = '\0';
					break;
				}
				tousr[i] = toaddr[i];
			}
			char fromaddr[SIZE];
			bzero(fromaddr, SIZE);
			strcpy(fromaddr, username);
			strcat(fromaddr, "@localhost");
			char subject[SIZE];
			bzero(subject, SIZE);
			read(sockfd, subject, SIZE);
			subject[strlen(subject)-1] = '\0';
			//open 2 files at username/mymailbox and tousr/mymailbox
			FILE *fpfrom = fopen(strcat(username, "/mymailbox"), "a");
			FILE *fpto = fopen(strcat(tousr, "/mymailbox"), "a");
			//write the mail preamble to the files
			//get the current time
			time_t t = time(NULL);
			char currenttime[SIZE];
			strcpy(currenttime, ctime(&t));
			fprintf(fpfrom, "%s\n", "`Live");
			fprintf(fpfrom, "%s\n", currenttime);
			fprintf(fpfrom, "%s", "From: ");
			fprintf(fpfrom, "%s\n", fromaddr);
			fprintf(fpfrom, "%s", "To: ");
			fprintf(fpfrom, "%s\n", toaddr);
			fprintf(fpfrom, "%s", "Subject: ");
			fprintf(fpfrom, "%s\n", subject);

			fprintf(fpto, "%s\n", "`Live");
			fprintf(fpto, "%s\n", currenttime);
			fprintf(fpto, "%s", "From: ");
			fprintf(fpto, "%s\n", fromaddr);
			fprintf(fpto, "%s", "To: ");
			fprintf(fpto, "%s\n", toaddr);
			fprintf(fpto, "%s", "Subject: ");
			fprintf(fpto, "%s\n", subject);
			//read the mail body
			bzero(buffer, SIZE);
			read(sockfd, buffer, SIZE);
			while(buffer[0] != '.') {
				fprintf(fpfrom, "%s", buffer);
				fprintf(fpto, "%s", buffer);
				printf("%s", buffer);
				bzero(buffer, SIZE);
				read(sockfd, buffer, SIZE);
			}
			printf("%s", buffer);
			fclose(fpfrom);
			fclose(fpto);
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
