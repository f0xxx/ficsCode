// item38.cpp : Defines the entry point for the console application.
//
#include <algorithm>
#include <deque>
#include <functional>
#include <iostream>
using namespace std;


class DoSomething:
	public unary_function<int, void>// 条款40解释了这个基类
{
public:
		void operator()(int x) {}
};

//template<typename T>
//class BPFC:     // BPFC = “Big Polymorphic
//	public     // Functor Class”
//	unary_function<T, void> {  // 条款40解释了这个基类
//private:
//	Widget w;     // 这个类有很多数据，
//	Int x;     // 所以用值传递
//	...     // 会影响效率
//public:
//	virtual void operator()(const T& val) const; // 这是一个虚函数，
//	...     // 所以切割时会出问题
//};

template<typename T>     // 用于修改的BPFC
class BPFCImpl;		// 的新实现类

template<typename T>
class BPFC:      // 小的，单态版的BPFC
	public unary_function<T, void> 
{
private:
	BPFCImpl<T> *pImpl;    // 这是BPFC唯一的数据
public:
	void operator()(const T& val) const   // 现在非虚；
	{      // 调用BPFCImpl的
		pImpl->operator() (val);
	}
	//...
};

template<typename T>     // 用于修改的BPFC
class BPFCImpl		// 的新实现类
{
private:
	//Widget w;      // 以前在BPFC里的所有数据
	int x;      // 现在在这里
	//...
	virtual ~BPFCImpl();    // 多态类需要
	// 虚析构函数
	virtual void operator()(const T& val) const;
	friend class BPFC<T>;    // 让BPFC可以访问这些数据
};


int main(int argc, char* argv[])
{
	typedef deque<int>::iterator DequeIntIter;  // 方便的typedef
	deque<int> di;
	DoSomething d;     // 建立一个函数对象

	for_each<DequeIntIter,    // 调用for_each，参数
	DoSomething&>(di.begin(),  // 类型是DequeIntIter
				  di.end(),    // 和DoSomething&；
				  d);    // 这迫使d按引用传递和返回

	//以上做法对stl使用者来说并不适用，因为如果函数对象是引用传递，有些STL算法的实现不能编译。
	//这意味着两件事。让仿函数类尽可能的小，而且让它们单态（不要使用虚函数）。

	for_each(di.begin(),di.end(),BPFC<long>());

	return 0;
}

