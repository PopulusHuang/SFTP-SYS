#ifndef _MYEPOLL_H_
#define _MYEPOLL_H_
#include <fcntl.h>
#include <sys/epoll.h>
int setnonblocking(int sockfd);
int Event_add(int epfd,int listenfd,struct epoll_event *ev);
int Epoll_wait(int epfd, 
			   struct epoll_event *events, 
			   int maxevents, 
			   int timeout);
#endif
