#define BOOT_INFO_ADDR 0x90000
struct BOOT_INFO
{
	char led,vmode;
	short scrnx,scrny,reserve;
	char * vram;
	short ext_mem_k;
};
extern void mem_init(long start, long end);
static struct BOOT_INFO* boot_info = (struct BOOT_INFO*)BOOT_INFO_ADDR;
static long memory_end = 0;//�������е��ڴ��ֽ���
static long buffer_memory_end = 0;//���ٻ�����ĩ�˵�ַ
static long main_memory_start = 0;//���ڴ濪ʼ�ĵ�ַ
void main()
{
	//���������ǵĽ�����ʼ��һ���ֵ�ַ���ⲿ�ֵ�ַ��ΪԶ��16M�ڴ��ˣ�����ò����ȫ���ÿ����ˡ�
	memory_end = (1 << 20) + (boot_info->ext_mem_k << 10);//�ڴ��С=1M + ��չ�ڴ棨KB��*1024
	memory_end &= 0xfffff000;						//���Բ���4k���ڴ���
	if(memory_end > 16 * 1024 * 1024)				//��ʱ����ڴ泬��16M������16M����
		memory_end = 16 * 1024 * 1024;
	if(memory_end > 12 * 1024 * 1024)				//����ڴ� > 12M �����û�����ĩ��=4M		
		buffer_memory_end =  4 * 1024 * 1024;
	else if(memory_end  > 6 * 1024 * 1024)          //�������6M������2M
		buffer_memory_end = 2 * 1024 * 1024;
	else											//��������1M	
		buffer_memory_end = 1 * 1024 * 1024; 
	
	main_memory_start = buffer_memory_end;			//���ڴ�����ʼλ��=������ĩ��
	mem_init(main_memory_start, memory_end);
	for(;;);
}