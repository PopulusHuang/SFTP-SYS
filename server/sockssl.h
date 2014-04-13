/*
 * ========================================================================
 *
 *       Filename:  sockssl.h
 *
 *    Description: 	this module bind a socket a with SSL session,
 * 					server will create a ssl session when a new 
 * 					connect comming,we use a sockssl to bind 
 * 					the socket and ssl 
 *
 *        Version:  1.0
 *           Date:  04/12/14 08:26:20 PM
 *       Compiler:  gcc-4.6.3
 *
 *         Author:  Populus Huang 
 *          Email:  thoughtsonlive@gmail.com
 *
 * ========================================================================
 */

#ifndef _SOCKSSL_H_
#define _SOCKSSL_H_
#include "ssl_wrap.h"
typedef struct sockssl{
	int sockfd;
	SSL *ssl;
}SOCKSSL;
int sockssl_init(SOCKSSL *sockssl,int size);
int sockssl_search(SOCKSSL *sockssl,int size,int sockfd);
int sockssl_bind(SOCKSSL *sockssl,int size,SSL *ssl,int sockfd);
int sockssl_unbind(SOCKSSL *sockssl,int size,int sockfd);
#endif
