#include <string>
#include <iostream>
using namespace std;

class vehicle
{
public:
	virtual void start() = 0;
	virtual vehicle* copy() const = 0;	//增加一个拷贝自身的虚函数
	virtual ~vehicle(){};				//虚析构函数
};

class Roadvehicle:public vehicle
{
public:
	vehicle* copy() const
	{
		return new Roadvehicle(*this);
	}
	void start()
	{
		cout<<"Roadvehicle GO!!!"<<endl;
	}
};

class Autovehicle:public Roadvehicle
{
public:
	vehicle* copy() const
	{
		return new Autovehicle(*this);
	}
	void start()
	{
		cout<<"Autovehicle GO!!!"<<endl;
	}
};

class Aircraft:public vehicle
{
public:
	vehicle* copy() const
	{
		return new Aircraft(*this);
	}
	void start()
	{
		cout<<"Aircraft GO!!!"<<endl;
	}
};

class helicopter:public Aircraft
{
public:
	vehicle* copy() const
	{
		return new helicopter(*this);
	}
	void start()
	{
		cout<<"helicopter GO!!!"<<endl;
	}
};

class vehicleSurrogate		//代理类
{
public:
	vehicleSurrogate();		//定义数组需要
	/*explicit*/ vehicleSurrogate(const vehicle&);	//允许隐式转换
	~vehicleSurrogate();
	vehicleSurrogate(const vehicleSurrogate&);
	vehicleSurrogate& operator=(const vehicleSurrogate&);

	void start();
private:
	vehicle *vp;
};

vehicleSurrogate::vehicleSurrogate():vp(0){}
vehicleSurrogate::vehicleSurrogate(const vehicle& v):vp(v.copy()){}
vehicleSurrogate::~vehicleSurrogate(){delete vp;}
vehicleSurrogate::vehicleSurrogate(const vehicleSurrogate& v):vp(v.vp?v.vp->copy():0){}
vehicleSurrogate& vehicleSurrogate::operator=(const vehicleSurrogate& v)
{
	if(this != &v)
	{
		delete vp;
		vp = (v.vp?v.vp->copy():0);
	}
	return *this;
}

void vehicleSurrogate::start()
{
	if(vp == 0)
		throw "empty !!!";
	vp->start();
}

int main(int argc, char* argv[])
{
	//vehicle parking_lot[1000];		//只能存储vehicle对象，但是vehicle为虚，而且派生类对象怎办呢？
	//vehicle *parking_lot[1000];		//可以兼顾派生类，但是缺点嘛，指针无法控制所指对象的生存时间，
										//一旦所指对象消失，指针就不知道指向什么东西了
	//Autovehicle x=**;
	//parking_lot[1000]=new Autovehicle(x);		//变通一下，存储副本，不过带来了动态内存管理的负担，
												//更重要的是我们必须事先知道Autovehicle这个类型才能这么办
												//如果我们要存储和元素q同样的类型该怎么办呢？
	//if(p!=q)									
	//{
	//	delete parking_lot[p];
	//	parking_lot[p]=parking_lot[q];			//指向了同样的对象
	//}

	//if(p!=q)
	//{
	//	delete parking_lot[p];
	//	parking_lot[p]=new vehicle(*parking_lot[q]) ;		//vehicle为虚，就算不为虚，也会被分割
	//}

	vehicleSurrogate parking_lot[1000];			//使用代理类
	Autovehicle x;
	parking_lot[2] = x;			//相当于parking_lot[2] = vehicleSurrogate(x);匹配operator=时用构造函数做了隐式转换

	parking_lot[2].start();
	
	return 0;
}