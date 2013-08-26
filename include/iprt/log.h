/** @file
 * IPRT - Logging.
 */

/*
 * Copyright (C) 2006-2007 Oracle Corporation
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

#ifndef ___iprt_log_h
#define ___iprt_log_h

#include <iprt/cdefs.h>
#include <iprt/types.h>
#include <iprt/stdarg.h>

RT_C_DECLS_BEGIN

/** @defgroup grp_rt_log    RTLog - Logging
 * @ingroup grp_rt
 * @{
 */

/**
 * IPRT Logging Groups.
 * (Remember to update RT_LOGGROUP_NAMES!)
 *
 * @remark It should be pretty obvious, but just to have
 *         mentioned it, the values are sorted alphabetically (using the
 *         english alphabet) except for _DEFAULT which is always first.
 *
 *         If anyone might be wondering what the alphabet looks like:
 *              a b c d e f g h i j k l m n o p q r s t u v w x y z
 */
typedef enum RTLOGGROUP
{
    /** Default logging group. */
    RTLOGGROUP_DEFAULT,
    RTLOGGROUP_DIR,
    RTLOGGROUP_FILE,
    RTLOGGROUP_FS,
    RTLOGGROUP_LDR,
    RTLOGGROUP_PATH,
    RTLOGGROUP_PROCESS,
    RTLOGGROUP_SYMLINK,
    RTLOGGROUP_THREAD,
    RTLOGGROUP_TIME,
    RTLOGGROUP_TIMER,
    RTLOGGROUP_ZIP = 31,
    RTLOGGROUP_FIRST_USER = 32
} RTLOGGROUP;

/** @def RT_LOGGROUP_NAMES
 * IPRT Logging group names.
 *
 * Must correspond 100% to RTLOGGROUP!
 * Don't forget commas!
 *
 * @remark It should be pretty obvious, but just to have
 *         mentioned it, the values are sorted alphabetically (using the
 *         english alphabet) except for _DEFAULT which is always first.
 *
 *         If anyone might be wondering what the alphabet looks like:
 *              a b c d e f g h i j k l m n o p q r s t u v w x y z
 */
#define RT_LOGGROUP_NAMES \
    "DEFAULT",      \
    "RT_DIR",       \
    "RT_FILE",      \
    "RT_FS",        \
    "RT_LDR",       \
    "RT_PATH",      \
    "RT_PROCESS",   \
    "RT_SYMLINK",   \
    "RT_THREAD",    \
    "RT_TIME",      \
    "RT_TIMER",     \
    "RT_11", \
    "RT_12", \
    "RT_13", \
    "RT_14", \
    "RT_15", \
    "RT_16", \
    "RT_17", \
    "RT_18", \
    "RT_19", \
    "RT_20", \
    "RT_21", \
    "RT_22", \
    "RT_23", \
    "RT_24", \
    "RT_25", \
    "RT_26", \
    "RT_27", \
    "RT_28", \
    "RT_29", \
    "RT_30", \
    "RT_ZIP"  \


/** @def LOG_GROUP
 * Active logging group.
 */
#ifndef LOG_GROUP
# define LOG_GROUP          RTLOGGROUP_DEFAULT
#endif

/** @def LOG_INSTANCE
 * Active logging instance.
 */
#ifndef LOG_INSTANCE
# define LOG_INSTANCE       NULL
#endif

/** @def LOG_REL_INSTANCE
 * Active release logging instance.
 */
#ifndef LOG_REL_INSTANCE
# define LOG_REL_INSTANCE   NULL
#endif

/** @def LOG_FN_FMT
 * You can use this to specify you desired way of printing __PRETTY_FUNCTION__
 * if you dislike the default one.
 */
#ifndef LOG_FN_FMT
# define LOG_FN_FMT "%Rfn"
#endif

/** Logger structure. */
#ifdef IN_RC
typedef struct RTLOGGERRC RTLOGGER;
#else
typedef struct RTLOGGER RTLOGGER;
#endif
/** Pointer to logger structure. */
typedef RTLOGGER *PRTLOGGER;
/** Pointer to const logger structure. */
typedef const RTLOGGER *PCRTLOGGER;


/** Guest context logger structure. */
typedef struct RTLOGGERRC RTLOGGERRC;
/** Pointer to guest context logger structure. */
typedef RTLOGGERRC *PRTLOGGERRC;
/** Pointer to const guest context logger structure. */
typedef const RTLOGGERRC *PCRTLOGGERRC;


/**
 * Logger function.
 *
 * @param   pszFormat   Format string.
 * @param   ...         Optional arguments as specified in the format string.
 */
typedef DECLCALLBACK(void) FNRTLOGGER(const char *pszFormat, ...);
/** Pointer to logger function. */
typedef FNRTLOGGER *PFNRTLOGGER;

/**
 * Flush function.
 *
 * @param   pLogger     Pointer to the logger instance which is to be flushed.
 */
typedef DECLCALLBACK(void) FNRTLOGFLUSH(PRTLOGGER pLogger);
/** Pointer to flush function. */
typedef FNRTLOGFLUSH *PFNRTLOGFLUSH;

/**
 * Flush function.
 *
 * @param   pLogger     Pointer to the logger instance which is to be flushed.
 */
typedef DECLCALLBACK(void) FNRTLOGFLUSHGC(PRTLOGGERRC pLogger);
/** Pointer to logger function. */
typedef RCPTRTYPE(FNRTLOGFLUSHGC *) PFNRTLOGFLUSHGC;

/**
 * Custom log prefix callback.
 *
 *
 * @returns The number of chars written.
 *
 * @param   pLogger     Pointer to the logger instance.
 * @param   pchBuf      Output buffer pointer.
 *                      No need to terminate the output.
 * @param   cchBuf      The size of the output buffer.
 * @param   pvUser      The user argument.
 */
typedef DECLCALLBACK(size_t) FNRTLOGPREFIX(PRTLOGGER pLogger, char *pchBuf, size_t cchBuf, void *pvUser);
/** Pointer to prefix callback function. */
typedef FNRTLOGPREFIX *PFNRTLOGPREFIX;



/**
 * Logger instance structure for GC.
 */
struct RTLOGGERRC
{
    /** Pointer to temporary scratch buffer.
     * This is used to format the log messages. */
    char                    achScratch[32768];
    /** Current scratch buffer position. */
    uint32_t                offScratch;
    /** This is set if a prefix is pending. */
    uint32_t                fPendingPrefix;
    /** Pointer to the logger function.
     * This is actually pointer to a wrapper which will push a pointer to the
     * instance pointer onto the stack before jumping to the real logger function.
     * A very unfortunate hack to work around the missing variadic macro support in C++. */
    RCPTRTYPE(PFNRTLOGGER)  pfnLogger;
    /** Pointer to the flush function. */
    PFNRTLOGFLUSHGC         pfnFlush;
    /** Magic number (RTLOGGERRC_MAGIC). */
    uint32_t                u32Magic;
    /** Logger instance flags - RTLOGFLAGS. */
    uint32_t                fFlags;
    /** Number of groups in the afGroups member. */
    uint32_t                cGroups;
    /** Group flags array - RTLOGGRPFLAGS.
     * This member have variable length and may extend way beyond
     * the declared size of 1 entry. */
    uint32_t                afGroups[1];
};

/** RTLOGGERRC::u32Magic value. (John Rogers Searle) */
#define RTLOGGERRC_MAGIC    0x19320731



#ifndef IN_RC
/**
 * Logger instance structure.
 */
struct RTLOGGER
{
    /** Pointer to temporary scratch buffer.
     * This is used to format the log messages. */
    char                    achScratch[32768];
    /** Current scratch buffer position. */
    uint32_t                offScratch;
    /** This is set if a prefix is pending. */
    uint32_t                fPendingPrefix;
    /** Pointer to the logger function.
     * This is actually pointer to a wrapper which will push a pointer to the
     * instance pointer onto the stack before jumping to the real logger function.
     * A very unfortunate hack to work around the missing variadic macro support in C++.
     * (The memory is (not R0) allocated using RTMemExecAlloc().) */
    PFNRTLOGGER             pfnLogger;
    /** Pointer to the flush function. */
    PFNRTLOGFLUSH           pfnFlush;
    /** Custom prefix callback. */
    PFNRTLOGPREFIX          pfnPrefix;
    /** Prefix callback argument. */
    void                   *pvPrefixUserArg;
    /** Spinning mutex semaphore. */
    RTSEMSPINMUTEX          hSpinMtx;
    /** Magic number. */
    uint32_t                u32Magic;
    /** Logger instance flags - RTLOGFLAGS. */
    uint32_t                fFlags;
    /** Destination flags - RTLOGDEST. */
    uint32_t                fDestFlags;
    /** Handle to log file (if open). */
    RTFILE                  File;
    /** Pointer to filename.
     * (The memory is allocated in the same block as RTLOGGER.) */
    char                   *pszFilename;
    /** Pointer to the group name array.
     * (The data is readonly and provided by the user.) */
    const char * const     *papszGroups;
    /** The max number of groups that there is room for in afGroups and papszGroups.
     * Used by RTLogCopyGroupAndFlags(). */
    uint32_t                cMaxGroups;
    /** Number of groups in the afGroups and papszGroups members. */
    uint32_t                cGroups;
    /** Group flags array - RTLOGGRPFLAGS.
     * This member have variable length and may extend way beyond
     * the declared size of 1 entry. */
    uint32_t                afGroups[1];
};

/** RTLOGGER::u32Magic value. (Avram Noam Chomsky) */
#define RTLOGGER_MAGIC      0x19281207

#endif /* !IN_RC */


/**
 * Logger flags.
 */
typedef enum RTLOGFLAGS
{
    /** The logger instance is disabled for normal output. */
    RTLOGFLAGS_DISABLED             = 0x00000001,
    /** The logger instance is using buffered output. */
    RTLOGFLAGS_BUFFERED             = 0x00000002,
    /** The logger instance expands LF to CR/LF. */
    RTLOGFLAGS_USECRLF              = 0x00000010,
    /** Append to the log destination where applicable. */
    RTLOGFLAGS_APPEND               = 0x00000020,
    /** Show relative timestamps with PREFIX_TSC and PREFIX_TS */
    RTLOGFLAGS_REL_TS               = 0x00000040,
    /** Show decimal timestamps with PREFIX_TSC and PREFIX_TS */
    RTLOGFLAGS_DECIMAL_TS           = 0x00000080,
    /** Open the file in write through mode. */
    RTLOGFLAGS_WRITE_THROUGH        = 0x00000100,
    /** Flush the file to disk when flushing the buffer. */
    RTLOGFLAGS_FLUSH                = 0x00000200,
    /** New lines should be prefixed with the write and read lock counts. */
    RTLOGFLAGS_PREFIX_LOCK_COUNTS   = 0x00008000,
    /** New lines should be prefixed with the CPU id (ApicID on intel/amd). */
    RTLOGFLAGS_PREFIX_CPUID         = 0x00010000,
    /** New lines should be prefixed with the native process id. */
    RTLOGFLAGS_PREFIX_PID           = 0x00020000,
    /** New lines should be prefixed with group flag number causing the output. */
    RTLOGFLAGS_PREFIX_FLAG_NO       = 0x00040000,
    /** New lines should be prefixed with group flag name causing the output. */
    RTLOGFLAGS_PREFIX_FLAG          = 0x00080000,
    /** New lines should be prefixed with group number. */
    RTLOGFLAGS_PREFIX_GROUP_NO      = 0x00100000,
    /** New lines should be prefixed with group name. */
    RTLOGFLAGS_PREFIX_GROUP         = 0x00200000,
    /** New lines should be prefixed with the native thread id. */
    RTLOGFLAGS_PREFIX_TID           = 0x00400000,
    /** New lines should be prefixed with thread name. */
    RTLOGFLAGS_PREFIX_THREAD        = 0x00800000,
    /** New lines should be prefixed with data from a custom callback. */
    RTLOGFLAGS_PREFIX_CUSTOM        = 0x01000000,
    /** New lines should be prefixed with formatted timestamp since program start. */
    RTLOGFLAGS_PREFIX_TIME_PROG     = 0x04000000,
    /** New lines should be prefixed with formatted timestamp (UCT). */
    RTLOGFLAGS_PREFIX_TIME          = 0x08000000,
    /** New lines should be prefixed with milliseconds since program start. */
    RTLOGFLAGS_PREFIX_MS_PROG       = 0x10000000,
    /** New lines should be prefixed with timestamp. */
    RTLOGFLAGS_PREFIX_TSC           = 0x20000000,
    /** New lines should be prefixed with timestamp. */
    RTLOGFLAGS_PREFIX_TS            = 0x40000000,
    /** The prefix mask. */
    RTLOGFLAGS_PREFIX_MASK          = 0x7dff8000
} RTLOGFLAGS;

/**
 * Logger per group flags.
 */
typedef enum RTLOGGRPFLAGS
{
    /** Enabled. */
    RTLOGGRPFLAGS_ENABLED      = 0x00000001,
    /** Level 1 logging. */
    RTLOGGRPFLAGS_LEVEL_1      = 0x00000002,
    /** Level 2 logging. */
    RTLOGGRPFLAGS_LEVEL_2      = 0x00000004,
    /** Level 3 logging. */
    RTLOGGRPFLAGS_LEVEL_3      = 0x00000008,
    /** Level 4 logging. */
    RTLOGGRPFLAGS_LEVEL_4      = 0x00000010,
    /** Level 5 logging. */
    RTLOGGRPFLAGS_LEVEL_5      = 0x00000020,
    /** Level 6 logging. */
    RTLOGGRPFLAGS_LEVEL_6      = 0x00000040,
    /** Flow logging. */
    RTLOGGRPFLAGS_FLOW         = 0x00000080,

    /** Lelik logging. */
    RTLOGGRPFLAGS_LELIK        = 0x00000100,
    /** Michael logging. */
    RTLOGGRPFLAGS_MICHAEL      = 0x00000200,
    /** dmik logging. */
    RTLOGGRPFLAGS_DMIK         = 0x00000400,
    /** sunlover logging. */
    RTLOGGRPFLAGS_SUNLOVER     = 0x00000800,
    /** Achim logging. */
    RTLOGGRPFLAGS_ACHIM        = 0x00001000,
    /** Sander logging. */
    RTLOGGRPFLAGS_SANDER       = 0x00002000,
    /** Klaus logging. */
    RTLOGGRPFLAGS_KLAUS        = 0x00004000,
    /** Frank logging. */
    RTLOGGRPFLAGS_FRANK        = 0x00008000,
    /** bird logging. */
    RTLOGGRPFLAGS_BIRD         = 0x00010000,
    /** aleksey logging. */
    RTLOGGRPFLAGS_ALEKSEY      = 0x00020000,
    /** dj logging. */
    RTLOGGRPFLAGS_DJ           = 0x00040000,
    /** NoName logging. */
    RTLOGGRPFLAGS_NONAME       = 0x00080000
} RTLOGGRPFLAGS;

/**
 * Logger destination type.
 */
typedef enum RTLOGDEST
{
    /** Log to file. */
    RTLOGDEST_FILE          = 0x00000001,
    /** Log to stdout. */
    RTLOGDEST_STDOUT        = 0x00000002,
    /** Log to stderr. */
    RTLOGDEST_STDERR        = 0x00000004,
    /** Log to debugger (win32 only). */
    RTLOGDEST_DEBUGGER      = 0x00000008,
    /** Log to com port. */
    RTLOGDEST_COM           = 0x00000010,
    /** Just a dummy flag to be used when no other flag applies. */
    RTLOGDEST_DUMMY         = 0x20000000,
    /** Log to a user defined output stream. */
    RTLOGDEST_USER          = 0x40000000
} RTLOGDEST;


RTDECL(void) RTLogPrintfEx(void *pvInstance, unsigned fFlags, unsigned iGroup, const char *pszFormat, ...);


#ifdef DOXYGEN_RUNNING
# define LOG_DISABLED
# define LOG_ENABLED
# define LOG_ENABLE_FLOW
#endif

/** @def LOG_DISABLED
 * Use this compile time define to disable all logging macros. It can
 * be overridden for each of the logging macros by the LOG_ENABLE*
 * compile time defines.
 */

/** @def LOG_ENABLED
 * Use this compile time define to enable logging when not in debug mode
 * or LOG_DISABLED is set.
 * This will enabled Log() only.
 */

/** @def LOG_ENABLE_FLOW
 * Use this compile time define to enable flow logging when not in
 * debug mode or LOG_DISABLED is defined.
 * This will enable LogFlow() only.
 */

/*
 * Determine whether logging is enabled and forcefully normalize the indicators.
 */
#if (defined(DEBUG) || defined(LOG_ENABLED)) && !defined(LOG_DISABLED)
# undef  LOG_DISABLED
# undef  LOG_ENABLED
# define LOG_ENABLED
#else
# undef  LOG_ENABLED
# undef  LOG_DISABLED
# define LOG_DISABLED
#endif


/** @def LOG_USE_C99
 * Governs the use of variadic macros.
 */
#ifndef LOG_USE_C99
# if defined(RT_ARCH_AMD64) || defined(RT_OS_DARWIN) || defined(RT_ARCH_SPARC) || defined(RT_ARCH_SPARC64)
#  define LOG_USE_C99
# endif
#endif


/** @def LogIt
 * Write to specific logger if group enabled.
 */
#ifdef LOG_ENABLED
# if defined(LOG_USE_C99)
#  define _LogRemoveParentheseis(...)                   __VA_ARGS__
#  define _LogIt(pvInst, fFlags, iGroup, ...)           RTLogLoggerEx((PRTLOGGER)pvInst, fFlags, iGroup, __VA_ARGS__)
#  define LogIt(pvInst, fFlags, iGroup, fmtargs)        _LogIt(pvInst, fFlags, iGroup, _LogRemoveParentheseis fmtargs)
#  define _LogItAlways(pvInst, fFlags, iGroup, ...)     RTLogLoggerEx((PRTLOGGER)pvInst, fFlags, ~0U, __VA_ARGS__)
#  define LogItAlways(pvInst, fFlags, iGroup, fmtargs)  _LogItAlways(pvInst, fFlags, iGroup, _LogRemoveParentheseis fmtargs)
        /** @todo invent a flag or something for skipping the group check so we can pass iGroup. LogItAlways. */
# else
#  define LogIt(pvInst, fFlags, iGroup, fmtargs) \
    do \
    { \
        register PRTLOGGER LogIt_pLogger = (PRTLOGGER)(pvInst) ? (PRTLOGGER)(pvInst) : RTLogDefaultInstance(); \
        if (LogIt_pLogger) \
        { \
            register unsigned LogIt_fFlags = LogIt_pLogger->afGroups[(unsigned)(iGroup) < LogIt_pLogger->cGroups ? (unsigned)(iGroup) : 0]; \
            if ((LogIt_fFlags & ((fFlags) | RTLOGGRPFLAGS_ENABLED)) == ((fFlags) | RTLOGGRPFLAGS_ENABLED)) \
                LogIt_pLogger->pfnLogger fmtargs; \
        } \
    } while (0)
#  define LogItAlways(pvInst, fFlags, iGroup, fmtargs) \
    do \
    { \
        register PRTLOGGER LogIt_pLogger = (PRTLOGGER)(pvInst) ? (PRTLOGGER)(pvInst) : RTLogDefaultInstance(); \
        if (LogIt_pLogger) \
            LogIt_pLogger->pfnLogger fmtargs; \
    } while (0)
# endif
#else
# define LogIt(pvInst, fFlags, iGroup, fmtargs)         do { } while (0)
# define LogItAlways(pvInst, fFlags, iGroup, fmtargs)   do { } while (0)
# if defined(LOG_USE_C99)
#  define _LogRemoveParentheseis(...)                   __VA_ARGS__
#  define _LogIt(pvInst, fFlags, iGroup, ...)           do { } while (0)
#  define _LogItAlways(pvInst, fFlags, iGroup, ...)     do { } while (0)
# endif
#endif


/** @def Log
 * Level 1 logging that works regardless of the group settings.
 */
#define LogAlways(a)    LogItAlways(LOG_INSTANCE, RTLOGGRPFLAGS_LEVEL_1, LOG_GROUP, a)

/** @def Log
 * Level 1 logging.
 */
#define Log(a)          LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_LEVEL_1, LOG_GROUP, a)

/** @def Log2
 * Level 2 logging.
 */
#define Log2(a)         LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_LEVEL_2,  LOG_GROUP, a)

/** @def Log3
 * Level 3 logging.
 */
#define Log3(a)         LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_LEVEL_3,  LOG_GROUP, a)

/** @def Log4
 * Level 4 logging.
 */
#define Log4(a)         LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_LEVEL_4,  LOG_GROUP, a)

/** @def Log5
 * Level 5 logging.
 */
#define Log5(a)         LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_LEVEL_5,  LOG_GROUP, a)

/** @def Log6
 * Level 6 logging.
 */
#define Log6(a)         LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_LEVEL_6,  LOG_GROUP, a)

/** @def LogFlow
 * Logging of execution flow.
 */
#define LogFlow(a)      LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_FLOW,     LOG_GROUP, a)

/** @def LogLelik
 *  lelik logging.
 */
#define LogLelik(a)     LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_LELIK,    LOG_GROUP, a)


/** @def LogMichael
 * michael logging.
 */
#define LogMichael(a)   LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_MICHAEL,  LOG_GROUP, a)

/** @def LogDmik
 * dmik logging.
 */
#define LogDmik(a)      LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_DMIK,     LOG_GROUP, a)

/** @def LogSunlover
 * sunlover logging.
 */
#define LogSunlover(a)  LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_SUNLOVER, LOG_GROUP, a)

/** @def LogAchim
 * Achim logging.
 */
#define LogAchim(a)     LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_ACHIM,    LOG_GROUP, a)

/** @def LogSander
 * Sander logging.
 */
#define LogSander(a)    LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_SANDER,   LOG_GROUP, a)

/** @def LogKlaus
 *  klaus logging.
 */
#define LogKlaus(a)     LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_KLAUS,    LOG_GROUP, a)

/** @def LogFrank
 *  frank logging.
 */
#define LogFrank(a)     LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_FRANK,    LOG_GROUP, a)

/** @def LogBird
 * bird logging.
 */
#define LogBird(a)      LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_BIRD,     LOG_GROUP, a)

/** @def LogAleksey
 * aleksey logging.
 */
#define LogAleksey(a)   LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_ALEKSEY,  LOG_GROUP, a)

/** @def LogDJ
 * dj logging.
 */
#define LogDJ(a)        LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_DJ,  LOG_GROUP, a)

/** @def LogNoName
 * NoName logging.
 */
#define LogNoName(a)    LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_NONAME,   LOG_GROUP, a)

/** @def LogWarning
 * The same as Log(), but prepents a <tt>"WARNING! "</tt> string to the message.
 *
 * @param   a   Custom log message in format <tt>("string\n" [, args])</tt>.
 */
#if defined(LOG_USE_C99)
# define LogWarning(a) \
    _LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_LEVEL_1, LOG_GROUP, "WARNING! %M", _LogRemoveParentheseis a )
#else
# define LogWarning(a) \
    do { Log(("WARNING! ")); Log(a); } while (0)
#endif

/** @def LogTrace
 * Macro to trace the execution flow: logs the file name, line number and
 * function name. Can be easily searched for in log files using the
 * ">>>>>" pattern (prepended to the beginning of each line).
 */
#define LogTrace() \
    LogFlow((">>>>> %s (%d): " LOG_FN_FMT "\n", __FILE__, __LINE__, __PRETTY_FUNCTION__))

/** @def LogTraceMsg
 * The same as LogTrace but logs a custom log message right after the trace line.
 *
 * @param   a   Custom log message in format <tt>("string\n" [, args])</tt>.
 */
#ifdef LOG_USE_C99
# define LogTraceMsg(a) \
    _LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_FLOW, LOG_GROUP, ">>>>> %s (%d): " LOG_FN_FMT ": %M", __FILE__, __LINE__, __PRETTY_FUNCTION__, _LogRemoveParentheseis a )
#else
# define LogTraceMsg(a) \
    do {  LogFlow((">>>>> %s (%d): " LOG_FN_FMT ": ", __FILE__, __LINE__, __PRETTY_FUNCTION__)); LogFlow(a); } while (0)
#endif

/** @def LogFunc
 * Level 1 logging inside C/C++ functions.
 *
 * Prepends the given log message with the function name followed by a
 * semicolon and space.
 *
 * @param   a   Log message in format <tt>("string\n" [, args])</tt>.
 */
#ifdef LOG_USE_C99
# define LogFunc(a) \
    _LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_LEVEL_1, LOG_GROUP, LOG_FN_FMT ": %M", __PRETTY_FUNCTION__, _LogRemoveParentheseis a )
#else
# define LogFunc(a) \
    do { Log((LOG_FN_FMT ": ", __PRETTY_FUNCTION__)); Log(a); } while (0)
#endif

/** @def LogThisFunc
 * The same as LogFunc but for class functions (methods): the resulting log
 * line is additionally prepended with a hex value of |this| pointer.
 *
 * @param   a   Log message in format <tt>("string\n" [, args])</tt>.
 */
#ifdef LOG_USE_C99
# define LogThisFunc(a) \
    _LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_LEVEL_1, LOG_GROUP, "{%p} " LOG_FN_FMT ": %M", this, __PRETTY_FUNCTION__, _LogRemoveParentheseis a )
#else
# define LogThisFunc(a) \
    do { Log(("{%p} " LOG_FN_FMT ": ", this, __PRETTY_FUNCTION__)); Log(a); } while (0)
#endif

/** @def LogFlowFunc
 * Macro to log the execution flow inside C/C++ functions.
 *
 * Prepends the given log message with the function name followed by
 * a semicolon and space.
 *
 * @param   a   Log message in format <tt>("string\n" [, args])</tt>.
 */
#ifdef LOG_USE_C99
# define LogFlowFunc(a) \
    _LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_FLOW, LOG_GROUP, LOG_FN_FMT ": %M", __PRETTY_FUNCTION__, _LogRemoveParentheseis a )
#else
# define LogFlowFunc(a) \
    do { LogFlow((LOG_FN_FMT ": ", __PRETTY_FUNCTION__)); LogFlow(a); } while (0)
#endif

/** @def LogWarningFunc
 * The same as LogWarning(), but prepents the log message with the function name.
 *
 * @param   a   Log message in format <tt>("string\n" [, args])</tt>.
 */
#ifdef LOG_USE_C99
# define LogWarningFunc(a) \
    _LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_LEVEL_1, LOG_GROUP, LOG_FN_FMT ": WARNING! %M", __PRETTY_FUNCTION__, _LogRemoveParentheseis a )
#else
# define LogWarningFunc(a) \
    do { Log((LOG_FN_FMT ": WARNING! ", __PRETTY_FUNCTION__)); Log(a); } while (0)
#endif

/** @def LogFlowThisFunc
 * The same as LogFlowFunc but for class functions (methods): the resulting log
 * line is additionally prepended with a hex value of |this| pointer.
 *
 * @param   a   Log message in format <tt>("string\n" [, args])</tt>.
 */
#ifdef LOG_USE_C99
# define LogFlowThisFunc(a) \
    _LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_FLOW, LOG_GROUP, "{%p} " LOG_FN_FMT ": %M", this, __PRETTY_FUNCTION__, _LogRemoveParentheseis a )
#else
# define LogFlowThisFunc(a) \
    do { LogFlow(("{%p} " LOG_FN_FMT ": ", this, __PRETTY_FUNCTION__)); LogFlow(a); } while (0)
#endif

/** @def LogWarningThisFunc
 * The same as LogWarningFunc() but for class functions (methods): the resulting
 * log line is additionally prepended with a hex value of |this| pointer.
 *
 * @param   a   Log message in format <tt>("string\n" [, args])</tt>.
 */
#ifdef LOG_USE_C99
# define LogWarningThisFunc(a) \
    _LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_LEVEL_1, LOG_GROUP, "{%p} " LOG_FN_FMT ": WARNING! %M", this, __PRETTY_FUNCTION__, _LogRemoveParentheseis a )
#else
# define LogWarningThisFunc(a) \
    do { Log(("{%p} " LOG_FN_FMT ": WARNING! ", this, __PRETTY_FUNCTION__)); Log(a); } while (0)
#endif

/** Shortcut to |LogFlowFunc ("ENTER\n")|, marks the beginnig of the function. */
#define LogFlowFuncEnter()      LogFlowFunc(("ENTER\n"))

/** Shortcut to |LogFlowFunc ("LEAVE\n")|, marks the end of the function. */
#define LogFlowFuncLeave()      LogFlowFunc(("LEAVE\n"))

/** Shortcut to |LogFlowThisFunc ("ENTER\n")|, marks the beginnig of the function. */
#define LogFlowThisFuncEnter()  LogFlowThisFunc(("ENTER\n"))

/** Shortcut to |LogFlowThisFunc ("LEAVE\n")|, marks the end of the function. */
#define LogFlowThisFuncLeave()  LogFlowThisFunc(("LEAVE\n"))

/** @def LogObjRefCnt
 * Helper macro to print the current reference count of the given COM object
 * to the log file.
 *
 * @param pObj  Pointer to the object in question (must be a pointer to an
 *              IUnknown subclass or simply define COM-style AddRef() and
 *              Release() methods)
 *
 * @note Use it only for temporary debugging. It leaves dummy code even if
 *       logging is disabled.
 */
#define LogObjRefCnt(pObj) \
    do { \
        int refc = (pObj)->AddRef(); \
        LogFlow((#pObj "{%p}.refCnt=%d\n", (pObj), refc - 1)); \
        (pObj)->Release(); \
    } while (0)


/** @def LogIsItEnabled
 * Checks whether the specified logging group is enabled or not.
 */
#ifdef LOG_ENABLED
# define LogIsItEnabled(pvInst, fFlags, iGroup) \
    LogIsItEnabledInternal((pvInst), (unsigned)(iGroup), (unsigned)(fFlags))
#else
# define LogIsItEnabled(pvInst, fFlags, iGroup) (false)
#endif

/** @def LogIsEnabled
 * Checks whether level 1 logging is enabled.
 */
#define LogIsEnabled()      LogIsItEnabled(LOG_INSTANCE, RTLOGGRPFLAGS_LEVEL_1, LOG_GROUP)

/** @def LogIs2Enabled
 * Checks whether level 2 logging is enabled.
 */
#define LogIs2Enabled()     LogIsItEnabled(LOG_INSTANCE, RTLOGGRPFLAGS_LEVEL_2, LOG_GROUP)

/** @def LogIs3Enabled
 * Checks whether level 3 logging is enabled.
 */
#define LogIs3Enabled()     LogIsItEnabled(LOG_INSTANCE, RTLOGGRPFLAGS_LEVEL_3, LOG_GROUP)

/** @def LogIs4Enabled
 * Checks whether level 4 logging is enabled.
 */
#define LogIs4Enabled()     LogIsItEnabled(LOG_INSTANCE, RTLOGGRPFLAGS_LEVEL_4, LOG_GROUP)

/** @def LogIs5Enabled
 * Checks whether level 5 logging is enabled.
 */
#define LogIs5Enabled()     LogIsItEnabled(LOG_INSTANCE, RTLOGGRPFLAGS_LEVEL_5, LOG_GROUP)

/** @def LogIs6Enabled
 * Checks whether level 6 logging is enabled.
 */
#define LogIs6Enabled()     LogIsItEnabled(LOG_INSTANCE, RTLOGGRPFLAGS_LEVEL_6, LOG_GROUP)

/** @def LogIsFlowEnabled
 * Checks whether execution flow logging is enabled.
 */
#define LogIsFlowEnabled()  LogIsItEnabled(LOG_INSTANCE, RTLOGGRPFLAGS_FLOW, LOG_GROUP)


/** @name Passing Function Call Position When Logging.
 *
 * This is a little bit ugly as we have to omit the comma before the
 * position parameters so that we don't inccur any overhead in non-logging
 * builds (!defined(LOG_ENABLED).
 *
 * @{  */
/** Source position for passing to a function call. */
#ifdef LOG_ENABLED
# define RTLOG_COMMA_SRC_POS        , __FILE__, __LINE__, __PRETTY_FUNCTION__
#else
# define RTLOG_COMMA_SRC_POS        RT_NOTHING
#endif
/** Source position declaration. */
#ifdef LOG_ENABLED
# define RTLOG_COMMA_SRC_POS_DECL   , const char *pszFile, unsigned iLine, const char *pszFunction
#else
# define RTLOG_COMMA_SRC_POS_DECL   RT_NOTHING
#endif
/** Source position arguments. */
#ifdef LOG_ENABLED
# define RTLOG_COMMA_SRC_POS_ARGS   , pszFile, iLine, pszFunction
#else
# define RTLOG_COMMA_SRC_POS_ARGS   RT_NOTHING
#endif
/** Applies NOREF() to the source position arguments. */
#ifdef LOG_ENABLED
# define RTLOG_SRC_POS_NOREF()      do { NOREF(pszFile); NOREF(iLine); NOREF(pszFunction); } while (0)
#else
# define RTLOG_SRC_POS_NOREF()      do { } while (0)
#endif
/** @}  */



/** @name Release Logging
 * @{
 */

#ifdef DOXYGEN_RUNNING
# define RTLOG_REL_DISABLED
# define RTLOG_REL_ENABLED
#endif

/** @def RTLOG_REL_DISABLED
 * Use this compile time define to disable all release logging
 * macros.
 */

/** @def RTLOG_REL_ENABLED
 * Use this compile time define to override RTLOG_REL_DISABLE.
 */

/*
 * Determine whether release logging is enabled and forcefully normalize the indicators.
 */
#if !defined(RTLOG_REL_DISABLED) || defined(RTLOG_REL_ENABLED)
# undef  RTLOG_REL_DISABLED
# undef  RTLOG_REL_ENABLED
# define RTLOG_REL_ENABLED
#else
# undef  RTLOG_REL_ENABLED
# undef  RTLOG_REL_DISABLED
# define RTLOG_REL_DISABLED
#endif


/** @def LogIt
 * Write to specific logger if group enabled.
 */
#ifdef RTLOG_REL_ENABLED
# if defined(LOG_USE_C99)
#  define _LogRelRemoveParentheseis(...)                __VA_ARGS__
#  define _LogRelIt(pvInst, fFlags, iGroup, ...) \
    do \
    { \
        PRTLOGGER LogRelIt_pLogger = (PRTLOGGER)(pvInst) ? (PRTLOGGER)(pvInst) : RTLogRelDefaultInstance(); \
        if (LogRelIt_pLogger) \
            RTLogLoggerEx(LogRelIt_pLogger, fFlags, iGroup, __VA_ARGS__); \
        _LogIt(LOG_INSTANCE, fFlags, iGroup, __VA_ARGS__); \
    } while (0)
#  define LogRelIt(pvInst, fFlags, iGroup, fmtargs)     _LogRelIt(pvInst, fFlags, iGroup, _LogRelRemoveParentheseis fmtargs)
# else
#  define LogRelIt(pvInst, fFlags, iGroup, fmtargs) \
   do \
   { \
       PRTLOGGER LogRelIt_pLogger = (PRTLOGGER)(pvInst) ? (PRTLOGGER)(pvInst) : RTLogRelDefaultInstance(); \
       if (LogRelIt_pLogger) \
       { \
           unsigned LogIt_fFlags = LogRelIt_pLogger->afGroups[(unsigned)(iGroup) < LogRelIt_pLogger->cGroups ? (unsigned)(iGroup) : 0]; \
           if ((LogIt_fFlags & ((fFlags) | RTLOGGRPFLAGS_ENABLED)) == ((fFlags) | RTLOGGRPFLAGS_ENABLED)) \
               LogRelIt_pLogger->pfnLogger fmtargs; \
       } \
       LogIt(LOG_INSTANCE, fFlags, iGroup, fmtargs); \
  } while (0)
# endif
#else   /* !RTLOG_REL_ENABLED */
# define LogRelIt(pvInst, fFlags, iGroup, fmtargs)      do { } while (0)
# if defined(LOG_USE_C99)
#  define _LogRelRemoveParentheseis(...)                __VA_ARGS__
#  define _LogRelIt(pvInst, fFlags, iGroup, ...)        do { } while (0)
# endif
#endif  /* !RTLOG_REL_ENABLED */


/** @def LogRel
 * Level 1 logging.
 */
#define LogRel(a)          LogRelIt(LOG_REL_INSTANCE, RTLOGGRPFLAGS_LEVEL_1, LOG_GROUP, a)

/** @def LogRel2
 * Level 2 logging.
 */
#define LogRel2(a)         LogRelIt(LOG_REL_INSTANCE, RTLOGGRPFLAGS_LEVEL_2,  LOG_GROUP, a)

/** @def LogRel3
 * Level 3 logging.
 */
#define LogRel3(a)         LogRelIt(LOG_REL_INSTANCE, RTLOGGRPFLAGS_LEVEL_3,  LOG_GROUP, a)

/** @def LogRel4
 * Level 4 logging.
 */
#define LogRel4(a)         LogRelIt(LOG_REL_INSTANCE, RTLOGGRPFLAGS_LEVEL_4,  LOG_GROUP, a)

/** @def LogRel5
 * Level 5 logging.
 */
#define LogRel5(a)         LogRelIt(LOG_REL_INSTANCE, RTLOGGRPFLAGS_LEVEL_5,  LOG_GROUP, a)

/** @def LogRel6
 * Level 6 logging.
 */
#define LogRel6(a)         LogRelIt(LOG_REL_INSTANCE, RTLOGGRPFLAGS_LEVEL_6,  LOG_GROUP, a)

/** @def LogRelFlow
 * Logging of execution flow.
 */
#define LogRelFlow(a)      LogRelIt(LOG_REL_INSTANCE, RTLOGGRPFLAGS_FLOW,     LOG_GROUP, a)

/** @def LogRelFunc
 * Release logging.  Prepends the given log message with the function name
 * followed by a semicolon and space.
 */
#ifdef LOG_USE_C99
# define LogRelFunc(a) \
    _LogRelIt(LOG_REL_INSTANCE, RTLOGGRPFLAGS_LEVEL_1, LOG_GROUP, LOG_FN_FMT ": %M", __PRETTY_FUNCTION__, _LogRemoveParentheseis a )
# define LogFunc(a) \
           _LogIt(LOG_INSTANCE, RTLOGGRPFLAGS_LEVEL_1, LOG_GROUP, LOG_FN_FMT ": %M", __PRETTY_FUNCTION__, _LogRemoveParentheseis a )
#else
# define LogRelFunc(a) \
    do { LogRel((LOG_FN_FMT ": ", __PRETTY_FUNCTION__)); LogRel(a); } while (0)
#endif

/** @def LogRelThisFunc
 * The same as LogRelFunc but for class functions (methods): the resulting log
 * line is additionally prepended with a hex value of |this| pointer.
 */
#ifdef LOG_USE_C99
# define LogRelThisFunc(a) \
    _LogRelIt(LOG_REL_INSTANCE, RTLOGGRPFLAGS_LEVEL_1, LOG_GROUP, "{%p} " LOG_FN_FMT ": %M", this, __PRETTY_FUNCTION__, _LogRemoveParentheseis a )
#else
# define LogRelThisFunc(a) \
    do { LogRel(("{%p} " LOG_FN_FMT ": ", this, __PRETTY_FUNCTION__)); LogRel(a); } while (0)
#endif

/** @def LogRelFlowFunc
 * Release logging.  Macro to log the execution flow inside C/C++ functions.
 *
 * Prepends the given log message with the function name followed by
 * a semicolon and space.
 *
 * @param   a   Log message in format <tt>("string\n" [, args])</tt>.
 */
#ifdef LOG_USE_C99
# define LogRelFlowFunc(a) \
    _LogRelIt(LOG_REL_INSTANCE, RTLOGGRPFLAGS_FLOW, LOG_GROUP, LOG_FN_FMT ": %M", __PRETTY_FUNCTION__, _LogRemoveParentheseis a )
#else
# define LogRelFlowFunc(a) \
    do { LogRelFlow((LOG_FN_FMT ": ", __PRETTY_FUNCTION__)); LogRelFlow(a); } while (0)
#endif

/** @def LogRelLelik
 *  lelik logging.
 */
#define LogRelLelik(a)     LogRelIt(LOG_REL_INSTANCE, RTLOGGRPFLAGS_LELIK,    LOG_GROUP, a)

/** @def LogRelMichael
 * michael logging.
 */
#define LogRelMichael(a)   LogRelIt(LOG_REL_INSTANCE, RTLOGGRPFLAGS_MICHAEL,  LOG_GROUP, a)

/** @def LogRelDmik
 * dmik logging.
 */
#define LogRelDmik(a)      LogRelIt(LOG_REL_INSTANCE, RTLOGGRPFLAGS_DMIK,     LOG_GROUP, a)

/** @def LogRelSunlover
 * sunlover logging.
 */
#define LogRelSunlover(a)  LogRelIt(LOG_REL_INSTANCE, RTLOGGRPFLAGS_SUNLOVER, LOG_GROUP, a)

/** @def LogRelAchim
 * Achim logging.
 */
#define LogRelAchim(a)     LogRelIt(LOG_REL_INSTANCE, RTLOGGRPFLAGS_ACHIM,    LOG_GROUP, a)

/** @def LogRelSander
 * Sander logging.
 */
#define LogRelSander(a)    LogRelIt(LOG_REL_INSTANCE, RTLOGGRPFLAGS_SANDER,   LOG_GROUP, a)

/** @def LogRelKlaus
 *  klaus logging.
 */
#define LogRelKlaus(a)     LogRelIt(LOG_REL_INSTANCE, RTLOGGRPFLAGS_KLAUS,    LOG_GROUP, a)

/** @def LogRelFrank
 *  frank logging.
 */
#define LogRelFrank(a)     LogRelIt(LOG_REL_INSTANCE, RTLOGGRPFLAGS_FRANK,    LOG_GROUP, a)

/** @def LogRelBird
 * bird logging.
 */
#define LogRelBird(a)      LogRelIt(LOG_REL_INSTANCE, RTLOGGRPFLAGS_BIRD,     LOG_GROUP, a)

/** @def LogRelNoName
 * NoName logging.
 */
#define LogRelNoName(a)    LogRelIt(LOG_REL_INSTANCE, RTLOGGRPFLAGS_NONAME,   LOG_GROUP, a)


/** @def LogRelIsItEnabled
 * Checks whether the specified logging group is enabled or not.
 */
#define LogRelIsItEnabled(pvInst, fFlags, iGroup) \
    LogRelIsItEnabledInternal((pvInst), (unsigned)(iGroup), (unsigned)(fFlags))

/** @def LogRelIsEnabled
 * Checks whether level 1 logging is enabled.
 */
#define LogRelIsEnabled()      LogRelIsItEnabled(LOG_REL_INSTANCE, RTLOGGRPFLAGS_LEVEL_1, LOG_GROUP)

/** @def LogRelIs2Enabled
 * Checks whether level 2 logging is enabled.
 */
#define LogRelIs2Enabled()     LogRelIsItEnabled(LOG_REL_INSTANCE, RTLOGGRPFLAGS_LEVEL_2, LOG_GROUP)

/** @def LogRelIs3Enabled
 * Checks whether level 3 logging is enabled.
 */
#define LogRelIs3Enabled()     LogRelIsItEnabled(LOG_REL_INSTANCE, RTLOGGRPFLAGS_LEVEL_3, LOG_GROUP)

/** @def LogRelIs4Enabled
 * Checks whether level 4 logging is enabled.
 */
#define LogRelIs4Enabled()     LogRelIsItEnabled(LOG_REL_INSTANCE, RTLOGGRPFLAGS_LEVEL_4, LOG_GROUP)

/** @def LogRelIs5Enabled
 * Checks whether level 5 logging is enabled.
 */
#define LogRelIs5Enabled()     LogRelIsItEnabled(LOG_REL_INSTANCE, RTLOGGRPFLAGS_LEVEL_5, LOG_GROUP)

/** @def LogRelIs6Enabled
 * Checks whether level 6 logging is enabled.
 */
#define LogRelIs6Enabled()     LogRelIsItEnabled(LOG_REL_INSTANCE, RTLOGGRPFLAGS_LEVEL_6, LOG_GROUP)

/** @def LogRelIsFlowEnabled
 * Checks whether execution flow logging is enabled.
 */
#define LogRelIsFlowEnabled()  LogRelIsItEnabled(LOG_REL_INSTANCE, RTLOGGRPFLAGS_FLOW, LOG_GROUP)


#ifndef IN_RC
/**
 * Sets the default release logger instance.
 *
 * @returns The old default instance.
 * @param   pLogger     The new default release logger instance.
 */
RTDECL(PRTLOGGER) RTLogRelSetDefaultInstance(PRTLOGGER pLogger);
#endif /* !IN_RC */

/**
 * Gets the default release logger instance.
 *
 * @returns Pointer to default release logger instance.
 * @returns NULL if no default release logger instance available.
 */
RTDECL(PRTLOGGER) RTLogRelDefaultInstance(void);

/** Internal worker function.
 * Don't call directly, use the LogRelIsItEnabled macro!
 */
DECLINLINE(bool) LogRelIsItEnabledInternal(void *pvInst, unsigned iGroup, unsigned fFlags)
{
    register PRTLOGGER pLogger = (PRTLOGGER)pvInst ? (PRTLOGGER)pvInst : RTLogRelDefaultInstance();
    if (pLogger)
    {
        register unsigned fGrpFlags = pLogger->afGroups[(unsigned)iGroup < pLogger->cGroups ? (unsigned)iGroup : 0];
        if ((fGrpFlags & (fFlags | RTLOGGRPFLAGS_ENABLED)) == (fFlags | RTLOGGRPFLAGS_ENABLED))
            return true;
    }
    return false;
}

/**
 * Write to a logger instance, defaulting to the release one.
 *
 * This function will check whether the instance, group and flags makes up a
 * logging kind which is currently enabled before writing anything to the log.
 *
 * @param   pLogger     Pointer to logger instance.
 * @param   fFlags      The logging flags.
 * @param   iGroup      The group.
 *                      The value ~0U is reserved for compatibility with RTLogLogger[V] and is
 *                      only for internal usage!
 * @param   pszFormat   Format string.
 * @param   ...         Format arguments.
 * @remark  This is a worker function for LogRelIt.
 */
RTDECL(void) RTLogRelLogger(PRTLOGGER pLogger, unsigned fFlags, unsigned iGroup, const char *pszFormat, ...);

/**
 * Write to a logger instance, defaulting to the release one.
 *
 * This function will check whether the instance, group and flags makes up a
 * logging kind which is currently enabled before writing anything to the log.
 *
 * @param   pLogger     Pointer to logger instance. If NULL the default release instance is attempted.
 * @param   fFlags      The logging flags.
 * @param   iGroup      The group.
 *                      The value ~0U is reserved for compatibility with RTLogLogger[V] and is
 *                      only for internal usage!
 * @param   pszFormat   Format string.
 * @param   args        Format arguments.
 */
RTDECL(void) RTLogRelLoggerV(PRTLOGGER pLogger, unsigned fFlags, unsigned iGroup, const char *pszFormat, va_list args);

/**
 * printf like function for writing to the default release log.
 *
 * @param   pszFormat   Printf like format string.
 * @param   ...         Optional arguments as specified in pszFormat.
 *
 * @remark The API doesn't support formatting of floating point numbers at the moment.
 */
RTDECL(void) RTLogRelPrintf(const char *pszFormat, ...);

/**
 * vprintf like function for writing to the default release log.
 *
 * @param   pszFormat   Printf like format string.
 * @param   args        Optional arguments as specified in pszFormat.
 *
 * @remark The API doesn't support formatting of floating point numbers at the moment.
 */
RTDECL(void) RTLogRelPrintfV(const char *pszFormat, va_list args);

/**
 * Changes the buffering setting of the default release logger.
 *
 * This can be used for optimizing longish logging sequences.
 *
 * @returns The old state.
 * @param   fBuffered       The new state.
 */
RTDECL(bool) RTLogRelSetBuffering(bool fBuffered);

/** @} */



/** @name COM port logging
 * {
 */

#ifdef DOXYGEN_RUNNING
# define LOG_TO_COM
# define LOG_NO_COM
#endif

/** @def LOG_TO_COM
 * Redirects the normal logging macros to the serial versions.
 */

/** @def LOG_NO_COM
 * Disables all LogCom* macros.
 */

/** @def LogCom
 * Generic logging to serial port.
 */
#if defined(LOG_ENABLED) && !defined(LOG_NO_COM)
# define LogCom(a) RTLogComPrintf a
#else
# define LogCom(a) do { } while (0)
#endif

/** @def LogComFlow
 * Logging to serial port of execution flow.
 */
#if defined(LOG_ENABLED) && defined(LOG_ENABLE_FLOW) && !defined(LOG_NO_COM)
# define LogComFlow(a) RTLogComPrintf a
#else
# define LogComFlow(a) do { } while (0)
#endif

#ifdef LOG_TO_COM
# undef Log
# define Log(a)             LogCom(a)
# undef LogFlow
# define LogFlow(a)         LogComFlow(a)
#endif

/** @} */


/** @name Backdoor Logging
 * @{
 */

#ifdef DOXYGEN_RUNNING
# define LOG_TO_BACKDOOR
# define LOG_NO_BACKDOOR
#endif

/** @def LOG_TO_BACKDOOR
 * Redirects the normal logging macros to the backdoor versions.
 */

/** @def LOG_NO_BACKDOOR
 * Disables all LogBackdoor* macros.
 */

/** @def LogBackdoor
 * Generic logging to the VBox backdoor via port I/O.
 */
#if defined(LOG_ENABLED) && !defined(LOG_NO_BACKDOOR)
# define LogBackdoor(a)     RTLogBackdoorPrintf a
#else
# define LogBackdoor(a)     do { } while (0)
#endif

/** @def LogBackdoorFlow
 * Logging of execution flow messages to the backdoor I/O port.
 */
#if defined(LOG_ENABLED) && !defined(LOG_NO_BACKDOOR)
# define LogBackdoorFlow(a) RTLogBackdoorPrintf a
#else
# define LogBackdoorFlow(a) do { } while (0)
#endif

/** @def LogRelBackdoor
 * Release logging to the VBox backdoor via port I/O.
 */
#if !defined(LOG_NO_BACKDOOR)
# define LogRelBackdoor(a)  RTLogBackdoorPrintf a
#else
# define LogRelBackdoor(a)  do { } while (0)
#endif

#ifdef LOG_TO_BACKDOOR
# undef Log
# define Log(a)         LogBackdoor(a)
# undef LogFlow
# define LogFlow(a)     LogBackdoorFlow(a)
# undef LogRel
# define LogRel(a)      LogRelBackdoor(a)
# if defined(LOG_USE_C99)
#  undef _LogIt
#  define _LogIt(pvInst, fFlags, iGroup, ...)  LogBackdoor((__VA_ARGS__))
# endif
#endif

/** @} */



/**
 * Gets the default logger instance, creating it if necessary.
 *
 * @returns Pointer to default logger instance.
 * @returns NULL if no default logger instance available.
 */
RTDECL(PRTLOGGER)   RTLogDefaultInstance(void);

/**
 * Gets the default logger instance.
 *
 * @returns Pointer to default logger instance.
 * @returns NULL if no default logger instance available.
 */
RTDECL(PRTLOGGER)   RTLogGetDefaultInstance(void);

#ifndef IN_RC
/**
 * Sets the default logger instance.
 *
 * @returns The old default instance.
 * @param   pLogger     The new default logger instance.
 */
RTDECL(PRTLOGGER) RTLogSetDefaultInstance(PRTLOGGER pLogger);
#endif /* !IN_RC */

#ifdef IN_RING0
/**
 * Changes the default logger instance for the current thread.
 *
 * @returns IPRT status code.
 * @param   pLogger     The logger instance. Pass NULL for deregistration.
 * @param   uKey        Associated key for cleanup purposes. If pLogger is NULL,
 *                      all instances with this key will be deregistered. So in
 *                      order to only deregister the instance associated with the
 *                      current thread use 0.
 */
RTDECL(int) RTLogSetDefaultInstanceThread(PRTLOGGER pLogger, uintptr_t uKey);
#endif /* IN_RING0 */


#ifdef LOG_ENABLED
/** Internal worker function.
 * Don't call directly, use the LogIsItEnabled macro!
 */
DECLINLINE(bool) LogIsItEnabledInternal(void *pvInst, unsigned iGroup, unsigned fFlags)
{
    register PRTLOGGER pLogger = (PRTLOGGER)pvInst ? (PRTLOGGER)pvInst : RTLogDefaultInstance();
    if (pLogger)
    {
        register unsigned fGrpFlags = pLogger->afGroups[(unsigned)iGroup < pLogger->cGroups ? (unsigned)iGroup : 0];
        if ((fGrpFlags & (fFlags | RTLOGGRPFLAGS_ENABLED)) == (fFlags | RTLOGGRPFLAGS_ENABLED))
            return true;
    }
    return false;
}
#endif


#ifndef IN_RC
/**
 * Creates the default logger instance for a iprt users.
 *
 * Any user of the logging features will need to implement
 * this or use the generic dummy.
 *
 * @returns Pointer to the logger instance.
 */
RTDECL(PRTLOGGER) RTLogDefaultInit(void);

/**
 * Create a logger instance.
 *
 * @returns iprt status code.
 *
 * @param   ppLogger            Where to store the logger instance.
 * @param   fFlags              Logger instance flags, a combination of the RTLOGFLAGS_* values.
 * @param   pszGroupSettings    The initial group settings.
 * @param   pszEnvVarBase       Base name for the environment variables for this instance.
 * @param   cGroups             Number of groups in the array.
 * @param   papszGroups         Pointer to array of groups. This must stick around for the life of the
 *                              logger instance.
 * @param   fDestFlags          The destination flags. RTLOGDEST_FILE is ORed if pszFilenameFmt specified.
 * @param   pszFilenameFmt      Log filename format string. Standard RTStrFormat().
 * @param   ...                 Format arguments.
 */
RTDECL(int) RTLogCreate(PRTLOGGER *ppLogger, uint32_t fFlags, const char *pszGroupSettings,
                        const char *pszEnvVarBase, unsigned cGroups, const char * const * papszGroups,
                        uint32_t fDestFlags, const char *pszFilenameFmt, ...);

/**
 * Create a logger instance.
 *
 * @returns iprt status code.
 *
 * @param   ppLogger            Where to store the logger instance.
 * @param   fFlags              Logger instance flags, a combination of the RTLOGFLAGS_* values.
 * @param   pszGroupSettings    The initial group settings.
 * @param   pszEnvVarBase       Base name for the environment variables for this instance.
 * @param   cGroups             Number of groups in the array.
 * @param   papszGroups         Pointer to array of groups. This must stick around for the life of the
 *                              logger instance.
 * @param   fDestFlags          The destination flags. RTLOGDEST_FILE is ORed if pszFilenameFmt specified.
 * @param   pszErrorMsg         A buffer which is filled with an error message if something fails. May be NULL.
 * @param   cchErrorMsg         The size of the error message buffer.
 * @param   pszFilenameFmt      Log filename format string. Standard RTStrFormat().
 * @param   ...                 Format arguments.
 */
RTDECL(int) RTLogCreateEx(PRTLOGGER *ppLogger, uint32_t fFlags, const char *pszGroupSettings,
                          const char *pszEnvVarBase, unsigned cGroups, const char * const * papszGroups,
                          uint32_t fDestFlags, char *pszErrorMsg, size_t cchErrorMsg, const char *pszFilenameFmt, ...);

/**
 * Create a logger instance.
 *
 * @returns iprt status code.
 *
 * @param   ppLogger            Where to store the logger instance.
 * @param   fFlags              Logger instance flags, a combination of the RTLOGFLAGS_* values.
 * @param   pszGroupSettings    The initial group settings.
 * @param   pszEnvVarBase       Base name for the environment variables for this instance.
 * @param   cGroups             Number of groups in the array.
 * @param   papszGroups         Pointer to array of groups. This must stick around for the life of the
 *                              logger instance.
 * @param   fDestFlags          The destination flags. RTLOGDEST_FILE is ORed if pszFilenameFmt specified.
 * @param   pszErrorMsg         A buffer which is filled with an error message if something fails. May be NULL.
 * @param   cchErrorMsg         The size of the error message buffer.
 * @param   pszFilenameFmt      Log filename format string. Standard RTStrFormat().
 * @param   args                Format arguments.
 */
RTDECL(int) RTLogCreateExV(PRTLOGGER *ppLogger, uint32_t fFlags, const char *pszGroupSettings,
                           const char *pszEnvVarBase, unsigned cGroups, const char * const * papszGroups,
                           uint32_t fDestFlags, char *pszErrorMsg, size_t cchErrorMsg, const char *pszFilenameFmt, va_list args);

/**
 * Create a logger instance for singled threaded ring-0 usage.
 *
 * @returns iprt status code.
 *
 * @param   pLogger             Where to create the logger instance.
 * @param   cbLogger            The amount of memory available for the logger instance.
 * @param   pfnLogger           Pointer to logger wrapper function for the clone.
 * @param   pfnFlush            Pointer to flush function for the clone.
 * @param   fFlags              Logger instance flags for the clone, a combination of the RTLOGFLAGS_* values.
 * @param   fDestFlags          The destination flags.
 */
RTDECL(int) RTLogCreateForR0(PRTLOGGER pLogger, size_t cbLogger, PFNRTLOGGER pfnLogger, PFNRTLOGFLUSH pfnFlush, uint32_t fFlags, uint32_t fDestFlags);

/**
 * Destroys a logger instance.
 *
 * The instance is flushed and all output destinations closed (where applicable).
 *
 * @returns iprt status code.
 * @param   pLogger             The logger instance which close destroyed. NULL is fine.
 */
RTDECL(int) RTLogDestroy(PRTLOGGER pLogger);

/**
 * Create a logger instance clone for RC usage.
 *
 * @returns iprt status code.
 *
 * @param   pLogger             The logger instance to be cloned.
 * @param   pLoggerRC           Where to create the RC logger instance.
 * @param   cbLoggerRC          Amount of memory allocated to for the RC logger
 *                              instance clone.
 * @param   pfnLoggerRCPtr      Pointer to logger wrapper function for this
 *                              instance (RC Ptr).
 * @param   pfnFlushRCPtr       Pointer to flush function (RC Ptr).
 * @param   fFlags              Logger instance flags, a combination of the RTLOGFLAGS_* values.
 */
RTDECL(int) RTLogCloneRC(PRTLOGGER pLogger, PRTLOGGERRC pLoggerRC, size_t cbLoggerRC,
                         RTRCPTR pfnLoggerRCPtr, RTRCPTR pfnFlushRCPtr, uint32_t fFlags);

/**
 * Flushes a RC logger instance to a R3 logger.
 *
 * @returns iprt status code.
 * @param   pLogger     The R3 logger instance to flush pLoggerRC to. If NULL
 *                      the default logger is used.
 * @param   pLoggerRC   The RC logger instance to flush.
 */
RTDECL(void) RTLogFlushRC(PRTLOGGER pLogger, PRTLOGGERRC pLoggerRC);

/**
 * Flushes the buffer in one logger instance onto another logger.
 *
 * @returns iprt status code.
 *
 * @param   pSrcLogger   The logger instance to flush.
 * @param   pDstLogger   The logger instance to flush onto.
 *                       If NULL the default logger will be used.
 */
RTDECL(void) RTLogFlushToLogger(PRTLOGGER pSrcLogger, PRTLOGGER pDstLogger);

/**
 * Sets the custom prefix callback.
 *
 * @returns IPRT status code.
 * @param   pLogger     The logger instance.
 * @param   pfnCallback The callback.
 * @param   pvUser      The user argument for the callback.
 *  */
RTDECL(int) RTLogSetCustomPrefixCallback(PRTLOGGER pLogger, PFNRTLOGPREFIX pfnCallback, void *pvUser);

/**
 * Copies the group settings and flags from logger instance to another.
 *
 * @returns IPRT status code.
 * @param   pDstLogger      The destination logger instance.
 * @param   pSrcLogger      The source logger instance. If NULL the default one is used.
 * @param   fFlagsOr        OR mask for the flags.
 * @param   fFlagsAnd       AND mask for the flags.
 */
RTDECL(int) RTLogCopyGroupsAndFlags(PRTLOGGER pDstLogger, PCRTLOGGER pSrcLogger, unsigned fFlagsOr, unsigned fFlagsAnd);

/**
 * Get the current log group settings as a string.
 *
 * @returns VINF_SUCCESS or VERR_BUFFER_OVERFLOW.
 * @param   pLogger             Logger instance (NULL for default logger).
 * @param   pszBuf              The output buffer.
 * @param   cchBuf              The size of the output buffer. Must be greater
 *                              than zero.
 */
RTDECL(int) RTLogGetGroupSettings(PRTLOGGER pLogger, char *pszBuf, size_t cchBuf);

/**
 * Updates the group settings for the logger instance using the specified
 * specification string.
 *
 * @returns iprt status code.
 *          Failures can safely be ignored.
 * @param   pLogger     Logger instance (NULL for default logger).
 * @param   pszVar      Value to parse.
 */
RTDECL(int) RTLogGroupSettings(PRTLOGGER pLogger, const char *pszVar);
#endif /* !IN_RC */

/**
 * Updates the flags for the logger instance using the specified
 * specification string.
 *
 * @returns iprt status code.
 *          Failures can safely be ignored.
 * @param   pLogger     Logger instance (NULL for default logger).
 * @param   pszVar      Value to parse.
 */
RTDECL(int) RTLogFlags(PRTLOGGER pLogger, const char *pszVar);

/**
 * Changes the buffering setting of the specified logger.
 *
 * This can be used for optimizing longish logging sequences.
 *
 * @returns The old state.
 * @param   pLogger         The logger instance (NULL is an alias for the
 *                          default logger).
 * @param   fBuffered       The new state.
 */
RTDECL(bool) RTLogSetBuffering(PRTLOGGER pLogger, bool fBuffered);

#ifndef IN_RC
/**
 * Get the current log flags as a string.
 *
 * @returns VINF_SUCCESS or VERR_BUFFER_OVERFLOW.
 * @param   pLogger             Logger instance (NULL for default logger).
 * @param   pszBuf              The output buffer.
 * @param   cchBuf              The size of the output buffer. Must be greater
 *                              than zero.
 */
RTDECL(int) RTLogGetFlags(PRTLOGGER pLogger, char *pszBuf, size_t cchBuf);

/**
 * Updates the logger destination using the specified string.
 *
 * @returns VINF_SUCCESS or VERR_BUFFER_OVERFLOW.
 * @param   pLogger             Logger instance (NULL for default logger).
 * @param   pszVar              The value to parse.
 */
RTDECL(int) RTLogDestinations(PRTLOGGER pLogger, char const *pszVar);

/**
 * Get the current log destinations as a string.
 *
 * @returns VINF_SUCCESS or VERR_BUFFER_OVERFLOW.
 * @param   pLogger             Logger instance (NULL for default logger).
 * @param   pszBuf              The output buffer.
 * @param   cchBuf              The size of the output buffer. Must be greater
 *                              than 0.
 */
RTDECL(int) RTLogGetDestinations(PRTLOGGER pLogger, char *pszBuf, size_t cchBuf);
#endif /* !IN_RC */

/**
 * Flushes the specified logger.
 *
 * @param   pLogger     The logger instance to flush.
 *                      If NULL the default instance is used. The default instance
 *                      will not be initialized by this call.
 */
RTDECL(void) RTLogFlush(PRTLOGGER pLogger);

/**
 * Write to a logger instance.
 *
 * @param   pLogger     Pointer to logger instance.
 * @param   pvCallerRet Ignored.
 * @param   pszFormat   Format string.
 * @param   ...         Format arguments.
 */
RTDECL(void) RTLogLogger(PRTLOGGER pLogger, void *pvCallerRet, const char *pszFormat, ...);

/**
 * Write to a logger instance.
 *
 * @param   pLogger     Pointer to logger instance.
 * @param   pszFormat   Format string.
 * @param   args        Format arguments.
 */
RTDECL(void) RTLogLoggerV(PRTLOGGER pLogger, const char *pszFormat, va_list args);

/**
 * Write to a logger instance.
 *
 * This function will check whether the instance, group and flags makes up a
 * logging kind which is currently enabled before writing anything to the log.
 *
 * @param   pLogger     Pointer to logger instance. If NULL the default logger instance will be attempted.
 * @param   fFlags      The logging flags.
 * @param   iGroup      The group.
 *                      The value ~0U is reserved for compatibility with RTLogLogger[V] and is
 *                      only for internal usage!
 * @param   pszFormat   Format string.
 * @param   ...         Format arguments.
 * @remark  This is a worker function of LogIt.
 */
RTDECL(void) RTLogLoggerEx(PRTLOGGER pLogger, unsigned fFlags, unsigned iGroup, const char *pszFormat, ...);

/**
 * Write to a logger instance.
 *
 * This function will check whether the instance, group and flags makes up a
 * logging kind which is currently enabled before writing anything to the log.
 *
 * @param   pLogger     Pointer to logger instance. If NULL the default logger instance will be attempted.
 * @param   fFlags      The logging flags.
 * @param   iGroup      The group.
 *                      The value ~0U is reserved for compatibility with RTLogLogger[V] and is
 *                      only for internal usage!
 * @param   pszFormat   Format string.
 * @param   args        Format arguments.
 */
RTDECL(void) RTLogLoggerExV(PRTLOGGER pLogger, unsigned fFlags, unsigned iGroup, const char *pszFormat, va_list args);

/**
 * printf like function for writing to the default log.
 *
 * @param   pszFormat   Printf like format string.
 * @param   ...         Optional arguments as specified in pszFormat.
 *
 * @remark The API doesn't support formatting of floating point numbers at the moment.
 */
RTDECL(void) RTLogPrintf(const char *pszFormat, ...);

/**
 * vprintf like function for writing to the default log.
 *
 * @param   pszFormat   Printf like format string.
 * @param   args        Optional arguments as specified in pszFormat.
 *
 * @remark The API doesn't support formatting of floating point numbers at the moment.
 */
RTDECL(void) RTLogPrintfV(const char *pszFormat, va_list args);


#ifndef DECLARED_FNRTSTROUTPUT          /* duplicated in iprt/string.h */
#define DECLARED_FNRTSTROUTPUT
/**
 * Output callback.
 *
 * @returns number of bytes written.
 * @param   pvArg       User argument.
 * @param   pachChars   Pointer to an array of utf-8 characters.
 * @param   cbChars     Number of bytes in the character array pointed to by pachChars.
 */
typedef DECLCALLBACK(size_t) FNRTSTROUTPUT(void *pvArg, const char *pachChars, size_t cbChars);
/** Pointer to callback function. */
typedef FNRTSTROUTPUT *PFNRTSTROUTPUT;
#endif

/**
 * Partial vsprintf worker implementation.
 *
 * @returns number of bytes formatted.
 * @param   pfnOutput   Output worker.
 *                      Called in two ways. Normally with a string an it's length.
 *                      For termination, it's called with NULL for string, 0 for length.
 * @param   pvArg       Argument to output worker.
 * @param   pszFormat   Format string.
 * @param   args        Argument list.
 */
RTDECL(size_t) RTLogFormatV(PFNRTSTROUTPUT pfnOutput, void *pvArg, const char *pszFormat, va_list args);

/**
 * Write log buffer to COM port.
 *
 * @param   pach        Pointer to the buffer to write.
 * @param   cb          Number of bytes to write.
 */
RTDECL(void) RTLogWriteCom(const char *pach, size_t cb);

/**
 * Prints a formatted string to the serial port used for logging.
 *
 * @returns Number of bytes written.
 * @param   pszFormat   Format string.
 * @param   ...         Optional arguments specified in the format string.
 */
RTDECL(size_t) RTLogComPrintf(const char *pszFormat, ...);

/**
 * Prints a formatted string to the serial port used for logging.
 *
 * @returns Number of bytes written.
 * @param   pszFormat   Format string.
 * @param   args        Optional arguments specified in the format string.
 */
RTDECL(size_t)  RTLogComPrintfV(const char *pszFormat, va_list args);


#if 0 /* not implemented yet */

/** Indicates that the semaphores shall be used to notify the other
 * part about buffer changes. */
#define LOGHOOKBUFFER_FLAGS_SEMAPHORED      1

/**
 * Log Hook Buffer.
 * Use to communicate between the logger and a log consumer.
 */
typedef struct RTLOGHOOKBUFFER
{
    /** Write pointer. */
    volatile void          *pvWrite;
    /** Read pointer. */
    volatile void          *pvRead;
    /** Buffer start. */
    void                   *pvStart;
    /** Buffer end (exclusive). */
    void                   *pvEnd;
    /** Signaling semaphore used by the writer to wait on a full buffer.
     * Only used when indicated in flags. */
    void                   *pvSemWriter;
    /** Signaling semaphore used by the read to wait on an empty buffer.
     * Only used when indicated in flags. */
    void                   *pvSemReader;
    /** Buffer flags. Current reserved and set to zero. */
    volatile unsigned       fFlags;
} RTLOGHOOKBUFFER;
/** Pointer to a log hook buffer. */
typedef RTLOGHOOKBUFFER *PRTLOGHOOKBUFFER;


/**
 * Register a logging hook.
 *
 * This type of logging hooks are expecting different threads acting
 * producer and consumer. They share a circular buffer which have two
 * pointers one for each end. When the buffer is full there are two
 * alternatives (indicated by a buffer flag), either wait for the
 * consumer to get it's job done, or to write a generic message saying
 * buffer overflow.
 *
 * Since the waiting would need a signal semaphore, we'll skip that for now.
 *
 * @returns iprt status code.
 * @param   pBuffer     Pointer to a logger hook buffer.
 */
RTDECL(int)     RTLogRegisterHook(PRTLOGGER pLogger, PRTLOGHOOKBUFFER pBuffer);

/**
 * Deregister a logging hook registered with RTLogRegisterHook().
 *
 * @returns iprt status code.
 * @param   pBuffer     Pointer to a logger hook buffer.
 */
RTDECL(int)     RTLogDeregisterHook(PRTLOGGER pLogger, PRTLOGHOOKBUFFER pBuffer);

#endif /* not implemented yet */



/**
 * Write log buffer to a debugger (RTLOGDEST_DEBUGGER).
 *
 * @param   pach        What to write.
 * @param   cb          How much to write.
 * @remark  When linking statically, this function can be replaced by defining your own.
 */
RTDECL(void) RTLogWriteDebugger(const char *pach, size_t cb);

/**
 * Write log buffer to a user defined output stream (RTLOGDEST_USER).
 *
 * @param   pach        What to write.
 * @param   cb          How much to write.
 * @remark  When linking statically, this function can be replaced by defining your own.
 */
RTDECL(void) RTLogWriteUser(const char *pach, size_t cb);

/**
 * Write log buffer to stdout (RTLOGDEST_STDOUT).
 *
 * @param   pach        What to write.
 * @param   cb          How much to write.
 * @remark  When linking statically, this function can be replaced by defining your own.
 */
RTDECL(void) RTLogWriteStdOut(const char *pach, size_t cb);

/**
 * Write log buffer to stdout (RTLOGDEST_STDERR).
 *
 * @param   pach        What to write.
 * @param   cb          How much to write.
 * @remark  When linking statically, this function can be replaced by defining your own.
 */
RTDECL(void) RTLogWriteStdErr(const char *pach, size_t cb);

#ifdef VBOX

/**
 * Prints a formatted string to the backdoor port.
 *
 * @returns Number of bytes written.
 * @param   pszFormat   Format string.
 * @param   ...         Optional arguments specified in the format string.
 */
RTDECL(size_t) RTLogBackdoorPrintf(const char *pszFormat, ...);

/**
 * Prints a formatted string to the backdoor port.
 *
 * @returns Number of bytes written.
 * @param   pszFormat   Format string.
 * @param   args        Optional arguments specified in the format string.
 */
RTDECL(size_t)  RTLogBackdoorPrintfV(const char *pszFormat, va_list args);

#endif /* VBOX */

RT_C_DECLS_END

/** @} */

#endif

