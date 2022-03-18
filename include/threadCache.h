/*
 * @Author: lqf
 * @page: www.Jackey.top
 * @Date: 2022-03-14 22:15:15
 * @LastEditors: lqf
 * @LastEditTime: 2022-03-17 15:12:29
 * @Description: 
 */
#ifndef ___THREADCACHE__H___
#define ___THREADCACHE__H___
#include "freeList.h"
#include <unistd.h>
#include <map>
#include <unordered_map>
namespace Itachi
{
    class ThreadCache
    {
    public:
        ThreadCache()
        {
        }
        void *Allocate(size_t size);
        void Deallocate(void *);
    private:
        ThreadCache(const ThreadCache &) = delete;
        ThreadCache &operator=(const ThreadCache &) = delete;

    private:
        FreeList m_lists[kMaxArraySize];
        std::unordered_map<void *, size_t> m_bigPtr;
        std::unordered_map<void *, size_t> m_smallPtr;
    };

}

#endif