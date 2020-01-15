#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "np_common.h"


#define BUF_SIZE 256
#define MONEY_DIGIT_SIZE 10

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
	// 子プロセスカウンタ
	int numChildProc = 0;

	// 待ち受け用ソケット
	int servSock = prepare_server_socket(10001);
    
	listen(servSock, 1);

	// 接続要求が来たときの動作

	// 接続要求が来たときの動作
	while (1) {
		szClientAddr = sizeof(clientAddress);
		// 接続要求受け入れ
		cliSock = accept(servSock, (struct sockaddr *)&clientAddress, &szClientAddr);
		
		int pid = fork();
		
		if (pid < 0) {
			// プロセスの作成失敗
			DieWithError("fork()に失敗");
		} else if (pid == 0) {
			// 子プロセス->通信して終了
			printf("子\n");
			close(servSock);
			commun(cliSock);
			close(cliSock);
			exit(0);
		} else{
			// 親プロセス->次の待受の処理に移動
			printf("親\n");
			close(cliSock);

			// 子プロセスカウンターをインクリメント
			numChildProc++;
			while (numChildProc) {
				// 子プロセスがある限りwaitpidする
				pid_t processID = waitpid((pid_t)-1, NULL, WNOHANG);
				if (processID < 0) {
					DieWithError("waitpid() failed");
				} else if (processID == 0) {
					// 新たに終了した子プロセスを確認できず
					break;
				} else {
					// 新たに終了した子プロセスを確認できたため、カウンタをデクリメント
					numChildProc--;
				}
			}
		}
	}

	// 待ち受け用ソケットを閉じる
    close(servSock);
    return 0;
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