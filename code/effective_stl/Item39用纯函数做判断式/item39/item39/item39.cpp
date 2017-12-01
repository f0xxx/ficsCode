#include <algorithm>
#include <vector>
#include <functional>
#include <iostream>
using namespace std;

class BadPredicate:    // �����������ĸ�����Ϣ
	public unary_function<int, bool> { // ��μ�����40
public:
	BadPredicate(): timesCalled(0) {}  // ��timesCalled��ʼ��Ϊ0
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
	vector<int> vw(data,data+9);    // ����vector��Ȼ��

	vw.erase(remove_if(vw.begin(),  // ȥ��������intֵ;
			 vw.end(),  // ����erase��remove_if�Ĺ�ϵ
			 BadPredicate()), // ��μ�����32
			 vw.end());
	for(vector<int>::iterator i=vw.begin();i<vw.end();i++)
		cout<<*i<<endl;//��ֻ��3��6Ҳ��ȥ���ˣ�ԭ������ں��������ֵ���ݣ�copy��,����Ӧ�ö����жϺ����ô�����
	return 0;
}