#include "menu.h"
#include "parse.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF_SIZE 1024
#define FILENAME_SIZE 100
#define CLIENT_PATH  1
#define SERVER_PATH  2
void create_recvdir(char *fdir)
{
  mode_t mode;
  mode=0755;
  /*Is the directly exist*/
  if(access(fdir,F_OK) != 0) 
  {
  	mkdir(fdir,mode);
  }
}
/* parse the client order */
int parse_clnt_order(SSL *ssl,char *order)
{
	int n;
	int ret = 0;
	n = is_order_exist(order);
	if(n < 0)
	{
		fprintf(stderr,"order is not exist!\n");	
		return -1;
	}
	switch(n)
	{
		case   CIN:  ret = clnt_login(ssl,order);break;
		case  CREG:  ret = clnt_register(ssl,order);break;
		case  CSCL:  scan_local_files();break;
		case  CSCS:  scan_serv_files(ssl,order);break; 
		case   CUP:  upload_files(ssl,order);break;
		case CDOWN:  download_files(ssl,order);break;
		case  COUT:  logout(ssl,order);break;
		default:     fprintf(stderr,"null order!\n");	
					 	ret = -1;break;
	}
	return ret;
}
/* fetch order and execute code from ack */
int parse_serv_order(char *ack,char *order)
{
	int rtnval;		/* return value */
	char value[2];
	/* get the first character */
	value[0] = ack[0];
	rtnval = atoi(value);
	if(order != NULL)
		strncpy(order,ack+1,ORDER_SIZE);
	return rtnval;
}
void package_order(char *order,char *data,char *order_pack)
{
	strcat(order_pack,order);
	strcat(order_pack,"*");
	strcat(order_pack,data);
}
/* send account to server */
int send_account(SSL *ssl,ACK *ack,ACCOUNT *account,char *order)
{
		
		/* send order to server */
		SSL_write_pk(ssl,order,ORDER_SIZE);
		SSL_read_pk(ssl,ack->order,ORDER_SIZE);
		printf("respond %s from server\n",ack->order);
		if(parse_serv_order(ack->order,NULL) == 0)
		{
			/* connect name and passwd  with '@' */
			strcpy(ack->data,account->name);	
			strcat(ack->data,"@"); 
			strcat(ack->data,account->passwd);
			/* send name and passwd */
			SSL_write_pk(ssl,ack->data,ORDER_SIZE);
			printf("write %s to server.\n",ack->data);
		}
		else
		{
			fprintf(stderr,"server busy!\n");
			exit(1);
		}
			return 0;

}
/* handle client login */
int clnt_login(SSL *ssl,char *order)
{
		ACK ack;
		ACCOUNT account;
		int ret = 0;
		bzero(&ack,sizeof(ack));
		bzero(&account,sizeof(account));
login:  printf("user name:");
		fgets(account.name,NAME_SIZE,stdin);
		/* remove the <Enter> key */
		account.name[strlen(account.name)-1] = '\0';   
passwd:	printf("\npasswd:");	
		fgets(account.passwd,PASSWD_SIZE,stdin);
		account.passwd[strlen(account.passwd)-1] = '\0';
		/* send account message to server */
		send_account(ssl,&ack,&account,order);
		bzero(&ack,sizeof(ack));
		/* get respond from server */	
		SSL_read_pk(ssl,ack.data,ORDER_SIZE);

		int n = parse_serv_order(ack.data,ack.order);
		if(strcmp(order,ack.order) == 0)	
		{
			switch(n)
			{
				/* entry succeed */
				case USER_OK:	ret = LOGIN_OK;break;	 	
				/* user not exist */
				case USER_ERROR: fprintf(stderr,
										"%s is not exist!\n",
										account.name);
								 goto login;
				/* passwd error */
				case PASSWD_ERROR:	fprintf(stderr,
										"password error\n");
									goto passwd;
				/* undefine error */
				default:	fprintf(stderr,"undefine error\n");
							ret = -1;break;

			}
		}

		return ret;
}
/* handle client register */
int clnt_register(SSL *ssl,char *order)
{
		char passwd[PASSWD_SIZE];
		int ret = 0;	
		ACK ack;
		ACCOUNT account;
		//bzero(order_pack,strlen(BUF_SIZE));
		bzero(&ack,sizeof(ack));
		bzero(passwd,sizeof(passwd));
		printf("new user name:");
		fgets(account.name,NAME_SIZE,stdin);
	    account.name[strlen(account.name)-1] = '\0';  
		
pwloop:	printf("new passwd:");
		fgets(account.passwd,PASSWD_SIZE,stdin);
		account.passwd[strlen(account.passwd)-1] = '\0';

		printf("confirm new passwd:");
		fgets(passwd,PASSWD_SIZE,stdin);
		passwd[strlen(passwd)-1] = '\0';

		if(strcmp(passwd,account.passwd) != 0)
		{
			fprintf(stderr,"Passwd isn't matching,confirm failed!\n");
			goto pwloop;	
		}
		else
		{
			send_account(ssl,&ack,&account,order);
			puts("after send_account...");
			bzero(&ack,sizeof(ack));
			/* get respond from server */	
			SSL_read_pk(ssl,ack.data,ORDER_SIZE);
			int n = parse_serv_order(ack.data,ack.order);
			if(strcmp(ack.order,order) == 0)
			{
				if(n == 1)
				{
					printf("%s register succeed!\n",account.name);
					puts("go to login now!");
					ret = REGISTER_OK;
				}
				else if(n == 2)
				{
					fprintf(stderr,"%s is already exist\n",account.name);	
					ret = -1;
				}
			}
		}
		return ret;
}
/* remove string's '<space>' character from 'src' 
 * and copy to 'dest'*/
void remove_space(char *src,char *dest)
{
	int i,j,n;
	n = strlen(src);
	bzero(dest,strlen(dest));
	for(i = 0,j = 0;i < n;i++)
	{
		if(src[i] == ' ')	
		{
			continue;
		}
		else
		{
			dest[j++] = src[i];
		}
	}
}
/* scan client's local files */
void scan_local_files(void)
{
	char cmd_buf[BUF_SIZE];

	bzero(cmd_buf,sizeof(cmd_buf));
	while(1)
	{
		if(get_path(cmd_buf,CLIENT_PATH) < 0)
				break;
		system(cmd_buf);
	}
}
/* input the path  to scan */
int get_path(char *scan_path,int flag)
{
		char buf[BUF_SIZE];	
		char path[BUF_SIZE];
		printf("\033[31m@~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~@ \033[0m\n");
		if(flag == CLIENT_PATH)
			printf("\n\033[47;32m please input the path of local:~$ \033[0m");
		else
			printf("\n\033[47;32m please input the path of server:~$ \033[0m");
		fgets(buf,BUF_SIZE,stdin);
		buf[strlen(buf)-1] = '\0';
		/* remove 'buf' <space> and copy to path*/
		remove_space(buf,path);
		if(strcmp(path,"quit")==0||strcmp(path,"exit")==0)
				return -1;
		strcpy(scan_path,"ls -l --color=auto ");
		strcat(scan_path,path);
		return 0;
}
/* Receive file list from server */
int recv_file_list(SSL *ssl)
{
	char buf[BUF_SIZE+1];
	int n;
	bzero(buf,sizeof(buf));
	while(1)
	{
		n =SSL_read_pk(ssl,buf,BUF_SIZE);
		if (n < BUF_SIZE )
		{
			if(n > 0)
				printf("%s",buf);	
			printf("\nreceive complete!\n");
			return 0;
		}
		else 
		{
			printf("%s",buf);	
		}
	}
	return 0;
}
/* scan server files */
int scan_serv_files(SSL *ssl,char *order)
{
	ACK ack;
	int rtn;
	char scan_path[DATA_SIZE];
	bzero(&ack,sizeof(ack));
	bzero(scan_path,sizeof(scan_path));
	if(is_order_exist(order) < 0)
	{
		fprintf(stderr,"order is not exist!");	
		return -1;
	}
	else
	{
		SSL_write_pk(ssl,order,ORDER_SIZE);
		/* get server respond */
		SSL_read_pk(ssl,ack.data,ORDER_SIZE);
		if(parse_serv_order(ack.data,ack.order) == 0)
		{
				if(strcmp(ack.order,order) == 0)
				{
					get_path(scan_path,SERVER_PATH);
					strcat(ack.data,scan_path);
					/* send the path to server */
					SSL_write_pk(ssl,ack.data,DATA_SIZE);
					/* receive server's file list */
					rtn = recv_file_list(ssl);
				}
		}
		else
		{
			fprintf(stderr,"scan server file failure!");	
		}


	}
	return 0;
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
  char buffer[BUF_SIZE + 1];

  bzero(buffer, BUF_SIZE + 1); 
  while((size=Read(fd,buffer,BUF_SIZE)))
  {
      len = SSL_write_pk(ssl, buffer, size);
      if (len < 0)
        printf("'%s'message Send failure ！Error code is %d，Error messages are '%s'\n", buffer, errno, strerror(errno));
      bzero(buffer, BUF_SIZE + 1);
  }
  printf("Send complete !\n");

}
/* send file to server */
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
/* upload files to server */
int upload_files(SSL *ssl,char *order)
{

	if(is_order_exist(order) < 0)
	{
		fprintf(stderr,"order is not exist!");	
		return -1;
	}
	else
	{
		send_order(ssl,order);
		recv_order(ssl,order);
		if(parse_serv_order(order,NULL) == 0)
			send_file(ssl);
	}
}
/* create an empty file by 'filename'*/
int create_emptyfile(char *filename)
{
		int fd = open(filename,O_CREAT | O_TRUNC | O_RDWR,0666);
		if(fd < 0)
		{
			perror("open:");	
			return -1;
		}
		printf("open file:%s succeed!\n",filename);
		return fd;
}
/* Receive file data from server and write to local file */
int recv_file_data(SSL *ssl,int fd)
{
	int n;		
	char buf[BUF_SIZE+1];
	while(1)
	{
		bzero(buf,BUF_SIZE+1);	
		/* read file data from server */
		n = SSL_read_pk(ssl,buf,BUF_SIZE);
		if(n == 0)
		{
			puts("Receive Complete!");
			break;
		}
		if(Write(fd,buf,n) < 0)
			return -1;
	}
	return 0;
}
/* download server's file */
int download_files(SSL *ssl,char *order)
{
	char filename[FILENAME_SIZE+1];
	ACK ack;
	char path[FILENAME_SIZE] = "./recv_dir";
	bzero(&ack,sizeof(ack));
    printf("\033[31m@~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~@ \033[0m\n");
    printf("\nPlease input the filename of you want to download :\n(~_^)'$");
	fgets(filename,FILENAME_SIZE,stdin);
	filename[strlen(filename)-1] = '\0';
	SSL_write_pk(ssl,order,ORDER_SIZE);
	/*send file name to server */
	SSL_write_pk(ssl,filename,FILENAME_SIZE);
	/* receive server respond */
	SSL_read_pk(ssl,ack.data,DATA_SIZE);
	if(parse_serv_order(ack.data,ack.order)==0)
	{   
		if(strcmp(ack.order,order) == 0)/* download respond */
		{
			create_recvdir(path);
			/* create an empty file on local */
			strcat(path,filename);
			int fd = create_emptyfile(path);
			/* write data to the empty file */
			recv_file_data(ssl,fd);
		}
	}
	else
	{
		fprintf(stderr,"open %s error or no such file\n",filename);	
		return -1;
	}
	return 0;
}
int logout(SSL *ssl,char *order)
{
	char buf[10];
	char verify[10];
	printf("Are you want to exit now? (y/n)\n");
	fgets(buf,9,stdin);	
	buf[strlen(buf)-1] = '\0';
	remove_space(buf,verify);
	if(strcmp(verify,"Y")==0||strcmp(verify,"y") == 0)
	{
		SSL_write_pk(ssl,order,ORDER_SIZE);
		exit(0);
	}
	return 0;
}
