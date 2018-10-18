#include "pkt.h"
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include <arpa/inet.h>
#include <zlib.h>
#include <endian.h>
typedef struct node {
  pkt_t *data; // pointeur vers la donnée stockée
  struct node *next; // pointeur vers le noeud suivant
} Node;
typedef struct queue_pkt{
  int full;
  Node* head;
  Node* tail;
}queue_pkt_t;
void init(queue_pkt_t* queue){
  queue->full=0;
  queue->head=NULL;
  queue->tail=NULL;
}
void free_Node(Node * node){
  if(node!=NULL){
    if(node->data!=NULL){
      free(node->data);
    }
    free(node);
  }
}
Node * addTail(queue_pkt_t *queue, pkt_t *data) {
  Node *node= (Node *) malloc(sizeof(Node));
  if(node==NULL)
  return NULL;
  node->data=data;
  if(queue->head==NULL) {
    queue->head=node;
    node->next=NULL;
    queue->tail=NULL;
    queue->full++;
    return node;
  } else {
  Node *node1=queue->tail;
  node1->next=node;
  node->next=NULL;
  queue->full++;
  queue->tail=node;
  return node;
}
}
int deletePrevious(queue_pkt_t *queue, int seqnum) {
  if(queue->head==NULL){
    return -1;
  }
  Node *node1=queue->head;
  int count=0;
  while(count!=seqnum){
    Node *node3=node1;
    if(node1->next!=NULL){
      node1=node1->next;
      free_Node(node3);
      queue->full--;
      count++;
      queue->head=node1;
    }
    else{
      free_Node(node3);
      queue->full--;
      count++;
      queue->head=NULL;
      queue->tail=NULL;
      return count;
    }
  }
  return count;
}
pkt_t * getPos(queue_pkt_t * queue, int position){
  if(queue==NULL){
    fprintf(stderr,"la queuee n'existe pas\n");
    return NULL;
  }
  if(queue->head==NULL){
    fprintf(stderr,"la queuee est vide \n");
    return NULL;
  }
  else{
    int pos=0;
    Node * node1=queue->head;
    while(pos!=position && node1!=NULL){
      node1=node1->next;
      pos++;
    }
    if(node1==NULL){
      fprintf(stderr,"il n'y a pas autant de positions dans la queuee \n");
      return NULL;
    }
    pkt_t* pkt=node1->data;
    return pkt;
    }
  }
