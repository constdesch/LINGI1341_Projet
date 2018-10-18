#include
typedef struct queue_pkt{
  int full;
  Node* head;
  Node* tail;

}
void init(*queue_pkt queue){
  queue->full=0;
  queue->head=NULL;
  queue->tail=NULL;
}
typedef struct node {
  pkt_t *data; // pointeur vers la donnée stockée
  struct node *next; // pointeur vers le noeud suivant
} Node
void free_Node(Node * node){
  if(node!=NULL){
    if(node->data!=NULL){
      free(node->data);
    }
    free(node);
  }
}
Node * addHead(queue_pkt *list, pkt_t *data) {
  Node *node= (Node *) malloc(sizeof(Node));
  if(node==NULL)
  return NULL;
  node->data=data;
  if(list->head==NULL) {
    list->tail=node;
    node->next=NULL;
  } else {
    node->next=list->head;
  }
  list->head=node;
  list->full++;
  return node;
}
int deleteAllYounger(queue_pkt *list, int age) {
  if(list==NULL)
  {return -1;}
  if(list->head==NULL){
    return -1;
  }
  Node *node1=list->head;
  pkt_t * pkt=node1->data;
  int count=0;
  while(pkt->window<age){
    Node *node3=node1;
    if(node1->next!=NULL){
      node1=node1->next;
      empl=node1->data;
      free(node3);
      list->full--;
      count++;
      list->head=node1;
    }
    else{
      node1=node1->next;
      free(node3);
      list->head=node1;
      list->tail=node1;
      count++;
      list->full--;
      return count;
    }
  }
  if(node1->next==NULL){
    list->tail=node1;
    list->head=node1;
    return count;
  }
  Node *node2=node1->next;
  pkt_t *pkt1=node2->data;
  while(node2!=NULL){
    empl1=node2->data;
    if(pkt1->window<age){
      if(node2!=list->tail){
        Node *node3=node2;
        node1->next=node2->next;
        node2=node2->next;
        free(node3);
        count++;
        list->full--;
      }
      else{
        Node *node3=node2;
        node1->next=node2->next;
        node2=node2->next;
        free(node3);
        count++;
        list->full--;
        list->tail=node1;
        return count;
      }
    }
    else{
      node1=node1->next;
      node2=node2->next;
    }
  }
  return count;
}
