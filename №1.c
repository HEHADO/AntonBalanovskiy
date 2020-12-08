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
//(pr1|| pr2) | (pr3 ; pr4)
int execute(char* argv, int inp, int out){
    int status = 0;
    if (fork()){
        wait(&status);
        printf("%d",status);
        return status;
    } else{
        if (inp !=0){
            dup2(inp,0);
            close(inp);
        }
        if(out != 1){
            dup2(out,1);
            close(out);
        }
        execlp(argv,argv,NULL);
        //execv(getenv("PATH"),arg);
        perror(argv[0]);
        exit(1);
    }
}

int main(int argc, char const *argv[])
{
    char* pr1 = argv[1];
    char* pr2 = argv[2];
    char* pr3 = argv[3];
    char* pr4 = argv[3];
    int fd[2];
    pipe (fd);
    if (execute(pr1,0,fd[1])) execute(pr2,0,fd[1]);
    close(fd[1]);
    execute(pr3,fd[0],1);
    execute(pr4,fd[0],1);
    return 0;
}
