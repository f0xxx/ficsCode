#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <functional>
using namespace std;

string program_name( "our_program" );
string version( "0.01" );
string mumble( int *array, int size )
{
	if ( ! array )
	{
		ostringstream out_message;
		out_message << "error: "<< program_name << "--" << version
			<< ": " << __FILE__ << ": " << __LINE__
			<<" -- ptr is set to 0; "<< " must address some array.\n";
		return out_message.str();		// 返回底层 string 对象
	}
	//...
		return "";		// 返回底层 string 对象
}

inline void max_out( int val1, int val2 ) 
{
	//cout << ( val1 > val2 ) ? val1 : val2;
	cout << (( val1 > val2 ) ? val1 : val2);
}

template <class InputIterator>
void filter_string( InputIterator first, InputIterator last, string filt_elems = string("\",?."))
{
	for ( ; first != last; first++ )
	{
		string::size_type pos = 0;
		while (( pos = (*first).find_first_of( filt_elems, pos ))
			!= string::npos )
			(*first).erase( pos, 1 );
	}
}

bool length_less( string s1, string s2 )
{ return s1.size() < s2.size(); }

//int main(int argc, char* argv[])
//{
//	string in_string;
//	
//	cout << "Please enter your name: ";		// 向用户终端写字符串
//	cin >> in_string;		// 把用户输入的读取到 in_string 中
//
//	if ( in_string.empty() )
//		cerr << "error: input string is empty!\n";	// 产生一个错误消息输出到用户终端
//	else
//		cout << "hello, " << in_string << "!\n";
//
//	return 0;
//}

//int main(int argc, char* argv[])
//{
//	string ifile;
//	cout << "Please enter file to sort: ";
//	cin >> ifile;
//	ifstream infile( ifile.c_str() );	// 构造一个 ifstream 输入文件对象
//
//	if( ! infile ) 
//	{
//		cerr << "error: unable to open input file: "<< ifile << endl;
//		return -1;
//	}
//	string ofile = ifile + ".sort";
//	ofstream outfile( ofile.c_str() );	// 构造一个 ofstream 输出文件对象
//
//	if( !outfile ) 
//	{
//		cerr << "error: unable to open output file: "
//			<< ofile << endl;
//		return -2;
//	}
//	string buffer;
//	vector<string> text;
//	int cnt = 1;
//	while ( infile >> buffer ) 
//	{
//		text.push_back( buffer );
//		cout << buffer << ( cnt++ % 8 ? " " : "\n" );
//	}
//	sort( text.begin(), text.end() );
//	vector<string>::iterator iter = text.begin();
//	for ( cnt = 1; iter != text.end(); ++iter, ++cnt )
//		outfile << *iter<< (cnt%8 ? " " : "\n" );		// ok: 把排序后的词打印到 outfile
//
//	return 0;
//}

//int main(int argc, char* argv[])
//{	
//	cout<<mumble(0,0)<<endl;
//	return 0;
//}

//int main(int argc, char* argv[])
//{
//	cout << "The length of \"ulysses\" is:\t";
//	cout << strlen( "ulysses" );
//	cout << '\n';
//	cout << "The size of \"ulysses\" is:\t";
//	cout << sizeof( "ulysses" );
//	cout << endl;
//
//	// 输出操作符可以被连接在一起
//	cout << "The length of \"ulysses\" is:\t"
//		<< strlen( "ulysses" ) << '\n';
//	cout << "The size of \"ulysses\" is:\t"
//		<< sizeof( "ulysses" ) << endl<<endl;
//
//	int i = 1024;
//	int *pi = &i;
//	cout << "i: " << i<< "\t&i:\t" << &i << '\n';
//	cout << "*pi: " << *pi<< "\tpi:\t" << pi << endl
//		<< "\t\t&pi:\t" << &pi << endl<<endl;
//
//	const char *str = "vermeer";
//	const char *pstr = str;
//	cout << "The address of pstr is: "<< pstr << endl;
//	cout << "The address of pstr is: "<< static_cast<void*>(const_cast<char*>(pstr)) << endl<<endl;
//
//	int ix = 10, jx = 20;
//	cout << "The larger of " << ix;
//	cout << ", " << jx << " is ";
//	max_out( ix, jx );
//	cout << endl<<endl;
//
//	cout << "default bool values: "<< true << " " << false
//		<< "\nalpha bool values: "<< boolalpha<< true << " " << false<<endl<<endl;
//
//	return 0;
//}

//int main(int argc, char* argv[])
//{
//	string pooh_pals[] = {"Tigger", "Piglet", "Eeyore", "Rabbit"};
//
//	vector<string> ppals( pooh_pals, pooh_pals+4 );
//	vector<string>::iterator iter = ppals.begin();
//	vector<string>::iterator iter_end = ppals.end();
//	cout << "These are Pooh's pals: ";
//	for ( ; iter != iter_end; iter++ )
//		cout << *iter << " ";
//	cout << endl;
//
//	iter = ppals.begin();
//	cout << "These are Pooh's pals: ";
//	ostream_iterator< string > output( cout, " " );
//
//	copy( iter, iter_end, output );	// 把每个元素拷贝到 cout ...
//	cout << endl;
//
//	return 0;
//}

//int main(int argc, char* argv[])
//{
//	int item_number;
//	string item_name;
//	double item_price;
//	cout << "Please enter the item_number, item_name, and price: "<< endl;
//	cin >> item_number;	//有两种情况会使一个istream对象被计算为false,读到文件结束或遇到一个无效的值
//	cin >> item_name;
//	cin >> item_price;	
//	cout << "The values entered are: item# "
//		<< item_number << " "
//		<< item_name << " @$"
//		<< item_price << endl;
//
//	return 0;
//}

int main(int argc, char* argv[])
{
	char ch;
	while ( cin >> ch )	// 读入每个字符然后输出不包括空白字符
		cout << ch;
	cout << endl;

	//while ( cin.get( ch ))	// 获取每个字符包括空白字符
	//	cout.put( ch );
	//cout << endl;
	//return 0;
}

//int main(int argc, char* argv[])
//{
//
//	istream_iterator< string > in( cin ), eos ;
//	vector< string > text ;
//	copy( in , eos , back_inserter( text ) ) ;	// 从标准输入向 text 拷贝值
//
//	sort( text.begin() , text.end() ) ;
//	vector< string >::iterator it ;
//	it = unique( text.begin() , text.end() ) ;
//	text.erase( it , text.end() ) ;	// 删除所有重复的值
//
//	int line_cnt = 1 ;
//	for ( vector< string >::iterator iter = text.begin();
//		iter != text.end() ; ++iter , ++line_cnt )
//		cout << *iter<< ( line_cnt % 9 ? " " : "\n" ) ;// 显示结果 vector
//	cout << endl;
//	return 0;
//}

//int main(int argc, char* argv[])
//{
//	const int bufSize = 24;
//	char buf[ bufSize ], largest[ bufSize ];
//	int curLen, max = -1, cnt = 0;	// 存放统计数;
//
//	while ( cin >>/* setw( bufSize ) >> */buf )	//setw()把长度等于或大于bufSize的字符串分成
//											//最大长度为bufSize - 1的两个或多个字符串
//	{
//		curLen = strlen( buf );
//		++cnt;
//		if ( curLen > max )		// new longest word? save it.
//		{
//			max = curLen;
//			strcpy( largest, buf );
//		}
//	}
//	cout << "The number of words read is "<< cnt << endl;
//	cout << "The longest word has a length of "<< max << endl;
//	cout << "The longest word is "<< largest << endl;
//	return 0;
//}

//int main(int argc, char* argv[])
//{
//	string buf, largest;
//	int curLen, max = -1, cnt = 0;	// 存放统计数:
//
//	while ( cin >> buf ) 
//	{
//		curLen = buf.size();
//		++cnt;
//		if ( curLen > max )	// 又出现最长单词了? 保存
//		{
//			max = curLen;
//			largest = buf;
//		}
//	}
//	cout << "The number of words read is "<< cnt << endl;
//	cout << "The longest word has a length of "<< max << endl;
//	cout << "The longest word is "<< largest << endl;
//	return 0;
//}

//int main(int argc, char* argv[])
//{
//	istream_iterator< string > input( cin ), eos;
//	vector< string > text;
//	copy( input, eos, back_inserter( text ));	// copy 是一个泛型算法
//
//	string filt_elems( "\",.?;:");
//	filter_string( text.begin(), text.end(), filt_elems );
//	int cnt = text.size();
//	vector< string >::iterator max = max_element( text.begin(), text.end(), length_less);	// max_element 是一个泛型算法
//
//	int len = max->size();
//	cout << "The number of words read is "<< cnt << endl;
//	cout << "The longest word has a length of "<< len << endl;
//	cout << "The longest word is "<< *max << endl;
//	return 0;
//}

//int main(int argc, char* argv[])
//{
//	int lineSize=1024;
//	char ch, next, lookahead;
//	while ( cin.get( ch ))
//	{
//		switch (ch) 
//		{
//		case '/':
//			// 是注释行吗? 用 peek() 看一看:
//			// 是的? ignore() 余下的行
//			next = cin.peek();
//			if ( next == '/' )
//				cin.ignore( lineSize, '\n' );
//			break;
//		case '>':
//			// 查找 >>=
//				next = cin.peek();
//			if ( next == '>' )
//			{
//				lookahead = cin.get();
//				next = cin.peek();
//				if ( next != '=' )
//					cin.putback( lookahead );
//			}
//		}
//	}
//	return 0;
//}

//int main(int argc, char* argv[])
//{
//	char ch;
//	int tab_cnt = 0, nl_cnt = 0, space_cnt = 0,
//		period_cnt = 0, comma_cnt = 0;
//	while ( cin.get( ch ))	//get(char& ch)
//	{
//		switch( ch )
//		{
//		case ' ': space_cnt++; break;
//		case '\t': tab_cnt++; break;
//		case '\n': nl_cnt++; break;
//		case '.': period_cnt++; break;
//		case ',': comma_cnt++; break;
//		}
//		cout.put( ch );
//	}
//	cout << "\nour statistics:\n\t"
//		<< "spaces: " << space_cnt << '\t'
//		<< "new lines: " << nl_cnt << '\t'
//		<< "tabs: " << tab_cnt << "\n\t"
//		<< "periods: " << period_cnt << '\t'
//		<< "commas: " << comma_cnt << endl;
//	return 0;
//}

//int main(int argc, char* argv[])	
//{
//	int ch;
//	// 或使用:
//	// while (( ch = cin.get() ) && ch != EOF )
//	while (( ch = cin.get()) != EOF )	//get()
//		cout.put(ch);
//	return 0;
//}

//int main(int argc, char* argv[])
//{
//	const int max_line = 1024;
//	char line[ max_line ];
//	while ( cin.get( line, max_line ))//get(char *sink, streamsize size, char delimiter='\n')
//	{
//		int get_count = cin.gcount();	// 最大读取数量 max_line - 1, 也可以为 null
//
//		cout << "characters actually read: "
//			<< get_count << endl;
//	
//		if ( get_count & max_line-1 )	// 处理每一行如果遇到换行符在读下一行之前去掉它
//			cin.ignore();	//ignore( streamsize length = 1, int delim = traits::eof )
//	}
//	return 0;
//}

//int main(int argc, char* argv[])
//{
//	const int lineSize = 1024;
//	int lcnt = 0; // 读入多少行
//	int max = -1; // 最长行的长度
//	char inBuf[ lineSize ]; 
//	while (cin.getline( inBuf, lineSize ))	// 读取 1024 个字符或者遇到换行符
//	{
//		int readin = cin.gcount();		// 实际读入多少字符
//
//		++lcnt;		// 统计: 行数最长行
//
//		if ( readin > max )
//			max = readin;
//		cout << "Line #" << lcnt<< "\tChars read: " << readin << endl;
//		cout.write( inBuf, readin).put('\n').put('\n');
//	}
//	cout << "Total lines read: " << lcnt << endl;
//	cout << "Longest line read: " << max << endl;
//	return 0;
//}

class Location 
{
	friend ostream& operator<<( ostream&, const Location& );
public:
	Location( int line=0, int col=0 ): _line( line ), _col( col ) {}
private:
	short _line;
	short _col;
};
ostream& operator <<( ostream& os, const Location& lc )
{
	// output of a Location object: < 10,37 >
	os << "<" << lc._line<< "," << lc._col << "> ";
	return os;
}

class WordCount
{
	friend ostream& operator<<(ostream&, const WordCount&);
	friend istream& operator>>( istream&, WordCount& );
public:
	WordCount(){}
	WordCount( const string &word ) : _word(word){}
	WordCount( const string &word, int ln, int col )
		: _word( word ){ insert_location( ln, col ); }
	string word() const { return _word; }
	int occurs() const { return _occurList.size(); }
	void found( int ln, int col )
	{ insert_location( ln, col ); }
private:
	void insert_location( int ln, int col )
	{ _occurList.push_back( Location( ln, col )); }
	string _word;
	vector< Location > _occurList;
};

ostream& operator<<( ostream& os, const WordCount& wd )
{
	//os << "< " << wd.occurs << " > "<< wd.word;	// 格式: <occurs> word
	//return os;
	os << "<" << wd._occurList.size() << "> "<< wd._word << endl;
	int cnt = 0, onLine = 6;
	vector< Location >::const_iterator first = wd._occurList.begin();
	vector< Location >::const_iterator last = wd._occurList.end();
	for ( ; first != last; ++first )
	{
		os << *first << " ";		// os << Location
		if ( ++cnt == onLine )		// 格式化: 6 个一行
		{ os << "\n"; cnt = 0; }
	}
	return os;
}

istream& operator>>( istream &is, WordCount &wd )
{
	/* WordCount 对象被读入的格式:
	* <2> string
	* <7,3> <12,36>
	*/
	int ch;
	/* 读入小于符号, 如果不存在
	* 则设置 istream 为失败状态并退出
	*/
	if ((ch = is.get()) != '<' )
	{
		is.setstate( ios_base::failbit );
		return is;
	}
	// 读入多少个
	int occurs;
	is >> occurs;
	// 取 >; 不检查错误
	while ( is && (ch = is.get()) != '>' ) ;
	is >> wd._word;
	// 读入位置
	// 每个位置的格式: < line, col >
	for ( int ix = 0; ix < occurs; ++ix )
	{
		int line, col;
		// 提取值
		while (is && (ch = is.get())!= '<' ) ;
		is >> line;
		while (is && (ch = is.get())!= ',' ) ;
		is >> col;
		while (is && (ch = is.get())!= '>' ) ;
		wd._occurList.push_back( Location( line, col ));
	}
	return is;
}


//int main(int argc, char* argv[])
//{
//	//WordCount wd( "sadness", 12 );
//	//cout << "wd:\n" << wd << endl;
//	WordCount search( "rosebud" );
//	search.found(11,3); search.found(11,8);	// 为简单起见, 直接写入出现次数
//	search.found(14,2); search.found(34,6);
//	search.found(49,7); search.found(67,5);
//	search.found(81,2); search.found(82,3);
//	search.found(91,4); search.found(97,8);
//	cout << "Occurrences: " << "\n"	<< search << endl;
//
//	stringstream ss;
//	ss<<search;
//	WordCount test;
//	ss>>test;
//	cout<<test<<endl;
//
//	ofstream outfile( "copy.out", ios_base::out );
//	outfile<<test;
//	outfile.close();
//	
//	ifstream infile("copy.out");
//	WordCount testfile;
//	infile>>testfile;
//	cout<<testfile<<endl;
//	return 0;
//}

//int main(int argc, char* argv[])
//{
//	ofstream outFile( "copy.out" );	// 打开文件 copy.out 用于输出
//
//	if ( ! outFile )
//	{
//		cerr << "Cannot open \"copy.out\" for output\n";
//		return -1 ;
//	}
//	char ch;
//	while ( cin.get( ch ) )
//		outFile.put( ch );
//	return 0;
//}

//int main(int argc, char* argv[])
//{
//	cout << "filename: ";
//	string file_name;
//	cin >> file_name;
//
//	ifstream inFile( file_name.c_str() );	// 打开文件 copy.out 用于输入
//	if ( !inFile ) {
//		cerr << "unable to open input file: "
//			<< file_name << " -- bailing out!\n";
//		return -1;
//	}
//	char ch;
//	while ( inFile.get( ch ))
//		cout.put( ch );
//	return 0;
//}

template <class InputIterator> 
void filter_string_new( InputIterator first, InputIterator last, string filt_elems = string("\",?."))
{
	for ( ; first != last; first++ )
	{
		string::size_type pos = 0;
		while ((pos=(*first).find_first_of(filt_elems,pos))!= string::npos )
			(*first).erase( pos, 1 );
	}
}

//int main(int argc, char* argv[])
//{
//	ifstream infile( "tt.txt" );
//	istream_iterator<string> ifile( infile );
//	istream_iterator<string> eos;
//	vector< string > text;
//	copy( ifile, eos, inserter( text, text.begin() ));
//	string filt_elems( "\",.?;:" );
//	filter_string_new( text.begin(), text.end(), filt_elems );
//	vector<string>::iterator iter;
//	sort( text.begin(), text.end() );
//	iter = unique( text.begin(), text.end() );
//	text.erase( iter, text.end() );
//	ofstream outfile( "tt_sort.txt" );
//	iter = text.begin();
//	for ( int line_cnt = 1; iter != text.end();++iter, ++line_cnt )
//	{
//		outfile << *iter << " ";
//		if ( ! ( line_cnt % 8 ))
//			outfile << '\n';
//	}
//	outfile << endl;
//	return 0;
//}

//int main(int argc, char* argv[])
//{
//	const int fileCnt = 5;
//	string fileTabl[ fileCnt ] = {"Melville","Joyce","Musil","Proust","Kafka"};
//
//	ifstream inFile; // 没有连接任何文件
//	for ( int ix = 0; ix < fileCnt; ++ix )
//	{
//		inFile.open( fileTabl[ix].c_str() );
//		// ... 判断是合打开成功
//		// ... 处理文件
//		inFile.close();
//	}
//	return 0;
//}

//int main(int argc, char* argv[])
//{
//	fstream inOut( "copy.out", ios_base::in|ios_base::app );
//	int cnt=0;
//	char ch;
//	//inOut.seekg(0);
//	while ( inOut.get( ch ) )
//	{
//		cout.put( ch );
//		cnt++;
//		if ( ch == '\n' )
//		{
//			fstream::pos_type mark = inOut.tellg();	// 标记当前位置
//			inOut << cnt << ' ';
//			inOut.seekg( mark ); // 恢复位置
//		}
//	}
//	inOut.clear();
//	inOut << cnt << endl;
//	cout << "[ " << cnt << " ]\n";
//	return 0;
//}

//int main(int argc, char* argv[])
//{
//	istream_iterator< int > input( cin );
//	istream_iterator< int > end_of_stream;
//	vector<int> vec;
//	copy ( input, end_of_stream, inserter( vec, vec.begin() ));
//	sort( vec.begin(), vec.end(), greater<int>() );
//	ostream_iterator< int > output( cout, " " );
//	unique_copy( vec.begin(), vec.end(), output );
//	return 0;
//}

//int main(int argc, char* argv[])
//{
//	copy( istream_iterator< int >( cin ),
//		istream_iterator< int >(),
//		ostream_iterator< int >( cout ));
//	return 0;
//}

//int main(int argc, char* argv[])
//{
//	string file_name;
//	cout << "please enter a file to open: ";
//	cin >> file_name;
//	if ( file_name.empty() || !cin )
//	{
//		cerr << "unable to read file name \n";
//		return -1;
//	}
//	ifstream infile( file_name.c_str());
//	if ( ! infile ) 
//	{
//		cerr << "unable to open " << file_name << endl;
//		return -2;
//	}
//	cout<<endl;
//	istream_iterator< string > ins( infile ), eos;
//	ostream_iterator< string > outs( cout, " " );
//	copy( ins, eos, outs );
//	cout<<endl;
//	
//	return 0;
//}
