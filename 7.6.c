#include<stdio.h>
#include<stdlib.h>
#include<string.h>

char* filecmp(FILE *, FILE *);

int main() {
    FILE *file1;
    FILE *file2;
    
    file1 = fopen(fl1.txt, "r");
    file2 = fopen(fl2.txt, "r");
        printf("%s\n", cmp(file1, file2));
        fclose(file1);
        fclose(file2);
    return 0;

}


char* cmp(FILE *f1, FILE *f2) {
    char *s1 = NULL;
    char *s2 = NULL;
    int t1, t2;
    int n = 0;
    int size = 0;
    while ((t1 = fgetc(f1) ) != EOF && ( t2 = fgetc(f2) ) != EOF) {
        if (size >= n) {
            size = size*2 + 1;
			s1 = realloc(s1, size);
            s2 = realloc(s2, size);
			if (s1 == NULL || s2 == NULL) {
				printf("Memory error");
				exit;
			}
        }
        if ((t1 == '\n') || (t2 == '\n')) {
            for (int i = 0; i < n; i++) {
                if (s1[i] != s2[i]) {
                    if (t1 == '\n') {
                        return s1;
                    } else return s2;
                }
            }
            n = 0;
            free(s1);
            free(s2);
            s1 = s2 = NULL;
        } else {
            s1[n] = t1;
            s2[n] = t2;
            n++;
        }
    }
    
    if ((t1 == EOF) && (t2 != EOF)) {
        return s1; 
    }
    if ((t1 != EOF) && (t2 == EOF)){
        return s2;
    }
    return "equal";
}
