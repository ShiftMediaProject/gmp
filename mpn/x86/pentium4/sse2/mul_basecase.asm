dnl  Pentium-4 mpn_mul_basecase -- Multiply two limb vectors and store the
dnl  result in a third limb vector.  This is just a placeholder for some real
dnl  code, avoiding the pessimal default mpn/x86/mul_basecase.asm.  We let this
dnl  live in the sse2 subdirectory, since we want this used only when the sse2
dnl  mul_1.asm and addmul_1.asm files are used.

dnl  Copyright 2001 Free Software Foundation, Inc.
dnl 
dnl  This file is part of the GNU MP Library.
dnl 
dnl  The GNU MP Library is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Lesser General Public License as
dnl  published by the Free Software Foundation; either version 2.1 of the
dnl  License, or (at your option) any later version.
dnl 
dnl  The GNU MP Library is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl  Lesser General Public License for more details.
dnl 
dnl  You should have received a copy of the GNU Lesser General Public
dnl  License along with the GNU MP Library; see the file COPYING.LIB.  If
dnl  not, write to the Free Software Foundation, Inc., 59 Temple Place -
dnl  Suite 330, Boston, MA 02111-1307, USA.


include(`../config.m4')

	TEXT
	ALIGN(16)
PROLOGUE(mpn_mul_basecase)
	subl $12,%esp
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 32(%esp),%ebx
	movl 40(%esp),%ebp
	movl 44(%esp),%edi
	movl 48(%esp),%esi
	pushl (%edi)
	pushl %ebp
	movl 44(%esp),%eax
	pushl %eax
	pushl %ebx
	call mpn_mul_1
	movl %eax,(%ebx,%ebp,4)
	addl $4,%ebx
	addl $4,%edi
	addl $16,%esp
	subl $1,%esi
	jz L(end)
L(loop):
	pushl (%edi)
	pushl %ebp
	movl 44(%esp),%eax
	pushl %eax
	pushl %ebx
	call mpn_addmul_1
	movl %eax,(%ebx,%ebp,4)
	addl $4,%ebx
	addl $4,%edi
	addl $16,%esp
	subl $1,%esi
	jnz L(loop)
L(end):
	popl %ebx
	popl %esi
	popl %edi
	popl %ebp
	addl $12,%esp
	ret
EPILOGUE()
