#include <algorithm>
#include <vector>
#include <functional>
#include <iostream>
using namespace std;

class BadPredicate:    // 关于这个基类的更多信息
	public unary_function<int, bool> { // 请参见条款40
public:
	BadPredicate(): timesCalled(0) {}  // 把timesCalled初始化为0
	bool operator()(const int&) /*const*/
	{
		return ++timesCalled == 3;
	}
private:
	size_t timesCalled;
};


int main(int argc, char* argv[])
{
	int data[]={1,2,3,4,5,6,7,8,9};
	vector<int> vw(data,data+9);    // 建立vector，然后

	vw.erase(remove_if(vw.begin(),  // 去掉第三个int值;
			 vw.end(),  // 关于erase和remove_if的关系
			 BadPredicate()), // 请参见条款32
			 vw.end());
	for(vector<int>::iterator i=vw.begin();i<vw.end();i++)
		cout<<*i<<endl;//不只是3，6也被去掉了，原因就在于函数对象的值传递（copy）,所以应该对做判断函数用纯函数
	return 0;
}