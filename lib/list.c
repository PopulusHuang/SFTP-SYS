/*
 * ========================================================================
 *
 *       Filename:  list.c
 *
 *    Description:  Send out or receive a directory's file list
 *
 *        Version:  1.0
 *           Date:  04/12/14 08:55:33 PM
 *       Compiler:  gcc-4.6.3
 *
 *         Author:  Populus Huang 
 *          Email:  thoughtsonlive@gmail.com
 *
 * ========================================================================
 */

#include "list.h"
#include "sftpack.h"
#include "ui.h"
#include <string.h>
#define BUF_SIZE 1024
/* list user files on server */
int list_server(SSL *ssl,int order,char *path,char *flag)
{
	int ack;
	SFT_PACK pack;
		sftpack_wrap(&pack,order,ASK,path);
		strcpy(pack.data.file_attr.name,flag);
		sftpack_send(ssl,&pack);
		/* get respond */
		ack = serv_ack_code(ssl,order);
		if(ack == ACCEPT)	
		{
			printf(GREEN_BG"|                             Sever List                           |"NONE"\n");           
			list_recv_print(ssl,order);	
			divline_ui();
			return 0;
		}
		else if(ack == PATH_ERR)
		{
			printf(RED"error: invalid path!"NONE"\n");
		}
		printf(RED"request file list failure!"NONE"\n");
		return -1;
}
/* list user files on local client,'flag' is 
 * the option of linux command 'ls'*/
int list_client(char *path,char *flag)
{
	char cmd_buf[BUF_SIZE];

	memset(cmd_buf,0,sizeof(cmd_buf));
	printf(GREEN_BG"|                             Client List                          |"NONE"\n");           
	sprintf(cmd_buf,"ls %s %s ",flag,path);
	system(cmd_buf);
	divline_ui();
}
int list_send_print(SSL *ssl,int order,char *cmd_path)
{
	FILE *list_stream;	/* point to file list stream */	
	SFT_PACK ack_pack;
	list_stream = popen(cmd_path,"r");	
	if(list_stream != NULL)
	{
		/* respond accept */
		sftpack_wrap(&ack_pack,order,ACCEPT,"");
		sftpack_send(ssl,&ack_pack);
	}
	else
	{
		sftpack_wrap(&ack_pack,order,NOT_FOUND,"");
		sftpack_send(ssl,&ack_pack);
		return -1;
	}
	/* send the result to client */
	while(fgets(ack_pack.buf,DATA_SIZE-1,list_stream) != NULL)
	{
		sftpack_send(ssl,&ack_pack);
	}
	/* respond finish */
	sftpack_wrap(&ack_pack,order,FINISH,"");
	sftpack_send(ssl,&ack_pack);
	pclose(list_stream);
	return 0;
}
/* Receive file list and print it out */
int list_recv_print(SSL *ssl,int order)
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
		//	printf("Receive file list complete!\n");
			break;
		}
			printf("%s",pack.buf);
	}
	return 0;
}
