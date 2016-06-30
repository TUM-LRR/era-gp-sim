	.file	"AbstractNodeFactories.cpp"
	.section .rdata,"dr"
_ZStL19piecewise_construct:
	.space 1
_ZStL13allocator_arg:
	.space 1
_ZStL6ignore:
	.space 1
	.text
	.align 2
	.globl	_ZN30AbstractInstructionNodeFactoryD2Ev
	.def	_ZN30AbstractInstructionNodeFactoryD2Ev;	.scl	2;	.type	32;	.endef
	.seh_proc	_ZN30AbstractInstructionNodeFactoryD2Ev
_ZN30AbstractInstructionNodeFactoryD2Ev:
.LFB1651:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movq	16(%rbp), %rax
	leaq	16+_ZTV30AbstractInstructionNodeFactory(%rip), %rdx
	movq	%rdx, (%rax)
	movl	$0, %eax
	testl	%eax, %eax
	je	.L1
	movq	16(%rbp), %rcx
	call	_ZdlPv
	nop
.L1:
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_ZN30AbstractInstructionNodeFactoryD1Ev
	.def	_ZN30AbstractInstructionNodeFactoryD1Ev;	.scl	2;	.type	32;	.endef
	.set	_ZN30AbstractInstructionNodeFactoryD1Ev,_ZN30AbstractInstructionNodeFactoryD2Ev
	.align 2
	.globl	_ZN30AbstractInstructionNodeFactoryD0Ev
	.def	_ZN30AbstractInstructionNodeFactoryD0Ev;	.scl	2;	.type	32;	.endef
	.seh_proc	_ZN30AbstractInstructionNodeFactoryD0Ev
_ZN30AbstractInstructionNodeFactoryD0Ev:
.LFB1653:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movq	16(%rbp), %rcx
	call	_ZN30AbstractInstructionNodeFactoryD1Ev
	movq	16(%rbp), %rcx
	call	_ZdlPv
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.align 2
	.globl	_ZN28AbstractImmediateNodeFactoryD2Ev
	.def	_ZN28AbstractImmediateNodeFactoryD2Ev;	.scl	2;	.type	32;	.endef
	.seh_proc	_ZN28AbstractImmediateNodeFactoryD2Ev
_ZN28AbstractImmediateNodeFactoryD2Ev:
.LFB1655:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movq	16(%rbp), %rax
	leaq	16+_ZTV28AbstractImmediateNodeFactory(%rip), %rdx
	movq	%rdx, (%rax)
	movl	$0, %eax
	testl	%eax, %eax
	je	.L6
	movq	16(%rbp), %rcx
	call	_ZdlPv
	nop
.L6:
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_ZN28AbstractImmediateNodeFactoryD1Ev
	.def	_ZN28AbstractImmediateNodeFactoryD1Ev;	.scl	2;	.type	32;	.endef
	.set	_ZN28AbstractImmediateNodeFactoryD1Ev,_ZN28AbstractImmediateNodeFactoryD2Ev
	.align 2
	.globl	_ZN28AbstractImmediateNodeFactoryD0Ev
	.def	_ZN28AbstractImmediateNodeFactoryD0Ev;	.scl	2;	.type	32;	.endef
	.seh_proc	_ZN28AbstractImmediateNodeFactoryD0Ev
_ZN28AbstractImmediateNodeFactoryD0Ev:
.LFB1657:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movq	16(%rbp), %rcx
	call	_ZN28AbstractImmediateNodeFactoryD1Ev
	movq	16(%rbp), %rcx
	call	_ZdlPv
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.align 2
	.globl	_ZN33AbstractRegisterAccessNodeFactoryD2Ev
	.def	_ZN33AbstractRegisterAccessNodeFactoryD2Ev;	.scl	2;	.type	32;	.endef
	.seh_proc	_ZN33AbstractRegisterAccessNodeFactoryD2Ev
_ZN33AbstractRegisterAccessNodeFactoryD2Ev:
.LFB1659:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movq	16(%rbp), %rax
	leaq	16+_ZTV33AbstractRegisterAccessNodeFactory(%rip), %rdx
	movq	%rdx, (%rax)
	movl	$0, %eax
	testl	%eax, %eax
	je	.L11
	movq	16(%rbp), %rcx
	call	_ZdlPv
	nop
.L11:
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_ZN33AbstractRegisterAccessNodeFactoryD1Ev
	.def	_ZN33AbstractRegisterAccessNodeFactoryD1Ev;	.scl	2;	.type	32;	.endef
	.set	_ZN33AbstractRegisterAccessNodeFactoryD1Ev,_ZN33AbstractRegisterAccessNodeFactoryD2Ev
	.align 2
	.globl	_ZN33AbstractRegisterAccessNodeFactoryD0Ev
	.def	_ZN33AbstractRegisterAccessNodeFactoryD0Ev;	.scl	2;	.type	32;	.endef
	.seh_proc	_ZN33AbstractRegisterAccessNodeFactoryD0Ev
_ZN33AbstractRegisterAccessNodeFactoryD0Ev:
.LFB1661:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movq	16(%rbp), %rcx
	call	_ZN33AbstractRegisterAccessNodeFactoryD1Ev
	movq	16(%rbp), %rcx
	call	_ZdlPv
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.align 2
	.globl	_ZN31AbstractMemoryAccessNodeFactoryD2Ev
	.def	_ZN31AbstractMemoryAccessNodeFactoryD2Ev;	.scl	2;	.type	32;	.endef
	.seh_proc	_ZN31AbstractMemoryAccessNodeFactoryD2Ev
_ZN31AbstractMemoryAccessNodeFactoryD2Ev:
.LFB1663:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movq	16(%rbp), %rax
	leaq	16+_ZTV31AbstractMemoryAccessNodeFactory(%rip), %rdx
	movq	%rdx, (%rax)
	movl	$0, %eax
	testl	%eax, %eax
	je	.L16
	movq	16(%rbp), %rcx
	call	_ZdlPv
	nop
.L16:
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_ZN31AbstractMemoryAccessNodeFactoryD1Ev
	.def	_ZN31AbstractMemoryAccessNodeFactoryD1Ev;	.scl	2;	.type	32;	.endef
	.set	_ZN31AbstractMemoryAccessNodeFactoryD1Ev,_ZN31AbstractMemoryAccessNodeFactoryD2Ev
	.align 2
	.globl	_ZN31AbstractMemoryAccessNodeFactoryD0Ev
	.def	_ZN31AbstractMemoryAccessNodeFactoryD0Ev;	.scl	2;	.type	32;	.endef
	.seh_proc	_ZN31AbstractMemoryAccessNodeFactoryD0Ev
_ZN31AbstractMemoryAccessNodeFactoryD0Ev:
.LFB1665:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movq	16(%rbp), %rcx
	call	_ZN31AbstractMemoryAccessNodeFactoryD1Ev
	movq	16(%rbp), %rcx
	call	_ZdlPv
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.align 2
	.globl	_ZN31AbstractArithmeticOpNodeFactoryD2Ev
	.def	_ZN31AbstractArithmeticOpNodeFactoryD2Ev;	.scl	2;	.type	32;	.endef
	.seh_proc	_ZN31AbstractArithmeticOpNodeFactoryD2Ev
_ZN31AbstractArithmeticOpNodeFactoryD2Ev:
.LFB1667:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movq	16(%rbp), %rax
	leaq	16+_ZTV31AbstractArithmeticOpNodeFactory(%rip), %rdx
	movq	%rdx, (%rax)
	movl	$0, %eax
	testl	%eax, %eax
	je	.L21
	movq	16(%rbp), %rcx
	call	_ZdlPv
	nop
.L21:
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_ZN31AbstractArithmeticOpNodeFactoryD1Ev
	.def	_ZN31AbstractArithmeticOpNodeFactoryD1Ev;	.scl	2;	.type	32;	.endef
	.set	_ZN31AbstractArithmeticOpNodeFactoryD1Ev,_ZN31AbstractArithmeticOpNodeFactoryD2Ev
	.align 2
	.globl	_ZN31AbstractArithmeticOpNodeFactoryD0Ev
	.def	_ZN31AbstractArithmeticOpNodeFactoryD0Ev;	.scl	2;	.type	32;	.endef
	.seh_proc	_ZN31AbstractArithmeticOpNodeFactoryD0Ev
_ZN31AbstractArithmeticOpNodeFactoryD0Ev:
.LFB1669:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movq	16(%rbp), %rcx
	call	_ZN31AbstractArithmeticOpNodeFactoryD1Ev
	movq	16(%rbp), %rcx
	call	_ZdlPv
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_ZTV31AbstractArithmeticOpNodeFactory
	.section	.rdata$_ZTV31AbstractArithmeticOpNodeFactory,"dr"
	.linkonce same_size
	.align 32
_ZTV31AbstractArithmeticOpNodeFactory:
	.quad	0
	.quad	_ZTI31AbstractArithmeticOpNodeFactory
	.quad	_ZN31AbstractArithmeticOpNodeFactoryD1Ev
	.quad	_ZN31AbstractArithmeticOpNodeFactoryD0Ev
	.quad	__cxa_pure_virtual
	.globl	_ZTV31AbstractMemoryAccessNodeFactory
	.section	.rdata$_ZTV31AbstractMemoryAccessNodeFactory,"dr"
	.linkonce same_size
	.align 32
_ZTV31AbstractMemoryAccessNodeFactory:
	.quad	0
	.quad	_ZTI31AbstractMemoryAccessNodeFactory
	.quad	_ZN31AbstractMemoryAccessNodeFactoryD1Ev
	.quad	_ZN31AbstractMemoryAccessNodeFactoryD0Ev
	.quad	__cxa_pure_virtual
	.globl	_ZTV33AbstractRegisterAccessNodeFactory
	.section	.rdata$_ZTV33AbstractRegisterAccessNodeFactory,"dr"
	.linkonce same_size
	.align 32
_ZTV33AbstractRegisterAccessNodeFactory:
	.quad	0
	.quad	_ZTI33AbstractRegisterAccessNodeFactory
	.quad	_ZN33AbstractRegisterAccessNodeFactoryD1Ev
	.quad	_ZN33AbstractRegisterAccessNodeFactoryD0Ev
	.quad	__cxa_pure_virtual
	.globl	_ZTV28AbstractImmediateNodeFactory
	.section	.rdata$_ZTV28AbstractImmediateNodeFactory,"dr"
	.linkonce same_size
	.align 32
_ZTV28AbstractImmediateNodeFactory:
	.quad	0
	.quad	_ZTI28AbstractImmediateNodeFactory
	.quad	_ZN28AbstractImmediateNodeFactoryD1Ev
	.quad	_ZN28AbstractImmediateNodeFactoryD0Ev
	.quad	__cxa_pure_virtual
	.globl	_ZTV30AbstractInstructionNodeFactory
	.section	.rdata$_ZTV30AbstractInstructionNodeFactory,"dr"
	.linkonce same_size
	.align 32
_ZTV30AbstractInstructionNodeFactory:
	.quad	0
	.quad	_ZTI30AbstractInstructionNodeFactory
	.quad	_ZN30AbstractInstructionNodeFactoryD1Ev
	.quad	_ZN30AbstractInstructionNodeFactoryD0Ev
	.quad	__cxa_pure_virtual
	.globl	_ZTI31AbstractArithmeticOpNodeFactory
	.section	.rdata$_ZTI31AbstractArithmeticOpNodeFactory,"dr"
	.linkonce same_size
	.align 16
_ZTI31AbstractArithmeticOpNodeFactory:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTS31AbstractArithmeticOpNodeFactory
	.globl	_ZTS31AbstractArithmeticOpNodeFactory
	.section	.rdata$_ZTS31AbstractArithmeticOpNodeFactory,"dr"
	.linkonce same_size
	.align 32
_ZTS31AbstractArithmeticOpNodeFactory:
	.ascii "31AbstractArithmeticOpNodeFactory\0"
	.globl	_ZTI31AbstractMemoryAccessNodeFactory
	.section	.rdata$_ZTI31AbstractMemoryAccessNodeFactory,"dr"
	.linkonce same_size
	.align 16
_ZTI31AbstractMemoryAccessNodeFactory:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTS31AbstractMemoryAccessNodeFactory
	.globl	_ZTS31AbstractMemoryAccessNodeFactory
	.section	.rdata$_ZTS31AbstractMemoryAccessNodeFactory,"dr"
	.linkonce same_size
	.align 32
_ZTS31AbstractMemoryAccessNodeFactory:
	.ascii "31AbstractMemoryAccessNodeFactory\0"
	.globl	_ZTI33AbstractRegisterAccessNodeFactory
	.section	.rdata$_ZTI33AbstractRegisterAccessNodeFactory,"dr"
	.linkonce same_size
	.align 16
_ZTI33AbstractRegisterAccessNodeFactory:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTS33AbstractRegisterAccessNodeFactory
	.globl	_ZTS33AbstractRegisterAccessNodeFactory
	.section	.rdata$_ZTS33AbstractRegisterAccessNodeFactory,"dr"
	.linkonce same_size
	.align 32
_ZTS33AbstractRegisterAccessNodeFactory:
	.ascii "33AbstractRegisterAccessNodeFactory\0"
	.globl	_ZTI28AbstractImmediateNodeFactory
	.section	.rdata$_ZTI28AbstractImmediateNodeFactory,"dr"
	.linkonce same_size
	.align 16
_ZTI28AbstractImmediateNodeFactory:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTS28AbstractImmediateNodeFactory
	.globl	_ZTS28AbstractImmediateNodeFactory
	.section	.rdata$_ZTS28AbstractImmediateNodeFactory,"dr"
	.linkonce same_size
	.align 16
_ZTS28AbstractImmediateNodeFactory:
	.ascii "28AbstractImmediateNodeFactory\0"
	.globl	_ZTI30AbstractInstructionNodeFactory
	.section	.rdata$_ZTI30AbstractInstructionNodeFactory,"dr"
	.linkonce same_size
	.align 16
_ZTI30AbstractInstructionNodeFactory:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTS30AbstractInstructionNodeFactory
	.globl	_ZTS30AbstractInstructionNodeFactory
	.section	.rdata$_ZTS30AbstractInstructionNodeFactory,"dr"
	.linkonce same_size
	.align 32
_ZTS30AbstractInstructionNodeFactory:
	.ascii "30AbstractInstructionNodeFactory\0"
	.section .rdata,"dr"
	.align 4
_ZN9__gnu_cxxL21__default_lock_policyE:
	.long	2
	.ident	"GCC: (GNU) 4.8.3"
	.def	_ZdlPv;	.scl	2;	.type	32;	.endef
	.def	__cxa_pure_virtual;	.scl	2;	.type	32;	.endef
