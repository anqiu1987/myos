//���ļ����������û��޸�������/�ж��ŵȵ�Ƕ��ʽ����
#define sti() __asm__("sti"::);
#define stihlt() __asm__("sti\n" \
		"hlt"::);
#define cli() __asm__("cli"::);
#define nop() __asm__("nop"::);
#define iret() __asm__("iret"::);
#define hlt()  __asm__("hlt"::);
#define load_eflags(x)\
__asm__ __volatile__("pushfl; popl %0":"=r"(x):/*no input*/:"memory")
#define restore_eflags(x)\
__asm__ __volatile__("pushl %0;popfl":/*no output*/:"r"(x):"memory")
//eax ���ǰ4���ֽڣ�ǰ�����ֽ��ǵ�ַ�ĵ�16λ����2���ֽ��ǶμĴ�����ѡ���
//edx��ɺ��ĸ��ֽڣ���һ���ֽ���0����2���ֽ������ԣ��������ֽ��ǵ�ַ�ĸ�16λ
/*

  7                           0
+---+---+---+---+---+---+---+---+
| P |  DPL  | S |    GateType   |
+---+---+---+---+---+---+---+---+
Possible IDT gate types :
0b0101	 0x5	 5	 80386 32 bit Task gate
0b0110	 0x6	 6	 80286 16-bit interrupt gate
0b0111	 0x7	 7	 80286 16-bit trap gate
0b1110	 0xE	 14	 80386 32-bit interrupt gate
0b1111	 0xF	 15	 80386 32-bit trap gate

*/
#define _set_gate(gate_addr, type, dpl, addr)\
		__asm__("movw %%dx, %%ax\n\t"\
		"movw %0, %%dx\n\t"\
		"movl  %%eax, %1\n\t"\
		"movl %%edx, %2\n\t"\
		::"i"((short)(0x8000+(dpl<<13) + (type<<8))),\
		"o"(*(char*)(gate_addr)),\
		"o"(*(4 + (char*)(gate_addr))),\
		"d"((char*)(addr)),"a"(0x80000)\
		:"3","4")
//����������������15 ��Ȩ��0		
#define set_trap_gate(n, addr) \
		_set_gate(&idt[n], 15,0,addr);
//����������������14 ��Ȩ��3	
#define set_system_gate(n, addr) \
		_set_gate(&idt[n],15,3,addr);
//����������������14 ��Ȩ��0	
#define set_intr_gate(n, addr)	\
		_set_gate(&idt[n], 14, 0, addr);
		