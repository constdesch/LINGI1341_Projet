#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include <arpa/inet.h>
#include <zlib.h>
#include <endian.h>
typedef struct node {
  pkt_t *data; // pointeur vers la donnée stockée
  struct node *next; // pointeur vers le noeud suivant
  int envoi;
} Node;
typedef struct queue_pkt{
  int full;
  Node* head;
  Node* tail;
}queue_pkt_t;

void init_queue(queue_pkt_t* queue);
void free_Node(Node * node);
Node * addTail(queue_pkt_t *queue, pkt_t *data);
int deletePrevious(queue_pkt_t *queue, uint8_t seqnum) ;
pkt_t * getPos(queue_pkt_t * queue, int position);
pkt_t * queue_get_seq(queue_pkt_t * queue,uint8_t seqnum);
pkt_t * queue_get_timestamp(queue_pkt_t * queue,uint32_t timestamp);
int queue_delete_pkt_timestamp(queue_pkt_t * queue,uint32_t timestamp);
int set_pkt_envoi_timestamp(queue_pkt_t * queue,uint32_t timestamp);
