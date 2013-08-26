/** @file
 * VirtualBox Video interface.
 */

/*
 * Copyright (C) 2006 Oracle Corporation
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

#ifndef ___VBox_VBoxVideo_h
#define ___VBox_VBoxVideo_h

#include <iprt/cdefs.h>
#include <iprt/types.h>

#include <VBox/VMMDev.h>

/*
 * The last 4096 bytes of the guest VRAM contains the generic info for all
 * DualView chunks: sizes and offsets of chunks. This is filled by miniport.
 *
 * Last 4096 bytes of each chunk contain chunk specific data: framebuffer info,
 * etc. This is used exclusively by the corresponding instance of a display driver.
 *
 * The VRAM layout:
 *     Last 4096 bytes - Adapter information area.
 *     4096 bytes aligned miniport heap (value specified in the config rouded up).
 *     Slack - what left after dividing the VRAM.
 *     4096 bytes aligned framebuffers:
 *       last 4096 bytes of each framebuffer is the display information area.
 *
 * The Virtual Graphics Adapter information in the guest VRAM is stored by the
 * guest video driver using structures prepended by VBOXVIDEOINFOHDR.
 *
 * When the guest driver writes dword 0 to the VBE_DISPI_INDEX_VBOX_VIDEO
 * the host starts to process the info. The first element at the start of
 * the 4096 bytes region should be normally be a LINK that points to
 * actual information chain. That way the guest driver can have some
 * fixed layout of the information memory block and just rewrite
 * the link to point to relevant memory chain.
 *
 * The processing stops at the END element.
 *
 * The host can access the memory only when the port IO is processed.
 * All data that will be needed later must be copied from these 4096 bytes.
 * But other VRAM can be used by host until the mode is disabled.
 *
 * The guest driver writes dword 0xffffffff to the VBE_DISPI_INDEX_VBOX_VIDEO
 * to disable the mode.
 *
 * VBE_DISPI_INDEX_VBOX_VIDEO is used to read the configuration information
 * from the host and issue commands to the host.
 *
 * The guest writes the VBE_DISPI_INDEX_VBOX_VIDEO index register, the the
 * following operations with the VBE data register can be performed:
 *
 * Operation            Result
 * write 16 bit value   NOP
 * read 16 bit value    count of monitors
 * write 32 bit value   sets the vbox command value and the command processed by the host
 * read 32 bit value    result of the last vbox command is returned
 */

#define VBOX_VIDEO_PRIMARY_SCREEN 0
#define VBOX_VIDEO_NO_SCREEN ~0

#define VBOX_VIDEO_MAX_SCREENS 64

/* The size of the information. */
/*
 * The minimum HGSMI heap size is PAGE_SIZE (4096 bytes) and is a restriction of the
 * runtime heapsimple API. Use minimum 2 pages here, because the info area also may
 * contain other data (for example HGSMIHOSTFLAGS structure).
 */
#ifndef VBOX_XPDM_MINIPORT
# define VBVA_ADAPTER_INFORMATION_SIZE (64*_1K)
#else
#define VBVA_ADAPTER_INFORMATION_SIZE  (16*_1K)
#define VBVA_DISPLAY_INFORMATION_SIZE  (64*_1K)
#endif
#define VBVA_MIN_BUFFER_SIZE           (64*_1K)


/* The value for port IO to let the adapter to interpret the adapter memory. */
#define VBOX_VIDEO_DISABLE_ADAPTER_MEMORY        0xFFFFFFFF

/* The value for port IO to let the adapter to interpret the adapter memory. */
#define VBOX_VIDEO_INTERPRET_ADAPTER_MEMORY      0x00000000

/* The value for port IO to let the adapter to interpret the display memory.
 * The display number is encoded in low 16 bits.
 */
#define VBOX_VIDEO_INTERPRET_DISPLAY_MEMORY_BASE 0x00010000


/* The end of the information. */
#define VBOX_VIDEO_INFO_TYPE_END          0
/* Instructs the host to fetch the next VBOXVIDEOINFOHDR at the given offset of VRAM. */
#define VBOX_VIDEO_INFO_TYPE_LINK         1
/* Information about a display memory position. */
#define VBOX_VIDEO_INFO_TYPE_DISPLAY      2
/* Information about a screen. */
#define VBOX_VIDEO_INFO_TYPE_SCREEN       3
/* Information about host notifications for the driver. */
#define VBOX_VIDEO_INFO_TYPE_HOST_EVENTS  4
/* Information about non-volatile guest VRAM heap. */
#define VBOX_VIDEO_INFO_TYPE_NV_HEAP      5
/* VBVA enable/disable. */
#define VBOX_VIDEO_INFO_TYPE_VBVA_STATUS  6
/* VBVA flush. */
#define VBOX_VIDEO_INFO_TYPE_VBVA_FLUSH   7
/* Query configuration value. */
#define VBOX_VIDEO_INFO_TYPE_QUERY_CONF32 8


#pragma pack(1)
typedef struct VBOXVIDEOINFOHDR
{
    uint8_t u8Type;
    uint8_t u8Reserved;
    uint16_t u16Length;
} VBOXVIDEOINFOHDR;


typedef struct VBOXVIDEOINFOLINK
{
    /* Relative offset in VRAM */
    int32_t i32Offset;
} VBOXVIDEOINFOLINK;


/* Resides in adapter info memory. Describes a display VRAM chunk. */
typedef struct VBOXVIDEOINFODISPLAY
{
    /* Index of the framebuffer assigned by guest. */
    uint32_t u32Index;

    /* Absolute offset in VRAM of the framebuffer to be displayed on the monitor. */
    uint32_t u32Offset;

    /* The size of the memory that can be used for the screen. */
    uint32_t u32FramebufferSize;

    /* The size of the memory that is used for the Display information.
     * The information is at u32Offset + u32FramebufferSize
     */
    uint32_t u32InformationSize;

} VBOXVIDEOINFODISPLAY;


/* Resides in display info area, describes the current video mode. */
#define VBOX_VIDEO_INFO_SCREEN_F_NONE   0x00
#define VBOX_VIDEO_INFO_SCREEN_F_ACTIVE 0x01

typedef struct VBOXVIDEOINFOSCREEN
{
    /* Physical X origin relative to the primary screen. */
    int32_t xOrigin;

    /* Physical Y origin relative to the primary screen. */
    int32_t yOrigin;

    /* The scan line size in bytes. */
    uint32_t u32LineSize;

    /* Width of the screen. */
    uint16_t u16Width;

    /* Height of the screen. */
    uint16_t u16Height;

    /* Color depth. */
    uint8_t bitsPerPixel;

    /* VBOX_VIDEO_INFO_SCREEN_F_* */
    uint8_t u8Flags;
} VBOXVIDEOINFOSCREEN;

/* The guest initializes the structure to 0. The positions of the structure in the
 * display info area must not be changed, host will update the structure. Guest checks
 * the events and modifies the structure as a response to host.
 */
#define VBOX_VIDEO_INFO_HOST_EVENTS_F_NONE        0x00000000
#define VBOX_VIDEO_INFO_HOST_EVENTS_F_VRDP_RESET  0x00000080

typedef struct VBOXVIDEOINFOHOSTEVENTS
{
    /* Host events. */
    uint32_t fu32Events;
} VBOXVIDEOINFOHOSTEVENTS;

/* Resides in adapter info memory. Describes the non-volatile VRAM heap. */
typedef struct VBOXVIDEOINFONVHEAP
{
    /* Absolute offset in VRAM of the start of the heap. */
    uint32_t u32HeapOffset;

    /* The size of the heap. */
    uint32_t u32HeapSize;

} VBOXVIDEOINFONVHEAP;

/* Display information area. */
typedef struct VBOXVIDEOINFOVBVASTATUS
{
    /* Absolute offset in VRAM of the start of the VBVA QUEUE. 0 to disable VBVA. */
    uint32_t u32QueueOffset;

    /* The size of the VBVA QUEUE. 0 to disable VBVA. */
    uint32_t u32QueueSize;

} VBOXVIDEOINFOVBVASTATUS;

typedef struct VBOXVIDEOINFOVBVAFLUSH
{
    uint32_t u32DataStart;

    uint32_t u32DataEnd;

} VBOXVIDEOINFOVBVAFLUSH;

#define VBOX_VIDEO_QCI32_MONITOR_COUNT       0
#define VBOX_VIDEO_QCI32_OFFSCREEN_HEAP_SIZE 1

typedef struct VBOXVIDEOINFOQUERYCONF32
{
    uint32_t u32Index;

    uint32_t u32Value;

} VBOXVIDEOINFOQUERYCONF32;
#pragma pack()

#ifdef VBOX_WITH_VIDEOHWACCEL
#pragma pack(1)

#define VBOXVHWA_VERSION_MAJ 0
#define VBOXVHWA_VERSION_MIN 0
#define VBOXVHWA_VERSION_BLD 6
#define VBOXVHWA_VERSION_RSV 0

typedef enum
{
    VBOXVHWACMD_TYPE_SURF_CANCREATE = 1,
    VBOXVHWACMD_TYPE_SURF_CREATE,
    VBOXVHWACMD_TYPE_SURF_DESTROY,
    VBOXVHWACMD_TYPE_SURF_LOCK,
    VBOXVHWACMD_TYPE_SURF_UNLOCK,
    VBOXVHWACMD_TYPE_SURF_BLT,
    VBOXVHWACMD_TYPE_SURF_FLIP,
    VBOXVHWACMD_TYPE_SURF_OVERLAY_UPDATE,
    VBOXVHWACMD_TYPE_SURF_OVERLAY_SETPOSITION,
    VBOXVHWACMD_TYPE_SURF_COLORKEY_SET,
    VBOXVHWACMD_TYPE_QUERY_INFO1,
    VBOXVHWACMD_TYPE_QUERY_INFO2,
    VBOXVHWACMD_TYPE_ENABLE,
    VBOXVHWACMD_TYPE_DISABLE,
    VBOXVHWACMD_TYPE_HH_CONSTRUCT,
    VBOXVHWACMD_TYPE_HH_RESET
#ifdef VBOX_WITH_WDDM
    , VBOXVHWACMD_TYPE_SURF_GETINFO
    , VBOXVHWACMD_TYPE_SURF_COLORFILL
#endif
    , VBOXVHWACMD_TYPE_HH_DISABLE
    , VBOXVHWACMD_TYPE_HH_ENABLE
    , VBOXVHWACMD_TYPE_HH_SAVESTATE_SAVEBEGIN
    , VBOXVHWACMD_TYPE_HH_SAVESTATE_SAVEEND
    , VBOXVHWACMD_TYPE_HH_SAVESTATE_SAVEPERFORM
    , VBOXVHWACMD_TYPE_HH_SAVESTATE_LOADPERFORM
} VBOXVHWACMD_TYPE;

/* the command processing was asynch, set by the host to indicate asynch command completion
 * must not be cleared once set, the command completion is performed by issuing a host->guest completion command
 * while keeping this flag unchanged */
#define VBOXVHWACMD_FLAG_HG_ASYNCH               0x00010000
/* asynch completion is performed by issuing the event */
#define VBOXVHWACMD_FLAG_GH_ASYNCH_EVENT         0x00000001
/* issue interrupt on asynch completion */
#define VBOXVHWACMD_FLAG_GH_ASYNCH_IRQ           0x00000002
/* guest does not do any op on completion of this command, the host may copy the command and indicate that it does not need the command anymore
 * by setting the VBOXVHWACMD_FLAG_HG_ASYNCH_RETURNED flag */
#define VBOXVHWACMD_FLAG_GH_ASYNCH_NOCOMPLETION  0x00000004
/* the host has copied the VBOXVHWACMD_FLAG_GH_ASYNCH_NOCOMPLETION command and returned it to the guest */
#define VBOXVHWACMD_FLAG_HG_ASYNCH_RETURNED      0x00020000
/* this is the host->host cmd, i.e. a configuration command posted by the host to the framebuffer */
#define VBOXVHWACMD_FLAG_HH_CMD                  0x10000000

typedef struct VBOXVHWACMD
{
    VBOXVHWACMD_TYPE enmCmd; /* command type */
    volatile int32_t rc; /* command result */
    int32_t iDisplay; /* display index */
    volatile int32_t Flags; /* ored VBOXVHWACMD_FLAG_xxx values */
    uint64_t GuestVBVAReserved1; /* field internally used by the guest VBVA cmd handling, must NOT be modified by clients */
    uint64_t GuestVBVAReserved2; /* field internally used by the guest VBVA cmd handling, must NOT be modified by clients */
    volatile uint32_t cRefs;
    int32_t Reserved;
    union
    {
        struct VBOXVHWACMD *pNext;
        uint32_t             offNext;
        uint64_t Data; /* the body is 64-bit aligned */
    } u;
    char body[1];
} VBOXVHWACMD;

#define VBOXVHWACMD_HEADSIZE() (RT_OFFSETOF(VBOXVHWACMD, body))
#define VBOXVHWACMD_SIZE_FROMBODYSIZE(_s) (VBOXVHWACMD_HEADSIZE() + (_s))
#define VBOXVHWACMD_SIZE(_tCmd) (VBOXVHWACMD_SIZE_FROMBODYSIZE(sizeof(_tCmd)))
typedef unsigned int VBOXVHWACMD_LENGTH;
typedef uint64_t VBOXVHWA_SURFHANDLE;
#define VBOXVHWA_SURFHANDLE_INVALID 0ULL
#define VBOXVHWACMD_BODY(_p, _t) ((_t*)(_p)->body)
#define VBOXVHWACMD_HEAD(_pb) ((VBOXVHWACMD*)((uint8_t *)(_pb) - RT_OFFSETOF(VBOXVHWACMD, body)))

typedef struct VBOXVHWA_RECTL
{
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
} VBOXVHWA_RECTL;

typedef struct VBOXVHWA_COLORKEY
{
    uint32_t low;
    uint32_t high;
} VBOXVHWA_COLORKEY;

typedef struct VBOXVHWA_PIXELFORMAT
{
    uint32_t flags;
    uint32_t fourCC;
    union
    {
        uint32_t rgbBitCount;
        uint32_t yuvBitCount;
    } c;

    union
    {
        uint32_t rgbRBitMask;
        uint32_t yuvYBitMask;
    } m1;

    union
    {
        uint32_t rgbGBitMask;
        uint32_t yuvUBitMask;
    } m2;

    union
    {
        uint32_t rgbBBitMask;
        uint32_t yuvVBitMask;
    } m3;

    union
    {
        uint32_t rgbABitMask;
    } m4;

    uint32_t Reserved;
} VBOXVHWA_PIXELFORMAT;

typedef struct VBOXVHWA_SURFACEDESC
{
    uint32_t flags;
    uint32_t height;
    uint32_t width;
    uint32_t pitch;
    uint32_t sizeX;
    uint32_t sizeY;
    uint32_t cBackBuffers;
    uint32_t Reserved;
    VBOXVHWA_COLORKEY DstOverlayCK;
    VBOXVHWA_COLORKEY DstBltCK;
    VBOXVHWA_COLORKEY SrcOverlayCK;
    VBOXVHWA_COLORKEY SrcBltCK;
    VBOXVHWA_PIXELFORMAT PixelFormat;
    uint32_t surfCaps;
    uint32_t Reserved2;
    VBOXVHWA_SURFHANDLE hSurf;
    uint64_t offSurface;
} VBOXVHWA_SURFACEDESC;

typedef struct VBOXVHWA_BLTFX
{
    uint32_t flags;
    uint32_t rop;
    uint32_t rotationOp;
    uint32_t rotation;
    uint32_t fillColor;
    uint32_t Reserved;
    VBOXVHWA_COLORKEY DstCK;
    VBOXVHWA_COLORKEY SrcCK;
} VBOXVHWA_BLTFX;

typedef struct VBOXVHWA_OVERLAYFX
{
    uint32_t flags;
    uint32_t Reserved1;
    uint32_t fxFlags;
    uint32_t Reserved2;
    VBOXVHWA_COLORKEY DstCK;
    VBOXVHWA_COLORKEY SrcCK;
} VBOXVHWA_OVERLAYFX;

#define VBOXVHWA_CAPS_BLT               0x00000040
#define VBOXVHWA_CAPS_BLTCOLORFILL      0x04000000
#define VBOXVHWA_CAPS_BLTFOURCC         0x00000100
#define VBOXVHWA_CAPS_BLTSTRETCH        0x00000200
#define VBOXVHWA_CAPS_BLTQUEUE          0x00000080

#define VBOXVHWA_CAPS_OVERLAY           0x00000800
#define VBOXVHWA_CAPS_OVERLAYFOURCC     0x00002000
#define VBOXVHWA_CAPS_OVERLAYSTRETCH    0x00004000
#define VBOXVHWA_CAPS_OVERLAYCANTCLIP   0x00001000

#define VBOXVHWA_CAPS_COLORKEY          0x00400000
#define VBOXVHWA_CAPS_COLORKEYHWASSIST  0x01000000

#define VBOXVHWA_SCAPS_BACKBUFFER       0x00000004
#define VBOXVHWA_SCAPS_COMPLEX          0x00000008
#define VBOXVHWA_SCAPS_FLIP             0x00000010
#define VBOXVHWA_SCAPS_FRONTBUFFER      0x00000020
#define VBOXVHWA_SCAPS_OFFSCREENPLAIN   0x00000040
#define VBOXVHWA_SCAPS_OVERLAY          0x00000080
#define VBOXVHWA_SCAPS_PRIMARYSURFACE   0x00000200
#define VBOXVHWA_SCAPS_SYSTEMMEMORY     0x00000800
#define VBOXVHWA_SCAPS_VIDEOMEMORY      0x00004000
#define VBOXVHWA_SCAPS_VISIBLE          0x00008000
#define VBOXVHWA_SCAPS_LOCALVIDMEM      0x10000000

#define VBOXVHWA_PF_PALETTEINDEXED8     0x00000020
#define VBOXVHWA_PF_RGB                 0x00000040
#define VBOXVHWA_PF_RGBTOYUV            0x00000100
#define VBOXVHWA_PF_YUV                 0x00000200
#define VBOXVHWA_PF_FOURCC              0x00000004

#define VBOXVHWA_LOCK_DISCARDCONTENTS   0x00002000

#define VBOXVHWA_CFG_ENABLED            0x00000001

#define VBOXVHWA_SD_BACKBUFFERCOUNT     0x00000020
#define VBOXVHWA_SD_CAPS                0x00000001
#define VBOXVHWA_SD_CKDESTBLT           0x00004000
#define VBOXVHWA_SD_CKDESTOVERLAY       0x00002000
#define VBOXVHWA_SD_CKSRCBLT            0x00010000
#define VBOXVHWA_SD_CKSRCOVERLAY        0x00008000
#define VBOXVHWA_SD_HEIGHT              0x00000002
#define VBOXVHWA_SD_PITCH               0x00000008
#define VBOXVHWA_SD_PIXELFORMAT         0x00001000
/*#define VBOXVHWA_SD_REFRESHRATE       0x00040000*/
#define VBOXVHWA_SD_WIDTH               0x00000004

#define VBOXVHWA_CKEYCAPS_DESTBLT                  0x00000001
#define VBOXVHWA_CKEYCAPS_DESTBLTCLRSPACE          0x00000002
#define VBOXVHWA_CKEYCAPS_DESTBLTCLRSPACEYUV       0x00000004
#define VBOXVHWA_CKEYCAPS_DESTBLTYUV               0x00000008
#define VBOXVHWA_CKEYCAPS_DESTOVERLAY              0x00000010
#define VBOXVHWA_CKEYCAPS_DESTOVERLAYCLRSPACE      0x00000020
#define VBOXVHWA_CKEYCAPS_DESTOVERLAYCLRSPACEYUV   0x00000040
#define VBOXVHWA_CKEYCAPS_DESTOVERLAYONEACTIVE     0x00000080
#define VBOXVHWA_CKEYCAPS_DESTOVERLAYYUV           0x00000100
#define VBOXVHWA_CKEYCAPS_SRCBLT                   0x00000200
#define VBOXVHWA_CKEYCAPS_SRCBLTCLRSPACE           0x00000400
#define VBOXVHWA_CKEYCAPS_SRCBLTCLRSPACEYUV        0x00000800
#define VBOXVHWA_CKEYCAPS_SRCBLTYUV                0x00001000
#define VBOXVHWA_CKEYCAPS_SRCOVERLAY               0x00002000
#define VBOXVHWA_CKEYCAPS_SRCOVERLAYCLRSPACE       0x00004000
#define VBOXVHWA_CKEYCAPS_SRCOVERLAYCLRSPACEYUV    0x00008000
#define VBOXVHWA_CKEYCAPS_SRCOVERLAYONEACTIVE      0x00010000
#define VBOXVHWA_CKEYCAPS_SRCOVERLAYYUV            0x00020000
#define VBOXVHWA_CKEYCAPS_NOCOSTOVERLAY            0x00040000

#define VBOXVHWA_BLT_COLORFILL                      0x00000400
#define VBOXVHWA_BLT_DDFX                           0x00000800
#define VBOXVHWA_BLT_EXTENDED_FLAGS                 0x40000000
#define VBOXVHWA_BLT_EXTENDED_LINEAR_CONTENT        0x00000004
#define VBOXVHWA_BLT_EXTENDED_PRESENTATION_STRETCHFACTOR 0x00000010
#define VBOXVHWA_BLT_KEYDESTOVERRIDE                0x00004000
#define VBOXVHWA_BLT_KEYSRCOVERRIDE                 0x00010000
#define VBOXVHWA_BLT_LAST_PRESENTATION              0x20000000
#define VBOXVHWA_BLT_PRESENTATION                   0x10000000
#define VBOXVHWA_BLT_ROP                            0x00020000


#define VBOXVHWA_OVER_DDFX                          0x00080000
#define VBOXVHWA_OVER_HIDE                          0x00000200
#define VBOXVHWA_OVER_KEYDEST                       0x00000400
#define VBOXVHWA_OVER_KEYDESTOVERRIDE               0x00000800
#define VBOXVHWA_OVER_KEYSRC                        0x00001000
#define VBOXVHWA_OVER_KEYSRCOVERRIDE                0x00002000
#define VBOXVHWA_OVER_SHOW                          0x00004000

#define VBOXVHWA_CKEY_COLORSPACE                    0x00000001
#define VBOXVHWA_CKEY_DESTBLT                       0x00000002
#define VBOXVHWA_CKEY_DESTOVERLAY                   0x00000004
#define VBOXVHWA_CKEY_SRCBLT                        0x00000008
#define VBOXVHWA_CKEY_SRCOVERLAY                    0x00000010

#define VBOXVHWA_BLT_ARITHSTRETCHY                  0x00000001
#define VBOXVHWA_BLT_MIRRORLEFTRIGHT                0x00000002
#define VBOXVHWA_BLT_MIRRORUPDOWN                   0x00000004

#define VBOXVHWA_OVERFX_ARITHSTRETCHY               0x00000001
#define VBOXVHWA_OVERFX_MIRRORLEFTRIGHT             0x00000002
#define VBOXVHWA_OVERFX_MIRRORUPDOWN                0x00000004

#define VBOXVHWA_CAPS2_CANRENDERWINDOWED            0x00080000
#define VBOXVHWA_CAPS2_WIDESURFACES                 0x00001000
#define VBOXVHWA_CAPS2_COPYFOURCC                   0x00008000
/*#define VBOXVHWA_CAPS2_FLIPINTERVAL                 0x00200000*/
/*#define VBOXVHWA_CAPS2_FLIPNOVSYNC                  0x00400000*/


#define VBOXVHWA_OFFSET64_VOID        (UINT64_MAX)

typedef struct VBOXVHWA_VERSION
{
    uint32_t maj;
    uint32_t min;
    uint32_t bld;
    uint32_t reserved;
} VBOXVHWA_VERSION;

#define VBOXVHWA_VERSION_INIT(_pv) do { \
        (_pv)->maj = VBOXVHWA_VERSION_MAJ; \
        (_pv)->min = VBOXVHWA_VERSION_MIN; \
        (_pv)->bld = VBOXVHWA_VERSION_BLD; \
        (_pv)->reserved = VBOXVHWA_VERSION_RSV; \
        } while(0)

typedef struct VBOXVHWACMD_QUERYINFO1
{
    union
    {
        struct
        {
            VBOXVHWA_VERSION guestVersion;
        } in;

        struct
        {
            uint32_t cfgFlags;
            uint32_t caps;

            uint32_t caps2;
            uint32_t colorKeyCaps;

            uint32_t stretchCaps;
            uint32_t surfaceCaps;

            uint32_t numOverlays;
            uint32_t curOverlays;

            uint32_t numFourCC;
            uint32_t reserved;
        } out;
    } u;
} VBOXVHWACMD_QUERYINFO1;

typedef struct VBOXVHWACMD_QUERYINFO2
{
    uint32_t numFourCC;
    uint32_t FourCC[1];
} VBOXVHWACMD_QUERYINFO2;

#define VBOXVHWAINFO2_SIZE(_cFourCC) RT_OFFSETOF(VBOXVHWACMD_QUERYINFO2, FourCC[_cFourCC])

typedef struct VBOXVHWACMD_SURF_CANCREATE
{
    VBOXVHWA_SURFACEDESC SurfInfo;
    union
    {
        struct
        {
            uint32_t bIsDifferentPixelFormat;
            uint32_t Reserved;
        } in;

        struct
        {
            int32_t ErrInfo;
        } out;
    } u;
} VBOXVHWACMD_SURF_CANCREATE;

typedef struct VBOXVHWACMD_SURF_CREATE
{
    VBOXVHWA_SURFACEDESC SurfInfo;
} VBOXVHWACMD_SURF_CREATE;

#ifdef VBOX_WITH_WDDM
typedef struct VBOXVHWACMD_SURF_GETINFO
{
    VBOXVHWA_SURFACEDESC SurfInfo;
} VBOXVHWACMD_SURF_GETINFO;
#endif

typedef struct VBOXVHWACMD_SURF_DESTROY
{
    union
    {
        struct
        {
            VBOXVHWA_SURFHANDLE hSurf;
        } in;
    } u;
} VBOXVHWACMD_SURF_DESTROY;

typedef struct VBOXVHWACMD_SURF_LOCK
{
    union
    {
        struct
        {
            VBOXVHWA_SURFHANDLE hSurf;
            uint64_t offSurface;
            uint32_t flags;
            uint32_t rectValid;
            VBOXVHWA_RECTL rect;
        } in;
    } u;
} VBOXVHWACMD_SURF_LOCK;

typedef struct VBOXVHWACMD_SURF_UNLOCK
{
    union
    {
        struct
        {
            VBOXVHWA_SURFHANDLE hSurf;
            uint32_t xUpdatedMemValid;
            uint32_t reserved;
            VBOXVHWA_RECTL xUpdatedMemRect;
        } in;
    } u;
} VBOXVHWACMD_SURF_UNLOCK;

typedef struct VBOXVHWACMD_SURF_BLT
{
    uint64_t DstGuestSurfInfo;
    uint64_t SrcGuestSurfInfo;
    union
    {
        struct
        {
            VBOXVHWA_SURFHANDLE hDstSurf;
            uint64_t offDstSurface;
            VBOXVHWA_RECTL dstRect;
            VBOXVHWA_SURFHANDLE hSrcSurf;
            uint64_t offSrcSurface;
            VBOXVHWA_RECTL srcRect;
            uint32_t flags;
            uint32_t xUpdatedSrcMemValid;
            VBOXVHWA_BLTFX desc;
            VBOXVHWA_RECTL xUpdatedSrcMemRect;
        } in;
    } u;
} VBOXVHWACMD_SURF_BLT;

#ifdef VBOX_WITH_WDDM
typedef struct VBOXVHWACMD_SURF_COLORFILL
{
    union
    {
        struct
        {
            VBOXVHWA_SURFHANDLE hSurf;
            uint64_t offSurface;
            uint32_t u32Reserved;
            uint32_t cRects;
            VBOXVHWA_RECTL aRects[1];
        } in;
    } u;
} VBOXVHWACMD_SURF_COLORFILL;
#endif

typedef struct VBOXVHWACMD_SURF_FLIP
{
    uint64_t TargGuestSurfInfo;
    uint64_t CurrGuestSurfInfo;
    union
    {
        struct
        {
            VBOXVHWA_SURFHANDLE hTargSurf;
            uint64_t offTargSurface;
            VBOXVHWA_SURFHANDLE hCurrSurf;
            uint64_t offCurrSurface;
            uint32_t flags;
            uint32_t xUpdatedTargMemValid;
            VBOXVHWA_RECTL xUpdatedTargMemRect;
        } in;
    } u;
} VBOXVHWACMD_SURF_FLIP;

typedef struct VBOXVHWACMD_SURF_COLORKEY_SET
{
    union
    {
        struct
        {
            VBOXVHWA_SURFHANDLE hSurf;
            uint64_t offSurface;
            VBOXVHWA_COLORKEY CKey;
            uint32_t flags;
            uint32_t reserved;
        } in;
    } u;
} VBOXVHWACMD_SURF_COLORKEY_SET;

#define VBOXVHWACMD_SURF_OVERLAY_UPDATE_F_SRCMEMRECT 0x00000001
#define VBOXVHWACMD_SURF_OVERLAY_UPDATE_F_DSTMEMRECT 0x00000002

typedef struct VBOXVHWACMD_SURF_OVERLAY_UPDATE
{
    union
    {
        struct
        {
            VBOXVHWA_SURFHANDLE hDstSurf;
            uint64_t offDstSurface;
            VBOXVHWA_RECTL dstRect;
            VBOXVHWA_SURFHANDLE hSrcSurf;
            uint64_t offSrcSurface;
            VBOXVHWA_RECTL srcRect;
            uint32_t flags;
            uint32_t xFlags;
            VBOXVHWA_OVERLAYFX desc;
            VBOXVHWA_RECTL xUpdatedSrcMemRect;
            VBOXVHWA_RECTL xUpdatedDstMemRect;
        } in;
    } u;
}VBOXVHWACMD_SURF_OVERLAY_UPDATE;

typedef struct VBOXVHWACMD_SURF_OVERLAY_SETPOSITION
{
    union
    {
        struct
        {
            VBOXVHWA_SURFHANDLE hDstSurf;
            uint64_t offDstSurface;
            VBOXVHWA_SURFHANDLE hSrcSurf;
            uint64_t offSrcSurface;
            uint32_t xPos;
            uint32_t yPos;
            uint32_t flags;
            uint32_t reserved;
        } in;
    } u;
} VBOXVHWACMD_SURF_OVERLAY_SETPOSITION;

typedef struct VBOXVHWACMD_HH_CONSTRUCT
{
    void    *pVM;
    /* VRAM info for the backend to be able to properly translate VRAM offsets */
    void    *pvVRAM;
    uint32_t cbVRAM;
} VBOXVHWACMD_HH_CONSTRUCT;

typedef struct VBOXVHWACMD_HH_SAVESTATE_SAVEPERFORM
{
    struct SSMHANDLE * pSSM;
} VBOXVHWACMD_HH_SAVESTATE_SAVEPERFORM;

typedef struct VBOXVHWACMD_HH_SAVESTATE_LOADPERFORM
{
    struct SSMHANDLE * pSSM;
} VBOXVHWACMD_HH_SAVESTATE_LOADPERFORM;

typedef DECLCALLBACK(void) FNVBOXVHWA_HH_CALLBACK(void*);
typedef FNVBOXVHWA_HH_CALLBACK *PFNVBOXVHWA_HH_CALLBACK;

#define VBOXVHWA_HH_CALLBACK_SET(_pCmd, _pfn, _parg) \
    do { \
        (_pCmd)->GuestVBVAReserved1 = (uint64_t)(uintptr_t)(_pfn); \
        (_pCmd)->GuestVBVAReserved2 = (uint64_t)(uintptr_t)(_parg); \
    }while(0)

#define VBOXVHWA_HH_CALLBACK_GET(_pCmd) ((PFNVBOXVHWA_HH_CALLBACK)(_pCmd)->GuestVBVAReserved1)
#define VBOXVHWA_HH_CALLBACK_GET_ARG(_pCmd) ((void*)(_pCmd)->GuestVBVAReserved2)

#pragma pack()
#endif /* #ifdef VBOX_WITH_VIDEOHWACCEL */

/* All structures are without alignment. */
#pragma pack(1)

typedef struct VBVAHOSTFLAGS
{
    uint32_t u32HostEvents;
    uint32_t u32SupportedOrders;
} VBVAHOSTFLAGS;

typedef struct VBVABUFFER
{
    VBVAHOSTFLAGS hostFlags;

    /* The offset where the data start in the buffer. */
    uint32_t off32Data;
    /* The offset where next data must be placed in the buffer. */
    uint32_t off32Free;

    /* The queue of record descriptions. */
    VBVARECORD aRecords[VBVA_MAX_RECORDS];
    uint32_t indexRecordFirst;
    uint32_t indexRecordFree;

    /* Space to leave free in the buffer when large partial records are transferred. */
    uint32_t cbPartialWriteThreshold;

    uint32_t cbData;
    uint8_t  au8Data[1]; /* variable size for the rest of the VBVABUFFER area in VRAM. */
} VBVABUFFER;

/* guest->host commands */
#define VBVA_QUERY_CONF32 1
#define VBVA_SET_CONF32   2
#define VBVA_INFO_VIEW    3
#define VBVA_INFO_HEAP    4
#define VBVA_FLUSH        5
#define VBVA_INFO_SCREEN  6
#define VBVA_ENABLE       7
#define VBVA_MOUSE_POINTER_SHAPE 8
#ifdef VBOX_WITH_VIDEOHWACCEL
# define VBVA_VHWA_CMD    9
#endif /* # ifdef VBOX_WITH_VIDEOHWACCEL */
#ifdef VBOX_WITH_VDMA
# define VBVA_VDMA_CTL   10 /* setup G<->H DMA channel info */
# define VBVA_VDMA_CMD    11 /* G->H DMA command             */
#endif
#define VBVA_INFO_CAPS   12 /* informs host about HGSMI caps. see _VBVACAPS below */

/* host->guest commands */
#define VBVAHG_EVENT              1
#define VBVAHG_DISPLAY_CUSTOM     2
#ifdef VBOX_WITH_VDMA
#define VBVAHG_SHGSMI_COMPLETION  3
#endif

#ifdef VBOX_WITH_VIDEOHWACCEL
#define VBVAHG_DCUSTOM_VHWA_CMDCOMPLETE 1
#pragma pack(1)
typedef struct VBVAHOSTCMDVHWACMDCOMPLETE
{
    uint32_t offCmd;
}VBVAHOSTCMDVHWACMDCOMPLETE;
#pragma pack()
#endif /* # ifdef VBOX_WITH_VIDEOHWACCEL */

#pragma pack(1)
typedef enum
{
    VBVAHOSTCMD_OP_EVENT = 1,
    VBVAHOSTCMD_OP_CUSTOM
}VBVAHOSTCMD_OP_TYPE;

typedef struct VBVAHOSTCMDEVENT
{
    uint64_t pEvent;
}VBVAHOSTCMDEVENT;


typedef struct VBVAHOSTCMD
{
    /* destination ID if >=0 specifies display index, otherwize the command is directed to the miniport */
    int32_t iDstID;
    int32_t customOpCode;
    union
    {
        struct VBVAHOSTCMD *pNext;
        uint32_t             offNext;
        uint64_t Data; /* the body is 64-bit aligned */
    } u;
    char body[1];
}VBVAHOSTCMD;

#define VBVAHOSTCMD_SIZE(_size) (sizeof(VBVAHOSTCMD) + (_size))
#define VBVAHOSTCMD_BODY(_pCmd, _tBody) ((_tBody*)(_pCmd)->body)
#define VBVAHOSTCMD_HDR(_pBody) ((VBVAHOSTCMD*)(((uint8_t*)_pBody) - RT_OFFSETOF(VBVAHOSTCMD, body)))
#define VBVAHOSTCMD_HDRSIZE (RT_OFFSETOF(VBVAHOSTCMD, body))

#pragma pack()

/* VBVACONF32::u32Index */
#define VBOX_VBVA_CONF32_MONITOR_COUNT  0
#define VBOX_VBVA_CONF32_HOST_HEAP_SIZE 1

typedef struct VBVACONF32
{
    uint32_t u32Index;
    uint32_t u32Value;
} VBVACONF32;

typedef struct VBVAINFOVIEW
{
    /* Index of the screen, assigned by the guest. */
    uint32_t u32ViewIndex;

    /* The screen offset in VRAM, the framebuffer starts here. */
    uint32_t u32ViewOffset;

    /* The size of the VRAM memory that can be used for the view. */
    uint32_t u32ViewSize;

    /* The recommended maximum size of the VRAM memory for the screen. */
    uint32_t u32MaxScreenSize;
} VBVAINFOVIEW;

typedef struct VBVAINFOHEAP
{
    /* Absolute offset in VRAM of the start of the heap. */
    uint32_t u32HeapOffset;

    /* The size of the heap. */
    uint32_t u32HeapSize;

} VBVAINFOHEAP;

typedef struct VBVAFLUSH
{
    uint32_t u32Reserved;

} VBVAFLUSH;

/* VBVAINFOSCREEN::u8Flags */
#define VBVA_SCREEN_F_NONE     0x0000
#define VBVA_SCREEN_F_ACTIVE   0x0001
/** The virtual monitor has been disabled by the guest and should be blacked
 * out by the host and ignored for purposes of pointer position calculation. */
#define VBVA_SCREEN_F_DISABLED 0x0002

typedef struct VBVAINFOSCREEN
{
    /* Which view contains the screen. */
    uint32_t u32ViewIndex;

    /* Physical X origin relative to the primary screen. */
    int32_t i32OriginX;

    /* Physical Y origin relative to the primary screen. */
    int32_t i32OriginY;

    /* Offset of visible framebuffer relative to the framebuffer start. */
    uint32_t u32StartOffset;

    /* The scan line size in bytes. */
    uint32_t u32LineSize;

    /* Width of the screen. */
    uint32_t u32Width;

    /* Height of the screen. */
    uint32_t u32Height;

    /* Color depth. */
    uint16_t u16BitsPerPixel;

    /* VBVA_SCREEN_F_* */
    uint16_t u16Flags;
} VBVAINFOSCREEN;


/* VBVAENABLE::u32Flags */
#define VBVA_F_NONE    0x00000000
#define VBVA_F_ENABLE  0x00000001
#define VBVA_F_DISABLE 0x00000002
/* extended VBVA to be used with WDDM */
#define VBVA_F_EXTENDED 0x00000004
/* vbva offset is absolute VRAM offset */
#define VBVA_F_ABSOFFSET 0x00000008

typedef struct VBVAENABLE
{
    uint32_t u32Flags;
    uint32_t u32Offset;
    int32_t  i32Result;
} VBVAENABLE;

typedef struct VBVAENABLE_EX
{
    VBVAENABLE Base;
    uint32_t u32ScreenId;
} VBVAENABLE_EX;


typedef struct VBVAMOUSEPOINTERSHAPE
{
    /* The host result. */
    int32_t i32Result;

    /* VBOX_MOUSE_POINTER_* bit flags. */
    uint32_t fu32Flags;

    /* X coordinate of the hot spot. */
    uint32_t u32HotX;

    /* Y coordinate of the hot spot. */
    uint32_t u32HotY;

    /* Width of the pointer in pixels. */
    uint32_t u32Width;

    /* Height of the pointer in scanlines. */
    uint32_t u32Height;

    /* Pointer data.
     *
     ****
     * The data consists of 1 bpp AND mask followed by 32 bpp XOR (color) mask.
     *
     * For pointers without alpha channel the XOR mask pixels are 32 bit values: (lsb)BGR0(msb).
     * For pointers with alpha channel the XOR mask consists of (lsb)BGRA(msb) 32 bit values.
     *
     * Guest driver must create the AND mask for pointers with alpha channel, so if host does not
     * support alpha, the pointer could be displayed as a normal color pointer. The AND mask can
     * be constructed from alpha values. For example alpha value >= 0xf0 means bit 0 in the AND mask.
     *
     * The AND mask is 1 bpp bitmap with byte aligned scanlines. Size of AND mask,
     * therefore, is cbAnd = (width + 7) / 8 * height. The padding bits at the
     * end of any scanline are undefined.
     *
     * The XOR mask follows the AND mask on the next 4 bytes aligned offset:
     * uint8_t *pXor = pAnd + (cbAnd + 3) & ~3
     * Bytes in the gap between the AND and the XOR mask are undefined.
     * XOR mask scanlines have no gap between them and size of XOR mask is:
     * cXor = width * 4 * height.
     ****
     *
     * Preallocate 4 bytes for accessing actual data as p->au8Data.
     */
    uint8_t au8Data[4];

} VBVAMOUSEPOINTERSHAPE;

/* the guest driver can handle asynch guest cmd completion by reading the command offset from io port */
#define VBVACAPS_COMPLETEGCMD_BY_IOREAD 0x00000001
/* the guest driver can handle video adapter IRQs */
#define VBVACAPS_IRQ                    0x00000002
typedef struct VBVACAPS
{
    int32_t rc;
    uint32_t fCaps;
} VBVACAPS;

#pragma pack()

typedef uint64_t VBOXVIDEOOFFSET;

#define VBOXVIDEOOFFSET_VOID ((VBOXVIDEOOFFSET)~0)

#ifdef VBOX_WITH_WDDM
# pragma pack(1)

/*
 * VBOXSHGSMI made on top HGSMI and allows receiving notifications
 * about G->H command completion
 */
/* SHGSMI command header */
typedef struct VBOXSHGSMIHEADER
{
    uint64_t pvNext;    /*<- completion processing queue */
    uint32_t fFlags;    /*<- see VBOXSHGSMI_FLAG_XXX Flags */
    uint32_t cRefs;     /*<- command referece count */
    uint64_t u64Info1;  /*<- contents depends on the fFlags value */
    uint64_t u64Info2;  /*<- contents depends on the fFlags value */
} VBOXSHGSMIHEADER, *PVBOXSHGSMIHEADER;

/* the command processing was asynch, set by the host to indicate asynch command completion
 * must not be cleared once set, the command completion is performed by issuing a host->guest completion command
 * while keeping this flag unchanged */
#define VBOXSHGSMI_FLAG_HG_ASYNCH               0x00010000
#if 0
/* if set     - asynch completion is performed by issuing the event,
 * if cleared - asynch completion is performed by calling a callback */
#define VBOXSHGSMI_FLAG_GH_ASYNCH_EVENT         0x00000001
#endif
/* issue interrupt on asynch completion, used for critical G->H commands,
 * i.e. for completion of which guest is waiting. */
#define VBOXSHGSMI_FLAG_GH_ASYNCH_IRQ           0x00000002
/* guest does not do any op on completion of this command,
 * the host may copy the command and indicate that it does not need the command anymore
 * by not setting VBOXSHGSMI_FLAG_HG_ASYNCH */
#define VBOXSHGSMI_FLAG_GH_ASYNCH_NOCOMPLETION  0x00000004
/* guest requires the command to be processed asynchronously,
 * not setting VBOXSHGSMI_FLAG_HG_ASYNCH by the host in this case is treated as command failure */
#define VBOXSHGSMI_FLAG_GH_ASYNCH_FORCE         0x00000008
/* force IRQ on cmd completion */
#define VBOXSHGSMI_FLAG_GH_ASYNCH_IRQ_FORCE     0x00000010
/* an IRQ-level callback is associated with the command */
#define VBOXSHGSMI_FLAG_GH_ASYNCH_CALLBACK_IRQ  0x00000020
/* guest expects this command to be completed synchronously */
#define VBOXSHGSMI_FLAG_GH_SYNCH                0x00000040

DECLINLINE(uint8_t *) VBoxSHGSMIBufferData (const VBOXSHGSMIHEADER* pHeader)
{
    return (uint8_t *)pHeader + sizeof (VBOXSHGSMIHEADER);
}

DECLINLINE(PVBOXSHGSMIHEADER) VBoxSHGSMIBufferHeader (const void *pvData)
{
    return (PVBOXSHGSMIHEADER)((uint8_t *)pvData - sizeof (VBOXSHGSMIHEADER));
}

typedef enum
{
    VBOXVDMACMD_TYPE_UNDEFINED         = 0,
    VBOXVDMACMD_TYPE_DMA_PRESENT_BLT   = 1,
    VBOXVDMACMD_TYPE_DMA_BPB_TRANSFER,
    VBOXVDMACMD_TYPE_DMA_BPB_FILL,
    VBOXVDMACMD_TYPE_DMA_PRESENT_SHADOW2PRIMARY,
    VBOXVDMACMD_TYPE_DMA_PRESENT_CLRFILL,
    VBOXVDMACMD_TYPE_DMA_PRESENT_FLIP,
    VBOXVDMACMD_TYPE_DMA_NOP,
    VBOXVDMACMD_TYPE_CHROMIUM_CMD,
    VBOXVDMACMD_TYPE_DMA_BPB_TRANSFER_VRAMSYS
} VBOXVDMACMD_TYPE;

# pragma pack()
#endif

#ifdef VBOX_WITH_VDMA
# pragma pack(1)

/* VDMA - Video DMA */

/* VDMA Control API */
/* VBOXVDMA_CTL::u32Flags */
typedef enum
{
    VBOXVDMA_CTL_TYPE_NONE = 0,
    VBOXVDMA_CTL_TYPE_ENABLE,
    VBOXVDMA_CTL_TYPE_DISABLE,
    VBOXVDMA_CTL_TYPE_FLUSH
} VBOXVDMA_CTL_TYPE;

typedef struct VBOXVDMA_CTL
{
    VBOXVDMA_CTL_TYPE enmCtl;
    uint32_t u32Offset;
    int32_t  i32Result;
} VBOXVDMA_CTL, *PVBOXVDMA_CTL;

typedef struct VBOXVDMA_RECTL
{
    int16_t left;
    int16_t top;
    uint16_t width;
    uint16_t height;
} VBOXVDMA_RECTL, *PVBOXVDMA_RECTL;

typedef enum
{
    VBOXVDMA_PIXEL_FORMAT_UNKNOWN      =  0,
    VBOXVDMA_PIXEL_FORMAT_R8G8B8       = 20,
    VBOXVDMA_PIXEL_FORMAT_A8R8G8B8     = 21,
    VBOXVDMA_PIXEL_FORMAT_X8R8G8B8     = 22,
    VBOXVDMA_PIXEL_FORMAT_R5G6B5       = 23,
    VBOXVDMA_PIXEL_FORMAT_X1R5G5B5     = 24,
    VBOXVDMA_PIXEL_FORMAT_A1R5G5B5     = 25,
    VBOXVDMA_PIXEL_FORMAT_A4R4G4B4     = 26,
    VBOXVDMA_PIXEL_FORMAT_R3G3B2       = 27,
    VBOXVDMA_PIXEL_FORMAT_A8           = 28,
    VBOXVDMA_PIXEL_FORMAT_A8R3G3B2     = 29,
    VBOXVDMA_PIXEL_FORMAT_X4R4G4B4     = 30,
    VBOXVDMA_PIXEL_FORMAT_A2B10G10R10  = 31,
    VBOXVDMA_PIXEL_FORMAT_A8B8G8R8     = 32,
    VBOXVDMA_PIXEL_FORMAT_X8B8G8R8     = 33,
    VBOXVDMA_PIXEL_FORMAT_G16R16       = 34,
    VBOXVDMA_PIXEL_FORMAT_A2R10G10B10  = 35,
    VBOXVDMA_PIXEL_FORMAT_A16B16G16R16 = 36,
    VBOXVDMA_PIXEL_FORMAT_A8P8         = 40,
    VBOXVDMA_PIXEL_FORMAT_P8           = 41,
    VBOXVDMA_PIXEL_FORMAT_L8           = 50,
    VBOXVDMA_PIXEL_FORMAT_A8L8         = 51,
    VBOXVDMA_PIXEL_FORMAT_A4L4         = 52,
    VBOXVDMA_PIXEL_FORMAT_V8U8         = 60,
    VBOXVDMA_PIXEL_FORMAT_L6V5U5       = 61,
    VBOXVDMA_PIXEL_FORMAT_X8L8V8U8     = 62,
    VBOXVDMA_PIXEL_FORMAT_Q8W8V8U8     = 63,
    VBOXVDMA_PIXEL_FORMAT_V16U16       = 64,
    VBOXVDMA_PIXEL_FORMAT_W11V11U10    = 65,
    VBOXVDMA_PIXEL_FORMAT_A2W10V10U10  = 67
} VBOXVDMA_PIXEL_FORMAT;

typedef struct VBOXVDMA_SURF_DESC
{
    uint32_t width;
    uint32_t height;
    VBOXVDMA_PIXEL_FORMAT format;
    uint32_t bpp;
    uint32_t pitch;
    uint32_t fFlags;
} VBOXVDMA_SURF_DESC, *PVBOXVDMA_SURF_DESC;

/*typedef uint64_t VBOXVDMAPHADDRESS;*/
typedef uint64_t VBOXVDMASURFHANDLE;

/* region specified as a rectangle, otherwize it is a size of memory pointed to by phys address */
#define VBOXVDMAOPERAND_FLAGS_RECTL       0x1
/* Surface handle is valid */
#define VBOXVDMAOPERAND_FLAGS_PRIMARY        0x2
/* address is offset in VRAM */
#define VBOXVDMAOPERAND_FLAGS_VRAMOFFSET  0x4


/* VBOXVDMACBUF_DR::phBuf specifies offset in VRAM */
#define VBOXVDMACBUF_FLAG_BUF_VRAM_OFFSET 0x00000001
/* command buffer follows the VBOXVDMACBUF_DR in VRAM, VBOXVDMACBUF_DR::phBuf is ignored */
#define VBOXVDMACBUF_FLAG_BUF_FOLLOWS_DR  0x00000002

/*
 * We can not submit the DMA command via VRAM since we do not have control over
 * DMA command buffer [de]allocation, i.e. we only control the buffer contents.
 * In other words the system may call one of our callbacks to fill a command buffer
 * with the necessary commands and then discard the buffer w/o any notification.
 *
 * We have only DMA command buffer physical address at submission time.
 *
 * so the only way is to */
typedef struct VBOXVDMACBUF_DR
{
    uint16_t fFlags;
    uint16_t cbBuf;
    /* RT_SUCCESS()     - on success
     * VERR_INTERRUPTED - on preemption
     * VERR_xxx         - on error */
    int32_t  rc;
    union
    {
        uint64_t phBuf;
        VBOXVIDEOOFFSET offVramBuf;
    } Location;
    uint64_t aGuestData[7];
} VBOXVDMACBUF_DR, *PVBOXVDMACBUF_DR;

#define VBOXVDMACBUF_DR_TAIL(_pCmd, _t) ( (_t*)(((uint8_t*)(_pCmd)) + sizeof (VBOXVDMACBUF_DR)) )
#define VBOXVDMACBUF_DR_FROM_TAIL(_pCmd) ( (VBOXVDMACBUF_DR*)(((uint8_t*)(_pCmd)) - sizeof (VBOXVDMACBUF_DR)) )

typedef struct VBOXVDMACMD
{
    VBOXVDMACMD_TYPE enmType;
    uint32_t u32CmdSpecific;
} VBOXVDMACMD, *PVBOXVDMACMD;

#define VBOXVDMACMD_HEADER_SIZE() sizeof (VBOXVDMACMD)
#define VBOXVDMACMD_SIZE_FROMBODYSIZE(_s) (VBOXVDMACMD_HEADER_SIZE() + (_s))
#define VBOXVDMACMD_SIZE(_t) (VBOXVDMACMD_SIZE_FROMBODYSIZE(sizeof (_t)))
#define VBOXVDMACMD_BODY(_pCmd, _t) ( (_t*)(((uint8_t*)(_pCmd)) + VBOXVDMACMD_HEADER_SIZE()) )
#define VBOXVDMACMD_FROM_BODY(_pCmd) ( (VBOXVDMACMD*)(((uint8_t*)(_pCmd)) - VBOXVDMACMD_HEADER_SIZE()) )
#define VBOXVDMACMD_BODY_FIELD_OFFSET(_ot, _t, _f) ( (_ot)(uintptr_t)( VBOXVDMACMD_BODY(0, uint8_t) + RT_OFFSETOF(_t, _f) ) )

typedef struct VBOXVDMACMD_DMA_PRESENT_BLT
{
    VBOXVIDEOOFFSET offSrc;
    VBOXVIDEOOFFSET offDst;
    VBOXVDMA_SURF_DESC srcDesc;
    VBOXVDMA_SURF_DESC dstDesc;
    VBOXVDMA_RECTL srcRectl;
    VBOXVDMA_RECTL dstRectl;
    uint32_t u32Reserved;
    uint32_t cDstSubRects;
    VBOXVDMA_RECTL aDstSubRects[1];
} VBOXVDMACMD_DMA_PRESENT_BLT, *PVBOXVDMACMD_DMA_PRESENT_BLT;

typedef struct VBOXVDMACMD_DMA_PRESENT_SHADOW2PRIMARY
{
    VBOXVDMA_RECTL Rect;
} VBOXVDMACMD_DMA_PRESENT_SHADOW2PRIMARY, *PVBOXVDMACMD_DMA_PRESENT_SHADOW2PRIMARY;


#define VBOXVDMACMD_DMA_BPB_TRANSFER_F_SRC_VRAMOFFSET 0x00000001
#define VBOXVDMACMD_DMA_BPB_TRANSFER_F_DST_VRAMOFFSET 0x00000002

typedef struct VBOXVDMACMD_DMA_BPB_TRANSFER
{
    uint32_t cbTransferSize;
    uint32_t fFlags;
    union
    {
        uint64_t phBuf;
        VBOXVIDEOOFFSET offVramBuf;
    } Src;
    union
    {
        uint64_t phBuf;
        VBOXVIDEOOFFSET offVramBuf;
    } Dst;
} VBOXVDMACMD_DMA_BPB_TRANSFER, *PVBOXVDMACMD_DMA_BPB_TRANSFER;

#define VBOXVDMACMD_SYSMEMEL_F_PAGELIST 0x00000001

typedef struct VBOXVDMACMD_SYSMEMEL
{
    uint32_t cPages;
    uint32_t fFlags;
    uint64_t phBuf[1];
} VBOXVDMACMD_SYSMEMEL, *PVBOXVDMACMD_SYSMEMEL;

#define VBOXVDMACMD_SYSMEMEL_NEXT(_pEl) (((_pEl)->fFlags & VBOXVDMACMD_SYSMEMEL_F_PAGELIST) ? \
        ((PVBOXVDMACMD_SYSMEMEL)(((uint8_t*)(_pEl))+RT_OFFSETOF(VBOXVDMACMD_SYSMEMEL, phBuf[(_pEl)->cPages]))) \
        : \
        ((_pEl)+1)

#define VBOXVDMACMD_DMA_BPB_TRANSFER_VRAMSYS_SYS2VRAM 0x00000001

typedef struct VBOXVDMACMD_DMA_BPB_TRANSFER_VRAMSYS
{
    uint32_t cTransferPages;
    uint32_t fFlags;
    VBOXVIDEOOFFSET offVramBuf;
    VBOXVDMACMD_SYSMEMEL FirstEl;
} VBOXVDMACMD_DMA_BPB_TRANSFER_VRAMSYS, *PVBOXVDMACMD_DMA_BPB_TRANSFER_VRAMSYS;

typedef struct VBOXVDMACMD_DMA_BPB_FILL
{
    VBOXVIDEOOFFSET offSurf;
    uint32_t cbFillSize;
    uint32_t u32FillPattern;
} VBOXVDMACMD_DMA_BPB_FILL, *PVBOXVDMACMD_DMA_BPB_FILL;

# pragma pack()
#endif /* #ifdef VBOX_WITH_VDMA */

#ifdef VBOX_WITH_CRHGSMI
# pragma pack(1)
typedef struct VBOXVDMACMD_CHROMIUM_BUFFER
{
    VBOXVIDEOOFFSET offBuffer;
    uint32_t cbBuffer;
    uint32_t u32GuesData;
    uint64_t u64GuesData;
} VBOXVDMACMD_CHROMIUM_BUFFER, *PVBOXVDMACMD_CHROMIUM_BUFFER;

typedef struct VBOXVDMACMD_CHROMIUM_CMD
{
    uint32_t cBuffers;
    uint32_t u32Reserved;
    VBOXVDMACMD_CHROMIUM_BUFFER aBuffers[1];
} VBOXVDMACMD_CHROMIUM_CMD, *PVBOXVDMACMD_CHROMIUM_CMD;

typedef enum
{
    VBOXVDMACMD_CHROMIUM_CTL_TYPE_UNKNOWN = 0,
    VBOXVDMACMD_CHROMIUM_CTL_TYPE_CRHGSMI_SETUP,
    VBOXVDMACMD_CHROMIUM_CTL_TYPE_SAVESTATE_BEGIN,
    VBOXVDMACMD_CHROMIUM_CTL_TYPE_SAVESTATE_END,
    VBOXVDMACMD_CHROMIUM_CTL_TYPE_SIZEHACK = 0xfffffffe
} VBOXVDMACMD_CHROMIUM_CTL_TYPE;

typedef struct VBOXVDMACMD_CHROMIUM_CTL
{
    VBOXVDMACMD_CHROMIUM_CTL_TYPE enmType;
    uint32_t cbCmd;
} VBOXVDMACMD_CHROMIUM_CTL, *PVBOXVDMACMD_CHROMIUM_CTL;

typedef struct VBOXVDMACMD_CHROMIUM_CTL_CRHGSMI_SETUP
{
    VBOXVDMACMD_CHROMIUM_CTL Hdr;
    union
    {
        void *pvRamBase;
        uint64_t uAlignment;
    };
} VBOXVDMACMD_CHROMIUM_CTL_CRHGSMI_SETUP, *PVBOXVDMACMD_CHROMIUM_CTL_CRHGSMI_SETUP;
# pragma pack()
#endif

#ifdef VBOXVDMA_WITH_VBVA
# pragma pack(1)

typedef struct VBOXVDMAVBVACMD
{
    HGSMIOFFSET offCmd;
} VBOXVDMAVBVACMD;

#pragma pack()
#endif

#endif
