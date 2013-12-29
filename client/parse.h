#ifndef _PARSE_H_
#define _PARSE_H_

#define ORDER_SIZE 8 	/* the size of each order */
#define ORDER_NUM  8	/* order number */

#define CNULL 	0
#define CIN 	1
#define CREG 	2
#define CSCL 	3
#define CSCS 	4
#define CUP 	5
#define CDOWN 	6
#define COUT 	7
extern order_set[ORDER_NUM][ORDER_SIZE];
int is_order_exist(char *order);
int send_order(SSL *ssl,char *order);
int recv_order(SSL *ssl,char *order);
void clnt_entry(SSL *ssl,char *order);
void clnt_register(SSL*ssl,char *order);
void scan_local_files(void);
void scan_serv_files(SSL *ssl,char *order);
void upload_files(SSL *ssl,char *order);
void download_files(SSL *ssl,char *order);
void package_order(char *order,char *data,char *order_pack);
int parse_clnt_order(SSL *ssl,char *order);
int parse_serv_order(char *order);
void remove_space(char *src,char *dest);
void logout(void);
#endif
