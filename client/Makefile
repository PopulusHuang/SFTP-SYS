obj_clnt=client.o 
obj_sock=../share/sock_wrap.o
obj_ssl=../share/ssl_wrap.o
obj_order=../share/order.o
obj_account=../share/account.o
obj_menu=menu.o
obj_parse=clnt_parse.o
obj_echo=echo_mode.o
obj_all=$(obj_clnt) $(obj_sock) $(obj_ssl) $(obj_menu)  $(obj_order) $(obj_account) $(obj_parse) $(obj_echo)
CC=gcc
CFLAGS= -lssl -lcrypto -lsqlite3 -g
ssl_client:$(obj_all)
	$(CC) -o $@ $(obj_all) $(CFLAGS)
clean:
	rm -rf *.o
	rm -rf ssl_client
