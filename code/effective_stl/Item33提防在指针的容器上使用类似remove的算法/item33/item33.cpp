// item33.cpp : Defines the entry point for the console application.
//

#include <algorithm>
#include <vector>
#include <iostream>
#include <functional>
using namespace std;

class Widget{
public:
	Widget(int d):data(d){};
	bool isCertified() const{return data<5;}; // 这个Widget是否通过检验
	int data;
};

void delAndNullifyUncertified(Widget*& pWidget)  // 如果*pWidget是一个未通过检验Widget,删除指针并且设置它为空
{       
	if (!pWidget->isCertified()) {   
		delete pWidget;    
		pWidget = 0;
	}
}

int main(int argc, char* argv[])
{

	vector<Widget*> v;   // 建立一个vector然后用动态分配的Widget的指针填充
	v.push_back(new Widget(3)); 
	(*v.begin())->data=4;
	v.push_back(new Widget(13)); 

	//v.erase(remove_if(v.begin(), v.end(),    // 删除未通过检验的
	//	not1(mem_fun(&Widget::isCertified))), // Widget指针,注意这里会造成内存泄漏
	//	v.end());    // mem_fun的信息可查看MSDN
	
	//正确做法
	for_each(v.begin(), v.end(),   // 把所有指向未通过检验Widget的
		delAndNullifyUncertified); // 指针删除并且设置为空

	v.erase(remove(v.begin(), v.end(),   // 从v中除去空指针
		static_cast<Widget*>(0)), // 0必须映射到一个指针，
		v.end());   // 让C++可以正确地推出remove的第三个参数的类型

	//当然也可以用智能指针哈
	return 0;
}
