#include "zipcompression.h"

int gfilecount;
struct drawData drawdata[1024];

zipcompression::zipcompression()
{
    memset(drawdata, 0, sizeof(drawdata));
    memset(filename, 0, sizeof(filename));
    tool = new Compression;
    pwidget = new processwidget;

    connect(tool, SIGNAL(mysignal(double)), pwidget, SLOT(movefileprocess(double)));
    connect(this, SIGNAL(onefilecomp(double)), pwidget, SLOT(moveallprocess(double)));
}

bool zipcompression::ListDirectoryContents(const char *sDir, int len)
{
    WIN32_FIND_DATAA fdFile;
    HANDLE hFind = NULL;

    char sPath[2048];

    sprintf(sPath, "%s\\*.*", sDir);

    if((hFind = FindFirstFileA(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
    {
        printf("Path not found: [%s]\n", sDir);
        return false;
    }

    do
    {
        if(strcmp(fdFile.cFileName, ".") != 0 && strcmp(fdFile.cFileName, "..") != 0)
        {
            sprintf(sPath, "%s\\%s", sDir, fdFile.cFileName);

            if(fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
            {
                printf("%s\\\n", sPath + len);
                sprintf(filename[filecount ++], "%s\\", sPath + len);
                ListDirectoryContents(sPath, len);
            }
            else{
                printf("%s\n", sPath + len);
                sprintf(filename[filecount ++], "%s", sPath + len);
            }
        }
    }

    while(FindNextFileA(hFind, &fdFile));

    FindClose(hFind);

    return true;
}

DWORD zipcompression::pack_onefileheader(char* infilepath, char* infilename, int method, FILE* output, DWORD &crc_32, int &compress_size, int &uncompress_size)
{
    //localFile
    struct localFile lf1;
    DWORD lfsize = 0;
    compress_size = 0;
    int isdir = 0;
    BYTE* out;

    if(infilename[strlen(infilename) - 1] == '\\')
    {
        isdir = 1;
        crc_32 = 0x0000;
        uncompress_size = 0x0000;
        compress_size = 0x0000;
    }
    else
    {

        BYTE data[BLOCKSIZE];
        FILE* file = fopen(infilepath, "rb");
        if(!file)
        {
            qDebug() << "pack_onefileheader: file not found.  path: " << infilepath;
            return 0;
        }

        //motify inlen & data
        uncompress_size = fread(&data, sizeof(BYTE), BLOCKSIZE, file);
        fclose(file);

        //motify outlen & data
        out = doCompress(compress_size, infilepath);

        crc_32 = crc32(0, (char*)data, uncompress_size);
        lf1.crc_32 = crc_32;
    }

    SYSTEMTIME sys_t;
    GetSystemTime(&sys_t);
    qDebug() << "time: " << sys_t.wHour+8 << ":" << sys_t.wMinute << ":" << sys_t.wSecond;
    lf1.last_mod_file_date = (sys_t.wDay & daymask) | ((sys_t.wMonth << 5) & monthmask) | (sys_t.wYear-1980 << 9);
    lf1.last_mod_file_time = (sys_t.wSecond / 2 & secondmask) | ((sys_t.wMinute << 5) & minutemask) | ((sys_t.wHour+8) << 11);
    lf1.compressed_size = compress_size;
    lf1.uncompressed_size = uncompress_size;
    lf1.file_name_length = strlen(infilename);

    //qDebug() << lf1.crc_32;
    //qDebug() << sizeof(lf1);

    //输出文件头
    fwrite(&lf1, sizeof(lf1), 1, output);

    //输出文件名
    for(int i = 0; i < strlen(infilename); i ++ ) if(infilename[i] == '\\') infilename[i] = '/';
    fputs(infilename, output);

    //输出压缩数据
    if(!isdir)
    {
        fwrite(out, sizeof(BYTE), compress_size, output);
    }

    lfsize += sizeof(lf1);
    lfsize += strlen(infilename);
    lfsize += compress_size;

    return lfsize;
}

DWORD zipcompression::pack_onecdheader(char* infilepath, char* infilename, int method, FILE* output, int compress_size, int uncompress_size, DWORD crc_32, DWORD offset)
{
    //centralDirectoryHeader
    DWORD cdsize = 0;
    struct centralDirectoryHeader cdh1;

    SYSTEMTIME sys_t;
    GetSystemTime(&sys_t);
    cdh1.last_mod_file_date = (sys_t.wDay & daymask) | ((sys_t.wMonth << 5) & monthmask) | (sys_t.wYear-1980 << 9);
    cdh1.last_mod_file_time = (sys_t.wSecond / 2 & secondmask) | ((sys_t.wMinute << 5) & minutemask) | ((sys_t.wHour+8) << 11);
    cdh1.crc_32 = crc_32;
    cdh1.compressed_size = compress_size;
    cdh1.uncompressed_size = uncompress_size;
    cdh1.file_name_length = strlen(infilename);
    cdh1.relative_offset_of_local_header = offset;

    fwrite(&cdh1, sizeof(cdh1), 1, output);
    for(int i = 0; i < strlen(infilename); i ++ ) if(infilename[i] == '\\') infilename[i] = '/';
    fputs(infilename, output);

    //NTFC_TIME
    struct NTFC_MACtime time;
    QWORD ntfstime = gettime();
    time.Mtime = time.Ctime = time.Atime = ntfstime;
    fwrite(&time, sizeof(time), 1, output);

    cdsize += sizeof(cdh1);
    cdsize += strlen(infilename);
    cdsize += sizeof(time);

    return cdsize;
}

long get_file_size(FILE *stream)
{
    long file_size = -1;
    long cur_offset = ftell(stream);	// 获取当前偏移位置
    if (cur_offset == -1) {
        printf("ftell failed :%s\n", strerror(errno));
        return -1;
    }
    if (fseek(stream, 0, SEEK_END) != 0) {	// 移动文件指针到文件末尾
        printf("fseek failed: %s\n", strerror(errno));
        return -1;
    }
    file_size = ftell(stream);	// 获取此时偏移值，即文件大小
    if (file_size == -1) {
        printf("ftell failed :%s\n", strerror(errno));
    }
    if (fseek(stream, cur_offset, SEEK_SET) != 0) {	// 将文件指针恢复初始位置
        printf("fseek failed: %s\n", strerror(errno));
        return -1;
    }
    return file_size;
}

BYTE* zipcompression::doCompress(int &outlen, char* infilepath)
{
    //将文件给哈夫曼压缩
    alltime += tool->Zip(QString(infilepath));

    //获取结果文件，获取文件大小，new一个字节流，写入，设定返回大小，关闭，删除文件，返回字节流
    FILE *compresstmp = fopen("haffuman.tmp", "rb");
    if(!compresstmp)
    {
        qDebug() << "doCompress: file not found.  path: haffuman.tmp";
    }

    int filesize = get_file_size(compresstmp);
    BYTE *data = new BYTE[filesize];

    fread(data, sizeof(BYTE), filesize, compresstmp);
    outlen = filesize;

    fclose(compresstmp);
    //remove("haffuman.tmp");

    return data;
}

void zipcompression::compressionDir(char* dir, char* outfile)
{
    filecount = 0;
    ListDirectoryContents(dir, strlen(dir));
    if(filecount == 0) return;

    FILE* output = fopen(outfile, "wb");
    FILE* tmpcdfile = fopen("tmp.tmp", "wb+");
    if(!output)
    {
        qDebug() << "compressDir: file not found.  path: " << outfile;
        return;
    }
    if(!tmpcdfile)
    {
        qDebug() << "compressDir: file not found.  path: " << "tmp.tmp";
        return;
    }
    DWORD lfsize, cdsize, crc_32, offset;
    lfsize = cdsize = crc_32 = offset = 0;
    int compress_size = 0;
    int uncompress_size = 0;

    offset = 0;

    double processvalue = 0;
    double processstep = 100 / filecount;
    alltime = allcompress = alluncompress = 0.0;
    pwidget->show();
    pwidget->init();
    for(int i = 0; i < filecount; i ++ )
    {
        char filepath[1024];
        sprintf(filepath, "%s%s", dir, filename[i] + 1);
        lfsize += pack_onefileheader(filepath, filename[i] + 1, 0, output, crc_32, compress_size, uncompress_size);
        cdsize += pack_onecdheader(filepath, filename[i] + 1, 0, tmpcdfile, compress_size, uncompress_size, crc_32, offset);
        offset = lfsize;

        allcompress += compress_size;
        alluncompress += uncompress_size;

        processvalue += processstep;
        emit onefilecomp(processvalue);
    }
    gfilecount = filecount;

    fclose(tmpcdfile);
    tmpcdfile = fopen("tmp.tmp", "r");
    if(!tmpcdfile)
    {
        qDebug() << "compressDir: file not found.  path: " << "tmp.tmp";
        return;
    }

    while(!feof(tmpcdfile))
    {
        char ch = fgetc(tmpcdfile);
        if(ch == EOF) break;
        fprintf(output, "%c", ch);
    }
    remove("tmp.tmp");

    //ECDrecord
    struct ECDrecord footer;
    footer.total_number_of_entries_in_cd_on_this_disk = filecount;
    footer.total_number_of_entries_in_cd = filecount;
    footer.size_of_the_central_directory = cdsize;
    footer.offset_of_cd_start_with_respect_to_the_starting_disk_number = lfsize;
    fwrite(&footer, sizeof(footer), 1, output);

    fclose(output);

    pwidget->finish(alltime, allcompress / alluncompress);
}

void zipcompression::compressionFile(char* outfile, int infilecount)
{
    filecount = infilecount;
    if(filecount == 0) return;

    FILE* output = fopen(outfile, "wb");
    FILE* tmpcdfile = fopen("tmp.tmp", "wb+");
    if(!output)
    {
        qDebug() << "compressionFile: file not found.  path: " << outfile;
        return;
    }
    if(!tmpcdfile)
    {
        qDebug() << "compressionFile: file not found.  path: " << "tmp.tmp";
        return;
    }
    DWORD lfsize, cdsize, crc_32, offset;
    int compress_size = 0;
    int uncompress_size = 0;
    lfsize = cdsize = crc_32 = offset = 0;

    double processvalue = 0;
    double processstep = 100 / filecount;
    alltime = allcompress = alluncompress = 0.0;
    pwidget->init();
    pwidget->show();
    for(int i = 0; i < filecount; i ++ )
    {
        lfsize += pack_onefileheader(filepath[i], filename[i], 0, output, crc_32, compress_size, uncompress_size);
        cdsize += pack_onecdheader(filepath[i], filename[i], 0, tmpcdfile, compress_size, uncompress_size, crc_32, offset);
        offset = lfsize;

        allcompress += compress_size;
        alluncompress += uncompress_size;

        processvalue += processstep;
        emit onefilecomp(processvalue);
    }

    fclose(tmpcdfile);
    tmpcdfile = fopen("tmp.tmp", "r");
    if(!tmpcdfile)
    {
        qDebug() << "compressionFile: file not found.  path: " << "tmp.tmp";
        return;
    }

    int cnt = 0;
    while(!feof(tmpcdfile))
    {
        char ch = fgetc(tmpcdfile);
        if(feof(tmpcdfile)) break;
        cnt ++;
        fprintf(output, "%c", ch);
    }
    qDebug() << "output " << cnt << " byte cdheader";
    remove("tmp.tmp");

    //ECDrecord
    struct ECDrecord footer;
    footer.total_number_of_entries_in_cd_on_this_disk = filecount;
    footer.total_number_of_entries_in_cd = filecount;
    footer.size_of_the_central_directory = cdsize;
    footer.offset_of_cd_start_with_respect_to_the_starting_disk_number = lfsize;
    fwrite(&footer, sizeof(footer), 1, output);

    fclose(output);

    pwidget->finish(alltime, allcompress / alluncompress);
}

BYTE* zipcompression::doDecompress(FILE* zipfile, int inlen, int &outlen)
{
    //分解文件头，zipfile输出inlen个字节
    FILE *decompresstmp = fopen("haffuman2.tmp", "wb");
    if(!decompresstmp)
    {
        qDebug() << "doDecompress: file not fouond.  path:" << "haffuman2.tmp";
        return NULL;
    }

    for(int i = 0; i < inlen; i ++ )
    {
        char ch = fgetc(zipfile);
        fprintf(decompresstmp, "%c", ch);
    }
    //关闭，将文件转交哈夫曼解压缩
    fclose(decompresstmp);
    alltime += tool->UnZip(QString("haffuman2.tmp"));

    //获取解压缩结果文件，获取大小，new一个字节流，读进去，设定返回大小，关闭文件，删除文件，返回文件流
    FILE *decompresstmp2 = fopen("haffuman3.tmp", "rb");
    if(!decompresstmp2)
    {
        qDebug() << "doDecompress: file not fouond.  path:" << "haffuman3.tmp";
        return NULL;
    }

    int filesize = get_file_size(decompresstmp2);
    BYTE *data = new BYTE[filesize];

    fread(data, sizeof(BYTE), filesize, decompresstmp2);
    fclose(decompresstmp2);
    //remove("haffuman3.tmp");

    outlen = filesize;
    return data;
}

bool zipcompression::viewzip(char *zipfilename)
{
    memset(filename, 0, sizeof(filename));
    FILE *zipfile = fopen(zipfilename, "rb");
    if(!zipfile)
    {
        qDebug() << "viewzip: file not found.  path: " << zipfilename;
        return 0;
    }

    //ECDrecord
    BYTE tmp[sizeof(ECDrecord)];
    fseek(zipfile, -sizeof(ECDrecord), SEEK_END);
    fread(tmp, sizeof(ECDrecord), 1, zipfile);
    struct ECDrecord *footer = (ECDrecord*)tmp;

    filecount = footer->total_number_of_entries_in_cd;
    cdoffset = footer->offset_of_cd_start_with_respect_to_the_starting_disk_number;

    //cdHeader
    fseek(zipfile, cdoffset, SEEK_SET);
    for(int i = 0; i < filecount; i ++ )
    {
        BYTE tmp[sizeof(centralDirectoryHeader)];
        fread(tmp, sizeof(centralDirectoryHeader), 1, zipfile);
        struct centralDirectoryHeader *cdheader = (centralDirectoryHeader*)tmp;
        WORD file_name_length = cdheader->file_name_length;
        fread(filename[i], sizeof(BYTE), file_name_length, zipfile);
        fseek(zipfile, 0x0024, SEEK_CUR);

        sprintf(drawdata[i].filename, "%s", filename[i]);
        drawdata[i].uncompress_size = cdheader->uncompressed_size;
        drawdata[i].compress_size = cdheader->compressed_size;
        drawdata[i].crc_32 = cdheader->crc_32;
        qDebug() << "viewzip: i: " << i <<  "filenamelength: " << file_name_length << "  filename: " << filename[i];
    }

    fclose(zipfile);

    return true;
}

bool zipcompression::decompress(char *zipfilename, char* where)
{
    viewzip(zipfilename);
    if(filecount == 0) return 0;
    alltime = allcompress = alluncompress = 0.0;

    FILE *zipfile = fopen(zipfilename, "rb");
    if(!zipfile)
    {
        qDebug() << "decompress: file not found.  path: " << zipfilename;
        return 0;
    }
    qDebug()<<filecount;
    double processvalue = 0;
    double processstep = 100.0 / filecount;
    pwidget->init();
    pwidget->show();
    fseek(zipfile, 0, SEEK_SET);
    for(int i = 0; i < filecount; i ++ )
    {
        BYTE tmp[sizeof(localFile)];
        fread(tmp, sizeof(localFile), 1, zipfile);

        struct localFile *fileheader = (localFile*)tmp;
        DWORD datasize = fileheader->compressed_size;

        allcompress += fileheader->compressed_size;
        alluncompress += fileheader->uncompressed_size;

        char rfilename[1024];
        fread(rfilename, sizeof(BYTE), fileheader->file_name_length, zipfile);
        rfilename[fileheader->file_name_length] = '\0';

        char onefilepath[1024];
        sprintf(onefilepath, "%s%s", where, rfilename);
        //fseek(zipfile, fileheader->file_name_length, SEEK_CUR);
        if(datasize == 0)
        {
            //mkdir
            CreateDirectoryA(onefilepath, NULL);
        }
        else
        {
            int outlen;
            BYTE *out = doDecompress(zipfile, datasize, outlen);

            FILE *outfile = fopen(onefilepath, "wb");
            fwrite(out, sizeof(BYTE), outlen, outfile);
            fclose(outfile);
        }

        processvalue += processstep;
        emit onefilecomp(processvalue);
    }

    fclose(zipfile);
    qDebug()<<"unzip done";

    pwidget->finish(alltime, allcompress / alluncompress);

    return true;
}

QWORD zipcompression::gettime()
{
    SYSTEMTIME sys_t;
    FILETIME   file_t;
    GetSystemTime(&sys_t);
    SystemTimeToFileTime(&sys_t, &file_t);
    QWORD ret = file_t.dwHighDateTime;
    ret = ret << 32;
    ret |= file_t.dwLowDateTime;

    //qDebug() << file_t.dwHighDateTime << " " << file_t.dwLowDateTime;
    //qDebug() << ret;

    return ret;
}

DWORD zipcompression::crc32(DWORD crc, const char *buf, int len)
{
    static uint32_t table[256];
    static int have_table = 0;
    uint32_t rem;
    uint8_t octet;
    int i, j;
    const char *p, *q;

    /* This check is not thread safe; there is no mutex. */
    if (have_table == 0) {
        /* Calculate CRC table. */
        for (i = 0; i < 256; i++) {
            rem = i;  /* remainder from polynomial division */
            for (j = 0; j < 8; j++) {
                if (rem & 1) {
                    rem >>= 1;
                    rem ^= 0xedb88320;
                } else
                    rem >>= 1;
            }
            table[i] = rem;
        }
        have_table = 1;
    }

    crc = ~crc;
    q = buf + len;
    for (p = buf; p < q; p++) {
        octet = *p;  /* Cast to unsigned octet. */
        crc = (crc >> 8) ^ table[(crc & 0xff) ^ octet];
    }
    return ~crc;
}

