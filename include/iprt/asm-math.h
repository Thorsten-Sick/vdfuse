/** @file
 * IPRT - Assembly Routines for Optimizing some Integers Math Operations.
 */

/*
 * Copyright (C) 2006-2010 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 *
 * The contents of this file may alternatively be used under the terms
 * of the Common Development and Distribution License Version 1.0
 * (CDDL) only, as it comes in the "COPYING.CDDL" file of the
 * VirtualBox OSE distribution, in which case the provisions of the
 * CDDL are applicable instead of those of the GPL.
 *
 * You may elect to license modified versions of this file under the
 * terms and conditions of either the GPL or the CDDL or both.
 */

#ifndef ___iprt_asm_math_h
#define ___iprt_asm_math_h

#include <iprt/types.h>


/** @defgroup grp_rt_asm_math   Interger Math Optimizations
 * @ingroup grp_rt_asm
 * @{ */

/**
 * Multiplies two unsigned 32-bit values returning an unsigned 64-bit result.
 *
 * @returns u32F1 * u32F2.
 */
#if RT_INLINE_ASM_EXTERNAL && defined(RT_ARCH_X86)
DECLASM(uint64_t) ASMMult2xU32RetU64(uint32_t u32F1, uint32_t u32F2);
#else
DECLINLINE(uint64_t) ASMMult2xU32RetU64(uint32_t u32F1, uint32_t u32F2)
{
# ifdef RT_ARCH_X86
    uint64_t u64;
#  if RT_INLINE_ASM_GNU_STYLE
    __asm__ __volatile__("mull %%edx"
                         : "=A" (u64)
                         : "a" (u32F2), "d" (u32F1));
#  else
    __asm
    {
        mov     edx, [u32F1]
        mov     eax, [u32F2]
        mul     edx
        mov     dword ptr [u64], eax
        mov     dword ptr [u64 + 4], edx
    }
#  endif
    return u64;
# else  /* generic: */
    return (uint64_t)u32F1 * u32F2;
# endif
}
#endif


/**
 * Multiplies two signed 32-bit values returning a signed 64-bit result.
 *
 * @returns u32F1 * u32F2.
 */
#if RT_INLINE_ASM_EXTERNAL && defined(RT_ARCH_X86)
DECLASM(int64_t) ASMMult2xS32RetS64(int32_t i32F1, int32_t i32F2);
#else
DECLINLINE(int64_t) ASMMult2xS32RetS64(int32_t i32F1, int32_t i32F2)
{
# ifdef RT_ARCH_X86
    int64_t i64;
#  if RT_INLINE_ASM_GNU_STYLE
    __asm__ __volatile__("imull %%edx"
                         : "=A" (i64)
                         : "a" (i32F2), "d" (i32F1));
#  else
    __asm
    {
        mov     edx, [i32F1]
        mov     eax, [i32F2]
        imul    edx
        mov     dword ptr [i64], eax
        mov     dword ptr [i64 + 4], edx
    }
#  endif
    return i64;
# else  /* generic: */
    return (int64_t)i32F1 * i32F2;
# endif
}
#endif


/**
 * Divides a 64-bit unsigned by a 32-bit unsigned returning an unsigned 32-bit result.
 *
 * @returns u64 / u32.
 */
#if RT_INLINE_ASM_EXTERNAL && defined(RT_ARCH_X86)
DECLASM(uint32_t) ASMDivU64ByU32RetU32(uint64_t u64, uint32_t u32);
#else
DECLINLINE(uint32_t) ASMDivU64ByU32RetU32(uint64_t u64, uint32_t u32)
{
# ifdef RT_ARCH_X86
#  if RT_INLINE_ASM_GNU_STYLE
    RTCCUINTREG uDummy;
    __asm__ __volatile__("divl %3"
                         : "=a" (u32), "=d"(uDummy)
                         : "A" (u64), "r" (u32));
#  else
    __asm
    {
        mov     eax, dword ptr [u64]
        mov     edx, dword ptr [u64 + 4]
        mov     ecx, [u32]
        div     ecx
        mov     [u32], eax
    }
#  endif
    return u32;
# else   /* generic: */
    return (uint32_t)(u64 / u32);
# endif
}
#endif


/**
 * Divides a 64-bit signed by a 32-bit signed returning a signed 32-bit result.
 *
 * @returns u64 / u32.
 */
#if RT_INLINE_ASM_EXTERNAL && defined(RT_ARCH_X86)
DECLASM(int32_t) ASMDivS64ByS32RetS32(int64_t i64, int32_t i32);
#else
DECLINLINE(int32_t) ASMDivS64ByS32RetS32(int64_t i64, int32_t i32)
{
# ifdef RT_ARCH_X86
#  if RT_INLINE_ASM_GNU_STYLE
    RTCCUINTREG iDummy;
    __asm__ __volatile__("idivl %3"
                         : "=a" (i32), "=d"(iDummy)
                         : "A" (i64), "r" (i32));
#  else
    __asm
    {
        mov     eax, dword ptr [i64]
        mov     edx, dword ptr [i64 + 4]
        mov     ecx, [i32]
        idiv    ecx
        mov     [i32], eax
    }
#  endif
    return i32;
# else  /* generic: */
    return (int32_t)(i64 / i32);
# endif
}
#endif


/**
 * Performs 64-bit unsigned by a 32-bit unsigned division with a 32-bit unsigned result,
 * returning the rest.
 *
 * @returns u64 % u32.
 *
 * @remarks It is important that the result is <= UINT32_MAX or we'll overflow and crash.
 */
#if RT_INLINE_ASM_EXTERNAL && defined(RT_ARCH_X86)
DECLASM(uint32_t) ASMModU64ByU32RetU32(uint64_t u64, uint32_t u32);
#else
DECLINLINE(uint32_t) ASMModU64ByU32RetU32(uint64_t u64, uint32_t u32)
{
# ifdef RT_ARCH_X86
#  if RT_INLINE_ASM_GNU_STYLE
    RTCCUINTREG uDummy;
    __asm__ __volatile__("divl %3"
                         : "=a" (uDummy), "=d"(u32)
                         : "A" (u64), "r" (u32));
#  else
    __asm
    {
        mov     eax, dword ptr [u64]
        mov     edx, dword ptr [u64 + 4]
        mov     ecx, [u32]
        div     ecx
        mov     [u32], edx
    }
#  endif
    return u32;
# else  /* generic: */
    return (uint32_t)(u64 % u32);
# endif
}
#endif


/**
 * Performs 64-bit signed by a 32-bit signed division with a 32-bit signed result,
 * returning the rest.
 *
 * @returns u64 % u32.
 *
 * @remarks It is important that the result is <= UINT32_MAX or we'll overflow and crash.
 */
#if RT_INLINE_ASM_EXTERNAL && defined(RT_ARCH_X86)
DECLASM(int32_t) ASMModS64ByS32RetS32(int64_t i64, int32_t i32);
#else
DECLINLINE(int32_t) ASMModS64ByS32RetS32(int64_t i64, int32_t i32)
{
# ifdef RT_ARCH_X86
#  if RT_INLINE_ASM_GNU_STYLE
    RTCCUINTREG iDummy;
    __asm__ __volatile__("idivl %3"
                         : "=a" (iDummy), "=d"(i32)
                         : "A" (i64), "r" (i32));
#  else
    __asm
    {
        mov     eax, dword ptr [i64]
        mov     edx, dword ptr [i64 + 4]
        mov     ecx, [i32]
        idiv    ecx
        mov     [i32], edx
    }
#  endif
    return i32;
# else  /* generic: */
    return (int32_t)(i64 % i32);
# endif
}
#endif


/**
 * Multiple a 64-bit by a 32-bit integer and divide the result by a 32-bit integer
 * using a 96 bit intermediate result.
 * @note    Don't use 64-bit C arithmetic here since some gcc compilers generate references to
 *          __udivdi3 and __umoddi3 even if this inline function is not used.
 *
 * @returns (u64A * u32B) / u32C.
 * @param   u64A    The 64-bit value.
 * @param   u32B    The 32-bit value to multiple by A.
 * @param   u32C    The 32-bit value to divide A*B by.
 *
 * @remarks Architecture specific.
 */
#if RT_INLINE_ASM_EXTERNAL || !defined(__GNUC__) || (!defined(RT_ARCH_AMD64) && !defined(RT_ARCH_X86))
DECLASM(uint64_t) ASMMultU64ByU32DivByU32(uint64_t u64A, uint32_t u32B, uint32_t u32C);
#else
DECLINLINE(uint64_t) ASMMultU64ByU32DivByU32(uint64_t u64A, uint32_t u32B, uint32_t u32C)
{
# if RT_INLINE_ASM_GNU_STYLE
#  ifdef RT_ARCH_AMD64
    uint64_t u64Result, u64Spill;
    __asm__ __volatile__("mulq %2\n\t"
                         "divq %3\n\t"
                         : "=a" (u64Result),
                           "=d" (u64Spill)
                         : "r" ((uint64_t)u32B),
                           "r" ((uint64_t)u32C),
                           "0" (u64A),
                           "1" (0));
    return u64Result;
#  else
    uint32_t u32Dummy;
    uint64_t u64Result;
    __asm__ __volatile__("mull %%ecx       \n\t" /* eax = u64Lo.lo = (u64A.lo * u32B).lo
                                                    edx = u64Lo.hi = (u64A.lo * u32B).hi */
                         "xchg %%eax,%%esi \n\t" /* esi = u64Lo.lo
                                                    eax = u64A.hi */
                         "xchg %%edx,%%edi \n\t" /* edi = u64Low.hi
                                                    edx = u32C */
                         "xchg %%edx,%%ecx \n\t" /* ecx = u32C
                                                    edx = u32B */
                         "mull %%edx       \n\t" /* eax = u64Hi.lo = (u64A.hi * u32B).lo
                                                    edx = u64Hi.hi = (u64A.hi * u32B).hi */
                         "addl %%edi,%%eax \n\t" /* u64Hi.lo += u64Lo.hi */
                         "adcl $0,%%edx    \n\t" /* u64Hi.hi += carry */
                         "divl %%ecx       \n\t" /* eax = u64Hi / u32C
                                                    edx = u64Hi % u32C */
                         "movl %%eax,%%edi \n\t" /* edi = u64Result.hi = u64Hi / u32C */
                         "movl %%esi,%%eax \n\t" /* eax = u64Lo.lo */
                         "divl %%ecx       \n\t" /* u64Result.lo */
                         "movl %%edi,%%edx \n\t" /* u64Result.hi */
                         : "=A"(u64Result), "=c"(u32Dummy),
                           "=S"(u32Dummy), "=D"(u32Dummy)
                         : "a"((uint32_t)u64A),
                           "S"((uint32_t)(u64A >> 32)),
                           "c"(u32B),
                           "D"(u32C));
    return u64Result;
#  endif
# else
    RTUINT64U   u;
    uint64_t    u64Lo = (uint64_t)(u64A & 0xffffffff) * u32B;
    uint64_t    u64Hi = (uint64_t)(u64A >> 32)        * u32B;
    u64Hi  += (u64Lo >> 32);
    u.s.Hi = (uint32_t)(u64Hi / u32C);
    u.s.Lo = (uint32_t)((((u64Hi % u32C) << 32) + (u64Lo & 0xffffffff)) / u32C);
    return u.u;
# endif
}
#endif

/** @} */
#endif

