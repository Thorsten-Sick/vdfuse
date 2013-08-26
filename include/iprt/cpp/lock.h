/** @file
 * IPRT - RTLock Classes for Scope-based Locking.
 */

/*
 * Copyright (C) 2007 Oracle Corporation
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

#ifndef ___iprt_cpp_lock_h
#define ___iprt_cpp_lock_h

#include <iprt/critsect.h>
#ifdef RT_LOCK_STRICT
# include <iprt/lockvalidator.h>
#endif

RT_C_DECLS_BEGIN

/** @defgroup grp_rt_lock       RTLock - Scope-based Locking (C++).
 * @ingroup grp_rt
 * @{
 */

class RTLock;

/**
 * The mutex lock.
 *
 * This is used as a object data member if the intention is to lock
 * a single object. This can also be used statically, initialized in
 * a global variable, for class wide purposes.
 *
 * This is best used together with RTLock.
 */
class RTLockMtx
{
    friend class RTLock;

    private:
        RTCRITSECT      mMtx;

    public:
        RTLockMtx()
        {
#ifdef RT_LOCK_STRICT_ORDER
            RTCritSectInitEx(&mMtx, 0 /*fFlags*/,
                             RTLockValidatorClassCreateUnique(RT_SRC_POS, NULL),
                             RTLOCKVAL_SUB_CLASS_NONE, NULL);
#else
            RTCritSectInit(&mMtx);
#endif
        }

        /** Use to when creating locks that belongs in the same "class".  */
        RTLockMtx(RT_SRC_POS_DECL, uint32_t uSubClass = RTLOCKVAL_SUB_CLASS_NONE)
        {
#ifdef RT_LOCK_STRICT_ORDER
            RTCritSectInitEx(&mMtx, 0 /*fFlags*/,
                             RTLockValidatorClassForSrcPos(RT_SRC_POS_ARGS, NULL),
                             uSubClass, NULL);
#else
            NOREF(uSubClass);
            RTCritSectInit(&mMtx);
            RT_SRC_POS_NOREF();
#endif
        }

        ~RTLockMtx()
        {
            RTCritSectDelete(&mMtx);
        }

    // lock() and unlock() are private so that only
    // friend RTLock can access them
    private:
        inline void lock()
        {
            RTCritSectEnter(&mMtx);
        }

        inline void unlock()
        {
            RTCritSectLeave(&mMtx);
        }
};


/**
 * The stack object for automatic locking and unlocking.
 *
 * This is a helper class for automatic locks, to simplify
 * requesting a RTLockMtx and to not forget releasing it.
 * To request a RTLockMtx, simply create an instance of RTLock
 * on the stack and pass the mutex to it:
 *
 * @code
    extern RTLockMtx gMtx;     // wherever this is
    ...
    if (...)
    {
        RTLock lock(gMtx);
        ... // do stuff
        // when lock goes out of scope, destructor releases the mutex
    }
   @endcode
 *
 * You can also explicitly release the mutex by calling RTLock::release().
 * This might be helpful if the lock doesn't go out of scope early enough
 * for your mutex to be released.
 */
class RTLock
{
    private:
        RTLockMtx  &mMtx;
        bool        mfLocked;

    public:
        RTLock(RTLockMtx &aMtx)
            : mMtx(aMtx)
        {
            mMtx.lock();
            mfLocked = true;
        }

        ~RTLock()
        {
            if (mfLocked)
                mMtx.unlock();
        }

        inline void release()
        {
            if (mfLocked)
            {
                mMtx.unlock();
                mfLocked = false;
            }
        }
};


/** @} */

RT_C_DECLS_END

#endif

