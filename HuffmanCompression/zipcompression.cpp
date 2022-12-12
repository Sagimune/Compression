#include "zipcompression.h"

zipcompression::zipcompression()
{

}

void zipcompression::nocompression(char* infilepath, char* infilename, char* outfile)
{
    FILE* file = fopen(infilepath, "rb");
    FILE* output = fopen(outfile, "wb");
    char data[1024];
    fgets(data, 1024, file);

    int lfsize = 0;
    int cdsize = 0;

    //localFile
    struct localFile lf1;
    lf1.crc_32 = crc32(0, data, strlen(data));
    lf1.compressed_size = strlen(data);
    lf1.uncompressed_size = strlen(data);
    lf1.file_name_length = strlen(infilename);

    qDebug() << lf1.crc_32;
    qDebug() << sizeof(lf1);

    fwrite(&lf1, sizeof(lf1), 1, output);
    fputs(infilename, output);
    fwrite(data, sizeof(char), strlen(data), output);

    lfsize += sizeof(lf1);
    lfsize += strlen(infilename);
    lfsize += strlen(data);

    //centralDirectoryHeader
    struct centralDirectoryHeader cdh1;
    cdh1.crc_32 = crc32(0, data, strlen(data));
    cdh1.compressed_size = strlen(data);
    cdh1.uncompressed_size = strlen(data);
    cdh1.file_name_length = strlen(infilename);

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

    //ECDrecord
    struct ECDrecord footer;
    footer.size_of_the_central_directory = cdsize;
    footer.offset_of_cd_start_with_respect_to_the_starting_disk_number = lfsize;
    fwrite(&footer, sizeof(footer), 1, output);

    fclose(file);
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
    qDebug() << ret;

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
