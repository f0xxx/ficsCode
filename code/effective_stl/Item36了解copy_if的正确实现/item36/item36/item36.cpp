// item36.cpp : Defines the entry point for the console application.
//
#include <algorithm>
#include <vector>
#include <functional>
#include <iostream>
using namespace std;

int isDefective(int w)
{
	if(w>10)
		return 1;
	else 
		return 0;
}

//template<typename InputIterator,    // 一个不很正确的
//typename OutputIterator,  // copy_if实现
//typename Predicate>
//OutputIterator copy_if(InputIterator begin,
//					   InputIterator end,
//					   OutputIterator destBegin, Predicate p)
//{
//	return remove_copy_if(begin, end, destBegin, not1(p));//函数p无法适配not1
//}

template<typename InputIterator,    // 一个copy_if的
typename OutputIterator,  // 正确实现
typename Predicate>
OutputIterator copy_if(InputIterator begin,
					   InputIterator end,
					   OutputIterator destBegin,
					   Predicate p) 
{
	   while (begin != end) 
	   {
		   if (p(*begin))*destBegin++ = *begin;
		   ++begin;
	   }
	   return destBegin;
}

int main(int argc, char* argv[])
{
	int data[]={1,313,41,45,6,7,2,3};
	vector<int> widgets(data,data+8);
	copy_if(widgets.begin(), widgets.end(),  
		ostream_iterator<int>(cerr, "\n"), 
		isDefective);
	return 0;
}

