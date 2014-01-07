#ifndef SRV_PARSE_H_
#define SRV_PARSE_H_
#include "../share/sftpack.h"
#include <sqlite3.h>
int parse_clnt(SSL *ssl,SFT_PACK *clnt_pack,sqlite3 *db);
#endif
