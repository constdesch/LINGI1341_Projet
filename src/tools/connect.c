#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/select.h>


#include "connect.h"

const char * real_address(const char *address, struct sockaddr_in6 *rval){
    struct addrinfo hints,*res;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET6; // use AF_INET6 to force IPv6
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol=IPPROTO_UDP;
    int err=getaddrinfo(address,NULL,&hints,&res);
    if(err!=0){
        char* error="getaddrinfo n'a pas fonctionné";
        return error;
    }
    else
    {
        struct sockaddr_in6* h2=(struct sockaddr_in6 *) (res->ai_addr);
        memcpy(rval,h2,sizeof(struct sockaddr_in6));
        if(rval!=NULL){
            freeaddrinfo(res);
            return NULL ;
        }
        char * error="l'adresse n'est pas bonne .";
        freeaddrinfo(res);
        return error;
    }
}

int create_socket(struct sockaddr_in6 *source_addr,
                  int src_port,
                  struct sockaddr_in6 *dest_addr,
                  int dst_port){
    /*CREATION SOCKET*/
    int fd = socket(AF_INET6, SOCK_DGRAM, 0);
    if(fd==-1) {fprintf(stderr,"bug pour socket \n"); return -1;}

    /*ON CONNECTE LE PORT SOURCE*/
    if(source_addr!=NULL){
        if(src_port>0) source_addr->sin6_port=htons(src_port);
    }

    /*ON CONNECTE LE PORT DEST*/
    if(dest_addr!=NULL){
        if (dst_port>0) dest_addr->sin6_port=htons(dst_port);
    }

    /*BIND*/
    if(source_addr!=NULL){
        source_addr->sin6_family=AF_INET6;
        int errb = bind(fd,(struct sockaddr *) source_addr,
                        sizeof(struct sockaddr_in6));
        if (errb==-1){
            fprintf(stderr,"bug pour bind \n");
            close(fd);
            return -1;
        }
    }
    /*CONNECT*/
    if(dest_addr!=NULL){
        dest_addr->sin6_family=AF_INET6;
        int errc=connect(fd,(struct sockaddr*) dest_addr,
                         sizeof(struct sockaddr_in6));
        if(errc == -1){
            int ernum=errno;
            fprintf(stderr,"l'erreur: %s \n",strerror(ernum));
            close(fd);
            fprintf(stderr,"bug pour connect \n");
            return -1;
        }
    }
    return fd;
}

void read_write_loop(int sfd){
    char bufread[1024]; /* char bufreadfile[512] */
    char bufwrite[1024];/* char bufwritedfs[512]*/
    /* char buffdecode[512] */
    /* char buffencode[512] */

    struct timeval tv;
    fd_set readfds, writefds;
    tv.tv_sec = 5;
    tv.tv_usec = 5;
    /* WHILE !EOF || QUEUE->FULL ==0 */
    while(1){
        FD_ZERO(&writefds);
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(sfd,&writefds);
        FD_SET(sfd,&readfds);
        memset(bufread,0,sizeof(bufread));
        memset(bufwrite,0,sizeof(bufwrite));
        int error = select(sfd+1,&readfds,NULL,NULL,&tv);
        if(error<0){
            fprintf(stderr,"select a pas fonctionne\n");
            return;
        }

        /* Something written on stdin/file */ /*SENDER*/ /*PLUTOT UTILISER READ NON ?*/
        /* if (queue->full < queue->MAXSIZE){
            int err = read(file,bufreadfile,512)
            if (err>0)
                CREATION PAQUET
                ENCODE PAQUET
                ENVOIE PAQUET SUR LA SOCKET
            if (err==-1) PROBLEME

        }*/
        /* CA ON POURRAIT BACKER */
        if(FD_ISSET(STDIN_FILENO,&readfds)){
            int err = read(0,bufread,sizeof(bufread));
            if (err==0) return;
            if(err==-1){ fprintf(stderr,"Erreur read"); }
            err = write(sfd,bufread,err);
            if(err==-1) {fprintf(stderr,"Erreur write");}
        }
        /* Something written on the socket */ /*RECEVEUR*/ /*SENDER AUSSI ICI? */
        else if(FD_ISSET(sfd, &readfds)){

            /* TOUJOURS*/
            /*
            pkt_t *pkt = pkt_new();
            status = pkt_decode(bufdata,len,pkt);
            if(status!=PKT_OK) return status;

            if(pkt_get_type==PTYPE_DATA){
                // Si DATA : DECODE -> STDOUT -> SEND (N)ACK
                int tr = pkt_get_tr(pkt)
                if(tr==1)

            }
            else if(pkt_get_type == PTYPE_ACK){
                uint8_t seqnum = pkt_get_seqnum(pkt);
                err = deletePrevious(queue, seqnum);
                if (err == 0) fprintf(stderr, "Seqnum not found in queue.\n");

            }
            else if(pkt_get_type == PTYPE_NACK){
                // Si NACK-> RENVOIE PKT
                uint8_t seqnum = pkt_get_seqnum(pkt);
                pkt_t *pkt = get_pkt(seqnum);
                int data_length;
                uint16_t length = pkt_get_length(pkt);
                if (length == 0) data_length = 12;
                else data_length = length+12;
                status = pkt_encode(pkt,bufencode,data_length);
                if(status!=PKT_OK) fprintf(stderr,"Encode failed : %d\n",status);
                err = write(sfd,data,data_length);
                if(err==-1) fprintf(stderr,"Could not write on the socket.\n");
            } */

            int err = read(sfd,bufwrite,sizeof(bufwrite));
            if (err==0) return;
            if(err==-1){ fprintf(stderr,"Erreur read");}
            err = write(1,bufwrite,err);
            if(err==-1) {fprintf(stderr,"Erreur write"); }
        }
        else{
            fprintf(stderr,"Time out\n");
            return;
        }
    }
}

int wait_for_client(int sfd){
    char buf[1024];
    struct sockaddr_in6 from;
    from.sin6_family = AF_INET6;
    socklen_t addrlen = sizeof(struct sockaddr_in6);

    int a = recvfrom(sfd,(void *) buf, sizeof(buf), MSG_PEEK, (struct sockaddr*)&from, &addrlen);
    if(a==-1) return -1;
    a = connect(sfd, (struct sockaddr*) &from,addrlen);
    if(a==-1) return -1;

    return 0;
}
