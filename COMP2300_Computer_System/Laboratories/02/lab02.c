#include <stdio.h>
#include <stdlib.h>
// prototype

void concat(char* str1, char* str2, char* output);

void concat(char* str1, char* str2, char* output){
    int i=0,j=0;
    printf("initialized..%c",*(str1+i));
    for (; *(str1+i) != '\00' ; i ++){
        *(output+i) = * (str1+i);
    }
    for (; *(str2+j) != '\00'; j++){
        *(output+i+j) = *(str2+j);
    }
    *(output+i+j+1) = '\00';
    return ;
}

int main(){

    char * str1, *str2;
    char * str3 ;
    str3 = (char *) malloc (100*sizeof(char));
    str2 = (char *) malloc (100*sizeof(char));
    str1 = (char *) malloc (100*sizeof(char));

    scanf("%s",str1);
    scanf("%s",str2);
    concat(str1,str2,str3);
    printf("Concated..\n");
    printf("\n result of concatination is : %s \n",str3);
    return 0;
}
