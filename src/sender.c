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





int timeOutRoutine(queue_pkt_t* queue, int sfd){
  printf("hello world\n");
    if(!queue) return 0;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    double tac = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ; /*Convertir en ms */
    if(queue==NULL){
      fprintf(stderr,"queue est vide\n");
    }
    Node *node = queue->head;
    if(node==NULL)
      fprintf(stderr,"queue->head est vide\n");
    pkt_t* pkt= node->data;
    if(pkt==NULL)
    fprintf(stderr,"pkt est null\n");
    double tic = (double)pkt_get_timestamp(pkt); /*Time when the packet was sent*/
    if (tac-tic>4500){ /* Timeout for the first node? Resend it! */
        /* New timestamp */
        gettimeofday(&tv, NULL);
        double timestamp = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ; /*Convertir en ms */
        printf("%f\n",timestamp);
     pkt_status_code   status = pkt_set_timestamp(pkt,uint32_t(imestamp));
        if(status != PKT_OK) printf("Setting timestamp failed.\n");
        /*Check total length of packet*/
        char buf[512];
        uint16_t length = pkt_get_length(pkt);
        size_t tot_length;
        if (length == 0) tot_length =12;
        else if (length>0) tot_length = 16 + (size_t) length;
        else fprintf(stderr,"Length should not be negative.\n");
        /*Encode the packet in a char* and write on sfd*/
        status = pkt_encode(pkt,buf,&tot_length);
        if(status!=PKT_OK) fprintf(stderr,"Encode failed : %d\n",status);
      int  erreur = write(sfd,buf,tot_length);
        if(erreur==-1) fprintf(stderr,"Could not write on the socket.\n");
    }
    return 0;
}
int main(int argc, char* argv[]){
  queue_pkt_t * queue=malloc(sizeof(queue_pkt_t));
  if(!queue){
    fprintf(stderr,"malloc failed for queue\n");
    return -1;
  }
  int windowSize=32;
  init_queue(queue);
  // int client=0;
  int port = 0;
  int opt;
  char *sender = "::1";
  char *receiver = NULL;
  char *filename = NULL;
  const char *err;
  int erreur;
  pkt_status_code status;
  int seqnum = 0;
  struct timeval tv;
  fd_set readfds;
  tv.tv_sec = 5;
  tv.tv_usec = 5;

    while ((opt = getopt(argc, argv, "f:")) != -1) {
        switch (opt) {
            case 'f':
                filename = optarg;
                break;
            case '?':
                if(optopt =='c')
                    fprintf(stderr,"Option -%d requires an argument.\n",optopt);
                else
                    fprintf (stderr,
                             "Unknown option character .\n");
                return 1;
            default:
                abort();
        }
    }
    if(argc - optind <2) fprintf(stderr,"Wrong number of arguments, expected two\n");
    receiver = argv[optind]; /*receiver est le premier argument */
    port = atoi(argv[optind+1]); /*port est le deuxieme argument */

    if(!receiver) fprintf(stderr, "Receiver is NULL\n");
    if(!port) fprintf(stderr, "Port is 0\n");
    printf("Receiver : %s\n",receiver);
    printf("Port : %d\n",port);


    /* Resolve receiver name */
    struct sockaddr_in6 dst_addr;
    err = real_address(receiver,&dst_addr);
    if (err){
        fprintf(stderr, "Could not resolve receiver name %s : %s\n",receiver,err);
    }
    /* Resolve sender name */
    struct sockaddr_in6 src_addr;
    err = real_address(sender,&src_addr);
    if(err){
        fprintf(stderr,"Could not resolve sender name %s : %s\n",sender,err);
    }


    /*Create a socket bound and connected */
    int sfd = create_socket(&src_addr, port, &dst_addr, port); /* Connected */ /* src_port = dst_port ? */
    if(sfd==-1) fprintf(stderr,"Could not create socket.\n");

    /* Option -f mentionned */
      int file;
    if(filename != NULL){
      file = open(filename,O_RDONLY);

    }
    else{ /* Option f not mentionned */
      filename = argv[argc-1]; /*ATTENTION suggere que ce sera toujours le dernier element */
      file = STDIN_FILENO; /* Only for reading */
    }

    if(file!=-1) {
        char bufreadfile[512];
        int byteRead = read(file,bufreadfile,512);
        if (byteRead==-1) fprintf(stderr,"Could not read in file specified\n");
        while(byteRead>0 || queue->full != 0){

            /* Renvoyer les paquets dont le delai a expire */

            /* Lire sur la socket */
            //FD_ZERO(&writefds);
            FD_ZERO(&readfds);
            //FD_SET(STDIN_FILENO, &readfds);
            //FD_SET(sfd,&writefds);
            FD_SET(sfd,&readfds);
            erreur = select(sfd+1,&readfds,NULL,NULL,&tv);

            if (FD_ISSET(sfd, &readfds)){ /* Quelque chose d'ecrit sur la socket */
                /* On decode ce qui est ecrit */
                char bufdata[512];
                if(read(sfd,bufdata,512)==-1){
                  fprintf(stderr,"impossible de lire sur la socket \n");
                  return -1;
                }
                int len=strlen(bufdata);
                pkt_t *receivedpkt = pkt_new();
                status = pkt_decode(bufdata,len,receivedpkt);
                if(status!=PKT_OK) return status;

                /* Case ACK */
                if(pkt_get_type(receivedpkt) == PTYPE_ACK){
                    uint8_t receivedseqnum = pkt_get_seqnum(receivedpkt); /*seqnum of received packet*/
                    pkt_t *testpkt = queue_get_timestamp(queue,pkt_get_timestamp(receivedpkt));/*pkt correponding to timestamp of receiving packet */
                    if (pkt_get_timestamp(receivedpkt) ==pkt_get_timestamp( testpkt)){ /*are the timestamp and seqnum from the same packet?*/
                        erreur = deletePrevious(queue,receivedseqnum+1);
                        if (erreur==0) fprintf(stderr,"Seqnum not found in queue.\n");
                    }
                    else{
                        erreur = deletePrevious(queue, receivedseqnum); /*should also test timestamp*/
                        if (erreur == 0) fprintf(stderr, "Seqnum not found in queue.\n");
                    }
                }

                /* Case NACK */
                if(pkt_get_type(receivedpkt) == PTYPE_NACK){
                    /* On supprime les paquets precedents dans la queue*/
                    uint8_t seqnum1 = pkt_get_seqnum(receivedpkt);
                    erreur = deletePrevious(queue, seqnum1); /*should also test timestamp*/
                    if (erreur == 0) fprintf(stderr, "Seqnum not found in queue.\n");

                    /* On renvoie le paquet nack */
                    size_t data_length;
                    uint16_t length = pkt_get_length(receivedpkt);
                    if (length == 0) data_length = 12;
                    else data_length =(size_t) length + 12;

                    char buf[512];

                    status = pkt_encode(receivedpkt,buf,&data_length);
                    if(status!=PKT_OK) fprintf(stderr,"Encode failed : %d\n",status);
                    erreur = write(sfd,buf,data_length);// data avant
                    if(erreur==-1) fprintf(stderr,"Could not write on the socket.\n");
                }

            /* Ecrire sur la socket */
            if (queue->full != windowSize){ /* Place dans la liste ? */
	      size_t data_length;
                /* Create the header */
                pkt_t* pkt1 = pkt_new();
                pkt_set_type(pkt1,PTYPE_DATA);
                pkt_set_tr(pkt1,0);
                pkt_set_window(pkt1,queue->full);// ou 32
                pkt_set_seqnum(pkt1,seqnum % 32);
                seqnum++;
                pkt_set_length(pkt1,byteRead);
                char buf[512];
		int length1=htons( pkt_get_length(pkt1));
                pkt_set_payload(pkt1,buf,byteRead);
                uLong crc2= crc32(0L, Z_NULL, 0);
                crc2= crc32(crc2, (Bytef *) buf+12,length1);
                uLong crc1 = crc32(0L, Z_NULL, 0);
		crc1=crc32(crc1,(Bytef*) buf,8);
                pkt_set_crc1(pkt1,htonl(crc1));
                pkt_set_crc2(pkt1,htonl(crc2));
                /* Encode the header */
                data_length = byteRead +16;
                char data[data_length];
                status = pkt_encode(pkt1,data,&data_length);
                if(status!=PKT_OK) fprintf(stderr,"Encode failed : %d\n",status);
                if( addTail(queue, pkt1)==NULL){
                  fprintf(stout,"l'ajout à la liste n'a pas réussi");
                  return -1;
                }
                erreur = write(sfd,data,data_length);
                if(erreur==-1) fprintf(stderr,"Could not write on the socket.\n");
                }
            }



            memset(bufreadfile,0,sizeof(bufreadfile));
            byteRead = read(file,bufreadfile,512);
            if(queue==NULL){
            fprintf(stder,"la queue est null\n");
          }
            timeOutRoutine(queue,sfd);
        }
        free(queue);
    }
    fprintf(stderr,"Could not open the file (or stdout in -f not mentionned.\n");
      free(queue);
    return -1;
}
