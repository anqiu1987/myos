INITSEG  EQU 0x9000
SETUPSEG EQU 0x9020
SYSSEG   EQU 0x1000

LEDS	EQU		0x9001			;����״̬
VMODE	EQU		0x9002			; ���ɫλ��
SCRNX	EQU		0x9004			; x�ֱ���
SCRNY	EQU		0x9006			; y�ֱ���
VRAM	EQU		0x9008			; �����ַ
VBEMODE EQU		0x105
		
;�������Ȱ��ڽ�������Ϊ1024*768���󲿷ֻ�����֧�֣����ǾͲ�����������
		mov ax,INITSEG
		mov ds, ax
		mov di,0;����Ƿ�֧��VESA
		mov ax,0x2800
		mov es,ax;�����ַ
		mov ax, 0x4f00
		int 0x10
		cmp ax,0x4f;��֧�ֵĻ�������ת��300*200ģʽ 
		jne scrn320
		
		mov ax, [es:di + 4]
		cmp ax, 0x200
		jb scrn320
		
		mov cx, VBEMODE;�����ܲ���ʹ��1024*768ģʽ
		mov ax,0x4f01
		int 0x10
		cmp ax, 0x4f
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
		jmp 	keystatus
scrn320:
		MOV		AL,0x13			; 
		MOV		AH,0x00
		INT		0x10
		MOV		BYTE [VMODE],8	; 
		MOV		WORD [SCRNX],320
		MOV		WORD [SCRNY],200
		MOV		DWORD [VRAM],0x000a0000

; 
keystatus:
		
		
		
		