#ifndef _WRAP_H_
#define _WRAP_H_
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <errno.h>

extern void perr_exit(const char *s);
extern int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
extern void Bind(int fd, const struct sockaddr *sa, socklen_t salen);
extern int Connect(int fd, const struct sockaddr *sa, socklen_t salen);
extern void Listen(int fd, int backlog);
extern int Socket(int family, int type, int protocol);
extern int Socketpair(int domain,int type,int protocol,int sv[2]);
extern ssize_t Read(int fd, void *ptr, size_t nbytes);
extern ssize_t Write(int fd, const void *ptr, size_t nbytes);
extern void Close(int fd);
extern ssize_t Readn(int fd, void *vptr, size_t n);
extern ssize_t Writen(int fd, const void *vptr, size_t n);
extern ssize_t Readline(int fd, void *vptr, size_t maxlen);

extern int Getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
extern int Recvfrom(int sockfd, void *buf, size_t len, int flags,
                   struct sockaddr *src_addr, socklen_t *addrlen);
extern int Sendto(int sockfd, const void *buf, size_t len, int flags,
		          const struct sockaddr *dest_addr, socklen_t addrlen);
extern int Select(int nfds, 
		   fd_set *readfds, 
		   fd_set *writefds,
		   fd_set *exceptfds, 
		   struct timeval *timeout);
#endif
