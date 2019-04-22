#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

void commun(int sock) {
    char buf[256];
    int len_r;
    char *message = "Hello server!";
    send(sock, message, strlen(message), 0);
    len_r = recv(sock, buf, 256, 0);
    buf[len_r] = '\0';
    printf("%s", buf);
}

int main(int argc, char **argv) {
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_addr.s_addr = inet_addr("10.13.64.20");
    target.sin_port = htons(10001);

    connect(sock, (struct sockaddr *)&target, sizeof(target));

    commun(sock);

    close(sock);
    return 0;
}
