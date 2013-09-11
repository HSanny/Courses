#include <stdio.h>
static void test(){
    int i = 0;
    if(i>10){
        printf("done\n");
        exit(0);
    } else {
        i++;
        printf("%x\n", &i);
        test();
    }
}

int main() {
    test();
    return 0;
}
