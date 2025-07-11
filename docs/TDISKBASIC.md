# Toshiba DISK BASIC
## Credits, Resources & Reference
- [1]. Special thanks to retrocomputing stackexchange user [**Simon Kissane**](https://retrocomputing.stackexchange.com/users/17803/simon-kissane) by creating this [answer](https://retrocomputing.stackexchange.com/a/27234)
- [2]. [Microsoft BASIC-80 version 5.0 Reference Manual (1979)](https://bitsavers.org/pdf/microsoft/cpm/Microsoft_BASIC-80_5.0_Reference_1979.pdf). [**Archived**](../resources/Microsoft_BASIC-80_5.0_Reference_1979.pdf):
  - [1] Section H.11: Disk Allocation Information ([page 165](https://bitsavers.org/pdf/microsoft/cpm/Microsoft_BASIC-80_5.0_Reference_1979.pdf#page=165))
## Information
- **Image file**: [ToshibaT100PersonalComputerTDISKBASICVersion101982.disk1of1.img](./../images/ToshibaT100PersonalComputerTDISKBASICVersion101982.disk1of1.img)
- **Original link**: https://archive.org/details/ToshibaT100PersonalComputerTDISKBASICVersion101982
- **Format**: Suspected FAT8[¹](#credits-resources--reference)

### Disk Geometry 
- **Sector size**: 256 bytes
- 8 sectors/cluster
- 2 clusters/track
- 35 tracks/side
- 2 sides/floppy
- **Evidence**: at offset 0x22CD0 ([more](#layout))

## Layout
When using `strings` program, a meaningful text appeared to be the disk layout at offset 0x22CD0
```
*** 8 sectors/cluster, 2 clusters/track, 35 tracks/side, 2 sides/floppy
*** track 0 not used because its single-density
*** tracks 1-7 hold IPL program and BASIC (if system disk)
*** tracks 8-17 hold user files
*** track 18 side 0 holds directory and file-allocation-table
*** track 18 side 1 holds user files
*** tracks 19-34 hold user files
```
Offsets of each section (in bytes):
- 0x00000000 - 0x00000FFF unused (aka NULL)
- 0x00001000 - 0x0000FFFF hold IPL program and BASIC (system disk only)
- 0x00010000 - 0x00023FFF hold user files
- 0x00024000 - 0x00024FFF holds directory and File-Allocation-Table
- 0x00025000 - 0x00025FFF holds user files
- 0x00026000 - 0x00045FFF hold user files
- 0x00046000 - 0x0004FFFF unused (aka NULL)
## Structure
### FAT8 directory
<details>
<summary>Hex view</summary>

```
00024000 46 44 55 54 49 4C 20 20 20 80 47 FF FF FF FF FF   FDUTIL   .G.....
00024010 46 4F 52 4D 41 54 20 20 20 80 49 FF FF FF FF FF   FORMAT   .I.....
00024020 56 4F 4C 43 4F 50 59 20 20 80 45 FF FF FF FF FF   VOLCOPY  .E.....
00024030 4D 45 4E 55 20 20 20 20 20 80 4B FF FF FF FF FF   MENU     .K.....
00024040 4E 45 4F 4E 20 20 20 20 20 80 41 FF FF FF FF FF   NEON     .A.....
00024050 4C 43 44 20 20 20 20 20 20 80 39 FF FF FF FF FF   LCD      .9.....
00024060 44 52 41 57 31 20 20 20 20 80 3E FF FF FF FF FF   DRAW1    .>.....
00024070 44 52 41 57 32 20 20 20 20 80 3D FF FF FF FF FF   DRAW2    .=.....
00024080 44 52 41 57 33 20 20 20 20 80 3A FF FF FF FF FF   DRAW3    .:.....
00024090 47 52 45 41 54 31 20 20 20 80 4C FF FF FF FF FF   GREAT1   .L.....
000240A0 42 55 53 31 20 20 20 20 20 80 4D FF FF FF FF FF   BUS1     .M.....
000240B0 47 52 41 50 48 31 20 20 20 80 36 FF FF FF FF FF   GRAPH1   .6.....
000240C0 00 45 53 54 50 52 20 20 20 80 35 FF FF FF FF FF   .ESTPR   .5.....
000240D0 50 49 43 31 20 20 20 20 20 80 4E FF FF FF FF FF   PIC1     .N.....
000240E0 47 41 4D 45 31 20 20 20 20 80 4F FF FF FF FF FF   GAME1    .O.....
```
</details>

The FAT8 directory starts exactly at 0x24000 and contains 15 used entries and 113 reserved entries  
"Each sector of the directory track contains eight file entries" [²˙¹](#credits-resources--reference), so inferred from that, each directory track will have 8 * 2 * 8 = 128 entries

### File Allocation Table
<details><summary>FAT 1st table</summary>

```
00024D00 FE FE FE FE FE FE FE FE FE FE FE FE FE FE FE FE   ................
00024D10 FE FE FE FE FE FE FE FE FE FE FE FE FE FE FE FE   ................
00024D20 FE FE FE FE FF FF FF FF FF FF FF FF FF FF FF FF   ................
00024D30 FF FF FF FF C7 FF C3 34 37 C8 C4 38 3B C4 C4 3C   .......47..8;..<
00024D40 C3 3F 40 C1 43 42 44 C5 FE 46 FE C3 C7 C6 C4 50   .?@.CBD..F.....P
00024D50 C4 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF   ................
00024D60 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF   ................
00024D70 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF   ................
00024D80 FF FF FF FF FF FF FF FF FF FF FF FF 73 6F 66 74   ............soft
00024D90 0D 0A 00 00 00 00 00 00 00 00 00 00 00 FF 00 00   ................
00024DA0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
00024DB0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
00024DC0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
00024DD0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
00024DE0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
00024DF0 00 00 00 00 00 00 00 00 00 00 00 1B E2 8D 0A 50   ...............P


```
</details>

The FAT table starts at offset 0x24d00 appeared to have 2 copies of itself at offset 0x24e00 and offset 0x24f00