#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#define MAX_TEXT 		256
#define TRANSFER_OK		11
#define TRANSFER_KO 		12

#include "transfer.h"

// Struc message used between the server and client
typedef struct {
  int newBalance;
  StructTransfer transfers[100];
  int sizeTransfers;
  char message[MAX_TEXT];
  int code;
} StructMessage;

#endif