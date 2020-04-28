;**************************************************
;**************************************************
;		LoadFile.inc
;		ES:BX points to data buffer
;**************************************************
%ifndef __Load_File_INC_67343546FDCC56AAB872_INCLUDED__
%define __Load_File_INC_67343546FDCC56AAB872_INCLUDED__
bits	16 
Load_File:  
LOAD_ROOT: 
; compute size of root directory and store in "cx" 
		push	cs
		pop		ax
		mov		ds,ax
		mov		es,ax
        xor     cx, cx
        xor     dx, dx
        mov     ax, 0x0020                           ; 32 byte directory entry
        mul     WORD [bpbRootEntries]                ; total size of directory
        div     WORD [bpbBytesPerSector]             ; sectors used by directory
        xchg    ax, cx
		

        
; compute location of root directory and store in "ax"    
        mov     al, BYTE [bpbNumberOfFATs]            ; number of FATs
        mul     WORD [bpbSectorsPerFAT]               ; sectors used by FATs
        add     ax, WORD [bpbReservedSectors]         ; adjust for bootsector
        mov     WORD [datasector], ax                 ; base of root directory
        add     WORD [datasector], cx
		  
; read root directory into memory (0500:1000)    
        mov     bx, 0x1000                            ; copy root dir above bootcode
        call    ReadSectors	
     ;----------------------------------------------------
     ; Find stage 2
     ;----------------------------------------------------
     ; browse root directory for binary image
          mov     cx, WORD [bpbRootEntries]             ; load loop counter
          mov     di, 0x1000                            ; locate first root entry
     .LOOP:
          push    cx
          mov     cx, 0x000B                            ; eleven character name
          mov     si, ImageName                         ; image name to find
          push    di
     rep  cmpsb                                         ; test for entry match
          pop     di
          je      LOAD_FAT
          pop     cx
          add     di, 0x0020                            ; queue next directory entry
          loop    .LOOP
          jmp     FAILURE


		  
     ;----------------------------------------------------
     ; Load FAT
     ;----------------------------------------------------
     LOAD_FAT:
			pop		dx
			mov		si,msgFoundImage
			call	Print
     ; save starting cluster of boot image   
          mov     dx, WORD [di + 0x001A]
          mov     WORD [cluster], dx                  ; file's first cluster
          
     ; compute size of FAT and store in "cx"    
          xor     ax, ax
          mov     al, BYTE [bpbNumberOfFATs]          ; number of FATs
          mul     WORD [bpbSectorsPerFAT]             ; sectors used by FATs
          mov     cx, ax

     ; compute location of FAT and store in "ax"
          mov     ax, WORD [bpbReservedSectors]       ; adjust for bootsector
          
     ; read FAT into memory (0050:1000)
          mov     bx, 0x1000                          ; copy FAT above bootcode		
          call    ReadSectors
     ; read image file into memory (0050:2C00)   
		mov		es,[Load_Segment]
        mov     bx, [Load_Offset]                          ; destination for image
        ;push    bx
		mov		[temp],bx
;----------------------------------------------------
; Load Stage 2
;----------------------------------------------------
LOAD_IMAGE: 
          mov     ax, WORD [cluster]                  ; cluster to read
          ;pop     bx                                  ; buffer to read into
          mov	bx,[temp]
		  call    ClusterLBA                          ; convert cluster to LBA
          xor     cx, cx
          mov     cl, BYTE [bpbSectorsPerCluster]     ; sectors to read
          call    ReadSectors
          ;push    bx
          mov	[temp],bx
; compute next cluster
          mov     ax, WORD [cluster]                  ; identify current cluster
          mov     cx, ax                              ; copy current cluster
          mov     dx, ax                              ; copy current cluster
          shr     dx, 0x0001                          ; divide by two
          add     cx, dx                              ; sum for (3/2)
          mov     bx, 0x1000                          ; location of FAT in memory
          add     bx, cx                              ; index into FAT
          mov     dx, WORD [bx]                       ; read two bytes from FAT
          test    ax, 0x0001
          jnz     .ODD_CLUSTER
          
.EVEN_CLUSTER:
          and     dx, 0000111111111111b               ; take low twelve bits
         jmp     .DONE
         
.ODD_CLUSTER:
          shr     dx, 0x0004                          ; take high twelve bits
          
.DONE:
          mov     WORD [cluster], dx                  ; store new cluster
          cmp     dx, 0x0FF0                          ; test for end of file
          jb      LOAD_IMAGE
          
DONE:
        mov     si, msgImageLoaded
        call    Print
		ret

	  
FAILURE:    
        mov     si, msgFailure
        call    Print
		ret	


	ret_segment		dw 0
	ret_offset		dw 0	
     msgCRLF     	db 0x0D, 0x0A, 0x00
     msgProgress 	db ".", 0x00
     msgFailure  	db 0x0D, 0x0A, " MISSING OR CURRUPT KRNLDR. Press Any Key to exit", 0x0D, 0x0A, 0x00
	 msgWlcome	 	db " Amirkabir University of Technology", 0x0D, 0x0A, 0x00
	 msgLoadImage 	db " Loading image...", 0x0D, 0x0A, 0x00
	 msgFoundImage 	db 0x0D, 0x0A, " Found BMP image file...",0X0D, 0X0A, " Loading...",0X00
	 msgImageLoaded db 0x0D, 0x0A, " Image loaded.",0X0D, 0X0A,0X00
	 FileSeg		dw 0
	 temp			dw 0
	 
	 
;************************************************;
;	Prints a string
;	DS=>SI: 0 terminated string
;************************************************;
Print:
	push	cs
	pop		ds
	lodsb						; load next byte from string from SI to AL
	or	al, al					; Does AL=0?
	jz	PrintDone				; Yep, null terminator found-bail out
	mov	ah, 0eh					; Nope-Print the character
	int	10h
	jmp	Print					; Repeat until null terminator found
PrintDone:
	ret						; we are done, so return

;************************************************;
; Convert CHS to LBA
; LBA = (cluster - 2) * sectors per cluster
;************************************************;

ClusterLBA:
          sub     ax, 0x0002                          ; zero base cluster number
          xor     cx, cx
          mov     cl, BYTE [bpbSectorsPerCluster]     ; convert byte to word
          mul     cx
          add     ax, WORD [datasector]               ; base data sector
          ret

;************************************************;
; Convert LBA to CHS
; AX=>LBA Address to convert
;
; absolute sector = (logical sector / sectors per track) + 1
; absolute head   = (logical sector / sectors per track) MOD number of heads
; absolute track  = logical sector / (sectors per track * number of heads)
;
;************************************************;

LBACHS:
          xor     dx, dx                              ; prepare dx:ax for operation
          div     WORD [bpbSectorsPerTrack]           ; calculate
          inc     dl                                  ; adjust for sector 0
          mov     BYTE [absoluteSector], dl
          xor     dx, dx                              ; prepare dx:ax for operation
          div     WORD [bpbHeadsPerCylinder]          ; calculate
          mov     BYTE [absoluteHead], dl
          mov     BYTE [absoluteTrack], al
          ret


;************************************************;
; Reads a series of sectors
; CX=>Number of sectors to read
; AX=>Starting sector
; ES:BX=>Buffer to read to
;************************************************;

ReadSectors:
     .MAIN
          mov     di, 0x0005                          ; five retries for error
     .SECTORLOOP
          push    ax
          push    bx
          push    cx
          call    LBACHS                              ; convert starting sector to CHS
          mov     ah, 0x02                            ; BIOS read sector
          mov     al, 0x01                            ; read one sector
          mov     ch, BYTE [absoluteTrack]            ; track
          mov     cl, BYTE [absoluteSector]           ; sector
          mov     dh, BYTE [absoluteHead]             ; head
          mov     dl, BYTE [bsDriveNumber]            ; drive
          int     0x13                                ; invoke BIOS
          jnc     .SUCCESS                            ; test for read error
          xor     ax, ax                              ; BIOS reset disk
          int     0x13                                ; invoke BIOS
          dec     di                                  ; decrement error counter
          pop     cx
          pop     bx
          pop     ax
          jnz     .SECTORLOOP                         ; attempt to read again
          int     0x18
     .SUCCESS
          pop     cx
          pop     bx
          pop     ax
          add     bx, WORD [bpbBytesPerSector]        ; queue next buffer
          inc     ax                                  ; queue next sector
          loop    .MAIN                               ; read next sector
          ret			
%endif
