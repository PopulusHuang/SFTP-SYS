#ifndef _ORDER_H_
#define _ORDER_H_

#define CUT_PATH     20		/* cut path flag */
#define CUT_NAME     21
#define	CUT_PASSWD   22  
#define CUT_ORDER	 23
/* client order */
#define CNULL 	0
#define CIN 	1
#define CREG 	2
#define CSCL 	3
#define CSCS 	4
#define CUP 	5
#define CDOWN 	6
#define COUT 	7
/* server ack code */
#define NOT_FOUND  		404
#define LOGIN_OK   		410
#define LOGIN_FAIL 		411
#define PASSWD_OK 		412
#define PASSWD_ERR		413 
#define FINISH			414 
#define ORDER_SIZE 12 	/* the size of each order */
#define ORDER_NUM  9	/* order number */
#define DATA_SIZE 1024		/* data size of network data */	
#include "ssl_wrap.h"
extern char order_set[ORDER_NUM][ORDER_SIZE];
typedef struct ack_order{
	char order[ORDER_SIZE];		/* ack order */
	char data[DATA_SIZE];		/* ack data */	
}ACK;
int is_order_exist(char *order);
int fetch_order(char *data,char *order);
int send_order(SSL *ssl,char *order);
int recv_order(SSL *ssl,char *order);
#endif
