#ifndef ZIPCOMPRESSION_H
#define ZIPCOMPRESSION_H

#include "stdatx.h"
#include "zipformat.h"

class zipcompression
{
public:
    zipcompression();

    QWORD gettime();
    DWORD crc32(DWORD crc, const char *buf, int len);
    void nocompression(char* infilepath, char* infilename, char* outfile);
};

#endif // ZIPCOMPRESSION_H
