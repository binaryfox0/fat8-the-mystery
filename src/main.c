#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "aparse.h"
#include "fat8.h"

#define min(a, b) ((a < b) ? (a) : (b))
#define max(a, b) ((a > b) ? (a) : (b))

#define FLOPPY_SECTOR_PER_TRACK 16
#define FLOPPY_SIDE_COUNT       2

#define FLOPPY_TRACK_SIZE   0x1000 // 4096
#define FLOPPY_CLUSTER_SIZE 0x800  // 2048
#define FLOPPY_SECTOR_SIZE  0x100  // 256


char* attribute_string(const uint8_t attribute)
{
    static char buffer[32];
    int index = 0;
    memset(buffer, 0, sizeof(buffer));

    struct {
        uint8_t flag;
        const char *name;
    } attribs[] = {
        { FAT8_FILE_ATTRIB_BINARY, "BINARY" },
        { FAT8_FILE_ATTRIB_FRAWC,  "FRAWC"  },
        { FAT8_FILE_ATTRIB_EBCDIC, "EBCDIC" },
        { FAT8_FILE_ATTRIB_WP,     "WP"     }
    };

    for (size_t i = 0; i < sizeof(attribs) / sizeof(attribs[0]); ++i) {
        if (attribute & attribs[i].flag) {
            if (index > 0) {
                strcat(buffer, ",");
                index++;
            }
            strcat(buffer, attribs[i].name);
            index += strlen(attribs[i].name);
        }
    }

    return buffer;
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

void analyze_fat_command(void* tmp)
{
    analyze_metadata* data = (analyze_metadata*)tmp;
    FILE *file = fopen(data->filename, "rb");
    if (!file) {
        printf("Failed to open image file '%s': %s\n", data->filename, strerror(errno));
        return;
    }

    fseek(file, data->offset, SEEK_SET);

    uint8_t fat[255] = {0};
    if (fread(fat, 1, sizeof(fat), file) != sizeof(fat)) {
        printf("Failed to read FAT table.\n");
        fclose(file);
        return;
    }

    for (int i = 0; i < 255; i++) {
        uint8_t entry = fat[i];

        printf("FAT[0x%02X]: ", i);

        if (entry == FAT8_FAT_FREE) {
            printf("free\n");
        } else if (entry == FAT8_FAT_RESERVED) {
            printf("reserved\n");
        } else if (entry >= FAT8_FAT_EOF_BASE && entry < FAT8_FAT_RESERVED) {
            printf("EOF marker (0x%02X, %d sectors)\n", entry, entry - FAT8_FAT_EOF_BASE);
        } else if (entry == 0xFF) {
            printf("invalid (0xFF)\n");
        } else {
            // Follow FAT chain
            bool visited[255] = {0};
            uint8_t current = i;
            int chain_len = 0;

            while (true) {
                if (visited[current]) {
                    printf("0x%02X (loop detected)\n", current);
                    break;
                }

                visited[current] = true;
                uint8_t next = fat[current];

                if (chain_len > 0) printf(" -> ");
                printf("0x%02X", current);
                chain_len++;

                if (next >= FAT8_FAT_EOF_BASE && next < FAT8_FAT_RESERVED) {
                    printf(" -> 0x%02X (EOF, %d sectors)\n", next, next - FAT8_FAT_EOF_BASE);
                    break;
                } else if (next == FAT8_FAT_RESERVED) {
                    printf(" -> 0x%02X (Reserved?)\n", next);
                    break;
                } else if (next == FAT8_FAT_FREE) {
                    printf(" -> 0x%02X (Free?)\n", next);
                    break;
                } else if (next == 0xFF) {
                    printf(" -> 0xFF (Invalid)\n");
                    break;
                } else {
                    current = next;
                }
            }

            if (chain_len == 1) {
                printf(" (singleton)\n");
            }
        }
    }

    fclose(file);
}

void hex_dump(uint8_t* bytes, uint32_t size, uint32_t offset)
{
    for (uint32_t i = 0; i < size; i += 16)
    {
        uint32_t line_size = (size - i >= 16) ? 16 : (size - i);
        printf("%08X  ", offset + i);
        for (uint32_t j = 0; j < 16; j++){
            if (j < line_size)
                printf("%02X ", bytes[i + j]);
            else
                printf("   "); 
        }
        printf(" ");
        for (uint32_t j = 0; j < line_size; j++) {
            uint8_t c = bytes[i + j];
            printf("%c", isprint(c) ? c : '.');
        }
        printf("\n");
    }
}


void analyze_file_block_command(void* tmp)
{
    analyze_metadata *data = tmp;
    FILE *file = fopen(data->filename, "rb");
    if(!file){
        printf("Failed to open specified image file: %s\n", strerror(errno));
        return;
    }
    fseek(file, data->offset, SEEK_SET);
    fat8_file_block fblk = {0};
    fread(&fblk, 1, sizeof(fblk), file);
    printf("File mode: 0o%04o\n", fblk.file_mode);
    printf("First cluster FAT index: 0x%02X\n", fblk.first_cluster_fat_index);
    printf("Last cluster FAT index: 0x%02X\n", fblk.last_cluster_fat_index);
    printf("Last sector accessed: 0x%02X\n", fblk.last_sector_accessed);
    printf("Disk number: %d\n", fblk.disk_number);
    printf("Last buffer size: 0x%02X bytes\n", fblk.last_buffer_size);
    printf("Buffer position: 0x%02X\n", fblk.buffer_position);
    printf("File flags: 0o%04o\n", fblk.file_flags);
    printf("Tab position: 0x%02X\n", fblk.tab_position);
    hex_dump(fblk.sector_buffer, 128, data->offset + offsetof(fat8_file_block, sector_buffer));
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
        aparse_arg_number("sides/floppy",  0, sizeof(uint16_t), 
            false, "Sides/floppy"),
        aparse_arg_number("sectors/track", 0, sizeof(uint16_t), 
            false, "Sectors/track"),
        aparse_arg_number("bytes/sector",  0, sizeof(uint16_t), 
            false, "Bytes/sector"),
        aparse_arg_number("offset",        0, sizeof(size_t),   
            false, "Offsets"),
        aparse_arg_number("base0",         0, sizeof(bool),     
            false, "If >0, sectors will start at 0-based indexing otherwise, using 1-based indexing"),
    };
    aparse_arg main_args[] = {
        aparse_arg_parser("command", (aparse_arg[]){
            aparse_arg_subparser_impl("analyze", (aparse_arg[]){
                aparse_arg_parser("subcommand", (aparse_arg[]){
                    aparse_arg_subparser("dir", analyze, 
                        analyze_dir_command, "Analyze FAT8 directory", analyze_metadata, filename, offset),
                    aparse_arg_subparser("fat", analyze, 
                        analyze_fat_command, "Analyze FAT8 File-Allocation-Table", analyze_metadata, filename, offset),
                    aparse_arg_subparser("fblk", analyze, 
                        analyze_file_block_command, "Analyze FAT8 file block", analyze_metadata, filename, offset),
                    aparse_arg_end_marker
                }),
                aparse_arg_end_marker
            }, analyze_file_block_command, "Analyze anything with given subcommand", 0, 0),
            aparse_arg_subparser_impl(
                "calculate", (aparse_arg[]){
                    aparse_arg_parser("subcommand", (aparse_arg[]){
                        aparse_arg_subparser(
                            "offset", calculate_offset, calculate_offset_command, 
                            "Calculate offset from given CHS coordinate",calculate_offset_metadata, 
                            sides_per_floppy, sectors_per_track, sector_size, string, base0),
                        aparse_arg_subparser(
                            "chs", calculate_chs, calculate_chs_command, 
                            "Calculate CHS coordinate with given offset", calculate_chs_metadata, 
                            sides_per_floppy, sectors_per_track, sector_size, offset, base0),
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