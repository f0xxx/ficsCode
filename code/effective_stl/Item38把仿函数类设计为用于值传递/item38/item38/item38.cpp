// item38.cpp : Defines the entry point for the console application.
//
#include <algorithm>
#include <deque>
#include <functional>
#include <iostream>
using namespace std;


class DoSomething:
	public unary_function<int, void>// ����40�������������
{  
public:
		void operator()(int x) {}
};

//template<typename T>
//class BPFC:     // BPFC = ��Big Polymorphic
//	public     // Functor Class��
//	unary_function<T, void> {  // ����40�������������
//private:
//	Widget w;     // ������кܶ����ݣ�
//	Int x;     // ������ֵ����
//	...     // ��Ӱ��Ч��
//public:
//	virtual void operator()(const T& val) const; // ����һ���麯����
//	...     // �����и�ʱ�������
//};

template<typename T>     // �����޸ĵ�BPFC
class BPFCImpl;		// ����ʵ����
	
template<typename T>
class BPFC:      // С�ģ���̬���BPFC
	public unary_function<T, void> 
{
private:
	BPFCImpl<T> *pImpl;    // ����BPFCΨһ������
public:
	void operator()(const T& val) const   // ���ڷ��飻
	{      // ����BPFCImpl��
		pImpl->operator() (val);
	}
	//...
};

template<typename T>     // �����޸ĵ�BPFC
class BPFCImpl		// ����ʵ����
{      
private:
	//Widget w;      // ��ǰ��BPFC�����������
	int x;      // ����������
	//...
	virtual ~BPFCImpl();    // ��̬����Ҫ
	// ����������
	virtual void operator()(const T& val) const;
	friend class BPFC<T>;    // ��BPFC���Է�����Щ����
};


int main(int argc, char* argv[])
{
	typedef deque<int>::iterator DequeIntIter;  // �����typedef
	deque<int> di;
	DoSomething d;     // ����һ����������
	
	for_each<DequeIntIter,    // ����for_each������
	DoSomething&>(di.begin(),  // ������DequeIntIter
				  di.end(),    // ��DoSomething&��
				  d);    // ����ʹd�����ô��ݺͷ���

	//����������stlʹ������˵�������ã���Ϊ����������������ô��ݣ���ЩSTL�㷨��ʵ�ֲ��ܱ��롣
	//����ζ�������¡��÷º����ྡ���ܵ�С�����������ǵ�̬����Ҫʹ���麯������
	
	for_each(di.begin(),di.end(),BPFC<long>());

	return 0;
}

