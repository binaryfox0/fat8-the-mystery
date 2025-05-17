#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#define FLOPPY_SECTOR_PER_TRACK 16
#define FLOPPY_SIDE_COUNT       2

#define FLOPPY_TRACK_SIZE   0x1000 // 4096
#define FLOPPY_CLUSTER_SIZE 0x800  // 2048
#define FLOPPY_SECTOR_SIZE  0x100  // 256

static inline int calculate_offset(int cylinder, int side, int sector) {
    int logical_sector = ((cylinder * FLOPPY_SIDE_COUNT + side) * FLOPPY_SECTOR_PER_TRACK) + sector;
    return logical_sector * FLOPPY_SECTOR_SIZE;
}

#pragma pack(push, 1)
typedef struct {
    char file_name[9];
    uint8_t attribute;
    uint8_t ptr;
    uint8_t reserved[5];
} fat8_file_entry;
#pragma pack(pop)

void parse_directory(FILE* file)
{
    fseek(file, 0x24000, SEEK_SET);
    for(int i = 0; i < 15; i++)
    {
        fat8_file_entry entry;
        fread(&entry, sizeof(entry), 1, file);
        
        for(int i = 0; i < 9; i++)
            printf("%c", entry.file_name[i]);
        printf(" %3o 0x%2X\n", entry.attribute, entry.ptr);
    }
}

int main()
{
    //FILE* file = fopen("example1.img", "rb");
        
    printf("%d\n", calculate_offset(18, 0, 14));
    //fclose(file);
    return 0;
}