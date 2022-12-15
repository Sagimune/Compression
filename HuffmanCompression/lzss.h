#ifndef LZSS_H
#define LZSS_H

#include"stdatx.h"

/*
 * https://pkware.cachefly.net/webdocs/APPNOTE/APPNOTE-6.3.9.TXT
Length Codes
------------
    Extra             Extra              Extra              Extra
Code Bits Length  Code Bits Lengths  Code Bits Lengths  Code Bits Length(s)
---- ---- ------  ---- ---- -------  ---- ---- -------  ---- ---- ---------
257   0     3     265   1   11,12    273   3   35-42    281   5  131-162
258   0     4     266   1   13,14    274   3   43-50    282   5  163-194
259   0     5     267   1   15,16    275   3   51-58    283   5  195-226
260   0     6     268   1   17,18    276   3   59-66    284   5  227-257
261   0     7     269   2   19-22    277   4   67-82    285   0    258
262   0     8     270   2   23-26    278   4   83-98
263   0     9     271   2   27-30    279   4   99-114
264   0    10     272   2   31-34    280   4  115-130

Distance Codes
--------------
    Extra           Extra             Extra               Extra
Code Bits Dist  Code Bits  Dist   Code Bits Distance  Code Bits Distance
---- ---- ----  ---- ---- ------  ---- ---- --------  ---- ---- --------
0   0    1      8   3   17-24    16    7  257-384    24   11  4097-6144
1   0    2      9   3   25-32    17    7  385-512    25   11  6145-8192
2   0    3     10   4   33-48    18    8  513-768    26   12  8193-12288
3   0    4     11   4   49-64    19    8  769-1024   27   12 12289-16384
4   1   5,6    12   5   65-96    20    9 1025-1536   28   13 16385-24576
5   1   7,8    13   5   97-128   21    9 1537-2048   29   13 24577-32768
6   2   9-12   14   6  129-192   22   10 2049-3072
7   2  13-16   15   6  193-256   23   10 3073-4096
*/

static int length_code_list[]  = {3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,258};
static int length_code_extra[] = {0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0};
static int distance_code_list[]  = {1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,25477};
static int distance_code_extra[] = {0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13};

class LZSS
{
public:
    LZSS(BYTE* _stream, int len);

    int reslen, streamlen;
    BYTE* stream;

    int swleft, swright, swcount;
    BYTE slideWindow[MAXLZSW];

    int bufferleft, bufferright, buffercount;
    BYTE buffer[MAXLZBUFFER];

    int rescount;
    int result[BLOCKSIZE * 4];

    //struct [length, result(4)]
    int ldcode_res[5];

    bool read(int len);
    bool find();

    void ld2code(int length, int distance);

    struct lzss_result* dolzss();
};


class LZSSDecode
{
public:
    LZSSDecode(int* stream, int len);

    int rescount;
    int result[BLOCKSIZE * 4];
};

#endif // LZSS_H
