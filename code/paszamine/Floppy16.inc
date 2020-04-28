;*******************************************************
;	Floppy16.inc
;		Floppy drive interface routines
;*******************************************************

%ifndef __FLOPPY16_INC_67343546FDCC56AAB872_INCLUDED__
%define __FLOPPY16_INC_67343546FDCC56AAB872_INCLUDED__

bits	16

bpbOEM					db "My OS   "
bpbBytesPerSector:  	DW 512
bpbSectorsPerCluster: 	DB 1
bpbReservedSectors: 	DW 1
bpbNumberOfFATs: 		DB 2
bpbRootEntries: 		DW 224
bpbTotalSectors: 		DW 2880
bpbMedia: 				DB 0xf0  ;; 0xF1
bpbSectorsPerFAT: 		DW 9
bpbSectorsPerTrack: 	DW 18
bpbHeadsPerCylinder: 	DW 2
bpbHiddenSectors: 		DD 0
bpbTotalSectorsBig:     DD 0
bsDriveNumber: 	        DB 0
bsUnused: 				DB 0
bsExtBootSignature: 	DB 0x29
bsSerialNumber:	        DD 0xa0a1a2a3
bsVolumeLabel: 	        DB "MOS FLOPPY "
bsFileSystem: 	        DB "FAT12   "

datasector  dw 0x0000
cluster     dw 0x0000

absoluteSector db 0x00
absoluteHead   db 0x00
absoluteTrack  db 0x00

%endif		;__FLOPPY16_INC_67343546FDCC56AAB872_INCLUDED__
