#include "test.h"

extern int gfilecount;
extern struct drawData drawdata[1024];

test::test()
{
    bool flag = true;
    //flag |= testQByteArray2QString();
    //flag |= testLZSS();
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



    char dedir[] = "D:\\git\\test\\testde\\";
    char defile[] = "D:\\git\\test\\test1215.zip";


    char outfile1[] = "D:\\git\\test\\test1216.zip";
    char onefilepath[] = "D:\\git\\test\\Test.txt";
    char onefilename[] = "Test.txt";

    sprintf(test->filepath[0], "%s", onefilepath);
    sprintf(test->filename[0], "%s", onefilename);
    test->compressionFile(outfile1, 1);


    char dir[] = "D:\\git\\test\\include\\";
    char outfile2[] = "D:\\git\\test\\test1217.zip";
    test->compressionDir(dir, outfile2);

    //char dir[] = "D:\\git\\test\\Test.txt";
    //char name[] = "Test.txt";
    //test->compressionOne(dir, name, outfile, 0);

    //test->viewzip(defile);
    //test->decompress(defile, dedir);


    return true;
}
