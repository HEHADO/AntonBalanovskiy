#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


unsigned char split(unsigned short i, int r,int t){
    //char u;
    i = i << 16-t;
    i = i >> 16-t+r;
    return i;
}


unsigned short reverse(unsigned short u16){
    unsigned short t, new16=0;
    int i = 0;
    while (i != 16){
        t = u16 & 0xFF;
        new16 <<= 8;
        new16 += t;
        u16 >>= 8;
        i += 8;
    }
    return new16;
}


void BIGEND(FILE* f1,FILE* f2, unsigned short u16){
    unsigned char u8;
    //unsigned short t;
    int p = 1;
    while ((p != EOF)&&(p > 0)&&(u16 != EOF)){
        //fprintf(stderr,"BIGEND\t");
        u16 = reverse (u16);
        //fprintf(stderr,"%x\t",u16);
        if (u16 <= 0x7f){
                                        //fprintf(stderr,"!\t");
            u8 = u16;
            fprintf(f2,"%c",u8);            
        } else if (u16 <= 0x7ff){
                                        //fprintf(stderr,"!!\t");
            u8 = split(u16,6,11);
            u8 = u8|0xC0;
            fprintf(f2,"%c",u8);
            u8 = split(u16,0,6);
            u8 = u8|0x80;
            fprintf(f2,"%c",u8);
        } else if (u16 <= 0xFFFF){      //fprintf(stderr,"!!!\t");
            u8 = split(u16,12,16);
            u8 = u8|0xE0;               //fprintf(stderr,"%x\t",u8);
            fprintf(f2,"%c",u8);
            u8 = split(u16,6,12);
            u8 = u8|0x80;               //fprintf(stderr,"%x\t",u8);
            fprintf(f2,"%c",u8);
            u8 = split(u16,0,6);
            u8 = u8|0x80;               //fprintf(stderr,"%x\t",u8);
            fprintf(f2,"%c",u8);
        }
        p = fread(&u16,sizeof u16,1,f1);
    }
}


void LITTLEEND(FILE* f1,FILE* f2, unsigned short u16){
    int p = 1;
    unsigned char u8;
    while ((p != EOF)&&(p > 0)&&(u16 != EOF)){
        //fprintf(stderr,"LiTTLEEND\t");
        //u16 = reverse (u16);
        if (u16 <= 0x7f){
            u8 = u16;
            fprintf(f2,"%c",u8);            
        } else if (u16 <= 0x7ff){
            u8 = split(u16,6,11);
            u8 = u8|0xC0;
            fprintf(f2,"%c",u8);
            u8 = split(u16,0,6);
            u8 = u8|0x80;
            fprintf(f2,"%c",u8);
        } else if (u16 <= 0xFFFF){
            u8 = split(u16,12,16);
            u8 = u8|0xE0;
            fprintf(f2,"%x",u8);
            u8 = split(u16,6,12);
            u8 = u8|0x80;
            fprintf(f2,"%x",u8);
            u8 = split(u16,0,6);
            u8 = u8|0x80;
            fprintf(f2,"%x",u8);
        }
        p = fread(&u16,sizeof u16,1,f1);
    }
    
}


int main (int argc,char* argv[]){
    unsigned  short u16;
    unsigned char ch;
    FILE* f1=stdin;
    FILE* f2=stdout;

    if (argc > 1) f1 = fopen(argv[1],"r");
    if (argc > 2) f2 = fopen(argv[2],"w");
    if (fread(&u16, sizeof u16, 1, f1) > 0){
        u16 = reverse (u16);
        if (u16 == 0xFEFF){
            if (fread(&u16, sizeof u16, 1, f1) > 0) BIGEND(f1,f2,u16);
        }
        else if (u16 == 0xFFFE){
            if (fread(&u16, sizeof u16, 1, f1) > 0) LITTLEEND(f1,f2,u16);
        }
        else if ((u16 != 0xFFFE)&&(u16 != 0xFEFF)){
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


