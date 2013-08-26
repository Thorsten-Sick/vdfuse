/** @file
 * IPRT - Generic Doubly Linked List.
 */

/*
 * Copyright (C) 2010 Oracle Corporation
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

#ifndef ___iprt_list_h
#define ___iprt_list_h

#include <iprt/types.h>

/** @defgroup grp_rt_list    RTList - Generic Doubly Linked List
 * @ingroup grp_rt
 *
 * The list implementation is circular without any type wise distintion between
 * the list and its nodes.  This can be confusing since the list head usually
 * resides in a different structure than the nodes, so care must be taken when
 * walking the list.
 *
 * @{
 */

RT_C_DECLS_BEGIN

/**
 * A list node of a doubly linked list.
 */
typedef struct RTLISTNODE
{
    /** Pointer to the next list node. */
    struct RTLISTNODE *pNext;
    /** Pointer to the previous list node. */
    struct RTLISTNODE *pPrev;
} RTLISTNODE;
/** Pointer to a list node. */
typedef RTLISTNODE *PRTLISTNODE;
/** Pointer to a list node pointer. */
typedef PRTLISTNODE *PPRTLISTNODE;

/**
 * Initialize a list.
 *
 * @param   pList               Pointer to an unitialised list.
 */
DECLINLINE(void) RTListInit(PRTLISTNODE pList)
{
    pList->pNext = pList;
    pList->pPrev = pList;
}

/**
 * Append a node to the end of the list.
 *
 * @param   pList               The list to append the node to.
 * @param   pNode               The node to append.
 */
DECLINLINE(void) RTListAppend(PRTLISTNODE pList, PRTLISTNODE pNode)
{
    pList->pPrev->pNext = pNode;
    pNode->pPrev        = pList->pPrev;
    pNode->pNext        = pList;
    pList->pPrev        = pNode;
}

/**
 * Add a node as the first element of the list.
 *
 * @param   pList               The list to prepend the node to.
 * @param   pNode               The node to prepend.
 */
DECLINLINE(void) RTListPrepend(PRTLISTNODE pList, PRTLISTNODE pNode)
{
    pList->pNext->pPrev = pNode;
    pNode->pNext        = pList->pNext;
    pNode->pPrev        = pList;
    pList->pNext        = pNode;
}

/**
 * Inserts a node after the specified one.
 *
 * @param   pCurNode            The current node.
 * @param   pNewNode            The node to insert.
 */
DECLINLINE(void) RTListNodeInsertAfter(PRTLISTNODE pCurNode, PRTLISTNODE pNewNode)
{
    RTListPrepend(pCurNode, pNewNode);
}

/**
 * Inserts a node before the specified one.
 *
 * @param   pCurNode            The current node.
 * @param   pNewNode            The node to insert.
 */
DECLINLINE(void) RTListNodeInsertBefore(PRTLISTNODE pCurNode, PRTLISTNODE pNewNode)
{
    RTListAppend(pCurNode, pNewNode);
}

/**
 * Remove a node from a list.
 *
 * @param   pNode               The node to remove.
 */
DECLINLINE(void) RTListNodeRemove(PRTLISTNODE pNode)
{
    PRTLISTNODE pPrev = pNode->pPrev;
    PRTLISTNODE pNext = pNode->pNext;

    pPrev->pNext = pNext;
    pNext->pPrev = pPrev;

    /* poison */
    pNode->pNext = NULL;
    pNode->pPrev = NULL;
}

/**
 * Checks if a node is the last element in the list.
 *
 * @retval  @c true if the node is the last element in the list.
 * @retval  @c false otherwise
 *
 * @param   pList               The list.
 * @param   pNode               The node to check.
 */
#define RTListNodeIsLast(pList, pNode)  ((pNode)->pNext == (pList))

/**
 * Checks if a node is the first element in the list.
 *
 * @retval  @c true if the node is the first element in the list.
 * @retval  @c false otherwise.
 *
 * @param   pList               The list.
 * @param   pNode               The node to check.
 */
#define RTListNodeIsFirst(pList, pNode) ((pNode)->pPrev == (pList))

/**
 * Checks if a type converted node is actually the dummy element (@a pList).
 *
 * @retval  @c true if the node is the dummy element in the list.
 * @retval  @c false otherwise.
 *
 * @param   pList               The list.
 * @param   pNodeStruct         The node structure to check.  Typically
 *                              something obtained from RTListNodeGetNext() or
 *                              RTListNodeGetPrev().  This is NOT a PRTLISTNODE
 *                              but something that contains a RTLISTNODE member!
 * @param   Type                Structure the list node is a member of.
 * @param   Member              The list node member.
 */
#define RTListNodeIsDummy(pList, pNode, Type, Member) \
         ( (pNode) == RT_FROM_MEMBER((pList), Type, Member) )

/**
 * Checks if a list is empty.
 *
 * @retval  @c true if the list is empty.
 * @retval  @c false otherwise.
 *
 * @param   pList               The list to check.
 */
#define RTListIsEmpty(pList)            ((pList)->pPrev == (pList))

/**
 * Returns the next node in the list.
 *
 * @returns The next node.
 *
 * @param   pCurNode            The current node.
 * @param   Type                Structure the list node is a member of.
 * @param   Member              The list node member.
 */
#define RTListNodeGetNext(pCurNode, Type, Member) \
    RT_FROM_MEMBER((pCurNode)->pNext, Type, Member)

/**
 * Returns the previous node in the list.
 *
 * @returns The previous node.
 *
 * @param   pCurNode            The current node.
 * @param   Type                Structure the list node is a member of.
 * @param   Member              The list node member.
 */
#define RTListNodeGetPrev(pCurNode, Type, Member) \
    RT_FROM_MEMBER((pCurNode)->pPrev, Type, Member)

/**
 * Returns the first element in the list (checks for empty list).
 *
 * @retval  Pointer to the first list element.
 * @retval  NULL if the list is empty.
 *
 * @param   pList               List to get the first element from.
 * @param   Type                Structure the list node is a member of.
 * @param   Member              The list node member.
 */
#define RTListGetFirst(pList, Type, Member) \
    (!RTListIsEmpty(pList) ? RTListNodeGetNext(pList, Type, Member) : NULL)

/**
 * Returns the last element in the list (checks for empty list).
 *
 * @retval  Pointer to the last list element.
 * @retval  NULL if the list is empty.
 *
 * @param   pList               List to get the last element from.
 * @param   Type                Structure the list node is a member of.
 * @param   Member              The list node member.
 */
#define RTListGetLast(pList, Type, Member) \
    (!RTListIsEmpty(pList) ? RTListNodeGetPrev(pList, Type, Member) : NULL)

/**
 * Returns the next node in the list or NULL if the end has been reached.
 *
 * @returns The next node or NULL.
 *
 * @param   pList               The list @a pCurNode is linked on.
 * @param   pCurNode            The current node, of type @a Type.
 * @param   Type                Structure the list node is a member of.
 * @param   Member              The list node member.
 */
#define RTListGetNext(pList, pCurNode, Type, Member) \
    ( (pCurNode)->Member.pNext != (pList) ? RT_FROM_MEMBER((pCurNode)->Member.pNext, Type, Member) : NULL )

/**
 * Returns the previous node in the list or NULL if the start has been reached.
 *
 * @returns The previous node or NULL.
 *
 * @param   pList               The list @a pCurNode is linked on.
 * @param   pCurNode            The current node, of type @a Type.
 * @param   Type                Structure the list node is a member of.
 * @param   Member              The list node member.
 */
#define RTListGetPrev(pList, pCurNode, Type, Member) \
    ( (pCurNode)->Member.pPrev != (pList) ? RT_FROM_MEMBER((pCurNode)->Member.pPrev, Type, Member) : NULL )

/**
 * Enumerate the list in head to tail order.
 *
 * @param   pList               List to enumerate.
 * @param   pIterator           The iterator variable name.
 * @param   Type                Structure the list node is a member of.
 * @param   Member              The list node member name.
 */
#define RTListForEach(pList, pIterator, Type, Member) \
    for (pIterator = RTListNodeGetNext(pList, Type, Member); \
         !RTListNodeIsDummy(pList, pIterator, Type, Member); \
         pIterator = RT_FROM_MEMBER((pIterator)->Member.pNext, Type, Member) )


/**
 * Enumerate the list in head to tail order, safe against removal of the
 * current node.
 *
 * @param   pList               List to enumerate.
 * @param   pIterator           The iterator variable name.
 * @param   pIterNext           The name of the variable saving the pointer to
 *                              the next element.
 * @param   Type                Structure the list node is a member of.
 * @param   Member              The list node member name.
 */
#define RTListForEachSafe(pList, pIterator, pIterNext, Type, Member) \
    for (pIterator = RTListNodeGetNext(pList, Type, Member), \
         pIterNext = RT_FROM_MEMBER((pIterator)->Member.pNext, Type, Member); \
         !RTListNodeIsDummy(pList, pIterator, Type, Member); \
         pIterator = pIterNext, \
         pIterNext = RT_FROM_MEMBER((pIterator)->Member.pNext, Type, Member) )


/**
 * Enumerate the list in reverse order (tail to head).
 *
 * @param   pList               List to enumerate.
 * @param   pIterator           The iterator variable name.
 * @param   Type                Structure the list node is a member of.
 * @param   Member              The list node member name.
 */
#define RTListForEachReverse(pList, pIterator, Type, Member) \
    for (pIterator = RTListNodeGetPrev(pList, Type, Member); \
         !RTListNodeIsDummy(pList, pIterator, Type, Member); \
         pIterator = RT_FROM_MEMBER((pIterator)->Member.pPrev, Type, Member) )


/**
 * Enumerate the list in reverse order (tail to head).
 *
 * @param   pList               List to enumerate.
 * @param   pIterator           The iterator variable name.
 * @param   pIterPrev           The name of the variable saving the pointer to
 *                              the previous element.
 * @param   Type                Structure the list node is a member of.
 * @param   Member              The list node member name.
 */
#define RTListForEachReverseSafe(pList, pIterator, pIterPrev, Type, Member) \
    for (pIterator = RTListNodeGetPrev(pList, Type, Member), \
         pIterPrev = RT_FROM_MEMBER((pIterator)->Member.pPrev, Type, Member); \
         !RTListNodeIsDummy(pList, pIterator, Type, Member); \
         pIterator = pIterPrev, \
         pIterPrev = RT_FROM_MEMBER((pIterator)->Member.pPrev, Type, Member) )


/**
 * Move the given list to a new list header.
 *
 * @param   pListDst            The new list.
 * @param   pListSrc            The list to move.
 */
DECLINLINE(void) RTListMove(PRTLISTNODE pListDst, PRTLISTNODE pListSrc)
{
    if (!RTListIsEmpty(pListSrc))
    {
        pListDst->pNext = pListSrc->pNext;
        pListDst->pPrev = pListSrc->pPrev;

        /* Adjust the first and last element links */
        pListDst->pNext->pPrev = pListDst;
        pListDst->pPrev->pNext = pListDst;

        /* Finally remove the elements from the source list */
        RTListInit(pListSrc);
    }
}

RT_C_DECLS_END

/** @} */

#endif
