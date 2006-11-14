	# $FreeBSD$






	.file	"cast-586.s"
	.version	"01.01"
gcc2_compiled.:
.text
	.align 16
.globl CAST_encrypt
	.type	CAST_encrypt,@function
CAST_encrypt:

	pushl	%ebp
	pushl	%ebx
	movl	12(%esp),	%ebx
	movl	16(%esp),	%ebp
	pushl	%esi
	pushl	%edi

	movl	(%ebx),		%edi
	movl	4(%ebx),	%esi

	movl	128(%ebp),	%eax
	pushl	%eax
	xorl	%eax,		%eax

	movl	(%ebp),		%edx
	movl	4(%ebp),	%ecx
	addl	%esi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	xorl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	subl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	addl	%ebx,		%ecx
	xorl	%ecx,		%edi

	movl	8(%ebp),	%edx
	movl	12(%ebp),	%ecx
	xorl	%edi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	subl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	addl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	xorl	%ebx,		%ecx
	xorl	%ecx,		%esi

	movl	16(%ebp),	%edx
	movl	20(%ebp),	%ecx
	subl	%esi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	addl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	xorl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	subl	%ebx,		%ecx
	xorl	%ecx,		%edi

	movl	24(%ebp),	%edx
	movl	28(%ebp),	%ecx
	addl	%edi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	xorl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	subl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	addl	%ebx,		%ecx
	xorl	%ecx,		%esi

	movl	32(%ebp),	%edx
	movl	36(%ebp),	%ecx
	xorl	%esi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	subl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	addl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	xorl	%ebx,		%ecx
	xorl	%ecx,		%edi

	movl	40(%ebp),	%edx
	movl	44(%ebp),	%ecx
	subl	%edi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	addl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	xorl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	subl	%ebx,		%ecx
	xorl	%ecx,		%esi

	movl	48(%ebp),	%edx
	movl	52(%ebp),	%ecx
	addl	%esi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	xorl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	subl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	addl	%ebx,		%ecx
	xorl	%ecx,		%edi

	movl	56(%ebp),	%edx
	movl	60(%ebp),	%ecx
	xorl	%edi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	subl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	addl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	xorl	%ebx,		%ecx
	xorl	%ecx,		%esi

	movl	64(%ebp),	%edx
	movl	68(%ebp),	%ecx
	subl	%esi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	addl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	xorl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	subl	%ebx,		%ecx
	xorl	%ecx,		%edi

	movl	72(%ebp),	%edx
	movl	76(%ebp),	%ecx
	addl	%edi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	xorl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	subl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	addl	%ebx,		%ecx
	xorl	%ecx,		%esi

	movl	80(%ebp),	%edx
	movl	84(%ebp),	%ecx
	xorl	%esi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	subl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	addl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	xorl	%ebx,		%ecx
	xorl	%ecx,		%edi

	movl	88(%ebp),	%edx
	movl	92(%ebp),	%ecx
	subl	%edi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	addl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	xorl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	subl	%ebx,		%ecx
	xorl	%ecx,		%esi

	popl	%edx
	orl	%edx,		%edx
	jnz	.L000cast_enc_done

	movl	96(%ebp),	%edx
	movl	100(%ebp),	%ecx
	addl	%esi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	xorl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	subl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	addl	%ebx,		%ecx
	xorl	%ecx,		%edi

	movl	104(%ebp),	%edx
	movl	108(%ebp),	%ecx
	xorl	%edi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	subl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	addl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	xorl	%ebx,		%ecx
	xorl	%ecx,		%esi

	movl	112(%ebp),	%edx
	movl	116(%ebp),	%ecx
	subl	%esi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	addl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	xorl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	subl	%ebx,		%ecx
	xorl	%ecx,		%edi

	movl	120(%ebp),	%edx
	movl	124(%ebp),	%ecx
	addl	%edi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	xorl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	subl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	addl	%ebx,		%ecx
	xorl	%ecx,		%esi
.L000cast_enc_done:
	nop
	movl	20(%esp),	%eax
	movl	%edi,		4(%eax)
	movl	%esi,		(%eax)
	popl	%edi
	popl	%esi
	popl	%ebx
	popl	%ebp
	ret
.L_CAST_encrypt_end:
	.size	CAST_encrypt,.L_CAST_encrypt_end-CAST_encrypt
.ident	"CAST_encrypt"
.text
	.align 16
.globl CAST_decrypt
	.type	CAST_decrypt,@function
CAST_decrypt:

	pushl	%ebp
	pushl	%ebx
	movl	12(%esp),	%ebx
	movl	16(%esp),	%ebp
	pushl	%esi
	pushl	%edi

	movl	(%ebx),		%edi
	movl	4(%ebx),	%esi

	movl	128(%ebp),	%eax
	orl	%eax,		%eax
	jnz	.L001cast_dec_skip
	xorl	%eax,		%eax

	movl	120(%ebp),	%edx
	movl	124(%ebp),	%ecx
	addl	%esi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	xorl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	subl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	addl	%ebx,		%ecx
	xorl	%ecx,		%edi

	movl	112(%ebp),	%edx
	movl	116(%ebp),	%ecx
	subl	%edi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	addl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	xorl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	subl	%ebx,		%ecx
	xorl	%ecx,		%esi

	movl	104(%ebp),	%edx
	movl	108(%ebp),	%ecx
	xorl	%esi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	subl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	addl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	xorl	%ebx,		%ecx
	xorl	%ecx,		%edi

	movl	96(%ebp),	%edx
	movl	100(%ebp),	%ecx
	addl	%edi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	xorl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	subl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	addl	%ebx,		%ecx
	xorl	%ecx,		%esi
.L001cast_dec_skip:

	movl	88(%ebp),	%edx
	movl	92(%ebp),	%ecx
	subl	%esi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	addl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	xorl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	subl	%ebx,		%ecx
	xorl	%ecx,		%edi

	movl	80(%ebp),	%edx
	movl	84(%ebp),	%ecx
	xorl	%edi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	subl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	addl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	xorl	%ebx,		%ecx
	xorl	%ecx,		%esi

	movl	72(%ebp),	%edx
	movl	76(%ebp),	%ecx
	addl	%esi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	xorl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	subl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	addl	%ebx,		%ecx
	xorl	%ecx,		%edi

	movl	64(%ebp),	%edx
	movl	68(%ebp),	%ecx
	subl	%edi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	addl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	xorl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	subl	%ebx,		%ecx
	xorl	%ecx,		%esi

	movl	56(%ebp),	%edx
	movl	60(%ebp),	%ecx
	xorl	%esi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	subl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	addl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	xorl	%ebx,		%ecx
	xorl	%ecx,		%edi

	movl	48(%ebp),	%edx
	movl	52(%ebp),	%ecx
	addl	%edi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	xorl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	subl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	addl	%ebx,		%ecx
	xorl	%ecx,		%esi

	movl	40(%ebp),	%edx
	movl	44(%ebp),	%ecx
	subl	%esi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	addl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	xorl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	subl	%ebx,		%ecx
	xorl	%ecx,		%edi

	movl	32(%ebp),	%edx
	movl	36(%ebp),	%ecx
	xorl	%edi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	subl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	addl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	xorl	%ebx,		%ecx
	xorl	%ecx,		%esi

	movl	24(%ebp),	%edx
	movl	28(%ebp),	%ecx
	addl	%esi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	xorl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	subl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	addl	%ebx,		%ecx
	xorl	%ecx,		%edi

	movl	16(%ebp),	%edx
	movl	20(%ebp),	%ecx
	subl	%edi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	addl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	xorl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	subl	%ebx,		%ecx
	xorl	%ecx,		%esi

	movl	8(%ebp),	%edx
	movl	12(%ebp),	%ecx
	xorl	%esi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	subl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	addl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	xorl	%ebx,		%ecx
	xorl	%ecx,		%edi

	movl	(%ebp),		%edx
	movl	4(%ebp),	%ecx
	addl	%edi,		%edx
	roll	%cl,		%edx
	movl	%edx,		%ebx
	xorl	%ecx,		%ecx
	movb	%dh,		%cl
	andl	$255,		%ebx
	shrl	$16,		%edx
	xorl	%eax,		%eax
	movb	%dh,		%al
	andl	$255,		%edx
	movl	CAST_S_table0(,%ecx,4),%ecx
	movl	CAST_S_table1(,%ebx,4),%ebx
	xorl	%ebx,		%ecx
	movl	CAST_S_table2(,%eax,4),%ebx
	subl	%ebx,		%ecx
	movl	CAST_S_table3(,%edx,4),%ebx
	addl	%ebx,		%ecx
	xorl	%ecx,		%esi
	nop
	movl	20(%esp),	%eax
	movl	%edi,		4(%eax)
	movl	%esi,		(%eax)
	popl	%edi
	popl	%esi
	popl	%ebx
	popl	%ebp
	ret
.L_CAST_decrypt_end:
	.size	CAST_decrypt,.L_CAST_decrypt_end-CAST_decrypt
.ident	"CAST_decrypt"
.text
	.align 16
.globl CAST_cbc_encrypt
	.type	CAST_cbc_encrypt,@function
CAST_cbc_encrypt:

	pushl	%ebp
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	movl	28(%esp),	%ebp

	movl	36(%esp),	%ebx
	movl	(%ebx),		%esi
	movl	4(%ebx),	%edi
	pushl	%edi
	pushl	%esi
	pushl	%edi
	pushl	%esi
	movl	%esp,		%ebx
	movl	36(%esp),	%esi
	movl	40(%esp),	%edi

	movl	56(%esp),	%ecx

	movl	48(%esp),	%eax
	pushl	%eax
	pushl	%ebx
	cmpl	$0,		%ecx
	jz	.L002decrypt
	andl	$4294967288,	%ebp
	movl	8(%esp),	%eax
	movl	12(%esp),	%ebx
	jz	.L003encrypt_finish
.L004encrypt_loop:
	movl	(%esi),		%ecx
	movl	4(%esi),	%edx
	xorl	%ecx,		%eax
	xorl	%edx,		%ebx
.byte 15
.byte 200	
.byte 15
.byte 203	
	movl	%eax,		8(%esp)
	movl	%ebx,		12(%esp)
	call	CAST_encrypt
	movl	8(%esp),	%eax
	movl	12(%esp),	%ebx
.byte 15
.byte 200	
.byte 15
.byte 203	
	movl	%eax,		(%edi)
	movl	%ebx,		4(%edi)
	addl	$8,		%esi
	addl	$8,		%edi
	subl	$8,		%ebp
	jnz	.L004encrypt_loop
.L003encrypt_finish:
	movl	52(%esp),	%ebp
	andl	$7,		%ebp
	jz	.L005finish
	call	.L006PIC_point
.L006PIC_point:
	popl	%edx
	leal	.L007cbc_enc_jmp_table-.L006PIC_point(%edx),%ecx
	movl	(%ecx,%ebp,4),	%ebp
	addl	%edx,		%ebp
	xorl	%ecx,		%ecx
	xorl	%edx,		%edx
	jmp	*%ebp
.L008ej7:
	xorl	%edx,		%edx
	movb	6(%esi),	%dh
	sall	$8,		%edx
.L009ej6:
	movb	5(%esi),	%dh
.L010ej5:
	movb	4(%esi),	%dl
.L011ej4:
	movl	(%esi),		%ecx
	jmp	.L012ejend
.L013ej3:
	movb	2(%esi),	%ch
	xorl	%ecx,		%ecx
	sall	$8,		%ecx
.L014ej2:
	movb	1(%esi),	%ch
.L015ej1:
	movb	(%esi),		%cl
.L012ejend:
	xorl	%ecx,		%eax
	xorl	%edx,		%ebx
.byte 15
.byte 200	
.byte 15
.byte 203	
	movl	%eax,		8(%esp)
	movl	%ebx,		12(%esp)
	call	CAST_encrypt
	movl	8(%esp),	%eax
	movl	12(%esp),	%ebx
.byte 15
.byte 200	
.byte 15
.byte 203	
	movl	%eax,		(%edi)
	movl	%ebx,		4(%edi)
	jmp	.L005finish
.align 16
.L002decrypt:
	andl	$4294967288,	%ebp
	movl	16(%esp),	%eax
	movl	20(%esp),	%ebx
	jz	.L016decrypt_finish
.L017decrypt_loop:
	movl	(%esi),		%eax
	movl	4(%esi),	%ebx
.byte 15
.byte 200	
.byte 15
.byte 203	
	movl	%eax,		8(%esp)
	movl	%ebx,		12(%esp)
	call	CAST_decrypt
	movl	8(%esp),	%eax
	movl	12(%esp),	%ebx
.byte 15
.byte 200	
.byte 15
.byte 203	
	movl	16(%esp),	%ecx
	movl	20(%esp),	%edx
	xorl	%eax,		%ecx
	xorl	%ebx,		%edx
	movl	(%esi),		%eax
	movl	4(%esi),	%ebx
	movl	%ecx,		(%edi)
	movl	%edx,		4(%edi)
	movl	%eax,		16(%esp)
	movl	%ebx,		20(%esp)
	addl	$8,		%esi
	addl	$8,		%edi
	subl	$8,		%ebp
	jnz	.L017decrypt_loop
.L016decrypt_finish:
	movl	52(%esp),	%ebp
	andl	$7,		%ebp
	jz	.L005finish
	movl	(%esi),		%eax
	movl	4(%esi),	%ebx
.byte 15
.byte 200	
.byte 15
.byte 203	
	movl	%eax,		8(%esp)
	movl	%ebx,		12(%esp)
	call	CAST_decrypt
	movl	8(%esp),	%eax
	movl	12(%esp),	%ebx
.byte 15
.byte 200	
.byte 15
.byte 203	
	movl	16(%esp),	%ecx
	movl	20(%esp),	%edx
	xorl	%eax,		%ecx
	xorl	%ebx,		%edx
	movl	(%esi),		%eax
	movl	4(%esi),	%ebx
.L018dj7:
	rorl	$16,		%edx
	movb	%dl,		6(%edi)
	shrl	$16,		%edx
.L019dj6:
	movb	%dh,		5(%edi)
.L020dj5:
	movb	%dl,		4(%edi)
.L021dj4:
	movl	%ecx,		(%edi)
	jmp	.L022djend
.L023dj3:
	rorl	$16,		%ecx
	movb	%cl,		2(%edi)
	sall	$16,		%ecx
.L024dj2:
	movb	%ch,		1(%esi)
.L025dj1:
	movb	%cl,		(%esi)
.L022djend:
	jmp	.L005finish
.align 16
.L005finish:
	movl	60(%esp),	%ecx
	addl	$24,		%esp
	movl	%eax,		(%ecx)
	movl	%ebx,		4(%ecx)
	popl	%edi
	popl	%esi
	popl	%ebx
	popl	%ebp
	ret
.align 16
.L007cbc_enc_jmp_table:
	.long 0
	.long .L015ej1-.L006PIC_point
	.long .L014ej2-.L006PIC_point
	.long .L013ej3-.L006PIC_point
	.long .L011ej4-.L006PIC_point
	.long .L010ej5-.L006PIC_point
	.long .L009ej6-.L006PIC_point
	.long .L008ej7-.L006PIC_point
.L_CAST_cbc_encrypt_end:
	.size	CAST_cbc_encrypt,.L_CAST_cbc_encrypt_end-CAST_cbc_encrypt
.ident	"desasm.pl"
