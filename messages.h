#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#define MAX_TEXT 		256

#define INSCRIPTION_REQUEST 10
#define TRANSFER_OK		11
#define TRANSFER_KO 		12

#include "transfer.h"

/* struct message used between server and client */
typedef struct {
  int newSolde;
  StructTransfer transfers[100];
  int sizeTransfers;
  char message[MAX_TEXT];
  int code;
} StructMessage;
#endif
