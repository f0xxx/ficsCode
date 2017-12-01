#include <string>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <deque>
#include <fstream>
#include <assert.h>
using namespace std;

/*
查找算法:
adjacent_find(), binary_search(), count(), count_if(), equal_range(),
find(), find_end(), find_first_of(), find_if(), lower_bound(),
upper_bound(), search(), search_n()

排序和通用整序算法:
inplace_merge(), merge(), nth_element(), partial_sort(),
partial_sort_copy(), partition(), random_shuffle(), reverse(),
reverse_copy(), rotate(), rotate_copy(), sort(), stable_sort(),
stable_partition()

删除和替换算法:
copy(), copy_backwards(), iter_swap(), remove(), remove_copy(),
remove_if(), remove_copy_if(), replace(), replace_copy(),
replace_if(), replace_copy_if(), swap(), swap_range(), unique(),
unique_copy()

排列组合算法:
next_permutation(), prev_permutation()

算术算法:
accumulate(), partial_sum(), inner_product(), adjacent_difference()

生成和异变算法:
fill(), fill_n(),for_each(), generate(), generate_n(), transform()

关系算法:
equal(), includes(), lexicographical_compare(), max(), max_element(),
min(), min_element(), mismatch()

集合算法:
set_union(), set_intersection(), set_difference(),
set_symmetric_difference()

堆算法:
make_heap(), pop_heap(), push_heap(), sort_heap()
*/

//int main(int argc, char* argv[])
//{
//	int search_value;
//	int ia[ 6 ] = { 27, 210, 12, 47, 109, 83 };
//	cout << "enter search value: ";
//	cin >> search_value;
//	int *presult = find( &ia[0], &ia[6], search_value );
//	cout << "The value " << search_value<< ( presult == &ia[6]? " is not present" : " is present" )<<endl;
//	return 0;
//}

//int main(int argc, char* argv[])
//{
//	int search_value;
//	int ia[ 6 ] = { 27, 210, 12, 47, 109, 83 };
//	//vector<int> vec( ia, ia+6 );
//	list<int> vec( ia, ia+6 );
//	cout << "enter search value: ";
//	cin >> search_value;
//	//vector<int>::iterator presult;
//	list<int>::iterator presult;
//	presult = find( vec.begin(), vec.end(), search_value );
//	cout << "The value " << search_value<< ( presult == vec.end()? " is not present" : " is present" )<< endl;
//	return 0;
//}

class GreaterThan 
{
public:
	GreaterThan( int sz = 6 ) : _size( sz ){}
	int size() { return _size; }
	bool operator()( const string &s1 )
	{ return s1.size() > _size; }
private:
	int _size;
};

class PrintElem 
{
public:
	PrintElem( int lineLen = 8 ): _line_length( lineLen ), _cnt( 0 ){}
	void operator()( const string &elem )
	{
		++_cnt;
		if ( _cnt % _line_length == 0 )
		{ cout << '\n'; }
		cout << elem << " ";
	}
private:
	int _line_length;
	int _cnt;
};

class LessThan 
{
public:
	bool operator()( const string & s1, const string & s2 )
	{ return s1.size() < s2.size(); }
};
typedef vector<string> textwords;

void process_vocab( vector<textwords>*pvec )
{
	if ( ! pvec ) {
		// 给出警告消息
		return;
	}
	vector< string> texts;
	vector<textwords>::iterator iter;
	for ( iter = pvec->begin(); iter != pvec->end(); ++iter )
		copy( (*iter).begin(), (*iter).end(),back_inserter( texts ));
	sort( texts.begin(), texts.end() );	// 排序 texts 的元素

	for_each( texts.begin(), texts.end(), PrintElem() );	// ok, 我们来看一看我们有什么

	cout << "\n\n"; // 只是分隔显示输出
	vector<string>::iterator it;
	it = unique( texts.begin(), texts.end() );
	texts.erase( it, texts.end() );	// 删除重复元素

	for_each( texts.begin(), texts.end(), PrintElem() );	// ok, 让我们来看一看现在我们有什么了

	cout << "\n\n";
	stable_sort( texts.begin(), texts.end(), LessThan() );	// 根据缺省的长度 6 排序元素stable_sort() 保留相等元素的顺序
	for_each( texts.begin(), texts.end(), PrintElem() );
	cout << "\n\n";

	int cnt = 0;	// 计数长度大于6 的字符串的个数
	cnt=count_if( texts.begin(), texts.end(), GreaterThan());
	cout << "Number of words greater than length six are "
		<< cnt << endl;
	static string rw[] = { "and", "if", "or", "but", "the" };
	vector<string> remove_words( rw, rw+5 );
	vector<string>::iterator it2 =remove_words.begin();
	for ( ; it2 != remove_words.end(); ++it2 )
	{
		int cnt = 0;
		cnt=count( texts.begin(), texts.end(), *it2 );
		cout << cnt << " instances removed: "
			<< (*it2) << endl;
		texts.erase(remove(texts.begin(),texts.end(),*it2),	texts.end());
	}
	cout << "\n\n";
	for_each( texts.begin(), texts.end(), PrintElem() );
}

//int main(int argc, char* argv[])
//{
//	vector<textwords> sample;
//	vector<string> t1, t2;
//	string t1fn, t2fn;
//	cout << "text file #1: "; cin >> t1fn;	// 要求用户输入文件,实际中的程序应该做错误检查
//	cout << "text file #2: "; cin >> t2fn;
//	ifstream infile1( t1fn.c_str());	// 打开文件
//	ifstream infile2( t2fn.c_str());
//	istream_iterator< string > input_set1( infile1 ),eos;
//	istream_iterator< string > input_set2( infile2 );
//	copy( input_set1, eos, back_inserter( t1 ));
//	copy( input_set2, eos, back_inserter( t2 ));
//	sample.push_back( t1 );
//	sample.push_back( t2 );
//	process_vocab( &sample );
//	return 0;
//}

//list为双向链表结构,无法按常数时间访问(不提供RandomAccessIterator),so比起泛型算法优先考虑以下的成员函数

//list::merge() 用第二个有序的list合并一个有序list
//list::remove() 删除等于某个值的元素
//list::remove_if() 删除满足某个条件的元素
//list::reverse() 将list中元素反向排列
//list::sort() 排序list的元素
//list::splice() 把一个list的元素移到另一个list中
//list::unique() 删除某个元素的重复连续拷贝
class Even 
{
public:
	bool operator()( int elem ) 
	{ return ! ( elem % 2 ); }
};

class EvenPair 
{
public:
	bool operator()( int val1, int val2 )
	{ return ! ( val2 % val1 ); }
};

//int main(int argc, char* argv[])
//{
//	int array1[ 10 ] = { 34, 0, 8, 3, 1, 13, 2, 5, 21, 1 };
//	int array2[ 6 ] = { 3,377, 89, 233, 55, 144 };
//	list< int > ilist1( array1, array1 + 10 );
//	list< int > ilist2( array2, array2+6 );
//	ilist1.sort(); ilist2.sort();	// merge 要求两个 list 已经排序
//	ilist1.merge( ilist2 );
//	ilist1.remove( 1 );
//	ilist1.remove_if( Even() );
//	ilist1.reverse();
//	ilist1.sort( less<int>() );
//
//	//void list::splice( iterator pos, list rhs );
//	//void list::splice( iterator pos, list rhs, iterator ix );
//	//void list::splice( iterator pos, list rhs, iterator first, iterator last );
//	int array[ 10 ] = { 0, 1, 1, 2, 3, 5, 8, 13, 21, 34 };
//	list< int > ilist3( array, array + 10 );
//	list< int > ilist4( array, array+2 ); // 包含 0, 1
//	//下面使用splice()把ilist1 的第一个元素移到ilist2 中现在ilist2 包含元素0 1 0 而ilist1 不再包含0
//	
//	// ilist2.end() 指示要接合元素的位置被接合的元素在该位置之前
//	// ilist1 指示从哪个 list 中移动元素ilist1.begin() 指示要被移动的元素
//	ilist4.splice( ilist4.end(), ilist3, ilist3.begin() );
//
//	list< int >::iterator first, last;
//	first = find( ilist3.begin(), ilist3.end(), 2);
//	last = find( ilist3.begin(), ilist3.end(), 13);
//	ilist4.splice( ilist4.begin(), ilist3, first, last );
//
//	list< int >::iterator pos = find(ilist4.begin(), ilist4.end(), 5 );
//	ilist4.splice( pos, ilist3 );
//
//	//去掉第二个元素能被第一个元素整除的相邻元素
//	ilist4.unique(EvenPair());
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//accumulate()
//
//template < class InputIterator, class Type >
//Type accumulate
//(InputIterator first, InputIterator last, Type init );
//
//template < class InputIterator, class Type,class BinaryOperation >
//Type accumulate
//(InputIterator first, InputIterator last, Type init, BinaryOperation op );

//int main(int argc, char* argv[])
//{
//	int ia[] = { 1, 2, 3, 4 };
//	list<int> ilist( ia, ia+4 );
//	int ia_result = accumulate(&ia[0], &ia[4], 0);
//	int ilist_res = accumulate(ilist.begin(), ilist.end(), 0, plus<int>() );
//
//	cout << "accumulate()\n\t"
//		<< "operating on values {1,2,3,4}\n\t"
//		<< "result with default addition: "
//		<< ia_result << "\n\t"
//		<< "result with plus<int> function object: "
//		<< ilist_res
//		<< endl;
//
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//adjacent_difference()
//
//template < class InputIterator, class OutputIterator >
//OutputIterator adjacent_difference
//(InputIterator first, InputIterator last,OutputIterator result );
//
//template < class InputIterator, class OutputIterator,class BinaryOperation >
//OutputIterator adjacent_difference
//(InputIterator first, InputIterator last,OutputIterator result, BinaryOperation op );

//int main(int argc, char* argv[])
//{
//	int ia[] = { 1, 1, 2, 3, 5, 8 };
//	list<int> ilist(ia, ia+6);
//	list<int> ilist_result(ilist.size());
//	adjacent_difference(ilist.begin(), ilist.end(), ilist_result.begin() );
//	// 输出为: 1 0 1 1 2 3
//	copy( ilist_result.begin(), ilist_result.end(),	ostream_iterator<int>(cout," "));
//	cout << endl;
//	adjacent_difference( ilist.begin(), ilist.end(), ilist_result.begin(), multiplies<int>() );
//	// 输出为: 1 1 2 6 15 40
//	copy( ilist_result.begin(), ilist_result.end(), ostream_iterator<int>(cout," "));
//	cout << endl;
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//adjacent_find()
//
//template < class ForwardIterator >
//ForwardIterator adjacent_find
//(ForwardIterator first, ForwardIterator last );
//
//template < class ForwardIterator, class BinaryPredicate >
//ForwardIterator adjacent_find
//(ForwardIterator first, ForwardIterator last, BinaryPredicate pred );

//class TwiceOver 
//{
//public:
//	bool operator() ( int val1, int val2 )
//	{ return val1 == val2/2 ? true : false; }
//};
//
//int main(int argc, char* argv[])
//{
//	int ia[] = { 1, 4, 4, 8 };
//	vector< int > vec( ia, ia+4 );
//	int *piter;
//	vector< int >::iterator iter;
//	piter = adjacent_find( ia, ia+4 );	// piter 指向 ia[1]
//	assert( *piter == ia[ 1 ] );
//	iter = adjacent_find( vec.begin(), vec.end(), TwiceOver() );	// iter 指向 vec[2]
//	assert( *iter == vec[ 2 ] );
//	cout << "ok: adjacent-find() succeeded!\n";	// 到达这里表示一切顺利
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//binary_search()
//
//template< class ForwardIterator, class Type >
//bool binary_search
//( ForwardIterator first,ForwardIterator last, const Type &value );
//
//template< class ForwardIterator, class Type,class Compare >
//bool binary_search
//( ForwardIterator first,ForwardIterator last, const Type &value, Compare comp );

//int main(int argc, char* argv[])
//{
//	int ia[] = {29,23,20,22,17,15,26,51,19,12,35,40};
//	sort( &ia[0], &ia[12] );
//	bool found_it = binary_search( &ia[0], &ia[12], 18 );
//	assert( found_it == false );
//	vector< int > vec( ia, ia+12 );
//	sort( vec.begin(), vec.end(), greater<int>() );
//	found_it = binary_search( vec.begin(), vec.end(), 26, greater<int>() );
//	assert( found_it == true );
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//copy()
//
//template < class InputIterator, class OutputIterator >
//OutputIterator copy
//( InputIterator first1, InputIterator last, OutputIterator first2 );

//int main(int argc, char* argv[])
//{
//	int ia[] = { 0, 1, 1, 3, 5, 8, 13 };
//	vector< int > vec( ia, ia+7 );
//	ostream_iterator< int > ofile( cout, " " );
//	cout << "original element sequence:\n";
//	copy( vec.begin(), vec.end(), ofile ); cout << '\n';
//	copy( ia+1, ia+7, ia );	// 左移 1 位
//	cout << "shifting array sequence left by 1:\n";
//	copy( ia, ia+7, ofile ); cout << '\n';
//	copy( vec.begin()+2, vec.end(), vec.begin() );	// 左移 2 位
//	cout << "shifting vector sequence left by 2:\n";
//	copy( vec.begin(), vec.end(), ofile ); cout << '\n';
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//copy_backward()
//
//template < class BidirectionalIterator1,class BidirectionalIterator2 >
//BidirectionalIterator2 copy_backward
//( BidirectionalIterator1 first,	BidirectionalIterator1 last1, BidirectionalIterator2 last2 );

class print_elements
{
public:
	void operator()( string elem ) 
	{
		cout << elem << ( _line_cnt++%8 ? " " : "\n\t" );
	}
	static void reset_line_cnt() { _line_cnt = 1; }
private:
	static int _line_cnt;
};
int print_elements::_line_cnt = 1;

//int main(int argc, char* argv[])
//{
//	string sa[] = {"The", "light", "untonsured", "hair","grained", "and", "hued", "like", "pale", "oak" };
//	vector< string > svec( sa, sa+10 );
//	cout << "original list of strings:\n\t";
//	for_each( svec.begin(), svec.end(), print_elements() );
//	cout << "\n\n";
//	copy_backward( svec.begin()+1, svec.end()-3, svec.end() );
//	print_elements::reset_line_cnt();
//	cout << "sequence after "<< "copy_backward( begin+1, end-3, end ):\n\t";
//	for_each( svec.begin(), svec.end(), print_elements() );
//	cout << "\n";
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//count()
//
//template< class InputIterator, class Type >
//iterator_traits<InputIterator>::distance_type count
//( InputIterator first,InputIterator last, const Type& value );

/**************************************************************
* text read:
Alice Emma has long flowing red hair. Her Daddy says
when the wind blows through her hair, it looks almost alive,
like a fiery bird in flight. A beautiful fiery bird, he tells her,
magical but untamed. "Daddy, shush, there is no such thing,"
she tells him, at the same time wanting him to tell her more.
Shyly, she asks, "I mean, Daddy, is there?"
****************************************************************
* 程序输出:
* count(): fiery occurs 2 times
****************************************************************
*/

//int main(int argc, char* argv[])
//{
//	ifstream infile( "alice_emma" );
//	assert ( infile != 0 );
//	istream_iterator< string > instream( infile ),eos;
//	list<string> textlines(instream,eos);
//	//copy( instream, eos, back_inserter( textlines ));
//	string search_item( "fiery" );
//	int elem_count = 0;
//	list<string>::iterator
//		ibegin = textlines.begin(),
//		iend = textlines.end();
//
//	elem_count=count( ibegin, iend, search_item );
//	cout << "count(): " << search_item << " occurs " << elem_count << " times\n";
//
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//count_if()
//
//template< class InputIterator, class Predicate >
//iterator_traits<InputIterator>::distance_type count_if
//( InputIterator first, InputIterator last, Predicate pred );

class Even_new
{
public:
	bool operator()( int val )
	{ return val%2 ? false : true; }
};

//int main(int argc, char* argv[])
//{
//	int ia[] = {0,1,1,2,3,5,8,13,21,34};
//	list< int > ilist( ia, ia+10 );
//
//	int ia_count = 0;
//	ia_count=count_if( &ia[0], &ia[10], Even_new() );
//	// 生成结果为: count_if(): there are 4 elements that are even.
//	cout << "count_if(): there are "<< ia_count << " elements that are even.\n";
//	int list_count = 0;
//	list_count=count_if( ilist.begin(), ilist.end(),bind2nd(less<int>(),10) );
//	// 生成结果为: count_if(): there are 7 elements that are less than 10.
//	cout << "count_if(): there are "<< list_count<< " elements that are less than 10.\n";
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//equal()
//
//template< class InputIterator1, class InputIterator2 >
//bool equal
//( InputIterator1 first1, InputIterator1 last, InputIterator2 first2 );
//
//template< class InputIterator1, class InputIterator2,class BinaryPredicate >
//bool equal
//( InputIterator1 first1, InputIterator1 last, InputIterator2 first2, BinaryPredicate pred );

//如果两个序列在范围[first,last)内包含的元素都相等则equal()返回true 如果第二序列
//包含更多的元素则不会考虑这些元素如果我们希望保证两个序列完全相等则需要写
//if ( vec1.size() == vec2.size() && equal( vec1.begin(), vec1.end(), vec2.begin() );

class equal_and_odd
{
public:
	bool
		operator()( int val1, int val2 )
	{
		return ( val1 == val2 &&( val1 == 0 || val1 % 2 ));
	}
};

//int main(int argc, char* argv[])
//{
//	int ia[] = { 0,1,1,2,3,5,8,13 };
//	int ia2[] = { 0,1,1,2,3,5,8,13,21,34 };
//	bool res=false;
//	// true: 都等于 ia. 的长度,生成结果为: int ia[7] equal to int ia2[9]? true.
//	res = equal( &ia[0], &ia[7], &ia2[0] );
//	cout << "int ia[7] equal to int ia2[9]? "<< ( res ? "true" : "false" ) << ".\n";
//	list< int > ilist( ia, ia+7 );
//	list< int > ilist2( ia2, ia2+9 );
//	// 生成结果为: list ilist equal to ilist2? true.
//	res = equal( ilist.begin(), ilist.end(), ilist2.begin() );
//	cout << "list ilist equal to ilist2? "<< ( res ? "true" : "false" ) << ".\n";
//	// false: 0, 2, 8 不相等, 也不是奇数
//	// 生成结果为: list ilist equal_and_odd() to ilist2? false.
//	res = equal( ilist.begin(), ilist.end(),ilist2.begin(), equal_and_odd() );
//	cout << "list ilist equal_and_odd() to ilist2? "<< ( res ? "true" : "false" ) << ".\n";
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//equal_range()
//
//template< class ForwardIterator, class Type >
//pair< ForwardIterator, ForwardIterator > equal_range
//( ForwardIterator first, ForwardIterator last, const Type &value );
//
//template< class ForwardIterator, class Type, class Compare >
//pair< ForwardIterator, ForwardIterator > equal_range
//( ForwardIterator first, ForwardIterator last, const Type &value, Compare comp );

//int main(int argc, char* argv[])
//{
//	int ia[] = { 29,23,20,22,22,22,17,15,26,51,19,12,35,40 };
//	vector< int > ivec( ia, ia+14 );
//	ostream_iterator< int > ofile( cout, " " );
//	sort( &ia[0], &ia[14] );
//	cout << "array element sequence after sort:\n";
//	copy( ia, ia+12, ofile ); cout << "\n\n";
//	pair< int*,int* > ia_iter;
//
//	ia_iter = equal_range( &ia[0], &ia[14], 22 );
//
//	cout << "equal_range result of search for value 22:\n\t"
//		<< "*ia_iter.first: " << *ia_iter.first << "\t"
//		<< "*ia_iter.second: " << *ia_iter.second << "\n\n";
//
//	ia_iter = equal_range( &ia[0], &ia[14], 21 );
//
//	cout << "equal_range result of search for "
//		<< "absent value 21:\n\t"
//		<< "*ia_iter.first: " << *ia_iter.first << "\t"
//		<< "*ia_iter.second: " << *ia_iter.second << "\n\n";
//
//	sort( ivec.begin(), ivec.end(), greater<int>() );
//	cout << "vector element sequence after sort:\n";
//	copy( ivec.begin(), ivec.end(), ofile ); cout << "\n\n";
//
//	typedef vector< int >::iterator iter_ivec;
//	pair< iter_ivec, iter_ivec > ivec_iter;
//
//	ivec_iter = equal_range( ivec.begin(), ivec.end(), 26, greater<int>() );
//
//	cout << "equal_range result of search for value 26:\n\t"
//		<< "*ivec_iter.first: " << *ivec_iter.first << "\t"
//		<< "*ivec_iter.second: " << *ivec_iter.second<< "\n\n";
//
//	ivec_iter = equal_range( ivec.begin(), ivec.end(), 21, greater<int>() );
//
//	cout << "equal_range result of search for "
//		<< "absent value 21:\n\t"
//		<< "*ivec_iter.first: " << *ivec_iter.first << "\t"
//		<< "*ivec_iter.second: " << *ivec_iter.second<< "\n\n";
//
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//fill()
//
//template< class ForwardIterator, class Type >
//void fill
//( ForwardIterator first, ForwardIterator last, const Type& value );

int main(int argc, char* argv[])
{
	const int value = 9;
	int ia[] = { 0, 1, 1, 2, 3, 5, 8 };
	ostream_iterator< int > ofile( cout, " " );
	cout << "original array element sequence:\n";
	copy( ia, ia+7, ofile ); cout << "\n\n";
	fill( ia+1, ia+6, value );
	cout << "array after fill(ia+1,ia+6):\n";
	copy( ia, ia+7, ofile ); cout << "\n\n";

	string the_lang( "c++" );
	string langs[5] = { "c", "eiffel", "java", "ada", "perl" };
	list< string > il( langs, langs+5 );
	ostream_iterator< string > sofile( cout, " " );
	cout << "original list element sequence:\n";
	copy( il.begin(), il.end(), sofile ); cout << "\n\n";
	typedef list<string>::iterator iterator;
	iterator ibegin = il.begin(), iend = il.end();
	fill( ++ibegin, --iend, the_lang );
	cout << "list after fill(++ibegin,--iend):\n";
	copy( il.begin(), il.end(), sofile ); cout << "\n\n";
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//fill_n()
//
//template< class ForwardIterator, class Size, class Type >
//void fill_n
//( ForwardIterator first, Size n, const Type& value );

class print_elements_new
{
public:
	void operator()( string elem ) 
	{
		cout << elem << ( _line_cnt++%8 ? " " : "\n\t" );
	}
	static void reset_line_cnt() { _line_cnt = 1; }
private:
	static int _line_cnt;
};

int print_elements_new::_line_cnt = 1;

//int main(int argc, char* argv[])
//{
//	int value = 9; int count = 3;
//	int ia[] = { 0, 1, 1, 2, 3, 5, 8 };
//	ostream_iterator< int > iofile( cout, " " );
//	cout << "original element sequence of array container:\n";
//	copy( ia, ia+7, iofile ); cout << "\n\n";
//	fill_n( ia+2, count, value );
//	cout << "array after fill_n( ia+2, 3, 9 ):\n";
//	copy( ia, ia+7, iofile ); cout << "\n\n";
//
//	string replacement( "xxxxx" );
//	string sa[] = { "Stephen", "closed", "his", "eyes", "to",
//		"hear", "his", "boots", "crush", "crackling",
//		"wrack", "and", "shells" };
//	vector< string > svec( sa, sa+13 );
//	cout << "original sequence of strings:\n\t";
//	for_each( svec.begin(), svec.end(), print_elements_new() );
//	cout << "\n\n";
//	fill_n( svec.begin()+3, count*2, replacement );
//	print_elements_new::reset_line_cnt();
//	cout << "sequence after fill_n() applied:\n\t";
//	for_each( svec.begin(), svec.end(), print_elements_new() );
//
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//find() 
//
//template< class InputIterator, class T >
//InputIterator find
//( InputIterator first, InputIterator last, const T &value );

//int main(int argc, char* argv[])
//{
//	int array[ 17 ] = { 7,3,3,7,6,5,8,7,2,1,3,8,7,3,8,4,3 };
//	int elem = array[ 9 ];
//	int *found_it;
//	found_it = find( &array[0], &array[17], elem );
//	// 结果: find the first occurrence of 1 found!
//	cout << "find the first occurrence of "<< elem << "\t"
//		<< ( found_it ? "found!\n" : "not found!\n" );
//
//	string beethoven[] = {"Sonata31", "Sonata32", "Quartet14", "Quartet15","Archduke", "Symphony7" };
//	string s_elem( beethoven[ 1 ] );
//	list< string > slist( beethoven, beethoven+6 );
//	list< string >::iterator iter;
//	iter = find( slist.begin(), slist.end(), s_elem );
//	// 结果: find the first occurrence of Sonata32 found!
//	cout << "find the first occurrence of "<< s_elem << "\t"
//		<< ( iter != slist.end() ? "found!\n" : "not found!\n" );
//
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//find_if()
//
//template< class InputIterator, class Predicate >
//InputIterator find_if
//( InputIterator first, InputIterator last, Predicate pred );

// 提供另一种等于操作符 如字符串包含在成员对象的友元集中返回 true
//class OurFriends 
//{
//public:
//	bool operator()( const string& str ) 
//	{
//		return ( friendset.count( str ));
//	}
//	static void	FriendSet( const string *fs, int count ) 
//	{
//			copy( fs, fs+count,	inserter( friendset, friendset.end() ));
//	}
//private:
//	static set< string, less<string> > friendset;
//};
//
//set< string, less<string> > OurFriends::friendset;
//
//int main(int argc, char* argv[])
//{
//	string Pooh_friends[] = { "Piglet", "Tigger", "Eyeore" };
//	string more_friends[] = { "Quasimodo", "Chip", "Piglet" };
//	list<string> lf( more_friends, more_friends+3 );
//	// 生成 pooh_friends 列表
//	OurFriends::FriendSet( Pooh_friends, 3 );
//	list<string>::iterator our_mutual_friend;
//	our_mutual_friend = find_if( lf.begin(), lf.end(), OurFriends());
//	// 结果:Ah, imagine our friend Piglet is also a friend of Pooh.
//	if ( our_mutual_friend != lf.end() )
//		cout << "Ah, imagine our friend "<< *our_mutual_friend<< " is also a friend of Pooh.\n";
//
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//find_end()
//
//template< class ForwardIterator1, class ForwardIterator2 >
//ForwardIterator1 find_end
//( ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2 );
//
//template< class ForwardIterator1, class ForwardIterator2, class BinaryPredicate >
//ForwardIterator1 find_end
//( ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2,	BinaryPredicate pred );

//int main(int argc, char* argv[])
//{
//	int array[ 17 ] = { 7,3,3,7,6,5,8,7,2,1,3,7,6,3,8,4,3 };
//	int subarray[ 3 ] = { 3, 7, 6 };
//	int *found_it;
//	// 在数组中查找最后一次出现的 3,7,6 序列 返回首元素的地址...
//	found_it = find_end( &array[0], &array[17],	&subarray[0], &subarray[3] );
//	assert( found_it == &array[10] );
//	vector< int > ivec( array, array+17 );
//	vector< int > subvec( subarray, subarray+3 );
//	vector< int >::iterator found_it2;
//	found_it2 = find_end( ivec.begin(), ivec.end(),subvec.begin(), subvec.end(), equal_to<int>() );
//	assert( found_it2 == ivec.begin()+10 );
//	cout << "ok: find_end correctly returned beginning of "<< "last matching sequence: 3,7,6!\n";
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//find_first_of()
//
//template< class ForwardIterator1, class ForwardIterator2 >
//ForwardIterator1 find_first_of
//( ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2);
//
//template< class ForwardIterator1, class ForwardIterator2, class BinaryPredicate >
//ForwardIterator1 find_first_of
//( ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2, BinaryPredicate pred );

//int main(int argc, char* argv[])
//{
//	string s_array[] = { "Ee", "eE", "ee", "Oo", "oo", "ee" };
//	string to_find[] = { "oo", "gg", "ee" };
//	// 返回第一次出现的 "ee" -- &s_array[2]
//	string *found_it = find_first_of( s_array, s_array+6, to_find, to_find+3 );
//	// 结果: 
//	//found it: ee  
//	//&s_array[2]: 0x7fff2dac  
//	//&found_it: 0x7fff2dac
//	if ( found_it != &s_array[6] )
//		cout << "found it: " << *found_it << "\n\t"
//		<< "&s_array[2]:\t" << &s_array[2] << "\n\t"
//		<< "&found_it:\t" << found_it << "\n\n";
//
//	vector< string > svec( s_array, s_array+6);
//	vector< string > svec_find( to_find, to_find+3 );
//	// 返回找到的 "oo" -- svec.end()-2
//	vector< string >::iterator found_it2;
//	found_it2 = find_first_of(svec.begin(), svec.end(),
//		svec_find.begin(), svec_find.end(), equal_to<string>());
//	// 结果:
//	// found it, too: oo
//	// &svec.end()-2: 0x100067b0
//	// &found_it2: 0x100067b0
//	if ( found_it2 != svec.end() ) 
//		cout << "found it, too: " << *found_it2 << "\n\t";
//			//<< "&svec.end()-2:\t" << svec.end()-2 << "\n\t"
//			//<< "&found_it2:\t" << found_it2 << "\n";
//
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//for_each()
//
//template< class InputIterator, class Function >
//Function for_each
//( InputIterator first, InputIterator last, Function func );

//template <class Type>
//void print_elements_new1( Type elem ) { cout << elem << " "; }
//
//int main(int argc, char* argv[])
//{
//	vector< int > ivec;
//	for ( int ix = 0; ix < 10; ix++ )
//		ivec.push_back( ix );
//	void (*pfi)( int ) = print_elements_new1;
//	for_each( ivec.begin(), ivec.end(), pfi );
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//generate()
//
//template< class ForwardIterator, class Generator >
//void generate
//( ForwardIterator first, ForwardIterator last, Generator gen );

int odd_by_twos() 
{
	static int seed = -1;
	return seed += 2;
}
template <class Type>
void print_elements_new2( Type elem ) { cout << elem << " "; }

//int main(int argc, char* argv[])
//{
//	list< int > ilist( 10 );
//	void (*pfi)( int ) = print_elements_new2;
//	generate( ilist.begin(), ilist.end(), odd_by_twos );
//	// 结果:
//	// elements within list the first invocation:
//	// 1 3 5 7 9 11 13 15 17 19
//	cout << "elements within list the first invocation:\n";
//	for_each( ilist.begin(), ilist.end(), pfi );
//	generate( ilist.begin(), ilist.end(), odd_by_twos );
//	// 结果:
//	// elements within list the second iteration:
//	// 21 23 25 27 29 31 33 35 37 39
//	cout << "\n\nelements within list the second iteration:\n";
//	for_each( ilist.begin(), ilist.end(), pfi );
//
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//generate_n()
//
//template< class OutputIterator, class Size, class Generator >
//void generate_n( OutputIterator first, Size n, Generator gen );

class even_by_twos
{
public:
	even_by_twos( int seed = 0 ) : _seed( seed ){}
	int operator()() { return _seed += 2; }
private:
	int _seed;
};

//int main(int argc, char* argv[])
//{
//	list< int > ilist( 10 );
//	void (*pfi)( int ) = print_elements_new2;
//	generate_n( ilist.begin(), ilist.size(), even_by_twos() );
//	// 结果:
//	// generate_n with even_by_twos():
//	// 2 4 6 8 10 12 14 16 18 20
//	cout << "generate_n with even_by_twos():\n";
//	for_each( ilist.begin(), ilist.end(), pfi ); cout << "\n";
//	generate_n(ilist.begin(),ilist.size(),even_by_twos(100));
//	// 结果:
//	// generate_n with even_by_twos( 100 ):
//	// 102 104 106 108 110 112 114 116 118 120
//	cout << "generate_n with even_by_twos( 100 ):\n";
//	for_each( ilist.begin(), ilist.end(), pfi );
//
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//includes()
//
//template< class InputIterator1, class InputIterator2 >
//bool includes
//( InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2 );
//
//template< class InputIterator1, class InputIterator2, class Compare >
//bool includes
//( InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2,	Compare comp );

//int main(int argc, char* argv[])
//{
//	int ia1[] = { 13, 1, 21, 2, 0, 34, 5, 1, 8, 3, 21, 34 };
//	int ia2[] = { 21, 2, 8, 5, 1 };
//	// includes 必须传入已排序的容器
//	sort( ia1, ia1+12 ); sort( ia2, ia2+5 );
//	// 结果: every element of ia2 contained in ia1? true
//	bool res = includes( ia1, ia1+12, ia2, ia2+5 );
//	cout << "every element of ia2 contained in ia1? "<< (res ? "true" : "false") << endl;
//	vector< int > ivect1( ia1, ia1+12 );
//	vector< int > ivect2( ia2, ia2+5 );
//	// 按降序排序
//	sort( ivect1.begin(), ivect1.end(), greater<int>() );
//	sort( ivect2.begin(), ivect2.end(), greater<int>() );
//	res = includes( ivect1.begin(), ivect1.end(),
//		ivect2.begin(), ivect2.end(),greater<int>() );
//	// 结果:
//	// every element of ivect2 contained in ivect1? true
//	cout << "every element of ivect2 contained in ivect1? "<< (res ? "true" : "false") << endl;
//
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//inner_product()
//
//template < class InputIterator1, class InputIterator2, class Type >
//Type inner_product
//(InputIterator1 first1, InputIterator1 last, InputIterator2 first2, Type init );
//
//template < class InputIterator1, class InputIterator2,
//class Type, class BinaryOperation1, class BinaryOperation2 >
//Type inner_product
//(InputIterator1 first1, InputIterator1 last,
//InputIterator2 first2, Type init,
//BinaryOperation1 op1, BinaryOperation2 op2 );

//int main(int argc, char* argv[])
//{
//	int ia[] = { 2, 3, 5, 8 };
//	int ia2[] = { 1, 2, 3, 4 };
//	// 两个数组的元素两两相乘,并将结果添加到初始值: 0
//	int res = inner_product( &ia[0], &ia[4], &ia2[0], 0);
//	cout << "inner product of arrays: "<< res << endl;	// 结果: inner product of arrays: 55
//
//	vector<int> vec( ia, ia+4 );
//	vector<int> vec2( ia2, ia2+4 );
//	// 两个向量中的元素相加,并从初始值中减去和: 0
//	res = inner_product( vec.begin(), vec.end(),vec2.begin(), 0,minus<int>(), plus<int>() );
//	cout << "inner product of vectors: "<< res << endl;	// 结果: inner product of vectors: -28
//
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//inplace_merge()
//
//template< class BidirectionalIterator >
//void inplace_merge
//( BidirectionalIterator first, BidirectionalIterator middle, BidirectionalIterator last );
//
//template< class BidirectionalIterator, class Compare >
//void inplace_merge
//( BidirectionalIterator first, BidirectionalIterator middle, BidirectionalIterator last, Compare comp );

template <class Type>
void print_elements_new3( Type elem ) { cout << elem << " "; }

//int main(int argc, char* argv[])
//{
//	int ia[] = { 29,23,20,17,15,26,51,12,35,40,74,16,54,21,44,62,10,41,65,71 };
//	vector< int > ivec( ia, ia+20 );
//	void (*pfi)( int ) = print_elements_new3;
//	sort( &ia[0], &ia[10] );	// 以一事实上排序排列两上子序列
//	sort( &ia[10], &ia[20] );
//	cout << "ia sorted into two sub-arrays: \n";
//	for_each( ia, ia+20, pfi ); cout << "\n\n";
//	inplace_merge( ia, ia+10, ia+20 );
//	cout << "ia inplace_merge:\n";
//	for_each( ia, ia+20, pfi ); cout << "\n\n";
//
//	sort( ivec.begin(), ivec.begin()+10, greater<int>() );
//	sort( ivec.begin()+10, ivec.end(), greater<int>() );
//	cout << "ivec sorted into two sub-vectors: \n";
//	for_each( ivec.begin(), ivec.end(), pfi ); cout << "\n\n";
//	inplace_merge( ivec.begin(), ivec.begin()+10,ivec.end(), greater<int>() );
//	cout << "ivec inplace_merge:\n";
//	for_each( ivec.begin(), ivec.end(), pfi ); cout << endl;
//
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//iter_swap()
//
//template <class ForwardIterator1, class ForwardIterator2>
//void iter_swap 
//( ForwardIterator1 a, ForwardIterator2 b );

//int main(int argc, char* argv[])
//{
//	int ia[] = { 5, 4, 3, 2, 1, 0 };
//	list< int > ilist( ia, ia+6 );
//	typedef list< int >::iterator iterator;
//	iterator iter1 = ilist.begin(), iter2, iter_end = ilist.end();
//	// 对列表进行冒泡排序...
//	for ( ; iter1 != iter_end; ++iter1 )
//		for ( iter2 = iter1; iter2 != iter_end; ++iter2 )
//			if ( *iter2 < *iter1 )
//				iter_swap( iter1, iter2 );
//	// 输出结果为:
//	// ilist after bubble sort using iter_swap():
//	// { 0 1 2 3 4 5 }
//	cout << "ilist afer bubble sort using iter_swap(): { ";
//	for ( iter1 = ilist.begin(); iter1 != iter_end; ++iter1 )
//		cout << *iter1 << " ";
//	cout << "}\n";
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//lexicographical_compare()
//
//template <class InputIterator1, class InputIterator2 >
//bool lexicographical_compare
//( InputIterator1 first1, InputIterator1 last1,InputIterator2 first2, InputIterator2 last2 );
//
//template < class InputIterator1, class InputIterator2,class Compare >
//bool lexicographical_compare
//( InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, Compare comp );

class size_compare 
{
public:
	bool operator()( const string &a, const string &b ) 
	{
		return a.length() < b.length();
	}
};

//int main(int argc, char* argv[])
//{
//	string arr1[] = { "Piglet", "Pooh", "Tigger" };
//	string arr2[] = { "Piglet", "Pooch", "Eeyore" };
//	bool res;
//	// 第二个元素值为 false
//	// Pooch 小于 Pooh
//	// 第二个元素值也为 false
//	res = lexicographical_compare( arr1, arr1+3, arr2, arr2+3 );
//	assert( res == false );
//	// 值为 true: ilist2 每个元素的
//	// 长度都小于或等于
//	// 对应的 ilist1 的元素
//	list< string > ilist1( arr1, arr1+3 );
//	list< string > ilist2( arr2, arr2+3 );
//	res = lexicographical_compare(ilist1.begin(), ilist1.end(),
//		ilist2.begin(), ilist2.end(), size_compare() );
//
//	assert( res == true );
//	cout << "ok: lexicographical_compare succeeded!\n";
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//lower_bound()
//
//template< class ForwardIterator, class Type >
//ForwardIterator lower_bound
//( ForwardIterator first, ForwardIterator last, const Type &value );
//
//template< class ForwardIterator, class Type, class Compare >
//ForwardIterator lower_bound
//( ForwardIterator first, ForwardIterator last, const Type &value, Compare comp );

//int main(int argc, char* argv[])
//{
//	int ia[] = {29,23,20,22,17,15,26,51,19,12,35,40};
//	sort( &ia[0], &ia[12] );
//	int search_value = 18;
//	int *ptr = lower_bound( ia, ia+12, search_value );
//	// 结果:
//	// The first element 18 can be inserted in front of is 19
//	// The previous value is 17
//	cout << "The first element "
//		<< search_value
//		<< " can be inserted in front of is "
//		<< *ptr << endl
//		<< "The previous value is "
//		<< *(ptr-1) << endl;
//
//	vector< int > ivec( ia, ia+12 );
//	sort( ivec.begin(), ivec.end(), greater<int>() );	// 降序排序...
//	search_value = 26;
//	vector< int >::iterator iter;
//	iter = lower_bound( ivec.begin(), ivec.end(),search_value, greater<int>() );	// 告诉它这里所用的正确的排序关系...
//	// 结果:
//	// The first element 26 can be inserted in front of is 26
//	// The previous value is 29
//	cout << "The first element "
//		<< search_value
//		<< " can be inserted in front of is "
//		<< *iter << endl
//		<< "The previous value is "
//		<< *(iter-1) << endl;
//
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//max()
//
//template< class Type >
//const Type& max
//( const Type &aval, const Type &bval );
//
//template< class Type, class Compare >
//const Type& max
//( const Type &aval, const Type &bval, Compare comp );

//int main(int argc, char* argv[])
//{
//	cout<<max('a','b')<<endl;
//	cout<<max('a','b',greater<char>())<<endl;
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//max_element()
//
//template< class ForwardIterator >
//ForwardIterator max_element
//( ForwardIterator first,ForwardIterator last );
//
//template< class ForwardIterator, class Compare >
//ForwardIterator max_element
//( ForwardIterator first, ForwardIterator last, Compare comp );


//max_element()返回一个iterator 指向[first,last)序列中值为最大的元素第一个版本使用
//底层元素类型的大于操作符第二个版本使用比较操作comp

//int main(int argc, char* argv[])
//{
//	//...
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//min()
//
//template< class Type >
//const Type& min
//( const Type &aval, const Type &bval );
//
//template< class Type, class Compare >
//const Type& min
//( const Type &aval, const Type &bval, Compare comp );

//min()返回aval 和bval 两个元素中较小的一个第一个版本使用与Type 相关联的小于操
//作符第二个版本使用比较操作comp

//int main(int argc, char* argv[])
//{
//	//...
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//min_element()
//
//template< class ForwardIterator >
//ForwardIterator min_element
//( ForwardIterator first, ForwardIterator last );
//
//template< class ForwardIterator, class Compare >
//ForwardIterator min_element
//( ForwardIterator first, ForwardIterator last, Compare comp );

//min_element()返回一个iterator 指向[first,last)序列中值为最小的元素第一个版本使用
//底层元素类型的小于操作符第二个版本使用比较操作comp

//int main(int argc, char* argv[])
//{
//	int ia[] = { 7, 5, 2, 4, 3 };
//	const vector< int > ivec( ia, ia+5 );
//
//	int mval = max( max( max( max( ivec[4], ivec[3]),ivec[2]),ivec[1]),ivec[0]);
//
//	// 输出: the result of nested invocations of max() is: 7
//	cout << "the result of nested invocations of max() is: "<< mval << endl;
//
//	mval = min( min( min( min( ivec[4], ivec[3]),ivec[2]),ivec[1]),ivec[0]);
//
//	// 输出: the result of nested invocations of min() is: 2
//	cout << "the result of nested invocations of min() is: "<< mval << endl;
//
//	vector< int >::const_iterator iter;
//	iter = max_element( ivec.begin(), ivec.end() );
//
//	// 输出: the result of invoking max_element() is also: 7
//	cout << "the result of invoking max_element() is also: "<< *iter << endl;
//
//	iter = min_element( ivec.begin(), ivec.end() );
//	// 输出: the result of invoking min_element() is also: 2
//	cout << "the result of invoking min_element() is also: "<< *iter << endl;
//
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//merge()
//
//template< class InputIterator1, class InputIterator2, class OutputIterator >
//OutputIterator merge
//( InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2,
// OutputIterator result );
//
//template< class InputIterator1, class InputIterator2, class OutputIterator, class Compare >
//OutputIterator merge
//( InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2,
// OutputIterator result, Compare comp );


template <class Type>
void print_elements_new4( Type elem ) { cout << elem << " "; }

//int main(int argc, char* argv[])
//{
//	void (*pfi)( int ) = print_elements_new4;
//
//	int ia[] = {29,23,20,22,17,15,26,51,19,12,35,40};
//	int ia2[] = {74,16,39,54,21,44,62,10,27,41,65,71};
//	vector< int > vec1( ia, ia +12 ),vec2( ia2, ia2+12 );
//	int ia_result[24];
//	vector<int> vec_result(vec1.size()+vec2.size());
//	sort( ia, ia +12 );
//	sort( ia2, ia2+12 );
//	// 输出:
//	// 10 12 15 16 17 19 20 21 22 23 26 27 29 35
//	// 39 40 41 44 51 54 62 65 71 74
//	merge( ia, ia+12, ia2, ia2+12, ia_result );
//	for_each( ia_result, ia_result+24, pfi ); cout << "\n\n";
//	sort( vec1.begin(), vec1.end(), greater<int>() );
//	sort( vec2.begin(), vec2.end(), greater<int>() );
//	merge( vec1.begin(), vec1.end(),vec2.begin(), vec2.end(),vec_result.begin(), greater<int>() );
//	// 输出:
//	// 74 71 65 62 54 51 44 41 40 39 35 29 27 26 23 22
//	// 21 20 19 17 16 15 12 10
//	for_each( vec_result.begin(), vec_result.end(), pfi );
//	cout << "\n\n";
//
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//mismatch()
//
//template< class InputIterator1, class InputIterator2 >
//pair<InputIterator1, InputIterator2> mismatch
//( InputIterator1 first1, InputIterator1 last, InputIterator2 first2 );
//
//template< class InputIterator1, class InputIterator2, class BinaryPredicate >
//pair<InputIterator1, InputIterator2> mismatch
//( InputIterator1 first1, InputIterator1 last, InputIterator2 first2, BinaryPredicate pred );

class equal_and_odd_new
{
public:
	bool operator()( int ival1, int ival2 )
	{
		return ( ival1 == ival2 && ( ival1 == 0 || ival1%2 ));// 两个值相等吗, 或都为 0 或都为奇数
	}
};

//int main(int argc, char* argv[])
//{
//	int ia[] = { 0,1,1,2,3,5,8,13 };
//	int ia2[] = { 0,1,1,2,4,6,10 };
//	pair<int*,int*> pair_ia = mismatch( ia, ia+7, ia2 );
//	// 输出: first mismatched pair: ia: 3 and ia2: 4
//	cout << "first mismatched pair: ia: "
//		<< *pair_ia.first << " and ia2: "
//		<< *pair_ia.second << endl;
//
//	list<int> ilist( ia, ia+7 );
//	list<int> ilist2( ia2, ia2+7 );
//	typedef list<int>::iterator iter;
//	pair< iter,iter > pair_ilist =mismatch( ilist.begin(), ilist.end(), ilist2.begin(), equal_and_odd_new() );
//	// 输出:
//	// first mismatched pair either not equal or not odd:
//	// ilist: 2 and ilist2: 2
//	cout << "first mismatched pair either not equal "
//		<< "or not odd: \n\tilist: "
//		<< *pair_ilist.first << " and ilist2: "
//		<< *pair_ilist.second << endl;
//
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//next_permutation()
//
//template< class BidirectionalIterator >
//bool next_permutation
//( BidirectionalIterator first, BidirectionalIterator last );
//
//template< class BidirectionalIterator, class Compare >
//bool next_permutation
//( BidirectionalIterator first, BidirectionalIterator last, Compare comp );

//void print_char( char elem ) { cout << elem ; }
//
//int main(int argc, char* argv[])
//{
//	void (*ppc)( char ) = print_char;
//
//	vector<char> vec(5);
//	// 字符顺序: musil
//	vec[0] = 'm'; vec[1] = 'u'; vec[2] = 's';
//	vec[3] = 'i'; vec[4] = 'l';
//	int cnt = 2;
//	sort( vec.begin(), vec.end() );
//	for_each( vec.begin(), vec.end(), ppc ); cout << "\t";
//	while( next_permutation( vec.begin(), vec.end()))	// 生成 "musil" 的所有排列组合
//	{
//		for_each( vec.begin(), vec.end(), ppc );
//		cout << "\t";
//		if ( ! ( cnt++ % 8 )) {
//			cout << "\n";
//			cnt = 1;
//		}
//	}
//	cout << "\n\n";
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//nth_element()
//
//template< class RandomAccessIterator >
//void nth_element
//( RandomAccessIterator first, RandomAccessIterator nth, RandomAccessIterator last );
//
//template< class RandomAccessIterator, class Compare >
//void nth_element
//( RandomAccessIterator first, RandomAccessIterator nth, RandomAccessIterator last, Compare comp );

//int main(int argc, char* argv[])
//{
//	int ia[] = {29,23,20,22,17,15,26,51,19,12,35,40};
//	vector< int > vec( ia, ia+12 );
//	ostream_iterator<int> out( cout," " );
//	cout << "original order of the vector: ";
//	copy( vec.begin(), vec.end(), out ); cout << endl;
//	cout << "sorting vector based on element "<< *( vec.begin()+6 ) << endl;
//	nth_element( vec.begin(), vec.begin()+6, vec.end() );
//	copy( vec.begin(), vec.end(), out ); cout << endl;
//	cout << "sorting vector in descending order "
//		<< "based on element "<< *( vec.begin()+6 ) << endl;
//	nth_element( vec.begin(), vec.begin()+6, vec.end(), greater<int>() );
//	copy( vec.begin(), vec.end(), out ); cout << endl;
//
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//partial_sort()
//
//template< class RandomAccessIterator >
//void partial_sort
//( RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last );
//
//template< class RandomAccessIterator, class Compare >
//void partial_sort
//( RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last, Compare comp );

//int main(int argc, char* argv[])
//{
//	//...
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//partial_sort_copy()
//
//template< class InputIterator, class RandomAccessIterator >
//RandomAccessIterator partial_sort_copy
//( InputIterator first, InputIterator last, RandomAccessIterator result_first, RandomAccessIterator result_last );
//
//template< class InputIterator, class RandomAccessIterator,class Compare >
//RandomAccessIterator partial_sort_copy
//( InputIterator first, InputIterator last, RandomAccessIterator result_first, RandomAccessIterator result_last,	Compare comp );

//int main(int argc, char* argv[])
//{
//	int ia[] = {69,23,80,42,17,15,26,51,19,12,35,8 };
//	vector< int > vec( ia, ia+12 );
//	ostream_iterator<int> out( cout," " );
//	cout << "original order of vector: ";
//	copy( vec.begin(), vec.end(), out ); cout << endl;
//	cout << "partial sort of vector: seven elements\n";
//	partial_sort( vec.begin(), vec.begin()+7, vec.end() );
//	copy( vec.begin(), vec.end(), out ); cout << endl;
//	vector< int > res(7);
//	cout << "partial_sort_copy() of first seven elements\n\t"<< "of vector in descending order\n";
//	partial_sort_copy( vec.begin(), vec.begin()+7, res.begin(), res.end(), greater<int>() );
//	copy( res.begin(), res.end(), out ); cout << endl;
//
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//partial_sum()
//
//template < class InputIterator, Class OutputIterator >
//OutputIterator partial_sum
//(InputIterator first, InputIterator last, OutputIterator result );
//
//template < class InputIterator, Class OutputIterator, class BinaryOperation >
//OutputIterator partial_sum
//(InputIterator first, InputIterator last, OutputIterator result, BinaryOperation op );

//int main(int argc, char* argv[])
//{
//	const int ia_size = 7;
//	int ia[ ia_size ] = { 1, 3, 4, 5, 7, 8, 9 };
//	int ia_res[ ia_size ];
//	ostream_iterator< int > outfile( cout, " " );
//	vector< int > vec( ia, ia+ia_size );
//	vector< int > vec_res( vec.size() );
//	cout << "elements: ";
//	copy( ia, ia+ia_size, outfile ); cout << endl;
//	cout << "partial sum of elements:\n";
//	partial_sum( ia, ia+ia_size, ia_res );
//	copy( ia_res, ia_res+ia_size, outfile ); cout << endl;
//	cout << "partial sum of elements using times<int>():\n";
//	partial_sum( vec.begin(), vec.end(), vec_res.begin(),multiplies<int>() );
//	copy( vec_res.begin(), vec_res.end(), outfile );
//	cout << endl;
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//partition()
//
//template < class BidirectionalIterator, class UnaryPredicate >
//BidirectionalIterator partition
//( BidirectionalIterator first, BidirectionalIterator last, UnaryPredicate pred );

class even_elem 
{
public:
	bool operator()( int elem )
	{ return elem%2 ? false : true; }
};

//int main(int argc, char* argv[])
//{
//	const int ia_size = 12;
//	int ia[ia_size] = { 29,23,20,22,17,15,26,51,19,12,35,40 };
//	vector< int > vec( ia, ia+ia_size );
//	ostream_iterator< int > outfile( cout, " " );
//	cout << "original order of elements: \n";
//	copy( vec.begin(), vec.end(), outfile ); cout << endl;
//	cout << "partition based on whether element is even:\n";
//	partition( &ia[0], &ia[ia_size], even_elem() );
//	copy( ia, ia+ia_size, outfile ); cout << endl;
//	cout << "partition based on whether element "<< "is less than 25:\n";
//	partition( vec.begin(), vec.end(), bind2nd(less<int>(),25) );
//	copy( vec.begin(), vec.end(), outfile ); cout << endl;
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//prev_permutation()
//
//template < class BidirectionalIterator >
//bool prev_permutation
//( BidirectionalIterator first, BidirectionalIterator last );
//
//template < class BidirectionalIterator, class Compare >
//bool prev_permutation
//( BidirectionalIterator first, BidirectionalIterator last, Compare comp );

//int main(int argc, char* argv[])
//{
//	vector< char > vec( 3 );
//	ostream_iterator< char > out_stream( cout, "" );
//	vec[0] = 'n'; vec[1] = 'd'; vec[2] = 'a';
//	copy( vec.begin(), vec.end(), out_stream ); cout << "\t";
//	// 生成 "dan" 的所有排列
//	while( prev_permutation( vec.begin(), vec.end() ))
//	{
//		copy( vec.begin(), vec.end(), out_stream );
//		cout << "\t";
//	}
//	cout << "\n\n";
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//random_shuffle()
//
//template< class RandomAccessIterator >
//void random_shuffle
//( RandomAccessIterator first, RandomAccessIterator last );
//
//template< class RandomAccessIterator,class RandomNumberGenerator >
//void random_shuffle
//( RandomAccessIterator first, RandomAccessIterator last,	RandomNumberGenerator rand );

//int main(int argc, char* argv[])
//{
//	vector< int > vec;
//	for ( int ix = 0; ix < 20; ix++ )
//		vec.push_back( ix );
//	random_shuffle( vec.begin(), vec.end() );
//	// 输出:
//	// random_shuffle of sequence of values 1 .. 20:
//	// 6 11 9 2 18 12 17 7 0 15 4 8 10 5 1 19 13 3 14 16
//	cout << "random_shuffle of sequence of values 1 .. 20:\n";
//	copy( vec.begin(), vec.end(), ostream_iterator< int >( cout," " ));
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//remove()
//
//template< class ForwardIterator, class Type >
//ForwardIterator remove
//( ForwardIterator first, ForwardIterator last, const Type &value );

//int main(int argc, char* argv[])
//{
//	//...
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//remove_copy()
//
//template< class InputIterator, class OutputIterator, class Type >
//OutputIterator remove_copy
//( InputIterator first, InputIterator last, OutputIterator result, const Type &value );

//int main(int argc, char* argv[])
//{
//	int value = 0;
//	int ia[] = { 0, 1, 0, 2, 0, 3, 0, 4, 0, 5 };
//	vector< int > vec( ia, ia+10 );
//	ostream_iterator< int > ofile( cout," ");
//	vector< int >::iterator vec_iter;
//	cout << "original vector sequence:\n";
//	copy( vec.begin(), vec.end(), ofile ); cout << '\n';
//	vec_iter = remove( vec.begin(), vec.end(), value );
//	cout << "vector after remove, without applying erase():\n";
//	copy( vec.begin(), vec.end(), ofile ); cout << '\n';
//	// erase the invalid elements from container
//	vec.erase( vec_iter, vec.end() );
//	cout << "vector after erase():\n";
//	copy( vec.begin(), vec.end(), ofile ); cout << '\n';
//	int ia2[5];
//	vector< int > vec2( ia, ia+10 );
//	remove_copy( vec2.begin(), vec2.end(), ia2, value );
//	cout << "array after remove_copy():\n";
//	copy( ia2, ia2+5, ofile ); cout << endl;
//
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//remove_if()
//
//template< class ForwardIterator, class Predicate >
//ForwardIterator remove_if
//( ForwardIterator first, ForwardIterator last, Predicate pred );

//int main(int argc, char* argv[])
//{
//	//...
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//remove_copy_if()
//
//template< class InputIterator, class OutputIterator, class Predicate >
//OutputIterator remove_copy_if
//( InputIterator first, InputIterator last, OutputIterator result, Predicate pred );


class EvenValue 
{
public:
	bool operator()( int value )
	{ return value % 2 ? false : true; }
};

//int main(int argc, char* argv[])
//{
//	int ia[] = { 0, 1, 1, 2, 3, 5, 8, 13, 21, 34 };
//	vector< int >::iterator iter;
//	vector< int > vec( ia, ia+10 );
//	ostream_iterator< int > ofile( cout, " " );
//	cout << "original element sequence:\n";
//	copy( vec.begin(), vec.end(), ofile ); cout << '\n';
//	iter = remove_if( vec.begin(), vec.end(), bind2nd(less<int>(),10) );
//	vec.erase( iter, vec.end() );
//	cout << "sequence after applying remove_if < 10:\n";
//	copy( vec.begin(), vec.end(), ofile ); cout << '\n';
//	vector< int > vec_res( 10 );
//	iter = remove_copy_if( ia, ia+10, vec_res.begin(), EvenValue() );
//	cout << "sequence after applying remove_copy_if even:\n";
//	copy( vec_res.begin(), iter, ofile ); cout << '\n';
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//replace()
//
//template< class ForwardIterator, class Type >
//void replace
//( ForwardIterator first, ForwardIterator last, const Type& old_value, const Type& new_value );
//
//int main(int argc, char* argv[])
//{
//	//...
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//replace_copy()
//
//template< class InputIterator, class OutputIterator, class Type >
//OutputIterator replace_copy
//( InputIterator first, InputIterator last, OutputIterator result, const Type& old_value, const Type& new_value );

//int main(int argc, char* argv[])
//{
//	string oldval( "Mr. Winnie the Pooh" );
//	string newval( "Pooh" );
//	ostream_iterator< string > ofile( cout, " " );
//	string sa[] = {"Christopher Robin", "Mr. Winnie the Pooh",
//		"Piglet", "Tigger", "Eeyore"};
//	vector< string > vec( sa, sa+5 );
//	cout << "original element sequence:\n";
//	copy( vec.begin(), vec.end(), ofile ); cout << '\n';
//	replace( vec.begin(), vec.end(), oldval, newval );
//	cout << "sequence after applying replace():\n";
//	copy( vec.begin(), vec.end(), ofile ); cout << '\n';
//	vector< string > vec2;
//	replace_copy( vec.begin(), vec.end(), inserter( vec2, vec2.begin() ), newval, oldval );
//	cout << "sequence after applying replace_copy():\n";
//	copy( vec2.begin(), vec2.end(), ofile ); cout << '\n';
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//replace_if()
//
//template< class ForwardIterator, class Predicate, class Type >
//void replace_if
//( ForwardIterator first, ForwardIterator last, Predicate pred, const Type& new_value );

//int main(int argc, char* argv[])
//{
//	//...
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//replace_copy_if()
//
//template< class ForwardIterator, class OutputIterator, class Predicate, class Type >
//OutputIterator replace_copy_if
//( ForwardIterator first, ForwardIterator last, OutputIterator result, Predicate pred, const Type& new_value );

class EvenValue_new
{
public:
	bool operator()( int value ) 
	{return value % 2 ? false : true; }
};

//int main(int argc, char* argv[])
//{
//	int new_value = 0;
//	int ia[] = { 0, 1, 1, 2, 3, 5, 8, 13, 21, 34 };
//	vector< int > vec( ia, ia+10 );
//	ostream_iterator< int > ofile( cout, " " );
//	cout << "original element sequence:\n";
//	copy( ia, ia+10, ofile ); cout << '\n';
//	replace_if( &ia[0], &ia[10], bind2nd(less<int>(),10), new_value );
//	cout << "sequence after applying replace_if < 10 with 0:\n";
//	copy( ia, ia+10, ofile ); cout << '\n';
//	replace_if( vec.begin(), vec.end(),	EvenValue_new(), new_value );
//	cout << "sequence after applying replace_if even with 0:\n";
//	copy( vec.begin(), vec.end(), ofile ); cout << '\n';
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//reverse()
//
//template< class BidirectionalIterator >
//void reverse
//( BidirectionalIterator first, BidirectionalIterator last );

//int main(int argc, char* argv[])
//{
//	//...
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//reverse_copy()
//
//template< class BidirectionalIterator, class OutputIterator >
//OutputIterator reverse_copy
//( BidirectionalIterator first, BidirectionalIterator last, OutputIterator result );

class print_elements_new5
{
public:
	void operator()( string elem ) 
	{
		cout << elem << ( _line_cnt++%8 ? " " : "\n\t" );
	}
	static void reset_line_cnt() { _line_cnt = 1; }
private:
	static int _line_cnt;
};
int print_elements_new5::_line_cnt = 1;

//int main(int argc, char* argv[])
//{
//	string sa[] = { "Signature", "of", "all", "things",
//		"I", "am", "here", "to", "read",
//		"seaspawn", "and", "seawrack", "that",
//		"rusty", "boot"};
//	list< string > slist( sa, sa+15 );
//	cout << "Original sequence of strings:\n\t";
//	for_each( slist.begin(), slist.end(), print_elements() );
//	cout << "\n\n";
//	reverse( slist.begin(), slist.end() );
//	print_elements::reset_line_cnt();
//	cout << "Sequence after reverse() applied:\n\t";
//	for_each( slist.begin(), slist.end(), print_elements() );
//	cout << "\n";
//	list< string > slist_copy( slist.size() );
//	reverse_copy( slist.begin(), slist.end(), slist_copy.begin() );
//
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//rotate()
//
//template< class ForwardIterator >
//void rotate
//( ForwardIterator first, ForwardIterator middle, ForwardIterator last );
//
//int main(int argc, char* argv[])
//{
//	//...
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//rotate_copy()
//
//template< class ForwardIterator, class OutputIterator >
//OutputIterator rotate_copy
//( ForwardIterator first, ForwardIterator middle, ForwardIterator last, OutputIterator result );

//int main(int argc, char* argv[])
//{
//	int ia[] = { 1, 3, 5, 7, 9, 0, 2, 4, 6, 8, 10 };
//	vector< int > vec( ia, ia+11 );
//	ostream_iterator< int > ofile( cout, " " );
//	cout << "original element sequence:\n";
//	copy( vec.begin(), vec.end(), ofile ); cout << '\n';
//	rotate( &ia[0], &ia[5], &ia[11] );
//	cout << "rotate on middle element(0) ::\n";
//	copy( ia, ia+11, ofile ); cout << '\n';
//	rotate( vec.begin(), vec.end()-2, vec.end() );
//	cout << "rotate on next to last element(8) ::\n";
//	copy( vec.begin(), vec.end(), ofile ); cout << '\n';
//	vector< int > vec_res( vec.size() );
//	rotate_copy( vec.begin(), vec.begin()+vec.size()/2, vec.end(), vec_res.begin() );
//	cout << "rotate_copy on middle element ::\n";
//	copy( vec_res.begin(), vec_res.end(), ofile );
//	cout << '\n';
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//search()
//
//template< class ForwardIterator1, class ForwardIterator2 >
//ForwardIterator search
//( ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2 );
//
//template< class ForwardIterator1, class ForwardIterator2, class BinaryPredicate >
//ForwardIterator	search
//( ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2, BinaryPredicate pred );

//int main(int argc, char* argv[])
//{
//	ostream_iterator< char > ofile( cout, "" );
//	char str[ 25 ] = "a fine and private place";
//	char substr[] = "ate";
//	char *found_str = search(str,str+25,substr,substr+3);
//	cout << "Expecting to find the substring 'ate': ";
//	copy( found_str, found_str+3, ofile ); cout << '\n';
//	vector< char > vec( str, str+24 );
//	vector< char > subvec(3);
//	subvec[0]='v'; subvec[1]='a'; subvec[2]='t';
//	vector< char >::iterator iter;
//	iter = search( vec.begin(), vec.end(), subvec.begin(), subvec.end(), equal_to< char >() );
//	cout << "Expecting to find the substring 'vat': ";
//	copy( iter, iter+3, ofile ); cout << '\n';
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//search_n()
//
//template< class ForwardIterator, class Size, class Type >
//ForwardIterator search_n
//( ForwardIterator first, ForwardIterator last, Size count, const Type &value );
//
//template< class ForwardIterator, class Size, class Type, class BinaryPredicate >
//ForwardIterator	search_n
//( ForwardIterator first, ForwardIterator last, Size count, const Type &value, BinaryPredicate pred );

//int main(int argc, char* argv[])
//{
//	ostream_iterator< char > ofile( cout, "" );
//	const char blank = ' ';
//	const char oh = 'o';
//	char str[ 26 ] = "oh my a mouse ate a moose";
//	char *found_str = search_n( str, str+25, 2, oh );
//	cout << "Expecting to find two instances of 'o': ";
//	copy( found_str, found_str+2, ofile ); cout << '\n';
//	vector< char > vec( str, str+25 );
//	// 寻找第一个这样的序列其中三个字符都不是空格: mouse 中的 mou
//	vector< char >::iterator iter;
//	iter = search_n( vec.begin(), vec.end(), 3, blank, not_equal_to< char >() );
//	cout << "Expecting to find the substring 'mou': ";
//	copy( iter, iter+3, ofile ); cout << '\n';
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//set_difference()
//
//template < class InputIterator1, class InputIterator2, class OutputIterator >
//OutputIterator set_difference
//( InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2,	OutputIterator result );
//
//template < class InputIterator1, class InputIterator2, class OutputIterator, class Compare >
//OutputIterator set_difference
//( InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result, Compare comp );


////set_difference()构造一个排过序的序列其中的元素出现在第一个序列中由[first,last)
////标记但是不包含在第二个序列中由[first2,last2]标记例如已失两个序列{0,1,2,3}
////和{0,2,4,6} 则差集为{1,3} 返回的OutputIterator 指向被放入result 所标记的容器中的最后
////元素的下一个位置第一个版本假设该序列是用底层元素类型的小于操作符来排序的第二
////个版本假设该序列是用comp 来排序的

//int main(int argc, char* argv[])
//{
//	//...
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//set_intersection()
//
//template < class InputIterator1, class InputIterator2, class OutputIterator >
//OutputIterator set_intersection
//(InputIterator1 first1, InputIterator1 last1,
//InputIterator2 first2, InputIterator2 last2,
//OutputIterator result );
//
//template < class InputIterator1, class InputIterator2, class OutputIterator, class Compare >
//OutputIterator set_intersection
//(InputIterator1 first1, InputIterator1 last1,
//InputIterator2 first2, InputIterator2 last2,
//OutputIterator result, Compare comp );

//set_intersection()构造一个排过序的序列其中的元素在[first1,last1)和[first2,last2)序列
//中都存在例如已知序列{0,1,2,3}和{0,2,4,6} 则交集为{0,2} 这些元素被从第一个序列
//中拷贝出来返回的OutputIterator 指向被放入result 所标记的容器内的最后元素的下一个位
//置第一个版本假设该序列是用底层类型的小于操作符来排序的而第二个版本假设该序列
//是用comp 来排序的

//int main(int argc, char* argv[])
//{
//	//...
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//set_symmetric_difference()
//
//template < class InputIterator1, class InputIterator2, class OutputIterator >
//OutputIterator set_symmetric_difference
//(InputIterator1 first1, InputIterator1 last1,
//InputIterator2 first2, InputIterator2 last2,
//OutputIterator result );
//
//template < class InputIterator1, class InputIterator2, class OutputIterator, class Compare >
//OutputIterator set_symmetric_difference
//(InputIterator1 first1, InputIterator1 last1,
//InputIterator2 first2, InputIterator2 last2,
//OutputIterator result, Compare comp );

//set_symmetric_difference()构造一个排过序的序列其中的元素在第一个序列中出现但
//不出现在第二个序列中或者在第二个序列中出现但不出现在第一个序列中例如已知
//两个序列{0,1,2,3}和{0,2,4,6} 则对称差集是{1,3,4,6} 返回的OutputIterator 指向被放入result
//所标记的容器内的最后元素的下一个位置第一个版本假设该序列是用底层类型的小于操作
//符来排序的而第二个版本假设该序列是用comp 来排序的

//int main(int argc, char* argv[])
//{
//	//...
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//set_union()
//
//template < class InputIterator1, class InputIterator2, class OutputIterator >
//OutputIterator set_union
//( InputIterator1 first1, InputIterator1 last1,
//	InputIterator2 first2, InputIterator2 last2,
//	OutputIterator result );
//
//template < class InputIterator1, class InputIterator2, class OutputIterator, class Compare >
//OutputIterator set_union
//( InputIterator1 first1, InputIterator1 last1,
//	InputIterator2 first2, InputIterator2 last2,
//	OutputIterator result, Compare comp );

//set_union()构造一个排过序的序列它包含了[first1,last1)和[first2,last2)这两个范围内的
//所有元素例如已知两个序列{0,1,2,3}和{0,2,4,6} 则并集为{0,1,2,3,4,6} 如果一个元素
//在两个容器中都存在比如0 和2 则拷贝第一个容器中的元素返回的OutputIterator 指向
//被放入result 所标记的容器内的最后元素的下一个位置第一个版本假设该序列是用底层类
//型的小于操作符来排序的而第二个版本假设该序列是用comp 来排序的

//int main(int argc, char* argv[])
//{
//	string str1[] = { "Pooh", "Piglet", "Tigger", "Eeyore" };
//	string str2[] = { "Pooh", "Heffalump", "Woozles" };
//	ostream_iterator< string > ofile( cout, " " );
//	set<string,less<string>> set1( str1, str1+4 );
//	set<string,less<string>> set2( str2, str2+3 );
//	cout << "set #1 elements:\n\t";
//	copy( set1.begin(), set1.end(), ofile ); cout << "\n\n";
//	cout << "set #2 elements:\n\t";
//	copy( set2.begin(), set2.end(), ofile ); cout << "\n\n";
//
//	set<string,less<string>> res;
//	set_union( set1.begin(), set1.end(), set2.begin(), set2.end(),
//		inserter( res, res.begin() ));
//	cout << "set_union() elements:\n\t";
//	copy( res.begin(), res.end(), ofile ); cout << "\n\n";
//	res.clear();
//
//	set_intersection( set1.begin(), set1.end(), set2.begin(), set2.end(),
//		inserter( res, res.begin() ));
//	cout << "set_intersection() elements:\n\t";
//	copy( res.begin(), res.end(), ofile ); cout << "\n\n";
//	res.clear();
//
//	set_difference( set1.begin(), set1.end(), set2.begin(), set2.end(),
//		inserter( res, res.begin() ));
//	cout << "set_difference() elements:\n\t";
//	copy( res.begin(), res.end(), ofile ); cout << "\n\n";
//	res.clear();
//
//	set_symmetric_difference( set1.begin(), set1.end(), set2.begin(), set2.end(),
//		inserter( res, res.begin() ));
//	cout << "set_symmetric_difference() elements:\n\t";
//	copy( res.begin(), res.end(), ofile ); cout << "\n\n";
//
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//sort()
//
//template< class RandomAccessIterator >
//void sort
//( RandomAccessIterator first, RandomAccessIterator last );
//
//template< class RandomAccessIterator, class Compare >
//void sort
//( RandomAccessIterator first, RandomAccessIterator last, Compare comp );

//int main(int argc, char* argv[])
//{
//	//...
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//stable_partition()
//
//template< class BidirectionalIterator, class Predicate >
//BidirectionalIterator stable_partition
//( BidirectionalIterator first, BidirectionalIterator last, Predicate pred );

class even_elem_new
{
public:
	bool operator()( int elem )
	{
		return elem%2 ? false : true;
	}
};

//int main(int argc, char* argv[])
//{
//	int ia[] = { 29,23,20,22,17,15,26,51,19,12,35,40 };
//	vector< int > vec( ia, ia+12 );
//	ostream_iterator< int > ofile( cout, " " );
//	cout << "original element sequence:\n";
//	copy( vec.begin(), vec.end(), ofile ); cout << '\n';
//	stable_partition( &ia[0], &ia[12], even_elem_new() );
//	cout << "stable_partition on even element:\n";
//	copy( ia, ia+11, ofile ); cout << '\n';
//	stable_partition( vec.begin(), vec.end(),bind2nd(less<int>(),25) );
//	cout << "stable_partition of less than 25:\n";
//	copy( vec.begin(), vec.end(), ofile ); cout << '\n';
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//stable_sort()
//
//template< class RandomAccessIterator >
//void stable_sort( RandomAccessIterator first, RandomAccessIterator last );
//
//template< class RandomAccessIterator, class Compare >
//void stable_sort
//( RandomAccessIterator first, RandomAccessIterator last, Compare comp );

//int main(int argc, char* argv[])
//{
//	int ia[] = { 29,23,20,22,12,17,15,26,51,19,12,23,35,40 };
//	vector< int > vec( ia, ia+14 );
//	ostream_iterator< int > ofile( cout, " " );
//	cout << "original element sequence:\n";
//	copy( vec.begin(), vec.end(), ofile ); cout << '\n';
//	stable_sort( &ia[0], &ia[14] );
//	cout << "stable sort -- default ascending order:\n";
//	copy( ia, ia+14, ofile ); cout << '\n';
//	stable_sort( vec.begin(), vec.end(), greater<int>() );
//	cout << "stable sort: descending order:\n";
//	copy( vec.begin(), vec.end(), ofile ); cout << '\n';
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//swap()
//
//template< class Type >
//void swap 
//( Type &ob1, Type &ob2 );

//int main(int argc, char* argv[])
//{
//	int ia[] = { 3, 4, 5, 0, 1, 2 };
//	vector< int > vec( ia, ia+6 );
//	for ( int ix = 0; ix < 6; ++ix )
//		for ( int iy = ix; iy < 6; ++iy )
//		{
//			if ( vec[iy] < vec[ ix ] )
//				swap( vec[iy], vec[ix] );
//		}
//	ostream_iterator< int > ofile( cout, " " );
//	cout << "original element sequence:\n";
//	copy( ia, ia+6, ofile ); cout << '\n';
//	cout << "sequence applying swap() "<< "to support bubble sort:\n";
//	copy( vec.begin(), vec.end(), ofile ); cout << '\n';
//
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//swap_range()
//
//template <class ForwardIterator1, class ForwardIterator2 >
//ForwardIterator2 swap_range
//( ForwardIterator1 first1, ForwardIterator1 last, ForwardIterator2 first2 );

//int main(int argc, char* argv[])
//{
//	int ia[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
//	int ia2[] = { 5, 6, 7, 8, 9 };
//	vector< int > vec( ia, ia+10 );
//	vector< int > vec2( ia2, ia2+5 );
//	ostream_iterator< int > ofile( cout, " " );
//	cout << "original element sequence of first container:\n";
//	copy( vec.begin(), vec.end(), ofile ); cout << '\n';
//	cout << "original element sequence of second container:\n";
//	copy( vec2.begin(), vec2.end(), ofile ); cout << '\n';
//
//	swap_ranges( &ia[0], &ia[5], &ia[5] );	// 在同一序列中进行交换
//	cout << "array after swap_ranges() in middle of array:\n";
//	copy( ia, ia+10, ofile ); cout << '\n';
//
//	vector< int >::iterator last = find( vec.begin(), vec.end(), 5 );	// 跨容器交换
//	swap_ranges( vec.begin(), last, vec2.begin() );
//	cout << "first container after " << "swap_ranges() of two vectors:\n";
//	copy( vec.begin(), vec.end(), ofile ); cout << '\n';
//	cout << "second container after " << "swap_ranges() of two vectors:\n";
//	copy( vec2.begin(), vec2.end(), ofile ); cout << '\n';
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//transform()
//
//template< class InputIterator, class OutputIterator, class UnaryOperation >
//OutputIterator transform
//( InputIterator first, InputIterator last, OutputIterator result, UnaryOperation op );
//
//template< class InputIterator1, class InputIterator2, class OutputIterator, class BinaryOperation >
//OutputIterator transform
//( InputIterator1 first1, InputIterator1 last, InputIterator2 first2, OutputIterator result,	BinaryOperation bop );

int double_val( int val ) { return val + val; }
int difference( int val1, int val2 ) { return abs( val1 - val2 ); }

//int main(int argc, char* argv[])
//{
//	int ia[] = { 3, 5, 8, 13, 21 };
//	vector<int> vec( 5 );
//	ostream_iterator<int> outfile( cout, " " );
//	cout << "original array values: ";
//	copy( ia, ia+5, outfile ); cout << endl;
//	cout << "transform each element by doubling: ";
//	transform( ia, ia+5, vec.begin(), double_val );
//	copy( vec.begin(), vec.end(), outfile ); cout << endl;
//	cout << "transform each element by difference: ";
//	transform( ia, ia+5, vec.begin(), outfile, difference );
//	cout << endl;
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//unique()
//
//template< class ForwardIterator >
//ForwardIterator unique( ForwardIterator first, ForwardIterator last );
//
//template< class ForwardIterator, class BinaryPredicate >
//ForwardIterator unique
//( ForwardIterator first, ForwardIterator last, BinaryPredicate pred );

//对于连续的元素如果它们包含相同的值使用底层类型的等于操作符来判断或者
//把它们传给pred 的计算结果都为true 则这些元素被折叠成一个元素例如在单词mississippi
//中语义上的结果是misisipi 注意四个i 不是连续的所以不会被折叠类似地因为两
//对s 也是不连续的所以也没有被折叠成单个实例为了保证所有重复的实例都被折叠起来
//我们必须先对容器进行排序
//实际上unique()的行为有些不太直观类似于remove()算法在这两种情况下容器的
//实际大小并没有变化每个惟一的元素都被依次拷贝到从first 开始的下一个空闲位置上
//因此在我们的例子中实际的结果是misisipippi 这里的ppi 字符序列可以说是算法
//的残留物返回的ForwordIterator 指向残留物的起始处典型的做法是这个iterator 被传递
//给erase() 以便删除无效的元素由于内置数组不支持erase()操作所以unique()不太适
//合于数组unique_copy()对数组更为合适一些

//int main(int argc, char* argv[])
//{
//	//...
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//unique_copy()
//
//template< class InputIterator, class OutputIterator >
//OutputIterator unique_copy
//( InputIterator first, InputIterator last, OutputIterator result );
//
//template< class InputIterator, class OutputIterator, class BinaryPredicate >
//OutputIterator unique_copy
//( InputIterator first, InputIterator last, OutputIterator result, BinaryPredicate pred );

template <class Type>
void print_elements_new6( Type elem ) { cout << elem << " "; }

void (*pfi)( int ) = print_elements_new6;
void (*pfs)( string ) = print_elements_new6;

//int main(int argc, char* argv[])
//{
//	int ia[] = { 0, 1, 0, 2, 0, 3, 0, 4, 0, 5 };
//	vector<int> vec( ia, ia+10 );
//	vector<int>::iterator vec_iter;
//	// 生成不能交换的序列: 没有连续的 0
//	// 结果: 0 1 0 2 0 3 0 4 0 5
//	vec_iter = unique( vec.begin(), vec.end() );
//	for_each( vec.begin(), vec.end(), pfi ); cout << "\n\n";
//	// 排了序的向量: 0 0 0 0 0 1 2 3 4 5
//	// 应用 unique() 后:
//	// 结果: 0 1 2 3 4 5 2 3 4 5
//	sort( vec.begin(), vec.end() );
//	vec_iter = unique( vec.begin(), vec.end() );
//	for_each( vec.begin(), vec.end(), pfi ); cout << "\n\n";
//	// 从容器中删除无效元素
//	// 结果: 0 1 2 3 4 5
//	vec.erase( vec_iter, vec.end() );
//	for_each( vec.begin(), vec.end(), pfi ); cout << "\n\n";
//	string sa[] = { "enough", "is", "enough", "enough", "is", "good" };
//	vector<string> svec( sa, sa+6 );
//	vector<string> vec_result( svec.size() );
//	vector<string>::iterator svec_iter;
//	sort( svec.begin(), svec.end() );
//	svec_iter = unique_copy( svec.begin(), svec.end(), vec_result.begin() );
//	// 结果: enough good is
//	for_each( vec_result.begin(), svec_iter, pfs );
//	cout << "\n\n";
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//upper_bound()
//
//template< class ForwardIterator, class Type >
//ForwardIterator upper_bound
//( ForwardIterator first, ForwardIterator last, const Type &value );
//
//template< class ForwardIterator, class Type, class Compare >
//ForwardIterator upper_bound
//( ForwardIterator first, ForwardIterator last, const Type &value, Compare comp );

//upper_bound()返同一个iterator 它指向在[first,last)标记的有序序列中可以插入value
//而不会破坏容器顺序的最后一个位置这个位置标记了一个大于value 的值例如已知序
//列int ia[] = {12,15,17,19,20,22,23,26,29,35,40,51};
//用值21 调用upper_bound() 返回一个指向值22 的iterator 用值22 调用upper_bound()
//则返回一个指向值23 的iterator 第一个版本使用底层类型的小于操作符而第二个版本根
//据comp 对元素进行排序和比较

template <class Type>
void print_elements_new7( Type elem ) { cout << elem << " "; }
void (*pfi_new)( int ) = print_elements_new7;

//int main(int argc, char* argv[])
//{
//	int ia[] = {29,23,20,22,17,15,26,51,19,12,35,40};
//	vector<int> vec(ia,ia+12);
//	sort(ia,ia+12);
//	int *iter = upper_bound(ia,ia+12,19);
//	assert( *iter == 20 );
//	sort( vec.begin(), vec.end(), greater<int>() );
//	vector<int>::iterator iter_vec;
//	iter_vec = upper_bound( vec.begin(), vec.end(),	27, greater<int>() );
//	assert( *iter_vec == 26 );
//	// 输出: 51 40 35 29 27 26 23 22 20 19 17 15 12
//	vec.insert( iter_vec, 27 );
//	for_each( vec.begin(), vec.end(), pfi_new ); cout << "\n\n";
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//堆算法
//标准库提供的heap 堆是一个max-heap 所谓max-heap 是一个用数组表示的二叉树
//它的每个节点上的键值大于或等于其儿子节点的键值完整的讨论见SEDGEWIOK88
//另外一种表示是min-heap 其中每个节点的键值小于或等于其儿子节点的键值在标准
//库的表示中最大的键值可以把它想像成树的根总是在数组的开始处例如以下的字
//母序列满足堆的要求
//满足堆要求的字母序列
//X T O G S M N A E R A I
//在这个例子中X 是根节点有一个左儿子T 和右儿子O 注意两个儿子之间的顺序
//是不要求的即左儿子不必小于右儿子G 和S 是T 的儿子而M 和N 是O 的儿子
//类似地A 和E 是G 的儿子R 和A 是S 的儿子I 是M 的左儿子而N 是叶节点没有
//儿子
//四个泛型堆算法make_heap() pop_heap() push_heap()和sort_heap()为堆的创建和操
//纵提供了支持后三个算法假定由iterator 对标记的序列代表了一个真正的堆如果该序列
//不是一个堆的话则算法的运行时刻行为是未定义的注意list 容器不能被用作堆因
//为它不支持随机访问内置数组可以被用来支持一个堆也是pop_heap()和push_heap()算法
//难以与数组一起使用因为这两个算法要求改变数组的大小我们先简要介绍这四个算法
//然后用一个小程序说明它们的用法
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//make_heap()
//
//template< class RandomAccessIterator >
//void make_heap
//( RandomAccessIterator first, RandomAccessIterator last );
//
//template< class RandomAccessIterator, class Compare >
//void make_heap
//( RandomAccessIterator first, RandomAccessIterator last, Compare comp );

//int main(int argc, char* argv[])
//{
//	//...
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//pop_heap()
//
//template< class RandomAccessIterator >
//void pop_heap
//( RandomAccessIterator first, RandomAccessIterator last );
//
//template< class RandomAccessIterator, class Compare >
//void pop_heap
//( RandomAccessIterator first, RandomAccessIterator last, Compare comp );

//int main(int argc, char* argv[])
//{
//	//...
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//push_heap()
//
//template< class RandomAccessIterator >
//void push_heap
//( RandomAccessIterator first, RandomAccessIterator last );
//
//template< class RandomAccessIterator, class Compare >
//void push_heap
//( RandomAccessIterator first, RandomAccessIterator last, Compare comp );

//int main(int argc, char* argv[])
//{
//	//...
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//sort_heap()
//
//template< class RandomAccessIterator >
//void sort_heap
//( RandomAccessIterator first, RandomAccessIterator last );
//
//template< class RandomAccessIterator, class Compare >
//void sort_heap
//( RandomAccessIterator first, RandomAccessIterator last, Compare comp );

template <class Type>
void print_elements_new8( Type elem ) { cout << elem << " "; }

//int main(int argc, char* argv[])
//{
//	int ia[] = {29,23,20,22,17,15,26,51,19,12,35,40 };
//	vector< int > vec( ia, ia+12 );
//	make_heap( &ia[0], &ia[12] );	// 结果: 51 35 40 23 29 20 26 22 19 12 17 15
//
//	void (*pfi)( int ) = print_elements_new8;
//	for_each( ia, ia+12, pfi ); cout << "\n\n";
//	make_heap( vec.begin(), vec.end(), greater<int>() );	// 结果: 12 17 15 19 23 20 26 51 22 29 35 40
//															// a min-heap: root is smallest element
//	for_each( vec.begin(), vec.end(), pfi ); cout << "\n\n";
//	
//	sort_heap( ia, ia+12 );	// 结果: 12 15 17 19 20 22 23 26 29 35 40 51
//	for_each( ia, ia+12, pfi ); cout << "\n\n";
//	vec.push_back( 8 );	// 再加一个新的最小的元素:
//
//	// 结果: 8 17 12 19 23 15 26 51 22 29 35 40 20
//	// 将最新最小的元素放在根处
//	push_heap( vec.begin(), vec.end(), greater<int>() );
//	for_each( vec.begin(), vec.end(), pfi ); cout << "\n\n";
//	// 结果: 12 17 15 19 23 20 26 51 22 29 35 40 8
//	// 应用次最小的元素替代最小的
//	pop_heap( vec.begin(), vec.end(), greater<int>() );
//	for_each( vec.begin(), vec.end(), pfi ); cout << "\n\n";
//
//	return 0;
//}

