#include <stdio.h>

struct money2{
    int deposit;
    unsigned short numDeps;
    int withdraw;
    unsigned short numWithdraw;
};
struct money3{
    unsigned short numDeps;
    int deposit;
};

struct var1 {
    int a;
    int b;
};

struct var2 {
    int a;
    int b;
    unsigned short c;
};

struct var3 {
    unsigned short a;
    unsigned short b;
    int c;
};

struct var4 {
    int a;
    unsigned short b;
    unsigned short c;
};

struct var5 {
    unsigned short a;
    int b;
    unsigned short c;
    int d;
};

struct var6 {
    int a;
    unsigned short b;
    int c;
    unsigned short d;
};

int main(int argc, char *argv[]) {
    printf("%lu バイト\n", sizeof(struct var1));
    printf("%lu バイト\n", sizeof(struct var2));
    printf("%lu バイト\n", sizeof(struct var3));
    printf("%lu バイト\n", sizeof(struct var4));
    printf("%lu バイト\n", sizeof(struct var5));
    printf("%lu バイト\n", sizeof(struct var6));

    return 0;
}
