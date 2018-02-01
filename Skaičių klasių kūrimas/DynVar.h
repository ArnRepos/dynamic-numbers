#pragma once

class DN
{
public:
	DN();
	DN(unsigned int a);
	DN(const DN &x);
	~DN();
	void operator = (DN x);
	void operator ++ (int);
	void operator -- (int);
	DN operator + (DN &x);
	void operator += (DN &x);
	DN operator - (DN &x);
	void operator -= (DN &x);
	DN operator * (DN &x);
	void operator *= (DN &x);
	DN operator / (DN x);
	DN operator /= (DN x);
	DN operator % (DN x);
	void operator %= (DN x);
	bool operator < (DN &x);
	bool operator > (DN &x);
	bool operator >= (DN &x);
	bool operator <= (DN &x);
	bool operator == (DN &x);
	unsigned int ToInt();
	unsigned long long ToIntLow64();
	double ToDouble();
	long double ToLongDouble();
	void multx2();
	void multx2pow(unsigned int x);
	bool divdx2();
	bool Positive();
	void SetPower(DN &x);
	int log2();
	char *TextNumberEx(unsigned int &l, unsigned __int8 s);
	char *TextNumberEx(unsigned __int8 s);
	char *TextNumber();
protected:
	unsigned int sz, csz;
	unsigned __int8 *sk;
	bool bigger(DN &x, unsigned int B);
private:
	bool fitplus(DN b);
	bool fitplus1();
	void Shorten();
	bool bigger(DN &x);
	bool same(DN &x);
	void alloc(unsigned int a);
	bool Bit(unsigned int a);
};

class DZ
{
public:
	DZ();
	DZ(int x);
	DZ(DN &x);
	void operator += (DZ &x);
	DZ operator + (DZ &x);
	void operator -= (DZ &x);
	DZ operator - (DZ &x);
	void operator *= (DZ &x);
	DZ operator * (DZ &x);
	void operator /= (DZ &x);
	DZ operator / (DZ &x);
	void operator %= (DZ &x);
	DZ operator % (DZ &x);
	bool DZ::operator == (DZ &x);
	void SetPower(DZ &x);
	int ToInt();
protected:
	DN dn;
	bool Pos;
private:
	void Add(DN &x);
	void Subtract(DN &x);
};

class DQ
{
public:
	DQ();
	DQ(unsigned int x);
	DQ(const char *x, unsigned __int8 s);
	DQ(DN &x);
	DQ(DQ &x);
	void operator = (int x);
	void operator = (DN x);
	void operator = (DQ x);
	DQ operator + (DQ x);
	void operator += (DQ x);
	DQ operator - (DQ x);
	void operator -= (DQ x);
	DQ operator * (DQ x);
	void operator *= (DQ x);
	DQ operator / (DQ x);
	void operator /= (DQ x);
	bool operator > (DQ x);
	bool operator < (DQ x);
	void SetPower(int x);
	void Inv();
	double DoubleApprox();
	char *TextNumberEx(unsigned int &l, unsigned int max, unsigned __int8 s);
	char *TextNumberEx(unsigned int max, unsigned __int8 s);
//protected:
	DN skai, vard;
//private:
	void Bendrav();
	DN DBD(DN a, DN b);
	void Nullify();
};
#ifdef _STRING_
std::string StrNumber(DN &Number, unsigned __int8 System);
std::string StrNumber(DN &Number);
std::string StrNumber(DQ &Number, unsigned int Max, unsigned __int8 System);
std::string StrNumber(DQ &Number, unsigned int Max);
#endif
long double clog2list(unsigned __int8 e);