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
	//	values.end()/*-3*/,		//��������ָ��������
	//	qualityCompare); 

	//partial_sort(values.begin(),    // ����õ�5��Ԫ��
	//	values.begin() + 5,  // ����˳�򣩷���values��ǰ��
	//	values.end(),
	//	qualityCompare);

	//nth_element (values.begin(),   // ����õ�5��Ԫ��
	//	values.begin() + 5,  // ����valuesǰ�ˣ�
	//	values.end(),   // �����������ǵ�˳��
	//	qualityCompare);    

	//vector<int>::iterator begin(values.begin()); // ����ر�ʾvalues��
	//vector<int>::iterator end(values.end());  // �����յ�������ı���
	//vector<int>::iterator goalPosition;  // ���������ָʾ���������Ҫ�ҵ��е������ȼ���values��λ��
	//goalPosition = begin + values.size() / 2;  // ��Ȥ��values���������vector���м�
	//nth_element(begin, goalPosition, end,  // �ҵ�values���е������ȼ���ֵ
	//	qualityCompare); 
	//cout<<*goalPosition<<endl;
	//vector<int>::iterator goaloffset;  // ���������ָʾ���������Ҫ�ҵ��е������ȼ���values��λ��
	//goaloffset = begin + values.size() * 1/16;  // ��Ȥ��values���������vector��1/16
	//nth_element(begin, goaloffset, end,  // �ҵ�values��15/16�����ȼ���ֵ
	//	qualityCompare);  
	//cout<<*goaloffset<<endl;

	vector<int>::iterator goodEnd =   // ����������hasAcceptableQuality
		partition(values.begin(),   // ��values�ƶ���valuesǰ�ˣ�
		values.end(),  // ���ҷ���һ��ָ���һ��
		hasAcceptableQuality); // �������values�ĵ�����

	return 0;
}

