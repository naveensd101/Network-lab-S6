#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

//void func_commun(int connfd) {
void func_commun(struct sockaddr_in client_addr, int connfd) {
	int client_len = sizeof(client_addr);
	char buffer[256];
	int n;
	int fruits_buffer[] = {5, 5, 5, 5, 5};
	while(1) {
		bzero(buffer, 256);
		n = recvfrom(connfd, buffer, 255, 0, (struct sockaddr *)&client_addr, &client_len);
		if (n < 0) {
			printf("ERROR reading from socket\n");
			exit(1);
		}
		if(strcmp(buffer, "exit\n") == 0) {
			printf("Client disconnected\n");
			break;
		}
		else if(strcmp(buffer, "SendInventory\n") == 0) {
			printf("Sending inventory\n");
			sendto(connfd, fruits_buffer, sizeof(fruits_buffer), 0, (struct sockaddr *)&client_addr, client_len);
		}
		else if(strcmp(buffer, "Fruits\n") == 0) {
			printf("Client asked for fruits\n");
			strcpy(buffer, "which fruit to reduce?\n");
			sendto(connfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, client_len);
			recvfrom(connfd, buffer, 255, 0, (struct sockaddr *)&client_addr, &client_len);
			printf("Client asked for %s", buffer);
			fruits_buffer[0]--;
			if(fruits_buffer[0] < 0) printf("No more fruits\n");
		}
		else {
			printf("Client sent: %s", buffer);
		}
	}
}
int main() {
	//use udp to communicate to a client
	int sockfd;
	struct sockaddr_in server_addr, client_addr;
	int client_addr_len = sizeof(client_addr);
	char buffer[256];
	//create a socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0) {
		printf("[@server]Error in creating socket\n");
		return -1;
	}
	printf("[@server]Socket created\n");
	//initialize server_addr
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(5000);
	//bind the socket to the server_addr
	if(bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		printf("[@server]Error in binding\n");
		return -1;
	}
	printf("[@server]Binded\n");
	//receive from client
	bzero(buffer, sizeof(buffer));
	if(recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_addr_len) < 0) {
		printf("[@server]Error in receiving\n");
		return -1;
	}
	printf("[@server]Received from client: %s", buffer);
	strcpy(buffer, "server message:Hello client\n");
	sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, client_addr_len);
	//close the socket
	func_commun(client_addr, sockfd);
	close(sockfd);
	return 0;
}
