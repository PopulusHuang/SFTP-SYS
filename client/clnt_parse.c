/*
 * ========================================================================
 *
 *       Filename:  clnt_parse.c
 *
 *    Description:  parse clients' order and send request to server 
 *
 *        Version:  1.0
 *           Date:  04/12/14 09:05:11 PM
 *       Compiler:  gcc-4.6.3
 *
 *         Author:  Populus Huang 
 *          Email:  thoughtsonlive@gmail.com
 *
 * ========================================================================
 */
#include "clnt_parse.h"
#include "echo_mode.h"
#include "list.h"
#include "ui.h"
#include "console.h"
#include "command.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define BUF_SIZE 1024
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
		case CLIST:  scan_main(ssl,order);break;
		case   CUP:  upload_files(ssl,order);break;
		case CDOWN:  download_files(ssl,order);break;
		case CMODIFY_PASSWD: modify_passwd(ssl,order);break;
		case CONSOLE:	console(ssl);break;
		case  COUT:  ret=logout(ssl,order);break;
		default:     fprintf(stderr,"null order!\n");	
					 	ret = -1;break;
	}
	return ret;
}
int modify_passwd(SSL *ssl,int order)
{
	ACCOUNT user;
	int ack;
	modify_passwd_ui();
	echo_mode(STDIN_FILENO,ECHO_OFF);	/* set echo off */
	printf(YELLOW"Old Password: ");	
	fgets(user.passwd,PASSWD_SIZE,stdin);	
	user.passwd[strlen(user.passwd)-1] = '\0';
	/* verify old password */
	if(strcmp(LOGIN_USER.passwd,user.passwd) == 0)
	{
		printf("\nNew Passwd: ");
		fgets(user.passwd,PASSWD_SIZE,stdin);	
		user.passwd[strlen(user.passwd)-1] = '\0';
		strcpy(user.name,LOGIN_USER.name);
		account_send(ssl,user,order);	
	}
	else
	{
		printf(RED"\nPassword error!\n"NONE);
	}
	ack = sftpack_recv_ack(ssl,order);	
	if( ack == PASSWD_OK)
	{
		printf("\nPasswd modify succeed!\n");	
		LOGIN_USER=user;
	}
	echo_mode(STDIN_FILENO,ECHO_ON);	/* set echo off */
	return 0;
}
/* handle client login */
int clnt_login(SSL *ssl,int order)
{
		ACCOUNT account;
		int ret = 0;
		memset(&account,0,sizeof(account));
		login_ui();
login:	account_input(account.name,"User name: ",NAME_SIZE);

		echo_mode(STDIN_FILENO,ECHO_OFF);	/* set echo off */
pwloop:	account_input(account.passwd,"Password: ",PASSWD_SIZE);
		echo_mode(STDIN_FILENO,ECHO_ON);	/* set echo on */

		/* send account message to server */
		account_send(ssl,account,order);
		/* get respond from server */	
		int n = sftpack_recv_ack(ssl,order);
		switch(n)
		{
 			/* entry succeed */
 			case USER_OK:	ret = LOGIN_OK;
							bzero(LOGIN_USER.name,NAME_SIZE);
							LOGIN_USER = account;
							system("clear");
							logo_ui();
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
		register_ui();
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
			int n = sftpack_recv_ack(ssl,order);
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
		printf("\n");
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
	char scan_path[PATH_SIZE];

	memset(scan_path,0,sizeof(scan_path));
	system("clear");
	logo_ui();
	printf("Current files:\n");
	list_client(".","-lhF --color=auto");
	while(1)
	{
		if(get_path(scan_path,"Local directory or files") < 0)
				break;
		list_client(scan_path,"-lhF --color=auto");
	}
}
/* input the path to scan */
int get_path(char *scan_path,char *prompt)
{
		char buf[BUF_SIZE];	
		bzero(buf,sizeof(buf));
		printf(RED"Enter 'quit' or 'exit' to goback!\n"NONE);

		printf(YELLOW"%s>>"NONE,prompt);

		fgets(buf,BUF_SIZE,stdin);
		buf[strlen(buf)-1] = '\0';
		/* remove blank character and copy to path*/
		remove_space(buf,scan_path);
		if(strcmp(scan_path,"quit")==0||strcmp(scan_path,"exit")==0)
				return -1;
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
/* scan user files on the server */
int scan_serv_files(SSL *ssl,int order)
{
	//int ack;
	//SFT_PACK pack;
	char path[DATA_SIZE];
	char scan_path[DATA_SIZE];

	memset(scan_path,0,sizeof(scan_path));
	system("clear");
	logo_ui();
	/* list user files on the server */
	list_server(ssl,order,LOGIN_USER.name," -lhgoF ");
	/*input the server path*/
	while(get_path(path,"Server directory or files") != -1)
	{
		/* send the path to server */
		if(strstr(path,"..")!=NULL)
		{
			fprintf(stderr,"Path error:deny contain '..'\n");	
			return -1;
		}
		sprintf(scan_path,"%s/%s",LOGIN_USER.name,path);
		list_server(ssl,order,scan_path," -lhgaoF ");
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
/* upload files to server */
int upload_files(SSL *ssl,int order)
{
  	int n;
	int fd;
	SFT_PACK pack;
	SFT_DATA data;
	char filename[FILENAME_SIZE];
	char *line = NULL;
	int file_size;

	bzero(filename,sizeof(filename));
	/* scan local and server files */
	scan_all(ssl);
	/* get file name*/
	//sftfile_get_name(filename,"Upload");
	line = readline("Upload file name>>");	
	strncpy(filename,line,FILENAME_SIZE);
	free(line);
    /* open the local file */
	fd = sftfile_open(filename,O_RDONLY);
	if(fd < 0)
	{
		return -1;	
	}
	file_size = sftfile_get_size(filename);
	cut_path(filename);
	sprintf(data.file_attr.name,"%s/%s",LOGIN_USER.name,filename);
	data.file_attr.size = file_size;
	//strcpy(data.file_attr.name,filename);
	/* package data and send */
	sftpack_wrap(&pack,order,ASK,"\0");	
	pack.data = data;
	strcpy(pack.buf,LOGIN_USER.name);
	sftpack_send(ssl,&pack);
#if 1
	/* get respond */
	n = sftpack_recv_ack(ssl,order);
	if(n == ACCEPT)
	{
	 sftfile_send(ssl,order,fd,file_size);
	 n = sftpack_recv_ack(ssl,order);
	 if(n == FINISH)
	 {
	 	printf("upload %s to sever succeed,total %0.1fKB\n",
	   	filename,(float)file_size/1024);
		list_server(ssl,CSCS,LOGIN_USER.name,"-xF");	
		divline_ui();
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
	SFT_PACK pack;
	SFT_DATA data;

	/* scan local and server files */
	scan_all(ssl);
	sftfile_userdir(DOWNLOAD_DIR);
	bzero(localname,sizeof(localname));
	/* input filename on server to download */
	sftfile_get_name(filename,"Download");
	if(strstr(filename,"..")!=NULL)
	{
		fprintf(stderr,"filename error: deny contain '..'\n");	
		return -1;
	}
	//sprintf(serv_path,"%s/%s",LOGIN_USER.name,filename);
	sprintf(data.file_attr.name,"%s/%s",LOGIN_USER.name,filename);

	cut_path(filename);
	sprintf(localname,"%s%s",DOWNLOAD_DIR,filename);
	/*send file information to server */
	sftpack_wrap(&pack,order,ASK,"");	
	pack.data = data;
	sftpack_send(ssl,&pack);
	/* receive server respond */
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
			list_client(DOWNLOAD_DIR," --color=auto ");
			divline_ui();
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
		strcpy(pack.data.user.name,LOGIN_USER.name);	
		sftpack_send(ssl,&pack);
		printf("%s logout \n",LOGIN_USER.name);
		rtn = COUT;
	}
	return rtn;
}
int scan_main(SSL *ssl)
{
	int order;
	system("clear");
	order = Mlist();	
	switch(order)
	{
		case CSCL:	scan_local_files();break;	
		case CSCS:	scan_serv_files(ssl,order);break;
		case CSCLS: scan_all(ssl);break;	
		default: fprintf(stderr,"No such option!\n");
				 return -1;
	}
	return 0;
}
/* scan client and server files both*/
int scan_all(SSL *ssl)
{
	system("clear");
	logo_ui();
	list_server(ssl,49,LOGIN_USER.name,"-xF");	
	list_client(DOWNLOAD_DIR," --color=auto ");
}
