#include <stdio.h>
#include <unistd.h>
#define F "fish"
int main(int argc, char const *argv[]) {
    execlp(F, F,NULL);
}