#include "zipcompression.h"

int gfilecount;
struct drawData drawdata[1024];

zipcompression::zipcompression()
{

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
        out = doCompress(data, inlen, outlen, 0);

        crc_32 = crc32(0, (char*)data, inlen);
        lf1.crc_32 = crc_32;
    }

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

BYTE* zipcompression::doCompress(BYTE* stream, int inlen, int &outlen, int method)
{
    if(method == 8)
    {

    }
    else
    {
        outlen = inlen;
        return stream;
    }
    return NULL;
}

DWORD zipcompression::pack_onecdheader(char* infilepath, char* infilename, int method, FILE* output, int datalen, DWORD crc_32, DWORD offset)
{
    //centralDirectoryHeader
    DWORD cdsize = 0;
    struct centralDirectoryHeader cdh1;
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

void zipcompression::nocompression(char* dir, char* outfile)
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
        qDebug() << "path:" << filepath;
        qDebug() << "name:" << filename[i] + 1;
        sprintf(drawdata[i].filename, "%s", filename[i] + 1);
        lfsize += pack_onefileheader(filepath, filename[i] + 1, 0, output, crc_32, datalen);
        cdsize += pack_onecdheader(filepath, filename[i] + 1, 0, tmpcdfile, datalen, crc_32, offset);
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
