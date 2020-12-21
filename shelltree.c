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
#define C3 "serialkiller"
#define SPEC(Char) ((Char == '|')||(Char == '>')||(Char == '<')||(Char == '&')||(Char == ';')||(Char == '(')||(Char == ')'))
#define YES 1
#define NO 0
#define BUFF 1024
#define PATH_MAX 4096


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

int proceses[BUFF];
int amp_mode=0;


void serialkiller();
int is_spesial_word(char* word);/*return true is it is spesial*/
void paralel(char** argv, int inp, int out);
void myfree(char** v,int i);
void runcommand(char** arg);
char* readword (FILE *f,int* t);
char** createargv(int n,char* ch);
int findsheet(char** a);
int count(char** v);
int status;
void treedel(runtree* t);
void exitwitheof(char* v){
    printf("%sExit\n%s",BLUE,BLACK);
    if (v !=NULL) free(v); 
    exit (0);
}


runtree* buildtree(char** str){
    runtree* tree = NULL;
    int i = 0;
        //int temp;
    if (tree == NULL) {
        tree = (runtree*) malloc(sizeof(runtree));
        tree->inp = -1;
        tree->out = -1;
        tree->key = NULL;
        tree->append_mode = 0;
        tree->argv = NULL;
        tree->l = tree->r = NULL;
    }
    while (str[i] != NULL){
        if (!strcmp(str[i],"(")){
            int count=1;
            int n=i;
            free(str[i]);
            str[i] = NULL;
            i++;
            while((str[i] !=NULL)){
                if(!strcmp(str[i],"(")) count++;
                else if(!strcmp(str[i],")"))count--;
                i++;
                if (!count) break;
            }
            if (count!=0) printf("%sTrY AgAiN%s\n",RED,CYAN);
            free(str[i]);
            str[i] = NULL;
            if(n){
                tree->key = strdup(str[n-1]);
                free(str[n-1]);
                str[n-1] = NULL;                    
                tree->l = buildtree(str);
                tree->r = buildtree(str+n+1);
            }else{
                if (str[i] != NULL) tree->key = strdup(str[i]);
                else(tree->key=NULL);
                free(str[i]);
                str[i] = NULL;
                if(tree->key == NULL) return buildtree(str+1);
                tree->l = buildtree(str+1);
                tree->r = buildtree(str+i+1);
                return tree;
            }
        }
        i++;
    }
    i = 0;
    while (str[i] != NULL){
        if (!strcmp(str[i],";")){ 
            tree->key = strdup(str[i]);
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
            tree->key = strdup(str[i]);
            free(str[i]);
            str[i] = NULL;
            tree->l = buildtree(str);
            tree->r = buildtree(str+i+1);
            return tree;
        }
        if (!strcmp(str[i],"||")){ 
            tree->key = strdup(str[i]);
            free(str[i]);
            str[i] = NULL;
            tree->l = buildtree(str);
            tree->r = buildtree(str+i+1);
            return tree;
        }
        if (!strcmp(str[i],"&&")){ 
            tree->key = strdup(str[i]);
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
        if (is_spesial_word(str[i])){
            //FILE*f;
            if(!strcmp(str[i],">")) tree->out = open(str[i+1],O_WRONLY | O_CREAT | O_TRUNC, 0777);
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
    if (fork()){
        wait(&status);
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
        perror(argv[0]);
        exit(2);
    }
}

void runprogram(runtree* t, int l){
        if(t->key != NULL){
        if(!strcmp(t->key,";")){
            if((t->inp != -1)&&(t->l->inp == -1)&&(t->l != NULL)) t->l->inp = t->inp;
            if((t->inp != -1)&&(t->r->inp == -1)&&(t->r != NULL)) t->r->inp = t->inp;
            if((t->out != -1)&&(t->l->out == -1)) t->l->out = t->out;
            if((t->out != -1)&&(t->r->out == -1)) t->r->out = t->out;
            runprogram(t->l,0);
            runprogram(t->r,1);
        }else if(!strcmp(t->key,"|")){
            int fd[2];
            pipe(fd);
            if ((t->inp == -1)&&(t->out == -1)){
                pipe(fd);
                if(t->l->inp == -1) t->l->inp = 0;
                t->l->out = fd[1];
                if(t->r->out == -1) t->r->out = 1;
                t->r->inp = fd[0];
            }else{
                if(t->inp != -1)t->l->inp = t->inp;
                else t->inp =0;
                t->l->out = fd[1];
                if(t->out != -1) t->r->out = t->out;
                else t->r->out = 1;
                t->r->inp = fd[0];
            }
            runprogram(t->l,0);
            close(fd[1]);
            runprogram(t->r,l);
            close(fd[0]);
        }else if(!strcmp(t->key,"||")){
            if((t->inp != -1)&&(t->l->inp == -1)&&(t->l != NULL)) t->l->inp = t->inp;
            if((t->inp != -1)&&(t->r->inp == -1)&&(t->r != NULL)) t->r->inp = t->inp;
            if((t->out != -1)&&(t->l->out == -1)) t->l->out = t->out;
            if((t->out != -1)&&(t->r->out == -1)) t->r->out = t->out;
            runprogram(t->l,0);
            if(status)runprogram(t->r,0);
        }else if(!strcmp(t->key,"&&")){
            if((t->inp != -1)&&(t->l->inp == -1)&&(t->l != NULL)) t->l->inp = t->inp;
            if((t->inp != -1)&&(t->r->inp == -1)&&(t->r != NULL)) t->r->inp = t->inp;
            if((t->out != -1)&&(t->l->out == -1)) t->l->out = t->out;
            if((t->out != -1)&&(t->r->out == -1)) t->r->out = t->out;
            runprogram(t->l,0);
            if(!status)runprogram(t->r,0);
        } else {
            if(t->inp == -1) t->inp = 0;
            if(t->out == -1) t->out = 1;
            if(amp_mode&&l) paralel(t->argv, t->inp, t->out);
            else execute(t->argv, t->inp, t->out);
            //if (t->inp != 0) close(t->inp);
            //if (t->out != 1) close(t->out);
            //myfree(t->argv);
            //free(t);
        }
    }
}
void paralel(char** argv, int inp, int out){
    int i = 0;
    while(proceses[i] != 0) i++;
    int pid =fork();
    if (!pid){
        if (inp !=0){
            dup2(inp,0);
            close(inp);
        }
        if(out != 1){
            dup2(out,1);
            close(out);
        }
        execvp(argv[0], argv);
    }else{
        proceses[i]=pid;
    }
}

void print_pid(){
    for (int i = 0; i < BUFF; i++) if (proceses[i]) if(waitpid(proceses[i], NULL, WNOHANG) != -1) {
        printf("%s%d IS Dead%s\n",CYAN,proceses[i],BLACK);
        proceses[i]=0;
    }
}

void sighandler(){
    char* argv[2048];
    argv[0]=(char*)&"killall";
    argv[1]=(char*)&"-eIq";
    printf("\n");
    argv[2]=NULL;
    execute(argv,0,1);
}

int main(){
    char* ch = NULL;
    char buff[PATH_MAX];
    int n = 0;
    runtree* tree;
    char** argv = NULL;
    for (int i = 0; i < BUFF; i++) proceses[i]=0;
    for(;;){    //l:
        n =0;
        amp_mode =  0;
        signal(SIGINT,sighandler);
        signal(SIGCHLD,print_pid);
        printf("%s%s > %s",GREEN,getcwd(buff,PATH_MAX),YELLOW);
        ch = readword(stdin,&n);
        printf("%s",BLACK);
                                            //printf("%s\t%d\n",ch,n);
        if (!strcmp(ch,C1)) {
            free(ch);
            serialkiller();
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
        if(!strcmp(ch,C3)){
            free(ch);
            serialkiller();
            continue;
        }
        argv = createargv(n, ch);
        int i = count(argv);
        if (findsheet(argv)){
            myfree(argv,i);
            printf("%sTrY AgAiN%s\n",RED,CYAN);
            continue;
        }
        //printf("%s",argv[0]);
        tree = buildtree(argv);
        //printtree(tree);
        runprogram (tree,1);
        myfree(argv,i);
        treedel(tree);
    }
}


int findsheet(char** argv){
    int parentheses_open,parentheses_close,i;
    i = parentheses_open = parentheses_close = 0;
    while(argv[i] != NULL){
        if (!strcmp(argv[i], "&")){
            if(argv[i+1] != NULL){
                return 1;
            }else {
                amp_mode = 1;
                free(argv[i]);
                argv[i] = NULL;
                return parentheses_open-parentheses_close;
            }
        } 
        if (!strcmp(argv[i], "(")) parentheses_open++;
        if (!strcmp(argv[i], ")")) parentheses_close++;
        i++;
    }
    return (parentheses_open-parentheses_close);
}

int is_spesial_word(char* word){
    /*return true is it is spesial*/
    int i;
    i = !(strcmp(word,"|")&&strcmp(word,">")&&strcmp(word,">>")&&strcmp(word,"<")&&strcmp(word,";")&&strcmp(word,"&&")&&strcmp(word,"||"));
    return i;
}

void treedel(runtree* t){
    if(t != NULL){
        treedel(t->l);
        treedel(t->r);
        if(is_spesial_word(t->key)) free(t->key);
        //if (t->inp != 0) close(t->inp);
        //if (t->out != 1) close(t->out);
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
    while (isspace(ch = getc(f)));
    if (ch == EOF) exitwitheof(word);
    while (((!isspace(ch))||(quotesisopen != 0)||(ch == '"'))&&(!*t)){
        if (word == NULL) word = malloc (0*sizeof(char));   
        if (size <= n){
            size = 2*size + 5;
            word = realloc(word,size*sizeof(char));
        }
        if (ch == '"'){
            if (quotesisopen == 0) quotesisopen = 1; 
            else if (quotesisopen == 1) quotesisopen = 0;
        }else{
            if(SPEC(ch)&&(!quotesisopen)) break;
            word[n] = ch;
            n++;
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
        if((ch = getchar())== '\n') *t=1;
        else ungetc(ch,stdin);
    }
    if ((n-1 >= 0)&&(word[n-1] == '>')&&(t)){
        ch = getc(f);
        if (ch == '>'){
            word[n]=ch;
            n++;
        }
        else ungetc(ch,f);
    }
        if ((n-1 >= 0)&&(word[n-1] == '&')&&(t)){
        ch = getc(f);
        if (ch == '&'){
            word[n]=ch;
            n++;
        }
        else ungetc(ch,f);
    }
        if ((n-1 >= 0)&&(word[n-1] == '|')&&(t)){
        ch = getc(f);
        if (ch == '|'){
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
    while (n == 0){
        if (size <= i+2){
            size = 2*size + 2;
            argv = realloc(argv,(size+2)*sizeof(char*));
        }
        argv[i] = ch;
        ch = readword(stdin, &n);
        i++;
    }
    if (size <= i+2){
        size = 2*size + 2;
        argv = realloc(argv,(size+2)*sizeof(char*));
    }
    argv[i] = ch;
    argv[i+1] = NULL;
    return argv;
}

void serialkiller(){
    for (int i = 0; i < BUFF; i++)
    {
        if(proceses[i] != 0) kill(proceses[i],2);
    }
}