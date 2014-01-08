/*clnt_parse.c*/
#include "clnt_parse.h"
#include "echo_mode.h"
#include "../share/sftpack.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define BUF_SIZE 1024
#define CLIENT_PATH  1
#define SERVER_PATH  2
char login_name[NAME_SIZE] = "\0";

/* parse the client order */
int parse_clnt_order(SSL *ssl,int order)
{
	int n;
	int ret = 0;
	n = order_isexist(order);
	if(n < 0)
	{
		fprintf(stderr,"order is not exist!\n");	
		return -1;
	}
	switch(order)
	{
		case   CIN:  ret = clnt_login(ssl,order);break;
		case  CREG:  ret = clnt_register(ssl,order);break;
		case  CSCL:  scan_local_files();break;
		case  CSCS:  scan_serv_files(ssl,order);break; 
		case   CUP:  upload_files(ssl,order);break;
		case CDOWN:  download_files(ssl,order);break;
		case  COUT:  ret=logout(ssl,order);break;
		default:     fprintf(stderr,"null order!\n");	
					 	ret = -1;break;
	}
	return ret;
}
/* handle client login */
int clnt_login(SSL *ssl,int order)
{
		ACCOUNT account;
		int ret = 0;
		memset(&account,0,sizeof(account));
login:	account_input(account.name,"User Name: ",NAME_SIZE);

		echo_mode(STDIN_FILENO,ECHO_OFF);	/* set echo off */
pwloop:	account_input(account.passwd,"\nPasswd: ",PASSWD_SIZE);
		echo_mode(STDIN_FILENO,ECHO_ON);	/* set echo on */

		/* send account message to server */
		account_send(ssl,account,order);
		/* get respond from server */	
		int n = serv_ack_code(ssl,order);
		switch(n)
		{
 			/* entry succeed */
 			case USER_OK:	ret = LOGIN_OK;
							bzero(login_name,NAME_SIZE);
							strcpy(login_name,account.name);
							break;	 	
 			/* user not exist */
 			case USER_ERROR: fprintf(stderr,
 									"%s is not exist!\n",
 									account.name);
 							 goto login;
 			/* passwd error */
 			case PASSWD_ERR:	fprintf(stderr,
 									"password error\n");
 								goto pwloop;
 			/* undefine error */
 			default:	fprintf(stderr,"undefine error\n");
 						ret = -1;break;

	    }

		return ret;
}
/* handle client register */
int clnt_register(SSL *ssl,int order)
{
		char passwd[PASSWD_SIZE];
		int ret = 0;	
		int n,ack;
		ACCOUNT account;
		bzero(passwd,sizeof(passwd));
login:	account_input(account.name,"New User Name: ",NAME_SIZE);

		echo_mode(STDIN_FILENO,ECHO_OFF);	/* set echo off */
pwloop:	account_input(account.passwd,"\nNew Passwd: ",PASSWD_SIZE);
	    account_input(passwd,"\nConfirm New Passwd: ",PASSWD_SIZE);
		echo_mode(STDIN_FILENO,ECHO_ON);	/* set echo on */

		if(strcmp(passwd,account.passwd) != 0)
		{
			fprintf(stderr,"Passwd isn't matching,confirm failed!\n");
			goto pwloop;	
		}
		else
		{
			account_send(ssl,account,order);
			/* get respond from server */	
			int n = serv_ack_code(ssl,order);
			if(n == REGISTER_OK)
			{
				printf("%s register succeed!\n",account.name);
				puts("go to login now!");
			}
			else if(n == USER_EXIST)
			{
				fprintf(stderr,"%s is already exist\n",account.name);	
			}
		}
		return n;
}
/* remove string's blank character from 'src' 
 * and copy to 'dest'*/
void remove_space(char *src,char *dest)
{
	int i,j,n;
	n = strlen(src);
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
	dest[j] = '\0';
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
/* input the path to scan */
int get_path(char *scan_path,int flag)
{
		char buf[BUF_SIZE];	
		char path[BUF_SIZE];
		bzero(path,sizeof(path));
		printf("\033[31m@~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~@ \033[0m\n");
		printf("\033[31m >>Enter 'quit' or 'exit' to goback!\033[0m\n");

		if(flag == CLIENT_PATH)
			printf("\n\033[47;32m please input the path of \033[33mlocal:~$ \033[0m");
		else
			printf("\n\033[47;32m please input the path of \033[33mserver:~$ \033[0m");

		fgets(buf,BUF_SIZE,stdin);
		buf[strlen(buf)-1] = '\0';
		/* remove blank character and copy to path*/
		remove_space(buf,path);
		if(strcmp(path,"quit")==0||strcmp(path,"exit")==0)
				return -1;
		strcpy(scan_path,"ls -l --color=auto ");
		strcat(scan_path,path);
		return 0;
}
/* Receive file list from server */
int recv_file_list(SSL *ssl,int order)
{
	int n;
	SFT_PACK pack;
	while(1)
	{
		/* Receive package of file list */
		if(sftpack_recv(ssl,&pack) < 0)
		{
			fprintf(stderr,"Receive file list failure");		
			break;
		}
		if((pack.order==order)&&(pack.ack == FINISH))
		{
			printf("Receive file list complete!\n");
			break;
		}
			printf("%s",pack.buf);
	}
	return 0;
}
/* scan server files */
int scan_serv_files(SSL *ssl,int order)
{
	int ack;
	SFT_PACK pack;
	char buf[DATA_SIZE];
	char scan_path[DATA_SIZE];

	//bzero(scan_path,sizeof(scan_path));
	memset(scan_path,0,sizeof(scan_path));
	/*input the server path*/
	while(get_path(scan_path,SERVER_PATH) != -1)
	{
		strcpy(buf,scan_path);
		/* send the path to server */
		sftpack_wrap(&pack,order,ASK,buf);
		sftpack_send(ssl,&pack);
		/* get respond */
		ack = serv_ack_code(ssl,order);
		if(ack == ACCEPT)	/* server accept,start to receive file list */
		{
			recv_file_list(ssl,order);	
		}
		else
		{
			fprintf(stderr,"request file list failure!\n");
		}
	}
	return 0;
}
int cut_path(char *filename)
{
	  int i,j = 0;
	  int len;
	  char purename[FILENAME_SIZE];
	  len = strlen(filename);
	  /* cut the path and extract the filename*/
#if 1
	  for(i=0;i<=strlen(filename);i++)
	  {
	    if(filename[i]=='/')
	    {
	      j=0;
	      continue;
	    }
	    else {
		  purename[j++]=filename[i];
		}
	  }
	  purename[j] = '\0';
	  strcpy(filename,purename);
#endif
  	return 0;
}
int file_backup(char *filename,int fd)
{
	int bakfd;
	char path[FILENAME_SIZE] = "./recv_dir/";
	char bakname[FILENAME_SIZE];
	char buf[DATA_SIZE];
	int n;
	bzero(path,sizeof(path));
	bzero(bakname,sizeof(bakname));
	strcpy(bakname,filename);
	cut_path(bakname);
	strcat(path,bakname);
	bakfd = sftfile_open(path,O_RDWR|O_CREAT|O_TRUNC);
	if(bakfd < 0)
			return -1;
	while((n = read(fd,buf,DATA_SIZE-1)) > 0)
	{
		write(bakfd,buf,n);	
	}
	return 0;
}
/* upload files to server */
int upload_files(SSL *ssl,int order)
{
  	int n;
	int fd;
	SFT_PACK pack;
	SFT_DATA data;
	char filename[FILENAME_SIZE];
	int file_size;
	/* get file name*/
	sftfile_get_name(filename,"Upload");
    /* open the local file */
	fd = sftfile_open(filename,O_RDONLY);
	if(fd < 0)
	{
		return -1;	
	}
#if 0
	if(file_backup(filename,fd) < 0)
			return -1;
#endif 
	file_size = sftfile_get_size(filename);
	cut_path(filename);
	data.file_attr.size = file_size;
	strcpy(data.file_attr.name,filename);
	/* package data and send */
	sftpack_wrap(&pack,order,ASK,"\0");	
	pack.data = data;
	sftpack_send(ssl,&pack);
#if 1
	/* get respond */
	n = serv_ack_code(ssl,order);
	if(n == ACCEPT)
	{
	 sftfile_send(ssl,order,fd,file_size);
	 n = serv_ack_code(ssl,order);
	 if(n == FINISH)
	 {
	 	printf("upload %s to sever succeed,total %0.1fKB\n",
	   	filename,(float)file_size/1024);
	 }
	 else
	 {
	 	printf("upload %s failure!\n",filename);	
	 }
	}
	else
		fprintf(stderr,"request upload failure!\n");
#endif
  	close(fd);
	return 0;
}
/* download server's file */
int download_files(SSL *ssl,int order)
{
	int size;
	int ack,fd;
	char filename[FILENAME_SIZE];
	char localname[FILENAME_SIZE];
	char *path = "./recv_dir/";
	SFT_PACK pack;
	SFT_DATA data;
	sftfile_recvdir(path);

	bzero(localname,sizeof(localname));
	/* input filename on server to download */
	sftfile_get_name(filename,"Download");
	strcpy(data.file_attr.name,filename);
	cut_path(filename);
	sprintf(localname,"%s%s",path,filename);
	/*send file information to server */
	sftpack_wrap(&pack,order,ASK,"");	
	pack.data = data;
	sftpack_send(ssl,&pack);
	sftpack_recv(ssl,&pack);
	ack =pack.ack;
	if(ack == ACCEPT)
	{
		size = pack.data.file_attr.size;
		fd = sftfile_open(localname,O_CREAT | O_TRUNC | O_RDWR);
		if(sftfile_recv(ssl,order,fd,size) == 0)
		{
			//size = sftfile_get_size(localname);	
			printf("Downlad %s succeed\n",filename);
		}	
		else
		{
			printf("Download %s failure!\n",filename);	
			return -1;
		}
	}

	return 0;
}
int logout(SSL *ssl,int order)
{
	char buf[10];
	char verify[10];
	int n;
	int rtn = 0;
	SFT_PACK pack;

	printf("Are you want to exit now? (y/n) ");
	fgets(buf,9,stdin);	
	buf[strlen(buf)-1] = '\0';
	remove_space(buf,verify);
	if(strcmp(verify,"Y")==0||strcmp(verify,"y") == 0)
	{
		/* inform server who want to bye */
		pack.order = order;
		pack.ack = ASK;
		strcpy(pack.data.user.name,login_name);	
		sftpack_send(ssl,&pack);
		printf("%s logout \n",login_name);
		rtn = COUT;
	}
	return rtn;
}
