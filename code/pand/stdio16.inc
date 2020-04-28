;**************************************************
;omid milani
;*************************************************
;	stdio16.inc
;	-Input/Output routines
;*************************************************
%ifndef __STDIO16_INC_67343546FDCC56AAB872_INCLUDED__
%define __STDIO16_INC_67343546FDCC56AAB872_INCLUDED__
bits	16
;----------------------------
%define	black			0x0
%define	blue			0x1
%define	green			0x2
%define	cyan			0x3
%define	red				0x4
%define	magenta			0x5
%define	brown			0x6
%define	light gray		0x7
%define	dark gray		0x8
%define	light_blue		0x9
%define	light_green		0xA
%define	light_cyan		0xB
%define	light_red		0xC
%define	light_magenta	0xD
%define	yellow			0xE
%define white			0xF
;----------------------------
%define vertical		0xBA
%define horizontal		0xCD
%define	top_left		0xC9
%define	top_right		0xBB
%define bottom_left		0xC8
%define bottom_right	0xBC
;----------------------------
%define	new_line		0xA
%define	cret			0xD
;------------------------------------------------
;Init_Text Routin
;- initializes video adaptor to text mod:80x25	
;-------------------------------------------------
Init_Text:
		push	ax
		mov		ah,0x0
		mov		al,0x3
		int		10h
		pop		ax
		ret
;-------------------------------------------------
;Init_Video Routin
;-initializes video adaptor to graphical mode:
;-320x200 pixel		
;-------------------------------------------------
Init_Video:
		push	ax
		mov		ah,0x0
		mov		al,0x13
		int		10h
		pop		ax
		ret
;-------------------------------------------------
;ShowWellcome Routin
;-shows welcome message:	
;-------------------------------------------------
ShowWellcome:
		jmp		ShowWellcome_Start
;----------------------------
SW_Top_Line		db	' ',top_left
	times 	76 	db 	horizontal
				db	top_right,' ',0
;----------------------------
SW_Bottom_Line	db	' ',bottom_left
	times 	76 	db 	horizontal
				db	bottom_right,' ',0
;----------------------------				
SW_Blank_Line	db ' ',vertical
	times	76	db ' '
				db vertical,' ',0
;----------------------------				
SW_Next_Line	db new_line,cret,0
;----------------------------				
SW_msg1			db ' ',vertical
	times	21	db ' ' 
				db	"*****DIGITAL ELECTRONIC AUT ******"
	times	21	db ' '
				db vertical,' ',0
;----------------------------
SW_Dep_S		db "                            "
SW_Dep_E
;----------------------------
SW_SName_S		db	"Hossein Gholami "
SW_SName_E
;----------------------------
SW_TName_S		db	"  D.r Motamedi "
SW_TName_E
;----------------------------
SW_CName_S		db	"        Micro II          "
SW_CName_E
;----------------------------
SW_key_msg		db " Press 'c' key to continue...",new_line,cret,0
;----------------------------

;----------------------------	
SW_Char_attrib	db	blue*16+white
;----------------------------
;
;----------------------------			
ShowWellcome_Start:
		pusha
		call	CLR_Screen
		mov		si,SW_Next_Line
		call	Print16		
		mov		si,SW_Top_Line
		call	Print16
		mov		si,SW_Blank_Line
		call	Print16
		mov		si,SW_msg1
		call	Print16
		mov		si,SW_Blank_Line
		call	Print16
		mov		si,SW_Bottom_Line
		call	Print16
		mov		si,SW_Top_Line
		call	Print16
;----------------------------
		mov		cx,16
		mov		si,SW_Blank_Line
SW_next_line:
		call	Print16
		loop	SW_next_line
;----------------------------		
		mov		si,SW_Bottom_Line
		call	Print16
;----------------------------		
		mov 	cx, SW_Dep_E - SW_Dep_S ; calculate message size.		
		mov 	dl, 2+(80-(SW_Dep_E - SW_Dep_S)-4)/2
		mov 	dh, 9	;row
		mov 	bp, SW_Dep_S	
		call	Write_String
;----------------------------		
		mov 	cx, SW_SName_E - SW_SName_S ; calculate message size.		
		mov 	dl, 2+(80-(SW_SName_E - SW_SName_S)-4)/2
		mov 	dh, 15	;row
		mov 	bp, SW_SName_S	
		call	Write_String
;----------------------------
		mov 	cx, SW_TName_E - SW_TName_S ; calculate message size.		
		mov 	dl, 2+(80-(SW_TName_E - SW_TName_S)-4)/2
		mov 	dh, 13	;row
		mov 	bp, SW_TName_S	
		call	Write_String
;----------------------------
		mov 	cx, SW_CName_E - SW_CName_S ; calculate message size.		
		mov 	dl, 2+(80-(SW_CName_E - SW_CName_S)-4)/2
		mov 	dh, 11	;row
		mov 	bp, SW_CName_S	
		call	Write_String
;----------------------------
;		mov 	cx, SW_EName_E - SW_EName_S ; calculate message size.		;
;		mov 	dl, 2+(80-(SW_EName_E - SW_EName_S)-4)/2
;		mov 	dh, 17	;row
;		mov 	bp, SW_EName_S	
;		call	Write_String
;----------------------------
;goto last line
		mov dh, 24	;row
		mov dl, 1	;column
		mov bh, 0	;page
		mov ah, 2	
		int 10h
;----------------------------			
		mov		si,SW_key_msg
		call	Print16
;----------------------------
;wait for key press
wait_loop:
		mov		ah,0x00
		int		16h
		cmp		al,'c'
		jne		wait_loop
;----------------------------
		call	Init_Text
		call	CLR_Screen
		popa
		ret
;-------------------------------------------------
;Print16 Routin
;-shows welcome message:	
;-------------------------------------------------
Print16:
		pusha
		mov		ah,0eh
Print16_Loop:
		mov		al,[si]
		or		al,al
		jz		Print16_End
		int		10h
		inc		si
		jmp		Print16_Loop
Print16_End:		
		popa
		ret
;-------------------------------------------------
;CLR_Screen Routin
;-clears the screen:	
;-------------------------------------------------		
CLR_Screen:
%define	VIDMEM	0xB800			; video memory
		pusha
		mov		ax,VIDMEM
		mov		es,ax
		mov		dh,[SW_Char_attrib]
		mov		dl,' '
		xor		si,si
		mov		cx,80*40
CLR_Loop:
		mov		[es:si],dx
		inc		si
		inc		si
		loop	CLR_Loop
		popa
		ret
;-------------------------------------------------
;Write_String Routin
;-clears the screen:	
;-------------------------------------------------	
Write_String:
		pusha		
		mov 	al, 1	;string contains attributes.
		mov 	bh, 0	;page number
		mov		bl,[SW_Char_attrib]
		push 	cs
		pop 	es
		mov 	ah, 13h
		int 	10h
		popa
		ret
%endif 