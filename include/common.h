/*
 * @Author: lqf
 * @page: www.Jackey.top
 * @Date: 2022-03-15 08:52:33
 * @LastEditors: lqf
 * @LastEditTime: 2022-03-17 16:03:58
 * @Description: 
 */
#ifndef __COMMON__H__
#define __COMMON__H__
#include <unistd.h>
#include <assert.h>
#include <stdint.h>

namespace Itachi
{
#define kPageSize 4 * 1024

#define kMaxSmallSize 1024
#define kMaxSize 256 * 1024

//小于64l在thread cache获取，因此需要 /8*1024
#define kMaxArraySize   1024
//小于256页，在系统中获取
#define kMaxSpanListsSize 128
    /**
 * @description: 对齐规则： 小于64k 在threadcatche获取,大于64k，直接在pagecache获取
 *              level   1.size <=512  8字节对齐
 *              level  2.513<=size<=1024 16字节对齐
 *              level  3.size <=kMaxSize 2^(n-6)=2^n>>6   7<=n<=15 
 *              level  4.大于kMaxSize 按照 kPageSize 对齐
 * @param {*}
 * @return {*}
 */

    //强制将void*转换为void** ，取值之后就是一个void*的大小，强取void*的前8个字节
    inline void *&get_next_block(void *cur)
    {
        return *static_cast<void **>(cur);
    }

    class SizeClass
    {
    public:
        /**
         * @description: 算出该内存在内存页中对应的下标，以便进行 内存分配
         * @param {size_t} size 原始内存大小
         * @return {size_t} 下标
         */
        static inline size_t Get_FreeLists_Index(const size_t &size)
        {
            //每一个freelist对应的是8字节的大小 因此需要右移3位确定下标位置
            size_t index = Get_After_Alignment(size) >> 3;
            // assert(index != 0);
            return index;
        }

        /**
         * @description: 获得一个内存块在对齐之后的大小 单位：字节
         * @param {size_t} size 原始内存的大小
         * @return {*} 对齐后的大小
         */
        static inline size_t Get_After_Alignment(const size_t &size)
        {
            size_t alignment = Get_Alignment_size(size);
            return (size + alignment - 1) & ~(alignment - 1);
        }

        /**
         * @description:  主要是在申请内存页的时候使用，计算一共需要多少页，最少是一页
         * @param {size_t} size 内存的大小
         * @return {*} 页数
         */
        static inline size_t Get_Page_Nums(const size_t &size)
        {
            size_t nPages; //= Get_Alignment_size(size);
            // nPages = (kMaxSize) / nPages;
            nPages = (size + kPageSize - 1) >> 12;
            if (nPages == 0)
            {
                nPages = 1;
            }
            return nPages;
        }
        /**
         * @description: 返回最多可以分配多少  大小为freelist的index位置中内存块大小   的内存块
         * @param {size_t index}下标
         * @return {*}最大分配的内存块个数
         */
        static inline size_t Get_Max_nblocks(const size_t &index)
        {
            size_t maxNblocks = (kMaxSize << 3) / index;
            if (maxNblocks == 0)
            {
                maxNblocks = 1;
            }
            if (maxNblocks > 512)
            {
                maxNblocks = 512;
            }
            return maxNblocks;
        }

    private:
        /**
         * @description:  获取第三级别的内存对齐大小，size <=kMaxSize 2^(n-6)=2^n>>6   7<=n<=15  ，n位size中为1的最高位的下标
         * @param {size_t} size 原始内存的大小
         * @return {*}  获取对齐大小
         */
        static inline size_t Get_Alignment_Of_Level3(size_t size)
        {
            size |= size >> 1;
            size |= size >> 2;
            size |= size >> 4;
            size |= size >> 8;
            size |= size >> 16;
            size |= size >> 32;
            return (size + 1) >> 7;
        }
        /**
     * @description: 对齐规则：
     *                  1.size <=512  8字节对齐
     *                  2.513<=size<=1024 16字节对齐
     *                  3.size <=kMaxSize 2^(n-6)=2^n>>6   7<=n<=15 
     *                  4.大于kMaxSize 按照 kPageSize 对齐
     * @param {*}
     * @return {*}
     */
        static inline size_t Get_Alignment_size(const size_t &size)
        {
            if (size <= 512)
            {
                return 8;
            }
            else if (size <= 1024)
            {
                return 16;
            }
            else if (size <= kMaxSize)
            {
                return Get_Alignment_Of_Level3(size);
            }
            return kPageSize;
        }
    };

    struct Span
    {
        //对于pagecache来说，Span是没经过戏份的，因此需要知道的是起始地址，以及有多少页数
        //有多少页数
        uint64_t s_npage = 0;
        //唯一是识别的id，其实就是指针
        uint64_t s_pageId = 0;
        //记录起始的地址
        void *s_list = nullptr;
        //对于centreCache来说，是经过了戏份的，所以需要知道每一块有多大
        uint64_t s_blockSize = 0;

        Span *s_next = nullptr;
        Span *s_prev = nullptr;

        inline bool Empty()
        {
            return s_list == nullptr;
        }

        inline void Push_Front(void *ptr)
        {
            get_next_block(ptr) = s_list;
            s_list = ptr;
        }
        inline void Push_Front_Range(void *start, void *end, const size_t &size)
        {
            get_next_block(end) = s_list;
            s_list = start;
        }
    };

    struct SpanLists
    {
        SpanLists()
        {
            m_header = new Span;
            m_header->s_next = m_header;
            m_header->s_prev = m_header;
        }
        inline Span *Pop()
        {
            auto it = m_header->s_next;
            m_header->s_next = it->s_next;
            it->s_next->s_prev = m_header;
            return it;
        }
        inline Span *Push_back(Span *it)
        {
            it->s_prev = m_header->s_prev;
            it->s_next = m_header;
            m_header->s_prev->s_next = it;
            m_header->s_prev = it;
        }
        inline Span *Begin() const
        {
            return m_header->s_next;
        }
        inline Span *End() const
        {
            return m_header;
        }
        inline bool Empty() const
        {
            return m_header->s_next == m_header;
        }
        inline static void Erase(Span *span)
        {
            if (span->s_next == nullptr || span->s_prev == nullptr)
                return;
            span->s_prev->s_next = span->s_next;
            span->s_next->s_prev = span->s_prev;
            span->s_next = nullptr;
            span->s_prev = nullptr;
        }

        Span *m_header;
        //想要知道什么时候回收，就要维护一个引用计数
        uint64_t s_useCount = 0;
    };
}


#endif