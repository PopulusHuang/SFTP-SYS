#include "order.h"
/* order set */
char order_set[ORDER_NUM][ORDER_SIZE]={"c00a&","c01b&","c02c&","c03d&","c04e&","c05f&","c06g&","c07h&"};
/* check order,and return the order index int the order set */
int is_order_exist(char *order)
{
	int i;
		for (i = 0; i < ORDER_NUM-1 ; i++) {
			if (strncmp(order_set[i],order,5) == 0)	
			{
				return i;
			}
		}
	return -1;
}
/* fetch order from data,0:succeed -1:failure */
int fetch_order(char *data,char *order)
{
	int i,data_size;
	data_size = strlen(data);
	for(i = 0;i < data_size;i++)
	{
		if(data[i] == '&')	/* order end of '&'*/	
			return 0;		/* complete copy */
		else 
			order[i]=data[i];
	}
	return -1;
}
/* find out the character 'c' in the string which 
 * the pointer 'data' point to*/
int find_char(char *data,char c)
{
	int i;
	int len;
	len = strlen(data);
	for (i = 0; i < len; i++) {
		if(data[i] == c)	
			return i;
	}
	return -1;
}
/* cut a string 'src' from 'min' to 'max',
 * save the result to dest*/
int cut_string(char *src,char *dest,int min,int max)
{
	int i,j = 0;
	for (i = min; i < max; i++) {
		dest[j++] = src[i];	
	}
	dest[j] = '\0';
	return 0;
}
/* fetch message from data,the 'cutflg' value was 
 * define at order.h; 
 * data format: ORDER & MESSAGE
 * example
 * 		1. ORDER&NAME@passwd
 * 		2. ORDER&$PATH
 * */
int fetch_data(char *data,char *cutstr,int cutflg)
{
	int min,max;
	int data_size = strlen(data)+1;
	if(data_size > (DATA_SIZE-1) )
	{
		printf("order is too long!\n");	
		return -1;
	}
	switch(cutflg)
	{
		/* order start at first, end by & */
		case CUT_ORDER: min = 0;
						max = find_char(data,'&')+1;
						break;
		/* path start at '$'*/
		case CUT_PATH:  min = find_char(data,'$')+1;/* skip '$'*/ 
						max = data_size;
						break;	
		/* name between '&' and '@'*/
		case CUT_NAME:  min = find_char(data,'&')+1; /* skip '&'*/
						max = find_char(data,'@');
						break;
		/* passwd behind @ */
		case CUT_PASSWD: min = find_char(data,'@')+1;/* skip '@'*/
						max = data_size;
						break;
		default:		return -1;
	}
	cut_string(data,cutstr,min,max);
	return 0;
}
int send_order(SSL *ssl,char *order)
{
	int n;
	n = strlen(order);
	if(n > DATA_SIZE)
	{
			fprintf(stderr,"order is too long !");
			return -1;
	}
	n = SSL_write_pk(ssl,order,n);
	return n;
}
int recv_order(SSL *ssl,char *order)
{
	int n;
	bzero(order,strlen(order));
	n = SSL_read_pk(ssl,order,strlen(order));
	if(n > DATA_SIZE)
	{
			fprintf(stderr,"order is too long !");
			return -1;
	}
	return n;	
}
