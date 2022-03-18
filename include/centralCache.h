/*
 * @Author: lqf
 * @page: www.Jackey.top
 * @Date: 2022-03-15 14:11:39
 * @LastEditors: lqf
 * @LastEditTime: 2022-03-17 15:24:49
 * @Description: 
 */
#ifndef __CENTRALCACHR__H__
#define __CENTRALCACHR__H__
#include "common.h"
#include <unordered_map>
#include <mutex>
namespace Itachi
{

    class CentralCache
    {
    public:
        //记录的是内存的地址，用于归还的时候使用
        CentralCache(const CentralCache &) = delete;
        CentralCache &operator=(const CentralCache &) = delete;

        /**
         * @description: 分配内存给 threadChahe
         * @param {void*}start  分配后内存的起始地址
         * @param {void*}end  分配后内存的终止地址
         * @param {size_t} size threadCaChe需要的块的大小
         * @param {size_t} size threadCaChe需要的块的数量
         * @return {size_t} 返回分配了多少块,至少会返回一个，不会尽力去满足size的个数
         * @description: 在自己的spanlist中取出span，如果拿不到就去page cache拿一个
         */
        static size_t Allocat_To_Thread(void *&start, void *&end, const size_t &index, const size_t &nblosks);
        static void RecoveryFromThread(void *, void *, const size_t &);

        static void ReturnSpanToPageCaChe(Span *&);
        static inline CentralCache &getInstance()
        {
            static CentralCache centralCache;
            return centralCache;
        }

    private:
        CentralCache() {}

    private:
        static SpanLists m_spanLists[kMaxSpanListsSize];
        static std::mutex m_mutex;
    };
}
#endif