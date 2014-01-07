#include "srv_handle.h"
#include <fcntl.h>
#include <sys/types.h>
/* handle client's login */
int handle_login(SSL *ssl,SFT_PACK *clnt_pack,sqlite3 *db)
{
	int ack;
	ACCOUNT user;
	SFT_PACK ack_pack;
	user = clnt_pack->data.user;
	ack = account_verify(db,user.name,user.passwd);
	/* ack to client */
	ack_pack.order = clnt_pack->order;
	ack_pack.ack = ack;
	sftpack_send(ssl,&ack_pack);
	return ack;
}
/* handle client's register */
int handle_register(SSL *ssl,SFT_PACK *clnt_pack,sqlite3 *db)
{
	ACCOUNT user;
	int ret;
	SFT_PACK ack_pack;
	user = clnt_pack->data.user;
	ret = account_register(db,user.name,user.passwd);
    ack_pack.order = clnt_pack->order;		
	ack_pack.ack = ret;
	return ret;
}

/* scan file and send the files' list to client */
int handle_scan_dir(SSL *ssl,SFT_PACK *clnt_pack)
{
	FILE *list_stream;	/* point to file list stream */	
	char buf[DATA_SIZE];
	SFT_PACK ack_pack;

	bzero(buf,sizeof(buf));
	/* list current directory */	
	list_stream = popen(clnt_pack->buf,"r");	
	ack_pack.order = clnt_pack->order;
	if(list_stream != NULL)
	{
		ack_pack.ack = ACCEPT;
		sftpack_send(ssl,&ack_pack);
	}
	else
	{
		ack_pack.ack= NOT_FOUND; 	
		sftpack_send(ssl,&ack_pack);
		return -1;
	}
	/* send the result to client */
	while(fgets(ack_pack.buf,DATA_SIZE-1,list_stream) != NULL)
	{
		//sftpack_wrap(&ack_pack,clnt_pack->order,ACCEPT,data);
		sftpack_send(ssl,&ack_pack);
	}
	pclose(list_stream);
	ack_pack.ack = FINISH;
	sftpack_send(ssl,&ack_pack);
	return 0;
}
int handle_recv_file(SSL *ssl,SFT_PACK *clnt_pack)
{
	int fd;
	char *path = "./client_file/";
	char filename[FILENAME_SIZE];
	int file_size = 0;
	int order;
	SFT_PACK ack_pack;
	bzero(filename,sizeof(filename));

	order = clnt_pack->order;	
	file_size = clnt_pack->data.file_attr.size;
	if(clnt_pack->ack == ASK)
	{
		sprintf(filename,"%s%s",path,clnt_pack->data.file_attr.name);
		fd = sftfile_open(filename,O_CREAT|O_RDWR|O_TRUNC);
		if(fd < 0)
		{
			sftpack_wrap(&ack_pack,order,FAIL,"\0");
			sftpack_send(ssl,&ack_pack);
			return -1;
		}
		else
		{
			sftpack_wrap(&ack_pack,order,ACCEPT,"\0");
			sftpack_send(ssl,&ack_pack);
		}
		sleep(1);	/*wait for client to send file*/
		if(sftfile_recv(ssl,ack_pack.order,fd,file_size)==0)
		{
			sftpack_wrap(&ack_pack,order,FINISH,"\0");
			sftpack_send(ssl,&ack_pack);
		}
	}
	return 0;
}
/* respond client's download */
int handle_send_file(SSL *ssl,SFT_PACK *clnt_pack)
{
	int fd;
	int file_size = 0;
	char filename[FILENAME_SIZE];	
	SFT_PACK ack_pack;

	bzero(filename,sizeof(filename));
	ack_pack.order = clnt_pack->order;	
	/* get filename from the pack */
	strcpy(filename,clnt_pack->data.file_attr.name);	

	fd = sftfile_open(filename,O_RDONLY);
	if(fd < 0)
	{
		ack_pack.ack = FAIL;
		sftpack_send(ssl,&ack_pack);
		return -1;
	}
	/* get file size */
	file_size = sftfile_get_size(filename);
	ack_pack.data.file_attr.size = file_size;
	ack_pack.ack = ACCEPT;
	sftpack_send(ssl,&ack_pack);
	sftfile_send(ssl,clnt_pack->order,fd,file_size);
}
void handle_logout(SSL *ssl,SFT_PACK *clnt_pack)
{
	printf("%s say goodbye!\n",clnt_pack->data.user.name);	
}
