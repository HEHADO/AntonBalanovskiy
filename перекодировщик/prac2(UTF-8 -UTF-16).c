#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

//addshit() 


unsigned short read(FILE* f1){
    unsigned char u8,t;
    unsigned short u16=0;
    int i;
    i = fread(&u8,sizeof u8,1,f1); 
    t = u8 & 0x80;
    if ((u8 & 0x80)==0){
        u16 = u8;
    }else if((u8 & 0xE0) == 0xC0){
        u16 = u8 & 0x1F;
        u16 <<= 6;
        i *= fread(&u8,sizeof u8,1,f1);
        if ((u8 & 0xC0) != 0x80) i=0;
        t = u8 & 0x3F;
        u16 |= t;
    }else if ((u8 & 0xF0) == 0xE0){
        u16 = u8 & 0xF;                     fprintf(stderr,"%x\t",u8);
        u16 <<= 6;
        i *= fread(&u8,sizeof u8,1,f1);    fprintf(stderr,"%x\t",u8);
        if ((u8 & 0xC0) != 0x80) i=0;   
        t = u8 & 0x3F;
        u16 |= t;
        u16 <<= 6;
        i *= fread(&u8,sizeof u8,1,f1);     fprintf(stderr,"%x\t",u8);
        if ((u8 & 0xC0) != 0x80) i=0;
        t = u8 & 0x3F;
        u16 |= t;
    }else fprintf(stderr,"somthing goes wrong");
    if (i == 0) fprintf(stderr,"somthing goes wrong");
    return u16;
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


void LE(FILE* f1,FILE* f2,unsigned  short u16){
    int p=1;
    while (!feof(f1)){
        u16 = reverse(u16);
        fprintf(f2,"%u",u16);
        u16 = read(f1);
    }
}


void BE(FILE* f1,FILE* f2,unsigned  short u16){
    int p=1;
    while (!feof(f1)){
        //reverse(u16);
        fprintf(f2,"%u",u16);
         u16 = read(f1);
    }
}


int main (int argc,char* argv[]){
    unsigned  short u16;
    unsigned char ch;
    FILE* f1=stdin;
    FILE* f2=stdout;

    if (argc > 1) f1 = fopen(argv[1],"r");
    if (argc > 2) f2 = fopen(argv[2],"w");

    if (!feof(f1)) u16 = read(f1);
    if (u16 == 0xFEFF) {if (!feof(f1)) BE(f1,f2,u16);}
    else LE (f1,f2,u16);
}