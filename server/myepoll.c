#include "myepoll.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/epoll.h>
/* set the handle to nonblocking mode */
int setnonblocking(int sockfd)
{
	int opts;
	opts=fcntl(sockfd,F_GETFL);
	if(opts<0)
	{
	 perror("fcntl(sock,GETFL)");
	 return -1;
	}

	opts = opts|O_NONBLOCK;
	if(fcntl(sockfd,F_SETFL,opts)<0)
	{
	 perror("fcntl(sock,SETFL,opts)");
	 return -1;
	}    
}
int Event_add(int epfd,int listenfd,struct epoll_event *ev)
{
    ev->events = EPOLLIN | EPOLLET;
    ev->data.fd = listenfd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, ev) < 0)
    {
        fprintf(stderr, "epoll set insertion error: fd=%d\n", listenfd);
        return -1;
    }
    else
    {
        printf("client socket add to epoll succeed！\n");
    }
}
int Epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
{
	int n;
	n = epoll_wait(epfd,events,maxevents,timeout);
	if (n == -1)
	{
		perror("epoll_wait");
		return -1;
	}
	
}
