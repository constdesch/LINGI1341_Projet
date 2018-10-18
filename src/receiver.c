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

#define true 1
#define false 0





void receiver(int argc, char* argv[]){
    int client = 0;
    int port = 0;
    int opt;
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
    if(argc - optind <2) fprintf(stderr,"Wrong number of arguments, expected at least two, got %d\n",arc-optind);
    receiver = argv[optind]; /*receiver est le premier argument */
    port = atoi(argv[optind+1]); /*port est le deuxieme argument */

    if(!receiver) fprintf(stderr, "Receiver is NULL\n");
    if(!port) fprintf(stderr, "Port is 0\n");
    printf("Receiver : %s\n",receiver);
    printf("Port : %d\n",port);
    
    /* Option -f mentionned */
    if(filename != NULL){
        int file = fopen(filename,"r");
    }
    else{
        int file = fopen(stdout, "r");
    }
    
    if(file!=NULL){
        /* DO SOMETHING */
    }
    return -1;
}
