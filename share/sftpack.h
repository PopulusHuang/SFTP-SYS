#ifndef _SFTPACK_H_
#define _SFTPACK_H_

#define CUT_PATH     20		/* cut path flag */
#define CUT_NAME     21
#define	CUT_PASSWD   22  
#define CUT_ORDER	 23
/* client order */
#define CNULL 	40
#define CIN 	41
#define CREG 	42
#define CSCL 	43
#define CSCS 	44
#define CUP 	45
#define CDOWN 	46
#define COUT 	47
/* ack code */
#define NOT_FOUND  		404
#define LOGIN_OK   		410
#define LOGIN_FAIL 		411
#define PASSWD_OK 		420
#define PASSWD_ERR		421 
#define REGISTER_OK 	422
#define USER_EXIST		423
#define UP_OK			454
#define DOWN_OK			466		

#define ACCEPT			412 
#define FINISH			414 
#define FAIL			415
#define ASK				520

#define ORDER_NUM  8	/* order number */
#define DATA_SIZE 1024		/* data size of network data */	
#include "ssl_wrap.h"
#include "account.h"
#include "sftfile.h"
extern int order_set[ORDER_NUM];

/* secure file transfer data */
typedef union sftdata{
	ACCOUNT user;	
	FILE_ATTR file_attr;
}SFT_DATA;

/* secure file transfer package */
typedef struct sft_pack{
		unsigned int order;					/* client order */	
		unsigned int ack;					/* server respond */
		char buf[DATA_SIZE];
		SFT_DATA data;
}SFT_PACK;

int order_isexist(int order);
int serv_ack_code(SSL *ssl,int order);
void sftpack_init(SFT_PACK *sftpack);
int sftpack_recv(SSL *ssl,SFT_PACK *sftpack);
int sftpack_send(SSL *ssl,SFT_PACK *sftpack);
int sftpack_wrap(SFT_PACK *sftpack,
				int order,int ack,
				char *buf);
#endif
