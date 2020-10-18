#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


struct tree{
    struct tree *l;
    struct tree *r;
    int cnt;
    char *str;
};

char *readword (FILE *f){
    int size,n=0;
    int ch;
    char *word = malloc(2*sizeof(char));
    while (isspace(ch = getc(f)) || (iscntrl(ch)));
    while (isalnum(ch)){
        if (size <= n){
            size = 2*size +1;
            word = realloc(word,size);
        }
        word[n] = ch;
        n++;
        ch = getc(f);
    }
    if ((n==0)&&(ch!=EOF)&&(ispunct(ch))) word[n]=ch;
    //else ungetc(ch,f);
    word[n+1]='\0';//in hope of luck

    return word;
    //free(word);
}

void treedel(struct tree *t){
    if (t != NULL){
        treedel (t->l);
        treedel (t->r);
        free (t->str);
        free (t);
    }
}

int max (int a, int b){
    if (a>=b) return a;
    else return b;
}

int maxn (struct tree *t){
    if (t == NULL) return 0;
    else return max(max(maxn(t->l),maxn(t->r)),t->cnt);
}
  
int trcnt (struct tree *t){
    if (t!=NULL) return (t->cnt + trcnt(t->l) + trcnt(t->r));
    else return 0;
}

void treeFound(struct tree* t, int m, int a, FILE* f){
    if (t != NULL){
        if (t -> cnt == m){
            fprintf(f, "%s %d %f\n", t -> str, t -> cnt, (double)t -> cnt / a); // вывод слова и частоты
        }
        treeFound(t -> l, m, a, f);
        treeFound(t -> r, m, a, f);
    }
}

void printtree (struct tree *t,int temp, FILE *f){
    int max = maxn(t);
    //int count = trcnt(t);
    
    for (int i = max; i > 0; i--){
        treeFound(t, i, temp, f); 
    }
}

struct tree *addtotree (struct tree *t,char *s){
    int temp;
    if (t == NULL) {
        t = (struct tree *) malloc(sizeof(struct tree));
        t->str=s;
        //free(s);
        t->cnt=1;
        t->l = t->r = NULL;
    }
    else {
        temp = strcmp(t->str, s);
        if (temp == 0) {
            (t->cnt)++;
            free(s);
        }
        if (temp < 0) t->r = addtotree((t->r) ,s);
        if (temp > 0) t->l = addtotree((t->l) ,s);
    }
    
    return t;
}

int main(int argc,char *argv[]){
    struct tree *t=NULL;
    char *w=NULL;
    FILE *f1;
    FILE *f2;
    f1=stdin;
    f2=stdout;
    if (argc>2){
        if (!strcmp(argv[1],"-i")) f1 = fopen(argv[2],"r");
        if (!strcmp(argv[1],"-o")) f2 = fopen(argv[2],"w");
    }
    if (argc>4) if (!strcmp(argv[3],"-o")) f2 = fopen(argv[4],"w");
    while (!feof(f1)){
        w = readword(f1);
        if (w[0] != '\0') t = addtotree(t,w);
        else free(w);
    }
    printtree(t,trcnt (t),f2);
    treedel(t);
    fclose(f1);
    fclose(f2);
}