// item30.cpp : Defines the entry point for the console application.
//

#include <algorithm>
#include <vector>
using namespace std;

int transmogrify(int x)  
{
	return x;
}

int main(int argc, char* argv[])
{
	int mdata[]={1,2,3,4,5,6,7,8,9};
	vector<int> values(mdata,mdata+9);       
	vector<int> results(mdata,mdata+9); 

	//transform(values.begin(), values.end(),   //��transmogrify��values�е�ÿ��ֵ���д����ӵ�results
	//	results.end(),							//�˴�resultsδ���㹻�ռ����values,����
	//	transmogrify);							//���֮ǰreserve���㹻�ռ�,��д����Ȼ��δ�����

	results.reserve(results.size() + values.size()); //�������·����ڴ�,���������������Ч��

	//transform(values.begin(), values.end(),   //�Ӻ����,��ȷ
	//	back_inserter(results),  
	//	transmogrify);   

	//transform(values.begin(), values.end(),   //��ǰ����,��ȷ,����deque
	//	front_inserter(results),  
	//	transmogrify);   

	transform(values.begin(), values.end(),   //���м����,��ȷ
		inserter(results,results.begin()+results.size()/2),  
		transmogrify);   

	return 0;
}

