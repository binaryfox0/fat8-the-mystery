# Alphatronic
## Important
For this file I will use 1-based sector indexing, completely different than the previous file
## Information
- **Original file**: [alpha4.imd](./../images/alpha4.imd) (**ImageDisk format**)
- **Original file link**: https://winworldpc.com/download/7922cb9c-c2aa-c592-7311-c3a5c28f1352
- **Extracted image**: [alpha4-extracted.img](./../images/alpha4-extracted.img)
  - Extracted using cli `disk-analyse alpha4.imd alpha4-extracted.img`
### Disk Geometry
**Note: Guess work**  
By using my [tool](../script/imd-geometry.py), i was able to extract the disk geometry (maybe that not accurate though)
- **Sector size**: 256 bytes
- 16 sectors/track
- 40 cylinders/side
- 2 sides/floppy

## Layout
This file isn't easy anymore for me (cuz no one has analyzed it yet), but by using a hex editor, i can still see some characteristics of FAT8 filesystem

- track 18 side 1 holds directory and file-allocation-table
## Structure
### FAT8 directory
<details>
<summary>Hex view</summary>

```
00025000 46 4F 52 4D 41 54 20 20 20 80 48 FF FF FF FF FF   FORMAT   .H.....
00025010 53 59 53 43 4F 50 20 20 20 80 49 FF FF FF FF FF   SYSCOP   .I.....
00025020 44 49 53 43 4F 50 20 20 20 80 45 FF FF FF FF FF   DISCOP   .E.....
00025030 50 43 44 55 4D 50 20 20 20 80 4C FF FF FF FF FF   PCDUMP   .L.....
```
</details>
The FAT8 directory starts exactly at 0x24000 (C:18,H:0,S:0) and contains 15 used entries and 113 reserved entries  

### File Allocation Table
<details><summary>Hex view</summary>

```
00025D00 FE FE FE FE FE FE FE FE FE FE FE FE FE FE FE FE   ................
00025D10 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF   ................
00025D20 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF   ................
00025D30 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF   ................
00025D40 C3 FF 40 C4 C4 42 43 44 47 46 FE FE 4D C5 FF FF   ..@..BCDGF..M...
00025D50 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF   ................
00025D60 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF   ................
00025D70 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF   ................
00025D80 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF   ................
00025D90 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF   ................
00025DA0 65 73 28 00 4C 4D 00 01                           es(.LM..
```

The FAT table starts at offset 0x25d00 (C:18,H:1,S:13), and there are two copies of itself at offset 0x25e00 (C:18,H:1,S:14) and offset 0x25f00 (C:18,H:1,S:15)
</details>