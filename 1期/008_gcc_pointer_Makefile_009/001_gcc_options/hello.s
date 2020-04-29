	.file	"hello.c"
	.section	.rodata
.LC0:
	.string	"Hello World "
	.align 8
.LC1:
	.string	"MAX = %d,MIN = %d,MAX + MIN = %d\n"
	.align 8
.LC2:
	.string	"SetBit(5) = %d,SetBit(6) = %d\n"
.LC3:
	.string	"SetBit( SetBit(2) ) = %d\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movl	$.LC0, %edi
	call	puts
	movl	$30, %ecx
	movl	$10, %edx
	movl	$20, %esi
	movl	$.LC1, %edi
	movl	$0, %eax
	call	printf
	movl	$64, %edx
	movl	$32, %esi
	movl	$.LC2, %edi
	movl	$0, %eax
	call	printf
	movl	$16, %esi
	movl	$.LC3, %edi
	movl	$0, %eax
	call	printf
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.4) 5.4.0 20160609"
	.section	.note.GNU-stack,"",@progbits
