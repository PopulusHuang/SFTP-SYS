obj_clnt=client.o clnt_parse.o menu.o echo_mode.o
obj_sock=../share/sock_wrap.o
obj_ssl=../share/ssl_wrap.o
obj_sftpack=../share/sftpack.o
obj_account=../share/account.o
obj_file=../share/sftfile.o
obj_all=$(obj_clnt) $(obj_sock) $(obj_ssl) $(obj_sftpack) $(obj_account) $(obj_file) 
CC=gcc
CFLAGS= -lssl -lcrypto -lsqlite3 -g
ssl_client:$(obj_all)
	$(CC) -o $@ $(obj_all) $(CFLAGS)
clean:
	rm -rf *.o
	rm -rf ssl_client
