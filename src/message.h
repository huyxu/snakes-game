#ifndef MESSAGE_H
#define MESSAGE_H

#include "constant.h"
typedef struct message {
  OPCODE code;
  char data[MAX]; 
} Message;

void splitData(char **argv, char *data);
void splitData2(char **argv, char *data);
int numOfArgv(char *data);

#endif