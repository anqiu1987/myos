#ifndef _TIME_H_
#define _TIME_H_
typedef unsigned int size_t;
typedef long clock_t;
#define CLOCKS_PER_SEC 100 //ϵͳ�δ�Ƶ��100HZ
struct tm
{	
	int tm_sec;//����
	int tm_min;//����
	int tm_hour;//Сʱ��
	int tm_mday;//һ���µ�����
	int tm_mon;//һ�����·�
	int tm_year;//��1900�꿪ʼ������
	int tm_wday;//һ���ڵ�ĳ��
	int tm_yday;//һ���е�ĳ��
	int tm_isdst;//����ʱ��־

};
struct FIFO;
struct TIMER
{
	struct TIMER *next;  //ֻ����һ����ʱ��
	unsigned long expire;//��ʱʱ��
	struct FIFO*fifo;    //��ʱ��ʱ��Ĺܵ�
	int data;   //���ܵ�д������
	unsigned char flag;   //ʹ��״̬
};
#define MAX_TIMER 255
#define TIMER_FLAGS_FREE  0//����״̬
#define TIMER_FLAGS_ALLOC 1//��������״̬
#define TIMER_FLAGS_USING 2//����ʹ��״̬
struct TIMECTL
{
	unsigned long min_expire;
	struct TIMER *t0;//���濪ʼָ��
	struct TIMER timer0[MAX_TIMER];
};
extern void init_pit();
extern void timer_free(struct TIMER*timer);
extern struct TIMER* timer_alloc();
extern void timer_settime(struct TIMER*time, unsigned long timeout);
extern void timer_init(struct TIMER*time, struct FIFO* fifo, int data);
#endif