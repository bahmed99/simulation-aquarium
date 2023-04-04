#include "server.h"
#include "utils.h"

Aquarium* aquarium = NULL;

char *commandPatterns[] = {
    "^load [a-zA-Z0-9]+$",
    "^hello( in as [a-zA-Z0-9]+)?$"
};

char *extractCommand[]={
    "^load ([a-zA-Z0-9]+)$",
    "^hello in as ([a-zA-Z0-9]+)$"
};

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
    int length_write;
    
    while(1) {
        int length = read(*(int *)client_fd, buffer, 255);
       
        if (length < 0) {
            printf("ERROR reading from socket\n");
            exit(1);
        }
        if (strncmp(buffer, "status\n", strlen("status\n")) == 0) {
            char *msg = status(aquarium, *(int*) client_fd);
            length_write = write(*(int*) client_fd, msg, strlen(msg));
            free(msg);
        }
        else if(verifRegex(buffer, commandPatterns[1]) == 1){
            char *id = extractString(buffer, extractCommand[1]);
            char *msg = authenticate(id, aquarium, *(int*) client_fd);
            char auth[256];

            if(msg == NULL) {
                msg = "no greeting \n";
            }
            else{
               sprintf(auth, "greeting %s \n", msg);
            }
            
            length_write = write(*(int*) client_fd, auth, strlen(auth));

        }
        else if (strncmp(buffer, "addFish\n", strlen("addFish\n")) == 0) {
            length_write = write(*(int*) client_fd, "OK\n", 3);
            // addFish();
        }
        else if (strncmp(buffer, "delFish\n", strlen("delFish\n")) == 0) {
            length_write = write(*(int*) client_fd, "OK\n", 3);
            // delFish();
        }
        else if (strncmp(buffer, "startFish\n", strlen("startFish\n")) == 0) {
            length_write = write(*(int*) client_fd, "OK\n", 3);
            // startFish();
        }
        else {
            length_write = write(*(int*) client_fd, "NOK : commande introuvable\n", strlen("NOK : commande introuvable\n"));
        }
       
        if (length == 0) {

            // Client has disconnected, close the socket and exit the thread
            disconnect(aquarium, *(int*)client_fd);
            close(*(int*)client_fd);
            pthread_exit(NULL);
        }
    }
    return NULL;      
}

void ServerAction(void *buffer) {
    int n;
    char *output[10];
    char *str_parse = malloc(100*sizeof(char));
  

    if (verifRegex(buffer, commandPatterns[0]) == 1) {
        aquarium=loadAquarium(extractString(buffer, extractCommand[0]));
    } 

    else if (strncmp(buffer, "show aquarium", strlen("show aquarium")) == 0) {
        showAquarium(aquarium);
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
    } else {
        printf("Command not found\n");
    }

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
                    if (i == MAX_CLIENTS -1) {
                        close(*new_ClntSock);
                        continue;
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
int ExtractPort() {
    FILE *config_file;
    char buffer[1024];
    int port;
    config_file = fopen("controller.cfg", "r");
    if (config_file == NULL) {
        perror("Error opening config file");
        exit(1);
    }

    while (fgets(buffer, sizeof(buffer), config_file) != NULL) {
        if (strstr(buffer, "controller-port =") != NULL) {
            port = atoi(strstr(buffer, "=") + 1);
            break;
        }
    }
    fclose(config_file);
    return port;
}

int main(int argc, char *argv[]) {
    char port[5];
    sprintf(port, "%d", ExtractPort());
    int ServSock;
    fd_set SocketSet;
    int maxDescriptor = SocketsCreator(&ServSock, port);
    for (int i = 0; i < MAX_CLIENTS; i++){
        clients_fds[i] = 0;
    }
    threads_management(maxDescriptor, ServSock, SocketSet);
    close(ServSock);

    return EXIT_SUCCESS;
}