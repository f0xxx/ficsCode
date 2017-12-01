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
�����㷨:
adjacent_find(), binary_search(), count(), count_if(), equal_range(),
find(), find_end(), find_first_of(), find_if(), lower_bound(),
upper_bound(), search(), search_n()

�����ͨ�������㷨:
inplace_merge(), merge(), nth_element(), partial_sort(),
partial_sort_copy(), partition(), random_shuffle(), reverse(),
reverse_copy(), rotate(), rotate_copy(), sort(), stable_sort(),
stable_partition()

ɾ�����滻�㷨:
copy(), copy_backwards(), iter_swap(), remove(), remove_copy(),
remove_if(), remove_copy_if(), replace(), replace_copy(),
replace_if(), replace_copy_if(), swap(), swap_range(), unique(),
unique_copy()

��������㷨:
next_permutation(), prev_permutation()

�����㷨:
accumulate(), partial_sum(), inner_product(), adjacent_difference()

���ɺ�����㷨:
fill(), fill_n(),for_each(), generate(), generate_n(), transform()

��ϵ�㷨:
equal(), includes(), lexicographical_compare(), max(), max_element(),
min(), min_element(), mismatch()

�����㷨:
set_union(), set_intersection(), set_difference(),
set_symmetric_difference()

���㷨:
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
		// ����������Ϣ
		return;
	}
	vector< string> texts;
	vector<textwords>::iterator iter;
	for ( iter = pvec->begin(); iter != pvec->end(); ++iter )
		copy( (*iter).begin(), (*iter).end(),back_inserter( texts ));
	sort( texts.begin(), texts.end() );	// ���� texts ��Ԫ��

	for_each( texts.begin(), texts.end(), PrintElem() );	// ok, ��������һ��������ʲô

	cout << "\n\n"; // ֻ�Ƿָ���ʾ���
	vector<string>::iterator it;
	it = unique( texts.begin(), texts.end() );
	texts.erase( it, texts.end() );	// ɾ���ظ�Ԫ��

	for_each( texts.begin(), texts.end(), PrintElem() );	// ok, ����������һ������������ʲô��

	cout << "\n\n";
	stable_sort( texts.begin(), texts.end(), LessThan() );	// ����ȱʡ�ĳ��� 6 ����Ԫ��stable_sort() �������Ԫ�ص�˳��
	for_each( texts.begin(), texts.end(), PrintElem() );
	cout << "\n\n";

	int cnt = 0;	// �������ȴ���6 ���ַ����ĸ���
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
//	cout << "text file #1: "; cin >> t1fn;	// Ҫ���û������ļ�,ʵ���еĳ���Ӧ����������
//	cout << "text file #2: "; cin >> t2fn;
//	ifstream infile1( t1fn.c_str());	// ���ļ�
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

//listΪ˫������ṹ,�޷�������ʱ�����(���ṩRandomAccessIterator),so�������㷨���ȿ������µĳ�Ա����

//list::merge() �õڶ��������list�ϲ�һ������list
//list::remove() ɾ������ĳ��ֵ��Ԫ��
//list::remove_if() ɾ������ĳ��������Ԫ��
//list::reverse() ��list��Ԫ�ط�������
//list::sort() ����list��Ԫ��
//list::splice() ��һ��list��Ԫ���Ƶ���һ��list��
//list::unique() ɾ��ĳ��Ԫ�ص��ظ���������
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
//	ilist1.sort(); ilist2.sort();	// merge Ҫ������ list �Ѿ�����
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
//	list< int > ilist4( array, array+2 ); // ���� 0, 1
//	//����ʹ��splice()��ilist1 �ĵ�һ��Ԫ���Ƶ�ilist2 ������ilist2 ����Ԫ��0 1 0 ��ilist1 ���ٰ���0
//	
//	// ilist2.end() ָʾҪ�Ӻ�Ԫ�ص�λ�ñ��Ӻϵ�Ԫ���ڸ�λ��֮ǰ
//	// ilist1 ָʾ���ĸ� list ���ƶ�Ԫ��ilist1.begin() ָʾҪ���ƶ���Ԫ��
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
//	//ȥ���ڶ���Ԫ���ܱ���һ��Ԫ������������Ԫ��
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
//	// ���Ϊ: 1 0 1 1 2 3
//	copy( ilist_result.begin(), ilist_result.end(),	ostream_iterator<int>(cout," "));
//	cout << endl;
//	adjacent_difference( ilist.begin(), ilist.end(), ilist_result.begin(), multiplies<int>() );
//	// ���Ϊ: 1 1 2 6 15 40
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
//	piter = adjacent_find( ia, ia+4 );	// piter ָ�� ia[1]
//	assert( *piter == ia[ 1 ] );
//	iter = adjacent_find( vec.begin(), vec.end(), TwiceOver() );	// iter ָ�� vec[2]
//	assert( *iter == vec[ 2 ] );
//	cout << "ok: adjacent-find() succeeded!\n";	// ���������ʾһ��˳��
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
//	copy( ia+1, ia+7, ia );	// ���� 1 λ
//	cout << "shifting array sequence left by 1:\n";
//	copy( ia, ia+7, ofile ); cout << '\n';
//	copy( vec.begin()+2, vec.end(), vec.begin() );	// ���� 2 λ
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
* �������:
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
//	// ���ɽ��Ϊ: count_if(): there are 4 elements that are even.
//	cout << "count_if(): there are "<< ia_count << " elements that are even.\n";
//	int list_count = 0;
//	list_count=count_if( ilist.begin(), ilist.end(),bind2nd(less<int>(),10) );
//	// ���ɽ��Ϊ: count_if(): there are 7 elements that are less than 10.
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

//������������ڷ�Χ[first,last)�ڰ�����Ԫ�ض������equal()����true ����ڶ�����
//���������Ԫ���򲻻ῼ����ЩԪ���������ϣ����֤����������ȫ�������Ҫд
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
//	// true: ������ ia. �ĳ���,���ɽ��Ϊ: int ia[7] equal to int ia2[9]? true.
//	res = equal( &ia[0], &ia[7], &ia2[0] );
//	cout << "int ia[7] equal to int ia2[9]? "<< ( res ? "true" : "false" ) << ".\n";
//	list< int > ilist( ia, ia+7 );
//	list< int > ilist2( ia2, ia2+9 );
//	// ���ɽ��Ϊ: list ilist equal to ilist2? true.
//	res = equal( ilist.begin(), ilist.end(), ilist2.begin() );
//	cout << "list ilist equal to ilist2? "<< ( res ? "true" : "false" ) << ".\n";
//	// false: 0, 2, 8 �����, Ҳ��������
//	// ���ɽ��Ϊ: list ilist equal_and_odd() to ilist2? false.
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
//	// ���: find the first occurrence of 1 found!
//	cout << "find the first occurrence of "<< elem << "\t"
//		<< ( found_it ? "found!\n" : "not found!\n" );
//
//	string beethoven[] = {"Sonata31", "Sonata32", "Quartet14", "Quartet15","Archduke", "Symphony7" };
//	string s_elem( beethoven[ 1 ] );
//	list< string > slist( beethoven, beethoven+6 );
//	list< string >::iterator iter;
//	iter = find( slist.begin(), slist.end(), s_elem );
//	// ���: find the first occurrence of Sonata32 found!
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

// �ṩ��һ�ֵ��ڲ����� ���ַ��������ڳ�Ա�������Ԫ���з��� true
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
//	// ���� pooh_friends �б�
//	OurFriends::FriendSet( Pooh_friends, 3 );
//	list<string>::iterator our_mutual_friend;
//	our_mutual_friend = find_if( lf.begin(), lf.end(), OurFriends());
//	// ���:Ah, imagine our friend Piglet is also a friend of Pooh.
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
//	// �������в������һ�γ��ֵ� 3,7,6 ���� ������Ԫ�صĵ�ַ...
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
//	// ���ص�һ�γ��ֵ� "ee" -- &s_array[2]
//	string *found_it = find_first_of( s_array, s_array+6, to_find, to_find+3 );
//	// ���: 
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
//	// �����ҵ��� "oo" -- svec.end()-2
//	vector< string >::iterator found_it2;
//	found_it2 = find_first_of(svec.begin(), svec.end(),
//		svec_find.begin(), svec_find.end(), equal_to<string>());
//	// ���:
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
//	// ���:
//	// elements within list the first invocation:
//	// 1 3 5 7 9 11 13 15 17 19
//	cout << "elements within list the first invocation:\n";
//	for_each( ilist.begin(), ilist.end(), pfi );
//	generate( ilist.begin(), ilist.end(), odd_by_twos );
//	// ���:
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
//	// ���:
//	// generate_n with even_by_twos():
//	// 2 4 6 8 10 12 14 16 18 20
//	cout << "generate_n with even_by_twos():\n";
//	for_each( ilist.begin(), ilist.end(), pfi ); cout << "\n";
//	generate_n(ilist.begin(),ilist.size(),even_by_twos(100));
//	// ���:
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
//	// includes ���봫�������������
//	sort( ia1, ia1+12 ); sort( ia2, ia2+5 );
//	// ���: every element of ia2 contained in ia1? true
//	bool res = includes( ia1, ia1+12, ia2, ia2+5 );
//	cout << "every element of ia2 contained in ia1? "<< (res ? "true" : "false") << endl;
//	vector< int > ivect1( ia1, ia1+12 );
//	vector< int > ivect2( ia2, ia2+5 );
//	// ����������
//	sort( ivect1.begin(), ivect1.end(), greater<int>() );
//	sort( ivect2.begin(), ivect2.end(), greater<int>() );
//	res = includes( ivect1.begin(), ivect1.end(),
//		ivect2.begin(), ivect2.end(),greater<int>() );
//	// ���:
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
//	// ���������Ԫ���������,���������ӵ���ʼֵ: 0
//	int res = inner_product( &ia[0], &ia[4], &ia2[0], 0);
//	cout << "inner product of arrays: "<< res << endl;	// ���: inner product of arrays: 55
//
//	vector<int> vec( ia, ia+4 );
//	vector<int> vec2( ia2, ia2+4 );
//	// ���������е�Ԫ�����,���ӳ�ʼֵ�м�ȥ��: 0
//	res = inner_product( vec.begin(), vec.end(),vec2.begin(), 0,minus<int>(), plus<int>() );
//	cout << "inner product of vectors: "<< res << endl;	// ���: inner product of vectors: -28
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
//	sort( &ia[0], &ia[10] );	// ��һ��ʵ��������������������
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
//	// ���б����ð������...
//	for ( ; iter1 != iter_end; ++iter1 )
//		for ( iter2 = iter1; iter2 != iter_end; ++iter2 )
//			if ( *iter2 < *iter1 )
//				iter_swap( iter1, iter2 );
//	// ������Ϊ:
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
//	// �ڶ���Ԫ��ֵΪ false
//	// Pooch С�� Pooh
//	// �ڶ���Ԫ��ֵҲΪ false
//	res = lexicographical_compare( arr1, arr1+3, arr2, arr2+3 );
//	assert( res == false );
//	// ֵΪ true: ilist2 ÿ��Ԫ�ص�
//	// ���ȶ�С�ڻ����
//	// ��Ӧ�� ilist1 ��Ԫ��
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
//	// ���:
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
//	sort( ivec.begin(), ivec.end(), greater<int>() );	// ��������...
//	search_value = 26;
//	vector< int >::iterator iter;
//	iter = lower_bound( ivec.begin(), ivec.end(),search_value, greater<int>() );	// �������������õ���ȷ�������ϵ...
//	// ���:
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


//max_element()����һ��iterator ָ��[first,last)������ֵΪ����Ԫ�ص�һ���汾ʹ��
//�ײ�Ԫ�����͵Ĵ��ڲ������ڶ����汾ʹ�ñȽϲ���comp

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

//min()����aval ��bval ����Ԫ���н�С��һ����һ���汾ʹ����Type �������С�ڲ�
//�����ڶ����汾ʹ�ñȽϲ���comp

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

//min_element()����һ��iterator ָ��[first,last)������ֵΪ��С��Ԫ�ص�һ���汾ʹ��
//�ײ�Ԫ�����͵�С�ڲ������ڶ����汾ʹ�ñȽϲ���comp

//int main(int argc, char* argv[])
//{
//	int ia[] = { 7, 5, 2, 4, 3 };
//	const vector< int > ivec( ia, ia+5 );
//
//	int mval = max( max( max( max( ivec[4], ivec[3]),ivec[2]),ivec[1]),ivec[0]);
//
//	// ���: the result of nested invocations of max() is: 7
//	cout << "the result of nested invocations of max() is: "<< mval << endl;
//
//	mval = min( min( min( min( ivec[4], ivec[3]),ivec[2]),ivec[1]),ivec[0]);
//
//	// ���: the result of nested invocations of min() is: 2
//	cout << "the result of nested invocations of min() is: "<< mval << endl;
//
//	vector< int >::const_iterator iter;
//	iter = max_element( ivec.begin(), ivec.end() );
//
//	// ���: the result of invoking max_element() is also: 7
//	cout << "the result of invoking max_element() is also: "<< *iter << endl;
//
//	iter = min_element( ivec.begin(), ivec.end() );
//	// ���: the result of invoking min_element() is also: 2
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
//	// ���:
//	// 10 12 15 16 17 19 20 21 22 23 26 27 29 35
//	// 39 40 41 44 51 54 62 65 71 74
//	merge( ia, ia+12, ia2, ia2+12, ia_result );
//	for_each( ia_result, ia_result+24, pfi ); cout << "\n\n";
//	sort( vec1.begin(), vec1.end(), greater<int>() );
//	sort( vec2.begin(), vec2.end(), greater<int>() );
//	merge( vec1.begin(), vec1.end(),vec2.begin(), vec2.end(),vec_result.begin(), greater<int>() );
//	// ���:
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
		return ( ival1 == ival2 && ( ival1 == 0 || ival1%2 ));// ����ֵ�����, ��Ϊ 0 ��Ϊ����
	}
};

//int main(int argc, char* argv[])
//{
//	int ia[] = { 0,1,1,2,3,5,8,13 };
//	int ia2[] = { 0,1,1,2,4,6,10 };
//	pair<int*,int*> pair_ia = mismatch( ia, ia+7, ia2 );
//	// ���: first mismatched pair: ia: 3 and ia2: 4
//	cout << "first mismatched pair: ia: "
//		<< *pair_ia.first << " and ia2: "
//		<< *pair_ia.second << endl;
//
//	list<int> ilist( ia, ia+7 );
//	list<int> ilist2( ia2, ia2+7 );
//	typedef list<int>::iterator iter;
//	pair< iter,iter > pair_ilist =mismatch( ilist.begin(), ilist.end(), ilist2.begin(), equal_and_odd_new() );
//	// ���:
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
//	// �ַ�˳��: musil
//	vec[0] = 'm'; vec[1] = 'u'; vec[2] = 's';
//	vec[3] = 'i'; vec[4] = 'l';
//	int cnt = 2;
//	sort( vec.begin(), vec.end() );
//	for_each( vec.begin(), vec.end(), ppc ); cout << "\t";
//	while( next_permutation( vec.begin(), vec.end()))	// ���� "musil" �������������
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
//	// ���� "dan" ����������
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
//	// ���:
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
//	// Ѱ�ҵ�һ���������������������ַ������ǿո�: mouse �е� mou
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


////set_difference()����һ���Ź�����������е�Ԫ�س����ڵ�һ����������[first,last)
////��ǵ��ǲ������ڵڶ�����������[first2,last2]���������ʧ��������{0,1,2,3}
////��{0,2,4,6} ��Ϊ{1,3} ���ص�OutputIterator ָ�򱻷���result ����ǵ������е����
////Ԫ�ص���һ��λ�õ�һ���汾������������õײ�Ԫ�����͵�С�ڲ�����������ĵڶ�
////���汾�������������comp �������

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

//set_intersection()����һ���Ź�����������е�Ԫ����[first1,last1)��[first2,last2)����
//�ж�����������֪����{0,1,2,3}��{0,2,4,6} �򽻼�Ϊ{0,2} ��ЩԪ�ر��ӵ�һ������
//�п����������ص�OutputIterator ָ�򱻷���result ����ǵ������ڵ����Ԫ�ص���һ��λ
//�õ�һ���汾������������õײ����͵�С�ڲ�����������Ķ��ڶ����汾���������
//����comp �������

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

//set_symmetric_difference()����һ���Ź�����������е�Ԫ���ڵ�һ�������г��ֵ�
//�������ڵڶ��������л����ڵڶ��������г��ֵ��������ڵ�һ��������������֪
//��������{0,1,2,3}��{0,2,4,6} ��ԳƲ��{1,3,4,6} ���ص�OutputIterator ָ�򱻷���result
//����ǵ������ڵ����Ԫ�ص���һ��λ�õ�һ���汾������������õײ����͵�С�ڲ���
//��������Ķ��ڶ����汾�������������comp �������

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

//set_union()����һ���Ź����������������[first1,last1)��[first2,last2)��������Χ�ڵ�
//����Ԫ��������֪��������{0,1,2,3}��{0,2,4,6} �򲢼�Ϊ{0,1,2,3,4,6} ���һ��Ԫ��
//�����������ж����ڱ���0 ��2 �򿽱���һ�������е�Ԫ�ط��ص�OutputIterator ָ��
//������result ����ǵ������ڵ����Ԫ�ص���һ��λ�õ�һ���汾������������õײ���
//�͵�С�ڲ�����������Ķ��ڶ����汾�������������comp �������

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
//	swap_ranges( &ia[0], &ia[5], &ia[5] );	// ��ͬһ�����н��н���
//	cout << "array after swap_ranges() in middle of array:\n";
//	copy( ia, ia+10, ofile ); cout << '\n';
//
//	vector< int >::iterator last = find( vec.begin(), vec.end(), 5 );	// ����������
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

//����������Ԫ��������ǰ�����ͬ��ֵʹ�õײ����͵ĵ��ڲ��������жϻ���
//�����Ǵ���pred �ļ�������Ϊtrue ����ЩԪ�ر��۵���һ��Ԫ�������ڵ���mississippi
//�������ϵĽ����misisipi ע���ĸ�i �������������Բ��ᱻ�۵����Ƶ���Ϊ��
//��s Ҳ�ǲ�����������Ҳû�б��۵��ɵ���ʵ��Ϊ�˱�֤�����ظ���ʵ�������۵�����
//���Ǳ����ȶ�������������
//ʵ����unique()����Ϊ��Щ��ֱ̫��������remove()�㷨�������������������
//ʵ�ʴ�С��û�б仯ÿ��Ωһ��Ԫ�ض������ο�������first ��ʼ����һ������λ����
//��������ǵ�������ʵ�ʵĽ����misisipippi �����ppi �ַ����п���˵���㷨
//�Ĳ����ﷵ�ص�ForwordIterator ָ����������ʼ�����͵����������iterator ������
//��erase() �Ա�ɾ����Ч��Ԫ�������������鲻֧��erase()��������unique()��̫��
//��������unique_copy()�������Ϊ����һЩ

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
//	// ���ɲ��ܽ���������: û�������� 0
//	// ���: 0 1 0 2 0 3 0 4 0 5
//	vec_iter = unique( vec.begin(), vec.end() );
//	for_each( vec.begin(), vec.end(), pfi ); cout << "\n\n";
//	// �����������: 0 0 0 0 0 1 2 3 4 5
//	// Ӧ�� unique() ��:
//	// ���: 0 1 2 3 4 5 2 3 4 5
//	sort( vec.begin(), vec.end() );
//	vec_iter = unique( vec.begin(), vec.end() );
//	for_each( vec.begin(), vec.end(), pfi ); cout << "\n\n";
//	// ��������ɾ����ЧԪ��
//	// ���: 0 1 2 3 4 5
//	vec.erase( vec_iter, vec.end() );
//	for_each( vec.begin(), vec.end(), pfi ); cout << "\n\n";
//	string sa[] = { "enough", "is", "enough", "enough", "is", "good" };
//	vector<string> svec( sa, sa+6 );
//	vector<string> vec_result( svec.size() );
//	vector<string>::iterator svec_iter;
//	sort( svec.begin(), svec.end() );
//	svec_iter = unique_copy( svec.begin(), svec.end(), vec_result.begin() );
//	// ���: enough good is
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

//upper_bound()��ͬһ��iterator ��ָ����[first,last)��ǵ����������п��Բ���value
//�������ƻ�����˳������һ��λ�����λ�ñ����һ������value ��ֵ������֪��
//��int ia[] = {12,15,17,19,20,22,23,26,29,35,40,51};
//��ֵ21 ����upper_bound() ����һ��ָ��ֵ22 ��iterator ��ֵ22 ����upper_bound()
//�򷵻�һ��ָ��ֵ23 ��iterator ��һ���汾ʹ�õײ����͵�С�ڲ��������ڶ����汾��
//��comp ��Ԫ�ؽ�������ͱȽ�

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
//	// ���: 51 40 35 29 27 26 23 22 20 19 17 15 12
//	vec.insert( iter_vec, 27 );
//	for_each( vec.begin(), vec.end(), pfi_new ); cout << "\n\n";
//	return 0;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//���㷨
//��׼���ṩ��heap ����һ��max-heap ��νmax-heap ��һ���������ʾ�Ķ�����
//����ÿ���ڵ��ϵļ�ֵ���ڻ��������ӽڵ�ļ�ֵ���������ۼ�SEDGEWIOK88
//����һ�ֱ�ʾ��min-heap ����ÿ���ڵ�ļ�ֵС�ڻ��������ӽڵ�ļ�ֵ�ڱ�׼
//��ı�ʾ�����ļ�ֵ���԰�����������ĸ�����������Ŀ�ʼ���������µ���
//ĸ��������ѵ�Ҫ��
//�����Ҫ�����ĸ����
//X T O G S M N A E R A I
//�����������X �Ǹ��ڵ���һ�������T ���Ҷ���O ע����������֮���˳��
//�ǲ�Ҫ��ļ�����Ӳ���С���Ҷ���G ��S ��T �Ķ��Ӷ�M ��N ��O �Ķ���
//���Ƶ�A ��E ��G �Ķ���R ��A ��S �Ķ���I ��M ������Ӷ�N ��Ҷ�ڵ�û��
//����
//�ĸ����Ͷ��㷨make_heap() pop_heap() push_heap()��sort_heap()Ϊ�ѵĴ����Ͳ�
//���ṩ��֧�ֺ������㷨�ٶ���iterator �Ա�ǵ����д�����һ�������Ķ����������
//����һ���ѵĻ����㷨������ʱ����Ϊ��δ�����ע��list �������ܱ���������
//Ϊ����֧�������������������Ա�����֧��һ����Ҳ��pop_heap()��push_heap()�㷨
//����������һ��ʹ����Ϊ�������㷨Ҫ��ı�����Ĵ�С�����ȼ�Ҫ�������ĸ��㷨
//Ȼ����һ��С����˵�����ǵ��÷�
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
//	make_heap( &ia[0], &ia[12] );	// ���: 51 35 40 23 29 20 26 22 19 12 17 15
//
//	void (*pfi)( int ) = print_elements_new8;
//	for_each( ia, ia+12, pfi ); cout << "\n\n";
//	make_heap( vec.begin(), vec.end(), greater<int>() );	// ���: 12 17 15 19 23 20 26 51 22 29 35 40
//															// a min-heap: root is smallest element
//	for_each( vec.begin(), vec.end(), pfi ); cout << "\n\n";
//	
//	sort_heap( ia, ia+12 );	// ���: 12 15 17 19 20 22 23 26 29 35 40 51
//	for_each( ia, ia+12, pfi ); cout << "\n\n";
//	vec.push_back( 8 );	// �ټ�һ���µ���С��Ԫ��:
//
//	// ���: 8 17 12 19 23 15 26 51 22 29 35 40 20
//	// ��������С��Ԫ�ط��ڸ���
//	push_heap( vec.begin(), vec.end(), greater<int>() );
//	for_each( vec.begin(), vec.end(), pfi ); cout << "\n\n";
//	// ���: 12 17 15 19 23 20 26 51 22 29 35 40 8
//	// Ӧ�ô���С��Ԫ�������С��
//	pop_heap( vec.begin(), vec.end(), greater<int>() );
//	for_each( vec.begin(), vec.end(), pfi ); cout << "\n\n";
//
//	return 0;
//}

