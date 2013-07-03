#define BOOT_INFO_ADDR 0x90000
#ifdef DJGPP
#define main main
#else
#define main _main
#endif
#include "./include/io.h"
#include"./include/time.h"
#include "./include/fifo.h"
#include "./include/system.h"
#include "./include/VGA.h"
#include "./include/char.h"
#include "./include/sheet.h"
#include "./include/stdlib.h"
#include "./include/kernel.h"
struct BOOT_INFO
{
	char led,vmode;
	short scrnx,scrny,reserve;
	unsigned char * vram;
	unsigned long ext_mem_k;
};
extern void mem_init(long start, long end);
extern void trap_init();
extern void init_pic();
extern void init_keyboard(struct FIFO * fifo) ;
extern int mousedecode(struct MOUSE_DEC* mdec, unsigned char dat);
extern void init_mouse(struct FIFO * fifo, int data0, struct MOUSE_DEC* dec);
extern int sprintf(char * buf, const char *fmt, ...);
extern void printf(const char*fmt, ...);
struct BOOT_INFO boot_info;
unsigned long memory_end = 0;//�������е��ڴ��ֽ���
unsigned long buffer_memory_end = 0;//���ٻ�����ĩ�˵�ַ
unsigned long main_memory_start = 0;//���ڴ濪ʼ�ĵ�ַ
#define BCD_TO_BIN(val) ((val) = ((val)&15) + ((val)>>4)*10) 
#define CMOS_READ(addr)\
 ({ outb_p(0x80|addr, 0x70);inb_p(0x71);})
 struct tm time;
static void time_init(void)
{
	
	do{
		time.tm_sec = CMOS_READ(0);
		time.tm_min = CMOS_READ(2);
		time.tm_hour = CMOS_READ(4);
		time.tm_mday = CMOS_READ(7);
		time.tm_mon = CMOS_READ(8);
		time.tm_year = CMOS_READ(9);
	}while(time.tm_sec != CMOS_READ(0));
	
	BCD_TO_BIN(time.tm_sec);
	BCD_TO_BIN(time.tm_min);
	BCD_TO_BIN(time.tm_hour);
	BCD_TO_BIN(time.tm_mday);
	BCD_TO_BIN(time.tm_mon);
	BCD_TO_BIN(time.tm_year);
}
void memory_set();
struct FIFO fifo;
struct FIFO fifomouse;

struct SHEET* open_console(struct SHTCTL *ctl)
{
	struct SHEET *sht_cons = sheet_alloc(ctl);
	unsigned char *buf_cons = (unsigned char *)malloc(256 * 165);
	if(0 == buf_cons)
	{
		panic("open console failed");
	}
	sheet_setbuf(sht_cons, buf_cons, 256, 165, -10);
	make_window(buf_cons, 256 , 165, "console", 0);
	make_textbox(sht_cons, 8, 28, 240, 128, VGA_BLACK);
	return sht_cons;
}
extern int counter ;
void main()
{
	int buf[128];
	int bufmouse[128];
	
	//int x=0,y=0;
	unsigned char kbdus[128] =
	{
		0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
	  '9', '0', '-', '=', '\b',	/* Backspace */
	  '\t',			/* Tab */
	  'q', 'w', 'e', 'r',	/* 19 */
	  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
		0,			/* 29   - Control */
	  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
	 '\'', '`',   0,		/* Left shift */
	 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
	  'm', ',', '.', '/',   0,				/* Right shift */
	  '*',
		0,	/* Alt */
	  ' ',	/* Space bar */
		0,	/* Caps lock */
		0,	/* 59 - F1 key ... > */
		0,   0,   0,   0,   0,   0,   0,   0,
		0,	/* < ... F10 */
		0,	/* 69 - Num lock*/
		0,	/* Scroll Lock */
		0,	/* Home key */
		0,	/* Up Arrow */
		0,	/* Page Up */
	  '-',
		0,	/* Left Arrow */
		0,
		0,	/* Right Arrow */
	  '+',
		0,	/* 79 - End key*/
		0,	/* Down Arrow */
		0,	/* Page Down */
		0,	/* Insert Key */
		0,	/* Delete Key */
		0,   0,   0,
		0,	/* F11 Key */
		0,	/* F12 Key */
		0,	/* All other keys are undefined */
	};
	
	struct MOUSE_DEC mdec = {0};
	int mx,my;
	struct SHTCTL *shtctl;
	unsigned char mousebuf[256], *buf_back;
	
	struct SHEET* sht_back, *sht_mouse, *sht_cons;
	fifo_init(&fifo, buf, 128);
	fifo_init(&fifomouse, bufmouse, 128);
	//�����ڴ�
	memory_set();
	//��ʼ�������ж�
	trap_init();
	//��ʼ��pic
	init_pic();
	time_init();
	//���������ж�
	init_keyboard(&fifo);
	//��������ж�
	init_mouse(&fifo, 256, &mdec);
	//��ʼ�����������
	init_pit();
	init_palette();
	shtctl = shtctl_init(boot_info.vram, boot_info.scrnx, boot_info.scrny);
	if(shtctl == 0)
	{
		printf("sheet init failed!");
		for(;;);
	}	
	
	//����ͼ��
	sht_back = sheet_alloc(shtctl);
	buf_back = (unsigned char * )malloc(boot_info.scrnx * boot_info.scrny);
	init_screen(buf_back, boot_info.scrnx, boot_info.scrny);
	sheet_setbuf(sht_back, buf_back, boot_info.scrnx, boot_info.scrny,-1);
	sheet_slide(shtctl, sht_back, 0, 0);
	//���ͼ��
	sht_mouse = sheet_alloc(shtctl);
	init_mouse_cursor(mousebuf, 99);
	sheet_setbuf(sht_mouse, mousebuf, 16, 16, 99);
	mx = (boot_info.scrnx - 16) / 2;
	my = (boot_info.scrny - 28 - 16) / 2;
	sheet_slide(shtctl, sht_mouse, mx, my);
	
	sht_cons = open_console(shtctl);
	sheet_slide(shtctl, sht_cons, 56,  6);
	
	sheet_updown(shtctl, sht_back,  0);
	//sheet_updown(sht_win, 3);
	sheet_updown(shtctl, sht_cons, 1);
	sheet_updown(shtctl, sht_mouse, 2);
	
	show_time(shtctl,sht_back);
	sti();
	
	for(;;)
	{
		cli();
		if(counter%100000 == 0)
		{
			time.tm_min++;
			show_time(shtctl,sht_back);
			
		}
		if((fifo_status(&fifo) +fifo_status(&fifomouse))  == 0)
		{	
		//sti();
			stihlt();
		}else 
		{
			int data;
			
			data = fifo_get(&fifo);
			sti();
			if(data < 256)
				draw_char(boot_info.vram, boot_info.scrnx, VGA_WHITE, 0, 0, kbdus[data]);
			else 
			{
				data -= 256;
				if(mousedecode(&mdec, data) == 1)	
				{
					//fill_rectangle(boot_info.vram, boot_info.scrnx, VGA_BLUE, mx, my, mx + 15, my + 15);
					mx += mdec.x;
					my += mdec.y;
					mx = (mx >= 0 ? ((mx <= boot_info.scrnx - 16) ? mx:boot_info.scrnx - 16) : 0);
					my = (my >= 0 ? ((my <= boot_info.scrny - 16) ? my:boot_info.scrny - 16) : 0);
					sheet_slide(shtctl, sht_mouse, mx, my);
					
				}				
				
					
			
				
			}
		}
		
	}
	//__asm__ __volatile__("hlt");
}

void memory_set()
{
//���������ǵĽ�����ʼ��һ���ֵ�ַ���ⲿ�ֵ�ַ��ΪԶ��16M�ڴ��ˣ�����ò����ȫ���ÿ����ˡ�
	boot_info = *((struct BOOT_INFO*)BOOT_INFO_ADDR);
	memory_end = (unsigned long)(1 << 20) + (((unsigned long)boot_info.ext_mem_k) << 10);//�ڴ��С=1M + ��չ�ڴ棨KB��*1024
	memory_end &= 0xfffff000;						//���Բ���4k���ڴ���
	if(memory_end > 16 * 1024 * 1024)				//��ʱ����ڴ泬��16M������16M����
		memory_end = 16 * 1024 * 1024;
	if(memory_end > 12 * 1024 * 1024)				//����ڴ� > 12M �����û�����ĩ��=4M		
		buffer_memory_end =  4 * 1024 * 1024;
	else if(memory_end  > 6 * 1024 * 1024)          //�������6M������2M
		buffer_memory_end = 2 * 1024 * 1024;
	else											//��������1M	
		buffer_memory_end = 1 * 1024 * 1024; 
	
	main_memory_start = buffer_memory_end;			//���ڴ�����ʼλ��=������ĩ��
	mem_init(main_memory_start, memory_end);
}
#include "./include/stdarg.h"
extern int vsprintf(char *buf, const char *fmt, va_list args);
 extern void draw_string_print(unsigned char* vram, int xsize, char color, int posx, int posy, char*str);
void printf(const char*fmt, ...)
{
	char buf[100];
	va_list args;

	va_start(args, fmt);
	vsprintf(buf,fmt,args);
	va_end(args);
	draw_string_print(boot_info.vram, boot_info.scrnx, VGA_WHITE, 0, 0, buf);
}

void show_time(struct SHTCTL* ctl,struct SHEET*sht)
{

	char tm[20];
	sprintf(tm,"%04d/%02d/%02d  %02d:%02d",time.tm_year,time.tm_mon, time.tm_wday,time.tm_hour,time.tm_min);
	draw_string(sht->buf,sht->bxsize,VGA_BLACK, 5, sht->bysize - 20, tm);
	sheet_refresh(ctl);
	
	
}


