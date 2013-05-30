[bits 32]
_divide_error:
		push _do_divide_error
no_error_code:
		xchg eax [esp]
		push ebx
		push ecx
		push edx
		push edi
		push esi
		push ebp
		push fs
		push es
		push fs
		push 0;error code
		lea edx,44[esp]
		push edx
		mov ax,ss
		mov ds,ax
		mov es,ax
		mov fs,ax
		call eax
		add esp,8
		pop fs
		pop es
		pop ds
		pop ebp
		pop esi
		pop edi
		pop edx
		pop ecx
		pop ebx
		pop eax
		iret
_debug:;int1 --debug �����ж����
		push _do_int3
		jmp no_error_code
_nmi:	;int2 �������жϵĵ������
		push _do_nmi
		jmp no_error_code
_int3:	;int3 �ϵ�ָ��������ж���ڣ��������ͬ_debug
		push _do_int3
		jmp no_error_code
_over_flow:;int4  ���������ж���ڵ�
		push _do_overfolw
		jmp no_error_code
_bounds:;int5 �߽���
		push _do_bounds
		jmp no_error_code
_invalid_op:;int6 ��Ч����ָ������ж����
		push _do_invalid_op
		jmp no_error_code
_coprocessor_segment_overrun:;int9 Э�������γ��������ж���ڵ�
		push _do_coprocessor_segment_overrun
		jmp no_error_code
_reserved:;int15 ����
		push _do_reserved
		jmp no_error_code

_irq13:;int45  ����Э�����������ж�
		push eax
		xor al,al
		out 0xF0 al
		mov al,0x20
		out 0x20,al
		jmp 1f
1f:		jmp 2f
2f:		out 0xa0,al
		pop eax
		jmp _coprocessor_error
_double_fault:
		push _do_double_fault
error_code:
		xchg eax 4[esp]
		xchg ebx [esp]
		push ecx
		push edx
		push edi
		push esi
		push ebp
		push ds
		push es
		push fs
		push eax
		lea eax 44[esp]
		push eax
		mov  ax,ss
		mov ds,ax
		mov es,ax
		mov fs,ax
		call ebx
		add esp,8
		pop fs
		pop es
		pop ds
		pop ebp
		pop esi
		pop edi
		pop edx
		pop ecx
		pop ebx
		pop eax
		iret
_invalid_TSS:;int10 ��Ч������״̬��
		push _do_invalid_TSS
		jmp error_code
_segment_not_present:;int11 �β�����
		push _do_segment_not_present
		jmp error_code
_stack_segment:;int12 ��ջ�δ���
		push _do_stack_segment
		jmp error_code
_general_protection:;int13 һ�㱣���Գ���
		push _do_general_protection
		jmp error_code


		
		
		