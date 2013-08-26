/** @file
 * IPRT - Directory Manipulation.
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

#ifndef ___iprt_dir_h
#define ___iprt_dir_h

#include <iprt/cdefs.h>
#include <iprt/types.h>
#include <iprt/fs.h>


RT_C_DECLS_BEGIN

/** @defgroup grp_rt_dir    RTDir - Directory Manipulation
 * @ingroup grp_rt
 * @{
 */

/**
 * Check for the existence of a directory.
 *
 * All symbolic links will be attemped resolved.  If that is undesirable, please
 * use RTPathQueryInfo instead.
 *
 * @returns true if exist and is a directory.
 * @returns false if not exists or isn't a directory.
 * @param   pszPath     Path to the directory.
 */
RTDECL(bool) RTDirExists(const char *pszPath);

/**
 * Creates a directory.
 *
 * @returns iprt status code.
 * @param   pszPath     Path to the directory to create.
 * @param   fMode       The mode of the new directory.
 */
RTDECL(int) RTDirCreate(const char *pszPath, RTFMODE fMode);

/**
 * Creates a directory including all parent directories in the path
 * if they don't exist.
 *
 * @returns iprt status code.
 * @param   pszPath     Path to the directory to create.
 * @param   fMode       The mode of the new directories.
 */
RTDECL(int) RTDirCreateFullPath(const char *pszPath, RTFMODE fMode);

/**
 * Creates a new directory with a unique name using the given template.
 *
 * One or more trailing X'es in the template will be replaced by random alpha
 * numeric characters until a RTDirCreate succeeds or we run out of patience.
 * For instance:
 *          "/tmp/myprog-XXXXXX"
 *
 * As an alternative to trailing X'es, it
 * is possible to put 3 or more X'es somewhere inside the directory name. In
 * the following string only the last bunch of X'es will be modified:
 *          "/tmp/myprog-XXX-XXX.tmp"
 *
 * The directory is created with mode 0700.
 *
 * @returns iprt status code.
 * @param   pszTemplate     The directory name template on input. The actual
 *                          directory name on success. Empty string on failure.
 */
RTDECL(int) RTDirCreateTemp(char *pszTemplate);

/**
 * Removes a directory if empty.
 *
 * @returns iprt status code.
 * @param   pszPath         Path to the directory to remove.
 */
RTDECL(int) RTDirRemove(const char *pszPath);

/**
 * Removes a directory tree recursively.
 *
 * @returns iprt status code.
 * @param   pszPath         Path to the directory to remove recursively.
 * @param   fFlags          Flags, see RTDIRRMREC_F_XXX.
 *
 * @remarks This will not work on a root directory.
 */
RTDECL(int) RTDirRemoveRecursive(const char *pszPath, uint32_t fFlags);

/** @name   RTDirRemoveRecursive flags.
 * @{ */
/** Delete the content of the directory and the directory itself. */
#define RTDIRRMREC_F_CONTENT_AND_DIR    UINT32_C(0)
/** Only delete the content of the directory, omit the directory it self. */
#define RTDIRRMREC_F_CONTENT_ONLY       RT_BIT_32(0)
/** Mask of valid flags. */
#define RTDIRRMREC_F_VALID_MASK         UINT32_C(0x00000001)
/** @} */

/**
 * Flushes the specified directory.
 *
 * This API is not implemented on all systems.  On some systems it may be
 * unnecessary if you've already flushed the file.  If you really care for your
 * data and is entering dangerous territories, it doesn't hurt calling it after
 * flushing and closing the file.
 *
 * @returns IPRT status code.
 * @retval  VERR_NOT_IMPLEMENTED must be expected.
 * @retval  VERR_NOT_SUPPORTED must be expected.
 * @param   pszPath     Path to the directory.
 */
RTDECL(int) RTDirFlush(const char *pszPath);

/**
 * Flushes the parent directory of the specified file.
 *
 * This is just a wrapper around RTDirFlush.
 *
 * @returns IPRT status code, see RTDirFlush for details.
 * @param   pszChild    Path to the file which parent should be flushed.
 */
RTDECL(int) RTDirFlushParent(const char *pszChild);


/** Pointer to an open directory (sort of handle). */
typedef struct RTDIR *PRTDIR;


/**
 * Filter option for RTDirOpenFiltered().
 */
typedef enum RTDIRFILTER
{
    /** The usual invalid 0 entry. */
    RTDIRFILTER_INVALID = 0,
    /** No filter should be applied (and none was specified). */
    RTDIRFILTER_NONE,
    /** The Windows NT filter.
     * The following wildcard chars: *, ?, <, > and "
     * The matching is done on the uppercased strings.  */
    RTDIRFILTER_WINNT,
    /** The UNIX filter.
     * The following wildcard chars: *, ?, [..]
     * The matching is done on exact case. */
    RTDIRFILTER_UNIX,
    /** The UNIX filter, uppercased matching.
     * Same as RTDIRFILTER_UNIX except that the strings are uppercased before comparing. */
    RTDIRFILTER_UNIX_UPCASED,

    /** The usual full 32-bit value. */
    RTDIRFILTER_32BIT_HACK = 0x7fffffff
} RTDIRFILTER;


/**
 * Directory entry type.
 *
 * This is the RTFS_TYPE_MASK stuff shifted down 12 bits and
 * identical to the BSD/LINUX ABI.
 */
typedef enum RTDIRENTRYTYPE
{
    /** Unknown type (DT_UNKNOWN). */
    RTDIRENTRYTYPE_UNKNOWN          = 0,
    /** Named pipe (fifo) (DT_FIFO). */
    RTDIRENTRYTYPE_FIFO             = 001,
    /** Character device (DT_CHR). */
    RTDIRENTRYTYPE_DEV_CHAR         = 002,
    /** Directory (DT_DIR). */
    RTDIRENTRYTYPE_DIRECTORY        = 004,
    /** Block device (DT_BLK). */
    RTDIRENTRYTYPE_DEV_BLOCK        = 006,
    /** Regular file (DT_REG). */
    RTDIRENTRYTYPE_FILE             = 010,
    /** Symbolic link (DT_LNK). */
    RTDIRENTRYTYPE_SYMLINK          = 012,
    /** Socket (DT_SOCK). */
    RTDIRENTRYTYPE_SOCKET           = 014,
    /** Whiteout (DT_WHT). */
    RTDIRENTRYTYPE_WHITEOUT         = 016
} RTDIRENTRYTYPE;


/**
 * Directory entry.
 *
 * This is inspired by the POSIX interfaces.
 */
#pragma pack(1)
typedef struct RTDIRENTRY
{
    /** The unique identifier (within the file system) of this file system object (d_ino).
     *
     * Together with INodeIdDevice, this field can be used as a OS wide unique id
     * when both their values are not 0.  This field is 0 if the information is not
     * available. */
    RTINODE         INodeId;
    /** The entry type. (d_type)
     * RTDIRENTRYTYPE_UNKNOWN is a common return value here since not all file
     * systems (or Unixes) stores the type of a directory entry and instead
     * expects the user to use stat() to get it.  So, when you see this you
     * should use RTPathQueryInfo to get the type, or if if you're lazy, use
     * RTDirReadEx. */
    RTDIRENTRYTYPE  enmType;
    /** The length of the filename, excluding the terminating nul character. */
    uint16_t        cbName;
    /** The filename. (no path)
     * Using the pcbDirEntry parameter of RTDirRead makes this field variable in size. */
    char            szName[260];
} RTDIRENTRY;
#pragma pack()
/** Pointer to a directory entry. */
typedef RTDIRENTRY *PRTDIRENTRY;


/**
 * Directory entry with extended information.
 *
 * This is inspired by the PC interfaces.
 */
#pragma pack(1)
typedef struct RTDIRENTRYEX
{
    /** Full information about the object. */
    RTFSOBJINFO     Info;
    /** The length of the short field (number of RTUTF16 entries (not chars)).
     * It is 16-bit for reasons of alignment. */
    uint16_t        cwcShortName;
    /** The short name for 8.3 compatibility.
     * Empty string if not available.
     * Since the length is a bit tricky for a UTF-8 encoded name, and since this
     * is practically speaking only a windows thing, it is encoded as UCS-2. */
    RTUTF16         wszShortName[14];
    /** The length of the filename. */
    uint16_t        cbName;
    /** The filename. (no path)
     * Using the pcbDirEntry parameter of RTDirReadEx makes this field variable in size. */
    char            szName[260];
} RTDIRENTRYEX;
#pragma pack()
/** Pointer to a directory entry. */
typedef RTDIRENTRYEX *PRTDIRENTRYEX;


/**
 * Opens a directory.
 *
 * @returns iprt status code.
 * @param   ppDir       Where to store the open directory pointer.
 * @param   pszPath     Path to the directory to open.
 */
RTDECL(int) RTDirOpen(PRTDIR *ppDir, const char *pszPath);

/**
 * Opens a directory filtering the entries using dos style wildcards.
 *
 * @returns iprt status code.
 * @param   ppDir       Where to store the open directory pointer.
 * @param   pszPath     Path to the directory to search, this must include wildcards.
 * @param   enmFilter   The kind of filter to apply. Setting this to RTDIRFILTER_NONE makes
 *                      this function behave like RTDirOpen.
 */
RTDECL(int) RTDirOpenFiltered(PRTDIR *ppDir, const char *pszPath, RTDIRFILTER enmFilter);

/**
 * Closes a directory.
 *
 * @returns iprt status code.
 * @param   pDir        Pointer to open directory returned by RTDirOpen() or RTDirOpenFiltered().
 */
RTDECL(int) RTDirClose(PRTDIR pDir);

/**
 * Reads the next entry in the directory.
 *
 * @returns VINF_SUCCESS and data in pDirEntry on success.
 * @returns VERR_NO_MORE_FILES when the end of the directory has been reached.
 * @returns VERR_BUFFER_OVERFLOW if the buffer is too small to contain the filename. If
 *          pcbDirEntry is specified it will be updated with the required buffer size.
 * @returns suitable iprt status code on other errors.
 *
 * @param   pDir        Pointer to the open directory.
 * @param   pDirEntry   Where to store the information about the next
 *                      directory entry on success.
 * @param   pcbDirEntry Optional parameter used for variable buffer size.
 *
 *                      On input the variable pointed to contains the size of the pDirEntry
 *                      structure. This must be at least OFFSET(RTDIRENTRY, szName[2]) bytes.
 *
 *                      On successful output the field is updated to
 *                      OFFSET(RTDIRENTRY, szName[pDirEntry->cbName + 1]).
 *
 *                      When the data doesn't fit in the buffer and VERR_BUFFER_OVERFLOW is
 *                      returned, this field contains the required buffer size.
 *
 *                      The value is unchanged in all other cases.
 */
RTDECL(int) RTDirRead(PRTDIR pDir, PRTDIRENTRY pDirEntry, size_t *pcbDirEntry);

/**
 * Reads the next entry in the directory returning extended information.
 *
 * @returns VINF_SUCCESS and data in pDirEntry on success.
 * @returns VERR_NO_MORE_FILES when the end of the directory has been reached.
 * @returns VERR_BUFFER_OVERFLOW if the buffer is too small to contain the filename. If
 *          pcbDirEntry is specified it will be updated with the required buffer size.
 * @returns suitable iprt status code on other errors.
 *
 * @param   pDir        Pointer to the open directory.
 * @param   pDirEntry   Where to store the information about the next
 *                      directory entry on success.
 * @param   pcbDirEntry Optional parameter used for variable buffer size.
 *
 *                      On input the variable pointed to contains the size of the pDirEntry
 *                      structure. This must be at least OFFSET(RTDIRENTRYEX, szName[2]) bytes.
 *
 *                      On successful output the field is updated to
 *                      OFFSET(RTDIRENTRYEX, szName[pDirEntry->cbName + 1]).
 *
 *                      When the data doesn't fit in the buffer and VERR_BUFFER_OVERFLOW is
 *                      returned, this field contains the required buffer size.
 *
 *                      The value is unchanged in all other cases.
 * @param   enmAdditionalAttribs
 *                      Which set of additional attributes to request.
 *                      Use RTFSOBJATTRADD_NOTHING if this doesn't matter.
 * @param   fFlags      RTPATH_F_ON_LINK or RTPATH_F_FOLLOW_LINK.
 */
RTDECL(int) RTDirReadEx(PRTDIR pDir, PRTDIRENTRYEX pDirEntry, size_t *pcbDirEntry, RTFSOBJATTRADD enmAdditionalAttribs, uint32_t fFlags);


/**
 * Renames a file.
 *
 * Identical to RTPathRename except that it will ensure that the source is a directory.
 *
 * @returns IPRT status code.
 * @returns VERR_ALREADY_EXISTS if the destination file exists.
 *
 * @param   pszSrc      The path to the source file.
 * @param   pszDst      The path to the destination file.
 *                      This file will be created.
 * @param   fRename     See RTPathRename.
 */
RTDECL(int) RTDirRename(const char *pszSrc, const char *pszDst, unsigned fRename);


/**
 * Query information about an open directory.
 *
 * @returns iprt status code.
 *
 * @param   pDir        Pointer to the open directory.
 * @param   pObjInfo                Object information structure to be filled on successful return.
 * @param   enmAdditionalAttribs    Which set of additional attributes to request.
 *                                  Use RTFSOBJATTRADD_NOTHING if this doesn't matter.
 */
RTR3DECL(int) RTDirQueryInfo(PRTDIR pDir, PRTFSOBJINFO pObjInfo, RTFSOBJATTRADD enmAdditionalAttribs);


/**
 * Changes one or more of the timestamps associated of file system object.
 *
 * @returns iprt status code.
 * @returns VERR_NOT_SUPPORTED is returned if the operation isn't supported by the OS.
 *
 * @param   pDir        Pointer to the open directory.
 * @param   pAccessTime         Pointer to the new access time. NULL if not to be changed.
 * @param   pModificationTime   Pointer to the new modifcation time. NULL if not to be changed.
 * @param   pChangeTime         Pointer to the new change time. NULL if not to be changed.
 * @param   pBirthTime          Pointer to the new time of birth. NULL if not to be changed.
 *
 * @remark  The file system might not implement all these time attributes,
 *          the API will ignore the ones which aren't supported.
 *
 * @remark  The file system might not implement the time resolution
 *          employed by this interface, the time will be chopped to fit.
 *
 * @remark  The file system may update the change time even if it's
 *          not specified.
 *
 * @remark  POSIX can only set Access & Modification and will always set both.
 */
RTR3DECL(int) RTDirSetTimes(PRTDIR pDir, PCRTTIMESPEC pAccessTime, PCRTTIMESPEC pModificationTime,
                            PCRTTIMESPEC pChangeTime, PCRTTIMESPEC pBirthTime);

/** @} */

RT_C_DECLS_END

#endif

