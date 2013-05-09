INITSEG  EQU 0x9000
SETUPSEG EQU 0x9020
SYSSEG   EQU 0x1000

LEDS	EQU		0x9001			;����״̬
VMODE	EQU		0x9002			; ���ɫλ��
SCRNX	EQU		0x9004			; x�ֱ���
SCRNY	EQU		0x9006			; y�ֱ���
VRAM	EQU		0x9008			; �����ַ
VBEMODE EQU		0x105
		
;�������Ȱѽ�������Ϊ1024*768���󲿷ֻ�����֧�֣������֧����������Ϊ320*200
		mov ax,INITSEG
		mov ds, ax
		mov di,0;����Ƿ�֧��VESA
		mov ax,0x2800;���ǰѻ����ַ�ŵ�0x28000��
		mov es,ax;�����ַ
		mov ax, 0x4f00
		int 0x10
		cmp ax,0x4f;��֧�ֵĻ�������ת��300*200ģʽ 
		jne scrn320
		
		mov ax, [es:di + 4]
		cmp ax, 0x200
		jb scrn320
		
		;�����Ǳ�׼bios����VESA�Ĺ��ܣ������http://community.osdev.info/index.php?VESA
		mov cx, VBEMODE;�����ܲ���ʹ��1024*768ģʽ
		mov ax,0x4f01
		int 0x10
		cmp ax, 0x4f;����0x4f��ʾ����
		jne scrn320
		
		cmp		byte [es:di + 0x19],8;��ɫ���������ǲ���8λ��ɫ��
		jne 	scrn320
		cmp 	byte [es:di + 0x1b],4;�����ǲ��ǵ�ɫ��ģʽ
		jne 	scrn320
		mov 	ax,[es:di+0x00];ģʽ���ԣ�bit7�������1�Ͳ��ð죨�����Ļ��Ͳ��ܼ���0x400��
		and		ax,0x0080
		jz		scrn320
		
		mov 	bx,VBEMODE+0x4000;1024*768ģʽ
		mov 	ax,0x4f02
		int 	0x10
		
		mov 	byte[VMODE],8
		mov 	ax,[es:di+0x12];����x�ֱ���
		mov 	[SCRNX],ax
		mov 	ax,[es:di+0x14]
		mov 	[SCRNY],ax
		mov 	eax,[es:di+0x28]
		mov 	[VRAM],eax
		jmp 	move
scrn320:
		MOV		AL,0x13			; 
		MOV		AH,0x00
		INT		0x10
		MOV		BYTE [VMODE],8	; 
		MOV		WORD [SCRNX],320
		MOV		WORD [SCRNY],200
		MOV		DWORD [VRAM],0x000a0000

move:;���ǰ�0x10000-0x8ffff��512k������ת�Ƶ���ַ��0����
		cli;�����Ƚ��� �ն�
		mov ax,0x00
		cld;�� ����
do_move:
		mov es,ax
		add ax,0x1000
		cmp ax,0x9000
		jz end_move
		mov ds,ax
		mov cx,0x8000;����һ���ƶ�0x10000�����ݼ�64k
		rep movsw
		jmp do_move
end_move:
		mov ax,SETUPSEG
		mov ds,ax
keyboard:
		MOV		ah,0x02
		INT		0x16 			; keyboard BIOS
		MOV		[LEDS],al
		
		;����pic�ж�
		MOV		al,0xff
		OUT		0x21,al
		NOP						; 
		OUT		0xa1,al
		
		
		;����A20�ܽ�
		CALL	waitkbdout
		MOV		AL,0xd1
		OUT		0x64,AL
		CALL	waitkbdout
		MOV		AL,0xdf			; enable A20
		OUT		0x60,AL
		CALL	waitkbdout
		
		
waitkbdout:
		IN		 AL,0x64
		AND		 AL,0x02
		JNZ		waitkbdout		; 
		RET
		
		LGDT	[GDTR0]			; 
		MOV		EAX,CR0
		AND		EAX,0x7fffffff	;��ֹ��ҳ
		OR		EAX,0x00000001	; ��������ģʽ
		MOV		CR0,EAX
		
		JMP		dword 8:0x00000000;dword����ģ���ʾ�μ�ת��
		ALIGNB	16
GDT0:
		RESB	8	
		DW		0x07ff,0x0000,0x9a00,0x0040	; ��ִ��		; 
		DW		0x07ff,0x0000,0x9200,0x00c0	; ����
		

		DW		0
GDTR0:
		DW		8*3-1
		DD		GDT0

		ALIGNB	16

		
; 

		
		
		
		