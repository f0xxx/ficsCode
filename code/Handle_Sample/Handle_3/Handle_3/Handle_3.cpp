#include <string>
#include <iostream>
using namespace std;

class Point
{
public:
	Point():xval(0),yval(0){}
	Point(int x,int y):xval(x),yval(y){}
	int x(){return xval;}
	int y(){return yval;}
	Point& x(int xv){xval=xv;return *this;}
	Point& y(int yv){yval=yv;return *this;}

private:
	int xval,yval;
};

class UseCount
{
public:
	UseCount();
	UseCount(const UseCount&);
	~UseCount();

	bool only();
	bool reattach(const UseCount&);
	bool makeonly();
private:
	UseCount& operator=(const UseCount&);

	int *p;
};

UseCount::UseCount():p(new int(1)){}
UseCount::UseCount(const UseCount& u):p(u.p){++*p;}
UseCount::~UseCount(){if(--*p==0) delete p;}
bool UseCount::only(){return *p==1;}
bool UseCount::reattach(const UseCount& u)
{
	++*u.p;
	if(--*p==0)
	{
		delete p;
		p=u.p;
		return true;
	}
	p=u.p;
	return false;
}

bool UseCount::makeonly()
{
	if(*p==1)
		return false;
	--*p;
	p=new int(1);
	return true;
}


class Handle		//Handle可以避免使用surrogate时的大量复制(使用引用避免副本产生)
{
public:
	Handle();
	Handle(int,int);
	Handle(const Point&);
	Handle(const Handle&);
	Handle& operator=(const Handle&);
	~Handle();

	int x()const;
	Handle& x(int);
	int y()const;
	Handle& y(int);	

private:
	Point* p;		//容纳Point的派生类
	UseCount u;			//抽象引用计数
};

Handle::Handle():p(new Point){}
Handle::Handle(int x,int y):p(new Point(x,y)){}
Handle::Handle(const Point& p0):p(new Point(p0)){}
Handle::~Handle()
{
	if(u.only())
	{
		delete p;
	}
}
Handle::Handle(const Handle& h):u(h.u),p(h.p){}
Handle& Handle::operator=(const Handle& h)
{
	if(u.reattach(h.u))
	{
		delete p;
	}
	p=h.p;
	return *this;
}

int Handle::x() const{return p->x();}
int Handle::y() const{return p->y();}

Handle& Handle::x(int x0)
{
	//p->x(x0);
	//return *this;

	if(u.makeonly())		//写时复制(copy on write)
	{
		p=new Point(*p);
	}
	p->x(x0);
	return *this;
}

Handle& Handle::y(int y0)
{
	//up->p.y(y0);			
	//return *this;

	if(u.makeonly())		//写时复制(copy on write)
	{
		p=new Point(*p);
	}
	p->y(y0);
	return *this;
}


int main(int argc, char* argv[])
{
	return 0;
}