INITSEG  EQU 0x9000
SETUPSEG EQU 0x9020
SYSSEG   EQU 0x1000

LEDS	EQU		0x0001			;����״̬
VMODE	EQU		0x0002			; ���ɫλ��
SCRNX	EQU		0x0004			; x�ֱ���
SCRNY	EQU		0x0006			; y�ֱ���
VRAM	EQU		0x0008			; �����ַ
VBEMODE EQU		0x105
[bits 16]		
;�������Ȱѽ�������Ϊ1024*768���󲿷ֻ�����֧�֣������֧����������Ϊ320*200
		mov ax,INITSEG
		mov ds, ax
		mov ax,0x2800
		mov es,ax;�����ַ
		mov di,0;����Ƿ�֧��vesa
		mov ax,0x4f00
		int 0x10
		cmp al,0x4f
		jne scrn320
		
		mov ax,[es:di + 4];�жϰ汾
		cmp ax,0x200
		jb scrn320
		;�±��Ǳ�׼��bios����vesa�Ĺ��ܣ������http://community.osdev.info/index.php?VESA
		mov cx,VBEMODE
		mov ax,0x4f01
		int 0x10
		cmp ax,0x004f;����0x4f��ʾ����
		jne scrn320
		
		cmp byte[es:di + 0x19],8;��ɫ�� 8λ
		jne scrn320
		cmp byte[es:di+0x1b],4;�����ǲ��ǵ�ɫ��
		jne scrn320
		mov ax,[es:di+0x00];ģʽ���ԣ�bit7�������1�Ͳ��ð죨�����Ļ��Ͳ��ܼ���0x400��
		and ax,0x0080
		jz scrn320
		
		mov bx, VBEMODE+0x4000;1024*768
		mov ax,0x4f02
		int 0x10
		
		mov byte[VMODE],8
		mov ax,[es:di+0x12];x�ֱ���
		mov word[SCRNX],ax
		mov ax,[es:di+0x14]
		mov word[SCRNY],ax
		mov eax,[es:di+0x28]
		mov dword[VRAM],eax
		jmp keyboard
		
		
		
scrn320:
		MOV		AL,0x13			; 
		MOV		AH,0x00
		INT		0x10
		MOV		BYTE [VMODE],8	
		MOV		WORD [SCRNX],320		
		MOV		WORD [SCRNY],200		
		MOV		DWORD [VRAM],0x000a0000
		


keyboard:
		MOV		ah,0x02
		INT		0x16 			; keyboard BIOS
		MOV		[LEDS],al
		
		;����pic�ж�
		MOV		al,0xff
		OUT		0x21,al
		NOP						; 
		OUT		0xa1,al
		CLI						; ���ÿ������ж� 
		
		;����A20�ܽ�,Ҳ���ǿ�����1M�����ڴ�
		CALL	waitkbdout
		MOV		AL,0xd1
		OUT		0x64,AL
		CALL	waitkbdout
		MOV		AL,0xdf			; enable A20
		OUT		0x60,AL
		CALL	waitkbdout
		
		

		lidt    [idt_48 + 512];��Ϊ����dsָ��0x9000�����ǵļ���ƫ����512
		LGDT	[GDTR0 + 512]			; 
		MOV		EAX,CR0
		AND		EAX,0x7fffffff	;��ֹ��ҳ
		OR		EAX,0x00000001	; ��������ģʽ
		MOV		CR0,EAX
	
		
	
move:;���ǰ�0x10000-0x8ffff��512k������ת�Ƶ���ַ��0����
		cli;�����Ƚ��� �ж�
		cld;�� ����
		mov ax,2*8
		mov ds,ax
		mov es,ax
		mov esi,0x10000
		mov edi,0x00000
		mov ecx ,0x80000/4
		a32 rep movsd
		
		

		JMP		dword 8:0x0000000;dword����ģ���ʾ�μ�ת��
		
waitkbdout:
		IN		 AL,0x64
		AND		 AL,0x02
		JNZ		waitkbdout		; 
		RET
		
		ALIGNB	16
GDT0:
		RESB	8	
		DW		0x07ff,0x0000,0x9a00,0x0040	; ��ִ��		; 
		DW		0xffff,0x0000,0x9200,0x00c0	; ����

		DW		0
idt_48:
		dw 0
		DD 0
GDTR0:
		DW		8*3-1
		DD		GDT0+512+0x90000

		ALIGNB	16

		
; 

		
		
		
		