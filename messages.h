#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#define MAX_TEXT 		256

#define INSCRIPTION_REQUEST 10
#define INSCRIPTION_OK		11
#define INSCRIPTION_KO 		12

/* struct message used between server and client */
typedef struct {
  int senderAccount;
  int beneficiaryAccount;
  int amount;
  int code;
  char messageText[MAX_TEXT];
} StructMessage;
#endif
