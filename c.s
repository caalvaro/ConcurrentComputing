	.file	"cc.c"
	.text
	.globl	x
	.data
	.align 4
	.type	x, @object
	.size	x, 4
x:
	.long	10
	.globl	queroEntrar_0
	.bss
	.align 4
	.type	queroEntrar_0, @object
	.size	queroEntrar_0, 4
queroEntrar_0:
	.zero	4
	.globl	queroEntrar_1
	.align 4
	.type	queroEntrar_1, @object
	.size	queroEntrar_1, 4
queroEntrar_1:
	.zero	4
	.comm	turn,4,4
	.section	.rodata
.LC0:
	.string	"x = %d\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	endbr32
	leal	4(%esp), %ecx
	.cfi_def_cfa 1, 0
	andl	$-16, %esp
	pushl	-4(%ecx)
	pushl	%ebp
	.cfi_escape 0x10,0x5,0x2,0x75,0
	movl	%esp, %ebp
	pushl	%ecx
	.cfi_escape 0xf,0x3,0x75,0x7c,0x6
	subl	$4, %esp
	movl	$1, queroEntrar_1
	movl	$0, turn
	nop
.L3:
	movl	queroEntrar_0, %eax
	testl	%eax, %eax
	je	.L2
	movl	turn, %eax
	testl	%eax, %eax
	je	.L3
.L2:
	movl	x, %eax
	subl	$1, %eax
	movl	%eax, x
	movl	x, %eax
	addl	$1, %eax
	movl	%eax, x
	movl	x, %eax
	cmpl	$10, %eax
	je	.L4
	movl	x, %eax
	subl	$8, %esp
	pushl	%eax
	pushl	$.LC0
	call	printf
	addl	$16, %esp
.L4:
	movl	$0, queroEntrar_1
	movl	$1, turn
	movl	$0, %eax
	movl	-4(%ebp), %ecx
	.cfi_def_cfa 1, 0
	leave
	.cfi_restore 5
	leal	-4(%ecx), %esp
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 4
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 4
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 4
4:
