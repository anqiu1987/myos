//���ļ���¼�˽����л������
#include "./../include/sched.h"
#include "./../include/time.h"
#include "./../include/stdlib.h"
#include "./../include/gdt.h"
#include "./../include/system.h"
struct TASKCTL *taskctl;
struct TIMER* task_timer;
void task_idle()
{
	while(1)
	{
		hlt();
	}
}
struct TASK* task_now()
{
	struct TASKLEVEL * tl;
	tl = &taskctl->level[taskctl->now_lv];
	return tl->tasks[tl->now];
}

static void task_add(struct TASK*task)
{
	struct TASKLEVEL* tl = &taskctl->level[task->level];
	tl->tasks[tl->running++] = task;
	task->flags = 2;
}

static void task_remove(struct TASK*task)
{
	int i;
	struct TASKLEVEL* tl = &taskctl->level[task->level];
	for(i = 0; i < tl->running; i++)//�ڸ�task���ڵĲ���Ѱ����Ӧ������
	{
		if(task == tl->tasks[i])
			break;
	}
	
	tl->running--;
	if(i < tl->now)
		tl->now--;
	task->flags = 1;//����
	
	//�ƶ�
	for(; i < tl->running; i++)
	{
		tl->tasks[i] = tl->tasks[i+1];
	}
	
}

static void task_switchsub()
{
	int i;
	//Ѱ�����ϲ��level
	for(i=0;i < MAX_TASKLEVELS; i++)
		if(taskctl->level[i].running > 0)
		 break;
	taskctl->now_lv = i;
	taskctl->lv_change = 0;
}
struct TASK* task_init()
{
	int i;
	struct TASK*task ,*idle;
	taskctl = (struct TASKCTL*)malloc(sizeof(struct TASKCTL));//���Ƕ�̬����taskctl���ڴ�
	for(i = 0; i < NR_TASKS; i++)
	{
		taskctl->tasks0[i].flags = 0;
		taskctl->tasks0[i].sel = (TASK_GDT0 + i) * 8;
		set_segmdesc(TASK_GDT0+i,sizeof(struct tss_struct), (int)&taskctl->tasks0[i].tss, 0x89, 0x40);
	}
	for(i = 0; i < MAX_TASKLEVELS; i++)
	{
		taskctl->level[i].now = 0;
		taskctl->level[i].running = 0;
	}
	//�ں˳�ʼ����ռ�ݵ�һ��
	task = task_alloc();
	task->flags = 2;
	task->priority = 2;//0.02m
	task->level = 0;
	task_add(task);//���ӽ���
	task_switchsub();//level ����
	load_tr(task->sel);
	
	
	idle = task_alloc();
	idle->tss.es = 2 * 8;
	idle->tss.cs=  1 * 8;
	idle->tss.ds = 2 * 8;
	idle->tss.fs = 2 * 8;
	idle->tss.gs = 2 * 8;
	idle->tss.ss = 2 * 8;
	idle->tss.esp = (int)malloc(1024) + 1024;
	idle->tss.eip = (int)&task_idle;
	task_run(idle, MAX_TASKLEVELS - 1, 1);
	
	//��ʼ������ʱ��Ƭ
	task_timer = timer_alloc();
	timer_settime(task_timer, task->priority);
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

void task_run(struct TASK* task, int level, int priority)
{
	if(level < 0)
		level = task->level;
	if(priority > 0)
		task->priority = priority;
	if(task->flags == 2 && task->level != level)//����ı�ý��̵�level
		task_remove(task);
	if(task->flags != 2)
	{
		task->level = level;
		task_add(task);
	}
	
	taskctl->lv_change = 1;//�´��л�ʱ���level
}

//�����л����̣�Ѱ��һ���������еĳ���
void task_switch(void)
{
	struct TASKLEVEL * tl = &taskctl->level[taskctl->now_lv];
	struct TASK* new_task, *now_task = tl->tasks[tl->now];
	tl->now++;
	if(tl->now == tl->running)
		tl->now = 0;
	if(taskctl->lv_change != 0)
	{
		task_switchsub();
		tl = &taskctl->level[taskctl->now_lv];
	}
	
	new_task = tl->tasks[tl->now];
	timer_settime(task_timer, new_task->priority);
	if(new_task != now_task)
		farjmp(0, new_task->sel);
}

void task_sleep(struct TASK* task)
{
	struct TASK* now_task;
	if(task->flags == 2)
	{
		now_task = task_now();
		task_remove(task);
		if(now_task == task)//������Լ�˯�ߣ���Ҫ���������л�
		{
			task_switchsub();
			now_task = task_now();
			farjmp(0, now_task->sel);
		}
	}
}