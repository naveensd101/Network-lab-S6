#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void func_commun(int sockfd) {
	char buff[512];
	int n;
	while(1) {
		bzero(buff, 500);
		printf("[above][@client]Enter the message: ");
		fgets(buff, 500, stdin);
		n = write(sockfd, buff, strlen(buff));
		if(strcmp(buff, "bye\n") == 0) {
			break;
		}
		else if(strcmp(buff, "vid\n") == 0) {
			//read the buffer data from the socket
			//and write it into a empty .mp4 file here
			FILE *fp;
			fp = fopen("client.mp4", "wb");
			bzero(buff, 500);
			int n;
			while((n = read(sockfd, buff, 500)) > 0) {
				if(strcmp(buff, "end\n") == 0) {
					//fclose(fp);
					break;
				}
				fwrite(buff, sizeof(char), n, fp);
				bzero(buff, 500);
			}
			fclose(fp);
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
	return 0;
}

