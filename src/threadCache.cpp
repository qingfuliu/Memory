/*
 * @Author: lqf
 * @page: www.Jackey.top
 * @Date: 2022-03-15 17:48:40
 * @LastEditors: lqf
 * @LastEditTime: 2022-03-17 15:18:05
 * @Description: 
 */
#include "threadCache.h"
#include "centralCache.h"
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
namespace Itachi
{

    void *ThreadCache::Allocate(size_t size)
    {
        if (size >= kMaxSize)
        {
            size = SizeClass::Get_After_Alignment(size);
            void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
            assert(ptr != MAP_FAILED);
            m_bigPtr[ptr] = size;
            return ptr;
        }
        size = SizeClass::Get_FreeLists_Index(size); //index
        if (!m_lists[size].Empty())
        {
            auto temp = m_lists[size].Pop();
            m_smallPtr[temp] = size;
            return temp;
        }
        void *start = 0;
        void *end = 0;
        uint64_t nums = m_lists[size].m_maxSize>= 256 ? m_lists[size].m_maxSize : 512;
        nums = CentralCache::getInstance().Allocat_To_Thread(start, end, size, nums);

        if (nums > 1)
        {
            m_lists[size].PushRange(get_next_block(start), end, nums - 1);
        }

        if (m_lists[size].m_maxSize<= 256)
            m_lists[size].m_maxSize+= nums;
        m_smallPtr[start] = size;
        return start;
    }
    void ThreadCache::Deallocate(void *ptr)
    {
        if (m_bigPtr.find(ptr) != m_bigPtr.end())
        {
            if (munmap(ptr, m_bigPtr[ptr]) == -1)
                abort();
            m_bigPtr.erase(ptr);
            return;
        }
        auto index = m_smallPtr[ptr];
        m_lists[index].Push(ptr);
        if (m_lists[index].isFullLoad())
        {
            CentralCache::RecoveryFromThread(m_lists[index].Begin(), m_lists[index].End(), index);
            m_lists[index].PopAll();
        }
    }
}