#include "test.h"

test::test()
{
    bool flag = true;
    flag |= testQByteArray2QString();
    flag |= testLZSS();
    flag |= testNoCompression();

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
    char stream[] = "ABCDEABCD ABCDEABCD";
    LZSS *test = new LZSS((BYTE*)stream, strlen(stream));
    int *result = test->dolzss();
    for(int i = 1; i <= result[0]; i ++ )
    {
        qDebug() << result[i] << " ";
    }
    return true;
}

bool test::testNoCompression()
{
    zipcompression *test = new zipcompression;
    char infilepath[] = "D:\\git\\test\\test1.txt";
    char infilename[] = "test1.txt";
    char outfile[] = "D:\\git\\test\\test1215.zip";
    //char dir[] = "D:\\git\\test\\mul\\";
    char dir[] = "D:\\git\\test\\include\\";
    test->nocompression(dir, outfile);
    //test->ListDirectoryContents("D:\\git\\test", strlen("D:\\git\\test\\"));
    return true;
}
