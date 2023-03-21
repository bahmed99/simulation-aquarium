#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLIENTS 10

int main(int argc, char *argv[]) {
    if (argc != 2) return EXIT_FAILURE;
    char buffer[256];
    int ServSock;
    int ClntSock;
    struct sockaddr_in ServAddr;
    unsigned int ServPort;
    struct sockaddr_in ClntAddr;
    int length;

    ServPort = atoi(argv[1]);

    if ((ServSock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket Error\n");
    }
    ServAddr.sin_family = AF_INET;
    ServAddr.sin_addr.s_addr = INADDR_ANY;
    ServAddr.sin_port = htons(ServPort);
    
    /* Binding socket*/
    if (bind(ServSock, (struct sockaddr*) &ServAddr, sizeof(ServAddr)) < 0) {
        printf("Binding Error\n");
        exit(1);
    }
    /* Start Listening*/
    if (listen(ServSock, 5) < 0) {
        printf("Listen() Error\n");
        exit(1);
    }
    
    /* */
    while(1) {
        int ClntLen = sizeof(ClntAddr);
        /* Waiting for a client to connect */
        if ((ClntSock = accept(ServSock, (struct sockaddr *) &ClntAddr, &ClntLen))< 0) {
            printf("Accept() Error\n");
            exit(1);
        }
        /* ClntSock is connected to a client */
        printf("Connected to %s:%d\n", inet_ntoa(ClntAddr.sin_addr), ClntAddr.sin_port);
        /* Handler  (Client)*/

        length = read(ClntSock, buffer, 255);
        if (length < 0) {
            printf("ERROR reading from socket\n");
            exit(1);
        }
        printf("Here is the message: %s\n", buffer);

        length = write(ClntSock, "I got your message", 18);
        if (length < 0) {
            printf("ERROR writing to socket\n");

            exit(1);
        }
    }
   

    return EXIT_SUCCESS;
}