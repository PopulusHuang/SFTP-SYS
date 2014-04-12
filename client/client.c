/*ssl client.c*/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <resolv.h>
#include <stdlib.h>
#include <fcntl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "ssl_wrap.h"
#include "sock_wrap.h"
#include "sftpack.h"
#include "ui.h"
#include "clnt_parse.h"
#define MAXBUF 1024
#define SERV_PORT 7838
void show_certs(SSL * ssl)
{
  X509 *cert;
  char *line;

  cert = SSL_get_peer_certificate(ssl);
  if (cert != NULL) {
    printf("Digital certificate information:\n");
    line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
    printf("Certificate: %s\n", line);
    free(line);
    line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
    printf("Issuer: %s\n", line);
    free(line);
    X509_free(cert);
  }
  else{
    printf("No certificate information！\n");
  }
}
/* print error message */
void print_error()
{
    printf("message Send failure ！Error code is %d, \
			Error messages are '%s'\n", 
			errno, strerror(errno));
}
/* initial server's socket */
int sock_init(char *server_ip,int server_port)
{
  int sockfd;
  struct sockaddr_in servaddr;
  int len;
  //char *ip = "127.0.0.1";
  sockfd = Socket(AF_INET, SOCK_STREAM, 0);

  /* initial server's address and port */
  bzero(&servaddr,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(server_port);//htons(atoi(argv[2]));
  if(inet_pton(AF_INET,server_ip,&servaddr.sin_addr) < 0)
  {
 	perror("server ip error"); 
	exit(errno);
  }

  /* Connect to server */
  len = sizeof(servaddr);
  Connect(sockfd,(struct sockaddr *)&servaddr,len);
  printf("Server connected successfully !\n\n");
  return sockfd;
}
int show_Mlogin(SSL *ssl)
{
  int order;
  while(1)
  {
		 logo_ui();
  	   	 order = Mlogin();  
		 order = parse_clnt_order(ssl,order);	 
	     if(order == LOGIN_OK)
		 {
			return 1;
		 }
		 else if(order == REGISTER_OK)  /* register new account */
		 {
			printf("Now to login with new account!\n"); 
			if(clnt_login(ssl,CIN) == LOGIN_OK)
			{
					return 1;
			}
		 }
		 else 
		 {
			fprintf(stderr,"login failure!\n"); 
			break;
		 }

  }
  return -1;
}
int show_Mmain(SSL *ssl)
{
	int order;
	while(1)
	{
//		logo_ui();
		primary_ui();
		order = Mmain();	
		order = parse_clnt_order(ssl,order);
		if(order == COUT)	/* client logout */
			break;
	}
}
int main(int argc, char **argv)
{
  int sockfd,fd;
//  char fileName[50];
  //char buffer[MAXBUF + 1];
  SSL_CTX *ctx;
  SSL *ssl;
#if 0
  if (argc != 3)
  {
    printf("Usage：%s IP Port\n\tSuch as:\t%s 127.0.0.1 80\n", argv[0], argv[0]);
	exit(0);
  }
#endif
  /* SSL init */
  SSL_library_init();
  OpenSSL_add_all_algorithms();
  SSL_load_error_strings();
  /* new a ctx session */
  ctx = SSL_CTX_new(SSLv23_client_method());
  if (ctx == NULL)
  {
    ERR_print_errors_fp(stdout);
    exit(1);
  }
  /*create a socket*/
  sockfd = sock_init("127.0.0.1",7838);
  /* create a ssl with */
  ssl = SSL_new(ctx);
  SSL_set_fd(ssl, sockfd);
  /* establish SSL connection */
  if (SSL_connect(ssl) == -1)
  {
    ERR_print_errors_fp(stderr);
  }
  else
  {
	system("clear");
    printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
    show_certs(ssl);
  }
  int n = show_Mlogin(ssl);
	if(n == 1)
	{
	//	system("clear");
		
		show_Mmain(ssl);
	}
	else if(n < 0) 
	{
		fprintf(stderr,"login failure!\n");		
	}

  /* Receiving user input file name, and open the file */
  //send_file(ssl);
  /* close connection */
  SSL_shutdown(ssl);
  SSL_free(ssl);
  close(sockfd);
  SSL_CTX_free(ctx);
  return 0;
}
