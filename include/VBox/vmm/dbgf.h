/** @file
 * DBGF - Debugger Facility.
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

#ifndef ___VBox_vmm_dbgf_h
#define ___VBox_vmm_dbgf_h

#include <VBox/types.h>
#include <VBox/log.h>                   /* LOG_ENABLED */
#include <VBox/vmm/vmm.h>
#include <VBox/vmm/dbgfsel.h>

#include <iprt/stdarg.h>
#include <iprt/dbg.h>

RT_C_DECLS_BEGIN


/** @defgroup grp_dbgf     The Debugger Facility API
 * @{
 */

#if defined(IN_RC) || defined(IN_RING0)
/** @addgroup grp_dbgf_rz  The RZ DBGF API
 * @ingroup grp_dbgf
 * @{
 */
VMMRZDECL(int) DBGFRZTrap01Handler(PVM pVM, PVMCPU pVCpu, PCPUMCTXCORE pRegFrame, RTGCUINTREG uDr6);
VMMRZDECL(int) DBGFRZTrap03Handler(PVM pVM, PVMCPU pVCpu, PCPUMCTXCORE pRegFrame);
/** @} */
#endif



/**
 * Mixed address.
 */
typedef struct DBGFADDRESS
{
    /** The flat address. */
    RTGCUINTPTR FlatPtr;
    /** The selector offset address. */
    RTGCUINTPTR off;
    /** The selector. DBGF_SEL_FLAT is a legal value. */
    RTSEL       Sel;
    /** Flags describing further details about the address. */
    uint16_t    fFlags;
} DBGFADDRESS;
/** Pointer to a mixed address. */
typedef DBGFADDRESS *PDBGFADDRESS;
/** Pointer to a const mixed address. */
typedef const DBGFADDRESS *PCDBGFADDRESS;

/** @name DBGFADDRESS Flags.
 * @{ */
/** A 16:16 far address. */
#define DBGFADDRESS_FLAGS_FAR16         0
/** A 16:32 far address. */
#define DBGFADDRESS_FLAGS_FAR32         1
/** A 16:64 far address. */
#define DBGFADDRESS_FLAGS_FAR64         2
/** A flat address. */
#define DBGFADDRESS_FLAGS_FLAT          3
/** A physical address. */
#define DBGFADDRESS_FLAGS_PHYS          4
/** A physical address. */
#define DBGFADDRESS_FLAGS_RING0         5
/** The address type mask. */
#define DBGFADDRESS_FLAGS_TYPE_MASK     7

/** Set if the address is valid. */
#define DBGFADDRESS_FLAGS_VALID         RT_BIT(3)

/** The address is within the hypervisor memoary area (HMA).
 * If not set, the address can be assumed to be a guest address. */
#define DBGFADDRESS_FLAGS_HMA           RT_BIT(4)

/** Checks if the mixed address is flat or not. */
#define DBGFADDRESS_IS_FLAT(pAddress)    ( ((pAddress)->fFlags & DBGFADDRESS_FLAGS_TYPE_MASK) == DBGFADDRESS_FLAGS_FLAT )
/** Checks if the mixed address is flat or not. */
#define DBGFADDRESS_IS_PHYS(pAddress)    ( ((pAddress)->fFlags & DBGFADDRESS_FLAGS_TYPE_MASK) == DBGFADDRESS_FLAGS_PHYS )
/** Checks if the mixed address is far 16:16 or not. */
#define DBGFADDRESS_IS_FAR16(pAddress)   ( ((pAddress)->fFlags & DBGFADDRESS_FLAGS_TYPE_MASK) == DBGFADDRESS_FLAGS_FAR16 )
/** Checks if the mixed address is far 16:32 or not. */
#define DBGFADDRESS_IS_FAR32(pAddress)   ( ((pAddress)->fFlags & DBGFADDRESS_FLAGS_TYPE_MASK) == DBGFADDRESS_FLAGS_FAR32 )
/** Checks if the mixed address is far 16:64 or not. */
#define DBGFADDRESS_IS_FAR64(pAddress)   ( ((pAddress)->fFlags & DBGFADDRESS_FLAGS_TYPE_MASK) == DBGFADDRESS_FLAGS_FAR64 )
/** Checks if the mixed address is valid. */
#define DBGFADDRESS_IS_VALID(pAddress)   ( !!((pAddress)->fFlags & DBGFADDRESS_FLAGS_VALID) )
/** Checks if the address is flagged as within the HMA. */
#define DBGFADDRESS_IS_HMA(pAddress)     ( !!((pAddress)->fFlags & DBGFADDRESS_FLAGS_HMA) )
/** @} */

VMMR3DECL(int)          DBGFR3AddrFromSelOff(PVM pVM, VMCPUID idCpu, PDBGFADDRESS pAddress, RTSEL Sel, RTUINTPTR off);
VMMR3DECL(int)          DBGFR3AddrFromSelInfoOff(PVM pVM, PDBGFADDRESS pAddress, PCDBGFSELINFO pSelInfo, RTUINTPTR off);
VMMR3DECL(PDBGFADDRESS) DBGFR3AddrFromFlat(PVM pVM, PDBGFADDRESS pAddress, RTGCUINTPTR FlatPtr);
VMMR3DECL(PDBGFADDRESS) DBGFR3AddrFromPhys(PVM pVM, PDBGFADDRESS pAddress, RTGCPHYS PhysAddr);
VMMR3DECL(bool)         DBGFR3AddrIsValid(PVM pVM, PCDBGFADDRESS pAddress);
VMMR3DECL(int)          DBGFR3AddrToPhys(PVM pVM, VMCPUID idCpu, PDBGFADDRESS pAddress, PRTGCPHYS pGCPhys);
VMMR3DECL(int)          DBGFR3AddrToHostPhys(PVM pVM, VMCPUID idCpu, PDBGFADDRESS pAddress, PRTHCPHYS pHCPhys);
VMMR3DECL(int)          DBGFR3AddrToVolatileR3Ptr(PVM pVM, VMCPUID idCpu, PDBGFADDRESS pAddress, bool fReadOnly, void **ppvR3Ptr);
VMMR3DECL(PDBGFADDRESS) DBGFR3AddrAdd(PDBGFADDRESS pAddress, RTGCUINTPTR uAddend);
VMMR3DECL(PDBGFADDRESS) DBGFR3AddrSub(PDBGFADDRESS pAddress, RTGCUINTPTR uSubtrahend);




/**
 * VMM Debug Event Type.
 */
typedef enum DBGFEVENTTYPE
{
    /** Halt completed.
     * This notifies that a halt command have been successfully completed.
     */
    DBGFEVENT_HALT_DONE = 0,
    /** Detach completed.
     * This notifies that the detach command have been successfully completed.
     */
    DBGFEVENT_DETACH_DONE,
    /** The command from the debugger is not recognized.
     * This means internal error or half implemented features.
     */
    DBGFEVENT_INVALID_COMMAND,


    /** Fatal error.
     * This notifies a fatal error in the VMM and that the debugger get's a
     * chance to first hand information about the the problem.
     */
    DBGFEVENT_FATAL_ERROR = 100,
    /** Breakpoint Hit.
     * This notifies that a breakpoint installed by the debugger was hit. The
     * identifier of the breakpoint can be found in the DBGFEVENT::u::Bp::iBp member.
     */
    DBGFEVENT_BREAKPOINT,
    /** Breakpoint Hit in the Hypervisor.
     * This notifies that a breakpoint installed by the debugger was hit. The
     * identifier of the breakpoint can be found in the DBGFEVENT::u::Bp::iBp member.
     */
    DBGFEVENT_BREAKPOINT_HYPER,
    /** Assertion in the Hypervisor (breakpoint instruction).
     * This notifies that a breakpoint instruction was hit in the hypervisor context.
     */
    DBGFEVENT_ASSERTION_HYPER,
    /** Single Stepped.
     * This notifies that a single step operation was completed.
     */
    DBGFEVENT_STEPPED,
    /** Single Stepped.
     * This notifies that a hypervisor single step operation was completed.
     */
    DBGFEVENT_STEPPED_HYPER,
    /** The developer have used the DBGFSTOP macro or the PDMDeviceDBGFSTOP function
     * to bring up the debugger at a specific place.
     */
    DBGFEVENT_DEV_STOP,
    /** The VM is terminating.
     * When this notification is received, the debugger thread should detach ASAP.
     */
    DBGFEVENT_TERMINATING,

    /** The usual 32-bit hack. */
    DBGFEVENT_32BIT_HACK = 0x7fffffff
} DBGFEVENTTYPE;


/**
 * The context of an event.
 */
typedef enum DBGFEVENTCTX
{
    /** The usual invalid entry. */
    DBGFEVENTCTX_INVALID = 0,
    /** Raw mode. */
    DBGFEVENTCTX_RAW,
    /** Recompiled mode. */
    DBGFEVENTCTX_REM,
    /** VMX / AVT mode. */
    DBGFEVENTCTX_HWACCL,
    /** Hypervisor context. */
    DBGFEVENTCTX_HYPER,
    /** Other mode */
    DBGFEVENTCTX_OTHER,

    /** The usual 32-bit hack */
    DBGFEVENTCTX_32BIT_HACK = 0x7fffffff
} DBGFEVENTCTX;

/**
 * VMM Debug Event.
 */
typedef struct DBGFEVENT
{
    /** Type. */
    DBGFEVENTTYPE   enmType;
    /** Context */
    DBGFEVENTCTX    enmCtx;
    /** Type specific data. */
    union
    {
        /** Fatal error details. */
        struct
        {
            /** The GC return code. */
            int                     rc;
        } FatalError;

        /** Source location. */
        struct
        {
            /** File name. */
            R3PTRTYPE(const char *) pszFile;
            /** Function name. */
            R3PTRTYPE(const char *) pszFunction;
            /** Message. */
            R3PTRTYPE(const char *) pszMessage;
            /** Line number. */
            unsigned                uLine;
        } Src;

        /** Assertion messages. */
        struct
        {
            /** The first message. */
            R3PTRTYPE(const char *) pszMsg1;
            /** The second message. */
            R3PTRTYPE(const char *) pszMsg2;
        } Assert;

        /** Breakpoint. */
        struct DBGFEVENTBP
        {
            /** The identifier of the breakpoint which was hit. */
            RTUINT                  iBp;
        } Bp;
        /** Padding for ensuring that the structure is 8 byte aligned. */
        uint64_t        au64Padding[4];
    } u;
} DBGFEVENT;
/** Pointer to VMM Debug Event. */
typedef DBGFEVENT *PDBGFEVENT;
/** Pointer to const VMM Debug Event. */
typedef const DBGFEVENT *PCDBGFEVENT;


/** @def DBGFSTOP
 * Stops the debugger raising a DBGFEVENT_DEVELOPER_STOP event.
 *
 * @returns VBox status code which must be propagated up to EM if not VINF_SUCCESS.
 * @param   pVM     VM Handle.
 */
#ifdef VBOX_STRICT
# define DBGFSTOP(pVM)  DBGFR3EventSrc(pVM, DBGFEVENT_DEV_STOP, __FILE__, __LINE__, __PRETTY_FUNCTION__, NULL)
#else
# define DBGFSTOP(pVM)  VINF_SUCCESS
#endif

VMMR3DECL(int)  DBGFR3Init(PVM pVM);
VMMR3DECL(int)  DBGFR3Term(PVM pVM);
VMMR3DECL(void) DBGFR3Relocate(PVM pVM, RTGCINTPTR offDelta);
VMMR3DECL(int)  DBGFR3VMMForcedAction(PVM pVM);
VMMR3DECL(int)  DBGFR3Event(PVM pVM, DBGFEVENTTYPE enmEvent);
VMMR3DECL(int)  DBGFR3EventSrc(PVM pVM, DBGFEVENTTYPE enmEvent, const char *pszFile, unsigned uLine, const char *pszFunction, const char *pszFormat, ...);
VMMR3DECL(int)  DBGFR3EventSrcV(PVM pVM, DBGFEVENTTYPE enmEvent, const char *pszFile, unsigned uLine, const char *pszFunction, const char *pszFormat, va_list args);
VMMR3DECL(int)  DBGFR3EventAssertion(PVM pVM, DBGFEVENTTYPE enmEvent, const char *pszMsg1, const char *pszMsg2);
VMMR3DECL(int)  DBGFR3EventBreakpoint(PVM pVM, DBGFEVENTTYPE enmEvent);
VMMR3DECL(int)  DBGFR3Attach(PVM pVM);
VMMR3DECL(int)  DBGFR3Detach(PVM pVM);
VMMR3DECL(int)  DBGFR3EventWait(PVM pVM, RTMSINTERVAL cMillies, PCDBGFEVENT *ppEvent);
VMMR3DECL(int)  DBGFR3Halt(PVM pVM);
VMMR3DECL(bool) DBGFR3IsHalted(PVM pVM);
VMMR3DECL(bool) DBGFR3CanWait(PVM pVM);
VMMR3DECL(int)  DBGFR3Resume(PVM pVM);
VMMR3DECL(int)  DBGFR3Step(PVM pVM, VMCPUID idCpu);
VMMR3DECL(int)  DBGFR3PrgStep(PVMCPU pVCpu);


/** Breakpoint type. */
typedef enum DBGFBPTYPE
{
    /** Free breakpoint entry. */
    DBGFBPTYPE_FREE = 0,
    /** Debug register. */
    DBGFBPTYPE_REG,
    /** INT 3 instruction. */
    DBGFBPTYPE_INT3,
    /** Recompiler. */
    DBGFBPTYPE_REM,
    /** ensure 32-bit size. */
    DBGFBPTYPE_32BIT_HACK = 0x7fffffff
} DBGFBPTYPE;


/**
 * A Breakpoint.
 */
typedef struct DBGFBP
{
    /** The number of breakpoint hits. */
    uint64_t        cHits;
    /** The hit number which starts to trigger the breakpoint. */
    uint64_t        iHitTrigger;
    /** The hit number which stops triggering the breakpoint (disables it).
     * Use ~(uint64_t)0 if it should never stop. */
    uint64_t        iHitDisable;
    /** The Flat GC address of the breakpoint.
     * (PC register value if REM type?) */
    RTGCUINTPTR     GCPtr;
    /** The breakpoint id. */
    RTUINT          iBp;
    /** The breakpoint status - enabled or disabled. */
    bool            fEnabled;

    /** The breakpoint type. */
    DBGFBPTYPE      enmType;

#if GC_ARCH_BITS == 64
    uint32_t        u32Padding;
#endif

    /** Union of type specific data. */
    union
    {
        /** Debug register data. */
        struct DBGFBPREG
        {
            /** The debug register number. */
            uint8_t     iReg;
            /** The access type (one of the X86_DR7_RW_* value). */
            uint8_t     fType;
            /** The access size. */
            uint8_t     cb;
        } Reg;
        /** Recompiler breakpoint data. */
        struct DBGFBPINT3
        {
            /** The byte value we replaced by the INT 3 instruction. */
            uint8_t     bOrg;
        } Int3;

        /** Recompiler breakpoint data. */
        struct DBGFBPREM
        {
            /** nothing yet */
            uint8_t fDummy;
        } Rem;
        /** Paddind to ensure that the size is identical on win32 and linux. */
        uint64_t    u64Padding;
    } u;
} DBGFBP;

/** Pointer to a breakpoint. */
typedef DBGFBP *PDBGFBP;
/** Pointer to a const breakpoint. */
typedef const DBGFBP *PCDBGFBP;


VMMR3DECL(int)  DBGFR3BpSet(PVM pVM, PCDBGFADDRESS pAddress, uint64_t iHitTrigger, uint64_t iHitDisable, uint32_t *piBp);
VMMR3DECL(int)  DBGFR3BpSetReg(PVM pVM, PCDBGFADDRESS pAddress, uint64_t iHitTrigger, uint64_t iHitDisable,
                               uint8_t fType, uint8_t cb, uint32_t *piBp);
VMMR3DECL(int)  DBGFR3BpSetREM(PVM pVM, PCDBGFADDRESS pAddress, uint64_t iHitTrigger, uint64_t iHitDisable, uint32_t *piBp);
VMMR3DECL(int)  DBGFR3BpClear(PVM pVM, uint32_t iBp);
VMMR3DECL(int)  DBGFR3BpEnable(PVM pVM, uint32_t iBp);
VMMR3DECL(int)  DBGFR3BpDisable(PVM pVM, uint32_t iBp);

/**
 * Breakpoint enumeration callback function.
 *
 * @returns VBox status code. Any failure will stop the enumeration.
 * @param   pVM         The VM handle.
 * @param   pvUser      The user argument.
 * @param   pBp         Pointer to the breakpoint information. (readonly)
 */
typedef DECLCALLBACK(int) FNDBGFBPENUM(PVM pVM, void *pvUser, PCDBGFBP pBp);
/** Pointer to a breakpoint enumeration callback function. */
typedef FNDBGFBPENUM *PFNDBGFBPENUM;

VMMR3DECL(int)          DBGFR3BpEnum(PVM pVM, PFNDBGFBPENUM pfnCallback, void *pvUser);
VMMDECL(RTGCUINTREG)    DBGFBpGetDR7(PVM pVM);
VMMDECL(RTGCUINTREG)    DBGFBpGetDR0(PVM pVM);
VMMDECL(RTGCUINTREG)    DBGFBpGetDR1(PVM pVM);
VMMDECL(RTGCUINTREG)    DBGFBpGetDR2(PVM pVM);
VMMDECL(RTGCUINTREG)    DBGFBpGetDR3(PVM pVM);
VMMDECL(bool)           DBGFIsStepping(PVMCPU pVCpu);



VMMR3DECL(CPUMMODE)     DBGFR3CpuGetMode(PVM pVM, VMCPUID idCpu);




/**
 * Info helper callback structure.
 */
typedef struct DBGFINFOHLP
{
    /**
     * Print formatted string.
     *
     * @param   pHlp        Pointer to this structure.
     * @param   pszFormat   The format string.
     * @param   ...         Arguments.
     */
    DECLCALLBACKMEMBER(void, pfnPrintf)(PCDBGFINFOHLP pHlp, const char *pszFormat, ...);

    /**
     * Print formatted string.
     *
     * @param   pHlp        Pointer to this structure.
     * @param   pszFormat   The format string.
     * @param   args        Argument list.
     */
    DECLCALLBACKMEMBER(void, pfnPrintfV)(PCDBGFINFOHLP pHlp, const char *pszFormat, va_list args);
} DBGFINFOHLP;


/**
 * Info handler, device version.
 *
 * @param   pDevIns     The device instance which registered the info.
 * @param   pHlp        Callback functions for doing output.
 * @param   pszArgs     Argument string. Optional and specific to the handler.
 */
typedef DECLCALLBACK(void) FNDBGFHANDLERDEV(PPDMDEVINS pDevIns, PCDBGFINFOHLP pHlp, const char *pszArgs);
/** Pointer to a FNDBGFHANDLERDEV function. */
typedef FNDBGFHANDLERDEV  *PFNDBGFHANDLERDEV;

/**
 * Info handler, USB device version.
 *
 * @param   pUsbIns     The USB device instance which registered the info.
 * @param   pHlp        Callback functions for doing output.
 * @param   pszArgs     Argument string. Optional and specific to the handler.
 */
typedef DECLCALLBACK(void) FNDBGFHANDLERUSB(PPDMUSBINS pUsbIns, PCDBGFINFOHLP pHlp, const char *pszArgs);
/** Pointer to a FNDBGFHANDLERUSB function. */
typedef FNDBGFHANDLERUSB  *PFNDBGFHANDLERUSB;

/**
 * Info handler, driver version.
 *
 * @param   pDrvIns     The driver instance which registered the info.
 * @param   pHlp        Callback functions for doing output.
 * @param   pszArgs     Argument string. Optional and specific to the handler.
 */
typedef DECLCALLBACK(void) FNDBGFHANDLERDRV(PPDMDRVINS pDrvIns, PCDBGFINFOHLP pHlp, const char *pszArgs);
/** Pointer to a FNDBGFHANDLERDRV function. */
typedef FNDBGFHANDLERDRV  *PFNDBGFHANDLERDRV;

/**
 * Info handler, internal version.
 *
 * @param   pVM         The VM handle.
 * @param   pHlp        Callback functions for doing output.
 * @param   pszArgs     Argument string. Optional and specific to the handler.
 */
typedef DECLCALLBACK(void) FNDBGFHANDLERINT(PVM pVM, PCDBGFINFOHLP pHlp, const char *pszArgs);
/** Pointer to a FNDBGFHANDLERINT function. */
typedef FNDBGFHANDLERINT  *PFNDBGFHANDLERINT;

/**
 * Info handler, external version.
 *
 * @param   pvUser      User argument.
 * @param   pHlp        Callback functions for doing output.
 * @param   pszArgs     Argument string. Optional and specific to the handler.
 */
typedef DECLCALLBACK(void) FNDBGFHANDLEREXT(void *pvUser, PCDBGFINFOHLP pHlp, const char *pszArgs);
/** Pointer to a FNDBGFHANDLEREXT function. */
typedef FNDBGFHANDLEREXT  *PFNDBGFHANDLEREXT;


/** @name Flags for the info registration functions.
 * @{ */
/** The handler must run on the EMT. */
#define DBGFINFO_FLAGS_RUN_ON_EMT       RT_BIT(0)
/** @} */

VMMR3DECL(int) DBGFR3InfoRegisterDevice(PVM pVM, const char *pszName, const char *pszDesc, PFNDBGFHANDLERDEV pfnHandler, PPDMDEVINS pDevIns);
VMMR3DECL(int) DBGFR3InfoRegisterDriver(PVM pVM, const char *pszName, const char *pszDesc, PFNDBGFHANDLERDRV pfnHandler, PPDMDRVINS pDrvIns);
VMMR3DECL(int) DBGFR3InfoRegisterInternal(PVM pVM, const char *pszName, const char *pszDesc, PFNDBGFHANDLERINT pfnHandler);
VMMR3DECL(int) DBGFR3InfoRegisterInternalEx(PVM pVM, const char *pszName, const char *pszDesc, PFNDBGFHANDLERINT pfnHandler, uint32_t fFlags);
VMMR3DECL(int) DBGFR3InfoRegisterExternal(PVM pVM, const char *pszName, const char *pszDesc, PFNDBGFHANDLEREXT pfnHandler, void *pvUser);
VMMR3DECL(int) DBGFR3InfoDeregisterDevice(PVM pVM, PPDMDEVINS pDevIns, const char *pszName);
VMMR3DECL(int) DBGFR3InfoDeregisterDriver(PVM pVM, PPDMDRVINS pDrvIns, const char *pszName);
VMMR3DECL(int) DBGFR3InfoDeregisterInternal(PVM pVM, const char *pszName);
VMMR3DECL(int) DBGFR3InfoDeregisterExternal(PVM pVM, const char *pszName);
VMMR3DECL(int) DBGFR3Info(PVM pVM, const char *pszName, const char *pszArgs, PCDBGFINFOHLP pHlp);
VMMR3DECL(int) DBGFR3InfoLogRel(PVM pVM, const char *pszName, const char *pszArgs);
VMMR3DECL(int) DBGFR3InfoStdErr(PVM pVM, const char *pszName, const char *pszArgs);
VMMR3DECL(int) DBGFR3InfoMulti(PVM pVM, const char *pszIncludePat, const char *pszExcludePat,
                               const char *pszSepFmt, PCDBGFINFOHLP pHlp);

/** @def DBGFR3InfoLog
 * Display a piece of info writing to the log if enabled.
 *
 * @param   pVM         VM handle.
 * @param   pszName     The identifier of the info to display.
 * @param   pszArgs     Arguments to the info handler.
 */
#ifdef LOG_ENABLED
#define DBGFR3InfoLog(pVM, pszName, pszArgs) \
    do { \
        if (LogIsEnabled()) \
            DBGFR3Info(pVM, pszName, pszArgs, NULL); \
    } while (0)
#else
#define DBGFR3InfoLog(pVM, pszName, pszArgs) do { } while (0)
#endif

/**
 * Enumeration callback for use with DBGFR3InfoEnum.
 *
 * @returns VBox status code.
 *          A status code indicating failure will end the enumeration
 *          and DBGFR3InfoEnum will return with that status code.
 * @param   pVM         VM handle.
 * @param   pszName     Info identifier name.
 * @param   pszDesc     The description.
 */
typedef DECLCALLBACK(int) FNDBGFINFOENUM(PVM pVM, const char *pszName, const char *pszDesc, void *pvUser);
/** Pointer to a FNDBGFINFOENUM function. */
typedef FNDBGFINFOENUM *PFNDBGFINFOENUM;

VMMR3DECL(int)              DBGFR3InfoEnum(PVM pVM, PFNDBGFINFOENUM pfnCallback, void *pvUser);
VMMR3DECL(PCDBGFINFOHLP)    DBGFR3InfoLogHlp(void);
VMMR3DECL(PCDBGFINFOHLP)    DBGFR3InfoLogRelHlp(void);



VMMR3DECL(int) DBGFR3LogModifyGroups(PVM pVM, const char *pszGroupSettings);
VMMR3DECL(int) DBGFR3LogModifyFlags(PVM pVM, const char *pszFlagSettings);
VMMR3DECL(int) DBGFR3LogModifyDestinations(PVM pVM, const char *pszDestSettings);



/** Max length (including '\\0') of a symbol name. */
#define DBGF_SYMBOL_NAME_LENGTH   512

/**
 * Debug symbol.
 */
typedef struct DBGFSYMBOL
{
    /** Symbol value (address). */
    RTGCUINTPTR         Value;
    /** Symbol size. */
    uint32_t            cb;
    /** Symbol Flags. (reserved). */
    uint32_t            fFlags;
    /** Symbol name. */
    char                szName[DBGF_SYMBOL_NAME_LENGTH];
} DBGFSYMBOL;
/** Pointer to debug symbol. */
typedef DBGFSYMBOL *PDBGFSYMBOL;
/** Pointer to const debug symbol. */
typedef const DBGFSYMBOL *PCDBGFSYMBOL;

/**
 * Debug line number information.
 */
typedef struct DBGFLINE
{
    /** Address. */
    RTGCUINTPTR         Address;
    /** Line number. */
    uint32_t            uLineNo;
    /** Filename. */
    char                szFilename[260];
} DBGFLINE;
/** Pointer to debug line number. */
typedef DBGFLINE *PDBGFLINE;
/** Pointer to const debug line number. */
typedef const DBGFLINE *PCDBGFLINE;

/** @name Address spaces aliases.
 * @{ */
/** The guest global address space. */
#define DBGF_AS_GLOBAL              ((RTDBGAS)-1)
/** The guest kernel address space.
 * This is usually resolves to the same as DBGF_AS_GLOBAL. */
#define DBGF_AS_KERNEL              ((RTDBGAS)-2)
/** The physical address space. */
#define DBGF_AS_PHYS                ((RTDBGAS)-3)
/** Raw-mode context. */
#define DBGF_AS_RC                  ((RTDBGAS)-4)
/** Ring-0 context. */
#define DBGF_AS_R0                  ((RTDBGAS)-5)
/** Raw-mode context and then global guest context.
 * When used for looking up information, it works as if the call was first made
 * with DBGF_AS_RC and then on failure with DBGF_AS_GLOBAL. When called for
 * making address space changes, it works as if DBGF_AS_RC was used. */
#define DBGF_AS_RC_AND_GC_GLOBAL    ((RTDBGAS)-6)

/** The first special one. */
#define DBGF_AS_FIRST               DBGF_AS_RC_AND_GC_GLOBAL
/** The last special one. */
#define DBGF_AS_LAST                DBGF_AS_GLOBAL
/** The number of special address space handles. */
#define DBGF_AS_COUNT               (6U)
/** Converts an alias handle to an array index. */
#define DBGF_AS_ALIAS_2_INDEX(hAlias) \
    ( (uintptr_t)(hAlias) - (uintptr_t)DBGF_AS_FIRST )
/** Predicat macro that check if the specified handle is an alias. */
#define DBGF_AS_IS_ALIAS(hAlias) \
    ( DBGF_AS_ALIAS_2_INDEX(hAlias)  <  DBGF_AS_COUNT )
/** Predicat macro that check if the specified alias is a fixed one or not. */
#define DBGF_AS_IS_FIXED_ALIAS(hAlias) \
    ( DBGF_AS_ALIAS_2_INDEX(hAlias)  <  (uintptr_t)DBGF_AS_PHYS - (uintptr_t)DBGF_AS_FIRST + 1U )

/** @} */

VMMR3DECL(int)          DBGFR3AsAdd(PVM pVM, RTDBGAS hDbgAs, RTPROCESS ProcId);
VMMR3DECL(int)          DBGFR3AsDelete(PVM pVM, RTDBGAS hDbgAs);
VMMR3DECL(int)          DBGFR3AsSetAlias(PVM pVM, RTDBGAS hAlias, RTDBGAS hAliasFor);
VMMR3DECL(RTDBGAS)      DBGFR3AsResolve(PVM pVM, RTDBGAS hAlias);
VMMR3DECL(RTDBGAS)      DBGFR3AsResolveAndRetain(PVM pVM, RTDBGAS hAlias);
VMMR3DECL(RTDBGAS)      DBGFR3AsQueryByName(PVM pVM, const char *pszName);
VMMR3DECL(RTDBGAS)      DBGFR3AsQueryByPid(PVM pVM, RTPROCESS ProcId);

VMMR3DECL(int)          DBGFR3AsLoadImage(PVM pVM, RTDBGAS hDbgAs, const char *pszFilename, const char *pszModName, PCDBGFADDRESS pModAddress, RTDBGSEGIDX iModSeg, uint32_t fFlags);
VMMR3DECL(int)          DBGFR3AsLoadMap(PVM pVM, RTDBGAS hDbgAs, const char *pszFilename, const char *pszModName, PCDBGFADDRESS pModAddress, RTDBGSEGIDX iModSeg, RTGCUINTPTR uSubtrahend, uint32_t fFlags);
VMMR3DECL(int)          DBGFR3AsLinkModule(PVM pVM, RTDBGAS hDbgAs, RTDBGMOD hMod, PCDBGFADDRESS pModAddress, RTDBGSEGIDX iModSeg, uint32_t fFlags);

VMMR3DECL(int)          DBGFR3AsSymbolByAddr(PVM pVM, RTDBGAS hDbgAs, PCDBGFADDRESS pAddress, PRTGCINTPTR poffDisp, PRTDBGSYMBOL pSymbol, PRTDBGMOD phMod);
VMMR3DECL(PRTDBGSYMBOL) DBGFR3AsSymbolByAddrA(PVM pVM, RTDBGAS hDbgAs, PCDBGFADDRESS pAddress, PRTGCINTPTR poffDisp, PRTDBGMOD phMod);
VMMR3DECL(int)          DBGFR3AsSymbolByName(PVM pVM, RTDBGAS hDbgAs, const char *pszSymbol, PRTDBGSYMBOL pSymbol, PRTDBGMOD phMod);

/* The following are soon to be obsoleted: */
VMMR3DECL(int)          DBGFR3ModuleLoad(PVM pVM, const char *pszFilename, RTGCUINTPTR AddressDelta, const char *pszName, RTGCUINTPTR ModuleAddress, unsigned cbImage);
VMMR3DECL(void)         DBGFR3ModuleRelocate(PVM pVM, RTGCUINTPTR OldImageBase, RTGCUINTPTR NewImageBase, RTGCUINTPTR cbImage,
                                             const char *pszFilename, const char *pszName);
VMMR3DECL(int)          DBGFR3SymbolAdd(PVM pVM, RTGCUINTPTR ModuleAddress, RTGCUINTPTR SymbolAddress, RTUINT cbSymbol, const char *pszSymbol);
VMMR3DECL(int)          DBGFR3SymbolByAddr(PVM pVM, RTGCUINTPTR Address, PRTGCINTPTR poffDisplacement, PDBGFSYMBOL pSymbol);
VMMR3DECL(int)          DBGFR3SymbolByName(PVM pVM, const char *pszSymbol, PDBGFSYMBOL pSymbol);

VMMR3DECL(int)          DBGFR3LineByAddr(PVM pVM, RTGCUINTPTR Address, PRTGCINTPTR poffDisplacement, PDBGFLINE pLine);
VMMR3DECL(PDBGFLINE)    DBGFR3LineByAddrAlloc(PVM pVM, RTGCUINTPTR Address, PRTGCINTPTR poffDisplacement);
VMMR3DECL(void)         DBGFR3LineFree(PDBGFLINE pLine);


/**
 * Return type.
 */
typedef enum DBGFRETRUNTYPE
{
    /** The usual invalid 0 value. */
    DBGFRETURNTYPE_INVALID = 0,
    /** Near 16-bit return. */
    DBGFRETURNTYPE_NEAR16,
    /** Near 32-bit return. */
    DBGFRETURNTYPE_NEAR32,
    /** Near 64-bit return. */
    DBGFRETURNTYPE_NEAR64,
    /** Far 16:16 return. */
    DBGFRETURNTYPE_FAR16,
    /** Far 16:32 return. */
    DBGFRETURNTYPE_FAR32,
    /** Far 16:64 return. */
    DBGFRETURNTYPE_FAR64,
    /** 16-bit iret return (e.g. real or 286 protect mode). */
    DBGFRETURNTYPE_IRET16,
    /** 32-bit iret return. */
    DBGFRETURNTYPE_IRET32,
    /** 32-bit iret return. */
    DBGFRETURNTYPE_IRET32_PRIV,
    /** 32-bit iret return to V86 mode. */
    DBGFRETURNTYPE_IRET32_V86,
    /** @todo 64-bit iret return. */
    DBGFRETURNTYPE_IRET64,
    /** The end of the valid return types. */
    DBGFRETURNTYPE_END,
    /** The usual 32-bit blowup. */
    DBGFRETURNTYPE_32BIT_HACK = 0x7fffffff
} DBGFRETURNTYPE;

/**
 * Figures the size of the return state on the stack.
 *
 * @returns number of bytes. 0 if invalid parameter.
 * @param   enmRetType  The type of return.
 */
DECLINLINE(unsigned) DBGFReturnTypeSize(DBGFRETURNTYPE enmRetType)
{
    switch (enmRetType)
    {
        case DBGFRETURNTYPE_NEAR16:         return 2;
        case DBGFRETURNTYPE_NEAR32:         return 4;
        case DBGFRETURNTYPE_NEAR64:         return 8;
        case DBGFRETURNTYPE_FAR16:          return 4;
        case DBGFRETURNTYPE_FAR32:          return 4;
        case DBGFRETURNTYPE_FAR64:          return 8;
        case DBGFRETURNTYPE_IRET16:         return 6;
        case DBGFRETURNTYPE_IRET32:         return 4*3;
        case DBGFRETURNTYPE_IRET32_PRIV:    return 4*5;
        case DBGFRETURNTYPE_IRET32_V86:     return 4*9;
        case DBGFRETURNTYPE_IRET64:
        default:
            return 0;
    }
}


/** Pointer to stack frame info. */
typedef struct DBGFSTACKFRAME *PDBGFSTACKFRAME;
/** Pointer to const stack frame info. */
typedef struct DBGFSTACKFRAME const  *PCDBGFSTACKFRAME;
/**
 * Info about a stack frame.
 */
typedef struct DBGFSTACKFRAME
{
    /** Frame number. */
    uint32_t        iFrame;
    /** Frame flags. */
    uint32_t        fFlags;
    /** The frame address.
     * The off member is [e|r]bp and the Sel member is ss. */
    DBGFADDRESS     AddrFrame;
    /** The stack address of the frame.
     * The off member is [e|r]sp and the Sel member is ss. */
    DBGFADDRESS     AddrStack;
    /** The program counter (PC) address of the frame.
     * The off member is [e|r]ip and the Sel member is cs. */
    DBGFADDRESS     AddrPC;
    /** Pointer to the symbol nearest the program counter (PC). NULL if not found. */
    PRTDBGSYMBOL    pSymPC;
    /** Pointer to the linnumber nearest the program counter (PC). NULL if not found. */
    PDBGFLINE       pLinePC;

    /** The return frame address.
     * The off member is [e|r]bp and the Sel member is ss. */
    DBGFADDRESS     AddrReturnFrame;
    /** The return stack address.
     * The off member is [e|r]sp and the Sel member is ss. */
    DBGFADDRESS     AddrReturnStack;
    /** The way this frame returns to the next one. */
    DBGFRETURNTYPE  enmReturnType;

    /** The program counter (PC) address which the frame returns to.
     * The off member is [e|r]ip and the Sel member is cs. */
    DBGFADDRESS     AddrReturnPC;
    /** Pointer to the symbol nearest the return PC. NULL if not found. */
    PRTDBGSYMBOL    pSymReturnPC;
    /** Pointer to the linnumber nearest the return PC. NULL if not found. */
    PDBGFLINE       pLineReturnPC;

    /** 32-bytes of stack arguments. */
    union
    {
        /** 64-bit view */
        uint64_t    au64[4];
        /** 32-bit view */
        uint32_t    au32[8];
        /** 16-bit view */
        uint16_t    au16[16];
        /** 8-bit view */
        uint8_t     au8[32];
    } Args;

    /** Pointer to the next frame.
     * Might not be used in some cases, so consider it internal. */
    PCDBGFSTACKFRAME pNextInternal;
    /** Pointer to the first frame.
     * Might not be used in some cases, so consider it internal. */
    PCDBGFSTACKFRAME pFirstInternal;
} DBGFSTACKFRAME;

/** @name DBGFSTACKFRAME Flags.
 * @{ */
/** Set if the content of the frame is filled in by DBGFR3StackWalk() and can be used
 * to construct the next frame. */
#define DBGFSTACKFRAME_FLAGS_ALL_VALID  RT_BIT(0)
/** This is the last stack frame we can read.
 * This flag is not set if the walk stop because of max dept or recursion. */
#define DBGFSTACKFRAME_FLAGS_LAST       RT_BIT(1)
/** This is the last record because we detected a loop. */
#define DBGFSTACKFRAME_FLAGS_LOOP       RT_BIT(2)
/** This is the last record because we reached the maximum depth. */
#define DBGFSTACKFRAME_FLAGS_MAX_DEPTH  RT_BIT(3)
/** 16-bit frame. */
#define DBGFSTACKFRAME_FLAGS_16BIT      RT_BIT(4)
/** 32-bit frame. */
#define DBGFSTACKFRAME_FLAGS_32BIT      RT_BIT(5)
/** 64-bit frame. */
#define DBGFSTACKFRAME_FLAGS_64BIT      RT_BIT(6)
/** @} */

/** @name DBGFCODETYPE
 * @{ */
typedef enum DBGFCODETYPE
{
    /** The usual invalid 0 value. */
    DBGFCODETYPE_INVALID = 0,
    /** Stack walk for guest code. */
    DBGFCODETYPE_GUEST,
    /** Stack walk for hypervisor code. */
    DBGFCODETYPE_HYPER,
    /** Stack walk for ring 0 code. */
    DBGFCODETYPE_RING0,
    /** The usual 32-bit blowup. */
    DBGFCODETYPE_32BIT_HACK = 0x7fffffff
} DBGFCODETYPE;
/** @} */

VMMR3DECL(int)              DBGFR3StackWalkBegin(PVM pVM, VMCPUID idCpu, DBGFCODETYPE enmCodeType, PCDBGFSTACKFRAME *ppFirstFrame);
VMMR3DECL(int)              DBGFR3StackWalkBeginEx(PVM pVM, VMCPUID idCpu, DBGFCODETYPE enmCodeType, PCDBGFADDRESS pAddrFrame,
                                                   PCDBGFADDRESS pAddrStack,PCDBGFADDRESS pAddrPC,
                                                   DBGFRETURNTYPE enmReturnType, PCDBGFSTACKFRAME *ppFirstFrame);
VMMR3DECL(PCDBGFSTACKFRAME) DBGFR3StackWalkNext(PCDBGFSTACKFRAME pCurrent);
VMMR3DECL(void)             DBGFR3StackWalkEnd(PCDBGFSTACKFRAME pFirstFrame);




/** Flags to pass to DBGFR3DisasInstrEx().
 * @{ */
/** Disassemble the current guest instruction, with annotations. */
#define DBGF_DISAS_FLAGS_CURRENT_GUEST      RT_BIT(0)
/** Disassemble the current hypervisor instruction, with annotations. */
#define DBGF_DISAS_FLAGS_CURRENT_HYPER      RT_BIT(1)
/** No annotations for current context. */
#define DBGF_DISAS_FLAGS_NO_ANNOTATION      RT_BIT(2)
/** No symbol lookup. */
#define DBGF_DISAS_FLAGS_NO_SYMBOLS         RT_BIT(3)
/** No instruction bytes. */
#define DBGF_DISAS_FLAGS_NO_BYTES           RT_BIT(4)
/** No address in the output. */
#define DBGF_DISAS_FLAGS_NO_ADDRESS         RT_BIT(5)
/** Set if the hidden selector registers are known to be valid. (REM hack to
 *  avoid assertions.) */
#define DBGF_DISAS_FLAGS_HID_SEL_REGS_VALID RT_BIT(6)
/** Disassemble in the default mode of the specific context. */
#define DBGF_DISAS_FLAGS_DEFAULT_MODE       UINT32_C(0x00000000)
/** Disassemble in 16-bit mode. */
#define DBGF_DISAS_FLAGS_16BIT_MODE         UINT32_C(0x10000000)
/** Disassemble in 16-bit mode with real mode address translation. */
#define DBGF_DISAS_FLAGS_16BIT_REAL_MODE    UINT32_C(0x20000000)
/** Disassemble in 32-bit mode. */
#define DBGF_DISAS_FLAGS_32BIT_MODE         UINT32_C(0x30000000)
/** Disassemble in 64-bit mode. */
#define DBGF_DISAS_FLAGS_64BIT_MODE         UINT32_C(0x40000000)
/** The disassembly mode mask. */
#define DBGF_DISAS_FLAGS_MODE_MASK          UINT32_C(0x70000000)
/** Mask containing the valid flags. */
#define DBGF_DISAS_FLAGS_VALID_MASK         UINT32_C(0x7000007f)
/** @} */

/** Special flat selector. */
#define DBGF_SEL_FLAT                       1

VMMR3DECL(int) DBGFR3DisasInstrEx(PVM pVM, VMCPUID idCpu, RTSEL Sel, RTGCPTR GCPtr, uint32_t fFlags,
                                  char *pszOutput, uint32_t cbOutput, uint32_t *pcbInstr);
VMMR3DECL(int) DBGFR3DisasInstrCurrent(PVMCPU pVCpu, char *pszOutput, uint32_t cbOutput);
VMMR3DECL(int) DBGFR3DisasInstrCurrentLogInternal(PVMCPU pVCpu, const char *pszPrefix);

/** @def DBGFR3DisasInstrCurrentLog
 * Disassembles the current guest context instruction and writes it to the log.
 * All registers and data will be displayed. Addresses will be attempted resolved to symbols.
 */
#ifdef LOG_ENABLED
# define DBGFR3DisasInstrCurrentLog(pVCpu, pszPrefix) \
    do { \
        if (LogIsEnabled()) \
            DBGFR3DisasInstrCurrentLogInternal(pVCpu, pszPrefix); \
    } while (0)
#else
# define DBGFR3DisasInstrCurrentLog(pVCpu, pszPrefix) do { } while (0)
#endif

VMMR3DECL(int) DBGFR3DisasInstrLogInternal(PVMCPU pVCpu, RTSEL Sel, RTGCPTR GCPtr);

/** @def DBGFR3DisasInstrLog
 * Disassembles the specified guest context instruction and writes it to the log.
 * Addresses will be attempted resolved to symbols.
 * @thread Any EMT.
 */
#ifdef LOG_ENABLED
# define DBGFR3DisasInstrLog(pVCpu, Sel, GCPtr) \
    do { \
        if (LogIsEnabled()) \
            DBGFR3DisasInstrLogInternal(pVCpu, Sel, GCPtr); \
    } while (0)
#else
# define DBGFR3DisasInstrLog(pVCpu, Sel, GCPtr) do { } while (0)
#endif


VMMR3DECL(int) DBGFR3MemScan(PVM pVM, VMCPUID idCpu, PCDBGFADDRESS pAddress, RTGCUINTPTR cbRange, RTGCUINTPTR uAlign,
                             const void *pvNeedle, size_t cbNeedle, PDBGFADDRESS pHitAddress);
VMMR3DECL(int) DBGFR3MemRead(PVM pVM, VMCPUID idCpu, PCDBGFADDRESS pAddress, void *pvBuf, size_t cbRead);
VMMR3DECL(int) DBGFR3MemReadString(PVM pVM, VMCPUID idCpu, PCDBGFADDRESS pAddress, char *pszBuf, size_t cbBuf);
VMMR3DECL(int) DBGFR3MemWrite(PVM pVM, VMCPUID idCpu, PCDBGFADDRESS pAddress, void const *pvBuf, size_t cbRead);


/** @name Flags for DBGFR3PagingDumpEx, PGMR3DumpHierarchyHCEx and
 * PGMR3DumpHierarchyGCEx
 * @{ */
/** The CR3 from the current CPU state. */
#define DBGFPGDMP_FLAGS_CURRENT_CR3     RT_BIT_32(0)
/** The current CPU paging mode (PSE, PAE, LM, EPT, NX). */
#define DBGFPGDMP_FLAGS_CURRENT_MODE    RT_BIT_32(1)
/** Whether PSE is enabled (!DBGFPGDMP_FLAGS_CURRENT_STATE).
 * Same value as X86_CR4_PSE. */
#define DBGFPGDMP_FLAGS_PSE             RT_BIT_32(4) /*  */
/** Whether PAE is enabled (!DBGFPGDMP_FLAGS_CURRENT_STATE).
 * Same value as X86_CR4_PAE. */
#define DBGFPGDMP_FLAGS_PAE             RT_BIT_32(5) /*  */
/** Whether LME is enabled (!DBGFPGDMP_FLAGS_CURRENT_STATE).
 * Same value as MSR_K6_EFER_LME. */
#define DBGFPGDMP_FLAGS_LME             RT_BIT_32(8)
/** Whether nested paging is enabled (!DBGFPGDMP_FLAGS_CURRENT_STATE). */
#define DBGFPGDMP_FLAGS_NP              RT_BIT_32(9)
/** Whether extended nested page tables are enabled
 * (!DBGFPGDMP_FLAGS_CURRENT_STATE). */
#define DBGFPGDMP_FLAGS_EPT             RT_BIT_32(10)
/** Whether no-execution is enabled (!DBGFPGDMP_FLAGS_CURRENT_STATE).
 * Same value as MSR_K6_EFER_NXE. */
#define DBGFPGDMP_FLAGS_NXE             RT_BIT_32(11)
/** Whether to print the CR3. */
#define DBGFPGDMP_FLAGS_PRINT_CR3       RT_BIT_32(27)
/** Whether to print the header. */
#define DBGFPGDMP_FLAGS_HEADER          RT_BIT_32(28)
/** Whether to dump additional page information. */
#define DBGFPGDMP_FLAGS_PAGE_INFO       RT_BIT_32(29)
/** Dump the shadow tables if set.
 * Cannot be used together with DBGFPGDMP_FLAGS_GUEST. */
#define DBGFPGDMP_FLAGS_SHADOW          RT_BIT_32(30)
/** Dump the guest tables if set.
 * Cannot be used together with DBGFPGDMP_FLAGS_SHADOW. */
#define DBGFPGDMP_FLAGS_GUEST           RT_BIT_32(31)
/** Mask of valid bits. */
#define DBGFPGDMP_FLAGS_VALID_MASK      UINT32_C(0xf8000f33)
/** The mask of bits controlling the paging mode. */
#define DBGFPGDMP_FLAGS_MODE_MASK       UINT32_C(0x00000f32)
/** @}  */
VMMDECL(int) DBGFR3PagingDumpEx(PVM pVM, VMCPUID idCpu, uint32_t fFlags, uint64_t cr3, uint64_t u64FirstAddr,
                                uint64_t u64LastAddr, uint32_t cMaxDepth, PCDBGFINFOHLP pHlp);


/** @name DBGFR3SelQueryInfo flags.
 * @{ */
/** Get the info from the guest descriptor table. */
#define DBGFSELQI_FLAGS_DT_GUEST            UINT32_C(0)
/** Get the info from the shadow descriptor table.
 * Only works in raw-mode.  */
#define DBGFSELQI_FLAGS_DT_SHADOW           UINT32_C(1)
/** If currently executing in in 64-bit mode, blow up data selectors. */
#define DBGFSELQI_FLAGS_DT_ADJ_64BIT_MODE   UINT32_C(2)
/** @} */
VMMR3DECL(int) DBGFR3SelQueryInfo(PVM pVM, VMCPUID idCpu, RTSEL Sel, uint32_t fFlags, PDBGFSELINFO pSelInfo);


/**
 * Register identifiers.
 */
typedef enum DBGFREG
{
    /* General purpose registers: */
    DBGFREG_AL  = 0,
    DBGFREG_AX  = DBGFREG_AL,
    DBGFREG_EAX = DBGFREG_AL,
    DBGFREG_RAX = DBGFREG_AL,

    DBGFREG_CL,
    DBGFREG_CX  = DBGFREG_CL,
    DBGFREG_ECX = DBGFREG_CL,
    DBGFREG_RCX = DBGFREG_CL,

    DBGFREG_DL,
    DBGFREG_DX  = DBGFREG_DL,
    DBGFREG_EDX = DBGFREG_DL,
    DBGFREG_RDX = DBGFREG_DL,

    DBGFREG_BL,
    DBGFREG_BX  = DBGFREG_BL,
    DBGFREG_EBX = DBGFREG_BL,
    DBGFREG_RBX = DBGFREG_BL,

    DBGFREG_SPL,
    DBGFREG_SP  = DBGFREG_SPL,
    DBGFREG_ESP = DBGFREG_SPL,
    DBGFREG_RSP = DBGFREG_SPL,

    DBGFREG_BPL,
    DBGFREG_BP  = DBGFREG_BPL,
    DBGFREG_EBP = DBGFREG_BPL,
    DBGFREG_RBP = DBGFREG_BPL,

    DBGFREG_SIL,
    DBGFREG_SI  = DBGFREG_SIL,
    DBGFREG_ESI = DBGFREG_SIL,
    DBGFREG_RSI = DBGFREG_SIL,

    DBGFREG_DIL,
    DBGFREG_DI  = DBGFREG_DIL,
    DBGFREG_EDI = DBGFREG_DIL,
    DBGFREG_RDI = DBGFREG_DIL,

    DBGFREG_R8,
    DBGFREG_R8B = DBGFREG_R8,
    DBGFREG_R8W = DBGFREG_R8,
    DBGFREG_R8D = DBGFREG_R8,

    DBGFREG_R9,
    DBGFREG_R9B = DBGFREG_R9,
    DBGFREG_R9W = DBGFREG_R9,
    DBGFREG_R9D = DBGFREG_R9,

    DBGFREG_R10,
    DBGFREG_R10B = DBGFREG_R10,
    DBGFREG_R10W = DBGFREG_R10,
    DBGFREG_R10D = DBGFREG_R10,

    DBGFREG_R11,
    DBGFREG_R11B = DBGFREG_R11,
    DBGFREG_R11W = DBGFREG_R11,
    DBGFREG_R11D = DBGFREG_R11,

    DBGFREG_R12,
    DBGFREG_R12B = DBGFREG_R12,
    DBGFREG_R12W = DBGFREG_R12,
    DBGFREG_R12D = DBGFREG_R12,

    DBGFREG_R13,
    DBGFREG_R13B = DBGFREG_R13,
    DBGFREG_R13W = DBGFREG_R13,
    DBGFREG_R13D = DBGFREG_R13,

    DBGFREG_R14,
    DBGFREG_R14B = DBGFREG_R14,
    DBGFREG_R14W = DBGFREG_R14,
    DBGFREG_R14D = DBGFREG_R14,

    DBGFREG_R15,
    DBGFREG_R15B = DBGFREG_R15,
    DBGFREG_R15W = DBGFREG_R15,
    DBGFREG_R15D = DBGFREG_R15,

    /* Segments and other special registers: */
    DBGFREG_CS,
    DBGFREG_CS_ATTR,
    DBGFREG_CS_BASE,
    DBGFREG_CS_LIMIT,

    DBGFREG_DS,
    DBGFREG_DS_ATTR,
    DBGFREG_DS_BASE,
    DBGFREG_DS_LIMIT,

    DBGFREG_ES,
    DBGFREG_ES_ATTR,
    DBGFREG_ES_BASE,
    DBGFREG_ES_LIMIT,

    DBGFREG_FS,
    DBGFREG_FS_ATTR,
    DBGFREG_FS_BASE,
    DBGFREG_FS_LIMIT,

    DBGFREG_GS,
    DBGFREG_GS_ATTR,
    DBGFREG_GS_BASE,
    DBGFREG_GS_LIMIT,

    DBGFREG_SS,
    DBGFREG_SS_ATTR,
    DBGFREG_SS_BASE,
    DBGFREG_SS_LIMIT,

    DBGFREG_IP,
    DBGFREG_EIP = DBGFREG_IP,
    DBGFREG_RIP = DBGFREG_IP,

    DBGFREG_FLAGS,
    DBGFREG_EFLAGS = DBGFREG_FLAGS,
    DBGFREG_RFLAGS = DBGFREG_FLAGS,

    /* FPU: */
    DBGFREG_FCW,
    DBGFREG_FSW,
    DBGFREG_FTW,
    DBGFREG_FOP,
    DBGFREG_FPUIP,
    DBGFREG_FPUCS,
    DBGFREG_FPUDP,
    DBGFREG_FPUDS,
    DBGFREG_MXCSR,
    DBGFREG_MXCSR_MASK,

    DBGFREG_ST0,
    DBGFREG_ST1,
    DBGFREG_ST2,
    DBGFREG_ST3,
    DBGFREG_ST4,
    DBGFREG_ST5,
    DBGFREG_ST6,
    DBGFREG_ST7,

    DBGFREG_MM0,
    DBGFREG_MM1,
    DBGFREG_MM2,
    DBGFREG_MM3,
    DBGFREG_MM4,
    DBGFREG_MM5,
    DBGFREG_MM6,
    DBGFREG_MM7,

    /* SSE: */
    DBGFREG_XMM0,
    DBGFREG_XMM1,
    DBGFREG_XMM2,
    DBGFREG_XMM3,
    DBGFREG_XMM4,
    DBGFREG_XMM5,
    DBGFREG_XMM6,
    DBGFREG_XMM7,
    DBGFREG_XMM8,
    DBGFREG_XMM9,
    DBGFREG_XMM10,
    DBGFREG_XMM11,
    DBGFREG_XMM12,
    DBGFREG_XMM13,
    DBGFREG_XMM14,
    DBGFREG_XMM15,
    /** @todo add XMM aliases. */

    /* System registers: */
    DBGFREG_GDTR_BASE,
    DBGFREG_GDTR_LIMIT,
    DBGFREG_IDTR_BASE,
    DBGFREG_IDTR_LIMIT,
    DBGFREG_LDTR,
    DBGFREG_LDTR_ATTR,
    DBGFREG_LDTR_BASE,
    DBGFREG_LDTR_LIMIT,
    DBGFREG_TR,
    DBGFREG_TR_ATTR,
    DBGFREG_TR_BASE,
    DBGFREG_TR_LIMIT,

    DBGFREG_CR0,
    DBGFREG_CR2,
    DBGFREG_CR3,
    DBGFREG_CR4,
    DBGFREG_CR8,

    DBGFREG_DR0,
    DBGFREG_DR1,
    DBGFREG_DR2,
    DBGFREG_DR3,
    DBGFREG_DR6,
    DBGFREG_DR7,

    /* MSRs: */
    DBGFREG_MSR_IA32_APICBASE,
    DBGFREG_MSR_IA32_CR_PAT,
    DBGFREG_MSR_IA32_PERF_STATUS,
    DBGFREG_MSR_IA32_SYSENTER_CS,
    DBGFREG_MSR_IA32_SYSENTER_EIP,
    DBGFREG_MSR_IA32_SYSENTER_ESP,
    DBGFREG_MSR_IA32_TSC,
    DBGFREG_MSR_K6_EFER,
    DBGFREG_MSR_K6_STAR,
    DBGFREG_MSR_K8_CSTAR,
    DBGFREG_MSR_K8_FS_BASE,
    DBGFREG_MSR_K8_GS_BASE,
    DBGFREG_MSR_K8_KERNEL_GS_BASE,
    DBGFREG_MSR_K8_LSTAR,
    DBGFREG_MSR_K8_SF_MASK,
    DBGFREG_MSR_K8_TSC_AUX,

    /** The number of registers to pass to DBGFR3RegQueryAll. */
    DBGFREG_ALL_COUNT,

    /* Misc aliases that doesn't need be part of the 'all' query: */
    DBGFREG_AH = DBGFREG_ALL_COUNT,
    DBGFREG_CH,
    DBGFREG_DH,
    DBGFREG_BH,
    DBGFREG_GDTR,
    DBGFREG_IDTR,

    /** The end of the registers.  */
    DBGFREG_END,
    /** The usual 32-bit type hack. */
    DBGFREG_32BIT_HACK = 0x7fffffff
} DBGFREG;
/** Pointer to a register identifier. */
typedef DBGFREG *PDBGFREG;
/** Pointer to a const register identifier. */
typedef DBGFREG const *PCDBGFREG;

/**
 * Register value type.
 */
typedef enum DBGFREGVALTYPE
{
    DBGFREGVALTYPE_INVALID = 0,
    /** Unsigned 8-bit register value. */
    DBGFREGVALTYPE_U8,
    /** Unsigned 16-bit register value. */
    DBGFREGVALTYPE_U16,
    /** Unsigned 32-bit register value. */
    DBGFREGVALTYPE_U32,
    /** Unsigned 64-bit register value. */
    DBGFREGVALTYPE_U64,
    /** Unsigned 128-bit register value. */
    DBGFREGVALTYPE_U128,
    /** Long double register value. */
    DBGFREGVALTYPE_R80,
    /** Descriptor table register value. */
    DBGFREGVALTYPE_DTR,
    /** End of the valid register value types. */
    DBGFREGVALTYPE_END,
    /** The usual 32-bit type hack. */
    DBGFREGVALTYPE_32BIT_HACK = 0x7fffffff
} DBGFREGVALTYPE;
/** Pointer to a register value type. */
typedef DBGFREGVALTYPE *PDBGFREGVALTYPE;

/**
 * A generic register value type.
 */
typedef union DBGFREGVAL
{
    uint8_t     u8;             /**< The 8-bit view. */
    uint16_t    u16;            /**< The 16-bit view. */
    uint32_t    u32;            /**< The 32-bit view. */
    uint64_t    u64;            /**< The 64-bit view. */
    RTUINT128U  u128;           /**< The 128-bit view. */
    RTFLOAT80U2 r80;            /**< The 80-bit floating point view. */
    /** GDTR or LDTR (DBGFREGVALTYPE_DTR). */
    struct
    {
        /** The table address. */
        uint64_t u64Base;
        /** The table limit (length minus 1). */
        uint32_t u32Limit;
    }           dtr;

    uint8_t     au8[16];        /**< The 8-bit array view.  */
    uint16_t    au16[8];        /**< The 16-bit array view.  */
    uint32_t    au32[4];        /**< The 32-bit array view.  */
    uint64_t    au64[2];        /**< The 64-bit array view.  */
    RTUINT128U  u;
} DBGFREGVAL;
/** Pointer to a generic register value type. */
typedef DBGFREGVAL *PDBGFREGVAL;
/** Pointer to a const generic register value type. */
typedef DBGFREGVAL const *PCDBGFREGVAL;

VMMDECL(ssize_t) DBGFR3RegFormatValue(char *pszBuf, size_t cbBuf, PCDBGFREGVAL pValue, DBGFREGVALTYPE enmType, bool fSpecial);
VMMDECL(ssize_t) DBGFR3RegFormatValueEx(char *pszBuf, size_t cbBuf, PCDBGFREGVAL pValue, DBGFREGVALTYPE enmType,
                                        unsigned uBase, signed int cchWidth, signed int cchPrecision, uint32_t fFlags);

/**
 * Register sub-field descriptor.
 */
typedef struct DBGFREGSUBFIELD
{
    /** The name of the sub-field.  NULL is used to terminate the array. */
    const char     *pszName;
    /** The index of the first bit.  Ignored if pfnGet is set. */
    uint8_t         iFirstBit;
    /** The number of bits.  Mandatory. */
    uint8_t         cBits;
    /** The shift count.  Not applied when pfnGet is set, but used to
     * calculate the minimum type. */
    int8_t          cShift;
    /** Sub-field flags, DBGFREGSUBFIELD_FLAGS_XXX.  */
    uint8_t         fFlags;
    /** Getter (optional). */
    DECLCALLBACKMEMBER(int, pfnGet)(void *pvUser, struct DBGFREGSUBFIELD const *pSubField, PRTUINT128U puValue);
    /** Setter (optional). */
    DECLCALLBACKMEMBER(int, pfnSet)(void *pvUser, struct DBGFREGSUBFIELD const *pSubField, RTUINT128U uValue, RTUINT128U fMask);
} DBGFREGSUBFIELD;
/** Pointer to a const register sub-field descriptor. */
typedef DBGFREGSUBFIELD const *PCDBGFREGSUBFIELD;

/** @name DBGFREGSUBFIELD_FLAGS_XXX
 * @{ */
/** The sub-field is read-only. */
#define DBGFREGSUBFIELD_FLAGS_READ_ONLY     UINT8_C(0x01)
/** @} */

/** Macro for creating a read-write sub-field entry without getters. */
#define DBGFREGSUBFIELD_RW(a_szName, a_iFirstBit, a_cBits, a_cShift) \
    { a_szName, a_iFirstBit, a_cBits, a_cShift, 0 /*fFlags*/, NULL /*pfnGet*/, NULL /*pfnSet*/ }
/** Macro for creating a read-write sub-field entry with getters. */
#define DBGFREGSUBFIELD_RW_SG(a_szName, a_cBits, a_cShift, a_pfnGet, a_pfnSet) \
    { a_szName, 0 /*iFirstBit*/, a_cBits, a_cShift, 0 /*fFlags*/, a_pfnGet, a_pfnSet }
/** Macro for creating a terminator sub-field entry.  */
#define DBGFREGSUBFIELD_TERMINATOR() \
    { NULL, 0, 0, 0, 0, NULL, NULL }

/**
 * Register alias descriptor.
 */
typedef struct DBGFREGALIAS
{
    /** The alias name.  NULL is used to terminate the array. */
    const char     *pszName;
    /** Set to a valid type if the alias has a different type. */
    DBGFREGVALTYPE  enmType;
} DBGFREGALIAS;
/** Pointer to a const register alias descriptor. */
typedef DBGFREGALIAS const *PCDBGFREGALIAS;

/**
 * Register descriptor.
 */
typedef struct DBGFREGDESC
{
    /** The normal register name. */
    const char             *pszName;
    /** The register identifier if this is a CPU register. */
    DBGFREG                 enmReg;
    /** The default register type. */
    DBGFREGVALTYPE          enmType;
    /** Flags, see DBGFREG_FLAGS_XXX.  */
    uint32_t                fFlags;
    /** The internal register indicator.
     * For CPU registers this is the offset into the CPUMCTX structure,
     * thuse the 'off' prefix. */
    uint32_t                offRegister;
    /** Getter. */
    DECLCALLBACKMEMBER(int, pfnGet)(void *pvUser, struct DBGFREGDESC const *pDesc, PDBGFREGVAL pValue);
    /** Setter. */
    DECLCALLBACKMEMBER(int, pfnSet)(void *pvUser, struct DBGFREGDESC const *pDesc, PCDBGFREGVAL pValue, PCDBGFREGVAL pfMask);
    /** Aliases (optional). */
    PCDBGFREGALIAS          paAliases;
    /** Sub fields (optional). */
    PCDBGFREGSUBFIELD       paSubFields;
} DBGFREGDESC;

/** @name Macros for constructing DBGFREGDESC arrays.
 * @{ */
#define DBGFREGDESC_RW(a_szName, a_TypeSuff, a_offRegister, a_pfnGet, a_pfnSet) \
    { a_szName, DBGFREG_END, DBGFREGVALTYPE_##a_TypeSuff, 0 /*fFlags*/,            a_offRegister, a_pfnGet, a_pfnSet, NULL /*paAlises*/, NULL /*paSubFields*/ }
#define DBGFREGDESC_RO(a_szName, a_TypeSuff, a_offRegister, a_pfnGet, a_pfnSet) \
    { a_szName, DBGFREG_END, DBGFREGVALTYPE_##a_TypeSuff, DBGFREG_FLAGS_READ_ONLY, a_offRegister, a_pfnGet, a_pfnSet, NULL /*paAlises*/, NULL /*paSubFields*/ }
#define DBGFREGDESC_RW_A(a_szName, a_TypeSuff, a_offRegister, a_pfnGet, a_pfnSet, a_paAliases) \
    { a_szName, DBGFREG_END, DBGFREGVALTYPE_##a_TypeSuff, 0 /*fFlags*/,            a_offRegister, a_pfnGet, a_pfnSet, a_paAliases, NULL /*paSubFields*/ }
#define DBGFREGDESC_RO_A(a_szName, a_TypeSuff, a_offRegister, a_pfnGet, a_pfnSet, a_paAliases) \
    { a_szName, DBGFREG_END, DBGFREGVALTYPE_##a_TypeSuff, DBGFREG_FLAGS_READ_ONLY, a_offRegister, a_pfnGet, a_pfnSet, a_paAliases, NULL /*paSubFields*/ }
#define DBGFREGDESC_RW_S(a_szName, a_TypeSuff, a_offRegister, a_pfnGet, a_pfnSet, a_paSubFields) \
    { a_szName, DBGFREG_END, DBGFREGVALTYPE_##a_TypeSuff, 0 /*fFlags*/,            a_offRegister, a_pfnGet, a_pfnSet, /*paAliases*/, a_paSubFields }
#define DBGFREGDESC_RO_S(a_szName, a_TypeSuff, a_offRegister, a_pfnGet, a_pfnSet, a_paSubFields) \
    { a_szName, DBGFREG_END, DBGFREGVALTYPE_##a_TypeSuff, DBGFREG_FLAGS_READ_ONLY, a_offRegister, a_pfnGet, a_pfnSet, /*paAliases*/, a_paSubFields }
#define DBGFREGDESC_RW_AS(a_szName, a_TypeSuff, a_offRegister, a_pfnGet, a_pfnSet, a_paAliases, a_paSubFields) \
    { a_szName, DBGFREG_END, DBGFREGVALTYPE_##a_TypeSuff, 0 /*fFlags*/,            a_offRegister, a_pfnGet, a_pfnSet, a_paAliases, a_paSubFields }
#define DBGFREGDESC_RO_AS(a_szName, a_TypeSuff, a_offRegister, a_pfnGet, a_pfnSet, a_paAliases, a_paSubFields) \
    { a_szName, DBGFREG_END, DBGFREGVALTYPE_##a_TypeSuff, DBGFREG_FLAGS_READ_ONLY, a_offRegister, a_pfnGet, a_pfnSet, a_paAliases, a_paSubFields }
#define DBGFREGDESC_TERMINATOR() \
    { NULL, DBGFREG_END, DBGFREGVALTYPE_INVALID, 0, 0, NULL, NULL, NULL, NULL }
/** @} */


/** @name DBGFREG_FLAGS_XXX
 * @{ */
/** The register is read-only. */
#define DBGFREG_FLAGS_READ_ONLY         RT_BIT_32(0)
/** @} */

/**
 * Entry in a batch query or set operation.
 */
typedef struct DBGFREGENTRY
{
    /** The register identifier. */
    DBGFREG         enmReg;
    /** The size of the value in bytes. */
    DBGFREGVALTYPE  enmType;
    /** The register value. The valid view is indicated by enmType. */
    DBGFREGVAL      Val;
} DBGFREGENTRY;
/** Pointer to a register entry in a batch operation. */
typedef DBGFREGENTRY *PDBGFREGENTRY;
/** Pointer to a const register entry in a batch operation. */
typedef DBGFREGENTRY const *PCDBGFREGENTRY;

/** Used with DBGFR3Reg* to indicate the hypervisor register set instead of the
 *  guest. */
#define DBGFREG_HYPER_VMCPUID       UINT32_C(0x01000000)

VMMR3DECL(int) DBGFR3RegCpuQueryU8(  PVM pVM, VMCPUID idCpu, DBGFREG enmReg, uint8_t     *pu8);
VMMR3DECL(int) DBGFR3RegCpuQueryU16( PVM pVM, VMCPUID idCpu, DBGFREG enmReg, uint16_t    *pu16);
VMMR3DECL(int) DBGFR3RegCpuQueryU32( PVM pVM, VMCPUID idCpu, DBGFREG enmReg, uint32_t    *pu32);
VMMR3DECL(int) DBGFR3RegCpuQueryU64( PVM pVM, VMCPUID idCpu, DBGFREG enmReg, uint64_t    *pu64);
VMMR3DECL(int) DBGFR3RegCpuQueryU128(PVM pVM, VMCPUID idCpu, DBGFREG enmReg, uint128_t   *pu128);
VMMR3DECL(int) DBGFR3RegCpuQueryLrd( PVM pVM, VMCPUID idCpu, DBGFREG enmReg, long double *plrd);
VMMR3DECL(int) DBGFR3RegCpuQueryXdtr(PVM pVM, VMCPUID idCpu, DBGFREG enmReg, uint64_t *pu64Base, uint16_t *pu16Limit);
#if 0
VMMR3DECL(int) DBGFR3RegCpuQueryBatch(PVM pVM,VMCPUID idCpu, PDBGFREGENTRY paRegs, size_t cRegs);
VMMR3DECL(int) DBGFR3RegCpuQueryAll( PVM pVM, VMCPUID idCpu, PDBGFREGENTRY paRegs, size_t cRegs);

VMMR3DECL(int) DBGFR3RegCpuSetU8(    PVM pVM, VMCPUID idCpu, DBGFREG enmReg, uint8_t     u8);
VMMR3DECL(int) DBGFR3RegCpuSetU16(   PVM pVM, VMCPUID idCpu, DBGFREG enmReg, uint16_t    u16);
VMMR3DECL(int) DBGFR3RegCpuSetU32(   PVM pVM, VMCPUID idCpu, DBGFREG enmReg, uint32_t    u32);
VMMR3DECL(int) DBGFR3RegCpuSetU64(   PVM pVM, VMCPUID idCpu, DBGFREG enmReg, uint64_t    u64);
VMMR3DECL(int) DBGFR3RegCpuSetU128(  PVM pVM, VMCPUID idCpu, DBGFREG enmReg, uint128_t   u128);
VMMR3DECL(int) DBGFR3RegCpuSetLrd(   PVM pVM, VMCPUID idCpu, DBGFREG enmReg, long double lrd);
VMMR3DECL(int) DBGFR3RegCpuSetBatch( PVM pVM, VMCPUID idCpu, PCDBGFREGENTRY paRegs, size_t cRegs);
#endif

VMMR3DECL(const char *) DBGFR3RegCpuName(PVM pVM, DBGFREG enmReg, DBGFREGVALTYPE enmType);

VMMR3_INT_DECL(int) DBGFR3RegRegisterCpu(PVM pVM, PVMCPU pVCpu, PCDBGFREGDESC paRegisters, bool fGuestRegs);
VMMR3DECL(int)      DBGFR3RegRegisterDevice(PVM pVM, PCDBGFREGDESC paRegisters, PPDMDEVINS pDevIns, const char *pszPrefix, uint32_t iInstance);

/**
 * Entry in a named batch query or set operation.
 */
typedef struct DBGFREGENTRYNM
{
    /** The register name. */
    const char     *pszName;
    /** The size of the value in bytes. */
    DBGFREGVALTYPE  enmType;
    /** The register value. The valid view is indicated by enmType. */
    DBGFREGVAL      Val;
} DBGFREGENTRYNM;
/** Pointer to a named register entry in a batch operation. */
typedef DBGFREGENTRYNM *PDBGFREGENTRYNM;
/** Pointer to a const named register entry in a batch operation. */
typedef DBGFREGENTRYNM const *PCDBGFREGENTRYNM;

VMMR3DECL(int) DBGFR3RegNmQuery(    PVM pVM, VMCPUID idDefCpu, const char *pszReg, PDBGFREGVAL pValue, PDBGFREGVALTYPE penmType);
VMMR3DECL(int) DBGFR3RegNmQueryU8(  PVM pVM, VMCPUID idDefCpu, const char *pszReg, uint8_t     *pu8);
VMMR3DECL(int) DBGFR3RegNmQueryU16( PVM pVM, VMCPUID idDefCpu, const char *pszReg, uint16_t    *pu16);
VMMR3DECL(int) DBGFR3RegNmQueryU32( PVM pVM, VMCPUID idDefCpu, const char *pszReg, uint32_t    *pu32);
VMMR3DECL(int) DBGFR3RegNmQueryU64( PVM pVM, VMCPUID idDefCpu, const char *pszReg, uint64_t    *pu64);
VMMR3DECL(int) DBGFR3RegNmQueryU128(PVM pVM, VMCPUID idDefCpu, const char *pszReg, PRTUINT128U  pu128);
/*VMMR3DECL(int) DBGFR3RegNmQueryLrd( PVM pVM, VMCPUID idDefCpu, const char *pszReg, long double *plrd);*/
VMMR3DECL(int) DBGFR3RegNmQueryXdtr(PVM pVM, VMCPUID idDefCpu, const char *pszReg, uint64_t *pu64Base, uint16_t *pu16Limit);
VMMR3DECL(int) DBGFR3RegNmQueryBatch(PVM pVM,VMCPUID idDefCpu, PDBGFREGENTRYNM paRegs, size_t cRegs);
VMMR3DECL(int) DBGFR3RegNmQueryAllCount(PVM pVM, size_t *pcRegs);
VMMR3DECL(int) DBGFR3RegNmQueryAll( PVM pVM,                   PDBGFREGENTRYNM paRegs, size_t cRegs);

VMMR3DECL(int) DBGFR3RegNmSet(      PVM pVM, VMCPUID idDefCpu, const char *pszReg, PCDBGFREGVAL pValue, DBGFREGVALTYPE enmType);
VMMR3DECL(int) DBGFR3RegNmSetU8(    PVM pVM, VMCPUID idDefCpu, const char *pszReg, uint8_t     u8);
VMMR3DECL(int) DBGFR3RegNmSetU16(   PVM pVM, VMCPUID idDefCpu, const char *pszReg, uint16_t    u16);
VMMR3DECL(int) DBGFR3RegNmSetU32(   PVM pVM, VMCPUID idDefCpu, const char *pszReg, uint32_t    u32);
VMMR3DECL(int) DBGFR3RegNmSetU64(   PVM pVM, VMCPUID idDefCpu, const char *pszReg, uint64_t    u64);
VMMR3DECL(int) DBGFR3RegNmSetU128(  PVM pVM, VMCPUID idDefCpu, const char *pszReg, RTUINT128U  u128);
VMMR3DECL(int) DBGFR3RegNmSetLrd(   PVM pVM, VMCPUID idDefCpu, const char *pszReg, long double lrd);
VMMR3DECL(int) DBGFR3RegNmSetBatch( PVM pVM, VMCPUID idDefCpu, PCDBGFREGENTRYNM paRegs, size_t cRegs);

/** @todo add enumeration methods.  */

VMMR3DECL(int) DBGFR3RegPrintf( PVM pVM, VMCPUID idDefCpu, char *pszBuf, size_t cbBuf, const char *pszFormat, ...);
VMMR3DECL(int) DBGFR3RegPrintfV(PVM pVM, VMCPUID idDefCpu, char *pszBuf, size_t cbBuf, const char *pszFormat, va_list va);


/**
 * Guest OS digger interface identifier.
 *
 * This is for use together with PDBGFR3QueryInterface and is used to
 * obtain access to optional interfaces.
 */
typedef enum DBGFOSINTERFACE
{
    /** The usual invalid entry. */
    DBGFOSINTERFACE_INVALID = 0,
    /** Process info. */
    DBGFOSINTERFACE_PROCESS,
    /** Thread info. */
    DBGFOSINTERFACE_THREAD,
    /** The end of the valid entries. */
    DBGFOSINTERFACE_END,
    /** The usual 32-bit type blowup. */
    DBGFOSINTERFACE_32BIT_HACK = 0x7fffffff
} DBGFOSINTERFACE;
/** Pointer to a Guest OS digger interface identifier. */
typedef DBGFOSINTERFACE *PDBGFOSINTERFACE;
/** Pointer to a const Guest OS digger interface identifier. */
typedef DBGFOSINTERFACE const *PCDBGFOSINTERFACE;


/**
 * Guest OS Digger Registration Record.
 *
 * This is used with the DBGFR3OSRegister() API.
 */
typedef struct DBGFOSREG
{
    /** Magic value (DBGFOSREG_MAGIC). */
    uint32_t u32Magic;
    /** Flags. Reserved. */
    uint32_t fFlags;
    /** The size of the instance data. */
    uint32_t cbData;
    /** Operative System name. */
    char szName[24];

    /**
     * Constructs the instance.
     *
     * @returns VBox status code.
     * @param   pVM     Pointer to the shared VM structure.
     * @param   pvData  Pointer to the instance data.
     */
    DECLCALLBACKMEMBER(int, pfnConstruct)(PVM pVM, void *pvData);

    /**
     * Destroys the instance.
     *
     * @param   pVM     Pointer to the shared VM structure.
     * @param   pvData  Pointer to the instance data.
     */
    DECLCALLBACKMEMBER(void, pfnDestruct)(PVM pVM, void *pvData);

    /**
     * Probes the guest memory for OS finger prints.
     *
     * No setup or so is performed, it will be followed by a call to pfnInit
     * or pfnRefresh that should take care of that.
     *
     * @returns true if is an OS handled by this module, otherwise false.
     * @param   pVM     Pointer to the shared VM structure.
     * @param   pvData  Pointer to the instance data.
     */
    DECLCALLBACKMEMBER(bool, pfnProbe)(PVM pVM, void *pvData);

    /**
     * Initializes a fresly detected guest, loading symbols and such useful stuff.
     *
     * This is called after pfnProbe.
     *
     * @returns VBox status code.
     * @param   pVM     Pointer to the shared VM structure.
     * @param   pvData  Pointer to the instance data.
     */
    DECLCALLBACKMEMBER(int, pfnInit)(PVM pVM, void *pvData);

    /**
     * Refreshes symbols and stuff following a redetection of the same OS.
     *
     * This is called after pfnProbe.
     *
     * @returns VBox status code.
     * @param   pVM     Pointer to the shared VM structure.
     * @param   pvData  Pointer to the instance data.
     */
    DECLCALLBACKMEMBER(int, pfnRefresh)(PVM pVM, void *pvData);

    /**
     * Terminates an OS when a new (or none) OS has been detected,
     * and before destruction.
     *
     * This is called after pfnProbe and if needed before pfnDestruct.
     *
     * @param   pVM     Pointer to the shared VM structure.
     * @param   pvData  Pointer to the instance data.
     */
    DECLCALLBACKMEMBER(void, pfnTerm)(PVM pVM, void *pvData);

    /**
     * Queries the version of the running OS.
     *
     * This is only called after pfnInit().
     *
     * @returns VBox status code.
     * @param   pVM         Pointer to the shared VM structure.
     * @param   pvData      Pointer to the instance data.
     * @param   pszVersion  Where to store the version string.
     * @param   cchVersion  The size of the version string buffer.
     */
    DECLCALLBACKMEMBER(int, pfnQueryVersion)(PVM pVM, void *pvData, char *pszVersion, size_t cchVersion);

    /**
     * Queries the pointer to a interface.
     *
     * This is called after pfnProbe.
     *
     * @returns Pointer to the interface if available, NULL if not available.
     * @param   pVM     Pointer to the shared VM structure.
     * @param   pvData  Pointer to the instance data.
     * @param   enmIf   The interface identifier.
     */
    DECLCALLBACKMEMBER(void *, pfnQueryInterface)(PVM pVM, void *pvData, DBGFOSINTERFACE enmIf);

    /** Trailing magic (DBGFOSREG_MAGIC). */
    uint32_t u32EndMagic;
} DBGFOSREG;
/** Pointer to a Guest OS digger registration record. */
typedef DBGFOSREG *PDBGFOSREG;
/** Pointer to a const Guest OS digger registration record. */
typedef DBGFOSREG const *PCDBGFOSREG;

/** Magic value for DBGFOSREG::u32Magic and DBGFOSREG::u32EndMagic. (Hitomi Kanehara) */
#define DBGFOSREG_MAGIC     0x19830808

VMMR3DECL(int)      DBGFR3OSRegister(PVM pVM, PCDBGFOSREG pReg);
VMMR3DECL(int)      DBGFR3OSDeregister(PVM pVM, PCDBGFOSREG pReg);
VMMR3DECL(int)      DBGFR3OSDetect(PVM pVM, char *pszName, size_t cchName);
VMMR3DECL(int)      DBGFR3OSQueryNameAndVersion(PVM pVM, char *pszName, size_t cchName, char *pszVersion, size_t cchVersion);
VMMR3DECL(void *)   DBGFR3OSQueryInterface(PVM pVM, DBGFOSINTERFACE enmIf);


VMMR3DECL(int)      DBGFR3CoreWrite(PVM pVM, const char *pszFilename, bool fReplaceFile);

/** @} */


RT_C_DECLS_END

#endif

