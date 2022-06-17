#include<stdio.h>
void f(){
    printf("%s\n", "hello");
}

int i=1;

int main(){
    f();
    i=4;
    printf("%d\n", i);
    return 0;
}
/*
r--run
b--breakpoint
语法糖
n--next（不进函数体）
s--step
d--delete breakpoint
info
c--continue
bt--backtrace
k--kill
*/