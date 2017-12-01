// ��׼��ͷ�ļ�
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <utility>
#include <map>
#include <set>
#include <stack>
#include <fstream>	// ��׼C++֮ǰ��iostream ͷ�ļ�
#include <stddef.h>	// ��׼C ͷ�ļ�
#include <ctype.h>
using namespace std;

// typedefs ʹ��������
typedef pair<short,short> location;
typedef vector<location> loc;
typedef vector<string> text;
typedef pair<text*,loc*> text_loc;

/*from effective stl

��׼STL����������vector��string��deque��list�� 
��׼STL����������set��multiset��map��multimap�� 
�Ǳ�׼��������slist��rope��slist��һ����������rope��������һ�������ַ�����
("rope"��һ������"string"����������)������ҵ�һ��������Щ�Ǳ�׼(��������)�����ĸ���������50�� 
�Ǳ�׼��������hash_set��hash_multiset��hash_map��hash_multimap��
��������25��������Щ���Թ㷺��õĻ���hash��������ͱ�׼���������Ĳ�ͬ�㡣 
vector<char>������Ϊstring�����Ʒ������13������������Ʒ���ܻ������������� 
vector��Ϊ��׼�������������Ʒ����������23��˵�ģ���ʱ��vector������ʱ��Ϳռ��϶����ֵñȱ�׼���������á� 
���ֱ�׼��STL�������������顢bitset��valarray��stack��queue��priority_queue����Ϊ�����Ƿ�STL������
�����ڱ����й���������˵�ú��٣���Ȼ����16�ᵽ�������STL�����������Ƶ�һ�������
������18��ʾ��Ϊʲôbitset���ܱ�vector<bool>Ҫ�á�ֵ��ע����ǣ�������Ժ�STL�㷨��ϣ���Ϊָ����Ե�������ĵ�����ʹ�á� 
*/
class TextQuery
{
public:
	TextQuery() { memset( this, 0, sizeof( TextQuery )); }
	static void	filter_elements( string felems ) { filt_elems = felems; }
	void query_text();
	void display_map_text();
	void display_text_locations();
	void doit() 
	{
		retrieve_text();
		separate_words();
		filter_text();
		suffix_text();
		strip_caps();
		build_word_map();
	}
private:
	void retrieve_text();
	void separate_words();
	void filter_text();
	void strip_caps();
	void suffix_text();
	void suffix_s( string& );
	void build_word_map();
private:
	vector<string> *lines_of_text;
	text_loc *text_locations;
	map< string,loc*,less<string>> *word_map;
	static string filt_elems;
};

string TextQuery::filt_elems( "\",.;:!<<)(\\/" );

void TextQuery::retrieve_text()
{
	string file_name;
	cout << "please enter file name: ";
	cin >> file_name;
	ifstream infile( file_name.c_str(), ios::in );
	if ( !infile ) 
	{
		cerr << "oops! unable to open file "
			<< file_name << " -- bailing out!\n";
		exit( - 1 );
	}
	else cout << "\n";
	lines_of_text = new vector<string>;
	string textline;
	while ( getline( infile, textline, '\n' ))
		lines_of_text->push_back( textline );
}

void TextQuery::separate_words()
{
	vector<string> *words = new vector<string>;
	vector<location> *locations = new vector<location>;
	for ( short line_pos = 0; line_pos < lines_of_text->size(); line_pos++ )
	{
		short word_pos = 0;
		string textline = (*lines_of_text)[ line_pos ];
		string::size_type eol = textline.length();
		string::size_type pos = 0, prev_pos = 0;
		while (( pos = textline.find_first_of( ' ', pos ))!= string::npos )
		{
			words->push_back(textline.substr( prev_pos, pos - prev_pos ));
			locations->push_back(make_pair( line_pos, word_pos ));
			++word_pos; prev_pos = ++pos;
		}
		words->push_back(textline.substr( prev_pos, pos - prev_pos ));
		locations ->push_back(make_pair(line_pos,word_pos));
	}
	text_locations = new text_loc( words, locations );
}

void TextQuery::filter_text()
{
	if ( filt_elems.empty() )
		return;
	vector<string> *words = text_locations->first;
	vector<string>::iterator iter = words->begin();
	vector<string>::iterator iter_end = words->end();
	while ( iter != iter_end )
	{
		string::size_type pos = 0;
		while (( pos = (*iter).find_first_of( filt_elems, pos ))!= string::npos )
			(*iter).erase(pos,1);
		++iter;
	}
}

void TextQuery::suffix_text()
{
	vector<string> *words = text_locations ->first;
	vector<string>::iterator iter = words ->begin();
	vector<string>::iterator iter_end = words ->end();
	while ( iter != iter_end )
	{
		if ( (*iter).size() <= 3 )
		{ iter++; continue; }
		if ( (*iter)[ (*iter).size()- 1 ] == 's' )
			suffix_s( *iter );
		// �����ĺ�׺�����������
		iter++;
	}
}

void TextQuery::suffix_s( string &word )
{
	string::size_type spos = 0;
	string::size_type pos3 = word.size()- 3;
	// "ous", "ss", "is", "ius"
	string suffixes( "oussisius" );
	if ( ! word.compare( pos3, 3, suffixes, spos, 3 ) ||
		! word.compare( pos3, 3, suffixes, spos+6, 3 ) ||
		! word.compare( pos3+1, 2, suffixes, spos+2, 2 ) ||
		! word.compare( pos3+1, 2, suffixes, spos+4, 2 ))
		return;
	string ies( "ies" );
	if ( ! word.compare( pos3, 3, ies ))
	{
		word.replace( pos3, 3, 1, 'y' );
		return;
	}
	string ses( "ses" );
	if ( ! word.compare( pos3, 3, ses ))
	{
		word.erase( pos3+1, 2 );
		return;
	}
	// ȥ��β���� 's'
	word.erase( pos3+2 );
	// watch out for "'s"
	if ( word[ pos3+1 ] == '\'' )
		word.erase( pos3+1 );
}

void TextQuery::strip_caps()
{
	vector<string> *words = text_locations ->first;
	vector<string>::iterator iter = words ->begin();
	vector<string>::iterator iter_end = words ->end();
	string caps( "ABCDEFGHIJKLMNOPQRSTUVWXYZ" );
	while ( iter != iter_end )
	{
		string::size_type pos = 0;
		while (( pos = (*iter).find_first_of( caps, pos ))!= string::npos )
			(*iter)[ pos ] = tolower( (*iter)[pos] );
		++iter;
	}
}

void TextQuery::build_word_map()
{
	word_map = new map< string, loc*, less<string> >;
	typedef map<string,loc*,less<string>>::value_type value_type;
	set<string,less<string>> exclusion_set;
	ifstream infile( "exclusion_set" );
	if ( !infile )
	{
		static string default_excluded_words[25] = {
			"the","and","but","that","then","are","been",
			"can","can't","cannot","could","did","for",
			"had","have","him","his","her","its","into",
			"were","which","when","with","would"
		};
		cerr << "warning! unable to open word exclusion file! -- " << "using default set\n";
		copy( default_excluded_words, default_excluded_words+25,inserter( exclusion_set, exclusion_set.begin() ));
	}
	else 
	{
		istream_iterator< string > input_set( infile ), eos;
		copy( input_set, eos, inserter( exclusion_set, exclusion_set.begin() ));
	}
	// ��������, �����/ֵ��
	vector<string> *text_words = text_locations ->first;
	vector<location> *text_locs = text_locations ->second;
	register int elem_cnt = text_words ->size();
	for ( int ix = 0; ix < elem_cnt; ++ix )
	{
		string textword = ( *text_words )[ ix ];
		if ( textword.size() < 3 ||	exclusion_set.count( textword ))
			continue;
		if ( ! word_map->count((*text_words)[ix] ))	// û��, ���:
		{ 
			loc *ploc = new vector<location>;
			ploc->push_back( (*text_locs)[ix] );
			word_map->insert( value_type( (*text_words)[ix], ploc ));
		}
		else 
		{
			(*word_map)[(*text_words)[ix]]->push_back( (*text_locs)[ix] );
		}
	}
}

void TextQuery::query_text()
{
	string query_text;
	do {
		cout << "enter a word against which to search the text.\n"
			<< "to quit, enter a single character ==> ";
		cin >> query_text;
		if ( query_text.size() < 2 ) break;
		string caps( "ABCDEFGHIJKLMNOPQRSTUVWXYZ" );
		string::size_type pos = 0;
		while (( pos = query_text.find_first_of( caps, pos ))!= string::npos )
			query_text[ pos ] = tolower( query_text[pos] );
		if ( !word_map->count( query_text ))	// �����map ����, ����query_text, ����˵��û��Ҫ�ҵĴ�
 		{
			cout << "\nSorry. There are no entries for "<< query_text << ".\n\n";
			continue;
		}
		loc *ploc = (*word_map)[ query_text ];
		set<short,less<short>> occurrence_lines;
		loc::iterator liter = ploc->begin(),liter_end = ploc->end();
		while ( liter != liter_end ) 
		{
			occurrence_lines.insert(occurrence_lines.end(), (*liter).first);
			++liter;
		}
		register int size = occurrence_lines.size();
		cout << "\n" << query_text
			<< " occurs " << size
			<< (size == 1 ? " time:" : " times:")
			<< "\n\n";
		set<short,less<short>>::iterator it=occurrence_lines.begin();
		for ( ; it != occurrence_lines.end(); ++it ) 
		{
			int line = *it;
			cout << "\t( line "
				<< line + 1 << " ) "	// ��Ҫ�ô� 0 ��ʼ���ı��а��û�Ū�Ժ���
				<< (*lines_of_text)[line] << endl;
		}
		cout << endl;
	}
	while ( ! query_text.empty() );
	cout << "Ok, bye!\n";
}

void TextQuery::display_map_text()
{
	typedef map<string,loc*,less<string>> map_text;
	map_text::iterator iter = word_map->begin(),iter_end = word_map->end();
	while ( iter != iter_end ) 
	{
		cout << "word: " << (*iter).first << " (";
		int loc_cnt = 0;
		loc *text_locs = (*iter).second;
		loc::iterator liter = text_locs->begin(),liter_end = text_locs->end();
		while ( liter != liter_end )
		{
			if ( loc_cnt )
				cout << ",";
			else ++loc_cnt;
			cout << "(" << (*liter).first
				<< "," << (*liter).second << ")";
			++liter;
		}
		cout << ")\n";
		++iter;
	}
	cout << endl;
}

void TextQuery::display_text_locations()
{
	vector<string> *text_words = text_locations ->first;
	vector<location> *text_locs = text_locations ->second;
	register int elem_cnt = text_words ->size();
	if ( elem_cnt != text_locs->size() )
	{
		cerr << "oops! internal error: word and position vectors "
			<< "are of unequal size \n"
			<< "words: " << elem_cnt << " "
			<< "locs: " << text_locs->size()
			<< " -- bailing out!\n";
		exit( - 2 );
	}
	for ( int ix = 0; ix < elem_cnt; ix++ )
	{
		cout << "word: " << (*text_words)[ ix ] << "\t"
			<< "location: ("
			<< (*text_locs)[ix].first << ","
			<< (*text_locs)[ix].second << ")"
			<< "\n";
	}
	cout << endl;
}

//int main(int argc, char* argv[])
//{
//	TextQuery tq;
//	tq.doit();
//	tq.query_text();
//	tq.display_map_text();
//
//	return 0;
//}

//int main()
//{
//	map< string, string > trans_map;
//	typedef map< string, string >::value_type valType;
//	// ��һ��Ȩ��֮��: ��ת���Թ̶�д�ڴ�����
//	trans_map.insert( valType( "gratz", "grateful" ));
//	trans_map.insert( valType( "'em", "them" ));
//	trans_map.insert( valType( "cuz", "because" ));
//	trans_map.insert( valType( "nah", "no" ));
//	trans_map.insert( valType( "sez", "says" ));
//	trans_map.insert( valType( "tanx", "thanks" ));
//	trans_map.insert( valType( "wuz", "was" ));
//	trans_map.insert( valType( "pos", "suppose" ));
//	// ok: ��ʾtrans_map
//	map< string,string >::iterator it;
//	cout << "Here is our transformation map: \n\n";
//	for ( it = trans_map.begin();it != trans_map.end(); ++it )
//		cout << "key: " << (*it).first << "\t" << "value: " << (*it).second << "\n";
//	cout << "\n\n";
//	// �ڶ���Ȩ��֮��: �̶�д������.
//	string textarray[14]={ "nah", "I", "sez", "tanx", "cuz", "I", "wuz", "pos", "to", "not", "cuz", "I", "wuz", "gratz" };
//	vector< string > text( textarray, textarray+14 );
//	vector< string >::iterator iter;
//	// ok: ��ʾ text
//	cout << "Here is our original string vector: \n\n";
//	int cnt = 1;
//	for ( iter = text.begin(); iter != text.end(); ++iter, ++cnt )
//		cout << *iter << ( cnt % 8 ? " " : "\n" );
//	cout << "\n\n\n";
//	// ����ͳ����Ϣ��map������̬����
//	map< string,int > stats;
//	typedef map< string,int >::value_type statsValType;
//	// ok: ������ map ������������ĺ���
//	for ( iter = text.begin(); iter != text.end(); ++iter )
//	{
//		if (( it = trans_map.find( *iter )) != trans_map.end() )
//		{
//			if ( stats.count( *iter ))
//				stats[ *iter ] += 1;
//			else stats.insert( statsValType( *iter, 1 ));
//			*iter = (*it).second;
//		}
//	}
//		// ok: ��ʾ��ת����� vector
//		cout << "Here is our transformed string vector: \n\n";
//		cnt = 1;
//		for ( iter = text.begin(); iter != text.end(); ++iter, ++cnt )
//			cout << *iter << ( cnt % 8 ? " " : "\n" );
//		cout << "\n\n\n";
//		// ok: ���ڶ�ͳ�� map ���е���
//		cout << "Finally, here are our statistics:\n\n";
//		map<string,int,less<string>>::iterator siter;
//		for ( siter = stats.begin(); siter != stats.end(); ++siter )
//			cout << (*siter).first << " "
//			<< "was transformed "
//			<< (*siter).second
//			<< ((*siter).second == 1 ? " time\n" : " times\n" );
//
//		return 0;
//}

int main()
{
	const int ia_size = 10;
	int ia[ia_size ] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	int ix = 0;
	stack< int > intStack;
	for ( ; ix < ia_size; ++ix )	// ��� stack
		intStack.push( ia[ ix ] );
	int error_cnt = 0;
	if ( intStack.size() != ia_size )
	{
		cerr << "oops! invalid intStack size: "
			<< intStack.size()
			<< "\t expected: " << ia_size << endl;
		++error_cnt;
	}
	int value;
	while ( intStack.empty() == false )
	{
		value = intStack.top();	// ��ȡջ��Ԫ��
		if ( value != --ix )
		{
			cerr << "oops! expected " << ix
				<< " received " << value
				<< endl;
			++error_cnt;
		}
		intStack.pop();	// ����ջ��Ԫ��
	}
	cout << "Our program ran with " << error_cnt << " errors!" << endl;

	return 0;
}

//queue and priority_queue ��...