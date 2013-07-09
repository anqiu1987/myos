#include "./../include/stdlib.h"
#include "./../include/sheet.h"
#define SHEET_USED 1
struct SHTCTL*shtctl_init(unsigned char * vram, int xsize, int ysize)	
{
	struct SHTCTL* ctl;
	int i;
	ctl = (struct SHTCTL*)malloc(sizeof(struct SHTCTL));//��Ϊ����ṹ�ܴ������������ͨ����̬�����ڴ棬�Է�ֹջ���
	if(ctl == 0)
	{
		return ctl;
	}
	
	ctl->map = (unsigned char*)malloc(MAP_PAGE_SIZE);//�����������ڴ�,����ڴ���������ͼ��ĸ��
	if(0 == ctl->map)
	{
		return (struct SHTCTL*)0;
	}
		
	
	ctl->vram = vram;
	ctl->xsize = xsize;
	ctl->ysize = ysize;
	ctl->top = -1;
	for(i = 0; i < MAX_SHEETS; i++)
	{
		ctl->sheet0[i].flags = 0;//���δʹ��
		ctl->sheet0[i].ctl = ctl;
	}
	return ctl;
}
//���ҿ��е�ͼ��
struct SHEET* sheet_alloc(struct SHTCTL* ctl)
{	
	int i;
	for(i = 0; i < MAX_SHEETS; i++)
	{
		if(ctl->sheet0[i].flags == 0)
		{
			ctl->sheet0[i].flags = SHEET_USED;
			ctl->sheet0[i].height = -1;
			return &(ctl->sheet0[i]);
		}
	}
	return (struct SHEET*)0;
}
void sheet_setbuf(struct SHEET*sht, unsigned char * buf, int xsize, int ysize, int col_inv)
{
	sht->buf = buf;
	sht->bxsize = xsize;
	sht->bysize = ysize; 
	sht->col_inv = col_inv;
}

void sheet_updown(struct SHEET* sht, int height)
{
	int h,  old = sht->height;//����߶�
	struct SHTCTL *ctl = sht->ctl;
	if(height > ctl->top + 1)//�������õĸ߶�
		height = ctl->top + 1;
	if(height < -1)	
		height = - 1;
	sht->height = height;
	if(old > height)//��������ƶ��ˣ����ǰѸ�ͼ��֮�µ�������
	{
		if(height >= 0)
		{
			for(h = old; h > height; h--)
			{
				ctl->sheets[h] = ctl->sheets[h - 1];
				ctl->sheets[h]->height = h;
				}
			ctl->sheets[height] = sht;
		}else
		{
			if(ctl->top > old)
			{
				for(h = old; h < ctl->top; h++)
					ctl->sheets[h] = ctl->sheets[h + 1];
					ctl->sheets[h]->height = h;
			}
			ctl->top--;
		}
		sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height + 1);
		sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height + 1, old);
		
	}else//��������ƶ������ǰ�֮�ϵ������ƶ�
	{
		if(old > 0)
		{
			for(h = old; h < height; h++)
			{
				ctl->sheets[h] = ctl->sheets[h + 1];
				ctl->sheets[h]->height = h;
			}
		}else
		{
			for(h = ctl->top; h >= height; h--)
			{
				ctl->sheets[h + 1] = ctl->sheets[h];
				ctl->sheets[h + 1]->height = h + 1;
			}
			
			ctl->sheets[height] = sht;
			ctl->top++;//���������²�����һ��ͼ��������Ҫ����top
		}
	sheet_refreshmap(ctl,sht->vx0,sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height );	
	sheet_refreshsub(ctl,sht->vx0,sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize,height, height);
	}
	
}
//ˢ��ͼ��
void sheet_refresh(struct SHEET*sht, int bx0, int by0, int bx1, int by1)
{
	 if(sht->height >= 0)
	{
		sheet_refreshsub(sht->ctl, sht->vx0+bx0,sht->vy0+by0, sht->vx0+bx1, sht->vy0+by1,sht->height, sht->height);
	}
}

//ͼ���Ƶ���λ��
void sheet_slide(struct SHEET* sht, int vx, int vy)
{
	int oldx = sht->vx0, oldy = sht->vy0;
	struct SHTCTL *ctl =sht->ctl;
	sht->vx0 = vx;
	sht->vy0 = vy;
	if(sht->height >= 0)
	{
		sheet_refreshmap(ctl, oldx, oldy, oldx + sht->bxsize, oldy + sht->bysize, 0);
		sheet_refreshmap(ctl, vx,   vy,   vx + sht->bxsize,    vy + sht->bysize, sht->height);
		sheet_refreshsub(ctl, oldx, oldy, oldx + sht->bxsize, oldy + sht->bysize, 0, sht->height - 1);
		sheet_refreshsub(ctl, vx,    vy,  vx  +  sht->bxsize, vy   + sht->bysize,sht->height, sht->height);
	}
}
//��ͼ��map�����Ǹ���ͼ��Ĳ�ι�ϵ��������map����ͼ�����ʽ�����ǾͿ���֪��ĳ��λ����ʾ�����Ǹ�ͼ��
//��������ˢ��һ�������[vx0��vy0]��[vx1��vy1]�����ݱ�������ͼ�㣬�鿴�������Ƿ���ˢ�·�Χ��Ȼ��ˢ�¸�λ��
void sheet_refreshmap(struct SHTCTL*ctl, int vx0, int vy0, int vx1, int vy1, int h0)
{
	int h, x0, y0,x1,y1,x,y,sid;
	struct SHEET *sht;
	unsigned char *vrambuf,*buf,c, *vram = ctl->map;
	 for(h = h0; h <= ctl->top; h++)
	 {
		sht = ctl->sheets[h];
		sid = sht - ctl->sheet0;//sht����ײ����
		x0 = vx0 - sht->vx0;//����ȥ�ö�Ӧ�ľ����ڸ�ͼ������λ��
		y0 = vy0 - sht->vy0;
		if(x0 < 0)
			x0 = 0;
		if(y0 < 0)
			y0 = 0;
		x1 = vx1 - sht->vx0;
		y1 = vy1 - sht->vy0;
		if(x1 > sht->bxsize) x1 = sht->bxsize;
		if(y1 > sht->bysize) y1 = sht->bysize;
		
		for(y = y0; y < y1; y++)
		{
			vrambuf = &vram[(y + sht->vy0)*ctl->xsize + sht->vx0];
			buf = &sht->buf[y * sht->bxsize];
			for(x = x0; x < x1; x++)
			{
				c = buf[x];
				if(c != sht->col_inv)//��λ���Ƿ���ʾΪ͸��
					vrambuf[x] = sid;
			}
		}
	}	

}
//����ͼ��map��������ص�����
void sheet_refreshsub(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1,int h0, int h1)
{
	int h, x0, y0,x1,y1,x,y,sid;
	struct SHEET *sht;
	unsigned char *vrambuf,*vrammap,*buf,c, *vram = ctl->vram;
	 for(h = h0; h <= h1; h++)
	 {
		sht = ctl->sheets[h];
		sid = sht - ctl->sheet0;
		x0 = vx0 - sht->vx0;
		y0 = vy0 - sht->vy0;
		if(x0 < 0)
			x0 = 0;
		if(y0 < 0)
			y0 = 0;
		x1 = vx1 - sht->vx0;
		y1 = vy1 - sht->vy0;
		if(x1 > sht->bxsize) x1 = sht->bxsize;
		if(y1 > sht->bysize) y1 = sht->bysize;
		
		for(y = y0; y <= y1; y++)
		{
			vrambuf = &vram[(y + sht->vy0)*ctl->xsize + sht->vx0];
			vrammap = &(ctl->map[(y + sht->vy0)*ctl->xsize + sht->vx0]);
			buf = &sht->buf[y * sht->bxsize];
			for(x = x0; x <= x1; x++)
			{
				c = buf[x];
				if(vrammap[x] == sid)
					vrambuf[x] = c;
			}
		}
	}	
}


//�ͷŸ�ҳ��
void sheet_free(struct SHEET *sht)
{
	sht->flags = 0;
}
