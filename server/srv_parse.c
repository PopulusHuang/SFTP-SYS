/* srv_parse.c */
#include <stdio.h>
#include "srv_parse.h"
#include "srv_handle.h"
#define BUF_SIZE 1024
/* parse client's order */
int parse_clnt_pack(SSL *ssl,SFT_PACK *clnt_pack,sqlite3 *db)
{
	int ret = 0;
	int n = order_isexist(clnt_pack->order);	
	if (n < 0)
	{
		fprintf(stderr,"%d:order is not exist!\n",clnt_pack->order);
		return -1;
	}
	switch(clnt_pack->order)
	{
		case   CIN:	 handle_login(ssl,clnt_pack,db);break;	
		case  CREG:	 handle_register(ssl,clnt_pack,db);break;
		case  CMODIFY_PASSWD:	handle_modify_passwd(ssl,clnt_pack,db);break; 
		case  CSCS:	 handle_scan_dir(ssl,clnt_pack);break;
		case   CUP:  handle_recv_file(ssl,clnt_pack);break;
		case CDOWN:	 handle_send_file(ssl,clnt_pack);break;
		case  COUT:  handle_logout(ssl,clnt_pack);
					 ret=COUT;break;
		default:  fprintf(stderr,"%d:null order:!\n",clnt_pack->order);
					 	ret = -1;break;
	}
	return ret;
}
#if 0
/* get account's name and password */
int get_account(SSL *ssl,char *order,ACK *ack,ACCOUNT *account)
{

	/* get the account data */
	fetch_name(ack->data,account->name);
	fetch_passwd(ack->data,account->passwd);
	return 0;
}
int create_emptyfile(SSL *ssl,char *new_fileName,char *buf)
{
	int n = 0;
	int fd;
//	n = strlen(new_fileName);
 //   bzero(buf, BUF_SIZE + 1);
//    bzero(new_fileName+n, n);
	strcat(new_fileName,buf);
    if((fd = open(new_fileName,O_CREAT|O_RDWR|O_TRUNC,0666))<0)
    {
      perror("open:");
	  return -1;
    }
	printf("open file:%s succeed\n",new_fileName);
	return fd;
}
/* write data to the empty file */
int write_data(SSL *ssl,char *order,int fd)
{
	int len;
	int n;
	char buf[BUF_SIZE];
	char clnt_ack[ORDER_SIZE];
    bzero(buf,sizeof(buf));
	bzero(clnt_ack,sizeof(clnt_ack));
	sprintf(clnt_ack,"0%s",order);
//	SSL_write_pk(ssl,buf,ORDER_SIZE);
    while(1)
    {
      len = SSL_read_pk(ssl,buf, BUF_SIZE-1); /* read data from client */
	  if(len < 0)
	  {
        printf("Receive File Failure!\n");
		sprintf(buf,"2%s",order);
		SSL_write_pk(ssl,buf,BUF_SIZE);
	 	 break;	  
	  }
	  if((strncmp(buf,clnt_ack,ORDER_SIZE) == 0)||len == 0)
      {
        printf("Receive File Complete !\n");
		sprintf(buf,"1%s",order);
		SSL_write_pk(ssl,buf,BUF_SIZE);
        break;
      }
      if(write(fd,buf,len)<0)	/* write data to server */
      {
        perror("write:");
		return -1;
      }
    bzero(buf,sizeof(buf));
    }
	return 0;
}
/* receive clients' file */
int recv_file(SSL *ssl,char *data)
{
	int fd;
	char fpath[FILENAME_SIZE]="./client_file/";	/* receive file's path */
	char filename[50];
	char order[8];
	char buf[8];
	bzero(filename,sizeof(filename));
	bzero(order,sizeof(order));
	bzero(buf,sizeof(buf));

	fetch_data(data,filename,CUT_PATH);	
	fetch_data(data,order,CUT_ORDER);
	/* start to receive file */
	strcat(fpath,filename);

    if((fd = open(fpath,O_CREAT|O_RDWR|O_TRUNC,0666))<0)
    {
      perror("open:");
	  return -1;
    }
	sleep(1);
	write_data(ssl,order,fd);
	close(fd);
	return 0;
}
/* send file to client */
int send_file(SSL *ssl,char *data)
{
	ACK ack;	
	char filename[FILENAME_SIZE+1];
	char order[8] = "";
	int fd,n;	
	int ret = 0;
	bzero(&ack,sizeof(ack));
	bzero(filename,sizeof(filename));
	/* get order */
	fetch_data(data,order,CUT_ORDER);
	/* get the file name */	
	fetch_data(data,filename,CUT_PATH);
	fd = open(filename,O_RDONLY);	
	if(fd < 0)
	{
		perror("open");	
		sprintf(ack.order,"2%s",order);
		ret = -1;
	}
	else
	{
		sprintf(ack.order,"0%s",order);	
	}
		/* inform client to get ready */
		SSL_write_pk(ssl,ack.order,ORDER_SIZE);
		/* copy the file to client */
	while((n=Read(fd,ack.data,DATA_SIZE)) > 0)
	{
		SSL_write_pk(ssl,ack.data,n);	
	}
		/* inform client to complete copy */
		sprintf(ack.order,"1%s",order);	
		SSL_write_pk(ssl,ack.order,ORDER_SIZE);
		return ret;
}
void clnt_logout(SSL *ssl,char *data)
{
	char account[NAME_SIZE]; 
	ACK ack;
	bzero(&ack,sizeof(ack));
	bzero(account,sizeof(account));

	//fetch_data(data,ack.order,CUT_ORDER);
	fetch_data(data,account,CUT_NAME);
	printf("%s say goodbye!\n",account);	
}
#endif
