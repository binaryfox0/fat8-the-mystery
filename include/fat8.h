#ifndef FAT8_H
#define FAT8_H

#include <stdint.h>

typedef enum fat8_file_attribute 
{
    FAT8_FILE_ATTRIB_BINARY      = 0200, // strlen 6
    FAT8_FILE_ATTRIB_FRAWC       = 0100, // Force read after write check, strlen 5
    FAT8_FILE_ATTRIB_EBCDIC      = 0040, // strlen 6
    FAT8_FILE_ATTRIB_WP          = 0020  // Write-protected strlen 2
} fat8_file_attribute;

// p167 h11.3 +0
typedef enum fat8_file_mode
{
    FAT8_FILE_MODE_INPUT_ONLY  = 0001,
    FAT8_FILE_MODE_OUTPUT_ONLY = 0002,
    FAT8_FILE_MODE_FILE        = 0004,
    FAT8_FILE_MODE_APPEND      = 0010,
    FAT8_FILE_MODE_DELETE      = 0020,
    FAT8_FILE_MODE_IBM         = 0040,
    FAT8_FILE_MODE_USR         = 0100, // Special format
    FAT8_FILE_MODE_BINARY_SAVE = 0200
} fat8_file_mode;

// p168 h11.3 +7
typedef enum fat8_file_flag
{
    FAT8_FILE_FLAG_READ_AFTER_WRITE = 0100,
    FAT8_FILE_FLAG_EBCDIC           = 0040,
    FAT8_FILE_FLAG_WRITE_PROTECTED  = 0020,
    FAT8_FILE_FLAG_BUFFER_PRINTED   = 0010,
    FAT8_FILE_FLAG_PUT_DONE         = 0004,
    FAT8_FILE_FLAG_BUFFER_EMPTY     = 0002
} fat8_file_flag;

// p165 h11.2.5
typedef enum fat8_fat_entry {
    FAT8_FAT_EOF_BASE  = 0300, // 0xc0
    FAT8_FAT_RESERVED  = 0xFE,
    FAT8_FAT_FREE      = 0xFF
} fat8_fat_entry;
#pragma pack(push, 1)
// p158 h1
typedef struct fat8_filename {
    uint8_t stem[6];
    uint8_t extension[3];
} fat8_filename;

// p165 h11.1
typedef struct {
    char file_name[9]; // fat8_filename
    uint8_t attribute;
    uint8_t ptr;
    uint8_t reserved[5];
} fat8_file_entry;

// p164 h10.1 (maybe fat8_file_block)
typedef struct fat8_file_format {
    uint8_t metadata[9];
    uint8_t data[128];
} fat8_file_format;

// p167 h11.3
typedef struct fat8_file_block {
    uint8_t file_mode;
    uint8_t first_cluster_fat_index;
    uint8_t last_cluster_fat_index;
    uint8_t last_sector_accessed;
    uint8_t disk_number;
    uint8_t last_buffer_size;
    uint8_t buffer_position;
    uint8_t file_flags;
    uint8_t tab_position;
    uint8_t sector_buffer[128];
} fat8_file_block;

// p161 h7
typedef enum fat8_disk_attribute
{
    FAT8_DSK_ATTRIB_READ_AFTER_WRITE  = 0100,
    FAT8_DSK_ATTRIB_WRITE_PROTECTED   = 0020,
    FAT8_DSK_ATTRIB_EBCDIC_CONVERSION = 0040
} fat8_disk_attribute;
#pragma pack(pop)

#endif