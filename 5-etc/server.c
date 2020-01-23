#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "np_common.h"
#include <string.h>

#define BUF_SIZE 256

void commun(int, int, int*);

int main(int argc, char *argv[]) {
	// クライアントの情報を格納するための変数
	struct sockaddr_in clientAddress;
	unsigned int szClientAddr;
	// クライアントとの通信用ソケット
	int cliSock;
	// 子プロセスカウンタ
	int numChildProc = 0;
	// pipe用ファイルディスクリプタ
	int fds[2];
	// an order of client
	int numClient = 0;

	// 実行時には待受ソケットに用いるポート番号を添えなければならない
	if (argc != 2)
		DieWithError("usage: ./server port");

	// 待ち受け用ソケット
	int servSock = prepare_server_socket(atoi(argv[1]));//10001);
    
	listen(servSock, 1);

	int ret_pipe = pipe(fds);
	if (ret_pipe < 0) {
		DieWithError("pipe() failed");
	}

	// 接続要求が来たときの動作
	while (1) {
		szClientAddr = sizeof(clientAddress);
		// 接続要求受け入れ
		cliSock = accept(servSock, (struct sockaddr *)&clientAddress, &szClientAddr);
		numClient++;
		int pid = fork();
		
		if (pid < 0) {
			// プロセスの作成失敗
			DieWithError("fork()に失敗");
		} else if (pid == 0) {
			// 子プロセス->通信して終了
			close(servSock);
			commun(cliSock, numClient, fds);
			close(cliSock);
			exit(0);
		} else{
			// 親プロセス->次の待受の処理に移動
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

void commun(int sock, int numClient, int *fds) {
	char buf[BUF_SIZE];
	int len_r = 0;
	char *openning_message = "こんにちは！何かメッセージを送ってね！";

	if (numClient == 1) {
		// clientは受信しようとしている
		len_r = read(fds[0], buf, BUF_SIZE);
		buf[len_r] = '\0';
		send(sock, buf, len_r, 0);
		len_r = recv(sock, buf, BUF_SIZE, 0);
	} else if (numClient == 2) {
		// clientは受信しようとしている
		send(sock, openning_message, strlen(openning_message),0);
		len_r = recv(sock, buf, BUF_SIZE, 0);
		write(fds[1], buf, len_r);
	}

	close(fds[0]);
	close(fds[1]);
}
