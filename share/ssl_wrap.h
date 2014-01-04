#ifndef _SSL_WRAP_H
#define _SSL_WRAP_H
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#define INVAILD  -1
#define SOCKSSL_SIZE  500
/* handle socket's ssl */
typedef struct sockssl
{
	int sockfd;	/* connected socket's file describe */
	SSL *ssl;
}SOCKSSL;
int SSL_read_pk(SSL *ssl, void *buf, int num);
int SSL_write_pk(SSL *ssl, const void *buf, int num);
int add_sockssl(SOCKSSL *sockssl,int size,SSL *ssl,int sockfd);
SSL *get_sockssl(SOCKSSL *sockssl,int size,int sockfd);
int init_sockssl(SOCKSSL *sockssl,int size);
int remove_sockssl(SOCKSSL *sockssl,int size,int sockfd);
int search_sockssl(SOCKSSL *sockssl,int size,int sockfd);
int ssl_close_pk(SSL_CTX *ctx, SSL *ssl, int sockfd, int new_fd);
int ssl_load_pk(SSL_CTX *ctx, char *certificate, char *privateKey);
#endif
