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

int execute(char* argv, int inp, int out){
    int status = 0;
    if (fork()){
        wait(&status);
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
    int fd[2];
    pipe (fd);
    int fd1[2];
    pipe (fd);
    if (execute(argv[1],0,fd[1])){
        execute(argv[1],fd[0],fd1[1]);
    }else{
        close(fd[0]);
        fd1[1]=fd[1];
        
    }
    return 0;
}