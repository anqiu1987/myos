;这里地址是从0x0000000开始的，并且是32位模式的代码
[bits 32]
extern _main
_pg_dir:;0x0000
startup_32:
		;修改寄存器的信息
		mov ax,0x10
		mov ds,ax
		mov es,ax
		mov fs,ax
		mov gs,ax
		mov esp, _sys_stack
		jmp after_page_tables

size equ $ - _pg_dir


resb 0x1000 - size
pg0:
resb 0x1000
pg1:;0x2000
resb 0x1000
pg2:;0x3000
resb 0x1000
pg3:;;0x4000
resb 0x1000
		resb 1024
		
section .text
after_page_tables:
		push _main
		jmp setup_paging
		
L6:
		jmp L6

		
alignb 4
setup_paging:
		mov ecx,1024*4;一个页目录四个页表
		xor eax,eax
		xor edi,edi
		cld
		rep stosd
		
		mov dword[_pg_dir],     pg0 + 7;7是保护标志，可读可写
		mov dword[_pg_dir + 4], pg1 + 7
		mov dword[_pg_dir +8],  pg2 + 7
		mov dword[_pg_dir +12], pg3 + 7
		
		mov edi, pg3 + 4092
		mov eax, 0xfff007
		std
b:		stosd
		
		sub eax, 0x1000
		jge b
		
		xor eax,eax
		mov cr3,eax
		mov eax,cr0
		or  eax,0x80000000
		mov cr0,eax
		ret
		
section .bss
		resb 4096
_sys_stack:
