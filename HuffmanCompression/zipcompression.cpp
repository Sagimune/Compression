#include "zipcompression.h"

int gfilecount;
struct drawData drawdata[1024];

zipcompression::zipcompression()
{
    memset(drawdata, 0, sizeof(drawdata));
    memset(filename, 0, sizeof(filename));
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
                //printf("%s\\\n", sPath + len);
                sprintf(filename[filecount ++], "%s\\", sPath + len);
                ListDirectoryContents(sPath, len);
            }
            else{
                //printf("%s\n", sPath + len);
                sprintf(filename[filecount ++], "%s", sPath + len);
            }
        }
    }

    while(FindNextFileA(hFind, &fdFile));

    FindClose(hFind);

    return true;
}

DWORD zipcompression::pack_onefileheader(char* infilepath, char* infilename, int method, FILE* output, DWORD &crc_32, int &inlen)
{
    //localFile
    struct localFile lf1;
    DWORD lfsize = 0;
    int outlen = 0;
    int isdir = 0;
    BYTE* out;

    if(infilename[strlen(infilename) - 1] == '\\')
    {
        isdir = 1;
        crc_32 = 0x0000;
        inlen = 0x0000;
        outlen = 0x0000;
    }
    else
    {

        BYTE data[BLOCKSIZE];
        FILE* file = fopen(infilepath, "rb");

        //motify inlen & data
        inlen = fread(&data, sizeof(BYTE), BLOCKSIZE, file);
        fclose(file);

        //motify outlen & data
        out = doCompress(data, inlen, outlen, method);

        crc_32 = crc32(0, (char*)data, inlen);
        lf1.crc_32 = crc_32;
    }

    SYSTEMTIME sys_t;
    GetSystemTime(&sys_t);
    qDebug() << "time: " << sys_t.wHour+8 << ":" << sys_t.wMinute << ":" << sys_t.wSecond;
    lf1.last_mod_file_date = (sys_t.wDay & daymask) | ((sys_t.wMonth << 5) & monthmask) | (sys_t.wYear-1980 << 9);
    lf1.last_mod_file_time = (sys_t.wSecond / 2 & secondmask) | ((sys_t.wMinute << 5) & minutemask) | ((sys_t.wHour+8) << 11);
    lf1.compressed_size = outlen;
    lf1.uncompressed_size = inlen;
    lf1.file_name_length = strlen(infilename);

    //qDebug() << lf1.crc_32;
    //qDebug() << sizeof(lf1);

    //输出文件头
    fwrite(&lf1, sizeof(lf1), 1, output);

    //输出文件名
    fputs(infilename, output);

    //输出压缩数据
    if(!isdir) fwrite(out, sizeof(BYTE), outlen, output);

    lfsize += sizeof(lf1);
    lfsize += strlen(infilename);
    lfsize += inlen;

    return lfsize;
}

DWORD zipcompression::pack_onecdheader(char* infilepath, char* infilename, int method, FILE* output, int datalen, DWORD crc_32, DWORD offset)
{
    //centralDirectoryHeader
    DWORD cdsize = 0;
    struct centralDirectoryHeader cdh1;

    SYSTEMTIME sys_t;
    GetSystemTime(&sys_t);
    cdh1.last_mod_file_date = (sys_t.wDay & daymask) | ((sys_t.wMonth << 5) & monthmask) | (sys_t.wYear-1980 << 9);
    cdh1.last_mod_file_time = (sys_t.wSecond / 2 & secondmask) | ((sys_t.wMinute << 5) & minutemask) | ((sys_t.wHour+8) << 11);
    cdh1.crc_32 = crc_32;
    cdh1.compressed_size = datalen;
    cdh1.uncompressed_size = datalen;
    cdh1.file_name_length = strlen(infilename);
    cdh1.relative_offset_of_local_header = offset;

    fwrite(&cdh1, sizeof(cdh1), 1, output);
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

BYTE* zipcompression::doCompress(BYTE* stream, int inlen, int &outlen, int method)
{
    if(method == 8)
    {
        Compression tool;
        LZSS *test = new LZSS((BYTE*)stream, inlen);
        lzss_result *result = test->dolzss();
        qDebug() << "doCompress: LZSS count";
        qDebug() << result->src_result[0];
        qDebug() << result->LL_result[0];
        qDebug() << result->Distance_result[0];

        huffman_result* LL_huffman = tool.ziphuffman_encode(result->LL_result + 1, result->LL_result[0]);
        huffman_result* Dist_huffman = tool.ziphuffman_encode(result->Distance_result + 1, result->Distance_result[0]);
        for(int i = 0; i < LL_huffman->outlen; i ++ )
        {
            ComparisonNode *data = &LL_huffman->ComNodeOut[i];
            if(data->Len)
            {
                std::string str = (data->Code).to_string();
                qDebug() << data->C << " : " << data->Len << " : " << QString::fromStdString(str);
            }
        }

        outlen = inlen;
        return stream;
    }
    else
    {
        outlen = inlen;
        return stream;
    }
    return NULL;
}

void zipcompression::compression(char* dir, char* outfile, int method)
{
    filecount = 0;
    ListDirectoryContents(dir, strlen(dir));

    FILE* output = fopen(outfile, "wb");
    FILE* tmpcdfile = fopen("tmp.tmp", "wb+");
    DWORD lfsize, cdsize, crc_32, offset;
    int datalen;

    offset = 0;
    for(int i = 0; i < filecount; i ++ )
    {
        char filepath[1024];
        sprintf(filepath, "%s%s", dir, filename[i] + 1);
        lfsize += pack_onefileheader(filepath, filename[i] + 1, method, output, crc_32, datalen);
        cdsize += pack_onecdheader(filepath, filename[i] + 1, method, tmpcdfile, datalen, crc_32, offset);
        offset = lfsize;
    }
    gfilecount = filecount;

    fclose(tmpcdfile);
    tmpcdfile = fopen("tmp.tmp", "r");

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
}

void zipcompression::compressionOne(char* dir, char* name, char* outfile, int method)
{
    filecount = 1;

    FILE* output = fopen(outfile, "wb");
    FILE* tmpcdfile = fopen("tmp.tmp", "wb+");
    DWORD lfsize, cdsize, crc_32, offset;
    int datalen;

    lfsize = cdsize = crc_32 = offset = 0;
    lfsize += pack_onefileheader(dir, name, method, output, crc_32, datalen);
    cdsize += pack_onecdheader(dir, name, method, tmpcdfile, datalen, crc_32, offset);
    offset = lfsize;


    fclose(tmpcdfile);
    tmpcdfile = fopen("tmp.tmp", "r");

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
}


BYTE* zipcompression::doDecompress(BYTE* stream, int inlen, int &outlen, int method)
{
    if(method == 8)
    {
        //比特归类法
        int op;
        /*
            1： read 3bit ： CCL
            2： read
        */
    }
    else
    {
        outlen = inlen;
        return stream;
    }
    return NULL;
}

bool zipcompression::viewzip(char *zipfilename)
{
    memset(filename, 0, sizeof(filename));
    FILE *zipfile = fopen(zipfilename, "rb");

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
        fread(filename[i], file_name_length, 1, zipfile);
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

    FILE *zipfile = fopen(zipfilename, "rb");
    BYTE datatmp[BLOCKSIZE];


    //localFile
    fseek(zipfile, 0, SEEK_SET);
    for(int i = 0; i < filecount; i ++ )
    {
        BYTE tmp[sizeof(localFile)];
        fread(tmp, sizeof(localFile), 1, zipfile);

        char onefilename[1024];
        sprintf(onefilename, "%s%s", where, filename[i]);

        struct localFile *fileheader = (localFile*)tmp;
        DWORD datasize = fileheader->compressed_size;

        fseek(zipfile, fileheader->file_name_length, SEEK_CUR);
        if(datasize == 0)
        {
            //mkdir
            CreateDirectoryA(onefilename, NULL);
        }
        else
        {
            fread(datatmp, sizeof(BYTE), datasize, zipfile);

            int outlen;
            BYTE *out = doDecompress(datatmp, datasize, outlen, fileheader->compression_method);

            FILE *outfile = fopen(onefilename, "wb");
            fwrite(out, sizeof(BYTE), outlen, outfile);
            fclose(outfile);
        }
    }

    fclose(zipfile);

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

int* zipcompression::clcodeEncode(int *stream, int inlen, int& outlen)
{
    outlen = 0;
    if(inlen < 3)
    {
        outlen = inlen;
        return stream;
    }
    int count = 0;
    stream[inlen] = 99;
    inlen ++;
    for(int i = 0; i < inlen - 1; i ++)
    {
        clcode_out[outlen ++] = stream[i];
        if(stream[i] == stream[i+1] && (count + 1 <= 137))
        {
            count ++;
        }
        else
        {   //count + 1; repeat
            if(count + 1 >= 3)
            {
                if(stream[i] == 0 && count + 1 <= 10)
                {
                    outlen -= count + 1;
                    clcode_out[outlen ++] = 17;
                    clcode_out[outlen ++] = count+1 - 3;
                }
                else if(stream[i] == 0 && count + 1 >= 11)
                {
                    outlen -= count + 1;
                    clcode_out[outlen ++] = 18;
                    clcode_out[outlen ++] = count+1 - 11;
                }
                else if(count + 1 >= 4)
                {
                    outlen -= count + 1;
                    clcode_out[outlen ++] = stream[i];
                    while(count >= 6)
                    {
                        clcode_out[outlen ++] = 16;
                        clcode_out[outlen ++] = 3;
                        count -= 6;
                    }
                    if(count >= 3)
                    {
                        clcode_out[outlen ++] = 16;
                        clcode_out[outlen ++] = count - 3;
                    }
                    else
                    {
                        for(int j = 0; j < count; j ++ ) clcode_out[outlen ++] = stream[i];
                    }
                }
            }
            count = 0;
        }
    }

    return clcode_out;
}
