;--------------------------------------------
bits	16
;---------------------------------------------
;6845 programming
;---------------------------------------------
%define	VGA_AC_INDEX		0x3C0
%define	VGA_AC_WRITE		0x3C0
%define	VGA_AC_READ			0x3C1
%define	VGA_MISC_WRITE		0x3C2
%define VGA_SEQ_INDEX		0x3C4
%define VGA_SEQ_DATA		0x3C5
%define	VGA_DAC_READ_INDEX	0x3C7
%define	VGA_DAC_WRITE_INDEX	0x3C8
%define	VGA_DAC_DATA		0x3C9
%define	VGA_MISC_READ		0x3CC
%define VGA_GC_INDEX 		0x3CE
%define VGA_GC_DATA 		0x3CF
			;COLOR emulation		MONO emulation */
%define VGA_CRTC_INDEX		0x3D4		;/* 0x3B4 */
%define VGA_CRTC_DATA		0x3D5		;/* 0x3B5 */
%define	VGA_INSTAT_READ		0x3DA

%define	VGA_NUM_SEQ_REGS	5
%define	VGA_NUM_CRTC_REGS	25
%define	VGA_NUM_GC_REGS		9
%define	VGA_NUM_AC_REGS		21


g_320x200x256 
			db	0x63;/* MISC */graphical
;/* SEQ */
			db	0x03, 0x01, 0x0F, 0x00, 0x0E
;/* CRTC */
			db	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F
			db	0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
			db	0x9C, 0x8E, 0x8F, 0x28,	0x40, 0x96, 0xB9, 0xA3
			db	0xFF,
;/* GC */
			db	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,0xFF
;/* AC */
			db	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
			db	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
			db	0x41, 0x00, 0x0F, 0x00, 0x00
			
g_40x25_text
			db	0x67	;/* MISC */text
;/* SEQ */
			db	0x03, 0x00, 0x03, 0x00, 0x02
;/* CRTC */
			db	0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F
			db	0x00, 0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x50
			db	0x9C, 0x0E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3
			db	0xFF
;/* GC */
			db	0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00
			db	0xFF
;/* AC */
			db	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07
			db	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F
			db	0x0C, 0x00, 0x0F, 0x08, 0x00

			
;---------------------------------------------
; read vga registers
;---------------------------------------------
read_vga_reg:
;/* read MISCELLANEOUS reg */
		mov		dx,VGA_MISC_READ	;inportb(VGA_MISC_READ)
		in		al,dx
		;call	print_byte
;/* read SEQUENCER regs */
		mov		cx,0
read_next_reg1:
		mov		dx,VGA_SEQ_INDEX
		mov		ax,cx
		out		dx,al	;outportb(VGA_SEQ_INDEX, i);
		mov		dx,VGA_SEQ_DATA
		in		al,dx	;inportb(VGA_SEQ_DATA);
		;call	print_byte
		inc		cx
		cmp		cx,VGA_NUM_SEQ_REGS
		jb		read_next_reg1
;/* read CRTC regs */
		mov		cx,0
read_next_reg2:
		mov		dx,VGA_CRTC_INDEX
		mov		ax,cx
		out		dx,al	;outportb(VGA_CRTC_INDEX, i);
		mov		dx,VGA_CRTC_DATA
		in		al,dx	;inportb(VGA_CRTC_DATA);
		;call	print_byte
		inc		cx
		cmp		cx,VGA_NUM_CRTC_REGS
		jb		read_next_reg2
;/* read GRAPHICS CONTROLLER regs */
		mov		cx,0
read_next_reg3:
		mov		dx,VGA_GC_INDEX
		mov		ax,cx
		out		dx,al	;outportb(VGA_GC_INDEX, i);
		mov		dx,VGA_GC_DATA
		in		al,dx	;inportb(VGA_GC_DATA);
		;call	print_byte
		inc		cx
		cmp		cx,VGA_NUM_GC_REGS
		jb		read_next_reg3
;/* read ATTRIBUTE CONTROLLER regs */
		mov		cx,0
read_next_reg4:
		mov		dx,VGA_INSTAT_READ
		in		al,dx	;(void)inportb(VGA_INSTAT_READ);
		mov		dx,VGA_AC_INDEX
		mov		ax,cx
		out		dx,al	;outportb(VGA_AC_INDEX, i);
		mov		dx,VGA_AC_READ
		in		al,dx	;inportb(VGA_AC_READ);
		;call	print_byte
		inc		cx
		cmp		cx,VGA_NUM_AC_REGS
		jb		read_next_reg4
;/* lock 16-color palette and unblank display */
		mov		dx,VGA_INSTAT_READ
		in		al,dx	;(void)inportb(VGA_INSTAT_READ);
		mov		dx,VGA_AC_INDEX
		mov		al,0x20
		out		dx,al	;outportb(VGA_AC_INDEX, 0x20);
		ret
;---------------------------------------------
; write vga registers
;---------------------------------------------
bits	32
write_vga_reg:
		mov		si,g_320x200x256
;/* write MISCELLANEOUS reg */
		mov		dx,VGA_MISC_WRITE
		mov		al,[si]
		out		dx,al
		inc		si
;/* write SEQUENCER regs */
		mov		cx,0
write_next_reg1:
		mov		dx,VGA_SEQ_INDEX
		mov		ax,cx
		out		dx,al
		mov		dx,VGA_SEQ_DATA
		mov		al,[si]
		out		dx,al
		inc		si
		inc		cx
		cmp		cx,VGA_NUM_SEQ_REGS
		jb		write_next_reg1
;/* unlock CRTC registers */
		mov		dx,VGA_CRTC_INDEX
		mov		al,0x03
		out		dx,al;outportb(VGA_CRTC_INDEX, 0x03);
		mov		dx,VGA_CRTC_DATA
		in		al,dx
		or		al,0x80
		out		dx,al;outportb(VGA_CRTC_DATA, inportb(VGA_CRTC_DATA) | 0x80);
		mov		dx,VGA_CRTC_INDEX
		mov		al,0x11
		out		dx,al;outportb(VGA_CRTC_INDEX, 0x11);
		mov		dx,VGA_CRTC_DATA
		in		al,dx
		and		al,0x7f
		out		dx,al;outportb(VGA_CRTC_DATA, inportb(VGA_CRTC_DATA) & ~0x80);
;/* make sure they remain unlocked */
		mov		al,[g_320x200x256+0x03]
		or		al,0x80
		mov		[g_320x200x256+0x03],al		;regs[0x03] |= 0x80;
		mov		al,[g_320x200x256+0x11]
		and		al,0x7f
		mov		[g_320x200x256+0x11],al		;regs[0x11] &= ~0x80;
;/* write CRTC regs */
		mov		cx,0
write_next_reg2:
		mov		dx,VGA_CRTC_INDEX	;outportb(VGA_CRTC_INDEX, i);
		mov		ax,cx
		out		dx,al
		mov		dx,VGA_CRTC_DATA	;outportb(VGA_CRTC_DATA, *regs);
		mov		al,[si]
		out		dx,al
		inc		si
		inc		cx
		cmp		cx,VGA_NUM_CRTC_REGS
		jb		write_next_reg2
;/* write GRAPHICS CONTROLLER regs */
		mov		cx,0
write_next_reg3:
		mov		dx,VGA_GC_INDEX
		mov		ax,cx
		out		dx,al
		mov		dx,VGA_GC_DATA
		mov		al,[si]
		out		dx,al
		inc		si
		inc		cx
		cmp		cx,VGA_NUM_GC_REGS
		jb		write_next_reg3
;/* write ATTRIBUTE CONTROLLER regs */
		mov		cx,0
write_next_reg4:
		mov		dx,VGA_INSTAT_READ
		in		al,dx	;(void)inportb(VGA_INSTAT_READ);
		mov		dx,VGA_AC_INDEX
		mov		ax,cx
		out		dx,al	;outportb(VGA_AC_INDEX, i);
		mov		dx,VGA_AC_WRITE
		mov		al,[si]
		out		dx,al	;outportb(VGA_AC_WRITE, *regs);
		inc		si
		inc		cx
		cmp		cx,VGA_NUM_AC_REGS
		jb		write_next_reg4
;/* lock 16-color palette and unblank display */
		mov		dx,VGA_INSTAT_READ
		in		al,dx	;(void)inportb(VGA_INSTAT_READ)
		mov		dx,VGA_AC_INDEX
		mov		al,0x20
		out		dx,al	;outportb(VGA_AC_INDEX, 0x20);
		ret
bits	16
		