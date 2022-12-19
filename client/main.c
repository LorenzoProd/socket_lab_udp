#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <string.h>
#include <netdb.h>

#define BUFFERSIZE 512

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
            return -1;
        }
    #endif

    int CliSock;
    struct sockaddr_in ServerAddress;
    int fromSize;
    char buffer[BUFFERSIZE];
    char okMessage[2];
    int respStringLen;
    char name[50];
    int port=0;
    struct hostent *host;
    struct in_addr *ina;
    char vowels[BUFFERSIZE];
    int v = 0;
    char nvoc[BUFFERSIZE];
    char capitalVowel[1];

    printf("Inserisci il nome del server: ");
    scanf("%s", name);
    printf("Inserisci il numero di porta del server (default = 5000): ");
    scanf("%d", &port);
    host = gethostbyname(name);
    if	(host == NULL)	{
        ErrorHandler("gethostbyname() failed.\n");
        return -1;
    } else {
        ina	= (struct in_addr*)	host->h_addr_list[0];
        printf("IP ottenuto tramite il nome dell'host(%s): %s\n", name, inet_ntoa(*ina));
    }

    // Initialize socket
    if ((CliSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        ErrorHandler("socket() failed\n");
        return -1;
    }

    // Server address construct
    memset(&ServerAddress, 0, sizeof(ServerAddress));
    ServerAddress.sin_family = PF_INET;
    ServerAddress.sin_port = htons(port);
    ServerAddress.sin_addr.s_addr = inet_addr(inet_ntoa(*ina));

    // Send string to server
    printf("Messaggio di benvenuto da inviare al server\n");
    scanf("%s", buffer);
    if ((strlen(buffer)) > BUFFERSIZE) {
        ErrorHandler("socket() failed\n");
        return -1;
    }

    if (sendto(CliSock, buffer, strlen(buffer), 0, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress)) != strlen(buffer)) {
        ErrorHandler("sendto() sent different number of bytes than expected\n");
        return -1;
    }

    // Receive confirm from server
    fromSize = sizeof(ServerAddress);
    respStringLen = recvfrom(CliSock, okMessage, 2, 0, (struct sockaddr*)&ServerAddress, &fromSize);
    okMessage[respStringLen] = '\0';
    printf("Ricevuto dal server: %s\n", okMessage);

    // Scan string to manipulate
    memset(buffer, '\0', sizeof(buffer));
    printf("Inserisci la stringa\n");
    fflush(stdin);
    fgets(buffer, sizeof(buffer), stdin);

    // save and count vowels
    for (int i = 0; i < strlen(buffer); i++){
        if ((buffer[i] == 'a')||(buffer[i] == 'e')||(buffer[i] == 'i')||(buffer[i] == 'o')||(buffer[i] == 'u')||(buffer[i] == 'A')||(buffer[i] == 'E')||(buffer[i] == 'I')||(buffer[i] == 'O')||(buffer[i] == 'U')){
            vowels[v]=buffer[i];
            v++;
        }
    }
    vowels[v]='\0';
    sprintf(nvoc, "%d", v);

    // send vowels number
    sendto(CliSock, nvoc, strlen(nvoc), 0, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress));
    printf("%s Vocali trovate: %s\n", nvoc, vowels);

    printf("Vocali della stringa inviata in MAIUSCOLO: \n");
    for (int i=0; i<v; i++){
        sendto(CliSock, &vowels[i], 1, 0, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress));
        recvfrom(CliSock, capitalVowel, 1, 0, (struct sockaddr*)&ServerAddress, &fromSize);
        printf("\n%c", capitalVowel[0]);
    }
    printf("\n");

    closesocket(CliSock);
    ClearWinSock();
    return -1;
}