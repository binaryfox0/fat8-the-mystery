# fat8-the-mystery
**fat8-the-mystery** is a research and reverse engineering project aimed at the forgotten FAT8 filesystem a primitive predecessor to the more well-known FAT12/FAT16 filesystems used in early DOS system

## What is FAT8?
**FAT8** is a informal name given to the early filesystem format used on some 1970s and early 1980s computer. It predates the standardize FAT12 structure and is characterized by:

- 8-bit wide FAT entry
- 16 bytes directory entry[¹](#references)

## Tools status
> [!IMPORTANT]
> The `calculate` command only works with CHS track, not the linear track like in the strings from TDISKBASIC. [See below](#mapped-files). Each linear track of that file have the size of 0x1000 bytes (4096)

## Mapped Files
> [!IMPORTANT]  
> These images are customized for this PC’s hardware and software configuration.      

**Status**: Currently analyzing on **Toshiba T100 Disk BASIC**   
The following files have been partially mapped, with known structures and offsets documented
- [Toshiba T100 Disk BASIC](./docs/TDISKBASIC.md)
- [Alphatronic PC](./docs/Alphatronic.md)

## Contributions
If you found something like new image, please submit it through [Discussion](https://github.com/binaryfox0/fat8-the-mystery/discussions)