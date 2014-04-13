/*
 * ========================================================================
 *
 *       Filename:  sock_wrap.c
 *
 *    Description:  Wrap some common functions' error handling 
 *    				of socket communicate.  
 *
 *        Version:  1.0
 *           Date:  04/12/14 08:44:25 PM
 *       Compiler:  gcc-4.6.3
 *
 *         Author:  Populus Huang 
 *          Email:  thoughtsonlive@gmail.com
 *
 * ========================================================================
 */

#include "sock_wrap.h"
void perr_exit(const char *s)
{
		perror(s);
		exit(1);
}
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
		int n;
again:
		if ( (n = accept(fd, sa, salenptr)) < 0) {
				if ((errno == ECONNABORTED) || (errno == EINTR))
						goto again;
				else
						perr_exit("accept error");
		}
		return n;
}
void Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
		if (bind(fd, sa, salen) < 0)
				perr_exit("bind error");
}
int Connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
		if (connect(fd, sa, salen) < 0)
				perr_exit("connect error");
		return fd;
}
void Listen(int fd, int backlog)
{
		if (listen(fd, backlog) < 0)
				perr_exit("listen error");
}
int Socket(int family, int type, int protocol)
{
		int n;
		if ( (n = socket(family, type, protocol)) < 0)
				perr_exit("socket error");
		return n;
}
int Socketpair(int domain,int type,int protocol,int sv[2])
{
	int n;
	if((n = socketpair(domain,type,protocol,sv)) == -1)
	{
		perr_exit("socketpair error");
	}
	return n;
}
int Getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	int n;
	if((n = getsockname(sockfd,addr,addrlen)) == -1)
	{
		perr_exit("getsockname error");
	}
	return n;
}
ssize_t Read(int fd, void *ptr, size_t nbytes)
{
		ssize_t n;
again:
		if ( (n = read(fd, ptr, nbytes)) == -1) {
				if (errno == EINTR)
						goto again;
				else
						return -1;
		}
		return n;
}
ssize_t Write(int fd, const void *ptr, size_t nbytes)
{
		ssize_t n;
again:
		if ( (n = write(fd, ptr, nbytes)) == -1) {
				if (errno == EINTR)
						goto again;
				else
						return -1;
		}
		return n;
}
void Close(int fd)
{
		if (close(fd) == -1)
				perr_exit("close error");
}
ssize_t Readn(int fd, void *vptr, size_t n)
{
		size_t nleft;
		ssize_t nread;
		char
				*ptr;
		ptr = vptr;
		nleft = n;
		while (nleft > 0) {
				if ( (nread = read(fd, ptr, nleft)) < 0) {
						if (errno == EINTR)
								nread = 0;
						else
								return -1;
				} else if (nread == 0)
						break;
				nleft -= nread;
				ptr += nread;
		}
		return n - nleft;
}
ssize_t Writen(int fd, const void *vptr, size_t n)
{
		size_t nleft;
		ssize_t nwritten;
		const char *ptr;
		ptr = vptr;
		nleft = n;
		while (nleft > 0) {
				if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
						if (nwritten < 0 && errno == EINTR)
								nwritten = 0;
						else
								return -1;
				}
				nleft -= nwritten;
				ptr += nwritten;
		}
		return n;
}
int Recvfrom(int sockfd, void *buf, size_t len, int flags,
			struct sockaddr *src_addr, socklen_t *addrlen)
{
		int n;
		n = recvfrom(sockfd,buf,len,flags,src_addr,addrlen);
		if(n < 0)
		{
			perr_exit("recvfrom error");	
		}
		return n;
}
int Sendto(int sockfd, const void *buf, size_t len, int flags,
		    const struct sockaddr *dest_addr, socklen_t addrlen)
{
	int n;
	n = sendto(sockfd,buf,len,flags,dest_addr,addrlen);
	if(n < 0)
	{
		perr_exit("sendto error");
	}
	return n;
}
int Select(int nfds, 
		   fd_set *readfds, 
		   fd_set *writefds,
		   fd_set *exceptfds, 
		   struct timeval *timeout)
{
	int n;
	n = select(nfds,readfds,writefds,exceptfds,timeout);
	if(n < 0)
	{
		perr_exit("select error:");	
	}
	return n;
}
