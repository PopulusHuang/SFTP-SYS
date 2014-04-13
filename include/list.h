#ifndef _LIST_H_
#define _LIST_H_
#include "ssl_wrap.h"
int list_server(SSL *ssl,int order,char *path,char *flag);
int list_client(char *path,char *flag);
int list_recv(SSL *ssl,int order);
#endif
