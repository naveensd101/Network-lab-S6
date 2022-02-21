#include <stdio.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int hash(char n[512]) {
	int N = atoi(n);
	return N+1;
}
void func_commun(int connfd) {
	char buffer[512];
	int n;
	char password[512];
	n = read(connfd, &password, sizeof(password));
	if(hash(password) != 1234) {
		printf("Wrong password\n");
		strcpy(buffer, "Wrong password");
		write(connfd, buffer, sizeof(buffer));
		close(connfd);
		exit(0);
	}
	strcpy(buffer, "Correct password");
	write(connfd, buffer, sizeof(buffer));
	printf("Correct password\n");
	bzero(buffer, sizeof(buffer));
	n = read(connfd, &buffer, sizeof(buffer));
	buffer[strlen(buffer)-1] = '\0';
	char Name[512];
	strcpy(Name, buffer);
	printf("person joined : ****%s***\n", buffer);
	//open a file and write the name of the person who joined
	FILE *fp8;
	fp8 = fopen("people.txt", "a");
	fprintf(fp8, "%s\n", buffer);
	fclose(fp8);
	while(1) {
		bzero(buffer,512);
		n = read(connfd,buffer,512);
		int separator_idx = 0;
		for(int i = 0; i < 512; i++) {
			if(buffer[i] == ':') {
				separator_idx = i;
				break;
			}
		}
		char data[512];
		int j = 0;
		for(int i = separator_idx+1; i < 512; i++) {
			data[j++] = buffer[i];
		}
		data[j] = '\0';
		if(n < 0) {
			perror("ERROR reading from socket");
			exit(1);
		}
		printf("%s",buffer);
		if(strcmp(data,"bye\n") == 0) {
			//read the name of the person exiting
			bzero(buffer,512);
			n = read(connfd,buffer,512);
			if(n < 0) {
				perror("ERROR reading from socket");
				exit(1);
			}
			printf("%s is exiting\n",buffer);

			//open a file and remove the name of the person who left
			/*
			FILE *fp8;
			FILE *fp9;
			fp8 = fopen("people.txt", "r");
			fp9 = fopen("oldpeople.txt", "w");
			char line[512];
			char new_people[512];
			while(fscanf(fp8, "%[^\n]", line) != EOF) {
				if(strcmp(line, Name) != 0) {
					fprintf(fp9, "%s\n", line);
				}
			}
			fclose(fp8);
			fclose(fp9);
			//copy contents of oldpeople.txt to people.txt and delete oldpeople.txt
			remove("people.txt");
			rename("oldpeople.txt", "people.txt");
			*/
			break;
		}
		else if(strcmp(data, "~\n") == 0) {
			FILE *fp1;
			fp1 = fopen("database.txt","r");
			char str[512];
			while(fgets(str,512,fp1) != NULL) {
				bzero(buffer,512);
				write(connfd,str,512);
			}
			fclose(fp1);
			bzero(str,512);
			strcpy(str, "end_of_reading_file");
			write(connfd,str,512);
		}
		else {
			//append the message to database.txt
			FILE *fp;
			fp = fopen("database.txt","a");
			fprintf(fp,"%s",buffer);
			fclose(fp);
			//now i have to read the file and send it back to the client
			//a special string will mark the end of the file and the client will know that it has to stop reading
			//and start the next cycle of communication
			//read database.txt from start to end and send it to the client
			FILE *fp1;
			fp1 = fopen("database.txt","r");
			char str[512];
			while(fgets(str,512,fp1) != NULL) {
				bzero(buffer,512);
				write(connfd,str,512);
			}
			fclose(fp1);
			bzero(str,512);
			strcpy(str, "end_of_reading_file");
			write(connfd,str,512);
		}
	}
}
int main() {
	//clear the contents of the file database.txt
	FILE *fp;
	fp = fopen("database.txt","w");
	fclose(fp);
	FILE *fp2;
	fp2 = fopen("people.txt","w");
	fclose(fp2);
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



