#ifndef _SERVER_H
#define _SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <signal.h>

/********* Déclaration des fonctions *********/

/**
 * @brief 
 * 
 * @param  ClntSock
 * @return NULL 
 */
void* ClientHandler(void *ClntSock);


/**
 * @brief 
 * 
 * @param buffer  
 */
void ServerHandler(char *buffer);


/**
 * @brief 
 * 
 * @param ServSock
 * @param NumPorts
 * @param ports
 * 
 */
int SocketsCreator(int *ServSock, int NumPorts, char *ports[]);

/**
 * @brief 
 * 
 * @param ClntSock
 * @param ServSock
 * @param NumPorts
 */
void SocketsCloser(int ClntSock, int *ServSock, int NumPorts);


/**
 * @brief 
 * 
 * @param ClntSock
 * @param maxDescriptor
 * @param ServSock
 * @param NumPorts
 */
void threads_management(int ClntSock, int maxDescriptor, int *ServSock, int NumPorts);


/**
 * @brief 
 * 
 * @param signal
 * 
 */
void handle_interrupt(int signal);
#endif