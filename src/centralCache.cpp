/*
 * @Author: lqf
 * @page: www.Jackey.top
 * @Date: 2022-03-15 14:11:03
 * @LastEditors: lqf
 * @LastEditTime: 2022-03-17 15:24:06
 * @Description: 
 */
#include "common.h"
#include "centralCache.h"
#include <mutex>
#include "pageCache.h"
namespace Itachi
{
    SpanLists CentralCache::m_spanLists[kMaxSpanListsSize];
    std::mutex CentralCache::m_mutex;
    //左闭区间 右闭区间   index下标    nblocks 一共多少块
    size_t CentralCache::Allocat_To_Thread(void *&start, void *&end, const size_t &index, const size_t &nblosks)
    {
        std::lock_guard<std::mutex> Lock(m_mutex);

        auto begin = m_spanLists[index].Begin();
        while (begin != m_spanLists[index].End())
        {
            if (!begin->Empty())
            {
                break;
            }
            begin = begin->s_next;
        }

        if (begin->Empty())
        {
            auto npages = SizeClass::Get_Page_Nums((nblosks*index)<<3);
            begin = PageCache::GetInstance().NewSpan(npages);
            begin->s_blockSize = (index << 3);

            void *cur = begin->s_list;
            void *end = begin->s_list + (npages << 12);
            void *prev = nullptr;
            while (cur != end)
            {
                if (prev == nullptr)
                    prev = cur;
                else
                    prev = get_next_block(prev);

                get_next_block(cur) = cur + begin->s_blockSize;
                cur = get_next_block(cur);
            }
            get_next_block(prev) = nullptr;
            m_spanLists[index].Push_back(begin);
        }

        size_t i;
        start = begin->s_list;
        end = begin->s_list;
        for (i = 1; i <= nblosks; ++i)
        {
            if (get_next_block(end) == nullptr)
            {
                break;
            }
            end = get_next_block(end);
        }

        begin->s_list = get_next_block(end);
        m_spanLists[index].s_useCount += i;

        return i;
    }

    void CentralCache::RecoveryFromThread(void *start, void *end, const size_t &index)
    {
        std::lock_guard<std::mutex> Lock(m_mutex);
        void *next;
        int i = 0;
        Itachi::Span *parent = m_spanLists[index].Begin();
        while (start)
        {
            ++i;
            next = get_next_block(start);
            parent->Push_Front(start);
            start = next;
        }
        m_spanLists[index].s_useCount -= i;
    }

    void CentralCache::ReturnSpanToPageCaChe(Span *&span)
    {
        SpanLists::Erase(span);
        PageCache::RecoveryFromCentral(span);
    }
}