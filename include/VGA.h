#ifndef _VGA_H_
#define _VGA_H_
#define VGA_BLACK         0//��
#define VGA_BLUE          1//��
#define VGA_GREEN         2//��
#define VGA_CYAN          3//����ɫ
#define VGA_RED           4//��
#define VGA_MAGENTA       5//����ɫ; ���
#define VGA_BROWN         6//��ɫ
#define VGA_LIGHT_GRAY    7//��ɫ
#define VGA_DARK_GRAY     8//����ɫ
#define VGA_LIGHT_BLUE    9//��ɫ
#define VGA_LIGHT_GREEN   10
#define VGA_LIGHT_CYAN    11
#define VGA_LIGHT_RED     12
#define VGA_LIGHT_MAGENTA 13
#define VGA_YELLOW        14
#define VGA_WHITE         15
//vga���ƶ˿�
#define PALETTE_MASK  0x3C6
#define PALETTE_READ  0x3C7
#define PALETTE_WRITE 0x3C8
#define PALETTE_DATA  0x3C9
#define CONTROL_ALL   0xff
#endif
