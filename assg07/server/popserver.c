//create a POP server

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
const int PORT = 5035;

void communicate_with_client(int sockfd) {
	char buffer[SIZE];
	int n;
	while(1) {
		bzero(buffer, SIZE);
		read(sockfd, buffer, SIZE);
		printf("[From client]: %s", buffer);
		if(strcmp(buffer, "quit\n") == 0) {
			bzero(buffer, SIZE);
			strcpy(buffer, "Good Bye from POPsd!!\n");
			write(sockfd, buffer, SIZE);
			return;
		}
		else if(strcmp(buffer, "Manage\n") == 0) {
			bzero(buffer, SIZE);
			//auth first
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
				printf("Not Authenitcated by the user %s\n", username);
				write(sockfd, buffer, SIZE);
				continue;
			}
			//send the user all the mails with
			//<slnum><from><from><time><subject>
			fp = fopen(strcat(username, "/mymailbox"), "rw");
			int slno = 0;
			//read line by line and increment slno when getting `
			bzero(buffer, SIZE);
			while(fgets(buffer, SIZE, fp) != NULL) {
				if(buffer[0] == '`') {
					slno++;
					char state[SIZE];
					bzero(state, SIZE);
					if(buffer[1] == 'L') {
						//this means the mail is live and not deleted
						strcpy(state, "LIVE");
					}
					else {
						//this means the mail is deleted
						strcpy(state, "DELETED");
					}
					//copy slno to the char buffer
					//bzero(buffer, SIZE);
					//strcpy(buffer, "--------------------------\n");
					//write(sockfd, buffer, SIZE);
					write(sockfd, state, SIZE);
					bzero(buffer, SIZE);
					sprintf(buffer, "(%d) ", slno);
					write(sockfd, buffer, SIZE);
					char time[SIZE];
					bzero(time, SIZE);
					fgets(time, SIZE, fp);
					write(sockfd, time, SIZE);
					fgets(buffer, SIZE, fp);
					char from[SIZE];
					bzero(from, SIZE);
					fgets(from, SIZE, fp);
					write(sockfd, from, SIZE);
					char to[SIZE];
					bzero(to, SIZE);
					fgets(to, SIZE, fp);
					write(sockfd, to, SIZE);
					char subject[SIZE];
					bzero(subject, SIZE);
					fgets(subject, SIZE, fp);
					write(sockfd, subject, SIZE);
					bzero(buffer, SIZE);
					strcpy(buffer, "--------------------------\n");
					write(sockfd, buffer, SIZE);
					printf("[To client]:---\n %s\n%d %s %s %s %s\n---------------\n", state, slno, time, from, to, subject);
				}
				bzero(buffer, SIZE);
			}
			fclose(fp);
			bzero(buffer, SIZE);
			strcpy(buffer, "End of the message stuff\n");
			write(sockfd, buffer, SIZE);
		}
		else if(strcmp(buffer, "view\n") == 0) {
			bzero(buffer, SIZE);
			//auth first
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
				printf("Not Authenitcated by the user %s\n", username);
				write(sockfd, buffer, SIZE);
				continue;
			}
			fp = fopen(strcat(username, "/mymailbox"), "rw");
			//get slno to view from the user
			int gotslno = 0;
			bzero(buffer, SIZE);
			read(sockfd, buffer, SIZE);
			buffer[strlen(buffer)-1] = '\0';
			gotslno = atoi(buffer);
			int slno = 0;
			//read line by line and increment slno when getting `
			bzero(buffer, SIZE);
			while(fgets(buffer, SIZE, fp) != NULL) {
				if(buffer[0] == '`') {
					slno++;
					char state[SIZE];
					bzero(state, SIZE);
					if(buffer[1] == 'L') {
						//this means the mail is live and not deleted
						strcpy(state, "LIVE");
					}
					else {
						//this means the mail is deleted
						strcpy(state, "DELETED");
					}
					//copy slno to the char buffer
					//bzero(buffer, SIZE);
					//strcpy(buffer, "--------------------------\n");
					//write(sockfd, buffer, SIZE);
					if(gotslno == slno) {
						write(sockfd, state, SIZE);
						bzero(buffer, SIZE);
						sprintf(buffer, "(%d) ", slno);
						write(sockfd, buffer, SIZE);
						char time[SIZE];
						bzero(time, SIZE);
						fgets(time, SIZE, fp);
						write(sockfd, time, SIZE);
						fgets(buffer, SIZE, fp);
						char from[SIZE];
						bzero(from, SIZE);
						fgets(from, SIZE, fp);
						write(sockfd, from, SIZE);
						char to[SIZE];
						bzero(to, SIZE);
						fgets(to, SIZE, fp);
						write(sockfd, to, SIZE);
						char subject[SIZE];
						bzero(subject, SIZE);
						fgets(subject, SIZE, fp);
						write(sockfd, subject, SIZE);
						printf("[To client]:---\n %s\n%d %s %s %s %s\n---------------\n", state, slno, time, from, to, subject);
						bzero(buffer, SIZE);
						while(fgets(buffer, SIZE, fp) && buffer[0] != '`') {
							write(sockfd, buffer, SIZE);
							bzero(buffer, SIZE);
						}
						bzero(buffer, SIZE);
						strcpy(buffer, "--------------------------\n");
						write(sockfd, buffer, SIZE);
					}
				}
				bzero(buffer, SIZE);
			}
			fclose(fp);
			bzero(buffer, SIZE);
			strcpy(buffer, "End of the message stuff\n");
			write(sockfd, buffer, SIZE);
		}
		else if(strcmp(buffer, "del\n") == 0) {
			bzero(buffer, SIZE);
			//auth first
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
				printf("Not Authenitcated by the user %s\n", username);
				write(sockfd, buffer, SIZE);
				continue;
			}
			fp = fopen(strcat(username, "/mymailbox"), "r+");
			//get slno to view from the user
			int gotslno = 0;
			bzero(buffer, SIZE);
			read(sockfd, buffer, SIZE);
			buffer[strlen(buffer)-1] = '\0';
			gotslno = atoi(buffer);
			int slno = 0;
			printf("gotslno : %d\n", gotslno);
			//read character by character from the mailbox file
			//increment slno till it gets equal to gotslno
			char ch;
			int flag = 0;
			while(1) {
				ch = fgetc(fp);
				if(feof(fp)) break;
				if(ch == '`') slno++;
				if(flag == 0 && slno == gotslno) {
					fputc('d', fp);
					flag = 1;
				}
			}
			fclose(fp);
			if(flag) {
				bzero(buffer, SIZE);
				strcpy(buffer, "delete done\n");
				write(sockfd, buffer, SIZE);
			}
			else {
				bzero(buffer, SIZE);
				strcpy(buffer, "mail doenst exist\n");
				write(sockfd, buffer, SIZE);
			}
		}
		else if(strcmp(buffer, "FilterFrom\n") == 0) {
			bzero(buffer, SIZE);
			//auth first
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
				printf("Not Authenitcated by the user %s\n", username);
				write(sockfd, buffer, SIZE);
				continue;
			}
			fp = fopen(strcat(username, "/mymailbox"), "rw");
			//get slno to view from the user
			char filter[SIZE];
			bzero(filter, SIZE);
			read(sockfd, filter, SIZE);
			filter[strlen(filter) - 1] = '\0';
			printf("filter from mail = %s\n", filter);
			int slno = 0;
			//read line by line and increment slno when getting `
			bzero(buffer, SIZE);
			while(fgets(buffer, SIZE, fp) != NULL) {
				if(buffer[0] == '`') {
					slno++;
					char state[SIZE];
					bzero(state, SIZE);
					if(buffer[1] == 'L') {
						//this means the mail is live and not deleted
						strcpy(state, "LIVE");
					}
					else {
						//this means the mail is deleted
						strcpy(state, "DELETED");
					}
					//copy slno to the char buffer
					//bzero(buffer, SIZE);
					//strcpy(buffer, "--------------------------\n");
					//write(sockfd, buffer, SIZE);
					if( 1) {
						bzero(buffer, SIZE);
						sprintf(buffer, "(%d) ", slno);

						char time[SIZE];
						bzero(time, SIZE);
						fgets(time, SIZE, fp);

						fgets(buffer, SIZE, fp);
						char from[SIZE];
						bzero(from, SIZE);
						fgets(from, SIZE, fp);

						printf("###%s####\n", from);
						from[strlen(from) - 1] = '\0';
						char tmp[SIZE];
						int j = 0;
						for(int i = 6; i < strlen(from); ++i) {
							tmp[j++] = from[i];
						}
						tmp[j] = '\0';
						printf("####%s####\n", tmp);

						char to[SIZE];
						bzero(to, SIZE);
						fgets(to, SIZE, fp);

						char subject[SIZE];
						bzero(subject, SIZE);
						fgets(subject, SIZE, fp);

						bzero(buffer, SIZE);
						if(strcmp(tmp, filter) == 0) {
							bzero(buffer, SIZE);
							sprintf(buffer, "(%d) ", slno);
							write(sockfd, buffer, SIZE);
							write(sockfd, state, SIZE);
							write(sockfd, time, SIZE);
							write(sockfd, from, SIZE);
							write(sockfd, to, SIZE);
							write(sockfd, subject, SIZE);
							while(fgets(buffer, SIZE, fp) && buffer[0] != '`') {
								write(sockfd, buffer, SIZE);
								bzero(buffer, SIZE);
							}
						}
						bzero(buffer, SIZE);
						strcpy(buffer, "--------------------------\n");
						write(sockfd, buffer, SIZE);
					}
				}
				bzero(buffer, SIZE);
			}
			fclose(fp);
			bzero(buffer, SIZE);
			strcpy(buffer, "End of the message stuff\n");
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
