// item34.cpp : Defines the entry point for the console application.
//
#include <algorithm>
#include <vector>
#include <functional>
using namespace std;
//��Ҫ���������㷨��:(�ɲ�ѯMSDN��ȡ�����Ϣ)
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
	vector<int> v(data,data+sizeof(data)/sizeof(data[0]));     // ����һ��vector��һЩ���ݷŽ�ȥ

	sort(v.begin(), v.end(), greater<int>());  // ��������ʹ�����vector

	//bool a5Exists = binary_search(v.begin(), v.end(), 5);    // �����vector������5����������������,������ΪĬ�������ǽ���

	bool a5Exists1 = binary_search(v.begin(), v.end(), 5,greater<int>());       // ����5��greater��Ϊ�ȽϺ���

	return 0;
}

