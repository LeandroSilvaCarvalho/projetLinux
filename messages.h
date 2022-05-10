#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#define MAX_TEXT 		256

#define INSCRIPTION_REQUEST 10
#define INSCRIPTION_OK		11
#define INSCRIPTION_KO 		12

#include "transfer.h"

/* struct message used between server and client */
typedef struct {
  char messageText[MAX_TEXT];
  StructTransfer transfers[100];
  int sizeTransfers;
  int code;
} StructMessage;
#endif
