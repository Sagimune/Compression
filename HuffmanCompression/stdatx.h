#ifndef STDATX_H
#define STDATX_H

#include<windows.h>
#include<string.h>
#include<qDebug>
#include<stdio.h>
#include<stdlib.h>

#define MAXLZBUFFER     9
#define MAXLZSW         32768
#define MINLZLEN        3
#define BLOCKSIZE       16383

struct huffman_result
{
    char huffman_code[BLOCKSIZE];
    int code_length[BLOCKSIZE];
    int outlen;
};

#endif // STDATX_H
