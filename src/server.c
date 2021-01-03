#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "account.h"
#include "constant.h"
#include "message.h"
#include "network.h"
#include "room.h"

// Initialise all client_socket[] to 0 so not checked
int client_socket[MAX_CLIENTS] = {0};
Session sessions[MAX_CLIENTS] = {{NULL, NULL}};
ListAccountPtr accounts= NULL;
ListRoomPtr rooms = NULL;

int main(int argc, char *argv[]) {
  int master_socket, new_socket, activity, addrlen, max_sd, sd, i;
  struct sockaddr_in address;
  fd_set readfds;
  Message *request = (Message *)malloc(sizeof(Message));
  accounts = readData("src/account.txt");
  if (argc != 2) {
    puts("Wrong parameter");
  } else {
    master_socket = initServer(atoi(argv[1]));
    while (true) {
      // Clear the socket set
      FD_ZERO(&readfds);

      // Add master socket to set
      FD_SET(master_socket, &readfds);
      max_sd = master_socket;

      // Add child sockets to set
      for (i = 0; i < MAX_CLIENTS; i++) {
        // Socket descriptor
        sd = client_socket[i];

        // If valid socket descriptor then add to read list
        if (sd > 0) {
          FD_SET(sd, &readfds);
        }

        if (sd > max_sd) {
          max_sd = sd;
        }
      }

      // Wait for an activity on one of the sockets
      activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
      if (activity < 0) {
        perror("Select failed");
        exit(0);
      }

      // If something happened on the master socket ,
      // then its an incoming connection
      if (FD_ISSET(master_socket, &readfds)) {
        new_socket = acceptConnection(master_socket);
        for (i = 0; i < MAX_CLIENTS; i++) {
          if (client_socket[i] == 0) {
            client_socket[i] = new_socket;
            printf("Adding to list of sockets as %d\n", i);
            break;
          }
        }
      }

      // Else its some IO operation on some other socket
      for (i = 0; i < MAX_CLIENTS; i++) {
        sd = client_socket[i];
        if (FD_ISSET(sd, &readfds)) {
          // Check if it was for closing , and also read the
          // incoming message
          receiveData(sd, request);
          printf("Code: %d\nData: %s\n", request->code, request->data);
          switch(request->code){
            case SIGNIN:{
              signIn(i,request->data);
              break;
            }
            case SIGNUP:{
              signUp(i,request->data);
              break;
            }
            case CHANGE_PASSWORD:{
              changePassword(i,request->data);
              break;
            }
            case SHOW_RANK:{
              showRank(i);
              break;
            }
            case CREATE_ROOM:{
              createRoom(i);
              break;
            }
            case SHOW_ROOM:{
              showRoom(i);
              break;
            }
            case JOIN_ROOM:{
              joinRoom(i,request->data);
              break;
            }
            case LEAVE_ROOM:{
              leaveRoom(i);
              break;
            }
            case CHAT:{
              sendChatMessage(i,request->data);
              break;
            }
            case START_GAME:{
              startGame(i);
              break;
            }
            default:
              break;
          }
        }
      }
    }
  }

  return 0;
}
