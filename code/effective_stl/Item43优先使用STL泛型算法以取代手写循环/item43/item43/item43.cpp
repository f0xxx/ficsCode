//#include <algorithm>
//#include <list>
//#include <functional>
#include <iostream>
//#include <assert.h>
using namespace std;

//class Widget
//{
//public:
//	//void show(){cout<<"hello world !";}
//protected:
//private:
//	int value;
//};
//
//
//template <class T>
//struct OpNewCreator
//{
//	static T* Create()
//	{
//		return new T;
//	}
//};
////template <class CreationPolicy>
////class WidgetManager : public CreationPolicy
////{ /*...*/ };
//
//template <template <class Created> class CreationPolicy>
//class WidgetManager : public CreationPolicy<Widget>
//{
//public:
//	void SwitchPrototype(Widget* pNewPrototype)
//{
//	CreationPolicy<Widget>& myPolicy = *this;
//	delete myPolicy.GetPrototype();
//	myPolicy.SetPrototype(pNewPrototype);
//}
//};
//
//template<bool> struct CompileTimeError;
//template<> struct CompileTimeError<true> {};
//#define MYASSERT(expr) \
//	(CompileTimeError<(expr)>())
//
//template <class To, class From>
//To safe_reinterpret_cast(From from)
//{
//	MYASSERT(sizeof(From) <= sizeof(To));
//	return (To)from;
//}
//typedef WidgetManager< OpNewCreator> MyWidgetMgr;

template <class Window, class Controller>
class Widget
{
};
class MyController
{
};
template <class T>
class Button
{
};

template <class Window> 
class Widget<Window, MyController> 
{
};

template <class ButtonArg>
class Widget<Button<ButtonArg>, MyController>
{
};

void Fun()
{
	class Local
	{
	public:
		int a;
	};

	Local bb;
	bb.a=1000;
}

int main(int argc, char* argv[])
{
	//MyWidgetMgr a;
	//Widget *ha=a.Create();
	//a.SwitchPrototype(ha);
	//ha->show();
	//void* somePointer =NULL;
	//int c = safe_reinterpret_cast<int>(somePointer);
	//myassert(i);
	//assert(i!=NULL);
	//int p=safe_reinterpret_cast<int>(i);
	Widget<Button<int>,MyController> a;
	return 0;
}

