/*
 * @Author: lqf
 * @page: www.Jackey.top
 * @Date: 2022-03-14 16:12:03
 * @LastEditors: lqf
 * @LastEditTime: 2022-03-17 15:19:13
 * @Description: 
 */
#ifndef __FREELISTT__H__
#define __FREELISTT__H__
#include <stdint.h>
#include <assert.h>
#include "common.h"
#include "centralCache.h"
namespace Itachi
{
    /**
     * @description: 一个freelist是一个内存连表，其中 m_list存储了第一块内存的地址，后面的内存地址被存放在该内存块的前八个字节，以此类推
     * @param {*}
     * @return {*}
     */
    struct FreeList
    {
        ~FreeList()
        {
            static thread_local int i = 0;
            if (!Empty())
            {
                CentralCache::RecoveryFromThread(m_list, m_end, (i++) << 3);
            }
        }
        /**
         * @brief  添加一块内存到freelist中,插入到头部
         * 
         * @param target  目标内存地址
         */
        inline void Push(void *target)
        {
            if (++m_size == 1)
                m_end = target;
            get_next_block(target) = m_list;
            m_list = target;
        }
        /**
         * @brief  插入一块内存到Freeist中，插入到头部
         * 
         * @param start 起始内存块头地址
         * @param end 终止内存块头地址
         * @param size 一共有多少块
         */
        inline void PushRange(void *start, void *end, uint64_t size)
        {
            if (m_size == 0)
                m_end = end;
            get_next_block(end) = m_list;
            m_list = start;
            m_size += size;
        }
        /**
         * @brief  放出一块内存到Freeist中,左闭右闭区间
         * 
         * @param start 起始内存块头地址
         * @param end 终止内存块头地址
         * @param size 一共有多少块
         */
        inline void PopRange(void *start, void *end, const uint64_t &size)
        {
            m_list = get_next_block(end);
            m_size -= size;
        }
        inline void PopAll()
        {
            m_list = nullptr;
            m_size = 0;
        }
        /**
         * @brief  拿出一块地址
         * 
         * @return void*  指向地址的指针
         */
        inline void *Pop()
        {
            void *temp = m_list;
            m_list = get_next_block(temp);
            --m_size;
            return temp;
        }
        /**
         * @brief  判度freelist是否为空
         * 
         * @return true 
         * @return false 
         */
        inline bool Empty()
        {
            return m_list == nullptr;
        }
        /**
         * @brief  返回推荐的最大容量
         * 
         * @return uint64_t  最大容量
         */

        inline uint64_t Size()
        {
            return m_size;
        }
        /**
         * @description: 返回连表是否满载，如果size大于maxsize的1.5倍证明满载
         * @param {*}
         * @return {bool*}
         */
        inline bool isFullLoad()
        {
            return m_maxSize < m_size;
        }
        inline void *Begin()
        {
            return m_list;
        }
        inline void *End()
        {
            return m_end;
        }
        uint64_t m_size = 0;
        void *m_list = nullptr;
        void *m_end = nullptr;
        uint64_t m_maxSize = 32;
    };
}
#endif