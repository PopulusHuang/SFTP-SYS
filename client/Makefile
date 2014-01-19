obj_clnt=client.o clnt_parse.o ../share/ui.o echo_mode.o menu.o console.o command.o
obj_sock=../share/sock_wrap.o
obj_ssl=../share/ssl_wrap.o
obj_sftpack=../share/sftpack.o
obj_account=../share/account.o
obj_file=../share/sftfile.o
obj_list=../share/list.o
obj_all=$(obj_clnt) $(obj_sock) $(obj_ssl) $(obj_sftpack) $(obj_account) $(obj_file) $(obj_list)
CC=gcc
CFLAGS= -lssl -lcrypto -lsqlite3 -lreadline -g
ssl_client:$(obj_all)
	$(CC) -o $@ $(obj_all) $(CFLAGS)
clean:
	rm -rf *.o
	rm -rf ssl_client
