	.section	__TEXT,__text,regular,pure_instructions
	.globl	_SIGINT_Handler
	.align	4, 0x90
_SIGINT_Handler:
Leh_func_begin1:
	pushq	%rbp
Ltmp0:
	movq	%rsp, %rbp
Ltmp1:
	subq	$32, %rsp
Ltmp2:
	movl	%edi, -4(%rbp)
	movl	$1, -20(%rbp)
	movl	-20(%rbp), %eax
	leaq	L_.str(%rip), %rcx
	movabsq	$10, %rdx
	movl	%eax, %edi
	movq	%rcx, %rsi
	callq	_write
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rax
	cmpq	$10, %rax
	je	LBB1_2
	movl	$4294966297, %eax
	movl	%eax, %edi
	callq	_exit
LBB1_2:
	addq	$32, %rsp
	popq	%rbp
	ret
Leh_func_end1:

	.globl	_SIGUSR1_Handler
	.align	4, 0x90
_SIGUSR1_Handler:
Leh_func_begin2:
	pushq	%rbp
Ltmp3:
	movq	%rsp, %rbp
Ltmp4:
	subq	$32, %rsp
Ltmp5:
	movl	%edi, -4(%rbp)
	movl	$1, -20(%rbp)
	movl	-20(%rbp), %eax
	leaq	L_.str1(%rip), %rcx
	movabsq	$9, %rdx
	movl	%eax, %edi
	movq	%rcx, %rsi
	callq	_write
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rax
	cmpq	$9, %rax
	je	LBB2_2
	movl	$4294966297, %eax
	movl	%eax, %edi
	callq	_exit
LBB2_2:
	movl	$1, %eax
	movl	%eax, %edi
	callq	_exit
Leh_func_end2:

	.globl	_main
	.align	4, 0x90
_main:
Leh_func_begin3:
	pushq	%rbp
Ltmp6:
	movq	%rsp, %rbp
Ltmp7:
	subq	$48, %rsp
Ltmp8:
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movl	$2, %eax
	leaq	_SIGINT_Handler(%rip), %rcx
	movl	%eax, %edi
	movq	%rcx, %rsi
	callq	_Signal
	movl	$30, %eax
	leaq	_SIGUSR1_Handler(%rip), %rcx
	movl	%eax, %edi
	movq	%rcx, %rsi
	callq	_Signal
	callq	_getpid
	movl	%eax, %ecx
	movl	%ecx, -28(%rbp)
	movl	-28(%rbp), %ecx
	xorb	%dl, %dl
	leaq	L_.str2(%rip), %rdi
	movl	%ecx, %esi
	movb	%dl, %al
	callq	_printf
LBB3_1:
	movq	$1, -48(%rbp)
	movq	$0, -40(%rbp)
	leaq	L_.str3(%rip), %rax
	movq	%rax, %rdi
	callq	_puts
LBB3_2:
	leaq	-48(%rbp), %rax
	movq	%rax, %rdi
	movq	%rax, %rsi
	callq	_nanosleep
	movl	%eax, %ecx
	cmpl	$-1, %ecx
	je	LBB3_2
	jmp	LBB3_1
Leh_func_end3:

	.section	__TEXT,__cstring,cstring_literals
L_.str:
	.asciz	 "Nice try.\n"

L_.str1:
	.asciz	 "Exiting.\n"

L_.str2:
	.asciz	 "pid is: %d\n"

L_.str3:
	.asciz	 "Still here"

	.section	__TEXT,__eh_frame,coalesced,no_toc+strip_static_syms+live_support
EH_frame0:
Lsection_eh_frame:
Leh_frame_common:
Lset0 = Leh_frame_common_end-Leh_frame_common_begin
	.long	Lset0
Leh_frame_common_begin:
	.long	0
	.byte	1
	.asciz	 "zR"
	.byte	1
	.byte	120
	.byte	16
	.byte	1
	.byte	16
	.byte	12
	.byte	7
	.byte	8
	.byte	144
	.byte	1
	.align	3
Leh_frame_common_end:
	.globl	_SIGINT_Handler.eh
_SIGINT_Handler.eh:
Lset1 = Leh_frame_end1-Leh_frame_begin1
	.long	Lset1
Leh_frame_begin1:
Lset2 = Leh_frame_begin1-Leh_frame_common
	.long	Lset2
Ltmp9:
	.quad	Leh_func_begin1-Ltmp9
Lset3 = Leh_func_end1-Leh_func_begin1
	.quad	Lset3
	.byte	0
	.byte	4
Lset4 = Ltmp0-Leh_func_begin1
	.long	Lset4
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset5 = Ltmp1-Ltmp0
	.long	Lset5
	.byte	13
	.byte	6
	.align	3
Leh_frame_end1:

	.globl	_SIGUSR1_Handler.eh
_SIGUSR1_Handler.eh:
Lset6 = Leh_frame_end2-Leh_frame_begin2
	.long	Lset6
Leh_frame_begin2:
Lset7 = Leh_frame_begin2-Leh_frame_common
	.long	Lset7
Ltmp10:
	.quad	Leh_func_begin2-Ltmp10
Lset8 = Leh_func_end2-Leh_func_begin2
	.quad	Lset8
	.byte	0
	.byte	4
Lset9 = Ltmp3-Leh_func_begin2
	.long	Lset9
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset10 = Ltmp4-Ltmp3
	.long	Lset10
	.byte	13
	.byte	6
	.align	3
Leh_frame_end2:

	.globl	_main.eh
_main.eh:
Lset11 = Leh_frame_end3-Leh_frame_begin3
	.long	Lset11
Leh_frame_begin3:
Lset12 = Leh_frame_begin3-Leh_frame_common
	.long	Lset12
Ltmp11:
	.quad	Leh_func_begin3-Ltmp11
Lset13 = Leh_func_end3-Leh_func_begin3
	.quad	Lset13
	.byte	0
	.byte	4
Lset14 = Ltmp6-Leh_func_begin3
	.long	Lset14
	.byte	14
	.byte	16
	.byte	134
	.byte	2
	.byte	4
Lset15 = Ltmp7-Ltmp6
	.long	Lset15
	.byte	13
	.byte	6
	.align	3
Leh_frame_end3:


.subsections_via_symbols
