#ifndef _TRANSFER_H_
#define _TRANSFER_H_

// Struc transfer between the server and client
typedef struct {
  int sender;
  int receiver;
  int amount;
} StructTransfer;

#endif