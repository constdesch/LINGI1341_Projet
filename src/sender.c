//
//  sender.c
//  
//
//  Created by Constantin de Schaetzen on 16/10/18.
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

#define true 1
#define false 0
void sender(int argc, char* argv[]){
    int client = 0;
    int port = 0;
    int opt;
    char *sender = "::1";
    char *receiver = NULL;
    char *filename = NULL;
    const char *err;
    pkt_status_code status;
    int seqnum = 0;
    
    while ((opt = getopt(argc, argv, "f:")) != -1) {
        switch (opt) {
            case 'f':
                filename = optarg;
                fflag = true;
                break;
            case '?':
                if(optopt =='c')
                    fprintf(stderr,"Option -%f requires an argument.\n",optopt);
                else
                    fprintf (stderr,
                             "Unknown option character `\\x%x'.\n",
                             optopt);
                return 1;
            default:
                abort();
        }
    }
    if(argc - optind !=2) fprintf(stderr,"Wrong number of arguments, expected two\n");
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
    if(filename != NULL){
        file = fopen(filename, "r"); /* Only for reading */
        if(file!=NULL) {
            char buf[512];
            int byteRead = read(file,buf,512);
            if (byteRead==-1) fprintf(stderr,"Could not read in file specified\n");
            while(byteRead>0){
                pkt_t* pkt = pkt_new();
                pkt_set_type(pkt,PTYPE_DATA);
                pkt_set_tr(pkt,0);
                pkt_set_window();
                pkt_set_seqnum(pkt,seqnum);
                seqnum++;
                pkt_set_length(pkt,byteRead);
                pkt_set_payload(pkt,buf,byteRead);
                uLong crc2= crc32(0L, Z_NULL, 0);
                crc2= crc32(crc2, (Bytef *) buf+12,pkt->length);
                uLong crc1 = crc32(0L, Z_NULL, 0);
                pkt_set_crc1(pkt,htonl(crc1));
                pkt_set_crc2(pkt,htonl(crc2));
                data_length = byteRead +16;
                char *data = malloc(data_length);
                if(!data) fprintf(stderr,"Data malloc failed\n");
                status = pkt_encode(pkt,data,data_length);
                if(status!=PKT_OK) fprintf(stderr,"Encode failed : %d\n",status);
                err = write(sfd,data,data_length);
                if(err==-1) fprintf(stderr,"Could not write on the socket.\n");
                buf = memset(buf,0,sizeof(buf));
                byteRead = read(file,buf,512)
            }
        
            
    }
    
    

        
    }*/
    
}

