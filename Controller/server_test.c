#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>


#define MAX_CLIENTS 10



enum poisson_t { clown = 0 };

struct view* current_views;

struct poisson {
    enum poisson_t type;
    int x;
    int y;
};
struct view {
    int vue_x;
    int vue_y;
    int view_width;
    int view_length;
};

struct aquarium {
    int width;
    int length;
    struct view *views;
    int num_views;
    struct poisson *poissons;
};


void load_aquarium(){
    ;
}

void show_aquarium(){
    ;
}

void save_aquarium(){
    ;
}

int error(char *s){
    perror(s);
    exit(1);
}

char buffer[256];

void* routine(void* ClntSock) {
    while(1){
        int length = read((intptr_t) ClntSock, buffer, 255);
        if (length < 0) {
            printf("ERROR reading from socket\n");
            exit(1);
        }
        printf("Here is the message: %s\n", buffer);
        length = write((intptr_t) ClntSock, "I got your message", 18);
        if (length < 0) {
            printf("ERROR writing to socket\n");
            exit(1);
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    
    if (argc != 2) return EXIT_FAILURE;
    char buffer2[256];
    int ServSock;
    int ClntSock;
    int n;
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

    int ClntLen = sizeof(ClntAddr);
    /* Waiting for a client to connect */
    if ((ClntSock = accept(ServSock, (struct sockaddr *) &ClntAddr, &ClntLen))< 0) {
        printf("Accept() Error\n");
        exit(1);
    }
        /* ClntSock is connected to a client */
    printf("Connected to %s:%d\n", inet_ntoa(ClntAddr.sin_addr), ClntAddr.sin_port);
        /* Handler  (Client)*/
/*
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
    }*/
    
       /* __________________________________  */
    pthread_t thread;
    pthread_create(&thread, NULL, &routine, (void *) (intptr_t) ClntSock);
    while (1) {
        write(1, "> \n", sizeof("> \n"));
        bzero(buffer2, 256);
        read(0, buffer2, 255);
        if (strncmp(buffer2, "load aquarium", strlen("load aquarium")) == 0) {
            load_aquarium(); // Needs to be coded later
            n = write(1, "Aquarium loaded !\n", strlen("Aquarium loaded !\n"));
        }
        else if (strncmp(buffer2, "show aquarium", strlen("show aquarium")) == 0) {
            show_aquarium();
        }

        else if (strncmp(buffer2, "save aquarium", strlen("save aquarium")) == 0) {
            save_aquarium();
            n = write(1, "Aquarium saved !\n", strlen("Aquarium saved !\n"));
        }

        else if (strncmp(buffer2, "save aquarium", strlen("save aquarium")) == 0) {
            save_aquarium();
            n = write(1, "Aquarium saved !\n", strlen("Aquarium saved !\n"));
        }

        else if (strncmp(buffer2, "save aquarium", strlen("save aquarium")) == 0) {
            save_aquarium();
            n = write(1, "Aquarium saved !\n", strlen("Aquarium saved !\n"));
        }

        if (n < 0)
            error("ERROR writing to socket");
    }
    /* __________________________________  */
    pthread_join(thread, NULL);
    return EXIT_SUCCESS;
}