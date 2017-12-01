// item32.cpp : Defines the entry point for the console application.
//

#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;


int main(int argc, char* argv[])
{
	vector<int> v;   // 建立一个vector<int> 用1-10填充它
	v.reserve(10);   // （调用reserve的解释在条款14）
	for (int i = 1; i <= 10; ++i) 
	{
		v.push_back(i);
	}
	cout << v.size()<<endl;   // 打印10
	v[3] = v[5] = v[9] = 99;  // 设置3个元素为99

	//remove(v.begin(), v.end(), 99); // 删除所有等于99的元素
	//cout << v.size()<<endl;   // 仍然是10！

	//vector<int>::iterator newEnd(remove(v.begin(), v.end(), 99));//返回要被删除的位置

	v.erase(remove(v.begin(), v.end(), 99), v.end());  // 真的删除所有等于99的元素
	cout << v.size()<<endl;      // 现在返回7

	//list中有直接供调用的remove成员函数实现上述功能

	return 0;
}

