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
#endif