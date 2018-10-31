/* The main function of this file tests almost every
   function of the file queue_pkt.c which contains
   all the things we wrote to use our linked lists.

   On succesful tests     : return 1
   On unsuccesfull tests  : return 0
   On error               : return -1
   */





/* 1 on success, 0 if queues are not equal, -1 on error */

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include "/etinfo/users/2018/codeschaetze/oasis/LINGI1341_Projet/projet1_deSchaetzen_Gillain/src/tools/connect.h"
#include "/etinfo/users/2018/codeschaetze/oasis/LINGI1341_Projet/projet1_deSchaetzen_Gillain/src/tools/pkt.h"
#include "/etinfo/users/2018/codeschaetze/oasis/LINGI1341_Projet/projet1_deSchaetzen_Gillain/src/tools/queue_pkt.h"
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int compareQueue(queue_pkt_t *queue1,queue_pkt_t *queue2);
int compareNode(Node *node1, Node *node2);
int comparePkt(pkt_t *pkt1, pkt_t *pkt2);


int compareQueue(queue_pkt_t *queue1,queue_pkt_t *queue2){
  if(queue1==NULL && queue2==NULL){
    printf("Both queues are NULL\n");
    return 1;
  }
  if(queue1==NULL || queue2==NULL){
    printf("One of the queues is NULL\n");
    return 0;
  }
  if(queue1->full != queue2->full) return 0;
  if(compareNode(queue1->head,queue2->head) != 1) return 0;
  if(compareNode(queue1->tail,queue2->tail) != 1) return 0;
  Node *node1 = malloc(sizeof(Node));
  if(!node1){printf("Malloc failed.\n"); return -1;}
  Node *node2 = malloc(sizeof(Node));
  if(!node2){printf("Malloc failed.\n"); free_Node(node2); return -1;}
  node1 = queue1->head;
  node2 = queue2->head;
  while(node1!=NULL && node2!=NULL){
    if(compareNode(node1,node2)!=0) {free_Node(node1); free_Node(node2); return 0;}
    node1 = node1->next;
    node2 = node2->next;
  }
  if(node1!=NULL || node2!=NULL){
    if(node1!=NULL) free_Node(node1);
    if(node2!=NULL) free_Node(node2);
    return 0;
  }
  return 1;
}

int compareNode(Node *node1, Node *node2){
  if(node1==NULL && node2 ==NULL){
    printf("Both nodes are NULL\n");
    return 1;
  }
  if(node1==NULL || node2==NULL){
    printf("One of the nodes is NULL\n");
    return 1;
  }
  return comparePkt(node1->data,node2->data);

}

int comparePkt(pkt_t *pkt1, pkt_t *pkt2){
  if(pkt_get_type(pkt1)!=pkt_get_type(pkt2)) return 0;
  if(pkt_get_tr(pkt1)  !=pkt_get_tr(pkt2))   return 0;
  if(pkt_get_timestamp(pkt1)!=pkt_get_timestamp(pkt2)) return 0;
  if(pkt_get_seqnum(pkt1)!= pkt_get_seqnum(pkt2)) return 0;
  if(pkt_get_length(pkt1) != pkt_get_length(pkt2)) return 0;
  if(pkt_get_crc1(pkt1) != pkt_get_crc1(pkt2)) return 0;
  if(pkt_get_crc2(pkt1) != pkt_get_crc2(pkt2)) return 0;
  if(pkt_get_window(pkt1) != pkt_get_window(pkt2)) return 0;
  return 1;
}



int main(int argc, char* argv[]){
  /* En imaginant que les noeuds et les structures soient initialisées*/

queue_pkt_t *queue1 = malloc(sizeof(queue_pkt_t));
queue_pkt_t *queue2 = malloc(sizeof(queue_pkt_t));
init_queue(queue1);
init_queue(queue2);

pkt_t *pkt1 = pkt_new(); pkt_set_seqnum(pkt1,1);
pkt_t *pkt2 = pkt_new(); pkt_set_seqnum(pkt2,2);
pkt_t *pkt3 = pkt_new(); pkt_set_seqnum(pkt3,3);

//pkt_t *pkt4 = pkt_new();

Node *Node1 = addTail(queue1, pkt1);
Node *Node2 = addTail(queue1, pkt2);
Node *Node3 = addTail(queue2, pkt1);
Node *Node4 = addTail(queue2, pkt2);
if(Node1==NULL) return 0;
if(Node2==NULL) return 0;
if(Node3==NULL) return 0;
if(Node4==NULL) return 0;
if(compareQueue(queue1,queue2)!=1){
  printf("addTail function does not seem to work.1\n");
  return 0;
}
Node *Node5 = addTail(queue1, pkt3);
if(Node5==NULL) return 0;

if(compareQueue(queue1,queue2)!=0){
  printf("addTail function does not seem to work.2\n");
  return 0;
}
int err = deletePrevious(queue1,1);
if(err==-1){printf("deletePrevious error\n"); return -1;}

if(compareNode(queue1->head, Node2)!=1){
  printf("deletePrevious does not seem to work.\n");
  return 0;
}
/*err = deletePrevious(queue1,3);
if(err==-1){printf("deletePrevious error\n"); return -1;}
if(queue1->head != NULL || queue1->tail != NULL){
  printf("Queue is not NULL after deleting all nodes.\n");
  return 0;
}*/
/*pkt4 = queue_get_seq(queue2, 2);
if(comparePkt(pkt4,pkt2)!=1){
  printf("Get seq is not correct.\n");
  return 0;
}*/
printf("Every tests passed, you did well young padawan.\n");
//free(queue1);
//free(queue2);

} /*end of main */
