/*
 * @Author: lqf
 * @page: www.Jackey.top
 * @Date: 2022-03-15 11:59:53
 * @LastEditors: lqf
 * @LastEditTime: 2022-03-17 14:29:45
 * @Description: 
 */
#ifndef __PAGECACHE__H__
#define __PAGECACHE__H__
#include<unistd.h>
#include"common.h"
#include<map>
namespace Itachi{
    class PageCache{
        public:
            using MapType=std::map<uint64_t,Span*>;
            PageCache(const PageCache&)=delete;
            PageCache&operator=(const PageCache&)=delete;
            PageCache&operator=(PageCache&&)=delete;
            PageCache(PageCache&&)=delete;
            static inline  PageCache&GetInstance(){
                static PageCache pageCache;
                return pageCache;
            }
            /**
             * @description:从central中回收cache，不用加锁，因为centralCache保证了线程安全
             *                      主要分为3个步骤：
             *                                 1，查看span前面的是否有可以合并的，有就合并
             *                                 2.查看span后面有无可以合并的，有就合并
             *                                 3.看看时候整个span的usecount为0，如果是就还给操作系统，不是就挂在连表上                     
             * @param {*}
             * @return {*}
             */            
            static void RecoveryFromCentral(Span*&);
            /**
             * @description:将span还给操作系统，主要i是通过mummap函数来归还。同时要清除map里面的映射 
             * @param {*}
             * @return {*}
             */            
            static void ReturnSpanToOs(Span*&);
            /**
             * @description: 给centralcache一个新的span，首先查找是否有可以的页数，如果有就分配出去，如果没有，需要新的分配，主要是通过os的mmap函数分配
             * @param {size_t} index 要分配的页数，也就是Pageache的下标
             * @return {*}
             */            
            static Span*NewSpan(const size_t& index);
        private:
        private:
            PageCache(){}
            static SpanLists m_SpanLists[kMaxSpanListsSize];
            static MapType m_map;
    };
}

#endif