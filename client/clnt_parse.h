#ifndef _CLNT_PARSE_H_
#define _CLNT_PARSE_H_
#include "menu.h"
#include "../share/ssl_wrap.h"
int clnt_login(SSL *ssl,int order);
int clnt_register(SSL *ssl,int order);
int cut_path(char *filename);
int download_files(SSL *ssl,int order);
int get_path(char *scan_path,int flag);
int logout(SSL *ssl,int order);
int parse_clnt_order(SSL *ssl,int order);
int recv_file_list(SSL *ssl,int order);
void remove_space(char *src,char *dest);
void scan_local_files(void);
int scan_serv_files(SSL *ssl,int order);
int upload_files(SSL *ssl,int order);
#endif
