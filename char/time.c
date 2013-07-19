#ifdef DJGPP
#define time_interrupt_asm time_interrupt_asm
#define do_time_interrupt  do_time_interrupt 
#else
#define time_interrupt_asm _time_interrupt_asm
#define do_time_interrupt  _do_time_interrupt
#endif
#define PIT_CMD 0x43       //mode/command register(write only, a read is ignored)
#define PIT_DATA 0x40      //channel 0 data port(read write)
#include "./../include/io.h"
#include "./../include/kernel.h"
#include "./../include/head.h"
#include "./../include/system.h"
#include "./../include/time.h"
#include "./../include/char.h"
#include "./../include/fifo.h"
#include "./../include/sched.h"
/*web :http://wiki.osdev.org/Programmable_Interval_Timer
Bits         Usage
 6 and 7      Select channel :
                 0 0 = Channel 0
                 0 1 = Channel 1
                 1 0 = Channel 2
                 1 1 = Read-back command (8254 only)
 4 and 5      Access mode :
                 0 0 = Latch count value command
                 0 1 = Access mode: lobyte only
                 1 0 = Access mode: hibyte only
                 1 1 = Access mode: lobyte/hibyte //�����ʾ�ȸ��ֽ�
 1 to 3       Operating mode :
                 0 0 0 = Mode 0 (interrupt on terminal count)
                 0 0 1 = Mode 1 (hardware re-triggerable one-shot)
                 0 1 0 = Mode 2 (rate generator)
                 0 1 1 = Mode 3 (square wave generator)
                 1 0 0 = Mode 4 (software triggered strobe)
                 1 0 1 = Mode 5 (hardware triggered strobe)
                 1 1 0 = Mode 2 (rate generator, same as 010b)
                 1 1 1 = Mode 3 (square wave generator, same as 011b)
 0            BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD
*/
extern void time_interrupt_asm();
unsigned long volatile counter;
static struct TIMECTL timerctl;//��ʱ��������
extern struct TIMER*task_timer;//������Ϊ�����л�ʱ��
void do_time_interrupt()
{
	int i;
	char ts = 0;
	struct TIMER*timer;
	PIC_sendEOI(0);
	counter++;
	if(timerctl.min_expire > counter)
	{
		return;
	}
	
	timer = timerctl.t0;
	while(1)
	{
		if(timer->expire > counter)
		{break;}
		timer->flag = TIMER_FLAGS_ALLOC;
		if(timer != task_timer)
			fifo_put(timer->fifo, timer->data);
		else
			ts = 1;
		timer = timer->next;
	}	
	
	timerctl.t0 = timer;
	timerctl.min_expire = timer->expire;
	if(ts != 0)
	{
		task_switch();
	}
}

//��ʼ����ʱ��
void timer_init(struct TIMER*time, struct FIFO* fifo, int data)
{
	time->fifo = fifo;
	time->data = data;
	
}

void timer_settime(struct TIMER*time, unsigned long timeout)
{
	struct TIMER *tmp;
	struct TIMER *pre;
	int e ;
	tmp = timerctl.t0;
	time->flag = TIMER_FLAGS_USING;
	time->expire = counter + timeout;//��������ʱ�ӵĳ���ʱ��Ϊ��ǰ�����������timeout
	load_eflags(e);
	cli();//�����ж��Է�ֹ��ռ������
	if(tmp->expire > time->expire)//������뵽��ͷ
	{
		time->next = tmp;
		timerctl.t0 = time;
		timerctl.min_expire = time->expire;
		restore_eflags(e);
		return;
	}
	while(1)//Ѱ�Ҳ����λ��
	{
		pre = tmp;
		tmp = tmp->next;
		if(time->expire <= tmp->expire)
		{
			pre->next = time;
			time->next = tmp;
			restore_eflags(e);
			return ;
		}
	}
}
//����һ��timer��ʱ��
struct TIMER* timer_alloc()
{
	int h;
	for(h = 0; h< MAX_TIMER; ++h)
	{
		if(timerctl.timer0[h].flag == TIMER_FLAGS_FREE)
		{
			timerctl.timer0[h].flag = TIMER_FLAGS_ALLOC;
			return &timerctl.timer0[h];
		}
	}
	return (struct TIMER*)0;
}

//�ͷŸö�ʱ��
void timer_free(struct TIMER*timer)
{
	timer->flag = 0;
}
#define PIT_FREQUENCY 1193182
void init_pit()
{
	int h;
	struct TIMER *t;
	set_intr_gate(0x20,time_interrupt_asm);//�����ж�
	outb(0x34,PIT_CMD);
	outb((PIT_FREQUENCY+CLOCKS_PER_SEC/2)/CLOCKS_PER_SEC,PIT_DATA);
	outb(((PIT_FREQUENCY+CLOCKS_PER_SEC/2)/CLOCKS_PER_SEC)>>8,PIT_DATA);
	IRQ_Clear_mask(0);//�����ж�
	//��ʼ����ʱ��������
	for(h = 0; h < MAX_TIMER; ++h)
	{
		timerctl.timer0[h].flag = TIMER_FLAGS_FREE;
	}
	//���Ǽ���һ���ڱ����Լ��ٲ���ʱ�Ĵ�������
	t  = timer_alloc();
	t->expire = 0xffffffff;
	t->flag = TIMER_FLAGS_USING;
	timerctl.t0 = (struct TIMER*)t;
}

