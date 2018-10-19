//
//  sender.c
//
//
//  Created by Constantin de Schaetzen and Jean Gillain on 16/10/18.
//


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
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

#define true 1
#define false 0





int main(int argc, char* argv[]){
  queue_pkt_t * queue=malloc(sizeof(queue_pkt_t));
  if(!queue){
    printf("malloc failed \n");
    return -1;
  }
  init_queue(queue);
  int erreur=0;
    int port = 0;
    int opt;
    char *filename = NULL;
    const char *err;
    pkt_status_code status;
    uint32_t seqnum = 0;
    char * receiver = "::1";
    char * sender= NULL;
    struct timeval tv;
    fd_set readfds;
    tv.tv_sec = 15;
    tv.tv_usec = 5;
  //  int openall=0;

    while ((opt = getopt(argc, argv, "f:")) != -1) {
        switch (opt) {
            case 'f':
                filename = optarg;
                break;
            case '?':
                if(optopt =='c')
                    fprintf(stderr,"Option -f requires an argument .\n");
                else
                    fprintf (stderr,
                             "Unknown option character `\\x%x'.\n",
                             optopt);
                return 1;
            default:
                abort();
        }
    }
    if(argc - optind <2) {fprintf(stderr,"Wrong number of arguments, expected at least two, got %d\n",argc-optind);free(queue); return -1;}
    sender = argv[optind]; /*sender est le premier argument */
  //  if(strlen(sender)==0 && sender[0]=='.' && sender[1]=='.'){
    //  openall=1;
    //}
    printf("sender:%s \n",sender);
    port = atoi(argv[optind+1]); /*port est le deuxieme argument */
    if(!sender) fprintf(stderr, "Sender is NULL\n");
    if(!port) fprintf(stderr, "Port is 0\n");
    printf("Sender : %s\n",sender);
    printf("Port : %d\n",port);

    /* Option -f mentionned */
    struct sockaddr_in6 dst_addr;
    err = real_address(sender,&dst_addr);
    if (err){
      fprintf(stderr, "Could not resolve sender name %s : %s\n",sender,err);
    }
    /* Resolve sender name */
    struct sockaddr_in6 src_addr;
    err = real_address(receiver,&src_addr);
    if(err){
      fprintf(stderr,"Could not resolve sender name %s : %s\n",sender,err);
    }
    int sfd = create_socket(&src_addr, port,NULL, -1); /* Connected */ /* src_port = dst_port ? */
    if (sfd > 0 && wait_for_client(sfd) < 0) { /* Connected */
			fprintf(stderr,
					"Could not connect the socket after the first message.\n");
			close(sfd);
      free(queue);
			return EXIT_FAILURE;
		}

int file;
if(filename != NULL){
      file = open(filename,O_WRONLY);
      if(file==-1){
      free(queue);
      printf("on ne sait pas ouvrir fichier %s \n",filename);
      return -1;
    }
    dup2(file,STDOUT_FILENO);
}
else{
     file = STDOUT_FILENO;
}
if(file!=-1){
  while(1){
    FD_ZERO(&readfds);
    FD_SET(sfd,&readfds);
    erreur=select(sfd+1,&readfds,NULL,NULL,&tv);
       if(erreur<0){
         free(queue);
           printf("select a pas fonctionne\n");
           return -1;
       }
       else if(FD_ISSET(sfd, &readfds)){
         char bufdata[528];
         if(read(sfd,bufdata,528)==-1){
           free(queue);
           printf("impossible de lire sur la socket dans le receiver \n");
           return -1;
         }
         size_t length=528;
         pkt_t *receivedpkt = pkt_new();
         status = pkt_decode(bufdata,length,receivedpkt);
         /*
         printf("Type : %d\n",pkt_get_type(receivedpkt));
         printf("Seqnum: %d\n",pkt_get_seqnum(receivedpkt));
         printf("Tr: %d\n",pkt_get_tr(receivedpkt));
         */
         if(status!=PKT_OK)
         {printf("le status est pas bon dans receiver?%d \n",(int)(status));
            return status;
         }
         /* Case ACK */
         if(pkt_get_type(receivedpkt) == PTYPE_DATA){
           uint8_t receivedseqnum = pkt_get_seqnum(receivedpkt); /*seqnum of received packet*/
           if( pkt_get_tr(receivedpkt)!=1){
             if(receivedseqnum==seqnum){
               pkt_t *pktToSend=pkt_new();
               if(!memcpy(pktToSend,receivedpkt,12))
                return E_NOMEM;
                status=pkt_set_type(pktToSend,PTYPE_ACK);
                if(status!=PKT_OK)
                  return status;
                status=pkt_set_length(pktToSend,0);
                  if(status!=PKT_OK)
                  return status;
                size_t len=12;
                status=pkt_encode( pktToSend,bufdata, &len);
                if(status!=PKT_OK) return status;
                erreur=write(sfd,bufdata,len);
                if(erreur==-1) printf("impossible de répondre via la socket(receiver)\n");
                pkt_del(pktToSend);
               erreur=write(file,pkt_get_payload(receivedpkt),pkt_get_length(receivedpkt));
               if(erreur==-1){
                 printf("impossible d'écrire dans le fichier file dans le receiver \n");
               }
               if(seqnum==255){
                 seqnum=0;
             }
             else{
               seqnum++;
             }
             pkt_del(receivedpkt);
             pkt_t * pktrec=queue_get_seq(queue,seqnum);
             while(pktrec!=NULL){
               memset(bufdata,0,sizeof(bufdata));
               pktToSend=pkt_new();
               if(!memcpy(pktToSend,pktrec,12))
                return E_NOMEM;
                status=pkt_set_type(pktToSend,PTYPE_ACK);
                if(status!=PKT_OK)
                  return status;
                  status=pkt_set_length(pktToSend,0);
                  if(status!=PKT_OK)
                    return status;
                 len=12;
                status=pkt_encode( pktToSend,bufdata, &len);
                if(status!=PKT_OK)
                  return status;
                pkt_del(pktToSend);
               erreur=write(file,pkt_get_payload(pktrec),pkt_get_length(pktrec));
               if(erreur==-1){
                    printf("impossible d'écrire des pkt hors séquence dans le fichier file dans le receiver  \n");
                    return -1;
               }
               queue_delete_pkt_timestamp(queue,pkt_get_timestamp(pktrec));
               if(seqnum==255){
                 seqnum=0;
               }
               else{
                 seqnum++;
               }
               pktrec= queue_get_seq(queue,seqnum);
             }
           }
               else{
                 pkt_t *pktToSend=pkt_new();
                 if(!memcpy(pktToSend,receivedpkt,12))
                  return E_NOMEM;
                  status=pkt_set_type(pktToSend,PTYPE_ACK);
                  if(status!=PKT_OK)
                    return status;
                  status=pkt_set_length(pktToSend,0);
                  if(status!=PKT_OK)
                      return status;
                  size_t len=12;
                  status=pkt_encode( pktToSend,bufdata, &len);
                  if(status!=PKT_OK)
                    return status;
                  erreur=write(sfd,bufdata,len);
                  if(erreur==-1)
                    printf("impossible de répondre via la socket(receiver)\n");
                  pkt_del(pktToSend);
                  addTail(queue,receivedpkt);
               }
             }
           else{
             pkt_t *pktToSend=pkt_new();
             if(!memcpy(pktToSend,receivedpkt,12))
              return E_NOMEM;
              status=pkt_set_type(pktToSend,PTYPE_NACK);
              if(status!=PKT_OK)
                return status;
              size_t len=12;
              status=pkt_encode( pktToSend,bufdata, &len);
              if(status!=PKT_OK)
                return status;
              erreur=write(sfd,bufdata,len);
              if(erreur==-1)
              printf("impossible de répondre via la socket(receiver)\n");
              pkt_del(pktToSend);
              addTail(queue,receivedpkt);
}
}
} else{
            printf("Time out\n");
            return 1;
        }
      }
      free(queue);
      printf("on sort sans problème\n");
      return 1;
    }
    free(queue);
printf("on rentre jamais \n");
        return -1;
  }
