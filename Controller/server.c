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

void* ClientHandler(void *ClntSock) {
    char buffer[256];
    char *output[10];
    char *str_parse = malloc(100*sizeof(char));
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
        else if(verifRegex(buffer, commandPatterns[1]) == 1){
            char *id = extractString(buffer, extractCommand[1]);
            char *msg = authenticate(id, aquarium, (intptr_t) ClntSock);
            char auth[256];

            if(msg == NULL) {
                msg = "no greeting \n";
            }
            else{
               sprintf(auth, "greeting %s \n", msg);
            }
            
            length = write((intptr_t) ClntSock, auth, strlen(auth));

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
int SocketsCreator(int *ServSock, int NumPorts, char *ports[]) {
    int maxDescriptor = -1;
    for (int port = 0; port < NumPorts; port++) {
        int Srv;
        struct sockaddr_in ServAddr;
        int portno = atoi(ports[port + 1]);

        if ((Srv = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
            printf("[-] Socket()     Error for %i\n", portno);
        }
        ServAddr.sin_family = AF_INET;
        ServAddr.sin_addr.s_addr = INADDR_ANY;
        ServAddr.sin_port = htons(portno);

        /* Binding socket*/
        if (bind(Srv, (struct sockaddr*) &ServAddr, sizeof(ServAddr)) < 0) {
            printf("[-] Bind() Error\n");
            exit(1);
        }
        /* Start Listening*/
        if (listen(Srv, 5) < 0) {
            printf("[-] Listen() Error\n");
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
        if (NumPorts > 1) {
            if (select(maxDescriptor + 1, &SocketSet, NULL, NULL, &selTimeout) < 0) {
            printf("[-] select Error\n");
            exit(1);
            }
        }
        for (int port = 0; port < NumPorts; port++) {
            if (FD_ISSET(ServSock[port], &SocketSet)) {
                int ClntLen = sizeof(ClntAddr);
                    /* Waiting for a client to connect */
                if ((ClntSock = accept(ServSock[port], (struct sockaddr *) &ClntAddr, &ClntLen))< 0) {
                     printf("[-] Accept() Error\n");
                    exit(1);
                }
                pthread_t thread;
                pthread_create(&thread, NULL, &ClientHandler, (void *) (intptr_t) ClntSock);    
                }
            }
        }
}

void handle_interrupt(int signal) {
    printf("\n[-]   Keyboard interrupt detected. Exiting...\n");
    exit(1);
}

int main(int argc, char *argv[]) {
    int NumPorts = argc - 1;
    char buffer2[256];
    int ServSock[NumPorts];
    int ClntSock;

    int maxDescriptor = SocketsCreator(ServSock, NumPorts, argv);

    pid_t process_id;
    if ((process_id = fork()) < 0) {
        printf("[-] fork() error\n");
        exit(1);
    }
    if (process_id == 0) { // Child Process(server)
        signal(SIGINT, handle_interrupt);
        int empty_input = 0;
        while (1) {
            write(1, "$ ", sizeof("$ "));
            bzero(buffer2, 256);
            read(0, buffer2, 255);
            if (strlen(buffer2) == 0) {
                empty_input++;
            }
            if (empty_input == 1) {
                exit(1);
                break;
            }
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