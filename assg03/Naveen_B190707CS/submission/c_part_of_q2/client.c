//implement a udp client with stop and wait protocol
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

struct packet {
	int num;
	char data[512];
};
int main() {
	int sockfd;
	struct sockaddr_in servaddr;
	int server_addr_len = sizeof(servaddr);
	struct packet pkt;
	bzero(&pkt, sizeof(pkt));
	bzero(&servaddr, sizeof(servaddr));
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0) {
		perror("socket not made");
		exit(1);
	}
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5000);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	strcmp(pkt.data, "hello this is the first packet just to initialize the number variable\n");
	pkt.num = 0;
	int n = sendto(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&servaddr, server_addr_len);
	if(n < 0) {
		perror("sendto failed");
		exit(1);
	}
	n = recvfrom(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&servaddr, &server_addr_len);
	if(n < 0) {
		perror("recvfrom failed");
		exit(1);
	}
	printf("%s ", pkt.data);
	printf("%d\n", pkt.num);
	close(sockfd);
	return 0;
}

