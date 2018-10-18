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
#include <time.h>
#include <sys/select.h>
#include "tools/connect.h"
#include "tools/pkt.h"
#include "tools/queue_pkt.h"
#define true 1
#define false 0





int timeOutRoutine(queue_pkt_t* queue, int sfd){
    if(!queue) return 0;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    double tac = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ; /*Convertir en ms */
    Node *node = queue->head;
    double tic = node->timestamp; /*Time when the packet was sent*/
    if (tac-tic>4500){ /* Timeout for the first node? Resend it! */
        pkt_t *pkt = node->data;

        /* New timestamp */
        gettimeofday(&tv, NULL);
        double timestamp = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ; /*Convertir en ms */
        status = pkt_set_timestamp(pkt,timestamp);
        if(status != PKT_OK) fprintf(stderr,"Setting timestamp failed.\n");
        /*Check total length of packet*/
        char buf[512];
        int length = pkt->length;
        int tot_length;
        if (length == 0) tot_length =12;
        else if (length>0) tot_length = 16 + length;
        else fprintf(stderr,"Length should not be negative.\n");
        /*Encode the packet in a char* and write on sfd*/
        status = pkt_encode(pkt,buf,tot_length);
        if(status!=PKT_OK) fprintf(stderr,"Encode failed : %d\n",status);
        err = write(sfd,data,tot_length);
        if(err==-1) fprintf(stderr,"Could not write on the socket.\n");
    }
}
int main(int argc, char* argv[]){
  queue_pkt_t * queue=malloc(sizeof(queue_pkt_t));
  if(!queue){
    fprintf(stderr,"malloc failed for queue\n");
    return -1;
  }
  init_queue(queue);
  int client = 0;
  int port = 0;
  int opt;
  char *sender = "::1";
  char *receiver = NULL;
  char *filename = NULL;
  const char *err;
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
       file = fopen(filename, "r");/* Only for reading */

    }
    else{ /* Option f not mentionned */
        filename = argv[argc-1] /*ATTENTION suggere que ce sera toujours le dernier element */
       file = fopen(STDIN_FILENO, "r"); /* Only for reading */
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
            err = select(sfd+1,&readfds,NULL,NULL,&tv);

            if (FD_ISSET(sfd, &readfds)){ /* Quelque chose d'ecrit sur la socket */
                /* On decode ce qui est ecrit */
                char bufdata[512];
                if(read(sfd,bufdata,512)==-1){
                  fprintf("impossible de lire sur la socket \n");
                  return -1;
                }
                int len=strlen(bufdata);
                pkt_t *receivedpkt = pkt_new();
                status = pkt_decode(bufdata,len,receivedpkt);
                if(status!=PKT_OK) return status;

                /* Case ACK */
                if(pkt_get_type(receivedpkt) == PTYPE_ACK){
                    uint8_t receivedseqnum = pkt_get_seqnum(receivedpkt); /*seqnum of received packet*/
                    pkt_t *testpkt = queue_get_timestamp(receivedpkt->timestamp);/*pkt correponding to timestamp of receiving packet */
                    if (receivedseqnum == testpkt->seqnum){ /*are the timestamp and seqnum from the same packet?*/
                        err = deletePrevious(queue,receivedseqnum+1);
                        if (err==0) fprintf(stderr,"Seqnum not found in queue.\n");
                    }
                    else{
                        err = deletePrevious(queue, receivedseqnum); /*should also test timestamp*/
                        if (err == 0) fprintf(stderr, "Seqnum not found in queue.\n");
                    }
                }

                /* Case NACK */
                if(pkt_get_type(receivedpkt) == PTYPE_NACK){
                    /* On supprime les paquets precedents dans la queue*/
                    uint8_t seqnum = pkt_get_seqnum(receivedpkt);
                    err = deletePrevious(queue, seqnum); /*should also test timestamp*/
                    if (err == 0) fprintf(stderr, "Seqnum not found in queue.\n");

                    /* On renvoie le paquet nack */
                    int data_length;
                    uint16_t length = pkt_get_length(receivedpkt);
                    if (length == 0) data_length = 12;
                    else data_length = length+12;

                    char buf[512];

                    status = pkt_encode(pkt,buf,&data_length);
                    if(status!=PKT_OK) fprintf(stderr,"Encode failed : %d\n",status);
                    err = write(sfd,data,data_length);
                    if(err==-1) fprintf(stderr,"Could not write on the socket.\n");
                }

            /* Ecrire sur la socket */
            if (queue->full != windowSize){ /* Place dans la liste ? */

                /* Create the header */
                pkt_t* pkt = pkt_new();
                pkt_set_type(pkt,PTYPE_DATA);
                pkt_set_tr(pkt,0);
                pkt_set_window();
                pkt_set_seqnum(pkt,seqnum % 32);
                seqnum++;
                pkt_set_length(pkt,byteRead);

                char buf[512];

                pkt_set_payload(pkt,buf,byteRead);
                uLong crc2= crc32(0L, Z_NULL, 0);
                crc2= crc32(crc2, (Bytef *) buf+12,pkt->length);
                uLong crc1 = crc32(0L, Z_NULL, 0);
                pkt_set_crc1(pkt,htonl(crc1));
                pkt_set_crc2(pkt,htonl(crc2));

                /* Encode the header */
                data_length = byteRead +16;
                char *data = malloc(data_length);
                if(!data) fprintf(stderr,"Data malloc failed\n");
                status = pkt_encode(pkt,data,data_length);
                if(status!=PKT_OK) fprintf(stderr,"Encode failed : %d\n",status);

                err = write(sfd,data,data_length);
                if(err==-1) fprintf(stderr,"Could not write on the socket.\n");
                }
            }



            bufreadfile = memset(bufreadfile,0,sizeof(bufreadfile));
            byteRead = read(file,bufreadfile,512);
            timeOutRoutine(queue,sfd);
        }
    }
    fprintf(stderr,"Could not open the file (or stdout in -f not mentionned.\n");
    return -1;
}
