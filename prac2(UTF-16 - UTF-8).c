#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


char split(unsigned short i, int r,int t){
    char u;
    i = i << 16-t;
    i = i >> 16-t+r;
    return i;
}


void BIGEND(FILE* f1,FILE* f2, unsigned short u16){
    char u8;
    //unsigned short t;
    int p = 1;
    while (p > 0){
        if (u16 <= 0x7f){
            u8 = u16;
            fprintf(f1,"%c",u8);            
        } else if (u16 <= 0x7ff){
            u8 = split(u16,6,11);
            u8 = u8|0xC0;
            fprintf(f1,"%c",u8);
            u8 = split(u16,0,6);
            u8 = u8|0x80;
            fprintf(f1,"%c",u8);
        } else if (u16 <= 0xFFFF){
            u8 = split(u16,12,16);
            u8 = u8|0xE0;
            fprintf(f1,"%c",u8);
            u8 = split(u16,6,12);
            u8 = u8|0x80;
            fprintf(f1,"%c",u8);
            u8 = split(u16,0,6);
            u8 = u8|0x80;
            fprintf(f1,"%c",u8);
        }
        p = fread(&u16,sizeof u16,1,f1);
    }
}


unsigned short reverse(unsigned short u16){
    unsigned short t, new16=0;
    int i = 0;
    while (i != 4){
        t = u16 & 0xF;
        new16 += t;
        new16 <<= 4;
        u16 >>= 4;
    }
    return new16;
}


void LITTLEEND(FILE* f1,FILE* f2, unsigned short u16){
    int p fdssd= 1;
    char u8;
    while (p > 0){
        u16 = reverse (u16);
        if (u16 <= 0x7f){
            u8 = u16;
            fprintf(f1,"%c",u8);            
        } else if (u16 <= 0x7ff){
            u8 = split(u16,6,11);
            u8 = u8|0xC0;
            fprintf(f1,"%c",u8);
            u8 = split(u16,0,6);
            u8 = u8|0x80;
            fprintf(f1,"%c",u8);
        } else if (u16 <= 0xFFFF){
            u8 = split(u16,12,16);
            u8 = u8|0xE0;
            fprintf(f1,"%c",u8);
            u8 = split(u16,6,12);
            u8 = u8|0x80;
            fprintf(f1,"%c",u8);
            u8 = split(u16,0,6);
            u8 = u8|0x80;
            fprintf(f1,"%c",u8);
        }
        p = fread(&u16,sizeof u16,1,f1);
    }
    
}


int main (int argc,char* argv[]){
    unsigned short u16;
    char ch;
    FILE* f1=stdin;
    FILE* f2=stdout;

    if (argc > 1) f1 = fopen(argv[1],"r");
    if (argc > 2) f2 = fopen(argv[2],"w");
    if (fread(&u16, sizeof u16, 1, f1) > 0){
        if (u16 == 0xFEFF){
            if (fread(&u16, sizeof u16, 1, f1) > 0) BIGEND(f1,f2,u16);
        }
        if (u16 == 0xFFFE){
            if (fread(&u16, sizeof u16, 1, f1) > 0) LITTLEEND(f1,f2,u16);
        }
        if ((u16 != 0xFFFE)&&(u16 != 0xFEFF)){
            fprintf(stderr,"There is no BOM\n");
            /*ch = u16;
            ungetc(ch);   
            ch = u16>>8;
            ungetc(ch);*/
            LITTLEEND(f1,f2,u16);
        }
    }

    fclose(f1);
    fclose(f2);
}