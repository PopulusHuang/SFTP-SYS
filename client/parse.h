#ifndef _PARSE_H_
#define _PARSE_H_
#define LOGIN_OK 1
#define REGISTER_OK 2
#include "../share/account.h"
#include "../share/order.h"
int clnt_login(SSL *ssl,char *order);
int clnt_register(SSL *ssl,char *order);
int create_emptyfile(char *filename);
void create_recvdir(char *fdir);
int download_files(SSL *ssl,char *order);
int get_path(char *scan_path,int flag);
int open_file(char *fileName);
void package_order(char *order,char *data,char *order_pack);
int parse_clnt_order(SSL *ssl,char *order);
int parse_serv_order(char *ack,char *order);
int recv_file_data(SSL *ssl,int fd);
int recv_file_list(SSL *ssl);
void remove_space(char *src,char *dest);
void scan_local_files(void);
int scan_serv_files(SSL *ssl,char *order);
int send_account(SSL *ssl,ACK *ack,ACCOUNT *account,char *order);
void send_data(SSL *ssl,int fd);
int send_file(SSL *ssl);
int send_filename(SSL *ssl,char *fileName);
int upload_files(SSL *ssl,char *order);
int logout(SSL *ssl,char *order);
#endif
