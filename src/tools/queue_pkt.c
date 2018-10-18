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
Node * addTail(queue_pkt *list, pkt_t *data) {
  Node *node= (Node *) malloc(sizeof(Node));
  if(node==NULL)
  return NULL;
  node->data=data;
  if(list->head==NULL) {
    list->head=node;
    node->next=NULL;
    list->tail=NULL;
    list->full++;
    return node;
  } else {
  Node *node1=list->tail;
  node1->next=node;
  node->next=NULL;
  list->full++;
  list->tail=node;
  list->full++;
  return node;
}
int deletePrevious(queue_pkt *list, int seqnum) {
  if(list==NULL)
  {return -1;}
  if(list->head==NULL){
    return -1;
  }
  Node *node1=list->head;
  int count=0;
  while(count!=seqnum){
    Node *node3=node1;
    if(node1->next!=NULL){
      node1=node1->next;
      free_Node(node3);
      list->full--;
      count++;
      list->head==node1;
    }
    else{
      free_Node(node3);
      list->full--;
      count++;
      list->head=NULL;
      list->tail==NULL;
      return count;
    }
  }
  return count;
}
pkt_t * getPos(queue_pkt * list, int position){
  if(list==NULL){
    fprintf(stderr,"la liste n'existe pas\n");
    return NULL;
  }
  if(list->head==NULL){
    fprintf(stder,"la liste est vide \n");
    return NULL;
  }
  else{
    int pos=0;
    Node * node1=list->head;
    while(pos!=position && node1!=NULL){
      node1=node1->next;
      pos++;
    }
    if(node1==NULL){
      fprintf(stderr,"il n'y a pas autant de positions dans la liste \n");
      return NULL;
    }
    pkt_t* pkt=node1->data;
    return pkt;
    }
  }
}
