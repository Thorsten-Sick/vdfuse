/** @file
 * PDM - Pluggable Device Manager, Core API.
 *
 * The 'Core API' has been put in a different header because everyone
 * is currently including pdm.h. So, pdm.h is for including all of the
 * PDM stuff, while pdmapi.h is for the core stuff.
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

#ifndef ___VBox_vmm_pdmapi_h
#define ___VBox_vmm_pdmapi_h

#include <VBox/vmm/pdmcommon.h>
#include <VBox/sup.h>


RT_C_DECLS_BEGIN

/** @defgroup grp_pdm       The Pluggable Device Manager API
 * @{
 */

VMMDECL(int)    PDMGetInterrupt(PVMCPU pVCpu, uint8_t *pu8Interrupt);
VMMDECL(int)    PDMIsaSetIrq(PVM pVM, uint8_t u8Irq, uint8_t u8Level);
VMMDECL(int)    PDMIoApicSetIrq(PVM pVM, uint8_t u8Irq, uint8_t u8Level);
VMMDECL(int)    PDMIoApicSendMsi(PVM pVM, RTGCPHYS GCAddr, uint32_t uValue);
VMMDECL(bool)   PDMHasIoApic(PVM pVM);
VMMDECL(int)    PDMApicHasPendingIrq(PVM pVM, bool *pfPending);
VMMDECL(int)    PDMApicSetBase(PVM pVM, uint64_t u64Base);
VMMDECL(int)    PDMApicGetBase(PVM pVM, uint64_t *pu64Base);
VMMDECL(int)    PDMApicSetTPR(PVMCPU pVCpu, uint8_t u8TPR);
VMMDECL(int)    PDMApicGetTPR(PVMCPU pVCpu, uint8_t *pu8TPR, bool *pfPending);
VMMDECL(int)    PDMApicWriteMSR(PVM pVM, VMCPUID iCpu, uint32_t u32Reg, uint64_t u64Value);
VMMDECL(int)    PDMApicReadMSR(PVM pVM, VMCPUID iCpu, uint32_t u32Reg, uint64_t *pu64Value);
VMMDECL(int)    PDMVMMDevHeapR3ToGCPhys(PVM pVM, RTR3PTR pv, RTGCPHYS *pGCPhys);
VMMDECL(bool)   PDMVMMDevHeapIsEnabled(PVM pVM);


/** @defgroup grp_pdm_r3    The PDM Host Context Ring-3 API
 * @ingroup grp_pdm
 * @{
 */

VMMR3DECL(int)  PDMR3InitUVM(PUVM pUVM);
VMMR3DECL(int)  PDMR3LdrLoadVMMR0U(PUVM pUVM);
VMMR3DECL(int)  PDMR3Init(PVM pVM);
VMMR3DECL(void) PDMR3PowerOn(PVM pVM);
VMMR3DECL(void) PDMR3ResetCpu(PVMCPU pVCpu);
VMMR3DECL(void) PDMR3Reset(PVM pVM);
VMMR3DECL(void) PDMR3Suspend(PVM pVM);
VMMR3DECL(void) PDMR3Resume(PVM pVM);
VMMR3DECL(void) PDMR3PowerOff(PVM pVM);
VMMR3DECL(void) PDMR3Relocate(PVM pVM, RTGCINTPTR offDelta);
VMMR3DECL(int)  PDMR3Term(PVM pVM);
VMMR3DECL(void) PDMR3TermUVM(PUVM pUVM);

/**
 * Module enumeration callback function.
 *
 * @returns VBox status.
 *          Failure will stop the search and return the return code.
 *          Warnings will be ignored and not returned.
 * @param   pVM             VM Handle.
 * @param   pszFilename     Module filename.
 * @param   pszName         Module name. (short and unique)
 * @param   ImageBase       Address where to executable image is loaded.
 * @param   cbImage         Size of the executable image.
 * @param   fRC             Set if raw-mode context, clear if host context.
 * @param   pvArg           User argument.
 */
typedef DECLCALLBACK(int) FNPDMR3ENUM(PVM pVM, const char *pszFilename, const char *pszName,
                                      RTUINTPTR ImageBase, size_t cbImage, bool fRC, void *pvArg);
/** Pointer to a FNPDMR3ENUM() function. */
typedef FNPDMR3ENUM *PFNPDMR3ENUM;
VMMR3DECL(int)  PDMR3LdrEnumModules(PVM pVM, PFNPDMR3ENUM pfnCallback, void *pvArg);
VMMR3DECL(void) PDMR3LdrRelocateU(PUVM pUVM, RTGCINTPTR offDelta);
VMMR3DECL(int)  PDMR3LdrGetSymbolR3(PVM pVM, const char *pszModule, const char *pszSymbol, void **ppvValue);
VMMR3DECL(int)  PDMR3LdrGetSymbolR0(PVM pVM, const char *pszModule, const char *pszSymbol, PRTR0PTR ppvValue);
VMMR3DECL(int)  PDMR3LdrGetSymbolR0Lazy(PVM pVM, const char *pszModule, const char *pszSearchPath, const char *pszSymbol, PRTR0PTR ppvValue);
VMMR3DECL(int)  PDMR3LdrLoadRC(PVM pVM, const char *pszFilename, const char *pszName);
VMMR3DECL(int)  PDMR3LdrGetSymbolRC(PVM pVM, const char *pszModule, const char *pszSymbol, PRTRCPTR pRCPtrValue);
VMMR3DECL(int)  PDMR3LdrGetSymbolRCLazy(PVM pVM, const char *pszModule, const char *pszSearchPath, const char *pszSymbol, PRTRCPTR pRCPtrValue);
VMMR3DECL(int)  PDMR3LdrQueryRCModFromPC(PVM pVM, RTRCPTR uPC,
                                         char *pszModName,  size_t cchModName,  PRTRCPTR pMod,
                                         char *pszNearSym1, size_t cchNearSym1, PRTRCPTR pNearSym1,
                                         char *pszNearSym2, size_t cchNearSym2, PRTRCPTR pNearSym2);
VMMR3DECL(int)  PDMR3LdrQueryR0ModFromPC(PVM pVM, RTR0PTR uPC,
                                         char *pszModName,  size_t cchModName,  PRTR0PTR pMod,
                                         char *pszNearSym1, size_t cchNearSym1, PRTR0PTR pNearSym1,
                                         char *pszNearSym2, size_t cchNearSym2, PRTR0PTR pNearSym2);
VMMR3DECL(int)  PDMR3LdrGetInterfaceSymbols(PVM pVM,
                                            void *pvInterface, size_t cbInterface,
                                            const char *pszModule, const char *pszSearchPath,
                                            const char *pszSymPrefix, const char *pszSymList,
                                            bool fRing0OrRC);

VMMR3DECL(int)  PDMR3QueryDevice(PVM pVM, const char *pszDevice, unsigned iInstance, PPPDMIBASE ppBase);
VMMR3DECL(int)  PDMR3QueryDeviceLun(PVM pVM, const char *pszDevice, unsigned iInstance, unsigned iLun, PPPDMIBASE ppBase);
VMMR3DECL(int)  PDMR3QueryLun(PVM pVM, const char *pszDevice, unsigned iInstance, unsigned iLun, PPPDMIBASE ppBase);
VMMR3DECL(int)  PDMR3QueryDriverOnLun(PVM pVM, const char *pszDevice, unsigned iInstance, unsigned iLun, const char *pszDriver,
                                      PPPDMIBASE ppBase);
VMMR3DECL(int)  PDMR3DeviceAttach(PVM pVM, const char *pszDevice, unsigned iInstance, unsigned iLun, uint32_t fFlags, PPDMIBASE *ppBase);
VMMR3DECL(int)  PDMR3DeviceDetach(PVM pVM, const char *pszDevice, unsigned iInstance, unsigned iLun, uint32_t fFlags);
VMMR3DECL(int)  PDMR3DriverAttach(PVM pVM, const char *pszDevice, unsigned iDevIns, unsigned iLun, uint32_t fFlags, PPPDMIBASE ppBase);
VMMR3DECL(int)  PDMR3DriverDetach(PVM pVM, const char *pszDevice, unsigned iDevIns, unsigned iLun,
                                  const char *pszDriver, unsigned iOccurance, uint32_t fFlags);
VMMR3DECL(int)  PDMR3DriverReattach(PVM pVM, const char *pszDevice, unsigned iDevIns, unsigned iLun,
                                    const char *pszDriver, unsigned iOccurance, uint32_t fFlags, PCFGMNODE pCfg, PPPDMIBASE ppBase);
VMMR3DECL(void) PDMR3DmaRun(PVM pVM);
VMMR3DECL(int)  PDMR3LockCall(PVM pVM);
VMMR3DECL(int)  PDMR3RegisterVMMDevHeap(PVM pVM, RTGCPHYS GCPhys, RTR3PTR pvHeap, unsigned cbSize);
VMMR3DECL(int)  PDMR3VMMDevHeapAlloc(PVM pVM, unsigned cbSize, RTR3PTR *ppv);
VMMR3DECL(int)  PDMR3VMMDevHeapFree(PVM pVM, RTR3PTR pv);
VMMR3DECL(int)  PDMR3UnregisterVMMDevHeap(PVM pVM, RTGCPHYS GCPhys);
/** @} */



/** @defgroup grp_pdm_rc    The PDM Raw-Mode Context API
 * @ingroup grp_pdm
 * @{
 */
/** @} */



/** @defgroup grp_pdm_r0    The PDM Ring-0 Context API
 * @ingroup grp_pdm
 * @{
 */

/**
 * Request buffer for PDMR0DriverCallReqHandler / VMMR0_DO_PDM_DRIVER_CALL_REQ_HANDLER.
 * @see PDMR0DriverCallReqHandler.
 */
typedef struct PDMDRIVERCALLREQHANDLERREQ
{
    /** The header. */
    SUPVMMR0REQHDR      Hdr;
    /** The driver instance. */
    PPDMDRVINSR0        pDrvInsR0;
    /** The operation. */
    uint32_t            uOperation;
    /** Explicit alignment padding. */
    uint32_t            u32Alignment;
    /** Optional 64-bit integer argument. */
    uint64_t            u64Arg;
} PDMDRIVERCALLREQHANDLERREQ;
/** Pointer to a PDMR0DriverCallReqHandler / VMMR0_DO_PDM_DRIVER_CALL_REQ_HANDLER
 * request buffer. */
typedef PDMDRIVERCALLREQHANDLERREQ *PPDMDRIVERCALLREQHANDLERREQ;

VMMR0_INT_DECL(int) PDMR0DriverCallReqHandler(PVM pVM, PPDMDRIVERCALLREQHANDLERREQ pReq);

/**
 * Request buffer for PDMR0DeviceCallReqHandler / VMMR0_DO_PDM_DEVICE_CALL_REQ_HANDLER.
 * @see PDMR0DeviceCallReqHandler.
 */
typedef struct PDMDEVICECALLREQHANDLERREQ
{
    /** The header. */
    SUPVMMR0REQHDR          Hdr;
    /** The device instance. */
    PPDMDEVINSR0            pDevInsR0;
    /** The request handler for the device. */
    PFNPDMDEVREQHANDLERR0   pfnReqHandlerR0;
    /** The operation. */
    uint32_t                uOperation;
    /** Explicit alignment padding. */
    uint32_t                u32Alignment;
    /** Optional 64-bit integer argument. */
    uint64_t                u64Arg;
} PDMDEVICECALLREQHANDLERREQ;
/** Pointer to a PDMR0DeviceCallReqHandler /
 * VMMR0_DO_PDM_DEVICE_CALL_REQ_HANDLER request buffer. */
typedef PDMDEVICECALLREQHANDLERREQ *PPDMDEVICECALLREQHANDLERREQ;

VMMR0_INT_DECL(int) PDMR0DeviceCallReqHandler(PVM pVM, PPDMDEVICECALLREQHANDLERREQ pReq);

/** @} */

RT_C_DECLS_END

/** @} */

#endif
