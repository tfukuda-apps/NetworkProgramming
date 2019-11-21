#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>

#define BUF_SIZE 256
#define MONEY_DIGIT_SIZE 10

void DieWithError(char *);
int prepare_server_socket(int);
void commun(int);
void read_until_delim(int, char *, char, int);
int get_current_balance();
void set_current_balance(int);
void read_certain_bytes(int, void *, int);

struct money{
    int deposit;
    int withdraw;
};

int main(int argc, char *argv[]) {
	// クライアントの情報を格納するための変数
	struct sockaddr_in clientAddress;
	unsigned int szClientAddr;
	// クライアントとの通信用ソケット
	int cliSock;

	// 待ち受け用ソケット
	int servSock = prepare_server_socket(10001);
    
	listen(servSock, 5);

	// 接続要求が来たときの動作
	while (1) {
		szClientAddr = sizeof(clientAddress);
		// 接続要求受け入れ
		cliSock = accept(servSock, (struct sockaddr *)&clientAddress, &szClientAddr);
		
		// クライアントとの通信
		commun(cliSock);

		// 通信終了
		close(cliSock);
	}

	// 待ち受け用ソケットを閉じる
    close(servSock);
    return 0;
}

// エラーが生じた場合にメッセージを表示して終了
void DieWithError(char *errorMessage) {
    perror(errorMessage);
    exit(1);
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

// 区切り文字が出るまで、または最大文字数を受信するまで受信を繰り返す
// ただしmax_lengthはバッファサイズ。末尾に必ず'\0'をつけて返す。
void read_until_delim(int sock, char *buf, char delimiter, int max_length) {
	int len_r = 0;			// 受信文字数
	int index_letter = 0;	// 受信文字数の合計

	while (index_letter < max_length - 1) {
		// 1文字だけ受信
        if ((len_r = recv(sock, buf + index_letter, 1, 0)) <= 0) {
			// エラー（-1）やソケットが閉じていた場合（0）には何もせず終了
			printf("接続が切れました\n");
			return;
		}

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

// 本来はデータベースから現在の預金残高を取得
int get_current_balance(){
	return 1000000;
}

// 本来は預金残高をデータベースに登録
void set_current_balance(int new_balance) {
	return;
}

void commun(int sock) {
	int balance = get_current_balance(); 	// 預金残高
	struct money msgMoney;					// 受信した金額

	// 引き出し/預け入れの金額を受信
    read_certain_bytes(sock, &msgMoney, (int)sizeof(msgMoney));
	msgMoney.deposit = ntohl(msgMoney.deposit);
	msgMoney.withdraw = ntohl(msgMoney.withdraw);
	
	balance += msgMoney.deposit;
	balance -= msgMoney.withdraw;

	// データベースの預金残高を更新
	set_current_balance(balance);

	// ネットワークバイトオーダへ変換
	balance = htonl(balance);

	// クライアントへ残高を送信
	if(send(sock, &balance, sizeof(balance), 0) != sizeof(balance))
		DieWithError("send() sent a message of unexpected bytes");
}