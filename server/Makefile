obj_srv=server.o 
obj_ssl=../../share/ssl_wrap.o
obj_sock=../../share/sock_wrap.o
obj_epoll=myepoll.o
CC=gcc
CFLAGS= -lssl -lcrypto -g
ssl_server:$(obj_srv) $(obj_ssl) $(obj_sock) $(obj_epoll)
	$(CC) -o $@ $(obj_srv) $(obj_ssl) $(obj_sock) $(obj_epoll) $(CFLAGS)
clean:
	rm -rf *.o
	rm -rf ssl_server
