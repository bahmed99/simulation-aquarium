#include "server.h"

#define MAX_CLIENTS 4

int clients_fds[MAX_CLIENTS];
pthread_t thread_ids[MAX_CLIENTS];

void GetListString(char *String, char **output) {
    char * temp = strtok(String, " ");
    int i = 0;
    int newline_position;
    while (temp != NULL) {
        newline_position = strcspn(temp, "\n");
        if (newline_position < strlen(temp)) {
            temp[newline_position] = '\0';
        }
        output[i] =  temp;
        i++;
        temp = strtok(NULL, " ");
    }
}

void* ClientHandler(void *client_fd) {
    char buffer[256];
    char *output[10];
    char *str_parse = malloc(100*sizeof(char));
    while(1) {
        int length = read(*(int *)client_fd, buffer, 255);
        
        if (length < 0) {
            printf("ERROR reading from socket\n");
            exit(1);
        }
        if (strncmp(buffer, "status\n", strlen("status\n")) == 0) {
            length = write(*(int*) client_fd, "OK\n", 3);
            // status();   
        }
        else if (strncmp(buffer, "addFish\n", strlen("addFish\n")) == 0) {
            length = write(*(int*) client_fd, "OK\n", 3);
            // addFish();
        }
        else if (strncmp(buffer, "delFish\n", strlen("delFish\n")) == 0) {
            length = write(*(int*) client_fd, "OK\n", 3);
            // delFish();
        }
        else if (strncmp(buffer, "startFish\n", strlen("startFish\n")) == 0) {
            length = write(*(int*) client_fd, "OK\n", 3);
            // startFish();
        }
        else {
            length = write(*(int*) client_fd, "Command not found\n", strlen("Command not found\n"));
        }
    }
    return NULL;      
}

void ServerAction(void *buffer) {
    int n;
    char *output[10];
    char *str_parse = malloc(100*sizeof(char));

    if (strstr(buffer, "load")!= NULL) {
        //load_aquarium(); 
        GetListString(buffer, output);
        if (output[1] != NULL) {
            printf("%s loaded\n", output[1]);    
        } else {
            printf("[-] Enter a valid aquarium");
        }
    }

    else if (strncmp(buffer, "show aquarium", strlen("show aquarium")) == 0) {
        //show_aquarium();
    }

    else if (strstr(buffer, "save")!= NULL) {
        //load_aquarium(); 
        GetListString(buffer, output);
        if (output[1] != NULL) {
            printf("%s saved\n", output[1]);    
        } else {
            printf("[-] Enter a valid aquarium");
        }
    }

    else if (strstr(buffer, "add view")!= NULL) {
        //add_view();
        n = write(1, "view added\n", strlen("view added\n"));
    }

    else if (strstr(buffer, "del view")!= NULL) {
        //del_view();
        GetListString(buffer, output);
        printf("view %s deleted\n", output[2]);
    }

    else if (strncmp(buffer, "exit", strlen("exit")) == 0) {
        printf("GoodBye\nPress Ctrl+C to exit\n");
        exit(1);
    } else {}

    if (n < 0)  {
        exit(1);
    }
}
void * ServerHandler(void *) {
    char* buffer[256];
    while (1) {
        write(1, "$ ", sizeof("$ "));
        bzero(buffer, 256);
        read(0, buffer, 255);
        ServerAction(buffer);
    }
}

int SocketsCreator(int *ServSock, char* port) {
    char * buffer[256];
    int maxDescriptor = -1;
    struct sockaddr_in ServAddr;
    int portno = atoi(port);
    
    if ((*ServSock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        printf("[-] Socket()     Error for %i\n", portno);
    }
    ServAddr.sin_family = AF_INET;
    ServAddr.sin_addr.s_addr = INADDR_ANY;
    ServAddr.sin_port = htons(portno);

 
    if (bind(*ServSock, (struct sockaddr*) &ServAddr, sizeof(ServAddr)) < 0) {
        printf("[-] Bind() Error\n");
        exit(1);
    }

    if (listen(*ServSock, MAX_CLIENTS) < 0) {
        printf("[-] Listen() Error\n");
        exit(1);
    }
    maxDescriptor = *ServSock;
    
    pthread_t server_thread; 
    if (pthread_create(&server_thread, NULL, ServerHandler, NULL) < 0) {
        printf("[-] Server Thread Error");
        exit(1);
    }
    return maxDescriptor;
}

void SocketsCloser(int ClntSock, int ServSock) {
    close(ServSock) ;
    close(ClntSock);
}

void threads_management(int maxDescriptor, int ServSock, fd_set SocketSet) {
    struct sockaddr_in ClntAddr;
    char buffer2[256];
    while(1) {
        FD_ZERO(&SocketSet);
        FD_SET(ServSock, &SocketSet);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int fd = clients_fds[i];
            if (fd > 0) {
                FD_SET(fd, &SocketSet);
                if (fd > maxDescriptor) {
                    maxDescriptor = fd;
                }
            }
            if (select(maxDescriptor + 1, &SocketSet, NULL, NULL, NULL) < 0) {
                printf("[-] select Error\n");
                exit(1);
            }
            if (FD_ISSET(ServSock, &SocketSet)) {
                int ClntLen = sizeof(ClntAddr);
                int *new_ClntSock = malloc(sizeof(int));

                /* Waiting for a client to connect */
                if ((*new_ClntSock = accept(ServSock, (struct sockaddr *) &ClntAddr, &ClntLen)) < 0) {
                    printf("[-] Accept() Error\n");
                    exit(1);
                }
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (clients_fds[i] == 0) {
                        clients_fds[i] = *new_ClntSock;
                        if (pthread_create(&thread_ids[i], NULL, ClientHandler, &clients_fds[i]) != 0) {
                            printf("[-] Thread create error");
                            exit(1);
                        } 
                        break;
                    }
                }
            }
        }
    }
}

void handle_interrupt(int signal) {
    printf("\n[-]   Keyboard interrupt detected. Exiting...\n");
    exit(1);
}

int main(int argc, char *argv[]) {
    int ServSock;
    fd_set SocketSet;
    int maxDescriptor = SocketsCreator(&ServSock, argv[1]);
    for (int i = 0; i < MAX_CLIENTS; i++){
        clients_fds[i] = 0;
    }
    threads_management(maxDescriptor, ServSock, SocketSet);
    
    /* Close sockets */
    //SocketsCloser(ClntSock, ServSock);

    return EXIT_SUCCESS;
}