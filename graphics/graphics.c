//ͼ�α�̵���ַhttp://www.oocities.org/garyneal_71/OldPages/cGraphicsPalette.html
//�ú����������һ������
//http://www.brackeen.com/vga/basics.html
#include "./../include/VGA.h"
#include "./../include/system.h"
#include "./../include/io.h"
void fill_rectangle(unsigned char* vram, int xsize, char c, int srcx,int srcy, int destx, int desty)
{
	int x,y;
	for(y = srcy; y <= desty; y++)
		for(x = srcx; x <= destx; x++)
			vram[y * xsize + x] = c;
}

void copy_rectangle(unsigned char *vram, int xsize, int srcx, int srcy, int width, int height, char *block)
{
	int x, y;
	for(y = srcy; y < srcy + height; y++)
		for(x = srcx; x < srcx + width; x++)
			vram[y * xsize + x] = block[(y - srcy) * width + (x - srcx)];
}

void draw_char8(unsigned char *vram, int xsize, char color, int posx, int posy, char s)
{
	int i;
	unsigned char *p;
	char *hankaku = (char*)0x6000;
	char *font = hankaku + s * 16;
	for(i = 0; i < 16; i++)
	{
		p = &vram[(posy + i) * xsize + posx];
		if((font[i] & 0x80) != 0)
			 p[0] = color;
		if((font[i] & 0x40) != 0) p[1] = color;
		if((font[i] & 0x20) != 0) p[2] = color;
		if((font[i] & 0x10) != 0) p[3] = color;
		if((font[i] & 0x08) != 0) p[4] = color;
		if((font[i] & 0x04) != 0) p[5] = color;
		if((font[i] & 0x02) != 0) p[6] = color;
		if((font[i] & 0x01) != 0) p[7] = color;
	}
}
inline void draw_string(unsigned char* vram, int xsize, char color, int posx, int posy, char*str)
{
	for(;*str!=0;str++)
	{
		draw_char8(vram,xsize ,color,posx,posy , *str);
		posx +=8;
	}
}
inline void draw_string_print(unsigned char* vram, int xsize, char color, int posx, int posy, char*str)
{
	static int x,y;
	for(;*str!=0;str++)
	{
		draw_char8(vram, xsize, color, x, y , *str);
		x += 8;
	}
}
inline void draw_char(unsigned char *vram, int xsize, char color, int posx, int posy, char s)
{
	draw_char8(vram,xsize ,color,posx,posy , s);
}

void init_mouse_cursor(unsigned char *mouse, char bc)
{
	__asm__("cld"::);//������֮ǰû���巽��Ĵ���,����copy�ڴ��ʱ�����ǳ���
	char cursor[16][16] = {
		"**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
		"*OOOOOOOOO*.....",
		"*OOOOOOOO*......",
		"*OOOOOOO*.......",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOO**OOO*.....",
		"*OOO*..*OOO*....",
		"*OO*....*OOO*...",
		"*O*......*OOO*..",
		"**........*OOO*.",
		"*..........*OOO*",
		"............*OO*",
		".............***"
	};
	int x, y;

	for (y = 0; y < 16; y++) {
		for (x = 0; x < 16; x++) {
			
			if(cursor[y][x] == '*') {
				mouse[y * 16 + x] = VGA_BLACK;
			}
			else if(cursor[y][x] == 'O') {
				mouse[y * 16 + x] = VGA_WHITE;
			}
			else if(cursor[y][x] == '.') {
				mouse[y * 16 + x] = bc;
			}
		}
	}
}

void init_screen(unsigned char *vram, int xsize, int ysize)
{
	fill_rectangle(vram, xsize, VGA_BLUE, 0, 0, xsize - 1, ysize - 29);//�ϲ���ɫ
	fill_rectangle(vram, xsize, VGA_LIGHT_GRAY, 0, ysize - 28, xsize - 1, ysize - 28);//����ɫ
	fill_rectangle(vram, xsize, VGA_WHITE, 0, ysize - 27, xsize - 1, ysize - 27);//��ɫ
	fill_rectangle(vram, xsize, VGA_LIGHT_GRAY, 0, ysize - 26, xsize - 1, ysize - 1);//����ɫ
	//����ߵĿ��������İ�ť
	fill_rectangle(vram, xsize, VGA_WHITE, 3, ysize - 24, 59, ysize - 24);//��ɫ��
	fill_rectangle(vram, xsize, VGA_WHITE, 2, ysize - 24, 2,  ysize - 4);//��ɫ��
	fill_rectangle(vram, xsize, VGA_DARK_GRAY, 3, ysize - 4, 59,  ysize - 4);//����ɫ
	fill_rectangle(vram, xsize, VGA_DARK_GRAY, 59,         ysize - 23, 59,         ysize - 5);//����ɫ
	fill_rectangle(vram, xsize, VGA_BLACK,     2,          ysize - 3,  60,         ysize - 3);//��ɫ��
	fill_rectangle(vram, xsize, VGA_BLACK,     60,         ysize - 24, 60,         ysize - 3);//��ɫ��
	//���ұߵ���ʾʱ��İ���
	fill_rectangle(vram, xsize, VGA_DARK_GRAY, xsize - 47, ysize - 24, xsize -  4, ysize - 24);
	fill_rectangle(vram, xsize, VGA_DARK_GRAY, xsize - 47, ysize - 23, xsize - 47, ysize -  4);
	fill_rectangle(vram, xsize, VGA_WHITE,     xsize - 47, ysize -  3, xsize -  4, ysize -  3);
	fill_rectangle(vram, xsize, VGA_WHITE,     xsize -  3, ysize - 24, xsize -  3, ysize -  3);

}
#define CONTROL_PORT 0x03c8
#define DATA_PORT 0x03c9
void set_palette(int start, int end, unsigned char * rgb)
{
	unsigned long i,eflags;
	load_eflags(eflags);
	cli();
	outb(start, CONTROL_PORT);
	for(i = start; i <= end; i++)
	{
		outb(rgb[0] / 4, DATA_PORT);
		outb(rgb[1] / 4, DATA_PORT);
		outb(rgb[2] / 4, DATA_PORT);
		rgb += 3;
	}
	
	restore_eflags(eflags);
	
}
void init_palette(void)//����������Ҫ����дһ�µ�ɫ�壬��Ϊ��bochs��virtualbox���ֳ�������ɫ��һ��
{
	//���ǿ����������õ�ɫ�壬ͨ���±ߵķ�ʽ
	 // static unsigned char table_rgb[16 * 3] = {//������ʵֻ��Ҫ15����ɫ ����㹻�ˡ�
		// 0x00, 0x00, 0x00,	//  0:��ɫ 
		// 0xff, 0x00, 0x00,	//  1:����
		// 0x00, 0xff, 0x00,	//  2:����
		// 0xff, 0xff, 0x00,	//  3:���� 
		// 0x00, 0x00, 0xff,	//  4:���� 
		// 0xff, 0x00, 0xff,	//  5:���� 
		// 0x00, 0xff, 0xff,	//  6:��ˮ 
		// 0xc6, 0xc6, 0xc6,	//  7:���� 
		// 0x84, 0x84, 0x84,	//  8:���� 
		// 0x84, 0x00, 0x00,	//  9:���� 
		// 0x00, 0x84, 0x00,	// 10:���� 
		// 0x84, 0x84, 0x00,	// 11:���� 
		// 0x00, 0x00, 0x84,	// 12:���� 
		// 0x84, 0x00, 0x84,	// 13:���� 
		// 0x00, 0x84, 0x84,	// 14:��ˮ 
		// 0xff, 0xff, 0xff,	// 15:�� 
	// };
	// set_palette(0, 15, table_rgb);
	//�����ĵ�ɫ�壬ò����virtualbox ��bochs��һ�£��Ҿ�������������
	unsigned long i,eflags;
	load_eflags(eflags);
	cli();
	outb(8, CONTROL_PORT);
	outb(0x84 / 4, DATA_PORT);
	outb(0x84 / 4, DATA_PORT);
	outb(0x84 / 4, DATA_PORT);
	restore_eflags(eflags);
	
}







