// item32.cpp : Defines the entry point for the console application.
//

#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;


int main(int argc, char* argv[])
{
	vector<int> v;   // ����һ��vector<int> ��1-10�����
	v.reserve(10);   // ������reserve�Ľ���������14��
	for (int i = 1; i <= 10; ++i) 
	{
		v.push_back(i);
	}
	cout << v.size()<<endl;   // ��ӡ10
	v[3] = v[5] = v[9] = 99;  // ����3��Ԫ��Ϊ99

	//remove(v.begin(), v.end(), 99); // ɾ�����е���99��Ԫ��
	//cout << v.size()<<endl;   // ��Ȼ��10��

	//vector<int>::iterator newEnd(remove(v.begin(), v.end(), 99));//����Ҫ��ɾ����λ��

	v.erase(remove(v.begin(), v.end(), 99), v.end());  // ���ɾ�����е���99��Ԫ��
	cout << v.size()<<endl;      // ���ڷ���7

	//list����ֱ�ӹ����õ�remove��Ա����ʵ����������

	return 0;
}

