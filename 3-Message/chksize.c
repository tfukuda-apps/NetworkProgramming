#include <stdio.h>

struct money2{
    int deposit;
    unsigned short numDeps;
    int withdraw;
    unsigned short numWithdraw;
};
struct money3{
    int deposit;
    int withdraw;
    unsigned short numDeps;
    unsigned short numWithdraw;
};

int main(int argc, char *argv[]) {
    printf("%lu バイト\n", sizeof(int));
    printf("%lu バイト\n", sizeof(double));
    printf("%lu バイト\n", sizeof(struct money2));
    printf("%lu バイト\n", sizeof(struct money3));

    return 0;
}
