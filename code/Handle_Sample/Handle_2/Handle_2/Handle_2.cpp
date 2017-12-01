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
	int* u;
};

Handle::Handle():u(new int(1)),p(new Point){}
Handle::Handle(int x,int y):u(new int(1)),p(new Point(x,y)){}
Handle::Handle(const Point& p0):u(new int(1)),p(new Point(p0)){}
Handle::~Handle()
{
	if(--*u==0)
	{
		delete u;
		delete p;
	}
}
Handle::Handle(const Handle& h):u(h.u),p(h.p){++*u;}
Handle& Handle::operator=(const Handle& h)
{
	++*h.u;
	if(--*u==0)
	{
		delete u;
		delete p;
	}
	u=h.u;
	p=h.p;
	return *this;
}

int Handle::x() const{return p->x();}
int Handle::y() const{return p->y();}

Handle& Handle::x(int x0)
{
	//p->x(x0);
	//return *this;

	if(*u!=1)		//写时复制(copy on write)
	{
		--*u;
		p=new Point(*p);
	}
	p->x(x0);
	return *this;
}

Handle& Handle::y(int y0)
{
	//up->p.y(y0);			
	//return *this;

	if(*u!=1)		//写时复制(copy on write)
	{
		--*u;
		p=new Point(*p);
	}
	p->y(y0);
	return *this;
}


int main(int argc, char* argv[])
{
	return 0;
}