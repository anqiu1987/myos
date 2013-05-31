//ȡ��seg�Ĵ�����addr��ַ��һ���ֽ�
#define get_seg_byte(seg,addr) ({\
register char __res;\
__asm__("push %%fs; mov %%ax, %%fs; mov %%fs:%2, ah;%pop fs":"=a"(__res):""(seg),"m"(*(addr)));\
__res;})
//ȡ��seg�Ĵ�����addr��ַ��һ������
#define get_seg_long(seg, addr) ({\
register unsigned long __res;\
__asm__("push %%fs; mov %%ax,%%fs; mov %%fs:%2,%%eax;pop %%fs"\
		:"=a"(__res):""(seg),"m"(*(addr)));\
__res;})
//ȡ��fs�μĴ�����ֵ
#define _fs() ({\
register unsigned short __res;\
__asm__("mov %%fs, %%ax":"=a"(__res):);\
__res;})

static void die(char * str, long esp_ptr, long nr)
{
	for(;;);
}

void do_double_fault(long esp, long error_code)
{
	die("double fault", esp, error_code);
}

void do_general_protection(long esp, long error_code)
{
	die("general protection", esp, error_code);
}
void do_divide_error(long esp, long error_code)
{
	die("divide error", esp, error_code);
}
void do_int3(long *esp, long error_code ,long fs, long es, long ds, long ebp, long esi, long edi, long edx, long ecx, long ebx, long eax)
{
	
}

void do_nmi(long esp, long error_code)
{
	die("nmi", esp, error_code);
}

void do_debug(long esp, long error_code)
{
	die("debug",esp, error_code);
}
void do_overflow(long esp, long error_code)
{
	die("debug", esp, error_code);
}
void do_bounds(long esp, long error_code)
{
	die("bounds", esp, error_code);
}
void do_invalid_op(long esp, long error_code)
{
	die("invalid operand", esp, error_code);
}

void do_device_not_available(long esp, long error_code)
{
	die("device  not available", esp, error_code);
}
void do_coprocessor_segment_overrun(long esp, long error_code)
{
	die("coprocessor segment overrun", esp, error_code);
}
void do_invalid_TSS(long esp, long error_code)
{
	die("invalid TSS", tsp, error_code);
}
void do_segment_not_present(long esp, long error_code)
{
	die("segment not present", esp, error_coe);
}
void do_stack_segment(long esp, long error_code)
{
	die("stack segment", esp, error_code);
}
void do_coprocessor_rror(long esp, long error_code)
{
	die("coprocessor error",esp, error_code);
}

void do_reserved(long esp, long error_code)
{
	die("reserved", esp ,error_code);
}

//�������쳣�жϳ����ʼ���ӳ��������жϵ����ţ� set_trap_gate()��set_system_gate()��Ҫ��������ǰ�����õ���Ȩ��Ϊ0������Ϊ3����˶ϵ�int3�����overflow��
//�߽�����ж�bounds�����κγ��������
void trap_init()
{
	set_trap_gate(0, &divide_error);
	set_trap_gate(1, &debug);
	set_trap_gate(2, &nmi);
	set_trap_gate(3, &int3);
	set_system_gate(4, &overflow);
	set_system_gate(5, &bounds);
	set_system_gate(6, & invalid_op);
	set_system_gate(7, & device_not_available);
	set_system_gate(8, & double_fault);
	set_system_gate(9, & coprocessor_segment_overrun);
	set_system_gate(10, & invalid_TSS);
	set_system_gate(11, & segment_not_present);
	set_system_gate(12, & stack_segment);
	set_system_gate(13, & general_protection);
	set_system_gate(14, & page_fault);
	set_system_gate(15, & reserved);
	set_system_gate(16, & coprocessor_error);
	for(i=17; i < 48; i++)
		set_trap_gate(i, &reserved);
	set_trap_gate(45, &irq13);
	
}