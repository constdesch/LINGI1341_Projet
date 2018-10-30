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
#include <time.h>

#define true 1
#define false 0





int main(int argc, char* argv[]){
  queue_pkt_t * queue=malloc(sizeof(queue_pkt_t));
  //uint32_t lasttimewereceive=0;
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
    uint8_t seqnum = 0;
    char * sender= NULL;
    struct timeval tv;
    fd_set readfds;
    tv.tv_sec = 0;
    tv.tv_usec = 100;
    uint32_t lasttimewereceive=0;
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
    //
    port = atoi(argv[optind+1]); /*port est le deuxieme argument */
    if(!sender) fprintf(stderr, "Sender is NULL\n");
    if(!port) fprintf(stderr, "Port is 0\n");
    /* Option -f mentionned */
    struct sockaddr_in6 dst_addr;
    err = real_address(sender,&dst_addr);
    if (err){
      fprintf(stderr, "Could not resolve sender name %s : %s\n",sender,err);
    }
    /* Resolve sender name */
    int sfd = create_socket(&dst_addr, port,NULL, -1); /* Connected */ /* src_port = dst_port ? */
    if (sfd > 0 && wait_for_client(sfd) < 0) { /* Connected */
			fprintf(stderr,
					"Could not connect the socket after the first message.\n");
			close(sfd);
      free(queue);
			return EXIT_FAILURE;
		}

int file;
if(filename != NULL){
      file = open(filename,O_WRONLY|O_CREAT,S_IRWXO|S_IRWXU );
    }
else{
     file = STDOUT_FILENO;
}
if(file!=-1){
  int fin=0;
  while(fin!=1){
    char envoi[12];
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
         erreur=read(sfd,bufdata,528);
         if(erreur==-1){
           free(queue);
           printf("impossible de lire sur la socket dans le receiver \n");
           return -1;
         }
         pkt_t *receivedpkt = pkt_new();
         status = pkt_decode(bufdata,erreur,receivedpkt);
         if(status!=PKT_OK)
         {printf("le status est pas bon dans receiver?%d \n",(int)(status));
         pkt_del(receivedpkt);
            continue;
         }
         /* Case ACK */
         if(pkt_get_type(receivedpkt) == PTYPE_DATA){
           memset(envoi,0,sizeof(envoi));
           uint8_t receivedseqnum = pkt_get_seqnum(receivedpkt); /*seqnum of received packet*/
           fprintf(stderr,"receivedseqnum:%d\n",(int) receivedseqnum);
           if( pkt_get_tr(receivedpkt)!=1){
             fprintf(stderr,"seqnum:%d\n",seqnum);
             if(receivedseqnum==seqnum){
               pkt_t *pktToSend=pkt_new();
               pkt_set_type(pktToSend, PTYPE_ACK);
               pkt_set_tr(pktToSend,0);
               if(pkt_set_window(pktToSend, pkt_get_window(receivedpkt))!=PKT_OK)
               fprintf(stderr,"la window a pas la bonne taille");
               pkt_set_seqnum(pktToSend, seqnum+1);
               pkt_set_timestamp(pktToSend, pkt_get_timestamp(receivedpkt));
               uLong crc1 = crc32(0L, Z_NULL, 0);
               crc1=crc32(crc1,(Bytef*) pktToSend,8);
               pkt_set_crc1(pktToSend,htonl(crc1));//recalculer
              lasttimewereceive=clock();//pkt_get_timestamp(receivedpkt);
                size_t len=12;
                status=pkt_encode(pktToSend,envoi, &len);
                if(status!=PKT_OK)
                { printf("%d,status",status);
                  return status;
                }
                if(pkt_get_length(receivedpkt)==0){
                  fin=1;
                  fprintf(stderr,"on sort pour la bonne raison cette fois\n");
                  pkt_del(receivedpkt);
                  pkt_del(pktToSend);
                  erreur=write(sfd,envoi,12);
                  continue;
                }
                erreur=write(file,pkt_get_payload(receivedpkt),pkt_get_length(receivedpkt));
                if(erreur==-1){
                  pkt_del(pktToSend);
                  printf("impossible d'écrire dans le fichier file dans le receiver \n");
                }
                 if(erreur!=0){
                   pkt_del(pktToSend);
                erreur=write(sfd,envoi,12);
                fprintf(stderr,"on envoie bien ");
                if(erreur==-1) printf("impossible de répondre via la socket(receiver)\n");
              }
//                pkt_del(pktToSend); le free puis le réalouer ça me fait pas kiffer
              if(seqnum==255)
                seqnum=0;
              else
                seqnum++;
             //si c'est celui qu'on attend pour débloquer la liste, il faut débloquer les autres.
             pkt_del(receivedpkt);
             pkt_t * pktrec=queue_get_seq(queue,seqnum);
             while(pktrec!=NULL){
               memset(envoi,0,sizeof(envoi));
               pktToSend=pkt_new();
               if(!memcpy(pktToSend,pktrec,12))
                return E_NOMEM;
                status=pkt_set_type(pktToSend,PTYPE_ACK);
                if(status!=PKT_OK)
                  return status;
                  status=pkt_set_length(pktToSend,0);
                  if(status!=PKT_OK)
                    return status;
                status=pkt_encode( pktToSend,envoi, &len);
                if(status!=PKT_OK)
                  return status;
                pkt_del(pktToSend);
                if(pkt_get_length(pktrec)==0){
                  fin=1;
                  fprintf(stderr,"on sort pour la bonne raison cette fois\n");
                  if(write(sfd,envoi,len)==-1){
                    return -1;
                  }
                //  queue_delete_pkt_timestamp(queue,pkt_get_timestamp(pktrec));
                  queue_delete_pkt_seqnum(queue,pkt_get_seqnum(pktrec));
                  continue;
                }
               erreur=write(file,pkt_get_payload(pktrec),pkt_get_length(pktrec));
               if(erreur==-1){
                    printf("impossible d'écrire des pkt hors séquence dans le fichier file dans le receiver  \n");
                    return -1;
               }
               //le supprime de la liste
              // queue_delete_pkt_timestamp(queue,pkt_get_timestamp(pktrec));
               queue_delete_pkt_seqnum(queue,pkt_get_seqnum(pktrec));
               if(seqnum==255)
                 seqnum=0;
               else
                 seqnum++;
               pktrec= queue_get_seq(queue,seqnum);
             }
           }
	     else {
                 pkt_t *pktToSend=pkt_new();
                pkt_set_type(pktToSend,PTYPE_ACK);
                pkt_set_tr(pktToSend,0);
                pkt_set_length(pktToSend,0);
                pkt_set_window(pktToSend,31);
                fprintf(stderr,"pourquoi le seqnum changerait ici, %d\n",seqnum);
                pkt_set_seqnum(pktToSend,seqnum);
                pkt_set_timestamp(pktToSend,pkt_get_timestamp(receivedpkt));
                uLong crc1 = crc32(0L, Z_NULL, 0);
                crc1=crc32(crc1,(Bytef*) pktToSend,8);
                pkt_set_crc1(pktToSend,htonl(crc1));
                lasttimewereceive=clock();//pkt_get_timestamp(receivedpkt);//clock()
                  size_t len=12;
                  status=pkt_encode( pktToSend,bufdata, &len);
                  if(status!=PKT_OK)
                    return status;
                  erreur=write(sfd,bufdata,len);
                  if(erreur==-1)
                    printf("impossible de répondre via la socket(receiver)\n");
                  pkt_del(pktToSend);
                  //on ajoute seulement si il est pas dedans
                  if(queue_get_seq(queue,pkt_get_seqnum(receivedpkt))==NULL)
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
              status=pkt_set_length(pktToSend,0);
              if(status!=PKT_OK)
                return status;
                status=pkt_set_seqnum(pktToSend,seqnum);
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
}
}
}
else{
  uint32_t actualtime=clock();
if(((-lasttimewereceive+actualtime)*1000/CLOCKS_PER_SEC>4500) && (lasttimewereceive!=0)){
  fprintf(stderr,"on break jamais mon cul?\n");
break;
}
}
      }
      fprintf(stderr,"on sort quand même à un moment jésus christ \n");
      free(queue);
      if(close(sfd)==-1)
      return -1;
      printf("on sort sans problème\n");
      return 0;
    }
    free(queue);
printf("on rentre jamais \n");
        return -1;
  }
