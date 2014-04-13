/*
 * ========================================================================
 *
 *       Filename:  srv_handle.c
 *
 *    Description:  handle client's requests 
 *
 *        Version:  1.0
 *           Date:  04/12/14 06:38:58 PM
 *       Compiler:  gcc-4.6.3
 *
 *         Author:  Populus Huang 
 *          Email:  thoughtsonlive@gmail.com
 *
 * ========================================================================
 */

#include "srv_handle.h"
#include "list.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
int handle_login(SSL *ssl,SFT_PACK *clnt_pack,sqlite3 *db)
{
	int ack;
	/* user's root directory */
	char path[PATH_SIZE] = "User/";
	ACCOUNT user;
	user = clnt_pack->data.user;
	ack = account_verify(db,user.name,user.passwd);
	if(ack == USER_OK)		
	{
		/* create a directory by user name 
		 * for each client if not exist.
		 * */
		strcat(path,user.name);
		sftfile_userdir(path);	
	}
	/* ack client */
	sftpack_send_ack(ssl,clnt_pack->order,ack);
	return ack;
}
int handle_register(SSL *ssl,SFT_PACK *clnt_pack,sqlite3 *db)
{
	ACCOUNT user;
	int ret;
	user = clnt_pack->data.user;
	ret = account_register(db,user.name,user.passwd);
	sftpack_send_ack(ssl,clnt_pack->order,ret);
	return ret;
}
int handle_modify_passwd(SSL *ssl,SFT_PACK *clnt_pack,sqlite3 *db)
{
	ACCOUNT user;
	int ack,ret;
	user = clnt_pack->data.user;
	ret = account_verify(db,user.name,NULL);
	if(ret == USER_OK)	/* user exist */
	{
		ack = account_modify_passwd(db,user.name,user.passwd);	
	}
		sftpack_send_ack(ssl,clnt_pack->order,ack);
	return 0;
}
/* scan files and send the files' list to client */
int handle_scan_dir(SSL *ssl,SFT_PACK *clnt_pack)
{
	char cmd_buf[DATA_SIZE];
	char *user_root="User/";
	int order;
	char path[PATH_SIZE];

	bzero(cmd_buf,sizeof(cmd_buf));
	bzero(path,sizeof(path));

	order = clnt_pack->order;
	/* scan user's file,use 'clnt_pack->buf' to get the name */
	sprintf(path,"%s%s",user_root,clnt_pack->buf);
	if(access(path,F_OK) != 0) /* path not exist */
	{
		sftpack_send_ack(ssl,order,PATH_ERR);
		return -1;
	}
	
	sprintf(cmd_buf,"ls %s %s ",
			clnt_pack->data.file_attr.name,path);
	/* send the files' list */
	list_send_print(ssl,order,cmd_buf);
	return 0;
}
int	write_log(char *logmsg) 
{
	int fd;
	fd = open("./sftp.log",O_CREAT|O_RDWR|O_APPEND,0666);
	if(fd < 0)
	{
		perror("open sftp.log:");	
		return -1;
	}
	Write(fd,logmsg,strlen(logmsg));
	return 0;
}
int get_local_date(char *date,int date_size)
{
	FILE *date_stream;
	date_stream = popen("date","r");
	while(fgets(date,date_size,date_stream) != NULL)
			;
	date[strlen(date)-1] = '\0';
	pclose(date_stream);
	return 0;
}
/* receive client's file */
int handle_recv_file(SSL *ssl,SFT_PACK *clnt_pack)
{
	int fd;
	char *path = "./User/";
	char filename[FILENAME_SIZE];
	char logmsg[DATA_SIZE];
	char date[40];
	int file_size = 0;
	int order;
	bzero(date,sizeof(date));
	memset(filename,0,sizeof(filename));
	memset(logmsg,0,sizeof(logmsg));

	order = clnt_pack->order;	
	file_size = clnt_pack->data.file_attr.size;

	if(clnt_pack->ack == ASK)
	{
		sprintf(filename,"%s%s",path,clnt_pack->data.file_attr.name);
		fd = sftfile_open(filename,O_CREAT|O_RDWR|O_TRUNC);
#if 1
		if(fd < 0)
		{
			sftpack_send_ack(ssl,order,FAIL);
			return -1;
		}
		else
		{
			sftpack_send_ack(ssl,order,ACCEPT);
		}
		sleep(1);	/*wait for client to send file*/
#endif
		if(sftfile_recv(ssl,order,fd,file_size)==0)
		{
			sftpack_send_ack(ssl,order,FINISH);
			/* write log*/
			get_local_date(date,40);
			sprintf(logmsg,"Client:%s\n\tUpload date:%s\n\tFile name:%s\n\tFile size:%0.0fKB\n",
					clnt_pack->buf,date,filename,(float)file_size/1024);
			write_log(logmsg);
		}
	}
	return 0;
}
/* respond client's download */
int handle_send_file(SSL *ssl,SFT_PACK *clnt_pack)
{
	int fd;
	int file_size = 0;
	int order;
	char filename[FILENAME_SIZE];	
	SFT_PACK ack_pack;
	bzero(filename,sizeof(filename));
	order = clnt_pack->order;	
	/* get filename from the package */
	sprintf(filename,"./User/%s",clnt_pack->data.file_attr.name);

	fd = sftfile_open(filename,O_RDONLY);
	if(fd < 0)
	{
		sftpack_send_ack(ssl,order,FAIL);
		return -1;
	}
	sftpack_wrap(&ack_pack,order,ACCEPT,"");
	ack_pack.data.file_attr.size = file_size;
	sftpack_send(ssl,&ack_pack);
	/* send file */
	file_size = sftfile_get_size(filename);
	sftfile_send(ssl,clnt_pack->order,fd,file_size);
}
void handle_logout(SSL *ssl,SFT_PACK *clnt_pack)
{
	printf("%s say goodbye!\n",clnt_pack->data.user.name);	
}
