#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void func_commun(int connfd) {
	char buffer[512];
	int n;
	while(1) {
		bzero(buffer,256);
		n = read(connfd,buffer,256);
		if(n < 0) {
			perror("ERROR reading from socket");
			exit(1);
		}
		printf("%s",buffer);
		if(strcmp(buffer,"bye\n") == 0) {
			break;
		}
		else if(strcmp(buffer, "vid\n") == 0) {
			FILE *f;
			f = fopen("Sintel.mp4", "rb");
			if(f == NULL) {
				printf("Error opening file\n");
				exit(1);
			}
			int count = 0;
			int time_now;
			struct tcp_info info;
			int res;
			socklen_t len = sizeof(info);
			FILE *f1 = fopen("packet_time.txt", "a");
			struct timeval start, next;
			gettimeofday(&start, NULL);
			while(!feof(f)) {
				//open a file to list all the packet number and the time stamp of the packet
				bzero(buffer,500);
				fread(buffer,1,500,f);
				time_now = time(NULL);
				gettimeofday(&next, NULL);
				n = write(connfd,buffer,500);
				if(n < 0) {
					perror("ERROR writing to socket");
					exit(1);
				}
				printf("%s %d", "sending video packet number ", count++);
				printf("\n");
				//find round trip time using getsocketopt
				//struct tcp_info info;
				//socklen_t len = sizeof(info);
				//getsockopt - gives info about the socket
				//getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
				//level - protocol level
				//optname - option name
				//optval - pointer to the buffer to store the option value
				//optlen - length of the buffer
				//returns 0 on success
				//returns -1 on error
				//IPPROTO_TCP - protocol level
				//TCP_INFO - option name
				//info - pointer to the buffer to store the option value
				//len - length of the buffer
				res = getsockopt(connfd, IPPROTO_TCP, TCP_INFO, &info, &len);
				//file pirnt packet number and time stamp and the rtt
				//double secs = (double)(next.tv_sec - start.tv_sec) + (double)(next.tv_usec - start.tv_usec) / 1000000;
				//get the milliseconds between the start and the next
				double secs = (double)(next.tv_sec - start.tv_sec) + (double)(next.tv_usec - start.tv_usec) / 1000000;
				fprintf(f1, "%d,%lf,%d\n", count, secs, info.tcpi_rtt);
				if(res < 0) {
					perror("ERROR getting socket option");
					exit(1);
				}
				printf("%s %dms\n", "RTT is ", info.tcpi_rtt);
			}
			strcpy(buffer, "end\n");
			n = write(connfd,buffer,500);
			printf("%s", "video sent\n");
			//clise the file
			fclose(f1);
			//fclose(f);
		}
		else {
			strcpy(buffer, "random message\n");
			write(connfd,buffer,256);
		}
	}
}
int main() {
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

	//socket create and verification
	//AF_INET - IPv4
	//SOCK_STREAM - TCP
	//0 - default protocol
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

	//bind - bind the socket to the port
	//bind returns 0 on success and -1 on failure
	if(bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
		printf("Socket bind failed\n");
		exit(1);
	}
	else
		printf("Socket successfully binded\n");

	//listen - listen for incoming connections
	//listen returns 0 on success and -1 on failure
	if(listen(sockfd, 5) != 0) {
		printf("Listen failed\n");
		exit(1);
	}
	else
		printf("Server listening\n");
	len = sizeof(cli);
	//accept - accept the incoming connection
	//accept returns -1 on failure and the socket file descriptor on success
	connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
	if(connfd < 0) {
		printf("Accept failed\n");
		exit(1);
	}
	else
		printf("Client connected\n");
	func_commun(connfd);
	close(sockfd);
}


