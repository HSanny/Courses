#include <stdio.h>
#include <stdlib.h>
int main(){
    int age;
    char * name ;
    name = (char *) malloc (100*sizeof(char));
    printf("Please input your name (end by enter): ");
    scanf("%s",name);
    printf("Please input your age: ");
    scanf("%d",&age);
    printf("input complete..\n");
	printf("Welcome: %s who is %d years old ..\n", name, age);
}
