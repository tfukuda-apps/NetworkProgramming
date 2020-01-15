#ifndef NP_COMMON
#define NP_COMMON

void DieWithError(char *);
int prepare_client_socket(char *, int);
void my_scanf(char *, int);
void read_until_delim(int, char *, char, int);
void read_certain_bytes(int, void *, int);

// 待ち受け用ソケットの生成
int prepare_server_socket(int);

#endif