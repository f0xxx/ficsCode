#include <string>
#include <iostream>
using namespace std;

class Expr_node
{
	friend class Expr;
	
	int use;

public:
	Expr_node():use(1){}
	virtual void print(ostream&)const =0;
	virtual int eval()const =0;
	virtual ~Expr_node(){}
};

class Expr
{
	friend ostream& operator<<(ostream&,const Expr&);

	Expr_node *p;

public:
	Expr(int);
	Expr(const string&,Expr);
	Expr(const string&,Expr,Expr);
	Expr(const string&,Expr,Expr,Expr);
	Expr(const Expr&);

	Expr& operator=(const Expr&);

	int eval() const{return p->eval();}

	~Expr()
	{
		if(--p->use==0)
			delete p;
	}
};

class Int_node:public Expr_node
{
	friend class Expr;

	int n;
	
	Int_node(int k):n(k){}
	void print(ostream& o)const {o<<n;}
	int eval()const{return n;}
};

class Unary_node:public Expr_node
{
	friend class Expr;
	
	string op;
	Expr opnd;

	Unary_node(const string& a,Expr b):op(a),opnd(b){}
	void print(ostream& o) const
	{o<<'('<<op<<opnd<<')';}

	int eval()const
	{
		if(op=="-")
			return -opnd.eval();
		if(op=="+")
			return opnd.eval();
		throw "error,bad op";
	}
};

class Binary_node:public Expr_node
{
	friend class Expr;

	string op;
	Expr lopnd;
	Expr ropnd;

	Binary_node(const string& a,Expr b,Expr c):op(a),lopnd(b),ropnd(c){}
	void print(ostream& o) const{o<<'('<<lopnd<<op<<ropnd<<')';}

	int eval()const
	{
		int op1=lopnd.eval();
		int op2=ropnd.eval();

		if(op=="-")
			return op1-op2;
		if(op=="+")
			return op1+op2;
		if(op=="*")
			return op1*op2;
		if(op=="/")
			return op1/op2;
			
		throw "error,bad op";
	}
};

class Ternary_node:public Expr_node
{
	friend class Expr;

	string op;
	Expr lopnd;
	Expr mopnd;
	Expr ropnd;

	Ternary_node(const string& a,Expr b,Expr c, Expr d):op(a),lopnd(b),mopnd(c),ropnd(d){}
	void print(ostream& o)const
	{
		o<<'('<<lopnd<<"?"<<mopnd<<":"<<ropnd<<")";
	}

	int eval()const
	{
		if(lopnd.eval())
			return mopnd.eval();
		else
			return ropnd.eval();
	}
};

Expr::Expr(int n)
{
	p=new Int_node(n);
}

Expr::Expr(const string& op,Expr t)
{
	p=new Unary_node(op,t);
}

Expr::Expr(const string& op,Expr left,Expr right)
{
	p=new Binary_node(op,left,right);
}

Expr::Expr(const string& op,Expr left,Expr middle,Expr right)
{
	p=new Ternary_node(op,left,middle,right);
}

Expr::Expr(const Expr& t)
{
	p=t.p;
	++p->use;
}

Expr& Expr::operator=(const Expr& rhs)
{
	rhs.p->use++;
	if(--p->use==0)
		delete p;
	p=rhs.p;
	return *this;
}

ostream& operator<<(ostream& o,const Expr& t)
{
	t.p->print(o);
	return o;
}

int main(int argc, char* argv[])
{
	Expr t=Expr("*",Expr("-",5),Expr("+",3,4));
	cout<<t<<"="<<t.eval()<<endl;
	t=Expr("*",t,t);
	cout<<t<<"="<<t.eval()<<endl;
	t=Expr("*",t,t);
	cout<<t<<"="<<t.eval()<<endl;

	Expr t1=Expr(" ",1,22,33);
	cout<<t1<<"="<<t1.eval()<<endl;
		
	return 0;
}