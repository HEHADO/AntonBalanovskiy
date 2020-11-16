#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>


#define BLACK "\033[0m"
#define RED "\033[1;31m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define WHITE "\033[1;37m"
#define GREEN "\033[1;32m"
#define C1 "exit"
#define C2 "cd"



void exitwitheof(char* v){
    printf("%sExit\n%s",BLUE,BLACK);
    if (v !=NULL) free(v); 
    exit (0);
}


char *readword (FILE *f,int* t){
    int n = 0;
    int size =0;
    int quotesisopen = 0;
    char ch;
    char* word = NULL;
    while (isspace(ch = getc(f)) );
    if (ch == EOF) exitwitheof(word);
    while ((!isspace(ch))||(quotesisopen != 0)||(ch == '"')){
        if (word == NULL) word = malloc (0*sizeof(char));
        if (size <= n){
            size = 2*size + 5;
            word = realloc(word,size);
        }
        if (ch == '"'){
            //printf("Kavichki\n");
            if (quotesisopen == 0) quotesisopen = 1; // СПРОСИТЬ
            else if (quotesisopen == 1) quotesisopen = 0;
        }else{
            word[n] = ch;
            n++;
            //printf("%d\n",n);
        }
        ch = getc(f);
        if (ch==EOF) exitwitheof(word);
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
    if (size <= n+10){
        size = 2*size +10;
        word = realloc(word,size);
    }
    word[n]='\0';
    return word;
}
/*
int size (char* ch){
    int i=0;
    while (ch[i] !='\0')
    {
        i++;
    }
    return i*sizeof(char);
}
*/

char** createargv(int n,char* ch){
    int size=2;
    int i=0;
    char** argv = malloc(2*sizeof(char*));
    //printf("%s\n",ch);
    //printf("%d\n",n);
    while (n == 0){
        if (size <= i+2){
            size = 2*size + 2;
            argv = realloc(argv,(size+2)*sizeof(char*));
        }
        //printf("%s\n",ch);
        argv[i] = ch;//?????????????????????????????????????????
        //free(ch);
        ch = readword(stdin, &n);
        i++;
        //printf("%d\n",i);
    }
    if (size <= i+2){
        size = 2*size + 2;
        argv = realloc(argv,(size+2)*sizeof(char*));
    }
    argv[i] = ch;
    argv[i+1] = NULL;
    //free(ch);
    return argv;
}


void runcommand(char** arg){
    int status = 0;
    if (fork()){
        wait(&status);
        if(status != 0) perror(arg[0]);
    } else{
        execvp(arg[0], arg);
        //execv(getenv("PATH"),arg);
        perror(arg[0]);
        exit(2);
    }
}


void myfree(char** v){
    int i=0;
    while (v[i] != NULL){
        free (v[i]);
        i++;
    }
    free (v); 
}


int main(){
    //char* ch1 = NULL;
    char* ch = NULL;
    char buff[PATH_MAX];
    int n = 0;
    char** argv = NULL;
    //int fd[2]; 
    for(;;){    l:
        printf("%s%s > %s",GREEN,getcwd(buff,PATH_MAX),YELLOW);
        ch = readword(stdin,&n);
        printf("%s",BLACK);
                                            //printf("%s\t%d\n",ch,n);
        if (!strcmp(ch,C1)) {
            free(ch);

            exit (0);
        }
        
        if (!strcmp(ch,C2)) {
            free(ch);
            if (n) ch = getenv("HOME");
            else ch = readword(stdin,&n);
            chdir(ch);
            if (!n) free(ch);
            goto l;
        }
        
        int i = 0;
        argv = createargv(n, ch);
        //printf("%s",argv[0]);
        /*while (argv[i] != NULL){
            printf("%s\n",argv[i]);
            i++;
        }*/
        runcommand (argv);
        myfree(argv);

        //free(ch);
        /*
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
                execvp(ch,argv);
                //perror(ch1);
                exit(2);
            default:
                break;
            }
            int status;
            wait (&status);

            close(fd[0]);
            close(fd[1]);
        }*/
    
    }
    
}