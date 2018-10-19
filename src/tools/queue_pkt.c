#include "pkt.h"
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include <arpa/inet.h>
#include <zlib.h>
#include <endian.h>


typedef struct node{
  pkt_t * data;
  struct node * next;
}Node;


typedef struct queue_pkt{
  int full;
  Node * head;
  Node *tail;
}queue_pkt_t;

void init_node(Node* node){
  node->data=NULL;
  node->next=NULL;

}
void init_queue(queue_pkt_t* queue){
  queue->full=0;
  queue->head=NULL;
  queue->tail=NULL;
}

void free_queue(queue_pkt_t* queue){
  if(queue!=NULL){
    if(!queue->head) free(queue->head);
    if(!queue->tail) free(queue->tail);
    free(queue);
  }
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
  if(node==NULL){
  fprintf(stdout,"malloc pour node failed \n");
  return NULL;
}
  node->data=data;
  if(queue->head==NULL) {
    queue->head=node;
    fprintf(stdout,"le payload du node à la head:%s\n",pkt_get_payload(data));
    node->next=NULL;
    queue->tail=node;
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
int deletePrevious(queue_pkt_t *queue, uint8_t seqnum) {
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
  pkt_t * queue_get_seq(queue_pkt_t * queue,uint8_t seqnum){
    if(queue==NULL){
      fprintf(stderr,"la queuee n'existe pas\n");
      return NULL;
    }
    if(queue->head==NULL){
      fprintf(stderr,"la queuee est vide \n");
      return NULL;
    }
    else{
      Node * node1=queue->head;
      while( node1!=NULL){
        pkt_t* pkt=node1->data;
        if(pkt_get_seqnum(pkt)==seqnum){
          return pkt;
        }
        node1=node1->next;
      }
      if(node1==NULL){
        fprintf(stderr,"il n'y a pas de pkt avec ce seqnum dans la liste \n");
        return NULL;
      }
  return NULL;
  }
}
pkt_t *queue_get_timestamp(queue_pkt_t * queue,uint32_t timestamp){
  if(queue==NULL){
    fprintf(stderr,"la queuee n'existe pas\n");
    return NULL;
  }
  if(queue->head==NULL){
    fprintf(stderr,"la queuee est vide \n");
    return NULL;
  }
  else{
    Node * node1=queue->head;
    while( node1!=NULL){
      pkt_t* pkt=node1->data;
      if(pkt_get_timestamp(pkt)==timestamp){
        return pkt;
      }
      node1=node1->next;
    }
    if(node1==NULL){
      fprintf(stderr,"il n'y a pas de pkt avec ce timestamp dans la liste \n");
      return NULL;
    }
return NULL;
}
}
int queue_delete_pkt_timestamp(queue_pkt_t * queue,uint32_t timestamp){
  if(queue==NULL){
    fprintf(stderr,"la queuee n'existe pas\n");
    return -1;
  }
  if(queue->head==NULL){
    fprintf(stderr,"la queue est vide \n");
    return -1;
  }
  else{
    Node * node1=queue->head;
    pkt_t * pkt=node1->data;
    if(node1->next==NULL && pkt_get_timestamp(pkt)!=timestamp){
      fprintf(stderr,"il n'y a pas de pkt avec ce timestamp dans la queue\n");
      return -1;
    }
    if(pkt_get_timestamp(pkt)==timestamp){
      Node* node2=node1;
      Node* node3=node1->next;
      free_Node(node2);
      queue->head=node3;
      queue->full--;
      return 1;
    }
  Node * node2=node1->next;
  pkt=node2->data;
    while(pkt_get_timestamp(pkt)!=timestamp && node2!=NULL){
      node1=node2;
      node2=node2->next;
      pkt=node2->data;
    }
    if(node2==NULL){
      fprintf(stderr,"il n'y a pas de pkt avec ce timestamp dans la queue\n");
      return -1;
    }
    else{
      Node * node3=node2;
      node1->next=node2->next;
      free_Node(node3);
      if(node1->next==NULL){
        queue->tail=node1;
      }
      queue->full--;
      return 1;
    }
  }
}
