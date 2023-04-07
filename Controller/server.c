#include "server.h"
#include "utils.h"

Aquarium *aquarium = NULL;
// regular expressions to handle server commands
char *serverCommand[] = {
    "^load [a-zA-Z0-9]+$",
    "^add view N[0-9]+ [0-9]+x[0-9]+\\+[0-9]+\\+[0-9]+$",
    "^del view N[0-9]+$",
    "^save [a-zA-Z0-9]+$",

};
char *extractServerCommand[] = {
    "^load ([a-zA-Z0-9]+)$",
    "^add view (N[0-9]+) ([0-9]+)x([0-9]+)\\+([0-9]+)\\+([0-9]+)$",
    "^del view (N[0-9]+)$",
    "^save ([a-zA-Z0-9]+)$",

};
// regular expressions to handle client commands
char *clientCommand[] = {
    "^hello( in as [a-zA-Z0-9]+)?$",
    "^addFish [a-zA-Z0-9]+ at [0-9]+x[0-9]+, [0-9]+x[0-9]+, [a-zA-Z]+$",
    "^delFish [a-zA-Z0-9]+$",
    "^startFish [a-zA-Z0-9]+$",
    "^ping [0-9]{1,5}$",
    "log out"

};

char *extractClientCommand[] = {
    "^hello in as ([a-zA-Z0-9]+)$",
    "^addFish ([a-zA-Z0-9]+) at ([0-9]+)x([0-9]+), ([0-9]+)x([0-9]+), ([a-zA-Z]+)$",
    "^delFish ([a-zA-Z0-9]+)$",
    "^startFish ([a-zA-Z0-9]+)$",
    "^ping ([0-9]{1,5})$"

};

#define MAX_CLIENTS 4

int clients_fds[MAX_CLIENTS];
pthread_t thread_ids[MAX_CLIENTS];

void GetListString(char *String, char **output)
{
    char *temp = strtok(String, " ");
    int i = 0;
    int newline_position;
    while (temp != NULL)
    {
        newline_position = strcspn(temp, "\n");
        if (newline_position < strlen(temp))
        {
            temp[newline_position] = '\0';
        }
        output[i] = temp;
        i++;
        temp = strtok(NULL, " ");
    }
}
void *ClientHandler(void *client_fd)
{
    char buffer[256];
    int length_write;

    while (1)
    {
        int length = read(*(int *)client_fd, buffer, 255);

        if (length < 0)
        {
            printf("ERROR reading from socket\n");
            exit(1);
        }
        if (strncmp(buffer, "status\n", strlen("status\n")) == 0)
        {
            char *msg = status(aquarium, *(int *)client_fd);
            length_write = write(*(int *)client_fd, msg, strlen(msg));
            free(msg);
        }
        else if (verifRegex(buffer, clientCommand[0]) == 1)
        {

            char *id;

            id = extractString(buffer, extractClientCommand[0]);

            char *msg = authenticate(id, aquarium, *(int *)client_fd);
            char auth[256];

            if (msg == NULL)
            {
                msg = "no greeting \n";
            }
            else
            {
                sprintf(auth, "greeting %s \n", msg);
            }

            length_write = write(*(int *)client_fd, auth, strlen(auth));
        }
        // add Fish
        else if (verifRegex(buffer, clientCommand[1]) == 1)
        {
        char **params = extractStrings(buffer, extractClientCommand[1], 6); 
        char* name = params[0];
        int x = atoi(params[1]);
        int y = atoi(params[2]);
        int width = atoi(params[3]);
        int height = atoi(params[4]);
        char* mobiliypattern = params[5];
        char* message = addFish(aquarium, *(int *)client_fd, name, x, y, width, height, mobiliypattern);
        length_write = write(*(int *)client_fd, message, strlen(message));
        }

        else if (verifRegex(buffer, clientCommand[2]) == 1)
        {
            char *name = extractStrings(buffer, extractClientCommand[2], 1)[0];
            int status = deleteFish(aquarium, *(int *)client_fd, name);
            if (status == 1)
            {
                length_write = write(*(int *)client_fd, "OK\n", 3);
            }
            else
            {
                length_write = write(*(int *)client_fd, "NOK :Poisson inexistant \n", 4);
            }
        }
        else if (verifRegex(buffer, clientCommand[3]) == 1)
        {
            char *name = extractStrings(buffer, extractClientCommand[3], 1)[0];
            // startFish()
        }
        else if (verifRegex(buffer, clientCommand[4]) == 1)
        {
            char *name = extractStrings(buffer, extractClientCommand[4], 1)[0];
            char *msg = pong(name);
            length_write = write(*(int *)client_fd, msg, strlen(msg));
            free(msg);
        }
        else if (verifRegex(buffer, clientCommand[5]) == 1)
        {
            disconnect(aquarium, *(int *)client_fd);
            length_write = write(*(int *)client_fd, "bye\n", 4);
            close(*(int *)client_fd);
            pthread_exit(NULL);
        }
        else
        {
            length_write = write(*(int *)client_fd, "NOK : commande introuvable\n", strlen("NOK : commande introuvable\n"));
        }

        if (length == 0)
        {

            // Client has disconnected, close the socket and exit the thread
            disconnect(aquarium, *(int *)client_fd);
            close(*(int *)client_fd);
            pthread_exit(NULL);
        }
    }
    return NULL;
}

void ServerAction(void *buffer)
{
    int n;
    char *output[10];
    char *str_parse = malloc(100 * sizeof(char));
    // debug
    //printf("le contenu du buffer est : %s", (char *)buffer);
    // load aquarium
    if (verifRegex(buffer, serverCommand[0]) == 1)
    {
        char **params = extractStrings(buffer, extractServerCommand[0], 1);
        aquarium = loadAquarium(params[0]);
    }

    // show aquarium
    else if (strncmp(buffer, "show aquarium", strlen("show aquarium")) == 0)
    {
        showAquarium(aquarium);
    }

    // add view
    else if (verifRegex(buffer, serverCommand[2]) == 1)
    {
        char **params = extractStrings(buffer, extractServerCommand[2], 5);
        int x = atoi(params[1]);
        int y = atoi(params[2]);
        int width = atoi(params[3]);
        int height = atoi(params[4]);
        addView(aquarium, params[0], x, y, width, height);
        for (int i = 0; i < 5; i++)
        {
            free(params[i]);
        }
        free(params);
    }
    // delete view
    else if (verifRegex(buffer, serverCommand[3]) == 1)
    {
        char **params = extractStrings(buffer, extractServerCommand[3], 1);
        deleteView(aquarium, params[0]);
    }
    // save aquarium
    else if (verifRegex(buffer, serverCommand[4]) == 1)
    {
        char **params = extractStrings(buffer, extractServerCommand[4], 1);
        saveAquarium(aquarium, params[0]);
    }

    else
    {
        printf("Command not found\n");
    }

    if (n < 0)
    {
        exit(1);
    }
}
void *ServerHandler(void *)
{
    char *buffer[256];
    while (1)
    {
        write(1, "$ ", sizeof("$ "));
        bzero(buffer, 256);
        read(0, buffer, 255);
        ServerAction(buffer);
    }
}

int SocketsCreator(int *ServSock, char *port)
{
    char *buffer[256];
    int maxDescriptor = -1;
    struct sockaddr_in ServAddr;
    int portno = atoi(port);

    if ((*ServSock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("[-] Socket()     Error for %i\n", portno);
    }
    ServAddr.sin_family = AF_INET;
    ServAddr.sin_addr.s_addr = INADDR_ANY;
    ServAddr.sin_port = htons(portno);

    if (bind(*ServSock, (struct sockaddr *)&ServAddr, sizeof(ServAddr)) < 0)
    {
        printf("[-] Bind() Error\n");
        exit(1);
    }

    if (listen(*ServSock, MAX_CLIENTS) < 0)
    {
        printf("[-] Listen() Error\n");
        exit(1);
    }
    maxDescriptor = *ServSock;

    pthread_t server_thread;
    if (pthread_create(&server_thread, NULL, ServerHandler, NULL) < 0)
    {
        printf("[-] Server Thread Error");
        exit(1);
    }
    return maxDescriptor;
}

void threads_management(int maxDescriptor, int ServSock, fd_set SocketSet)
{
    struct sockaddr_in ClntAddr;
    char buffer2[256];
    while (1)
    {
        FD_ZERO(&SocketSet);
        FD_SET(ServSock, &SocketSet);
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int fd = clients_fds[i];
            if (fd > 0)
            {
                FD_SET(fd, &SocketSet);
                if (fd > maxDescriptor)
                {
                    maxDescriptor = fd;
                }
            }
            if (select(maxDescriptor + 1, &SocketSet, NULL, NULL, NULL) < 0)
            {
                printf("[-] select Error\n");
                exit(1);
            }
            if (FD_ISSET(ServSock, &SocketSet))
            {
                int ClntLen = sizeof(ClntAddr);
                int *new_ClntSock = malloc(sizeof(int));

                /* Waiting for a client to connect */
                if ((*new_ClntSock = accept(ServSock, (struct sockaddr *)&ClntAddr, &ClntLen)) < 0)
                {
                    printf("[-] Accept() Error\n");
                    exit(1);
                }
                for (int i = 0; i < MAX_CLIENTS; i++)
                {
                    if (clients_fds[i] == 0)
                    {
                        clients_fds[i] = *new_ClntSock;
                        if (pthread_create(&thread_ids[i], NULL, ClientHandler, &clients_fds[i]) != 0)
                        {
                            printf("[-] Thread create error");
                            exit(1);
                        }
                        break;
                    }
                    if (i == MAX_CLIENTS - 1)
                    {
                        close(*new_ClntSock);
                        continue;
                    }
                }
            }
        }
    }
}

void handle_interrupt(int signal)
{
    printf("\n[-]   Keyboard interrupt detected. Exiting...\n");
    exit(1);
}
int ExtractPort()
{
    FILE *config_file;
    char buffer[1024];
    int port;
    config_file = fopen("controller.cfg", "r");
    if (config_file == NULL)
    {
        perror("Error opening config file");
        exit(1);
    }

    while (fgets(buffer, sizeof(buffer), config_file) != NULL)
    {
        if (strstr(buffer, "controller-port =") != NULL)
        {
            port = atoi(strstr(buffer, "=") + 1);
            break;
        }
    }
    fclose(config_file);
    return port;
}

int main(int argc, char *argv[])
{
    char port[5];
    sprintf(port, "%d", ExtractPort());
    int ServSock;
    fd_set SocketSet;
    int maxDescriptor = SocketsCreator(&ServSock, port);
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        clients_fds[i] = 0;
    }
    threads_management(maxDescriptor, ServSock, SocketSet);
    close(ServSock);

    return EXIT_SUCCESS;
}