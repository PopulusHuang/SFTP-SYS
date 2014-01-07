/**************************************************************  
 * Module:	sockssl.h
 * 
 * Describe: this module bind a socket a with SSL session,
 * 			server will create a ssl session when a new 
 * 			connect comming,we use a sockssl to bind 
 * 			the socket and ssl 
 *
 * Function: bind socket and ssl session  
 * Author:   Huang Yaofeng
 *  Dmail:   notdoevil@sin.cn 
 *   Date:   Tue Jan  7 11:55:54 CST 2014
 * *************************************************************/
#ifndef _SOCKSSL_H_
#define _SOCKSSL_H_
#include "../share/ssl_wrap.h"
typedef struct sockssl{
	int sockfd;
	SSL *ssl;
}SOCKSSL;
int sockssl_init(SOCKSSL *sockssl,int size);
int sockssl_search(SOCKSSL *sockssl,int size,int sockfd);
int sockssl_bind(SOCKSSL *sockssl,int size,SSL *ssl,int sockfd);
int sockssl_unbind(SOCKSSL *sockssl,int size,int sockfd);
#endif
