#ifndef ZIPCOMPRESSION_H
#define ZIPCOMPRESSION_H

#include "stdatx.h"
#include "zipformat.h"

class zipcompression
{
public:
    zipcompression();

    int filecount;
    char filename[1024][255];

    bool ListDirectoryContents(const char *sDir, int len);
    QWORD gettime();
    DWORD crc32(DWORD crc, const char *buf, int len);
    BYTE* doCompress(BYTE* stream, int inlen, int &outlen, int method);
    DWORD pack_onefileheader(char* infilepath, char* infilename, int method, FILE* output, DWORD &crc_32, int &datalen);
    DWORD pack_onecdheader(char* infilepath, char* infilename, int method, FILE* output, int datalen, DWORD crc_32, DWORD offset);
    void nocompression(char* dir, char* outfile);
};

#endif // ZIPCOMPRESSION_H
