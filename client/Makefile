obj_clnt=client.o 
obj_sock=../share/sock_wrap.o
obj_ssl=../share/ssl_wrap.o
obj_menu=menu.o
CC=gcc
CFLAGS= -lssl -lcrypto 
ssl_client:$(obj_clnt) $(obj_sock) $(obj_ssl) $(obj_menu)
	$(CC) -o $@ $(obj_clnt) $(obj_sock) $(obj_ssl) $(obj_menu) $(CFLAGS)
clean:
	rm -rf *.o
	rm -rf ssl_client
