obj_srv=server.o srv_handle.o myepoll.o sockssl.o
obj_ssl=../share/ssl_wrap.o
obj_sock=../share/sock_wrap.o
obj_account=../share/account.o
obj_pack=../share/sftpack.o
obj_file=../share/sftfile.o
obj_list=../share/list.o  
obj_parse=srv_parse.o
obj_all=$(obj_srv) $(obj_ssl) $(obj_sock) $(obj_parse) $(obj_file) $(obj_account) $(obj_pack) $(obj_list)
CC=gcc
CFLAGS= -lssl -lcrypto -lsqlite3 -g -rdynamic#-fno-stack-protector
ssl_server:$(obj_all)
	$(CC) -o $@ $(obj_all) $(CFLAGS)
clean:
	rm -rf *.o
	rm -rf ssl_server
