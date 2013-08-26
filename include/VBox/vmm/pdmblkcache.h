/** @file
 * PDM - Pluggable Device Manager, Block cache.
 */

/*
 * Copyright (C) 2007-2010 Oracle Corporation
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

#ifndef ___VBox_vmm_pdmblkcache_h
#define ___VBox_vmm_pdmblkcache_h

#include <VBox/types.h>
#include <VBox/err.h>
#include <iprt/assert.h>
#include <iprt/sg.h>


RT_C_DECLS_BEGIN

/** @defgroup grp_pdm_blk_cache  The PDM Block Cache API
 * @ingroup grp_pdm
 * @{
 */

/** Pointer to a PDM block cache. */
typedef struct PDMBLKCACHE *PPDMBLKCACHE;
/** Pointer to a PDM block cache pointer. */
typedef PPDMBLKCACHE *PPPDMBLKCACHE;

/** I/O transfer handle. */
typedef struct PDMBLKCACHEIOXFER *PPDMBLKCACHEIOXFER;

/**
 * Block cache I/O request transfer direction.
 */
typedef enum PDMBLKCACHEXFERDIR
{
    /** Read */
    PDMBLKCACHEXFERDIR_READ = 0,
    /** Write */
    PDMBLKCACHEXFERDIR_WRITE,
    /** FLush */
    PDMBLKCACHEXFERDIR_FLUSH
} PDMBLKCACHEXFERDIR;

/**
 * Completion callback for drivers.
 *
 * @param   pDrvIns        The driver instance.
 * @param   pvUser         User argument given during request initiation.
 * @param   rc             The status code of the completed request.
 */
typedef DECLCALLBACK(void) FNPDMBLKCACHEXFERCOMPLETEDRV(PPDMDRVINS pDrvIns, void *pvUser, int rc);
/** Pointer to a FNPDMBLKCACHEXFERCOMPLETEDRV(). */
typedef FNPDMBLKCACHEXFERCOMPLETEDRV *PFNPDMBLKCACHEXFERCOMPLETEDRV;

/**
 * I/O enqueue callback for drivers.
 *
 * @param   pDrvIns        The driver instance.
 * @param   pvUser         User argument given during request initiation.
 * @param   rc             The status code of the completed request.
 */
typedef DECLCALLBACK(int) FNPDMBLKCACHEXFERENQUEUEDRV(PPDMDRVINS pDrvIns,
                                                      PDMBLKCACHEXFERDIR enmXferDir,
                                                      uint64_t off, size_t cbXfer,
                                                      PCRTSGBUF pcSgBuf, PPDMBLKCACHEIOXFER hIoXfer);
/** Pointer to a FNPDMBLKCACHEXFERENQUEUEDRV(). */
typedef FNPDMBLKCACHEXFERENQUEUEDRV *PFNPDMBLKCACHEXFERENQUEUEDRV;

/**
 * Completion callback for devices.
 *
 * @param   pDrvIns        The driver instance.
 * @param   pvUser         User argument given during request initiation.
 * @param   rc             The status code of the completed request.
 */
typedef DECLCALLBACK(void) FNPDMBLKCACHEXFERCOMPLETEDEV(PPDMDEVINS pDevIns, void *pvUser, int rc);
/** Pointer to a FNPDMBLKCACHEXFERCOMPLETEDEV(). */
typedef FNPDMBLKCACHEXFERCOMPLETEDEV *PFNPDMBLKCACHEXFERCOMPLETEDEV;

/**
 * I/O enqueue callback for devices.
 *
 * @param   pDrvIns        The driver instance.
 * @param   pvUser         User argument given during request initiation.
 * @param   rc             The status code of the completed request.
 */
typedef DECLCALLBACK(int) FNPDMBLKCACHEXFERENQUEUEDEV(PPDMDEVINS pDevIns,
                                                      PDMBLKCACHEXFERDIR enmXferDir,
                                                      uint64_t off, size_t cbXfer,
                                                      PCRTSGBUF pcSgBuf, PPDMBLKCACHEIOXFER hIoXfer);
/** Pointer to a FNPDMBLKCACHEXFERENQUEUEDEV(). */
typedef FNPDMBLKCACHEXFERENQUEUEDEV *PFNPDMBLKCACHEXFERENQUEUEDEV;

/**
 * Completion callback for drivers.
 *
 * @param   pDrvIns        The driver instance.
 * @param   pvUser         User argument given during request initiation.
 * @param   rc             The status code of the completed request.
 */
typedef DECLCALLBACK(void) FNPDMBLKCACHEXFERCOMPLETEINT(void *pvUserInt, void *pvUser, int rc);
/** Pointer to a FNPDMBLKCACHEXFERCOMPLETEINT(). */
typedef FNPDMBLKCACHEXFERCOMPLETEINT *PFNPDMBLKCACHEXFERCOMPLETEINT;

/**
 * I/O enqueue callback for drivers.
 *
 * @param   pDrvIns        The driver instance.
 * @param   pvUser         User argument given during request initiation.
 * @param   rc             The status code of the completed request.
 */
typedef DECLCALLBACK(int) FNPDMBLKCACHEXFERENQUEUEINT(void *pvUser,
                                                      PDMBLKCACHEXFERDIR enmXferDir,
                                                      uint64_t off, size_t cbXfer,
                                                      PCRTSGBUF pcSgBuf, PPDMBLKCACHEIOXFER hIoXfer);
/** Pointer to a FNPDMBLKCACHEXFERENQUEUEINT(). */
typedef FNPDMBLKCACHEXFERENQUEUEINT *PFNPDMBLKCACHEXFERENQUEUEINT;

/**
 * Completion callback for USB.
 *
 * @param   pDrvIns        The driver instance.
 * @param   pvUser         User argument given during request initiation.
 * @param   rc             The status code of the completed request.
 */
typedef DECLCALLBACK(void) FNPDMBLKCACHEXFERCOMPLETEUSB(PPDMUSBINS pUsbIns, void *pvUser, int rc);
/** Pointer to a FNPDMBLKCACHEXFERCOMPLETEUSB(). */
typedef FNPDMBLKCACHEXFERCOMPLETEUSB *PFNPDMBLKCACHEXFERCOMPLETEUSB;

/**
 * I/O enqueue callback for drivers.
 *
 * @param   pDrvIns        The driver instance.
 * @param   pvUser         User argument given during request initiation.
 * @param   rc             The status code of the completed request.
 */
typedef DECLCALLBACK(int) FNPDMBLKCACHEXFERENQUEUEUSB(PPDMUSBINS pUsbIns,
                                                      PDMBLKCACHEXFERDIR enmXferDir,
                                                      uint64_t off, size_t cbXfer,
                                                      PCRTSGBUF pcSgBuf, PPDMBLKCACHEIOXFER hIoXfer);
/** Pointer to a FNPDMBLKCACHEXFERENQUEUEUSB(). */
typedef FNPDMBLKCACHEXFERENQUEUEUSB *PFNPDMBLKCACHEXFERENQUEUEUSB;

/**
 * Create a block cache user for a driver instance.
 *
 * @returns VBox status code.
 * @param   pVM             Pointer to the shared VM structure.
 * @param   pDrvIns         The driver instance.
 * @param   ppBlkCache      Where to store the handle to the block cache.
 * @param   pfnXferComplete The I/O transfer complete callback.
 * @param   pfnXferEnqueue  The I/O request enqueue callback.
 * @param   pcszId          Unique ID used to identify the user.
 */
VMMR3DECL(int) PDMR3BlkCacheRetainDriver(PVM pVM, PPDMDRVINS pDrvIns, PPPDMBLKCACHE ppBlkCache,
                                         PFNPDMBLKCACHEXFERCOMPLETEDRV pfnXferComplete,
                                         PFNPDMBLKCACHEXFERENQUEUEDRV pfnXferEnqueue,
                                         const char *pcszId);

/**
 * Create a block cache user for a device instance.
 *
 * @returns VBox status code.
 * @param   pVM             Pointer to the shared VM structure.
 * @param   pDevIns         The device instance.
 * @param   ppBlkCache      Where to store the handle to the block cache.
 * @param   pfnXferComplete The I/O transfer complete callback.
 * @param   pfnXferEnqueue  The I/O request enqueue callback.
 * @param   pcszId          Unique ID used to identify the user.
 */
VMMR3DECL(int) PDMR3BlkCacheRetainDevice(PVM pVM, PPDMDEVINS pDevIns, PPPDMBLKCACHE ppBlkCache,
                                         PFNPDMBLKCACHEXFERCOMPLETEDEV pfnXferComplete,
                                         PFNPDMBLKCACHEXFERENQUEUEDEV pfnXferEnqueue,
                                         const char *pcszId);

/**
 * Create a block cache user for a USB instance.
 *
 * @returns VBox status code.
 * @param   pVM             Pointer to the shared VM structure.
 * @param   pUsbIns         The USB device instance.
 * @param   ppBlkCache      Where to store the handle to the block cache.
 * @param   pfnXferComplete The I/O transfer complete callback.
 * @param   pfnXferEnqueue  The I/O request enqueue callback.
 * @param   pcszId          Unique ID used to identify the user.
 */
VMMR3DECL(int) PDMR3BlkCacheRetainUsb(PVM pVM, PPDMUSBINS pUsbIns, PPPDMBLKCACHE ppBlkCache,
                                      PFNPDMBLKCACHEXFERCOMPLETEUSB pfnXferComplete,
                                      PFNPDMBLKCACHEXFERENQUEUEUSB pfnXferEnqueue,
                                      const char *pcszId);

/**
 * Create a block cache user for internal use by VMM.
 *
 * @returns VBox status code.
 * @param   pVM             Pointer to the shared VM structure.
 * @param   pvUser          Opaque user data.
 * @param   ppBlkCache      Where to store the handle to the block cache.
 * @param   pfnXferComplete The I/O transfer complete callback.
 * @param   pfnXferEnqueue  The I/O request enqueue callback.
 * @param   pcszId          Unique ID used to identify the user.
 */
VMMR3DECL(int) PDMR3BlkCacheRetainInt(PVM pVM, void *pvUser, PPPDMBLKCACHE ppBlkCache,
                                      PFNPDMBLKCACHEXFERCOMPLETEINT pfnXferComplete,
                                      PFNPDMBLKCACHEXFERENQUEUEINT pfnXferEnqueue,
                                      const char *pcszId);

/**
 * Releases a block cache handle.
 *
 * @returns nothing.
 * @param   pBlkCache       Block cache handle.
 */
VMMR3DECL(void) PDMR3BlkCacheRelease(PPDMBLKCACHE pBlkCache);

/**
 * Releases all block cache handles for a device instance.
 *
 * @returns nothing.
 * @param   pVM             Pointer to the shared VM structure.
 * @param   pDevIns         The device instance.
 */
VMMR3DECL(void) PDMR3BlkCacheReleaseDevice(PVM pVM, PPDMDEVINS pDevIns);

/**
 * Releases all block cache handles for a driver instance.
 *
 * @returns nothing.
 * @param   pVM             Pointer to the shared VM structure.
 * @param   pDrvIns         The driver instance.
 */
VMMR3DECL(void) PDMR3BlkCacheReleaseDriver(PVM pVM, PPDMDRVINS pDrvIns);

/**
 * Releases all block cache handles for a USB device instance.
 *
 * @returns nothing.
 * @param   pVM             Pointer to the shared VM structure.
 * @param   pUsbIns         The USB device instance.
 */
VMMR3DECL(void) PDMR3BlkCacheReleaseUsb(PVM pVM, PPDMUSBINS pUsbIns);

/**
 * Creates a read task on the given endpoint.
 *
 * @returns VBox status code.
 * @param   pEndpoint       The file endpoint to read from.
 * @param   off             Where to start reading from.
 * @param   paSegments      Scatter gather list to store the data in.
 * @param   cSegments       Number of segments in the list.
 * @param   cbRead          The overall number of bytes to read.
 * @param   pvUser          Opaque user data returned in the completion callback
 *                          upon completion of the read.
 */
VMMR3DECL(int) PDMR3BlkCacheRead(PPDMBLKCACHE pBlkCache, uint64_t off,
                                 PCRTSGBUF pcSgBuf, size_t cbRead, void *pvUser);

/**
 * Creates a write task on the given endpoint.
 *
 * @returns VBox status code.
 * @param   pEndpoint       The file endpoint to write to.
 * @param   off             Where to start writing at.
 * @param   paSegments      Scatter gather list of the data to write.
 * @param   cSegments       Number of segments in the list.
 * @param   cbWrite         The overall number of bytes to write.
 * @param   pvUser          Opaque user data returned in the completion callback
 *                          upon completion of the task.
 */
VMMR3DECL(int) PDMR3BlkCacheWrite(PPDMBLKCACHE pBlkCache, uint64_t off,
                                  PCRTSGBUF pcSgBuf, size_t cbWrite, void *pvUser);

/**
 * Creates a flush task on the given endpoint.
 *
 * @returns VBox status code.
 * @param   pEndpoint       The file endpoint to flush.
 * @param   pvUser          Opaque user data returned in the completion callback
 *                          upon completion of the task.
 * @param   ppTask          Where to store the task handle on success.
 */
VMMR3DECL(int) PDMR3BlkCacheFlush(PPDMBLKCACHE pBlkCache, void *pvUser);

/**
 * Notify the cache of a complete I/O transfer.
 *
 * @returns nothing.
 * @param   pBlkCache       The cache instance.
 * @param   hIoXfer         The I/O transfer handle which completed.
 * @param   rcIoXfer        The status code of the completed request.
 */
VMMR3DECL(void) PDMR3BlkCacheIoXferComplete(PPDMBLKCACHE pBlkCache, PPDMBLKCACHEIOXFER hIoXfer, int rcIoXfer);

/**
 * Suspends the block cache. The cache waits until all I/O transfers completed
 * and stops to enqueue new requests after the call returned but will not accept
 * reads, write or flushes either.
 *
 * @returns VBox status code.
 * @param   pBlkCache       The cache instance.
 */
VMMR3DECL(int) PDMR3BlkCacheSuspend(PPDMBLKCACHE pBlkCache);

/**
 * Resumes operation of the block cache.
 *
 * @returns VBox status code.
 * @param   pBlkCache       The cache instance.
 */
VMMR3DECL(int) PDMR3BlkCacheResume(PPDMBLKCACHE pBlkCache);

/** @} */

RT_C_DECLS_END

#endif

