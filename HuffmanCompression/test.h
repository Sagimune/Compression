#ifndef TEST_H
#define TEST_H

#include "lzss.h"
#include "stdatx.h"
#include "zipcompression.h"
#include <QDebug>
#include <QByteArray>

class test
{
public:
    test();

    bool testQByteArray2QString();
    bool testLZSS();
    bool testNoCompression();
    bool testCLCode();
};

#endif // TEST_H
