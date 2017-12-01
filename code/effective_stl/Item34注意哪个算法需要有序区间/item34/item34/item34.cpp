// item34.cpp : Defines the entry point for the console application.
//
#include <algorithm>
#include <vector>
#include <functional>
using namespace std;
//需要此条件的算法有:(可查询MSDN获取相关信息)
//binary_search
//lower_bound
//upper_bound
//equal_range
//
//set_union
//set_intersection
//set_difference
//set_symmetric_difference
//
//merge
//inplace_merge
//
//includes
//
//unique
//unique_copy


int main(int argc, char* argv[])
{
	int data[]={21,3,45,3,5,6,4,545,61,522};
	vector<int> v(data,data+sizeof(data)/sizeof(data[0]));     // 建立一个vector把一些数据放进去

	sort(v.begin(), v.end(), greater<int>());  // 降序排列使用这个vector

	//bool a5Exists = binary_search(v.begin(), v.end(), 5);    // 在这个vector中搜索5假设它是升序排列,报错因为默认排序是降序

	bool a5Exists1 = binary_search(v.begin(), v.end(), 5,greater<int>());       // 搜索5把greater作为比较函数

	return 0;
}

