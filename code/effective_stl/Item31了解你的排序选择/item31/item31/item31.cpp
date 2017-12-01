// item31.cpp : Defines the entry point for the console application.
//

#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;

bool qualityCompare(int x,int y)  
{
	return x<y;
}

bool hasAcceptableQuality(const int w)

{
	return w<5;
}


int main(int argc, char* argv[])
{
	int mdata[]={100,101,102,103,104,105,106,1,2,3,4,5,6,7,8,9};
	vector<int> values(mdata,mdata+16);       

	//sort(values.begin()/*+3*/,
	//	values.end()/*-3*/,		//可以排序指定的区间
	//	qualityCompare); 

	//partial_sort(values.begin(),    // 把最好的5个元素
	//	values.begin() + 5,  // （按顺序）放在values的前端
	//	values.end(),
	//	qualityCompare);

	//nth_element (values.begin(),   // 把最好的5个元素
	//	values.begin() + 5,  // 放在values前端，
	//	values.end(),   // 但不关心它们的顺序
	//	qualityCompare);    

	//vector<int>::iterator begin(values.begin()); // 方便地表示values的
	//vector<int>::iterator end(values.end());  // 起点和终点迭代器的变量
	//vector<int>::iterator goalPosition;  // 这个迭代器指示了下面代码要找的中等质量等级的values的位置
	//goalPosition = begin + values.size() / 2;  // 兴趣的values会是有序的vector的中间
	//nth_element(begin, goalPosition, end,  // 找到values中中等质量等级的值
	//	qualityCompare); 
	//cout<<*goalPosition<<endl;
	//vector<int>::iterator goaloffset;  // 这个迭代器指示了下面代码要找的中等质量等级的values的位置
	//goaloffset = begin + values.size() * 1/16;  // 兴趣的values会是有序的vector的1/16
	//nth_element(begin, goaloffset, end,  // 找到values中15/16质量等级的值
	//	qualityCompare);  
	//cout<<*goaloffset<<endl;

	vector<int>::iterator goodEnd =   // 把所有满足hasAcceptableQuality
		partition(values.begin(),   // 的values移动到values前端，
		values.end(),  // 并且返回一个指向第一个
		hasAcceptableQuality); // 不满足的values的迭代器

	return 0;
}

