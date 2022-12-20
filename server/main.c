#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>

#define CHARMAX 255
#define PORT 5000

#include <string.h>
#include <ctype.h>

char *strupr(char *str)
{
    unsigned char *p = (unsigned char *)str;
    while (*p) {
        *p = toupper((unsigned char)*p);
        p++;
    }
    return str;
}

void ErrorHandler(char *errorMessage) {
    printf ("%s", errorMessage);
}

void ClearWinSock() {
#if defined WIN32
    WSACleanup();
#endif
}

int main() {
#if defined WIN32
    WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2 ,2), &wsaData);
		if (iResult != 0) {
		printf ("error at WSASturtup\n");
		return EXIT_FAILURE;
	}
#endif

    int ServerSock;
    struct sockaddr_in ClientAddr;
    struct hostent *host;
    int cliAddrLen;
    char buffer[CHARMAX];
    int recvMsgSize;
    const char*	ip;
    struct in_addr addr;
    char* canonical_name;
    char nvoc[1];
    char voc[1];

    // Initialize socket
    if ((ServerSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        ErrorHandler("socket() failed\n");
        return -1;
    }

    struct sockaddr_in ServerAddress;
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    ServerAddress.sin_port = htons(PORT);

    // Socket Bind
    if ((bind(ServerSock, (struct sockaddr *)&ServerAddress, sizeof(ServerAddress))) < 0) {
        ErrorHandler("bind() failed\n");
        return -1;
    }

    while(1) {
        cliAddrLen = sizeof(ClientAddr);

        // Receive welcome message from Client
        recvMsgSize = recvfrom(ServerSock, buffer, CHARMAX, 0, (struct sockaddr*)&ClientAddr, &cliAddrLen);
        ip = inet_ntoa(ClientAddr.sin_addr);
        addr.s_addr = inet_addr(ip);
        host = gethostbyaddr((char*) &addr, 4, AF_INET);
        canonical_name = host->h_name;
        buffer[recvMsgSize] = '\0';
        printf("MSG ricevuto dal client '%s': %s\n", canonical_name, buffer);

        // Send OK message to Client
        if (sendto(ServerSock, "OK", 2, 0, (struct sockaddr *)&ClientAddr, sizeof(ClientAddr)) != 2) {
            ErrorHandler("sendto() sent different number of bytes than expected\n");
            return -1;
        }

        // receive vowels count
        recvfrom(ServerSock, nvoc, CHARMAX, 0, (struct sockaddr*)&ClientAddr, &cliAddrLen);
        int vowelsCount = atoi(nvoc);
        printf("%d vocali presenti nella stringa: ", vowelsCount);

        for (int i = 0; i < vowelsCount; i++){
            recvfrom(ServerSock, voc, 1, 0, (struct sockaddr*)&ClientAddr, &cliAddrLen);
            printf("%c",voc[0]);
            strupr(voc);
            if (i < vowelsCount - 1)
                printf(", ");
            sendto(ServerSock, voc, 1, 0, (struct sockaddr *)&ClientAddr, sizeof(ClientAddr));
        }
        printf("\n");
    }
}
