obj_srv=server.o 
obj_ssl=../share/ssl_wrap.o
obj_sock=../share/sock_wrap.o
obj_account=../share/account.o
obj_order=../share/order.o
obj_epoll=myepoll.o
obj_parse=srv_parse.o
obj_all=$(obj_srv) $(obj_ssl) $(obj_sock) $(obj_epoll) $(obj_parse) $(obj_order) $(obj_account)
CC=gcc
CFLAGS= -lssl -lcrypto -lsqlite3 -g #-fno-stack-protector
ssl_server:$(obj_all)
	$(CC) -o $@ $(obj_all) $(CFLAGS)
clean:
	rm -rf *.o
	rm -rf ssl_server
