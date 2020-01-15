#include "np_common.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>

// エラーが生じた場合にメッセージを表示して終了
void DieWithError(char *errorMessage) {
    perror(errorMessage);
    exit(1);
}

// 通信用ソケットの生成
int prepare_client_socket(char *ipaddr, int port) {
	// ソケット生成
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        DieWithError("socket() failed");
    
	// サーバの情報を設定
    struct sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_addr.s_addr = inet_addr(ipaddr);
    target.sin_port = htons(port);

	// サーバへ接続
    if (connect(sock, (struct sockaddr*)&target, sizeof(target)) < 0)
        DieWithError("connect() failed");

    return sock;
}

void my_scanf(char *buf, int num_letter) {
    // num_letter文字だけ入力させる（scanfの脆弱性対策）
    char format[20];
    sprintf(format, "%s%d%s", "%", num_letter, "s%*[^\n]");
    scanf(format, buf);
    getchar();
}


// 区切り文字が出るまで、または最大文字数を受信するまで受信を繰り返す
// ただしmax_lengthはバッファサイズ。末尾に必ず'\0'をつけて返す。
void read_until_delim(int sock, char *buf, char delimiter, int max_length) {
	int len_r = 0;			// 受信文字数
	int index_letter = 0;	// 受信文字数の合計

	while (index_letter < max_length - 1) {
		// 1文字だけ受信
        if ((len_r = recv(sock, buf + index_letter, 1, 0)) <= 0)
			DieWithError("recv() failed");

		// 受信した文字が区切り文字ならループを抜ける
		if (buf[index_letter] == delimiter)
			break;
		else
			index_letter++;
	}
	// nullを末尾に追加
	buf[index_letter] = '\0';
}

// 特定のバイト数だけ受信する
void read_certain_bytes(int sock, void *buf, int length) {
    int len_r = 0;
    int len_sum = 0;

    while (len_sum < length) {
        if ((len_r = recv(sock, buf + len_sum, length - len_sum, 0)) <= 0)
			DieWithError("recv() failed");
        len_sum += len_r;
    }
}

// 待ち受け用ソケットの生成
int prepare_server_socket(int port) {
	// ソケット生成
	int servSock = socket(PF_INET, SOCK_STREAM, 0);
    if (servSock < 0)
        DieWithError("socket() failed");
    
	// サーバの情報を設定
    struct sockaddr_in servAddress;
    servAddress.sin_family = AF_INET;
    servAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddress.sin_port = htons(port);

	// ソケットにサーバの情報を登録
	bind(servSock, (struct sockaddr*)&servAddress, sizeof(servAddress));

    return servSock;
}
