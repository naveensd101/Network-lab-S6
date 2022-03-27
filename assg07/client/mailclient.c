// create a mail client that sends to a SMTP server and receives from a POP3 server

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
const int SPORT = 4035; //SMTP port
const int PPORT = 5035; //POP3 port

int AUTH = 0;

void communicate_with_servers(int SMTP, int POP3) {
	char buffer[SIZE];
	int n;
	while(1) {
		bzero(buffer, SIZE);
		printf("[@Client] Enter a command: ");
		fgets(buffer, SIZE, stdin);
		n = write(SMTP, buffer, SIZE);
		if(strcmp(buffer, "quit\n") == 0) {
			//this we have to communicate with the SMTP and POP3 servers
			printf("[@Client] Closing connection to SMTP...\n");
			bzero(buffer, SIZE);
			read(SMTP, buffer, SIZE);
			printf("[msg from SMTP] %s", buffer);

			close(SMTP);
			strcpy(buffer, "quit\n");
			write(POP3, buffer, SIZE);
			printf("[@Client] Closing connection to POP3...\n");
			bzero(buffer, SIZE);
			read(POP3, buffer, SIZE);
			printf("[msg from POP3] %s", buffer);
			return;
		}
		else if(strcmp(buffer, "auth\n") == 0) {
			//auth will not be implemented in this version
			//we will authenticate each time we send or receive mail
			printf("[@Client] Enter username: ");
			bzero(buffer, SIZE);
			fgets(buffer, SIZE, stdin);
			write(SMTP, buffer, SIZE);
			printf("[@Client] Enter password: ");
			bzero(buffer, SIZE);
			fgets(buffer, SIZE, stdin);
			write(SMTP, buffer, SIZE);
			bzero(buffer, SIZE);
			read(SMTP, buffer, SIZE);
			if(strcmp(buffer, "Authenticated\n") == 0) AUTH = 1;
			printf("[msg from SMTP] %s", buffer);
		}
		else if(strcmp(buffer, "SendMail\n") == 0) {
			//send mail will be done only by SMTP server
			//first authenticate
			printf("[@Client] Enter username: ");
			bzero(buffer, SIZE);
			fgets(buffer, SIZE, stdin);
			write(SMTP, buffer, SIZE);
			printf("[@Client] Enter password: ");
			bzero(buffer, SIZE);
			fgets(buffer, SIZE, stdin);
			write(SMTP, buffer, SIZE);
			bzero(buffer, SIZE);
			read(SMTP, buffer, SIZE);
			if(strcmp(buffer, "Authenticated\n") == 0) {
				printf("Authenticated\n");
			}
			else {
				printf("Wront username or password\n");
				continue;
			}

			//send mail
			bzero(buffer, SIZE);
			printf("[@Client]---MAIL-FORM---\n");
			printf("To: ");
			fgets(buffer, SIZE, stdin);
			write(SMTP, buffer, SIZE);
			printf("Subject: ");
			bzero(buffer, SIZE);
			fgets(buffer, SIZE, stdin);
			write(SMTP, buffer, SIZE);
			printf("Enter the message followed by a .\n");
			bzero(buffer, SIZE);
			fgets(buffer, SIZE, stdin);
			while(buffer[0] != '.') {
				write(SMTP, buffer, SIZE);
				fgets(buffer, SIZE, stdin);
			}
			write(SMTP, buffer, SIZE);
		}
		else if(strcmp(buffer, "Manage\n") == 0) {
			//manage done by pop server
			//first authenticate
			write(POP3, buffer, SIZE);
			printf("[@Client] Enter username: ");
			char username[SIZE];
			bzero(username, SIZE);
			fgets(username, SIZE, stdin);
			write(POP3, username, SIZE);
			char password[SIZE];
			bzero(password, SIZE);
			fgets(password, SIZE, stdin);
			write(POP3, password, SIZE);
			bzero(buffer, SIZE);
			read(POP3, buffer, SIZE);
			if(strcmp(buffer, "Authenticated\n") == 0) {
				printf("Authenticated\n");
			}
			else {
				printf("Wront username or password\n");
				continue;
			}
			bzero(buffer, SIZE);
			printf("[@Client]---MAIL-MANAGE---\n");
			//recieve data till you get a this is over message
			read(POP3, buffer, SIZE);
			while(strcmp(buffer, "End of the message stuff\n") != 0) {
				printf("%s", buffer);
				bzero(buffer, SIZE);
				read(POP3, buffer, SIZE);
			}
		}
		else if(strcmp(buffer, "view\n") == 0) {
			//view is done by pop server
			write(POP3, buffer, SIZE);
			printf("[@Client] Enter username: ");
			char username[SIZE];
			bzero(username, SIZE);
			fgets(username, SIZE, stdin);
			write(POP3, username, SIZE);
			char password[SIZE];
			bzero(password, SIZE);
			fgets(password, SIZE, stdin);
			write(POP3, password, SIZE);
			bzero(buffer, SIZE);
			read(POP3, buffer, SIZE);
			if(strcmp(buffer, "Authenticated\n") == 0) {
				printf("Authenticated\n");
			}
			else {
				printf("Wront username or password\n");
				continue;
			}
			//send slno to be viewed
			int slno = 0;
			printf("Enter the slno of the message to be viewed: ");
			scanf("%d", &slno);
			bzero(buffer, SIZE);
			sprintf(buffer, "%d\n", slno);
			write(POP3, buffer, SIZE);
			bzero(buffer, SIZE);
			printf("[@Client]---MAIL-MANAGE---\n");
			//recieve data till you get a this is over message
			read(POP3, buffer, SIZE);
			while(strcmp(buffer, "End of the message stuff\n") != 0) {
				printf("%s", buffer);
				bzero(buffer, SIZE);
				read(POP3, buffer, SIZE);
			}
		}
		else if(strcmp(buffer, "del\n") == 0) {
			//del is done by pop server
			write(POP3, buffer, SIZE);
			printf("[@Client] Enter username: ");
			char username[SIZE];
			bzero(username, SIZE);
			fgets(username, SIZE, stdin);
			write(POP3, username, SIZE);
			char password[SIZE];
			bzero(password, SIZE);
			fgets(password, SIZE, stdin);
			write(POP3, password, SIZE);
			bzero(buffer, SIZE);
			read(POP3, buffer, SIZE);
			if(strcmp(buffer, "Authenticated\n") == 0) {
				printf("Authenticated\n");
			}
			else {
				printf("Wront username or password\n");
				continue;
			}
			//send slno to be viewed
			int slno = 0;
			printf("Enter the slno of the message to be deleted: ");
			scanf("%d", &slno);
			bzero(buffer, SIZE);
			sprintf(buffer, "%d\n", slno);
			write(POP3, buffer, SIZE);
			bzero(buffer, SIZE);
			read(POP3, buffer, SIZE);
			printf("%s", buffer);
		}
		else if(strcmp(buffer, "FilterFrom\n") == 0) {
			//view is done by pop server
			write(POP3, buffer, SIZE);
			printf("[@Client] Enter username: ");
			char username[SIZE];
			bzero(username, SIZE);
			fgets(username, SIZE, stdin);
			write(POP3, username, SIZE);
			char password[SIZE];
			bzero(password, SIZE);
			fgets(password, SIZE, stdin);
			write(POP3, password, SIZE);
			bzero(buffer, SIZE);
			read(POP3, buffer, SIZE);
			if(strcmp(buffer, "Authenticated\n") == 0) {
				printf("Authenticated\n");
			}
			else {
				printf("Wront username or password\n");
				continue;
			}
			char from[SIZE];
			printf("Enter the from address to filter: ");
			bzero(from, SIZE);
			fgets(from, SIZE, stdin);
			write(POP3, from, SIZE);
			printf("[@Client]---MAIL-MANAGE---\n");
			//recieve data till you get a this is over message
			read(POP3, buffer, SIZE);
			while(strcmp(buffer, "End of the message stuff\n") != 0) {
				printf("%s", buffer);
				bzero(buffer, SIZE);
				read(POP3, buffer, SIZE);
			}
		}
	}
}

int main() {
	//-----------------SMTP server-----------------
	int sockfd_smtp;
	struct sockaddr_in serv_addr;

	//create a socket
	sockfd_smtp = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd_smtp < 0) {
		printf("Error opening SMTP socket\n");
		exit(1);
	}
	else {
		printf("SMTP Socket opened\n");
	}

	//set up the server address
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SPORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	//connect to the server
	if (connect(sockfd_smtp, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		printf("Error connecting to SMTP server\n");
		exit(1);
	}
	else {
		printf("Connected to SMTP server\n");
	}
	//-----------------SMTP init over-----------------
	//-----------------POP3 server--------------------
	int sockfd_pop3;
	struct sockaddr_in serv_addr_pop3;

	//create a socket
	sockfd_pop3 = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd_pop3 < 0) {
		printf("Error opening POP3 socket\n");
		exit(1);
	}
	else {
		printf("POP3 Socket opened\n");
	}

	//set up the server address
	bzero(&serv_addr_pop3, sizeof(serv_addr_pop3));
	serv_addr_pop3.sin_family = AF_INET;
	serv_addr_pop3.sin_port = htons(PPORT);
	serv_addr_pop3.sin_addr.s_addr = INADDR_ANY;

	//connect to the server
	if(connect(sockfd_pop3, (struct sockaddr *) &serv_addr_pop3, sizeof(serv_addr_pop3)) < 0) {
		printf("Error connecting to POP3 server\n");
		exit(1);
	}
	else {
		printf("Connected to POP3 server\n");
	}
	//-----------------POP3 init over-----------------

	communicate_with_servers(sockfd_smtp, sockfd_pop3);
	close(sockfd_smtp);
	//close(sockfd_pop3);
	return 0;
}
