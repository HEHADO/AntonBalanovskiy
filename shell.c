#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#define BLACK "\033[0m"
#define RED "\033[1;31m"
#define YELLOW "\033[1;33m"
#define WHITE "\033[1;37m"
#define GREEN "\033[1;32m"
#define C1 "exit"
#define C2 "cd"


char *readword (FILE *f,int* t){
    int size,n = 0;
    int quotesisopen = 0;
    int ch;
    char *word = malloc(2*sizeof(char));

    while (isspace(ch = getc(f)) );
    while ((!isspace(ch))||(quotesisopen != 0)||(ch == '"')){
        if (size <= n){
            size = 2*size +1;
            word = realloc(word,size);
        }
        if (ch == '"'){
            //printf("Kavichki\n");
            if (quotesisopen == 0) quotesisopen = 1; //СПРОСИТЬ
            else if (quotesisopen == 1) quotesisopen = 0;
        }else{
            word[n] = ch;
            n++;
            //printf("%d\n",n);
        }
        ch = getc(f);
        if (ch == '\n'){

            *t = 1;
            goto m;
        }else
        {
            *t=0;
        }
        
    }
    m:
    if (quotesisopen != 0) fprintf(stderr,"%sI'll close it for u bro%s\n",RED,BLACK);
    if ((n==0)&&(ch!=EOF)&&(ispunct(ch))) word[n]=ch;
    if (size <= n+1){
        size = 2*size +1;
        word = realloc(word,size);
    }
    word[n+1]='\0';
    return word;
}

int size (char* ch){
    int i=0;
    while (ch[i] !='\0')
    {
        i++;
    }
    return i*sizeof(char);
}

int main(){
    char* ch,ch1=NULL;
    char buff[1024];
    int argc = 0;
    int n = 0;
    int size=1;
    char** argv = malloc(1*sizeof(char*));
    int fd[2];
    //while 
    for(;;){
        printf("%s%s >%s",GREEN,getcwd(buff,1024),YELLOW);
        ch = readword(stdin,&n);
        printf("%s",BLACK);
        //rintf("%s\t%d\n",ch,n);
        if (!strcmp(ch,C1)) {
            return 0;
            free(ch);
            exit;
        }

        if (!strcmp(ch,C2)) {
            free(ch);
            ch = readword(stdin,&n);
            chdir(ch);
        }

        //ch1 = malloc(sizeof(ch));
        ch1 = ch;
        while (!n){
            if (size <= argc+1){
                size = 2*size + 1;
                argv = realloc(argv,size);
            }
            *argv[argc] = ch;
            ch = readword(0,&n);
            argc++;
        }
        if (size <= argc+2){
            size = 2*size + 1;
            argv = realloc(argv,size);
        }
        *argv[argc++] = ch;
        *argv[argc+1] = NULL;


        if (pipe(fd)<0){
            perror("pipe");
            exit(1);
            pid_t p1 = fork();
            switch (p1){
            case -1:
                perror("pipe");
                exit(2);
            case 0:
                dup2(fd[0],0);
                dup2(fd[1],1);
                close(fd[1]);
                close(fd[0]);
                execvp(ch1,argv);
            default:
                break;
            }
            close(fd[0]);
            close(fd[1]);
        }
    }    
}