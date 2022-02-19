//implement a udf server with stop and wait protocol
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
	//establish a udp socket
	int sockfd;
	struct sockaddr_in servaddr, clientaddr;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0) {
		printf("socket error\n");
		exit(1);
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(5000);
	if(bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		printf("bind error\n");
		exit(1);
	}
	printf("binding is done, udp server is running\n");
	struct packet pkt;
	int len = sizeof(clientaddr);
	if(recvfrom(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&clientaddr, &len) < 0) {
		printf("recvfrom error\n");
		exit(1);
	}
	//extract the rtt information
	struct tcp_info info;
	int buffersize = sizeof(pkt);
	int res = getsockopt(sockfd, SOL_TCP, TCP_INFO, &info, &buffersize);
	printf("recvfrom is done\n");
	printf("data: %s\n", pkt.data);
	printf("num: %d\n", pkt.num);
	printf("rtt: %d\n", info.tcpi_rtt);
	//send ack
	struct packet nextpkt;
	nextpkt.num = pkt.num + 1;
	strcpy(nextpkt.data, "ack of last packet and this is the data of this packet");
	if(sendto(sockfd, &nextpkt, sizeof(nextpkt), 0, (struct sockaddr *)&clientaddr, len) < 0) {
		printf("sendto error\n");
		exit(1);
	}
	printf("sendto is done\n");
	close(sockfd);


	return 0;
}
