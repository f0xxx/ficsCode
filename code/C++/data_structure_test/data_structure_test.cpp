#include <iostream>
#include <time.h>
using namespace std;

template <class T>
void Swap(T& a,T& b)
{
	T temp;
	temp=a;
	a=b;
	b=temp;
}

//求解多项式和P(x)=Eci*x^n
template <class T>
T PolyEval(T ci[],int n,const T x)
{
	T y=1;
	T value=ci[0];
	for(int i=1;i<=n;i++)
	{
		y*=x;
		value+=ci[i]*y;
	}
	return value;
}

template <class T>
T PolyEvalNew(T ci[],int n,const T x)
{
	T value=ci[n];
	for(int i=1;i<=n;i++)
		value=value*x+ci[n-i];
	return value;
}

//计算名次
template <class T>
void Rank(T a[],int n,T r[])
{
	for(int i=0;i<n;i++)
		r[i]=0;
	for(int i=0;i<n;i++)
		for(int j=i+1;j<n;j++)
		{
			if(a[i]>a[j])
				r[i]++;
			else
				r[j]++;
		}
		
}

//按名次排序
template <class T>
void Rearrange(T a[],int n,T r[])
{
	//T* pa=new T[n];
	//for(int i=0;i<n;i++)
	//	pa[r[i]]=a[i];
	//for(int i=0;i<n;i++)
	//	a[i]=pa[i];
	//delete []pa;
	for(int i=0;i<n;++i)
		while(r[i]!=i)
		{
			Swap(a[r[i]],a[i]);
			Swap(r[i],r[r[i]]);
		}
}

//选择排序
template <class T>
int ChooseMax(T a[],int n)
{
	int ret=0;
	for(int i=1;i<n;++i)
		if(a[ret]<a[i])
			ret=i;
	return ret;
}

template <class T>
void SelectionSort(T a[],int n)
{
	//int index=0;
	//for(int i=0;i<n;++i)
	//{
	//	index=ChooseMax(a,n-i);
	//	Swap(a[index],a[n-i-1]);
	//}
	bool IsSorted=false;
	for(int size=n;size>1&&!IsSorted;--size)
	{
		int index=0;
		IsSorted=true;
		for(int j=1;j<size;++j)
		{
			if(a[index]<a[j])
				index=j;
			else 
				IsSorted=false;
		}
		Swap(a[index],a[size-1]);
	}

}

//冒泡排序
//template <class T>
//void Bubble(T a[],int n)
//{
//	for(int i=0;i<n-1;++i)
//		if(a[i]>a[i+1])
//			Swap(a[i],a[i+1]);
//}

template <class T>
bool Bubble(T a[],int n)
{
	bool IsSwap=false;
	for(int i=0;i<n-1;++i)
		if(a[i]>a[i+1])
		{
			Swap(a[i],a[i+1]);
			IsSwap=true;
		}
	return IsSwap;
}
template <class T>
void BubbleSort(T a[],int n)
{
	//for(int i=n;i>=0;--i)
	//	Bubble(a,i);
	for(int i=n;i>1&&Bubble(a,i);--i);
}

//插入排序
template <class T>
void Insert(T a[],int n,T &b)
{
	int i;
	for(i=n-1;i>=0&&a[i]>b;--i)
		a[i+1]=a[i];
	a[i+1]=b;
}

template <class T>
void InsertSort(T a[],int n)
{
	T temp;
	for(int i=1;i<n;i++)
	{
		temp=a[i];
		Insert(a,i,temp);
	}
}

//折半查找
template<class T>
int BinarySearch(T a[], const T& x, int n)
{
	int left=0,right=n-1,middle=0;
	while(left<=right)
	{
		middle=(left+right)/2;
		if(a[middle]==x)
			return middle;
		if(x>a[middle])
			left=middle;
		else
			right=middle;
	}
	return -1;
}

class stact
{
public:
	stact(int size=5):top(NULL),start(NULL)
	{
		size=size;
		start=top=new int[size];
		memset(start,0,size*sizeof(int));
	};
	bool pop(int &x)
	{
		if(isempty())
			return false;
		x=*(--top);
		return true;
	};
	bool push(int x)
	{
		if(isfull())
			return false;
		*(top++)=x;
		return true;
	};
	bool isfull()
	{
		if(top-start==size)
			return true;
		else
			return false;
	};
	bool isempty()
	{
		if(top==start)
			return true;
		else
			return false;
	};
	~stact()
	{
		if(start!=NULL)
			delete []start;
	};
private:
	int *top,*start,size;
};

class hanoi
{
public:
	hanoi()
	{
		for(int i=5;i>0;--i)
		{
			s[1].push(i);
		}
	}
	void TowerOfHanoi(int n,int a,int b,int c)
	{
		int x;
		if(n>0)
		{
			TowerOfHanoi(n-1,a,c,b);
			s[a].pop(x);
			s[b].push(x);
			cout<<"tower"<<a<<">>"<<x<<">>tower"<<b<<endl;
			TowerOfHanoi(n-1,c,b,a);
		}
	};
private:
	stact s[4];
};

template<class T>
class BinaryTreeNode 
{
public:
	BinaryTreeNode(T data,BinaryTreeNode* left,BinaryTreeNode* right):data(data),left(left),right(right)
	{}
	BinaryTreeNode():data(0),left(NULL),right(NULL)
	{}

	T data;
	BinaryTreeNode* left;
	BinaryTreeNode* right;
};

template<class T>
class BinaryTree 
{
public :
	BinaryTree( ) {root = 0; };
	~BinaryTree( ) { };
	bool IsEmpty( ) const
	{return ((root) ? false : true);}
	bool Root(T& x) const;
	void MakeTree(const T& element, BinaryTree<T>& left,BinaryTree<T>& right);
	void BreakTree(T& element,BinaryTree<T>& left ,BinaryTree<T>& right);
	void PreOrder(void (*Visit)(BinaryTreeNode<T> *u))
	{ PreOrder( Visit,root ) ; }
	void InOrder(void (*Visit)(BinaryTreeNode<T> *u))
	{ InOrder( Visit,root ) ; }
	void PostOrder(void (*Visit)(BinaryTreeNode<T> *u))
	{ PostOrder(Visit, root); }
	void LevelOrder(void (*Visit)(BinaryTreeNode<T> *u));
private:
	BinaryTreeNode<T> *root; // 根节点指针
	void PreOrder(void (*Visit)(BinaryTreeNode<T> *u),BinaryTreeNode<T> *t);
	void InOrder(void (*Visit)(BinaryTreeNode<T> *u),BinaryTreeNode<T> *t);
	void PostOrder(void (*Visit)(BinaryTreeNode<T> *u),BinaryTreeNode<T> *t);
};

template<class T>
bool BinaryTree<T>::Root(T& x) const
{
	if(root)
	{
		x=root->data;
		return true;
	}
	return false;
}

template<class T>
void BinaryTree<T>::MakeTree(const T& element, BinaryTree<T>& left,BinaryTree<T>& right)
{
	root=new BinaryTreeNode<T>(element, left.root, right.root);
	left.root=right.root=0;
}

template<class T>
void BinaryTree<T>::BreakTree(T& element, BinaryTree<T>& left,BinaryTree<T>& right)
{
	if(!root) return;

	element=root->data;
	left.root=root->left;
	right.root=root->right;
	
	delete root;
	root=0;
}

template<class T>
void BinaryTree<T>::PreOrder(void (*Visit)(BinaryTreeNode<T> *u),BinaryTreeNode<T> *t)
{
	if(t)
	{
		Visit(t);
		PreOrder(Visit,t->left);
		PreOrder(Visit,t->right);
	}
}

template<class T>
void BinaryTree<T>::InOrder(void (*Visit)(BinaryTreeNode<T> *u),BinaryTreeNode<T> *t)
{
	if(t)
	{
		InOrder(Visit,t->left);
		Visit(t);
		InOrder(Visit,t->right);
	}
}

template<class T>
void BinaryTree<T>::PostOrder(void (*Visit)(BinaryTreeNode<T> *u),BinaryTreeNode<T> *t)
{
	if(t)
	{
		PostOrder(Visit,t->left);
		PostOrder(Visit,t->right);
		Visit(t);
	}
}

int count = 0;
BinaryTree<char> a,b,c,d,e,f,g,h,j,z ;
template<class T>
void ct(BinaryTreeNode<T> *t) {count++;cout<<t->data;}

int main()
{
	//int a[6]={45,23,567,12,5,5};
	//int b[6]={0};
	//hanoi test;
	//test.TowerOfHanoi(5,1,2,3);
	//Rank(a,6,b);
	//Rearrange(a,6,b);
	//SelectionSort(a,6);
	//InsertSort(a,6);
	//clock_t start,finish;
	//start=clock();
	//InsertSort(a,6);
	//finish=clock();
	//cout<<(finish-start)/*/CLK_TCK*/<<endl;
	//for(int i=0;i<6;i++)
	//	cout<<a[i]<<"	";
	//cout<<endl;
	//cout<<BinarySearch(a,45,6)<<endl;
	d.MakeTree( 'D' , z , z ) ;
	b.MakeTree( 'B' , d , z ) ;
	g.MakeTree( 'G' , z , z ) ;
	e.MakeTree( 'E' , z , g ) ;
	h.MakeTree( 'H' , z , z ) ;
	j.MakeTree( 'J' , z , z ) ;
	f.MakeTree( 'F' , h , j ) ;
	c.MakeTree( 'C' , e , f ) ;
	a.MakeTree( 'A' , b , c ) ;
	
	a.InOrder( ct ) ;
	cout<<endl << count << endl;
}