#ifndef _TRANSFER_H_
#define _TRANSFER_H_

/* struct message used between server and client */
typedef struct {
  int sender;
  int receiver;
  int amount;
} structTransfer;
#endif
