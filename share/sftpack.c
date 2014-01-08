#include "sftpack.h"
#include <string.h>
#define INVAILD		-1
/* order set */
int order_set[ORDER_NUM] = {40,41,42,43,44,45,46,47};
/* check order,and return the order index int the order set */
int order_isexist(int order)
{
	int i;
		for (i = 0; i < ORDER_NUM ; i++) {
			if(order_set[i] == order)
			{
				return i;
			}
		}
	return -1;
}
/* init the package data */
void sftpack_init(SFT_PACK *sftpack)
{
	memset(sftpack,0,sizeof(SFT_PACK));
}
/* wrap the package */
int sftpack_wrap(SFT_PACK *sftpack,int order,int ack,char *buf)
{
	sftpack_init(sftpack);
	sftpack->order = order;
	sftpack->ack = ack;
	/* non-text files contain some '\0' characters ,so it  
	 * will lose some message even cause garbled code when 
	 * use strcpy. */
	memcpy(sftpack->buf,buf,DATA_SIZE);
	
	return 0;
}
/* send package */
int sftpack_send(SSL *ssl,SFT_PACK *sftpack)
{
	int n;
	n = SSL_write(ssl,sftpack,sizeof(SFT_PACK));
	if(n < 0)
	{
		perror("sftpack_send:");	
	}
	return n;
}
/* receive package */
int sftpack_recv(SSL *ssl,SFT_PACK *sftpack)
{
	int n;
	n = SSL_read(ssl,sftpack,sizeof(SFT_PACK));
	if(n < 0)
	{
		perror("sftpack_recv:");	
	}
	return n;
}
/* get server respond */
int serv_ack_code(SSL *ssl,int order)
{
	SFT_PACK sftpack;
	int ret = -1;
	int n = 0;
	do{
	/* get respond from server */	
		if(sftpack_recv(ssl,&sftpack) > 0)	/* get pack */
			break;
		printf("server is busy,wait for a moment...\n");
		sleep(1);	/* wait respond*/
		n++;
	}while(n < 3);
	if(sftpack.order == order)
		ret = sftpack.ack;
	return ret;
}
