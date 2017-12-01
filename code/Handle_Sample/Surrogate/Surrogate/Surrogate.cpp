#include <string>
#include <iostream>
using namespace std;

class vehicle
{
public:
	virtual void start() = 0;
	virtual vehicle* copy() const = 0;	//����һ������������麯��
	virtual ~vehicle(){};				//����������
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

class vehicleSurrogate		//������
{
public:
	vehicleSurrogate();		//����������Ҫ
	/*explicit*/ vehicleSurrogate(const vehicle&);	//������ʽת��
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
	//vehicle parking_lot[1000];		//ֻ�ܴ洢vehicle���󣬵���vehicleΪ�飬������������������أ�
	//vehicle *parking_lot[1000];		//���Լ�������࣬����ȱ���ָ���޷�������ָ���������ʱ�䣬
										//һ����ָ������ʧ��ָ��Ͳ�֪��ָ��ʲô������
	//Autovehicle x=**;
	//parking_lot[1000]=new Autovehicle(x);		//��ͨһ�£��洢���������������˶�̬�ڴ����ĸ�����
												//����Ҫ�������Ǳ�������֪��Autovehicle������Ͳ�����ô��
												//�������Ҫ�洢��Ԫ��qͬ�������͸���ô���أ�
	//if(p!=q)									
	//{
	//	delete parking_lot[p];
	//	parking_lot[p]=parking_lot[q];			//ָ����ͬ���Ķ���
	//}

	//if(p!=q)
	//{
	//	delete parking_lot[p];
	//	parking_lot[p]=new vehicle(*parking_lot[q]) ;		//vehicleΪ�飬���㲻Ϊ�飬Ҳ�ᱻ�ָ�
	//}

	vehicleSurrogate parking_lot[1000];			//ʹ�ô�����
	Autovehicle x;
	parking_lot[2] = x;			//�൱��parking_lot[2] = vehicleSurrogate(x);ƥ��operator=ʱ�ù��캯��������ʽת��

	parking_lot[2].start();
	
	return 0;
}