#ifndef _STDLIB_
#define _STDLIB_
extern void * malloc(unsigned int len);
extern void printf(const char*fmt, ...);
unsigned long get_liner_pages(int count);//��ô��ڴ�
void free_liner_pages(unsigned long addr, int count);
#endif