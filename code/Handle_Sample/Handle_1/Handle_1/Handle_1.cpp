#include <string>
#include <iostream>
using namespace std;

class UPoint;

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
	UPoint* up;
};

class UPoint
{
	friend class Handle;		//都为私有成员
	Point p;
	int u;

	UPoint():u(1){}
	UPoint(int x,int y):p(x,y),u(1){}
	UPoint(const Point& p0):p(p0),u(1){}
};

Handle::Handle():up(new UPoint){}
Handle::Handle(int x,int y):up(new UPoint(x,y)){}
Handle::Handle(const Point& p):up(new UPoint(p)){}
Handle::~Handle()
{
	if(--up->u==0)
		delete up;
}
Handle::Handle(const Handle& h):up(h.up){++up->u;}
Handle& Handle::operator=(const Handle& h)
{
	++h.up->u;
	if(--up->u==0)
		delete up;
	up=h.up;
	return *this;
}

int Handle::x() const{return up->p.x();}
int Handle::y() const{return up->p.y();}

Handle& Handle::x(int x0)
{
	//up->p.x(x0);
	//return *this;

	if(up->u!=1)		//写时复制(copy on write)
	{
		--up->u;
		up=new UPoint(up->p);
	}
	up->p.x(x0);
	return *this;
}

Handle& Handle::y(int y0)
{
	//up->p.y(y0);			
	//return *this;

	if(up->u!=1)		//写时复制(copy on write)
	{
		--up->u;
		up=new UPoint(up->p);
	}
	up->p.y(y0);
	return *this;
}


int main(int argc, char* argv[])
{
	return 0;
}