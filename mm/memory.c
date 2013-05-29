#define invalidate() __asm__("movl %%eax, %%cr3"::"a"(0))   //ˢ��Ҷ�任���ٻ���
#define LOW_MEM 0x100000									//�ڴ�Ͷ�
#define PAGING_MEMORY (15 * 1024 * 1024)                   //��ҳ�ڴ�15M
#define PAGING_PAGES (PAGING_MEMORY >> 12)				    //��ҳ��������ڴ�ҳ��
#define MAP_NR(addr) (((addr)-LOW_MEM) >> 12)					//ָ���ڴ��ַӳ��Ϊҳ��
#define USED 100										   //ҳ�汻ռ�ñ�־
#define CODE_SPACE(addr) ((((addr) + 4095)& ~4095) < current->stat_code + current->end_code)//�жϸ����ĵ�ַ�Ƿ�λ�ڵ�ǰ���̵Ĵ������
static long HIGH_MEMORY = 0;
#define copy_page(from, to) __asm__("cld;rep;movsl"::"S"(from),"D"(to),"c"(1024):"cx","di","si")
static unsigned char mem_map[PAGING_PAGES] ={0,};
void mem_init(long start_mem, long end_mem)
{
	int i;
	HIGH_MEMORY = end_mem;									//�����ڴ����߶�
	for(i = 0; i < PAGING_PAGES; i++)						//���������е�ҳ��Ϊ��ռ�ã�USED = 100��״̬
	{
		mem_map[i] = USED;
	}
	
	i = MAP_NR(start_mem);								//Ȼ������ʹ����ʼ�ڴ��ҳ��
	end_mem -= start_mem;									//�ټ���ɷ�ҳ������ڴ���ҳ��
	end_mem >>= 12;											//�Ӷ������������ҳ���Ӧ��ҳ��ӳ����������	
	while(end_mem-- > 0)									//�����Щ����ҳ���Ӧ��ҳ��ӳ����������
		mem_map[i++] = 0;
}