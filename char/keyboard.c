//������ַhttp://www.computer-engineering.org/ps2keyboard/
//http://wiki.osdev.org/PS2_Keyboard
#ifdef DJGPP
#define kb_interupt_asm kb_interupt_asm
#define do_kb_interupt  do_kb_interupt 
#else
#define kb_interupt_asm _kb_interupt_asm 
#define do_kb_interupt  _do_kb_interupt 
#endif
#include "./../include/head.h"
#include "./../include/system.h"
#include "./../include/io.h"
#include "./../include/fifo.h"
#include "./../include/char.h"
extern void kb_interupt_asm();
extern void IRQ_Clear_mask(unsigned char IRQline);
void draw_char(char *vram, int xsize, char color, int posx, int posy, char s);
#define IQR_KB 0x21
#define KB_DATA 0x0060 
#define KB_CMD 0x64
#define KEYCMD_WRITEMODE 0x60
#define kEYCMD_READMODE 0x20
#define KBC_MODE 0x47
struct FIFO* kbFIFO;
static unsigned int offset;
void init_keyboard(struct FIFO * fifo,unsigned int data) 
{
	char ch;
	kbFIFO = fifo; 
	offset = data;
	set_intr_gate(IQR_KB, kb_interupt_asm); 
	//����pic 1 �жϴ���
	IRQ_Clear_mask(1);
	//����̿�������������
	 wait_keyboard();
	 //outb_p(KEYCMD_WRITEMODE, KB_CMD);//ģʽ�趨
	 // wait_keyboard();
	 // outb_p(KBC_MODE, KB_DATA);//�������ģʽ
	 outb_p(kEYCMD_READMODE, KB_CMD);//��ȡ����״̬λ����
	 wait_keyboard();
	 ch = inb(KB_DATA);//��ȡ
	 ch |= (1 << 1);//����int 12���жϵ�
	 ch &= (~(1 << 4));//�������̿���
	
	wait_keyboard();
	outb_p(KEYCMD_WRITEMODE, KB_CMD);//д����������̿�����
	wait_keyboard();
	outb_p(ch, KB_DATA);//�����д
}
void do_kb_interupt()
{
	char data;
	PIC_sendEOI(1);
	//outb(0x61, 0x20);
	data = inb(KB_DATA);

	if(data & 0x80)
	{
	}
	else
	{
		fifo_put(kbFIFO, offset + data);
	}
	//draw_char(0xe0000000, 1024, 15, 0,0, data);
}
