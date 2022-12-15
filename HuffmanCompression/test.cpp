#include "test.h"

extern int gfilecount;
extern struct drawData drawdata[1024];

test::test()
{
    bool flag = true;
    //flag |= testQByteArray2QString();
    //flag |= testLZSS();
    flag |= testNoCompression();
    //flag |= testCLCode();

    if(flag)
        qDebug() << "测试通过 ";
    else
        qDebug() << "测试不通过 ";
}

bool test::testQByteArray2QString()
{
    qDebug() << "Ascii范围内测试 ";
    QByteArray ba("abc123");
    QString str = ba;
    qDebug() << str;

    qDebug() << "中文范围内测试 ";
    QByteArray ba2("你好abc123 ");
    QString str2 = ba2;
    qDebug() << str2;

    qDebug() << "testQByteArray2QString测试完毕 ";
    return true;
}

bool test::testLZSS()
{
    char stream[] = "As mentioned above,there are many kinds of wireless systems other than cellular.";
    //char stream[] = "ABCDEABCD ABCDEABCD";
    LZSS *test = new LZSS((BYTE*)stream, strlen(stream));
    //int *result = test->dolzss();
    //for(int i = 1; i <= result[0]; i ++ )
    //{
    //    qDebug() << result[i] << " ";
    //}
    return true;
}

bool test::testNoCompression()
{
    zipcompression *test = new zipcompression;
    char outfile[] = "D:\\git\\test\\test1216.zip";
    char dir[] = "D:\\git\\test\\Test.txt";
    char name[] = "Test.txt";
    //char dir[] = "D:\\git\\test\\include\\";

    char dedir[] = "D:\\git\\test\\testde\\";
    char defile[] = "D:\\git\\test\\test1215.zip";

    test->compressionOne(dir, name, outfile, 0);
    //test->compressionOne(dir, name, outfile, 8);
    //test->viewzip(defile);
    //test->decompress(defile, dedir);

    //for(int i = 0; i < gfilecount; i ++ )
    //{
    //    qDebug() << i << ": "<< drawdata[i].filename;
    //}


    return true;
}

bool test::testCLCode()
{
    zipcompression *test = new zipcompression;

    int llcode[286];
    memset(llcode, 0, sizeof(llcode));

    llcode[32] = llcode[65] = 4;
    for(int i = 66; i <= 69; i ++ ) llcode[i] = 3;
    llcode[256] = llcode[258] = llcode[263] = 3;

    int outlen = 0;
    int *outstream = test->clcodeEncode(llcode, 285, outlen);
    for(int i = 0; i < outlen; i ++ )
    {
        qDebug() << outstream[i];
    }

    qDebug() << "CLCodeEncode测试完毕";

    return true;
}
