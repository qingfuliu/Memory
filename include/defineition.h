/*
 * @Author: lqf
 * @page: www.Jackey.top
 * @Date: 2022-03-17 15:45:18
 * @LastEditors: lqf
 * @LastEditTime: 2022-03-17 16:02:04
 * @Description: 
 */
#ifndef __DEFINE__H__
#define __DEFINE__H__
#include "threadCache.h"

static thread_local Itachi::ThreadCache _thread_t_cache;

inline void DeAmalloc(void *ptr)
{
    _thread_t_cache.Deallocate(ptr);
}
inline void *Amalloc(const size_t &size)
{
    return _thread_t_cache.Allocate(Itachi::SizeClass::Get_After_Alignment(size));
}

#endif