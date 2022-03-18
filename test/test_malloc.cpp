/*
 * @Author: lqf
 * @page: www.Jackey.top
 * @Date: 2022-03-15 10:03:05
 * @LastEditors: lqf
 * @LastEditTime: 2022-03-17 16:04:17
 * @Description: 
 */
#include <iostream>
#include "common.h"
#include <unistd.h>
#include <string>
#include "defineition.h"
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

using namespace std;
using namespace Itachi;
#include <vector>
class A
{
public:
	A()
	{
		std::cout << "construct " << std::endl;
	}
	~A()
	{
		std::cout << "deconstruct " << std::endl;
	}
	void setName(int name)
	{
		m_name = name;
	}
	void show()
	{
		std::cout << m_name << std::endl;
	}
	int m_name;
};
#include <thread>
void *tets(void *)
{

	std::vector<void *> vvv(1500);
	for (int i = 0; i < 1500; i++)
	{
		A *b = (A *)Amalloc(sizeof(A));
		vvv[i] = (void *)b;
	}
	for (int i = 0; i < 1500; i++)
	{
		DeAmalloc(vvv[i]);
	}
	for (int i = 0; i < 1500; i++)
	{
		A *b = (A *)Amalloc(sizeof(A));
		vvv[i] = (void *)b;
	}
	for (int i = 0; i < 1500; i++)
	{
		DeAmalloc(vvv[i]);
	}
	for (int i = 0; i < 1500; i++)
	{
		A *b = (A *)Amalloc(sizeof(A));
		vvv[i] = (void *)b;
	}
	for (int i = 0; i < 1500; i++)
	{
		DeAmalloc(vvv[i]);
	}
	for (int i = 0; i < 1500; i++)
	{
		A *b = (A *)Amalloc(sizeof(A));
		vvv[i] = (void *)b;
	}
	for (int i = 0; i < 1500; i++)
	{
		DeAmalloc(vvv[i]);
	}
	for (int i = 0; i < 1500; i++)
	{
		A *b = (A *)Amalloc(sizeof(A));
		vvv[i] = (void *)b;
	}
	for (int i = 0; i < 1500; i++)
	{
		DeAmalloc(vvv[i]);
	}

	for (int i = 0; i < 1500; i++)
	{
		A *b = (A *)Amalloc(sizeof(A));
		vvv[i] = (void *)b;
	}
	for (int i = 0; i < 1500; i++)
	{
		DeAmalloc(vvv[i]);
	}
	// a->m_name=std::string();5
}
int main()
{
	// char *p = (char *)sbrk(0);
	// std::cout << (void *)p << std::endl;
	// char *ans = (char*)mmap(NULL, 100, PROT_READ | PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	// if (MAP_FAILED == ans)
	// {
	// 	std::cout << "mmap: error!!"<< std::endl;
	// }
	// ans[0]='a';
	// std::cout <<ans[0]<< std::endl;
	// munmap((void*)ans,100);
	// std::cout <<ans[0]<< std::endl;

	struct timeval sTime, eTime;
	gettimeofday(&sTime, NULL);
	// tets(nullptr);//
	vector<pthread_t> vec(1000);
	for (int i = 0; i < vec.size(); ++i)
	{
		pthread_create(&vec[i], NULL, tets, NULL);
	}
	for (int i = 0; i < vec.size(); ++i)
	{

		pthread_join(vec[i], NULL);
	}
	gettimeofday(&eTime, NULL);

	long long exeTime = (eTime.tv_sec - sTime.tv_sec) * 1000000 + (eTime.tv_usec - sTime.tv_usec); //exeTime 单位是微秒
	std::cout << (long long)exeTime;
}