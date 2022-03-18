/*
 * @Author: lqf
 * @page: www.Jackey.top
 * @Date: 2022-03-15 11:59:44
 * @LastEditors: lqf
 * @LastEditTime: 2022-03-17 15:41:22
 * @Description: 
 */
#include "common.h"
#include "pageCache.h"
#include <sys/mman.h>
#include <unistd.h>
namespace Itachi
{
    SpanLists PageCache::m_SpanLists[kMaxSpanListsSize];
    PageCache::MapType PageCache::m_map;
    /**
     * @description: 在pagecache里面取出页数
     * @param {size_t} index 代表我要取多少页，也就是pagecache的下标
     * @return {*}
     */
    Span *PageCache::NewSpan(const size_t &index)
    {
        assert(index < kMaxSpanListsSize);
        if (!m_SpanLists[index].Empty())
        {
            auto temp = m_SpanLists[index].Pop();
            temp->s_list = (void *)(temp->s_pageId << 12);
            return temp;
        }
        //没有的话就在后面找一个
        for (int i = index + 1; i < kMaxSpanListsSize; ++i)
        {
            if (!m_SpanLists[i].Empty())
            {
                auto temp = m_SpanLists[i].Pop();

                Span *it = new Span;

                it->s_pageId = temp->s_pageId;

                m_map[it->s_pageId] = it;
                // it->s_useCount = 0;
                it->s_list = (void *)(temp->s_pageId << 12);
                it->s_npage = index;

                temp->s_npage -= index;
                temp->s_pageId += index;
                m_map[temp->s_pageId] = temp;
                //将pagecache里面的span放到合适的位置
                if (temp->s_npage != 0)
                    m_SpanLists[temp->s_npage].Push_back(temp);
                return it;
            }
        }
        //去找操作系统要
        /**
         * @description: 匿名映射 参数fd需要设置为1
         * @param {flags} 影响映射区域的各种特性。在调用mmap()时必须要指定MAP_SHARED 或MAP_PRIVATE。 
         *                                                                 1.MAP_SHARED对映射区域的写入数据会复制回文件内，而且允许其他映射该文件的进程共享。
                                                                            2.MAP_PRIVATE 对映射区域的写入操作会产生一个映射文件的复制，
                                                                            即私人的 写入时复制 （copy on write）对此区域作的任何修改都不会写回原来的文件内容。
                                                                            3.MAP_ANONYMOUS建立匿名映射。此时会忽略参数fd，不涉及文件，而且映射区域无法和其他进程共享。
         * @param {PORT}   PROT_READ //页内容可以被读取          PROT_WRITE //页可以被写入                                                        
         * @return {void*} 失败返回MAP_FAILED 成功返回地址指针
         */
        // //分配128页给内存

        void *ptr = mmap(NULL, (kPageSize << 8), PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (ptr == MAP_FAILED)
        {
            abort();
        }

        // Span *span = new Span;
        // span->s_pageId = (uint64_t)ptr >> 12;
        // span->s_npage = kMaxSpanListsSize - 1;
        // m_SpanLists[span->s_npage].Push_back(span);
        // m_map[span->s_pageId] = span;

        // return NewSpan(index);
        Span *span = new Span;
        span->s_pageId = ((uint64_t)(ptr)>>12)+index;
        span->s_npage = kMaxSpanListsSize -index- 1;
        m_SpanLists[span->s_npage].Push_back(span);
        m_map[span->s_pageId] = span;

        Span *ans = new Span;
        ans->s_pageId = (uint64_t)(ptr) >> 12;
        ans->s_list = ptr;
        ans->s_npage = index;
        m_map[ans->s_pageId] = ans;
        return ans;

    }
    void PageCache::RecoveryFromCentral(Span *&span)
    {
        //需要合并
        // assert(m_map.find(span->s_pageId) != m_map.end());/
        auto cur = span;
        //向前合并
        while (1)
        {
            auto it = m_map.find(span->s_pageId - 1);

            if (it == m_map.end())
                break;

            auto next = it->second;
            if (next->s_npage + cur->s_npage >= kMaxSpanListsSize)
                break;
            SpanLists::Erase(next);
            m_map[cur->s_pageId + cur->s_npage - 1] = next;
            next->s_npage += cur->s_npage;
            m_SpanLists[next->s_npage].Push_back(next);
            cur = next;
        }
        //向后合并
        auto head = cur;
        cur = span;
        while (1)
        {
            auto it = m_map.find(cur->s_pageId + cur->s_npage);
            if (it == m_map.end())
                break;
            auto next = it->second;
            if (head->s_npage + cur->s_npage >= kMaxSpanListsSize)
                break;
            SpanLists::Erase(next);
            m_map[next->s_pageId + next->s_npage - 1] = head;
            head->s_npage += next->s_npage;
            cur = next;
        }
        if (head->s_npage == kMaxSpanListsSize - 1)
        {
            ReturnSpanToOs(head);
            delete head;
            if (head != span)
            {
                delete span;
            }
        }
        else
        {
            if (head != span)
            {
                delete span;
            }
            m_SpanLists[head->s_npage].Push_back(head);
        }
    }
    void PageCache::ReturnSpanToOs(Span *&span)
    {
        for (int i = 0; i < kMaxSpanListsSize - 1; ++i)
        {
            m_map.erase(span->s_pageId + i);
        }
        if (munmap((void *)(span->s_pageId << 12), kPageSize << 8) == -1)
        {
            abort();
        }
    }

}