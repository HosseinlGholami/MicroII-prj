;**************************************************
;Hossein Gholami
;**************************************************
;		 Main.asm
;		-main boot loader
;		- this code is loaded into memory from 0x500
;**************************************************
;User memory space: 0x500 - 0xA0000(638.75KB)
;stack memory: 0x9F000-0xA0000 (4KB)-SS=0x9F00,SP=0x1000
;image buffer space: > 0x10000	:ES=0x1000,Bx=0x00
bits	16
org		0x500
%define new_line 10
%define cret 13
start:	jmp 	main
bootdevice		db 0
ImageName   	db "GRAPHIC4BMP";"PICTURE BMP"
RM_msg1			db	new_line,cret," Operating mode: Real",new_line,cret,0
RM_msg2			db	" Preparing to load image file (Picture.bmp)...",new_line,cret,0
RM_msg3			db	new_line,cret," Press 'P' key to switch to protected mode.",new_line,cret,0
Load_Segment	dw 0x1000	;File will be loaded into 0x10000
Load_Offset		dw 0		;
%define			VIDMEM	0xB8000			; video memory
vector3			dd	0xFFFFFFFF
                        dd      0xFFFFFFFF
				 
;*********************************************
;	included parts
;*********************************************
%include "stdio.inc"
%include "stdio16.inc"
%include "Floppy16.inc"
%include "Load_File.inc"
%include "Gdt.inc"
%include "A20.inc"
%include "common.inc"
%include "set_vga.inc"
;*********************************************
;	Bootloader Entry Point
;*********************************************
main:
;---------------------------------------------
;adjust segment registers
;---------------------------------------------   
		cli	
		push	cs		;CS=0x500
		pop		ax
        mov     ds, ax	;DS=0x500
        mov     es, ax	;ES=0x500
;---------------------------------------------
; create stack
;---------------------------------------------
        mov     ax, 0x9F00
        mov     ss, ax
        mov     sp, 0x1000
		
		mov		ah,0
		mov		al,0x3
		int		10h
;---------------------------------------------
;
;---------------------------------------------
		call	_EnableA20
		call	InstallGDT
        sti
        mov  [bootdevice], dl
;---------------------------------------------
;show welcome message
;---------------------------------------------
		call	Init_Text
		call	ShowWellcome
		;call	Real_Mode_Message
		mov		si,RM_msg1
		call	Print16
		mov		si,RM_msg2
		call	Print16
		mov		word [Load_Segment],0x1000
		mov		word [Load_Offset],0x00
		call	Load_File
		mov		si,RM_msg3
		call	Print16	
;----------------------------
;wait for key press
		mov		ah,0x00
PM_wait_loop:
		int		16h
		cmp		al,'p'
		jne		PM_wait_loop
		mov		al,03h	;initializes graphical mode
		mov		ah,00h	;
		int		10h		;
		;----------------------
EnterStage3:
	xor		ax, ax             ; null segments
	mov		ds, ax
	cli	                           ; clear interrupts
	mov	eax, cr0                   ; set bit 0 in cr0--enter pmode
	or	eax, 1
	mov	cr0, eax

	jmp	CODE_DESC:Stage3                ; far jump to fix CS. Remember that the code selector is 0x8!


;******************************************************
;	ENTRY POINT FOR STAGE 3
;******************************************************

bits 32

BadImage db 0xa,0xa,0xa,0xa,"    Welcome to Protected Mode ...", 0x00
;BadImage db al, 0

Stage3:
	mov	ax, DATA_DESC		; set data segments to data selector (0x10)
	mov	ds, ax
	mov	ss, ax
	mov	es, ax
	mov	esp, 0x1000000		; stack begins from 90000h
;*******************************************
; Main Function()
;*******************************************
;disp:
;       mov            BadImage,0x50
;  	mov		ebx, 0x32333450
  	mov		ebx, BadImage


  	call	Puts32


	push ecx
	push dx
	mov dx,0x01
	mov ecx,0x000ffffff
loop_delay:
	loop loop_delay
	dec dx
	jnz loop_delay
	pop dx


	call	disp_BMP
	mov		ebx,0x10000
	call	Puts32	
	call	disp_BMP	
	mov edi,0A0000h
	mov	dl,180
	mov	ecx,320*100
next_dott:
	inc	edi
loop next_dott



		MOV		cx,320*200/8
		mov		esi,0x10000+54+0x400

next_mmx:
		MOVQ 	MM1,[vector3];
		MOVQ 	MM0,[esi];
		pmaddwd	MM1,MM0
		MOVQ	[esi],MM1
		
		emms
		add		esi,8
		loop	next_mmx
	pop ecx


	push ecx
	push dx
	mov dx,0x01
	mov ecx,0x0000ffff
loop_delay3:
	loop loop_delay3
	dec dx
	jnz loop_delay
	pop dx


	call	disp_BMP	

here:	
		jmp		here
%include "ShowBMP.inc"