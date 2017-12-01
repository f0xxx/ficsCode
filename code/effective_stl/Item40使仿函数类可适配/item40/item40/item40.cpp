#include <algorithm>
#include <list>
#include <functional>
#include <iostream>
using namespace std;
#define TRUE 1

class Widget
{
public:
protected:
private:
	int value;
};
template<typename T>
class MeetsThreshold: public std::unary_function<Widget*, bool>{
private:
	const T threshold;
public:
	MeetsThreshold(const T& threshold1):threshold(threshold1){};
	bool operator()(const Widget* pw) const{return TRUE;};
	//...
};

struct WidgetNameCompare:
	std::binary_function<Widget, Widget, bool>{
		bool operator()(const Widget& lhs, const Widget& rhs) const;
};

bool isInteresting(const Widget* pw)
{
	return TRUE;
}

int main(int argc, char* argv[])
{
	list<Widget*> widgetPtrs;

	//list<Widget*>::iterator i =
	//	find_if(widgetPtrs.begin(), widgetPtrs.end(),
	//	not1(isInteresting));  // 有问题无法适配

	list<Widget*>::iterator i =
		find_if(widgetPtrs.begin(), widgetPtrs.end(),
		not1(ptr_fun(isInteresting)));  // 没问题可以适配

	list<Widget*>::iterator j =
		find_if(widgetPtrs.begin(), widgetPtrs.end(),
		not1(MeetsThreshold<int>(10)));  // 没问题可以适配

	return 0;
}

