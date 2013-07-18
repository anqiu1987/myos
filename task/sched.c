//���ļ���¼�˽����л������
#include "./../include/sched.h"
#include "./../include/time.h"
#include "./../include/stdlib.h"
#include "./../include/gdt.h"
struct TASKCTL *taskctl;
struct TIMER* task_timer;
struct TASK* task_init()
{
	int i;
	struct TASK*task;
	taskctl = (struct TASKCTL*)malloc(sizeof(struct TASKCTL));//���Ƕ�̬����taskctl���ڴ�
	for(i = 0; i < NR_TASKS; i++)
	{
		taskctl->tasks0[i].flags = 0;
		taskctl->tasks0[i].sel = (TASK_GDT0 + i) * 8;
		set_segmdesc(TASK_GDT0+i,sizeof(struct tss_struct), (int)&taskctl->tasks0[i].tss, 0x89, 0x40);
	}
	//�ں˳�ʼ����ռ�ݵ�һ��
	task = task_alloc();
	task->flags = 2;
	taskctl->running = 1;
	taskctl->now = 0;
	taskctl->tasks[0] = task;
	load_tr(task->sel);
	
	//��ʼ������ʱ��Ƭ
	task_timer = timer_alloc();
	timer_settime(task_timer,2);
	return task;
}

struct TASK* task_alloc()
{
	int i;
	struct TASK* task;
	for(i = 0; i < NR_TASKS; i++)
	{
		if(taskctl->tasks0[i].flags == 0)
		{
			task = &taskctl->tasks0[i];
			task->flags = 1;//��ʾ����ʹ��
			task->tss.eflags = 0x202;
			task->tss.eax = 0;
			task->tss.ecx = 0;
			task->tss.edx = 0;
			task->tss.ebx = 0;
			task->tss.ebp = 0;
			task->tss.esi = 0;
			task->tss.edi = 0;
			task->tss.es = 0;
			task->tss.cs = 0;
			task->tss.ss = 0;
			task->tss.ds = 0;
			task->tss.fs = 0;
			task->tss.gs = 0;
			task->tss.iomap = sizeof task->tss;
			task->tss.ldt = 0;
			return task;
		}
	}
	
	return 0;//ȫ������ʹ����
}

void task_run(struct TASK* task)
{
	task->flags = 2;//��еı�־
	taskctl->tasks[taskctl->running++] = task;
}

//�����л����̣�Ѱ��һ���������еĳ���
void task_switch(void)
{
	timer_settime(task_timer,2);
	if(taskctl->running >= 2)
	{
		taskctl->now++;
		if(taskctl->now == taskctl->running)
		{
			taskctl->now = 0;
		}
		
		farjmp(0, taskctl->tasks[taskctl->now]->sel);
	}
}

void task_sleep(struct TASK* task)
{
	int i;
	char ts = 0;
	if(task->flags == 2)
	{
		if(task == taskctl->tasks[taskctl->now])//�����Ҫ���ߵ������ǵ�ǰ������
		{
			ts = 1;
		}
		
		for(i = 0; i < taskctl->running; i++)//����Ѱ��task��Ӧ��λ��
		{
			if(taskctl->tasks[i] == task)
				break;
		}
	}
	
	taskctl->running--;
	
	if(i < taskctl->now)
	{
		taskctl->now--;
	}
	//���ǰѶ�Ӧ�ĳ�Ա��ǰ��һλ��
	for(; i< taskctl->running; i++)
	{
		taskctl->tasks[i] = taskctl->tasks[i+1];
	}
	task->flags = 1;//��ͣ��״̬
	
	if(ts != 0)//��Ϊ���Լ���ͣ�ˣ����Ե�ִ�������л�
	{
		if(taskctl->now > taskctl->running || taskctl->now == taskctl->running)
		{
			taskctl->now = 0;
		}
		farjmp(0, taskctl->tasks[taskctl->now]->sel);
	}
}