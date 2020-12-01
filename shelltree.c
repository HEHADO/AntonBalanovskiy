#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits.h>


#define BLACK "\033[0m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define PURPLE "\033[0;35m"
#define CYAN "\033[0;36m"
#define WHITE "\033[1;37m"
#define C1 "exit"
#define C2 "cd"
#define SPEC(Char) ((Char == '|')||(Char == '>')||(Char == '<'))
#define YES 1
#define NO 0



int is_spesial_word(char* word);/*return true is it is spesial*/

void myfree(char** v,int i);

void runcommand(char** arg);

char* readword (FILE *f,int* t);

char** createargv(int n,char* ch);

typedef struct tr
{
    struct tr* l;
    struct tr* r;
    int inp;
    int out;
    char* key;
    int append_mode;
    char** argv;
} runtree;

int count(char**);

void treedel(runtree* t);

void exitwitheof(char* v){
    printf("%sExit\n%s",BLUE,BLACK);
    if (v !=NULL) free(v); 
    exit (0);
}


runtree* buildtree(char** str){
    runtree* tree = NULL;
    int i = 0;
        int temp;
    if (tree == NULL) {
        tree = (runtree*) malloc(sizeof(runtree));
        tree->inp = -1;
        tree->out = -1;
        tree->key = NULL;
        tree->append_mode = 0;
        tree->argv = NULL;
        tree->l = tree->r = NULL;
        //char* kword = NULL;
        //char** key = NULL;
    }
    while (str[i] != NULL){
        if (!strcmp(str[i],";")){ 
            tree->key = ";";
            free(str[i]);
            str[i] = NULL;
            tree->l = buildtree(str);
            tree->r = buildtree(str+i+1);
            return tree;
        }
        i++;
    }
    i = 0;
    while (str[i] != NULL){
        if (!strcmp(str[i],"|")){ 
            tree->key = "|";
            free(str[i]);
            str[i] = NULL;
            tree->l = buildtree(str);
            tree->r = buildtree(str+i+1);
            i+1;
            return tree;
        }
        i++;
    }
    i = 0;
    while (str[i] != NULL){
        if (is_spesial_word(str[i])){
            //FILE*f;
            if(!strcmp(str[i],">")) tree->out = tree->out = open(str[i+1],O_WRONLY | O_CREAT | O_TRUNC, 0777);
            if(!strcmp(str[i],"<")) tree->inp = open(str[i+1],O_RDONLY | O_CREAT , 0777);
            if(!strcmp(str[i],">>"))tree->out = open(str[i+1],O_WRONLY | O_CREAT| O_APPEND , 0777);
            while (str[i] != NULL){
                free(str[i]);
                str[i] = NULL;
                i++;
            }
            break;
        }
    i++;
    }
    i++;
    tree->key = *str;
    tree->argv = str;
    return tree;
}


void printtree (runtree *t){
    if (t != NULL){
        int i = 0;
        if (!is_spesial_word(t->key)){
            while (t->argv[i] != NULL){
                printf("%s\n",t->argv[i]);
                i++;
            }
        }
        printtree(t->l);
        printtree(t->r);
    }
    return;
}


void execute(char** argv, int inp, int out){
    int status = 0;
    if (fork()){
        wait(&status);
        if(status != 0) perror(argv[0]);
    } else{
        if (inp !=0){
            dup2(inp,0);
            close(inp);
        }
        if(out != 1){
            dup2(out,1);
            close(out);
        }
        execvp(argv[0], argv);
        //execv(getenv("PATH"),arg);
        perror(argv[0]);
        exit(2);
    }
}


void runprogram(runtree* t){
    if(!strcmp(t->key,"|")){
        int fd[2];
        pipe(fd);
        if (t->inp == -1){
            pipe(fd);
            if(t->l->inp == -1) t->l->inp = 0;
            t->l->out = fd[1];
            if(t->r->out == -1) t->r->out = 1;
            t->r->inp = fd[0];
        }else{
            t->l->inp = t->inp;
            t->l->out = fd[1];
            //t->r->out = t->out;
            t->r->inp = fd[0];
        }
        runprogram(t->l);
        close(fd[1]);
        runprogram(t->r);
        close(fd[0]);
        //free(t);
    }else {
        if(t->inp == -1) t->inp = 0;
        if(t->out == -1) t->out = 1;
        execute(t->argv, t->inp, t->out);
        if (t->inp != 0) close(t->inp);
        if (t->out != 1) close(t->out);
        //myfree(t->argv);
        //free(t);
    }
}

int main(){
    char* ch = NULL;
    char buff[PATH_MAX];
    int n = 0;
    runtree* tree;
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
            continue;
        }
        
        argv = createargv(n, ch);
        int i = count(argv);
        //printf("%s",argv[0]);
        tree = buildtree(argv);
        printtree(tree);
        runprogram (tree);
        myfree(argv,i);
        treedel(tree);
    }
}


int is_spesial_word(char* word){
    /*return true is it is spesial*/
    int i;
    i = !(strcmp(word,"|")&&strcmp(word,">")&&strcmp(word,">>")&&strcmp(word,"<"));
    return i;
}

void treedel(runtree* t){
    if(t != NULL){
        treedel(t->l);
        treedel(t->r);
        //if(is_spesial_word(t->key)) free(t->key);
        free(t);
        t = NULL;
    }
    return;
}


void myfree(char** v,int i){
    while (i >= 0){
        if(v[i] != NULL) free (v[i]);
        i--;
    }
    free (v); 
}


int count(char** v){
    int i=0;
    while (v[i] != NULL) i++;
    return i;
}


char* readword (FILE *f,int* t){
    int n = 0;
    int size =0;
    int quotesisopen = 0;
    char ch;
    char* word = NULL;
    while (isspace(ch = getc(f)) );
    if (ch == EOF) exitwitheof(word);
    while (((!isspace(ch))||(quotesisopen != 0)||(ch == '"'))&&(!SPEC(ch))){
        if (word == NULL) word = malloc (0*sizeof(char));
        if (size <= n){
            size = 2*size + 5;
            word = realloc(word,size*sizeof(char));
        }
        if (ch == '"'){
            //printf("Kavichki\n");
            if (quotesisopen == 0) quotesisopen = 1; 
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
    if (size <= n+10){
        size = 2*size +10;
        word = realloc(word,size*sizeof(char));
    }
    if (SPEC(ch)&&(n != 0)) ungetc(ch,f);
    if ((n == 0)&&(ch != EOF)&&(ispunct(ch))){
        word[n]=ch;
        n++;
    }
    if ((n-1 >= 0)&&(word[n-1] == '>')){
        ch = getc(f);
        if (ch == '>'){
            word[n]=ch;
            n++;
        }
        else ungetc(ch,f);
    }
    word[n]='\0';
    return word;
}


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
    return argv;
} 


/*
char** modded_createargv(int n,char* ch,char** save){
    int size=2;
    int i=0;
    char** argv = malloc(2*sizeof(char*));
    //printf("%s\n",ch);
    //printf("%d\n",n);
    while ((n == 0)&&(!is_spesial_word(ch))){
        if (size <= i+2){
            size = 2*size + 2;
            argv = realloc(argv,(size+2)*sizeof(char*));
        }
        //printf("%s\n",ch);
        argv[i] = ch;
        //free(ch);
        ch = readword(stdin, &n);
        i++;
        //printf("%d\n",i);
    }
    if (size <= i+2){
        size = 2*size + 2;
        argv = realloc(argv,(size+2)*sizeof(char*));
    }
    if(is_spesial_word(ch)) {
        *save = ch;
        i--;
    }
    else argv[i] = ch;
    argv[i+1] = NULL;
    //free(ch);
    return argv;
}


/*
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
*/
