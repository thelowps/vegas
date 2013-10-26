/*
 * server_vegas.c
 *
 * Written by David Lopes and Ryan Boccabella
 * Cryptography, Fall 2013
 * University of Notre Dame, Computer Science 
 *
 * The server half of the vegas chat client.
 *
 * 1. Listen for incoming clients
 * 2. An incoming client will request logon by sending a username over.
 *    The server will remember this username and link it to this connection.
 *    The server will also send a confirmation signal to the client signifying
 *    that the logon was succesful.
 * 3. Clients that are logged in may request the creation of a chatroom.
 *    The server will store basic necessary information about this chatroom.
 *    The server will signal that the
 * 4. Clients that are logged in may request to enter a chatroom.
 *    The client will send a chatroom id and an encrypted password.
 *    The server will check that the chatroom exists. If it does, it will 
 *    forward the encrypted password to the owner of the chatroom.
 *    (How do we know who to send it to? What if the owner of the chatroom has left?
 *    Should we maybe send the client the names of everyone logged in? We need
 *    a designated password confirmer, and this job has to bounce around)
 * 5. 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eztcp.h"
#include "pthread.h"

#define NUM_CONNECTIONS 5

int connections [NUM_CONNECTIONS];

void* handle_client (void* arg) {
  int conn = *((int*)arg);
  char username [50];
  ezreceive(conn, username, 50);
  printf("%s has logged on.\n", username);
  
  int msglen, i, error = 0;
  char msg [100];
  while (1) {
    error = 0;
    error += ezreceive(conn, &msglen, sizeof(int));
    error += ezreceive(conn, msg, msglen);
    if (error <= 0) {
      close(conn);
      return NULL;
    }
    printf("Error is %d\n", error);
    
    for (i = 0; i < NUM_CONNECTIONS; ++i) {
      if (connections[i] == conn || connections[i] == -1) continue;
      ezsend(connections[i], &msglen, sizeof(int));
      ezsend(connections[i], msg, msglen);
      printf("SERVER: sending msg = %s to user %d\n", msg, i);
    }
    
  }
  
  close(conn);  
}

int main (int argc, char** argv) {
  
  int sock, conn, i;
  for (i = 0; i < NUM_CONNECTIONS; ++i) {
    connections[i] = -1;
  }

  ezlisten(&sock, 9042);
  for (i = 0; i < NUM_CONNECTIONS; ++i) {
    conn = ezaccept(sock); 
    if (conn < 0) return 1;

    connections[i] = conn;
    pthread_t thread;
    pthread_create(&thread, NULL, handle_client, &conn);
  }

  pthread_exit(NULL);
  return 0;
}
