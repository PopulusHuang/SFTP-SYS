#ifndef _SSL_WRAP_H
#define _SSL_WRAP_H
#include <openssl/ssl.h>
#include <openssl/err.h>
#define INVAILD  1
#define SOCKSSL_SIZE  500
int SSL_read_pk(SSL *ssl, void *buf, int num);
int SSL_write_pk(SSL *ssl, const void *buf, int num);
int ssl_close_pk(SSL_CTX *ctx, SSL *ssl, int sockfd);
int ssl_load_pk(SSL_CTX *ctx, char *certificate, char *privateKey);
void ssl_load_cert_priv(SSL_CTX *ctx,char *path,char *cacert,char *privkey);
#endif
