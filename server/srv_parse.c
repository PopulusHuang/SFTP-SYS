/*
 * ========================================================================
 *
 *       Filename:  srv_parse.c
 *
 *    Description:  Parsing clients' package,extract the order and  
 *    				make appropriate treatment. 
 *
 *        Version:  1.0
 *           Date:  04/12/14 07:09:46 PM
 *       Compiler:  gcc
 *
 *         Author:  Populus Huang 
 *          Email:  thoughtsonlive@gmail.com
 *
 * ========================================================================
 */

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
