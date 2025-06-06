#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "aparse.h"

#define max(a, b) ((a > b) ? (a) : (b))

#define FLOPPY_SECTOR_PER_TRACK 16
#define FLOPPY_SIDE_COUNT       2

#define FLOPPY_TRACK_SIZE   0x1000 // 4096
#define FLOPPY_CLUSTER_SIZE 0x800  // 2048
#define FLOPPY_SECTOR_SIZE  0x100  // 256

// static inline int calculate_offset(int cylinder, int side, int sector, int base) {
    // int logical_sector = ((cylinder * FLOPPY_SIDE_COUNT + side) * FLOPPY_SECTOR_PER_TRACK) + (sector - !!base);
    // return logical_sector * FLOPPY_SECTOR_SIZE;
// }

void offset_to_chs(int offset,
                   int heads_per_cylinder, int sectors_per_track, int base,
                   int *cylinder, int *head, int *sector) {
    int sectors_per_cylinder = heads_per_cylinder * sectors_per_track;

    *cylinder = offset / sectors_per_cylinder;
    int temp = offset % sectors_per_cylinder;

    *head = temp / sectors_per_track;
    *sector = (temp % sectors_per_track) + !!base;
}

#pragma pack(push, 1)
typedef struct fat8_filename {
    uint8_t stem[6];
    uint8_t extension[3];
} fat8_filename;

typedef struct {
    char file_name[9]; // fat8_filename
    uint8_t attribute;
    uint8_t ptr;
    uint8_t reserved[5];
} fat8_file_entry;

typedef struct {
    
} fat8_file_format;
#pragma pack(pop)

typedef enum fat8_file_attribute 
{
    FAT8_FILE_BINARY      = 0200,
    FAT8_FILE_FRAWC       = 0100, // Force read after write check
    FAT8_FILE_EBCDIC      = 0040,
    FAT8_WRITE_PROCTECTED = 0020
} fat8_file_attribute;

const char* attribute_string(const uint8_t attribute)
{
    
    switch(attribute)
    {
    case FAT8_FILE_BINARY:
        return "Binary";
    case FAT8_FILE_FRAWC:
        return "FRAWC";
    case FAT8_FILE_EBCDIC:
        return "EBCDIC";
    case FAT8_WRITE_PROCTECTED:
        return "Write-protected";
    default:
        return "Unknown";
    }
}

typedef struct analyze_metadata { char* filename; size_t offset; } analyze_metadata;
void analyze_dir_command(void* tmp)
{
    analyze_metadata* data = tmp;
    FILE *file = fopen(data->filename, "rb");
    if(!file){
        printf("Failed to open specified image file: %s\n", strerror(errno));
        return;
    }
    fseek(file, data->offset, SEEK_SET);
    fat8_file_entry entry;
    while(fread(&entry, 1, sizeof(entry), file) == sizeof(entry))
    {
        if(entry.file_name[0] == -1)
            break;
        else if(entry.file_name[0] == 0)
            printf("Empty-marked. ");
        printf("Name: '");
        for(int i = 0; i < 9; i++)
            printf("%c", entry.file_name[i]);
        printf("', ");
        printf("Attribute: %03o (%s), Pointer: %d\n", entry.attribute, attribute_string(entry.attribute), entry.ptr);
    }
    fclose(file);
}

typedef enum fat8_fat_entry {
    FAT8_FAT_LAST_BASE = 0300, // 0xc0
    FAT8_FAT_RESERVED  = 0xFE,
    FAT8_FAT_FREE      = 0xFF
} fat8_fat_entry;
void analyze_fat_command(void* tmp)
{
    analyze_metadata* data = tmp;
    FILE *file = fopen(data->filename, "rb");
    if(!file){
        printf("Failed to open specified image file: %s\n", strerror(errno));
        return;
    }
    fseek(file, data->offset, SEEK_SET);
    for(int i = 0; i < 0xff; i++)
    {
        uint8_t entry = 0;
        fread(&entry, 1, 1, file);
        if(entry == FAT8_FAT_FREE)
            printf("Free cluster\n");
        else if(entry == FAT8_FAT_RESERVED)
            printf("Reserved cluster\n");
    }
    fclose(file);
}

typedef struct calculate_offset_metadata { 
    uint16_t sides_per_floppy, sectors_per_track, sector_size; 
    char* string; 
    bool base0;
} calculate_offset_metadata;
void calculate_offset_command(void *tmp) {
    calculate_offset_metadata* data = tmp;
    int cylinder = 0, head = 0, sector = 0;
    sscanf(data->string, "%d,%d,%d", &cylinder, &head, &sector);
    int logical_sector = ((cylinder * data->sides_per_floppy + head) * data->sectors_per_track) + (sector - !data->base0);
    int offset = logical_sector * data->sector_size;
    printf("Offset: %d (0x%08X)\n", offset, offset);
}

typedef struct calculate_chs_metadata {
    uint16_t sides_per_floppy, sectors_per_track, sector_size; 
    size_t offset;
    bool base0;
} calculate_chs_metadata;
void calculate_chs_command(void* tmp) {
    calculate_chs_metadata* data = tmp;
    int sectors_per_cylinder = data->sides_per_floppy * data->sectors_per_track;

    int cylinder = data->offset / sectors_per_cylinder;
    int temp = data->offset % sectors_per_cylinder;

    int head = temp / data->sectors_per_track;
    int sector = (temp % data->sectors_per_track) + !data->base0;
    printf("Output: C: %d, H: %d, S: %d\n", cylinder, head, sector);
}

int main(int argc, char** argv)
{
    aparse_arg analyze[] = {
        aparse_arg_string("filename", 0, 0, "Name of file to analyzer"),
        aparse_arg_number("offset", 0, sizeof(size_t), 0, "Suspected directory offset"),
        aparse_arg_end_marker
    };
    aparse_arg calculate_offset[] = {
        aparse_arg_number("sides/floppy",  0, sizeof(uint16_t), false, "Sides/floppy"),
        aparse_arg_number("sectors/track", 0, sizeof(uint16_t), false, "Sectors/track"),
        aparse_arg_number("bytes/sector",  0, sizeof(uint16_t), false, "Bytes/sector"),
        aparse_arg_string("chs", 0, 0, "CHS-coordinate. In format: \"<cylinder>,<head>,<sector>\""),
        aparse_arg_number("base0", 0, sizeof(bool), false, "If >0, sectors will start at base 0 otherwise, at base 1"),
        aparse_arg_end_marker
    };
    aparse_arg calculate_chs[] = {
        aparse_arg_number("sides/floppy",  0, sizeof(uint16_t), false, "Sides/floppy"),
        aparse_arg_number("sectors/track", 0, sizeof(uint16_t), false, "Sectors/track"),
        aparse_arg_number("bytes/sector",  0, sizeof(uint16_t), false, "Bytes/sector"),
        aparse_arg_number("offset",        0, sizeof(size_t),   false, "Offsets"),
        aparse_arg_number("base0",         0, sizeof(bool),     false, "If >0, sectors will start at 0-based indexing otherwise, using 1-based indexing"),
    };
    aparse_arg main_args[] = {
        aparse_arg_parser("command", (aparse_arg[]){
            aparse_arg_subparser("analyze-dir", analyze, analyze_dir_command, "Analyze FAT8 directory",             analyze_metadata, filename, offset),
            aparse_arg_subparser("analyze-fat", analyze, analyze_fat_command, "Analyze FAT8 File-Allocation-Table", analyze_metadata, filename, offset),
            aparse_arg_subparser_impl(
                "calculate", (aparse_arg[]){
                    aparse_arg_parser("subcommand", (aparse_arg[]){
                        aparse_arg_subparser(
                            "offset", calculate_offset, calculate_offset_command, "Calculate offset from given CHS coordinate",
                            calculate_offset_metadata, sides_per_floppy, sectors_per_track, sector_size, string, base0),
                        aparse_arg_subparser(
                            "chs", calculate_chs, calculate_chs_command, "Calculate CHS coordinate with given offset", 
                            calculate_chs_metadata, sides_per_floppy, sectors_per_track, sector_size, offset, base0),
                        aparse_arg_end_marker
                    })
                }, calculate_offset_command, 
                "Calculate anything with given subcommand", 0, 0
            ),
            aparse_arg_end_marker
        }),
        aparse_arg_end_marker
    };
    aparse_parse(argc, argv, main_args, "Tools to analyze suspected FAT8 image disk file");
    //FILE* file = fopen("example1.img", "rb");
    // int c, h, s;
    // offset_to_chs(0x25e00, &c, &h, &s);
    // printf("%d, %d, %d\n", c, h, s);
    //fclose(file);
    return 0;
}