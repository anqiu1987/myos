#ifndef _MM_H
#define _MM_H
#define PAGE_SIZE 4096
extern unsigned long get_free_page();
//extern unsigned long put_page();
extern void free_page(unsigned long addr);
extern unsigned long get_liner_pages(int count);//��ô��ڴ�
extern void free_liner_pages(unsigned long addr, int count);
#endif
