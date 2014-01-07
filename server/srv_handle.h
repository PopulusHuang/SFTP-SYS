#ifndef _HANDLE_H_
#define _HANDLE_H_
#include "../share/ssl_wrap.h"
#include "../share/sock_wrap.h"
#include "../share/sftpack.h"
#include <sqlite3.h>
int handle_login(SSL *ssl,SFT_PACK *clnt_pack,sqlite3 *db);
void handle_logout(SSL *ssl,SFT_PACK *clnt_pack);
int handle_recv_file(SSL *ssl,SFT_PACK *clnt_pack);
int handle_register(SSL *ssl,SFT_PACK *clnt_pack,sqlite3 *db);
int handle_scan_dir(SSL *ssl,SFT_PACK *clnt_pack);
int handle_send_file(SSL *ssl,SFT_PACK *clnt_pack);
#endif
