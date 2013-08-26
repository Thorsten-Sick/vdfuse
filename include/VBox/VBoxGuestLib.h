/** @file
 * VBoxGuestLib - VirtualBox Guest Additions Library.
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

#ifndef ___VBox_VBoxGuestLib_h
#define ___VBox_VBoxGuestLib_h

#include <VBox/types.h>
#include <VBox/VMMDev2.h>
#include <VBox/VMMDev.h>     /* grumble */
#ifdef IN_RING0
# include <VBox/VBoxGuest2.h>
#endif


/** @defgroup grp_guest_lib     VirtualBox Guest Additions Library
 * @{
 */

/** @page pg_guest_lib  VirtualBox Guest Library
 *
 * This is a library for abstracting the additions driver interface. There are
 * multiple versions of the library depending on the context. The main
 * distinction is between kernel and user mode where the interfaces are very
 * different.
 *
 *
 * @section sec_guest_lib_ring0     Ring-0
 *
 * In ring-0 there are two version:
 *  - VBOX_LIB_VBGL_R0_BASE / VBoxGuestR0LibBase for the VBoxGuest main driver,
 *    who is responsible for managing the VMMDev virtual hardware.
 *  - VBOX_LIB_VBGL_R0 / VBoxGuestR0Lib for other (client) guest drivers.
 *
 *
 * The library source code and the header have a define VBGL_VBOXGUEST, which is
 * defined for VBoxGuest and undefined for other drivers. Drivers must choose
 * right library in their makefiles and set VBGL_VBOXGUEST accordingly.
 *
 * The libraries consists of:
 *  - common code to be used by both VBoxGuest and other drivers;
 *  - VBoxGuest specific code;
 *  - code for other drivers which communicate with VBoxGuest via an IOCTL.
 *
 *
 * @section sec_guest_lib_ring3     Ring-3
 *
 * There are more variants of the library here:
 *  - VBOX_LIB_VBGL_R3 / VBoxGuestR3Lib for programs.
 *  - VBOX_LIB_VBGL_R3_XFREE86 / VBoxGuestR3LibXFree86 for old style XFree
 *    drivers which uses special loader and or symbol resolving strategy.
 *  - VBOX_LIB_VBGL_R3_SHARED / VBoxGuestR3LibShared for shared objects / DLLs /
 *    Dylibs.
 *
 */

RT_C_DECLS_BEGIN


/** @defgroup grp_guest_lib_r0     Ring-0 interface.
 * @{
 */
#if defined(IN_RING0) && !defined(IN_RING0_AGNOSTIC)
/** @def DECLR0VBGL
 * Declare a VBGL ring-0 API with the right calling convention and visibilitiy.
 * @param type      Return type.  */
# define DECLR0VBGL(type) type VBOXCALL
# define DECLVBGL(type) DECLR0VBGL(type)

typedef uint32_t VBGLIOPORT; /**< @todo r=bird: We have RTIOPORT (uint16_t) for this. */


# ifdef VBGL_VBOXGUEST

/**
 * The library initialization function to be used by the main
 * VBoxGuest system driver.
 *
 * @return VBox status code.
 */
DECLVBGL(int) VbglInit (VBGLIOPORT portVMMDev, struct VMMDevMemory *pVMMDevMemory);

# else

/**
 * The library initialization function to be used by all drivers
 * other than the main VBoxGuest system driver.
 *
 * @return VBox status code.
 */
DECLVBGL(int) VbglInit (void);

# endif

/**
 * The library termination function.
 */
DECLVBGL(void) VbglTerminate (void);


/** @name Generic request functions.
 * @{
 */

/**
 * Allocate memory for generic request and initialize the request header.
 *
 * @param ppReq    pointer to resulting memory address.
 * @param cbSize   size of memory block required for the request.
 * @param reqType  the generic request type.
 *
 * @return VBox status code.
 */
DECLVBGL(int) VbglGRAlloc (VMMDevRequestHeader **ppReq, uint32_t cbSize, VMMDevRequestType reqType);

/**
 * Perform the generic request.
 *
 * @param pReq     pointer the request structure.
 *
 * @return VBox status code.
 */
DECLVBGL(int) VbglGRPerform (VMMDevRequestHeader *pReq);

/**
 * Free the generic request memory.
 *
 * @param pReq     pointer the request structure.
 *
 * @return VBox status code.
 */
DECLVBGL(void) VbglGRFree (VMMDevRequestHeader *pReq);

/**
 * Verify the generic request header.
 *
 * @param pReq     pointer the request header structure.
 * @param cbReq    size of the request memory block. It should be equal to the request size
 *                 for fixed size requests. It can be greater than the request size for
 *                 variable size requests.
 *
 * @return VBox status code.
 */
DECLVBGL(int) VbglGRVerify (const VMMDevRequestHeader *pReq, size_t cbReq);
/** @} */

# ifdef VBOX_WITH_HGCM

#  ifdef VBGL_VBOXGUEST

/**
 * Callback function called from HGCM helpers when a wait for request
 * completion IRQ is required.
 *
 * @returns VINF_SUCCESS, VERR_INTERRUPT or VERR_TIMEOUT.
 * @param   pvData      VBoxGuest pointer to be passed to callback.
 * @param   u32Data     VBoxGuest 32 bit value to be passed to callback.
 */
typedef DECLVBGL(int) FNVBGLHGCMCALLBACK(VMMDevHGCMRequestHeader *pHeader, void *pvData, uint32_t u32Data);
/** Pointer to a FNVBGLHGCMCALLBACK. */
typedef FNVBGLHGCMCALLBACK *PFNVBGLHGCMCALLBACK;

/**
 * Perform a connect request. That is locate required service and
 * obtain a client identifier for future access.
 *
 * @note This function can NOT handle cancelled requests!
 *
 * @param   pConnectInfo        The request data.
 * @param   pfnAsyncCallback    Required pointer to function that is calledwhen
 *                              host returns VINF_HGCM_ASYNC_EXECUTE. VBoxGuest
 *                              implements waiting for an IRQ in this function.
 * @param   pvAsyncData         An arbitrary VBoxGuest pointer to be passed to callback.
 * @param   u32AsyncData        An arbitrary VBoxGuest 32 bit value to be passed to callback.
 *
 * @return  VBox status code.
 */

DECLR0VBGL(int) VbglR0HGCMInternalConnect (VBoxGuestHGCMConnectInfo *pConnectInfo,
                                           PFNVBGLHGCMCALLBACK pfnAsyncCallback, void *pvAsyncData, uint32_t u32AsyncData);


/**
 * Perform a disconnect request. That is tell the host that
 * the client will not call the service anymore.
 *
 * @note This function can NOT handle cancelled requests!
 *
 * @param   pDisconnectInfo     The request data.
 * @param   pfnAsyncCallback    Required pointer to function that is called when
 *                              host returns VINF_HGCM_ASYNC_EXECUTE. VBoxGuest
 *                              implements waiting for an IRQ in this function.
 * @param   pvAsyncData         An arbitrary VBoxGuest pointer to be passed to callback.
 * @param   u32AsyncData        An arbitrary VBoxGuest 32 bit value to be passed to
 *                              callback.
 *
 * @return  VBox status code.
 */

DECLR0VBGL(int) VbglR0HGCMInternalDisconnect (VBoxGuestHGCMDisconnectInfo *pDisconnectInfo,
                                              PFNVBGLHGCMCALLBACK pfnAsyncCallback, void *pvAsyncData, uint32_t u32AsyncData);

/** Call a HGCM service.
 *
 * @note This function can deal with cancelled requests.
 *
 * @param   pCallInfo           The request data.
 * @param   fFlags              Flags, see VBGLR0_HGCMCALL_F_XXX.
 * @param   pfnAsyncCallback    Required pointer to function that is called when
 *                              host returns VINF_HGCM_ASYNC_EXECUTE. VBoxGuest
 *                              implements waiting for an IRQ in this function.
 * @param   pvAsyncData         An arbitrary VBoxGuest pointer to be passed to callback.
 * @param   u32AsyncData        An arbitrary VBoxGuest 32 bit value to be passed to callback.
 *
 * @return VBox status code.
 */
DECLR0VBGL(int) VbglR0HGCMInternalCall (VBoxGuestHGCMCallInfo *pCallInfo, uint32_t cbCallInfo, uint32_t fFlags,
                                        PFNVBGLHGCMCALLBACK pfnAsyncCallback, void *pvAsyncData, uint32_t u32AsyncData);

/** Call a HGCM service. (32 bits packet structure in a 64 bits guest)
 *
 * @note This function can deal with cancelled requests.
 *
 * @param   pCallInfo           The request data.
 * @param   fFlags              Flags, see VBGLR0_HGCMCALL_F_XXX.
 * @param   pfnAsyncCallback    Required pointer to function that is called when
 *                              host returns VINF_HGCM_ASYNC_EXECUTE. VBoxGuest
 *                              implements waiting for an IRQ in this function.
 * @param   pvAsyncData         An arbitrary VBoxGuest pointer to be passed to callback.
 * @param   u32AsyncData        An arbitrary VBoxGuest 32 bit value to be passed to callback.
 *
 * @return  VBox status code.
 */
DECLR0VBGL(int) VbglR0HGCMInternalCall32 (VBoxGuestHGCMCallInfo *pCallInfo, uint32_t cbCallInfo, uint32_t fFlags,
                                          PFNVBGLHGCMCALLBACK pfnAsyncCallback, void *pvAsyncData, uint32_t u32AsyncData);

/** @name VbglR0HGCMInternalCall flags
 * @{ */
/** User mode request.
 * Indicates that only user mode addresses are permitted as parameters. */
#define VBGLR0_HGCMCALL_F_USER          UINT32_C(0)
/** Kernel mode request.
 * Indicates that kernel mode addresses are permitted as parameters. Whether or
 * not user mode addresses are permitted is, unfortunately, OS specific. The
 * following OSes allows user mode addresses: Windows, TODO.
 */
#define VBGLR0_HGCMCALL_F_KERNEL        UINT32_C(1)
/** Mode mask. */
#define VBGLR0_HGCMCALL_F_MODE_MASK     UINT32_C(1)
/** @} */

#  else  /* !VBGL_VBOXGUEST */

struct VBGLHGCMHANDLEDATA;
typedef struct VBGLHGCMHANDLEDATA *VBGLHGCMHANDLE;

/** @name HGCM functions
 * @{
 */

/**
 * Connect to a service.
 *
 * @param pHandle     Pointer to variable that will hold a handle to be used
 *                    further in VbglHGCMCall and VbglHGCMClose.
 * @param pData       Connection information structure.
 *
 * @return VBox status code.
 */
DECLVBGL(int) VbglHGCMConnect (VBGLHGCMHANDLE *pHandle, VBoxGuestHGCMConnectInfo *pData);

/**
 * Connect to a service.
 *
 * @param handle      Handle of the connection.
 * @param pData       Disconnect request information structure.
 *
 * @return VBox status code.
 */
DECLVBGL(int) VbglHGCMDisconnect (VBGLHGCMHANDLE handle, VBoxGuestHGCMDisconnectInfo *pData);

/**
 * Call to a service.
 *
 * @param handle      Handle of the connection.
 * @param pData       Call request information structure, including function parameters.
 * @param cbData      Length in bytes of data.
 *
 * @return VBox status code.
 */
DECLVBGL(int) VbglHGCMCall (VBGLHGCMHANDLE handle, VBoxGuestHGCMCallInfo *pData, uint32_t cbData);

/**
 * Call to a service with timeout.
 *
 * @param handle      Handle of the connection.
 * @param pData       Call request information structure, including function parameters.
 * @param cbData      Length in bytes of data.
 * @param cMillies    Timeout in milliseconds.  Use RT_INDEFINITE_WAIT to wait forever.
 *
 * @return VBox status code.
 */
DECLVBGL(int) VbglHGCMCallTimed (VBGLHGCMHANDLE handle,
                                 VBoxGuestHGCMCallInfoTimed *pData, uint32_t cbData);
/** @} */

#  endif /* !VBGL_VBOXGUEST */

# endif /* VBOX_WITH_HGCM */


/**
 * Initialize the heap.
 *
 * @return VBox error code.
 */
DECLVBGL(int) VbglPhysHeapInit (void);

/**
 * Shutdown the heap.
 */
DECLVBGL(void) VbglPhysHeapTerminate (void);


/**
 * Allocate a memory block.
 *
 * @param cbSize    Size of block to be allocated.
 * @return Virtual address of allocated memory block.
 */
DECLVBGL(void *) VbglPhysHeapAlloc (uint32_t cbSize);

/**
 * Get physical address of memory block pointed by
 * the virtual address.
 *
 * @note WARNING!
 *       The function does not acquire the Heap mutex!
 *       When calling the function make sure that
 *       the pointer is a valid one and is not being
 *       deallocated.
 *       This function can NOT be used for verifying
 *       if the given pointer is a valid one allocated
 *       from the heap.
 *
 *
 * @param p    Virtual address of memory block.
 * @return Physical memory block.
 */
DECLVBGL(RTCCPHYS) VbglPhysHeapGetPhysAddr (void *p);

/**
 * Free a memory block.
 *
 * @param p    Virtual address of memory block.
 */
DECLVBGL(void) VbglPhysHeapFree (void *p);

DECLVBGL(int) VbglQueryVMMDevMemory (VMMDevMemory **ppVMMDevMemory);
DECLR0VBGL(bool) VbglR0CanUsePhysPageList(void);

#endif /* IN_RING0 && !IN_RING0_AGNOSTIC */
/** @} */


/** @defgroup grp_guest_lib_r3     Ring-3 interface.
 * @{
 */
#ifdef IN_RING3

/** @def VBGLR3DECL
 * Ring 3 VBGL declaration.
 * @param   type    The return type of the function declaration.
 */
# define VBGLR3DECL(type) type VBOXCALL

/** @name General-purpose functions
 * @{ */
VBGLR3DECL(int)     VbglR3Init(void);
VBGLR3DECL(int)     VbglR3InitUser(void);
VBGLR3DECL(void)    VbglR3Term(void);
# ifdef ___iprt_time_h
VBGLR3DECL(int)     VbglR3GetHostTime(PRTTIMESPEC pTime);
# endif
VBGLR3DECL(int)     VbglR3InterruptEventWaits(void);
VBGLR3DECL(int)     VbglR3WriteLog(const char *pch, size_t cch);
VBGLR3DECL(int)     VbglR3CtlFilterMask(uint32_t fOr, uint32_t fNot);
VBGLR3DECL(int)     VbglR3Daemonize(bool fNoChDir, bool fNoClose);
VBGLR3DECL(int)     VbglR3PidFile(const char *pszPath, PRTFILE phFile);
VBGLR3DECL(void)    VbglR3ClosePidFile(const char *pszPath, RTFILE hFile);
VBGLR3DECL(int)     VbglR3SetGuestCaps(uint32_t fOr, uint32_t fNot);
VBGLR3DECL(int)     VbglR3WaitEvent(uint32_t fMask, uint32_t cMillies, uint32_t *pfEvents);

VBGLR3DECL(int)     VbglR3ReportAdditionsStatus(VBoxGuestFacilityType Facility, VBoxGuestFacilityStatus StatusCurrent, uint32_t uFlags);
VBGLR3DECL(int)     VbglR3GetAdditionsVersion(char **ppszVer, char **ppszRev);
VBGLR3DECL(int)     VbglR3GetAdditionsInstallationPath(char **ppszPath);
VBGLR3DECL(int)     VbglR3GetSessionId(uint64_t *pu64IdSession);

/** @} */

/** @name Shared clipboard
 * @{ */
VBGLR3DECL(int)     VbglR3ClipboardConnect(uint32_t *pu32ClientId);
VBGLR3DECL(int)     VbglR3ClipboardDisconnect(uint32_t u32ClientId);
VBGLR3DECL(int)     VbglR3ClipboardGetHostMsg(uint32_t u32ClientId, uint32_t *pMsg, uint32_t *pfFormats);
VBGLR3DECL(int)     VbglR3ClipboardReadData(uint32_t u32ClientId, uint32_t fFormat, void *pv, uint32_t cb, uint32_t *pcb);
VBGLR3DECL(int)     VbglR3ClipboardReportFormats(uint32_t u32ClientId, uint32_t fFormats);
VBGLR3DECL(int)     VbglR3ClipboardWriteData(uint32_t u32ClientId, uint32_t fFormat, void *pv, uint32_t cb);
/** @} */

/** @name Seamless mode
 * @{ */
VBGLR3DECL(int)     VbglR3SeamlessSetCap(bool fState);
VBGLR3DECL(int)     VbglR3SeamlessWaitEvent(VMMDevSeamlessMode *pMode);
VBGLR3DECL(int)     VbglR3SeamlessSendRects(uint32_t cRects, PRTRECT pRects);
VBGLR3DECL(int)     VbglR3SeamlessGetLastEvent(VMMDevSeamlessMode *pMode);

/** @}  */

/** @name Mouse
 * @{ */
VBGLR3DECL(int)     VbglR3GetMouseStatus(uint32_t *pfFeatures, uint32_t *px, uint32_t *py);
VBGLR3DECL(int)     VbglR3SetMouseStatus(uint32_t fFeatures);
/** @}  */

/** @name Video
 * @{ */
VBGLR3DECL(int)     VbglR3VideoAccelEnable(bool fEnable);
VBGLR3DECL(int)     VbglR3VideoAccelFlush(void);
VBGLR3DECL(int)     VbglR3SetPointerShape(uint32_t fFlags, uint32_t xHot, uint32_t yHot, uint32_t cx, uint32_t cy, const void *pvImg, size_t cbImg);
VBGLR3DECL(int)     VbglR3SetPointerShapeReq(struct VMMDevReqMousePointer *pReq);
/** @}  */

/** @name Display
 * @{ */
VBGLR3DECL(int)     VbglR3GetDisplayChangeRequest(uint32_t *pcx, uint32_t *pcy, uint32_t *pcBits, uint32_t *piDisplay, bool fAck);
VBGLR3DECL(bool)    VbglR3HostLikesVideoMode(uint32_t cx, uint32_t cy, uint32_t cBits);
VBGLR3DECL(int)     VbglR3SaveVideoMode(const char *pszName, uint32_t cx, uint32_t cy, uint32_t cBits);
VBGLR3DECL(int)     VbglR3RetrieveVideoMode(const char *pszName, uint32_t *pcx, uint32_t *pcy, uint32_t *pcBits);
/** @}  */

/** @name VM Statistics
 * @{ */
VBGLR3DECL(int)     VbglR3StatQueryInterval(uint32_t *pu32Interval);
VBGLR3DECL(int)     VbglR3StatReport(VMMDevReportGuestStats *pReq);
/** @}  */

/** @name Memory ballooning
 * @{ */
VBGLR3DECL(int)     VbglR3MemBalloonRefresh(uint32_t *pcChunks, bool *pfHandleInR3);
VBGLR3DECL(int)     VbglR3MemBalloonChange(void *pv, bool fInflate);
/** @}  */

/** @name Core Dump
 * @{ */
VBGLR3DECL(int)     VbglR3WriteCoreDump(void);

/** @}  */

# ifdef VBOX_WITH_GUEST_PROPS
/** @name Guest properties
 * @{ */
/** @todo Docs. */
typedef struct VBGLR3GUESTPROPENUM VBGLR3GUESTPROPENUM;
/** @todo Docs. */
typedef VBGLR3GUESTPROPENUM *PVBGLR3GUESTPROPENUM;
VBGLR3DECL(int)     VbglR3GuestPropConnect(uint32_t *pu32ClientId);
VBGLR3DECL(int)     VbglR3GuestPropDisconnect(uint32_t u32ClientId);
VBGLR3DECL(int)     VbglR3GuestPropWrite(uint32_t u32ClientId, const char *pszName, const char *pszValue, const char *pszFlags);
VBGLR3DECL(int)     VbglR3GuestPropWriteValue(uint32_t u32ClientId, const char *pszName, const char *pszValue);
VBGLR3DECL(int)     VbglR3GuestPropWriteValueV(uint32_t u32ClientId, const char *pszName, const char *pszValueFormat, va_list va);
VBGLR3DECL(int)     VbglR3GuestPropWriteValueF(uint32_t u32ClientId, const char *pszName, const char *pszValueFormat, ...);
VBGLR3DECL(int)     VbglR3GuestPropRead(uint32_t u32ClientId, const char *pszName, void *pvBuf, uint32_t cbBuf, char **ppszValue, uint64_t *pu64Timestamp, char **ppszFlags, uint32_t *pcbBufActual);
VBGLR3DECL(int)     VbglR3GuestPropReadValue(uint32_t ClientId, const char *pszName, char *pszValue, uint32_t cchValue, uint32_t *pcchValueActual);
VBGLR3DECL(int)     VbglR3GuestPropReadValueAlloc(uint32_t u32ClientId, const char *pszName, char **ppszValue);
VBGLR3DECL(void)    VbglR3GuestPropReadValueFree(char *pszValue);
VBGLR3DECL(int)     VbglR3GuestPropEnumRaw(uint32_t u32ClientId, const char *paszPatterns, char *pcBuf, uint32_t cbBuf, uint32_t *pcbBufActual);
VBGLR3DECL(int)     VbglR3GuestPropEnum(uint32_t u32ClientId, char const * const *ppaszPatterns, uint32_t cPatterns, PVBGLR3GUESTPROPENUM *ppHandle,
                                        char const **ppszName, char const **ppszValue, uint64_t *pu64Timestamp, char const **ppszFlags);
VBGLR3DECL(int)     VbglR3GuestPropEnumNext(PVBGLR3GUESTPROPENUM pHandle, char const **ppszName, char const **ppszValue, uint64_t *pu64Timestamp,
                                            char const **ppszFlags);
VBGLR3DECL(void)    VbglR3GuestPropEnumFree(PVBGLR3GUESTPROPENUM pHandle);
VBGLR3DECL(int)     VbglR3GuestPropDelSet(uint32_t u32ClientId, char const * const *papszPatterns, uint32_t cPatterns);
VBGLR3DECL(int)     VbglR3GuestPropWait(uint32_t u32ClientId, const char *pszPatterns, void *pvBuf, uint32_t cbBuf, uint64_t u64Timestamp, uint32_t cMillies, char ** ppszName, char **ppszValue, uint64_t *pu64Timestamp, char **ppszFlags, uint32_t *pcbBufActual);
/** @}  */

/** @name Host version handling
 * @{ */
VBGLR3DECL(int)     VbglR3HostVersionCheckForUpdate(uint32_t u32ClientId, bool *pfUpdate, char **ppszHostVersion, char **ppszGuestVersion);
VBGLR3DECL(int)     VbglR3HostVersionLastCheckedLoad(uint32_t u32ClientId, char **ppszVer);
VBGLR3DECL(int)     VbglR3HostVersionLastCheckedStore(uint32_t u32ClientId, const char *pszVer);
/** @}  */
# endif /* VBOX_WITH_GUEST_PROPS defined */

# ifdef VBOX_WITH_SHARED_FOLDERS
/** @name Shared folders
 * @{ */
/**
 * Structure containing mapping information for a shared folder.
 */
typedef struct VBGLR3SHAREDFOLDERMAPPING
{
    /** Mapping status. */
    uint32_t u32Status;
    /** Root handle. */
    uint32_t u32Root;
} VBGLR3SHAREDFOLDERMAPPING;
/** Pointer to a shared folder mapping information struct. */
typedef VBGLR3SHAREDFOLDERMAPPING *PVBGLR3SHAREDFOLDERMAPPING;

VBGLR3DECL(int)     VbglR3SharedFolderConnect(uint32_t *pu32ClientId);
VBGLR3DECL(int)     VbglR3SharedFolderDisconnect(uint32_t u32ClientId);
VBGLR3DECL(bool)    VbglR3SharedFolderExists(uint32_t u32ClientId, const char *pszShareName);
VBGLR3DECL(int)     VbglR3SharedFolderGetMappings(uint32_t u32ClientId, bool fAutoMountOnly,
                                                  PVBGLR3SHAREDFOLDERMAPPING *ppaMappings, uint32_t *pcMappings);
VBGLR3DECL(void)    VbglR3SharedFolderFreeMappings(PVBGLR3SHAREDFOLDERMAPPING paMappings);
VBGLR3DECL(int)     VbglR3SharedFolderGetName(uint32_t  u32ClientId,uint32_t u32Root, char **ppszName);
VBGLR3DECL(int)     VbglR3SharedFolderGetMountPrefix(char **ppszPrefix);
VBGLR3DECL(int)     VbglR3SharedFolderGetMountDir(char **ppszDir);
/** @}  */
# endif /* VBOX_WITH_SHARED_FOLDERS defined */

# ifdef VBOX_WITH_GUEST_CONTROL
/** @name Guest control
 * @{ */
VBGLR3DECL(int)     VbglR3GuestCtrlConnect(uint32_t *pu32ClientId);
VBGLR3DECL(int)     VbglR3GuestCtrlDisconnect(uint32_t u32ClientId);
VBGLR3DECL(int)     VbglR3GuestCtrlGetHostMsg(uint32_t u32ClientId, uint32_t *puMsg, uint32_t *puNumParms);
VBGLR3DECL(int)     VbglR3GuestCtrlCancelPendingWaits(uint32_t u32ClientId);
VBGLR3DECL(int)     VbglR3GuestCtrlExecGetHostCmd(uint32_t  u32ClientId,    uint32_t  uNumParms,
                                                  uint32_t *puContext,
                                                  char     *pszCmd,         uint32_t  cbCmd,
                                                  uint32_t *puFlags,
                                                  char     *pszArgs,        uint32_t  cbArgs,   uint32_t *puNumArgs,
                                                  char     *pszEnv,         uint32_t *pcbEnv,   uint32_t *puNumEnvVars,
                                                  char     *pszUser,        uint32_t  cbUser,
                                                  char     *pszPassword,    uint32_t  cbPassword,
                                                  uint32_t *puTimeLimit);
VBGLR3DECL(int)     VbglR3GuestCtrlExecGetHostCmdInput(uint32_t  u32ClientId,    uint32_t   uNumParms,
                                                       uint32_t *puContext,      uint32_t  *puPID,
                                                       uint32_t *puFlags,        void      *pvData,
                                                       uint32_t  cbData,         uint32_t  *pcbSize);
VBGLR3DECL(int)     VbglR3GuestCtrlExecGetHostCmdOutput(uint32_t  u32ClientId,    uint32_t  uNumParms,
                                                        uint32_t *puContext,      uint32_t *puPID,
                                                        uint32_t *puHandle,       uint32_t *puFlags);
VBGLR3DECL(int)     VbglR3GuestCtrlExecReportStatus(uint32_t  u32ClientId,
                                                    uint32_t  u32Context,
                                                    uint32_t  u32PID,
                                                    uint32_t  u32Status,
                                                    uint32_t  u32Flags,
                                                    void     *pvData,
                                                    uint32_t  cbData);
VBGLR3DECL(int)     VbglR3GuestCtrlExecSendOut(uint32_t     u32ClientId,
                                               uint32_t     u32Context,
                                               uint32_t     u32PID,
                                               uint32_t     u32Handle,
                                               uint32_t     u32Flags,
                                               void        *pvData,
                                               uint32_t     cbData);
VBGLR3DECL(int)     VbglR3GuestCtrlExecReportStatusIn(uint32_t     u32ClientId,
                                                      uint32_t     u32Context,
                                                      uint32_t     u32PID,
                                                      uint32_t     u32Status,
                                                      uint32_t     u32Flags,
                                                      uint32_t     cbWritten);
/** @}  */
# endif /* VBOX_WITH_GUEST_CONTROL defined */

/** @name User credentials handling
 * @{ */
VBGLR3DECL(int)     VbglR3CredentialsQueryAvailability(void);
VBGLR3DECL(int)     VbglR3CredentialsRetrieve(char **ppszUser, char **ppszPassword, char **ppszDomain);
VBGLR3DECL(void)    VbglR3CredentialsDestroy(char *pszUser, char *pszPassword, char *pszDomain, uint32_t cPasses);
/** @}  */

/** @name CPU hotplug monitor
 * @{ */
VBGLR3DECL(int)     VbglR3CpuHotPlugInit(void);
VBGLR3DECL(int)     VbglR3CpuHotPlugTerm(void);
VBGLR3DECL(int)     VbglR3CpuHotPlugWaitForEvent(VMMDevCpuEventType *penmEventType, uint32_t *pidCpuCore, uint32_t *pidCpuPackage);
/** @} */

/** @name Page sharing
 * @{ */
VBGLR3DECL(int)     VbglR3RegisterSharedModule(char *pszModuleName, char *pszVersion, RTGCPTR64  GCBaseAddr, uint32_t cbModule, unsigned cRegions, VMMDEVSHAREDREGIONDESC *pRegions);
VBGLR3DECL(int)     VbglR3UnregisterSharedModule(char *pszModuleName, char *pszVersion, RTGCPTR64  GCBaseAddr, uint32_t cbModule);
VBGLR3DECL(int)     VbglR3CheckSharedModules(void);
VBGLR3DECL(bool)    VbglR3PageSharingIsEnabled(void);
VBGLR3DECL(int)     VbglR3PageIsShared(RTGCPTR pPage, bool *pfShared, uint64_t *puPageFlags);
/** @} */

#endif /* IN_RING3 */
/** @} */

RT_C_DECLS_END

/** @} */

#endif

