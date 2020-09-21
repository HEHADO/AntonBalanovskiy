#include <stdio.h>
#include <stdlib.h>

char*lst;
char*ls;
int main (){
    char ch;
    int i;
    int m = 0;
    int n = 1;
    int size = 1;
    lst = malloc(size);
    while ((ch = getchar()) != EOF){
        while ((ch != '\n')&&(ch != EOF)) {
            if (size == n){
                size = 2*size;
                lst = realloc(lst, size);
        }
            n++;
            lst[n]=ch;
            if ((ch != '\n')&&(ch != EOF))
                ch = getchar();
            }
        if (n > m) {
            m = n;
            ls = realloc(ls,size);
            for (i = 0; i < n; i++)
                ls[i]=lst[i];
            }
        free(lst);
        n = 0;
    }    
    for (;m != n;n++)
    putchar(ls[n]);
    free(ls);
    return 0;
}