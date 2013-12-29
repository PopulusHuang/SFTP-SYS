/*ssl client.c*/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <resolv.h>
#include <stdlib.h>
#include <fcntl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "../share/ssl_wrap.h"
#include "../share/sock_wrap.h"
#include "menu.h"
#include "parse.h"
#define MAXBUF 1024
#define ORDER_SIZE 10
#define LOGIN_OK 1
#define REGISTER_OK 2
void ShowCerts(SSL * ssl)
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
int sock_init(char **argv)
{
  int sockfd;
  struct sockaddr_in servaddr;
  int len;
  sockfd = Socket(AF_INET, SOCK_STREAM, 0);

  /* initial server's address and port */
  bzero(&servaddr,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(argv[2]));
  if (inet_aton(argv[1], (struct in_addr *)servaddr.sin_addr.s_addr) == 0)
  {
    perror(argv[1]);
    exit(errno);
  }
  printf("address created\n");

  /* Connect to server */
  len =sizeof(servaddr);
  Connect(sockfd,(struct sockaddr *)&servaddr,len);
  printf("server connected\n\n");
  return sockfd;
}
/* open local file */
int open_file(char *fileName)
{
  int fd;
  printf("\nPlease input the filename of you want to load :\n(~_^)'$");

  scanf("%s",fileName);
  if((fd = open(fileName,O_RDONLY,0666))<0)
  {
    perror("open");
    exit(1);
  }else{
	printf("open %s succeed!\n",fileName);
  }
  return fd;
}
/* send the filename to server */
int send_filename(SSL *ssl,char *fileName)
{
  int i,j = 0;
  int len;
  char sendFN[20];
  printf("start send %s to server\n",fileName);
  bzero(sendFN,strlen(sendFN));
  /* remove the path and extract the filename*/
#if 1
  for(i=0;i<=strlen(fileName);i++)
  {
    if(fileName[i]=='/')
    {
      j=0;
      continue;
    }
    else {
	  sendFN[j]=fileName[i];
	  ++j;
	}
  }
#endif
  printf("sendFN:%s\n",sendFN);
  SSL_write_pk(ssl, sendFN, strlen(sendFN)); /*send to server*/
  return 0;
}
/* send the local file to server */
void send_data(SSL *ssl,int fd)
{
  int size;
  int len;
  char buffer[MAXBUF + 1];

  bzero(buffer, MAXBUF + 1); 
  while((size=Read(fd,buffer,MAXBUF)))
  {
      len = SSL_write_pk(ssl, buffer, size);
      if (len < 0)
        printf("'%s'message Send failure ！Error code is %d，Error messages are '%s'\n", buffer, errno, strerror(errno));
      bzero(buffer, MAXBUF + 1);
  }
  printf("Send complete !\n");

}
int show_Mlogin(SSL *ssl)
{
  char order[ORDER_SIZE];
  while(1)
  {
  	   	 Mlogin(order);  
	     if(parse_clnt_order(order) == LOGIN_OK)
		 {
			return 1;
		 }
		 else if(parse_clnt_order(order) == REGISTER_OK)  /* register new account */
		 {
			printf("Now to login with new account!\n"); 
			if(clnt_entry(order_set[CIN]))
			{
					return 1;
			}
		 }

  }
  return -1;
}
int show_Mmain(SSL *ssl)
{
	char order[ORDER_SIZE];
	while(1)
	{
		Mmain(order);	
		(parse_clnt_order(order);
	}
}
int send_file(SSL *ssl)
{
  int fd;
  char fileName[50];
  fd = open_file(fileName);
  send_filename(ssl,fileName);
  send_data(ssl,fd);
  close(fd);
  return 0;
}
int main(int argc, char **argv)
{
  int sockfd,fd;
//  char fileName[50];
  //char buffer[MAXBUF + 1];
  SSL_CTX *ctx;
  SSL *ssl;

  if (argc != 3)
  {
    printf("Usage：\n\t\t%s IP Port\n\tSuch as:\t%s 127.0.0.1 80\n", argv[0], argv[0]);
	exit(0);
  }

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
  sockfd = sock_init(argv);
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
    printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
    ShowCerts(ssl);
  }
	if(show_Mlogin(ssl))
	{
		show_Mmain(ssl);
	}
	else
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
