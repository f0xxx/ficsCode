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

	//transform(values.begin(), values.end(),   //用transmogrify对values中的每个值进行处理附加到results
	//	results.end(),							//此处results未有足够空间放入values,报错
	//	transmogrify);							//如果之前reserve了足够空间,此写法依然是未定义的

	results.reserve(results.size() + values.size()); //避免重新分配内存,提高下面插入操作的效率

	//transform(values.begin(), values.end(),   //从后插入,正确
	//	back_inserter(results),  
	//	transmogrify);   

	//transform(values.begin(), values.end(),   //从前插入,正确,用于deque
	//	front_inserter(results),  
	//	transmogrify);   

	transform(values.begin(), values.end(),   //从中间插入,正确
		inserter(results,results.begin()+results.size()/2),  
		transmogrify);   

	return 0;
}

