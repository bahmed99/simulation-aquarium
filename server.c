#include "server.h"


void* ClientHandler(void *ClntSock) {
    char buffer[256];
    while(1) {
        int length = read((intptr_t) ClntSock, buffer, 255);
        if (length < 0) {
            printf("ERROR reading from socket\n");
            exit(1);
        }
        if (strncmp(buffer, "status\n", strlen("status\n")) == 0) {
            length = write((intptr_t) ClntSock, "OK\n", 3);
            // status();   
        }
        else if (strncmp(buffer, "addFish\n", strlen("addFish\n")) == 0) {
            length = write((intptr_t) ClntSock, "OK\n", 3);
            // addFish();
        }
        else if (strncmp(buffer, "delFish\n", strlen("delFish\n")) == 0) {
            length = write((intptr_t) ClntSock, "OK\n", 3);
            // delFish();
        }
        else if (strncmp(buffer, "startFish\n", strlen("startFish\n")) == 0) {
            length = write((intptr_t) ClntSock, "OK\n", 3);
            // startFish();
        }
        else {
            length = write((intptr_t) ClntSock, "Command not found\n", strlen("Command not found\n"));
        }
        if (length < 0) {
            printf("ERROR writing to socket\n");
            exit(1);
        }
    }
    return NULL;      
}

void ServerHandler(char *buffer) {
    int n;
    if (strncmp(buffer, "load aquarium", strlen("load aquarium")) == 0) {
        //load_aquarium(); 
            n = write(1, "Aquarium loaded !\n", strlen("Aquarium loaded !\n"));
    }
    else if (strncmp(buffer, "show aquarium", strlen("show aquarium")) == 0) {
        //show_aquarium();
    }
    else if (strncmp(buffer, "save aquarium", strlen("save aquarium")) == 0) {
        //save_aquarium();
        n = write(1, "Aquarium saved !\n", strlen("Aquarium saved !\n"));
    } 
    else if (strncmp(buffer, "exit", strlen("exit")) == 0) {
        n = write(1, "GoodBye\n", strlen("GoodBye\n"));
        exit(1);
    } else {}

    if (n < 0)  {
        exit(1);
    }
}
int SocketsCreator(int *ServSock, int NumPorts, char *ports[]) {
    int maxDescriptor = -1;
    for (int port = 0; port < NumPorts; port++) {
        int Srv;
        struct sockaddr_in ServAddr;
        int portno = atoi(ports[port + 1]);

        if ((Srv = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
            printf("Socket Error for %i\n", portno);
        }
        ServAddr.sin_family = AF_INET;
        ServAddr.sin_addr.s_addr = INADDR_ANY;
        ServAddr.sin_port = htons(portno);

        /* Binding socket*/
        if (bind(Srv, (struct sockaddr*) &ServAddr, sizeof(ServAddr)) < 0) {
            printf("Binding Error\n");
            exit(1);
        }
        /* Start Listening*/
        if (listen(Srv, 5) < 0) {
            printf("Listen() Error\n");
            exit(1);
        }
        ServSock[port] = Srv;
        /*Saving the max sockets*/
        if (ServSock[port] > maxDescriptor) {
            maxDescriptor = ServSock[port];
        }
    }
    return maxDescriptor;
}

void SocketsCloser(int ClntSock, int *ServSock, int NumPorts) {
    for (int port = 0; port < NumPorts; port++)
        close(ServSock[port]) ;
    close(ClntSock);
}

void threads_management(int ClntSock, int maxDescriptor, int *ServSock, int NumPorts) {
    fd_set SocketSet;
    struct timeval selTimeout;
    struct sockaddr_in ClntAddr;
    while(1) {
        FD_ZERO(&SocketSet);
        FD_SET(STDIN_FILENO, &SocketSet);
        for (int port = 0; port < NumPorts; port++) {
            FD_SET(ServSock[port], &SocketSet);
        }
        if (select(maxDescriptor + 1, &SocketSet, NULL, NULL, &selTimeout) < 0) {
            printf("select Error\n");
            exit(1);
        } else {
            for (int port = 0; port < NumPorts; port++) {
                if (FD_ISSET(ServSock[port], &SocketSet)) {
                    int ClntLen = sizeof(ClntAddr);
                    /* Waiting for a client to connect */
                    if ((ClntSock = accept(ServSock[port], (struct sockaddr *) &ClntAddr, &ClntLen))< 0) {
                        printf("Accept() Error\n");
                        exit(1);
                    }
                    pthread_t thread;
                    pthread_create(&thread, NULL, &ClientHandler, (void *) (intptr_t) ClntSock);    
                    }
                }
            }
        }
}

int main(int argc, char *argv[]) {
    int NumPorts = argc - 1;
    char buffer2[256];
    int ServSock[NumPorts];
    int ClntSock;

    int maxDescriptor = SocketsCreator(ServSock, NumPorts, argv);

    pid_t process_id;
    if ((process_id = fork()) < 0) {
        printf("fork() error\n");
        exit(1);
    }
    if (process_id == 0) { // Child Process(server)
        while (1) {
            write(1, "> ", sizeof("> "));
            bzero(buffer2, 256);
            read(0, buffer2, 255);
            ServerHandler(buffer2);
        }
    }
    if (process_id != 0) { // Parent process(client handler)
        threads_management(ClntSock, maxDescriptor, ServSock, NumPorts);
    }
    /* Close sockets */
    SocketsCloser(ClntSock, ServSock, NumPorts);

    return EXIT_SUCCESS;
}