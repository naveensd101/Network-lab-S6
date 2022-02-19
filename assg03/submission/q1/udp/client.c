#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void func_commun(int sockfd, struct sockaddr_in server_addr) {
	int server_addr_len = sizeof(server_addr);
	char buffer[256];
	int n;
	while(1) {
		bzero(buffer, 256);
		printf("[@client] Enter the message: ");
		fgets(buffer, 255, stdin);
		//n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
		n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, server_addr_len);
		if (n < 0) {
			perror("[@client] Error in sendto()");
			exit(1);
		}

		if(strcmp(buffer, "exit\n") == 0) {
			printf("[@client] Exiting...\n");
			break;
		}
		else if(strcmp(buffer, "SendInventory\n") == 0) {
			int fruits[5];
			//n = recvfrom(sockfd, buffer, 255, 0, (struct sockaddr *)&server_addr, &server_addr_len);
			n = recvfrom(sockfd, fruits, sizeof(fruits), 0, (struct sockaddr *)&server_addr, &server_addr_len);
			if (n < 0) {
				perror("[@client] Error in sendto()");
				break;
			}
			for(int i = 0; i < 5; ++i) {
				printf("%d ", fruits[i]);
			}
			printf("\n");
		}
		else if(strcmp(buffer, "Fruits\n") == 0) {
			n = recvfrom(sockfd, buffer, 255, 0, (struct sockaddr *)&server_addr, &server_addr_len);
			if (n < 0) {
				perror("[@client] Error in sendto()");
				break;
			}
			printf("[@client] %s", buffer);
			bzero(buffer, 256);
			fgets(buffer, 255, stdin);
			sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, server_addr_len);
		}
	}
}
int main() {
	//used udp to talk to a server
	int sockfd;
	struct sockaddr_in servaddr;
	char buff[256];
	int server_struct_size = sizeof(servaddr);
	bzero(&servaddr, server_struct_size);
	bzero(buff, 256);
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket < 0) {
		printf("[@client]Error in creating socket\n");
		exit(0);
	}
	printf("[@client]Socket created\n");
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(5000);

	//send the message to the server
	strcpy(buff, "Hello\n");
	int n = sendto(sockfd, buff, strlen(buff), 0, (struct sockaddr *)&servaddr, server_struct_size);
	if(n < 0) {
		printf("[@client]Error in sending message\n");
		exit(0);
	}
	printf("[@client]Message sent\n");
	recvfrom(sockfd, buff, 256, 0, (struct sockaddr *)&servaddr, &server_struct_size);
	printf("[@client]Message received : %s", buff);
	func_commun(sockfd, servaddr);
	//close the socket
	close(sockfd);
}

