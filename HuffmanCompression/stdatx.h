#ifndef STDATX_H
#define STDATX_H

#include<windows.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<QDebug>
#include<bitset>

#define MAXLZBUFFER     9
#define MAXLZSW         32768
#define MINLZLEN        3
#define BLOCKSIZE       16383

struct ComparisonNode
{
    std::bitset<1024>Code;
    unsigned int Len,C;
};

struct undecode_huffman
{
    int codelength[BLOCKSIZE];
    int inlen;
};

struct huffman_result
{
    ComparisonNode ComNodeOut[BLOCKSIZE];
    int outlen;
};

#endif // STDATX_H
