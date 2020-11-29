#include "tcp.h"

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		printf("invalid parameters.\n");
		printf("USAGE %s <listen-port> <send-port>\n", argv[0]);
		return 1;
	}
	
	srand(time(NULL));

	int sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	if (sock == -1)
	{
		printf("socket creation failed\n");
		return 1;
	}

	struct sockaddr_in adr_inet;
	socklen_t len_inet;
	memset(&adr_inet,0,sizeof adr_inet);
    adr_inet.sin_family = AF_INET;
    adr_inet.sin_port = htons(atoi(argv[1]));
    adr_inet.sin_addr.s_addr = INADDR_ANY;

    

	if (adr_inet.sin_addr.s_addr == INADDR_NONE) {
		printf("bad address");
		return 1;
	}

	len_inet = sizeof(struct sockaddr_in);

    // call bind with port number specified as zero to get an unused source port
	if (bind(sock, (struct sockaddr*)&adr_inet, sizeof(struct sockaddr)) == -1)
	{
		printf("bind() failed\n");
		return 1;
	}


    // tell the kernel that headers are included in the packet
	int one = 1;
	const int *val = &one;
	if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) == -1)
	{
		printf("setsockopt(IP_HDRINCL, 1) failed\n");
		return 1;
	}

	printf("server listening on port %u\n", adr_inet.sin_port);

	struct sockaddr_in clientAddr;
  	memset(&clientAddr, 0, sizeof(struct sockaddr_in));
  	socklen_t clientAddrLen = sizeof(clientAddr);
    
    unsigned short d_port;
	int received = 0;
    char recvbuf[DATAGRAM_LEN];
    char clientip[INET_ADDRSTRLEN];


	BYTE * data = (BYTE *) malloc(DATAGRAM_LEN);
	do {
		int receive = recvfrom(sock, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *) &clientAddr, &clientAddrLen);
        memcpy(&d_port, recv + 22, sizeof(d_port));
		if (receive <= 0) {
			printf("receive failed\n");
			return 1;
		}
        memcpy(&d_port, recvbuf + 22, sizeof(d_port));
        // printf("got packet on port: %u : %u\n", d_port, adr_inet.sin_port);
	}
    while (d_port != adr_inet.sin_port);
	inet_ntop(AF_INET, &(clientAddr.sin_addr), clientip, INET_ADDRSTRLEN);
	printf("connected to: %s , bytes : %d\n", clientip, received);
	clientAddr.sin_port = htons(atoi(argv[2]));

    char* packet;
	int packet_len;
    char request[] = "Hello there! what are you doing";
    int sent;
	// // send data
	int i = 10;
    while (i > 0) {
		create_data_packet(&adr_inet, &clientAddr, 1 + i, 32 + i, request, sizeof(request) - 1/sizeof(char), &packet, &packet_len);
        if ((sent = sendto(sock, packet, packet_len, 0, (struct sockaddr*)&clientAddr, sizeof(struct sockaddr))) == -1)
        {
            printf("send failed\n");
            return 0;
        }
        else
        {
            printf("successfully sent %d bytes to %u from %u\n", sent, clientAddr.sin_port, adr_inet.sin_port);
        }
		sleep(1);
		i--;
    }
	

	close(sock);
	return 0;
}