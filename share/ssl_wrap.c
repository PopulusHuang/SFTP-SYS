#include "ssl_wrap.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#if 0
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif
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
int ssl_close_pk(SSL_CTX *ctx, SSL *ssl, int sockfd)
{
    if (NULL == ctx || NULL == ssl || sockfd < 0 ) return -1;
    SSL_shutdown(ssl);
    SSL_free(ssl);
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
/* current path */
void Getcwd(char *pwd,int size)
{
	getcwd(pwd,size);
  	if(strlen(pwd)==1)
	{
		pwd[0]='\0';
	}
}
/* Load the user's digital certificate of current 
 * directory that is used to send to the client. 
 * A certificate containing a public key*/
void ssl_load_cert_priv(SSL_CTX *ctx)
{
  char certpwd[100];
  char privpwd[100];
  char *certificate;
  char *privkey;
  /* get the current path */
  Getcwd(certpwd,100);
  certificate=strcat(certpwd,"/cacert.pem");
  Getcwd(privpwd,100);
  privkey=strcat(privpwd,"/privkey.pem");
  ssl_load_pk(ctx,certificate,privkey);
}
