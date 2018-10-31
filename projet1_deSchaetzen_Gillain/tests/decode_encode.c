
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
#include "tools/connect.h"
#include "tools/pkt.h"
#include "tools/queue_pkt.h"
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int comparePkt(pkt_t *pkt1, pkt_t pkt2){
  if(pkt_get_type(pkt1)!=pkt_get_type(pkt2)) return 0;
  if(pkt_get_tr(pkt1)  !=pkt_get_tr(pkt2))   return 0;
  if(pkt_get_timestamp(pkt1)!=pkt_get_timestamp(pkt2)) return 0;
  if(pkt_get_seqnum(pkt1)!= pkt_get_seqnum(pkt2)) return 0;
  if(pkt_get_length(pkt1) != pkt_get_length(pkt2)) return 0;
  if(pkt_get_crc1(pkt1) != pkt_get_crc1(pkt2)) return 0;
  if(pkt_get_crc2(pkt1) != pkt_get_crc2(pkt2)) return 0;
  if(pkt_get_window(pkt1) != pkt_get_window(pkt2)) return 0;
  if(strcmp(pkt_get_payload(pkt1),pkt_get_payload(pkt2))!=0) return 0;
  return 1;
}
int main(int argc, char *argv[]){

  pkt_t *pkt = pkt_new();
  pkt_t pkt1 = pkt_new();

  pkt_set_tr(pkt,0);
  pkt_set_seqnum(pkt,1);
  pkt_set_type(pkt,1);
  char data[] = "On teste encode et decode";
  pkt_set_timestamp(1000);
  pkt_set_payload(pkt,data,strlen(data));
  pkt_set_length(strlen(data));
  uLong crc1 = crc32(0L, Z_NULL, 0);
  crc1=crc32(crc1,(Bytef*) pkt,8);
  pkt_set_crc1(pkt,htonl(crc1));
  uLong crc2= crc32(0L, Z_NULL, 0);
  crc2= crc32(crc2, (Bytef *) pkt_get_payload(pkt),htons(strlen(data)));
  pkt_set_crc2(pkt,htonl(crc2));
  char *buf = malloc(strlen(data) +16);
  if(!buf) {return -1;}

  encode(pkt,buf,&(strlen(data) +16));
  decode(buf,strlen(buf),pkt1);

  int comp = comparePkt(pkt,pkt1);
  if(comp == 1){
    printf("Encode et Decode fonctionnent bien\n");
  }
  else printf("Encode et Decode ne fonctionnent pas\n");
  pkt_del(pkt);
  pkt_del(pkt1);

  return 0;
}
