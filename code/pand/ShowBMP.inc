;---------------------------------------
;Hossein
;---------------------------------------
bits 	32
pkey 			db "press any key...$"
HeadBuff        times 54 db 'H'
;palBuff         times 1024 db'P'
;ScrLine         times 320 db 0
BMPStart        db 'BM'
PalSize         dw 0
BMPHeight       dw 0
BMPWidth        dw 0
msgInvBMP       db "Not a valid BMP file.",0Dh,0Ah,0
msgFileErr      db "Error opening file.",0Dh,0Ah,0 
filename        db "A:\P1.BMP", 0  
handle          dw 0
FilePosition	dw 0
bits	32
disp_BMP:
		mov		ebx,0x10000
		call	CheckValid
		jc		InvalidBMP
		call	write_vga_reg
		
	mov edi,0A0000h
	mov	dl,3
	mov	ecx,320*200
next_dot:
	mov byte [edi],40
	inc	edi
loop next_dot		
		
		
		call    GetBMPInfo 
		call	WritePal
		call	LoadBMP
		jmp		procDone
		InvalidBMP:
		;write is not a valid bmp file
		mov		ebx,msgInvBMP
		call	Puts32
		procDone:
		ret
		
;---------------------------------------
;ReadHeader Function()
;---------------------------------------
CheckValid:
		
		mov		ax,[0x10000]
		mov		dx,[BMPStart]
		cmp		ax,dx
		jne		NotValid
		jmp		CVdone	
NotValid:
		stc
		ret
CVdone:
		clc		
		ret
;---------------------------------------
;GetBMPInfo Function()
;---------------------------------------
GetBMPInfo:
		mov		ax,[0x10000+0xa]
		sub		ax,54
		shr		ax,2
		mov     [PalSize],ax 
		mov     ax,[0x10000+12h]
		mov     [BMPWidth],ax 
		mov     ax,[0x10000+16h]
		mov     [BMPHeight],ax
		ret
;*******************************************
; WritePal Function()
;*******************************************
WritePal:
mov     cx,[PalSize]              ; CX = Number of colors in palette.
;shl     cx,2                    ; CX = Multiply by 4 to get size (in bytes)
mov		ebx,0x10000+54             ; of palette.
mov     dx,3c8h
mov     al,0                    ; We will start at 0.
out     dx,al
inc     dx                      ; DX = 3C9h.
sndLoop:
mov		al,[ebx+2]
shr		al,2
out     dx,al                   ; Send it.
mov     al,[ebx+1]               ; Get green value.
shr     al,2
out     dx,al                   ; Send it.
mov     al,[ebx]                 ; Get blue value.
shr     al,2
out     dx,al                   ; Send it.
add     ebx,4                    ; Point to next color.
                                ; (There is a null chr. after every color.)
loop    sndLoop
ret
;---------------------------------------
;LoadBMP Function()
;---------------------------------------
LoadBMP:
		mov		cx,320*200
		mov		esi,ebx
		mov		edi,0
ShowLoop:
		push    ecx
		mov     edi,ecx                   ; Make a copy of CX
		;add		edi,
		mov		ecx,320							; pixel on the desired screen line.
		add		edi,0xa0000
	rep	movsb
		pop		ecx
		sub		ecx,320
		jnz ShowLoop
		ret
bits	16
