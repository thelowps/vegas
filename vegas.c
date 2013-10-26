/*
 * vegas.c
 *
 * Written by David Lopes and Ryan Boccabella
 * Cryptography, Fall 2013
 * University of Notre Dame, Computer Science 
 *
 * The client half of the vegas chat client.
 * 
 * One thread needs to be scanning input from the user and sending it.
 * The other thread needs to be listening for chats from the server
 * and printing them.
 * We can use ncurses to split the chatbox input and window, or work in our
 * own kind of output buffering. Either way, we need to solve the problem
 * of input interfering with output.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "eztcp.h"

// Threaded function to listen to sever input and print to the screen
void* listen_and_print (void* arg) {
  int sock = *((int*)arg);
  int msglen, error;
  char msg [100];
  while (1) {
    error = 0;
    error += ezreceive(sock, &msglen, sizeof(int));
    error += ezreceive(sock, msg, msglen);
    if (error <= 0) {
      close(sock);
      return NULL;
    } else {
      printf("%s\n", msg); 
    }
  }
}



int main (int argc, char** argv) {
  
  int sock;
  char username [50] = "derp_bot";
  ezconnect(&sock, "127.0.0.1", 9042);
  ezsend(sock, username, 50); // login

  // Spawn listening thread
  pthread_t listener;
  pthread_create(&listener, NULL, listen_and_print, &sock);

  while (1) {
    char msg [100];
    scanf("%s", msg);
    int len = strlen(msg);
    ezsend(sock, &len, sizeof(int));
    ezsend(sock, msg, strlen(msg));
  }

  pthread_exit(NULL);
  return 0;
}
