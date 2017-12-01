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
	bool isCertified() const{return data<5;}; // ���Widget�Ƿ�ͨ������
	int data;
};

void delAndNullifyUncertified(Widget*& pWidget)  // ���*pWidget��һ��δͨ������Widget,ɾ��ָ�벢��������Ϊ��
{       
	if (!pWidget->isCertified()) {   
		delete pWidget;    
		pWidget = 0;
	}
}

int main(int argc, char* argv[])
{

	vector<Widget*> v;   // ����һ��vectorȻ���ö�̬�����Widget��ָ�����
	v.push_back(new Widget(3)); 
	(*v.begin())->data=4;
	v.push_back(new Widget(13)); 

	//v.erase(remove_if(v.begin(), v.end(),    // ɾ��δͨ�������
	//	not1(mem_fun(&Widget::isCertified))), // Widgetָ��,ע�����������ڴ�й©
	//	v.end());    // mem_fun����Ϣ�ɲ鿴MSDN
	
	//��ȷ����
	for_each(v.begin(), v.end(),   // ������ָ��δͨ������Widget��
		delAndNullifyUncertified); // ָ��ɾ����������Ϊ��

	v.erase(remove(v.begin(), v.end(),   // ��v�г�ȥ��ָ��
		static_cast<Widget*>(0)), // 0����ӳ�䵽һ��ָ�룬
		v.end());   // ��C++������ȷ���Ƴ�remove�ĵ���������������

	//��ȻҲ����������ָ���
	return 0;
}
