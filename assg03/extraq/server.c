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
	printf("recvfrom is done\n");
	char val[512];
	strcpy(val, pkt.data);
	printf("data: %s\n", val);
	printf("num: %d\n", pkt.num);
	struct packet nextpkt;
	for(int i = 0; i < 5; ++i) {
		if(pkt.num == i) {
			//calculate rtt
			int start = time(NULL);
			sprintf(nextpkt.data, "hello %d", i);
			nextpkt.num = i + 1;
			if(sendto(sockfd, &nextpkt, sizeof(nextpkt), 0, (struct sockaddr *)&clientaddr, len) < 0) {
				printf("sendto error\n");
				exit(1);
			}
			printf("%dth sendto is done\n", i);
			//get the rtt of the packet
			struct tcp_info info;
			socklen_t len = sizeof(info);
			getsockopt(sockfd, SOL_TCP, TCP_INFO, &info, &len);
			printf("rtt: %d\n", info.tcpi_rtt);
			recvfrom(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&clientaddr, &len);
			int end = time(NULL);
			printf("%dth recvfrom is done with POSIX rtt %d\n", i, end - start);
		}
	}
	close(sockfd);


	return 0;
}
