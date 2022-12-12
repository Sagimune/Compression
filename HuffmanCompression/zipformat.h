#include"stdatx.h"

#ifndef ZIPFORMAT_H
#define ZIPFORMAT_H


/*
    遵循规范
    https://pkware.cachefly.net/webdocs/APPNOTE/APPNOTE-6.3.9.TXT
    File:    APPNOTE.TXT - .ZIP File Format Specification
    Version: 6.3.9
    Status: FINAL - replaces version 6.3.8
    Revised: July 15, 2020
    Copyright (c) 1989 - 2014, 2018, 2019, 2020 PKWARE Inc., All Rights Reserved.
*/

typedef unsigned __int64 QWORD;

#pragma pack(1)
struct localFile
{
    //local file header x
    DWORD local_file_header_signature = 0x04034b50;
    WORD  version_needed_to_extract = 0x0014;
    WORD  general_purpose_bit_flag = 0x0000;
    WORD  compression_method = 0x0000;
    WORD  last_mod_file_time = 0x5c27;
    WORD  last_mod_file_date = 0x5587;
    DWORD crc_32;
    DWORD compressed_size;
    DWORD uncompressed_size;
    WORD  file_name_length;
    WORD  extra_field_length = 0x0000;

    //BYTE* filename;             //new byte[]    (变量定义长度)
    //BYTE* extra_field;

    //encryption header x       (可选)

    //file data x

    //data descriptor x


    //4.3.9.1 This descriptor MUST exist if bit 3 of the general purpose bit flag is set (see below).
    //4.3.9.2 When compressing files, compressed and uncompressed sizes SHOULD be stored in ZIP64 format (as 8 byte values) wh

};

/*
struct archiveMsg
{
    //archive decryption header

    //archive extra data record 4.3.11
    DWORD archive_extra_data_signature = 0x08064b50;
    DWORD extra_field_length;

    BYTE* extra_field_data;             //(长度变量定义)

    //4.3.10 The Archive Decryption Header is introduced in version 6.2 of the ZIP format specification.  This record exists in support
    //of the Central Directory Encryption Feature implemented as part of the Strong Encryption Specification as described in this document.

};
*/
struct NTFC_MACtime
{
    WORD    signature = 0x000a;
    WORD    TSize = 0x0020;
    DWORD   Reserved = 0x00000000;
    WORD    tag_signature = 0x0001;
    WORD    tag_size = 0x0018;
    QWORD   Mtime = 0x000000000000;
    QWORD   Atime = 0x000000000000;
    QWORD   Ctime = 0x000000000000;
};

struct centralDirectoryHeader
{
    //central directory header x 4.3.12
    DWORD central_file_header_signature = 0x02014b50;
    WORD  version_made_by = 0x0014;                              //4.4.2 这里设置0x0014 低位（规范版本2.0，版本见4.4.3）:0x14, 高位（兼容性）:0x00 MS-DOS
    WORD  version_needed_to_extract = 0x0014;                    //4.4.3 这里设置0x0014
    WORD  general_purpose_bit_flag = 0x0000;                     //4.4.4 这里设置0x0000 0 ? ? 0 ? 0 0 0 | 0 0 0 ? 0 0 0 0
                                                        //      问号依次代表：deflate速度选项2bit，deflate保留增强，语言编码标识（1则强制UTF-8）
    WORD  compression_method = 0x0000;                           //4.4.5 这里设置0x0008 压缩方式8号:deflate
    WORD  last_mod_file_time = 0x5c27;                           //4.4.6 以标准MS-DOS格式编码
    WORD  last_mod_file_date = 0x5587;
    DWORD crc_32;                                       //4.4.7 CRC-32校验
    DWORD compressed_size;                              //      压缩后大小
    DWORD uncompressed_size;                            //      未压缩大小

                                                        //任何目录记录和这三个字段的组合长度通常不应超过 65,535 字节。
    WORD  file_name_length;                             //      文件名大小
    WORD  extra_field_length = 0x0024;                           //      额外字段长度
    WORD  file_comment_length = 0x0000;                          //      文件注释长度

    WORD  disk_number_start = 0x0000;                            //设置为0x0000, 文件所在磁盘编号
    WORD  internal_file_attributes = 0x0000;                     //设置为0x0000
    DWORD external_file_attributes = 0x00000020;                     //4.4.15 依赖于主机系统，对于MS-DOS低位字节是MS-DOS目录属性字节  0x0020?
    DWORD relative_offset_of_local_header = 0x00000000;              //0x0000???

    //BYTE* file_name;                                    //4.4.17 不得包含驱动器或设备字母，必须正斜杠'/'
    //BYTE* extra_field;                                  //4.5 扩展域说明  4.5.5 NTFS  包含Mtime/Atime/Ctime
    //BYTE* file_comment;
};

/*
struct digitalSignture
{
    //digital signture 4.3.13
    DWORD header_signature = 0x05054b50;
    WORD  size_of_data;

    BYTE* signature_data;
};

struct zip64ECDredord
{
    //zip64 end of central directory record 4.3.14
    DWORD zip64_end_of_central_dir_signature = 0x06064b50;
    QWORD size_of_zip64_end_of_central;
    QWORD directory_record;
    WORD  version_made_by;
    WORD  version_needed_to_extract;
    DWORD number_of_this_disk;
    DWORD number_of_the_disk_with_cd_start;
    QWORD total_number_of_entries_in_cd_on_disk;
    QWORD total_number_of_entries_in_cd;
    QWORD size_of_the_central_directory;
    QWORD offset_of_start_of_cd_with_respect_to_starting_disk_number;

    BYTE* zip64_extensible_data_sector;
};

struct zip64ECDlocator
{
    //zip64 end of central directory locator 4.3.15
    DWORD zip64_end_of_central_dir_locator_signature = 0x07064b50;
    DWORD number_of_the_disk_with_start_of_zip64_cd_end;
    QWORD relative_offset_of_zip64_cd_end_record;
    DWORD total_number_of_disks;
};
*/

struct ECDrecord
{
    //end of central directory record 4.3.16
    DWORD end_of_central_dir_signature = 0x06054b50;
    WORD  number_of_this_disk = 0x0000;                                                  //设置为0x0000
    WORD  number_of_the_disk_with_cd_start = 0x0000;                                     //设置为0x0000
    WORD  total_number_of_entries_in_cd_on_this_disk = 0x0001;                           //总条目数(所有文件数目)
    WORD  total_number_of_entries_in_cd = 0x0001;                                        //4.4.22 .ZIP文件中文件条目总数
    DWORD size_of_the_central_directory;                                        //中心目录大小
    DWORD offset_of_cd_start_with_respect_to_the_starting_disk_number;          //相对于磁盘开始的偏移量
    WORD dotZIP_file_comment_length = 0x0000;                                            //注释长度

    //BYTE* dotZIP_file_comment;

    //end of central directory record
};
#pragma pack()

class ZipFormat
{
public:
    ZipFormat();
};

#endif // ZIPFORMAT_H
