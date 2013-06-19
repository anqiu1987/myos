#ifndef _MEMORY_C_
#define _MEMORY_C_
#include "../include/kernel.h"
#include "../include/stdlib.h"
#define invalidate() __asm__("movl %%eax, %%cr3"::"a"(0))   //ˢ��Ҷ�任���ٻ���
#define LOW_MEM 0x100000									//�ڴ�Ͷ�
#define PAGING_MEMORY (15 * 1024 * 1024)                   //��ҳ�ڴ�15M
#define PAGING_PAGES (PAGING_MEMORY >> 12)				    //��ҳ��������ڴ�ҳ��
#define MAP_NR(addr) (((addr)-LOW_MEM) >> 12)					//ָ���ڴ��ַӳ��Ϊҳ��
#define USED 100										   //ҳ�汻ռ�ñ�־
#define CODE_SPACE(addr) ((((addr) + 4095)& ~4095) < current->stat_code + current->end_code)//�жϸ����ĵ�ַ�Ƿ�λ�ڵ�ǰ���̵Ĵ������
unsigned long HIGH_MEMORY = 0;
#define copy_page(from, to) __asm__("cld;rep;movsl"::"S"(from),"D"(to),"c"(1024):"cx","di","si")
unsigned char mem_map[PAGING_PAGES] ={0,};
static inline volatile void oom()
{
	
}
void mem_init(unsigned long start_mem, unsigned long end_mem)
{
	int i;
	HIGH_MEMORY = end_mem;									//�����ڴ����߶�
	for(i = 0; i < PAGING_PAGES; i++)						//���������е�ҳ��Ϊ��ռ�ã�USED = 100��״̬
	{
		mem_map[i] = USED;
	}
	
	i = MAP_NR(start_mem);								//Ȼ������ʹ����ʼ�ڴ��ҳ��
	end_mem -= start_mem;									//�ټ���ɷ�ҳ������ڴ���ҳ��
	end_mem >>= 12;											//�Ӷ������������ҳ���Ӧ��ҳ��ӳ����������	
	while(end_mem-- > 0)									//�����Щ����ҳ���Ӧ��ҳ��ӳ����������
		mem_map[i++] = 0;
}
/*
EFLAG ����
0	CF	Carry flag	Status
1	1	Reserved	 
2	PF	Parity flag	Status
3	0	Reserved	 
4	AF	Adjust flag	Status
5	0	Reserved	 
6	ZF	Zero flag	Status
7	SF	Sign flag	Status
8	TF	Trap flag (single step)	System
9	IF	Interrupt enable flag	Control
10	DF	Direction flag	Control
11	OF	Overflow flag	Status
12-13	IOPL	I/O privilege level (286+ only), always 1 on 8086 and 186	System
14	NT	Nested task flag (286+ only), always 1 on 8086 and 186	System
15	0	Reserved, always 1 on 8086 and 186, always 0 on later models	 
*/
unsigned long get_free_page()
{
	register unsigned long __res asm("eax");
	__asm__("std;repne;scasb\n\t"                       //�Ƚ�al ��[es:edi]��ֵ������Ӧ�ı�־�Ĵ�����ֵ�����dfΪ0������edi������Ӻ���ǰɨ��
			"jne 1f\n\t"									//û���ҵ�
			"movb $1,1(%%edi)\n\t"							//��1��ʾ�ҵ���
			"sall $12,%%ecx\n\t"							//����12λ4K��Ҳ���Ǽ���������ڴ��ƫ����
			"addl %2,%%ecx\n\t"							   //��������ڴ�Ҳ���Ƕ�Ӧ�����Ե�ַ
			"movl %%ecx,%%edx\n\t"
			"movl $1024,%%ecx\n\t"							//�Ѵ˿��ڴ��ʼ��Ϊ0
			"leal 4092(%%edx),%%edi\n\t"                 //��eax��ֵ�洢��[es:edi]��	
			"rep;stosl\n\t"
			"movl %%edx,%%eax\n"					
			"1:"
			:"=a"(__res)
			:"0"(0),"i"(LOW_MEM),"c"(PAGING_PAGES),
			"D"(mem_map + PAGING_PAGES - 1)
			:"3","4","dx");
	return __res;//���ؿ���ҳ��
}
unsigned long get_liner_pages(int count)
{
	//cli();
	int i;
	int j;
	for(i = PAGING_PAGES - 1; i >=0; i--)
	{
		if(mem_map[i] == 0)
		{
			for(j = i; j >=0 && j > i - count; j--)
			{
				if(mem_map[j] != 0)
				{
					i = j;
					break;
				}
			}
			if(j == i - count)
			{
				for(++j;j<=i; j++)
				{
					mem_map[j] = 1;
				}
				break;
			}
		}
	}
	//sti();
	if(i == 0)
		return 0;
	return LOW_MEM + ((i - count + 1) << 12);
	
}
void free_liner_pages(unsigned long addr, int count)
{
	int i;
	if(addr < LOW_MEM) return;
	if(addr + count * 4096 >= HIGH_MEMORY) panic("trying to free noneexistent page!");
	addr -= LOW_MEM;
	addr >>= 12;
	for(i = 0; i< count; i++)
	{
		if(mem_map[addr + i]-- < 1)
			panic("trying to free free page!");
	}
}
void free_page(unsigned long addr)
{
	if(addr < LOW_MEM) return;
	if(addr >= HIGH_MEMORY)	   panic("trying to free noneexistent page!");
	addr -= LOW_MEM;
	addr >>=12;
	if(mem_map[addr]--) return;
	mem_map[addr] = 0;
	panic("trying to free free page!");
}
#endif