// avec l'aide de Const de Schaetzen + Lucille Dierkxc + Konstaninos
#include "packet_interface.h"
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include <arpa/inet.h>
#include <zlib.h>
#include <endian.h>
/* Extra #includes */
/* Your code will be inserted here */

typedef struct __attribute__ (( __packed__ )) pkt {


    uint8_t window:5;
    uint8_t tr:1;
    uint8_t type:2;
    uint8_t seqNum;
    uint16_t length;
    uint32_t timestamp;
    uint32_t crc1;
    char * payload;
    uint32_t crc2;
} pkt_t ;

/* Extra code */
/* Your code will be inserted here */

pkt_t* pkt_new()
{
    pkt_t * pkt=calloc(1,sizeof(pkt_t));
    return pkt;

}

void pkt_del(pkt_t *pkt)
{
    if(pkt!=NULL){
        if(pkt->payload!=NULL){
            free(pkt->payload);
        }
        free(pkt);
    }
}

pkt_status_code pkt_decode(const char *data, const size_t len, pkt_t *pkt)
{
    if(len<12){
        return E_UNCONSISTENT;
    }

    if( memcpy(pkt,data,12)==NULL)
        return E_NOMEM;

    pkt->length=ntohs(pkt->length);

    if(memcpy(&pkt->crc2,data+12+pkt_get_length(pkt),4)==NULL)
        return E_NOMEM;


   if( memcpy(pkt,data,12)==NULL)
       return E_NOMEM;
    pkt->length=ntohs(pkt->length);
    if(memcpy(&pkt->crc2,data+12+pkt_get_length(pkt),4)==NULL)
        return E_NOMEM;

    if (pkt_get_type(pkt)!=PTYPE_DATA && pkt_get_tr(pkt)==1){
        return E_UNCONSISTENT;
    }

    if(pkt_set_tr(pkt,pkt->tr)!=PKT_OK){
        return E_TR;
    }
    if(pkt_set_window(pkt,pkt->window)!=PKT_OK){
        return E_WINDOW;
    }
    if(pkt_set_seqnum(pkt,pkt->seqNum)!=PKT_OK){
        return E_SEQNUM;
    }
    if(pkt_set_length(pkt,pkt->length)!=PKT_OK){
        return E_LENGTH;
    }
    if(pkt_set_timestamp(pkt,pkt->timestamp)!=PKT_OK){
        return PKT_OK;
    }
    if(len>12){
        if(pkt_set_payload(pkt,data+12,pkt_get_length(pkt))!=PKT_OK){
            return E_NOMEM;
    }}
    if(pkt_set_payload(pkt,data+12,pkt_get_length(pkt))!=PKT_OK){
        return E_NOMEM;
    }
    // calcul de crc1
    pkt->tr=0;
    uLong crc1 = crc32(0L, Z_NULL, 0);
    crc1 = crc32(crc1, ( const Bytef *) data, 8*sizeof(char));
    if(htonl(crc1)!=(pkt->crc1)){
        return E_CRC;
    }
    if(pkt->length!=0){
        uLong crc2 = crc32(0L, Z_NULL, 0);
        crc2 = crc32(crc2, (Bytef *)data+12, pkt->length);
        if (htonl(crc2) != (pkt->crc2)){
            return E_CRC;
        }
    }
    return PKT_OK;
}



pkt_status_code pkt_encode(const pkt_t* pkt, char *buf, size_t *len)

{
    {
        int ll1=12;
        if(pkt->length>0){
            ll1=ll1+4+pkt->length;
        }
        if((int)* len<ll1){
            return E_NOMEM;
        }
        uint16_t length1=htons(pkt->length);
        if(memcpy(buf,pkt,2)==NULL)
            return E_NOMEM;
        if(memcpy(buf+2,&length1,2)==NULL)
            return E_NOMEM;
        if( memcpy(buf+4,&(pkt->timestamp),4)==NULL)
            return E_NOMEM;
        uLong crc1 = crc32(0L, Z_NULL, 0);
        crc1 = htobe32(crc32(crc1, ( const Bytef *) buf, 8*sizeof(char)));
        if( memcpy(buf+8,&crc1,4)==NULL)
            return E_NOMEM;
        if(pkt->length!=0){
            if(memcpy(buf+12,pkt->payload,pkt->length)==NULL)
                return E_NOMEM;
            uLong crc2= crc32(0L, Z_NULL, 0);
            crc2= crc32(crc2, (Bytef *) buf+12,pkt->length);
            uint32_t crc22=htonl(crc2);
            if  (memcpy(buf+12+pkt->length,&crc22,4)==NULL)
                return E_NOMEM;
        }
        *len= ll1 ;
        return PKT_OK;
    }

    uint16_t length1=htons(pkt->length);
   	if(memcpy(buf,pkt,2)==NULL)
        return E_NOMEM;
    if(memcpy(buf+2,&length1,2)==NULL)
        return E_NOMEM;
   if( memcpy(buf+4,&(pkt->timestamp),4)==NULL)
       return E_NOMEM;
uLong crc1 = crc32(0L, Z_NULL, 0);
    crc1 = htobe32(crc32(crc1, ( const Bytef *) buf, 8*sizeof(char)));
if( memcpy(buf+8,&crc1,4)==NULL)
    return E_NOMEM;
    if(pkt->length!=0){
    if(memcpy(buf+12,pkt->payload,pkt->length)==NULL)
        return E_NOMEM;
        uLong crc2= crc32(0L, Z_NULL, 0);
    crc2= crc32(crc2, (Bytef *) buf+12,pkt->length);
    uint32_t crc22=htonl(crc2);
  if  (memcpy(buf+12+pkt->length,&crc22,4)==NULL)
      return E_NOMEM;
       }
    *len= ll1 ;
    return PKT_OK;
}
}

ptypes_t pkt_get_type  (const pkt_t* pkt)
{
    return pkt->type;}

uint8_t  pkt_get_tr(const pkt_t* pkt)
{
    return pkt->tr;
}


uint8_t  pkt_get_window(const pkt_t* pkt)
{
    return pkt->window;
    /* Your code will be inserted here */
}

uint8_t  pkt_get_seqnum(const pkt_t* pkt)
{
    return pkt->seqNum;
    /* Your code will be inserted here */
}

uint16_t pkt_get_length(const pkt_t* pkt )
{
    return pkt->length;
    /* Your code will be inserted here */
}

uint32_t pkt_get_timestamp   (const pkt_t* pkt )
{
    return pkt->timestamp;
    /* Your code will be inserted here */
}

uint32_t pkt_get_crc1   (const pkt_t* pkt )
{
    return pkt->crc1;
    /* Your code will be inserted here */
}

uint32_t pkt_get_crc2   (const pkt_t* pkt)
{
    return pkt->crc2;
    /* Your code will be inserted here */
}

const char* pkt_get_payload(const pkt_t* pkt)
{
    return pkt->payload;
    /* Your code will be inserted here */

}


pkt_status_code pkt_set_type(pkt_t *pkt, const ptypes_t type)
{
    if (type!= PTYPE_DATA && type!=PTYPE_ACK  && type!=PTYPE_NACK){
        return E_TYPE;
    }
    pkt->type=type;
    return PKT_OK;
}

pkt_status_code pkt_set_tr(pkt_t *pkt, const uint8_t tr)
{
    if((tr!=0) && (tr!=1)) return E_TR;
    pkt->tr = tr;
    return PKT_OK;
}

pkt_status_code pkt_set_window(pkt_t *pkt, const uint8_t window)
{
    if(window < MAX_WINDOW_SIZE){
        pkt->window = window;
        return PKT_OK;}
    return E_WINDOW;
    pkt->window = window;
    return PKT_OK;
}



pkt_status_code pkt_set_seqnum(pkt_t *pkt, const uint8_t seqnum)
{
    pkt->seqNum=seqnum;
    return PKT_OK;
    /* Your code will be inserted here */
}

pkt_status_code pkt_set_length(pkt_t *pkt, const uint16_t length)
{
    pkt->length =length;
    return PKT_OK;
    //sur que la taille est bonne
    if(length < MAX_PAYLOAD_SIZE && length>0 )
    {
        pkt->length =length;
        return PKT_OK;
    }
    return E_LENGTH;
}

pkt_status_code pkt_set_timestamp(pkt_t *pkt, const uint32_t timestamp)
{
    pkt->timestamp=timestamp;
    return PKT_OK;
}

pkt_status_code pkt_set_crc1(pkt_t *pkt, const uint32_t crc)
{
    pkt->crc1=crc;
    return PKT_OK;
}

pkt_status_code pkt_set_crc2(pkt_t *pkt, const uint32_t crc)
{
    pkt->crc2=crc;
    return PKT_OK;
    /* Your code will be inserted here */
}

pkt_status_code pkt_set_payload(pkt_t *pkt,
                                const char *data,
                                const uint16_t length)
{
    if(pkt_set_length(pkt,length)!=PKT_OK){
        return E_LENGTH;
    }
    if(pkt->payload!=NULL){
        free(pkt->payload);
    }
    pkt->payload=malloc(length);
    if(!pkt->payload){
        return E_NOMEM;
    }
    if(! memcpy(pkt->payload,data,length))
        return E_NOMEM;
=======
   if(! memcpy(pkt->payload,data,length))
       return E_NOMEM;
    return PKT_OK;
}
