#ifndef _HEAD_H
#define _HEAD_H
//���ļ�������cpu���������ṹ
typedef struct desc_struct{
	unsigned long a,b;
}desc_table[256];
 unsigned long pg_dir[1024];
extern desc_table idt,gdt;
#endif