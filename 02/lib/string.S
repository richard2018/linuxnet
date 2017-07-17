	.file	"string.c"
	.text
.globl StrLen
	.type	StrLen, @function
StrLen:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	movl	$0, -4(%ebp)
	jmp	.L2
.L3:
	incl	-4(%ebp)
.L2:
	movl	8(%ebp), %eax
	movzbl	(%eax), %eax
	testb	%al, %al
	setne	%al
	incl	8(%ebp)
	testb	%al, %al
	jne	.L3
	movl	-4(%ebp), %eax
	leave
	ret
	.size	StrLen, .-StrLen
	.ident	"GCC: (GNU) 4.1.2 20061115 (prerelease) (Debian 4.1.1-21)"
	.section	.note.GNU-stack,"",@progbits


//威海路203号，妇女儿童批发市场
扬格、塞思、外语