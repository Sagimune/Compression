#ifndef ZIPCOMPRESSION_H
#define ZIPCOMPRESSION_H

#include "stdatx.h"
#include "zipformat.h"
#include "lzss.h"
#include <bitset>
#include "compression.h"

struct drawData
{
    char filename[255];
    int uncompress_size;
    int compress_size;
    int crc_32;
};

static WORD secondmask = 0x1f;
static WORD minutemask = 0x7e0;
static WORD daymask = 0x1f;
static WORD monthmask = 0x1e0;

class zipcompression
{
public:
    zipcompression();

    int filecount;
    char filename[1024][255];
    char filepath[1024][255];

    DWORD cdoffset;

    int clcode_out[BLOCKSIZE*4];
    Compression *tool;

    bool ListDirectoryContents(const char *sDir, int len);
    QWORD gettime();
    DWORD crc32(DWORD crc, const char *buf, int len);
    BYTE* doCompress(int &outlen, char* infilepath);
    DWORD pack_onefileheader(char* infilepath, char* infilename, int method, FILE* output, DWORD &crc_32, int &compress_size, int &uncompress_size);
    DWORD pack_onecdheader(char* infilepath, char* infilename, int method, FILE* output, int compress_size, int uncompress_size, DWORD crc_32, DWORD offset);
    void compressionDir(char* dir, char* outfile);
    void compressionFile(char* outfile, int infilecount);

    BYTE* doDecompress(FILE* zipfile, int inlen, int &outlen);
    bool viewzip(char *zipfilename);
    bool decompress(char *zipfilename, char* where);

    void compressCtrl(char* outfile);

};

#endif // ZIPCOMPRESSION_H
