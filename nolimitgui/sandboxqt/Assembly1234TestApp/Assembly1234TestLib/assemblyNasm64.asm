

section .text ; required or visual studio will not link the function

global get_assembly_test_num ; makes this visible to linker

get_assembly_test_num:
	mov rax,1234 ; just return a constant
	ret

get_assembly_reverse_num:
    mov rax,4321 ; just return a constant
    ret
