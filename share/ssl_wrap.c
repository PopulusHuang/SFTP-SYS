#include "ssl_wrap.h"
#include <stdlib.h>
/* load  secret key and  verify */
int ssl_load_pk(SSL_CTX *ctx, char *certificate, char *privateKey)
{
    if (NULL == ctx || NULL == certificate || NULL == privateKey) return -1;
	/* Load the user's digital certificate that is 
	 * used to send to the client. A certificate 
	 * containing a public key*/
    if (SSL_CTX_use_certificate_file(ctx,certificate, SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        return -1;
    }
    /* load user's private Key */
    if (SSL_CTX_use_PrivateKey_file(ctx, privateKey, SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        return -1;
    }
	/* verify the user's private key */
    if (!SSL_CTX_check_private_key(ctx))
    {
        ERR_print_errors_fp(stderr);
        return -1;
    }
}
int ssl_close_pk(SSL_CTX *ctx, SSL *ssl, int sockfd, int new_fd)
{
    if (NULL == ctx || NULL == ssl || sockfd < 0 || new_fd < 0) return -1;
    SSL_shutdown(ssl);
    SSL_free(ssl);
    if (new_fd)
        close(new_fd);
    close(sockfd);
    SSL_CTX_free(ctx);
    return 0;
}
int SSL_write_pk(SSL *ssl, const void *buf, int num)
{
	int n;
	n = SSL_write(ssl,buf,num);
	if (n < 0)
	{
		perror("SSL_write:");
	}
	return n;
}
int SSL_read_pk(SSL *ssl, void *buf, int num)
{
	int n;
	n = SSL_read(ssl,buf,num);
	if (n < 0)
	{
		perror("SSL_read:");
	}
	return n;
}
/* init sockssl by default,the 'size' is the length of sockssl */
int init_sockssl(SOCKSSL *sockssl,int size)
{
	int i;
	for (i = 0; i < size; i++) {
		sockssl[i].sockfd = INVAILD;	
		sockssl[i].ssl = NULL;
	}
}
/* find the index of 'sockfd' in sockssl array */
int search_sockssl(SOCKSSL *sockssl,int size,int sockfd)
{
	int i;
	for (i = 0; i < size ; i++) {
		if(sockssl[i].sockfd == sockfd)	
		{
			return i;
		}
	}
	return -1;
}
/* add socket and ssl */
int add_sockssl(SOCKSSL *sockssl,int size,SSL *ssl,int sockfd)
{
	int i;	
	/* add to invaild item,INVAILD was define at ssl_wrap.h */
	i = search_sockssl(sockssl,size,INVAILD);
	if( i >= 0)
	{
		sockssl[i].sockfd = sockfd;	
		sockssl[i].ssl = ssl;
		return 0;
	}
	return -1;
}
int remove_sockssl(SOCKSSL *sockssl,int size,int sockfd)
{
	int i;
	i = search_sockssl(sockssl,size,sockfd);
	if(i >= 0)
	{
		sockssl[i].sockfd = INVAILD;	
		sockssl[i].ssl = NULL;
		return 0;
	}
	return -1;
}
SSL *get_sockssl(SOCKSSL *sockssl,int size,int sockfd)
{
	SSL *ssl = NULL;
	int i;
	i = search_sockssl(sockssl,size,sockfd);
	if(i >= 0)
	{
		ssl = sockssl[i].ssl;	
	}
	return ssl;
}
