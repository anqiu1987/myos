#define BOOT_INFO_ADDR 0x90000
#ifdef DJGPP
#define main main
#else
#define main _main
#endif
#include "./include/io.h"
#include"./include/time.h"
struct BOOT_INFO
{
	char led,vmode;
	short scrnx,scrny,reserve;
	char * vram;
	unsigned long ext_mem_k;
};
extern void mem_init(long start, long end);
void init_screen(char *vram, int xsize, int ysize);
void draw_char8(char *vram, int xsize, char color, int posx, int posy, char s);
extern void trap_init();
extern void kbd_init();
struct BOOT_INFO boot_info;
unsigned long memory_end = 0;//�������е��ڴ��ֽ���
unsigned long buffer_memory_end = 0;//���ٻ�����ĩ�˵�ַ
unsigned long main_memory_start = 0;//���ڴ濪ʼ�ĵ�ַ
#define CMOS_READ(addr)\
 ({ outb_p(0x80|addr, 0x70);inb_p(0x71);})
static void time_init(void)
{
	struct tm time;
	do{
		time.tm_sec = CMOS_READ(0);
		time.tm_min = CMOS_READ(2);
		time.tm_hour = CMOS_READ(4);
		time.tm_mday = CMOS_READ(7);
		time.tm_mon = CMOS_READ(8);
		time.tm_year = CMOS_READ(9);
	}while(time.tm_sec != CMOS_READ(0));
	
}
void main()
{
	int i = 0;
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
	trap_init();
	init_screen(boot_info.vram, boot_info.scrnx, boot_info.scrny);
	init_pic();
	time_init();
	kbd_init();
	for(;;);
}
