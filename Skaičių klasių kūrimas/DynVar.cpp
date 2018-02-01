#include "stdafx.h"
#include "DynVar.h"

DN::DN()
{
	sz = csz = 0;
	sk = nullptr;
}
DN::DN(unsigned int a)
{
	sk = new unsigned __int8[4];
	for (int i = 0; i < 4; i++)
	{
		sk[i] = a % 256;
		a /= 256;
	}
	sz = csz = 4;
	Shorten();
}
DN::DN(const DN &x)
{
	csz = sz = x.sz;
	if (sz > 0)
	{
		sk = new unsigned __int8[sz];
		memcpy(sk, x.sk, sz);
	}
}
DN::~DN()
{
	if (csz > 0)
	{
		delete[] sk;
	}
	return;
}
void DN::operator = (DN x)
{
	if (csz > 0)
	{
		delete[] sk;
	}
	csz = sz = x.sz;
	sk = new unsigned __int8[sz];
	for (unsigned int i = 0; i < sz; i++)
	{
		sk[i] = x.sk[i];
	}
	return;
}
DN DN::operator + (DN &x)
{
	DN a = this[0];
	a += x;
	return a;
}
void DN::operator += (DN &x)
{
	if (sz == 0)
	{
		this[0] = x;
		return;
	}
	if (!fitplus(x))
	{
		alloc((sz > x.sz) ? 1 : (x.sz - sz + 1));
		/*unsigned int nsz = (csz > x.sz) ? csz : x.sz;
		unsigned __int8 *nsk = new unsigned __int8[nsz + 1];
		for (unsigned int i = 0; i < nsz; i++)
		{
			nsk[i] = sk[i];
		}
		delete[] sk;
		for (unsigned int i = csz; i <= nsz; i++)
		{
			nsk[i] = 0;
		}
		nsk[nsz] = 0;
		sk = nsk;
		csz = nsz + 1;*/
	}
	bool mintyje = false;
	for (unsigned int i = 0; i < sz; i++)
	{
		unsigned __int8 xsk = (i < x.sz) ? x.sk[i] : 0;
		bool nmintyje = ((int)sk[i] + (int)xsk + (int)mintyje > 255);
		sk[i] = (unsigned __int8)(((int)sk[i] + (int)xsk + (int)mintyje) % 256);
		mintyje = nmintyje;
	}
	Shorten();
	return;
}
DN DN::operator - (DN &x)
{
	DN a = this[0];
	a -= x;
	return a;
}
void DN::operator -= (DN &x)
{
	if (!bigger(x))
	{
		bool mintyje = false;
		for (unsigned int i = 0; i < x.sz || mintyje; i++)
		{
			unsigned __int8 xsk = (i < x.sz) ? x.sk[i] : 0;
			bool nmintyje = ((int)sk[i] < (int)xsk + (int)mintyje);
			sk[i] = (unsigned __int8)(((int)sk[i] + (int)256 - (int)xsk - (int)mintyje) % 256);
			mintyje = nmintyje;
		}
		Shorten();
	}
	else
	{
		delete[] sk;
		sk = nullptr;
		sz = 0;
	}
	return;
}
DN DN::operator * (DN &x)
{
	DN a = this[0];
	a *= x;
	return a;
}
void DN::operator *= (DN &x)
{
	DN t(this[0]);
	if (sz > 0)
	{
		if (sz == 1 && sk[0] == 1)
		{
			this[0] = x;
			return;
		}
		//sz = ((log2() + x.log2() + 3) / 8) + 1;
		for (unsigned int i = 0; i < sz; i++)
		{
			sk[i] = 0;
		}
		alloc(x.sz);
		/*sz += x.sz;
		delete[] sk;
		sk = nullptr;
		sk = new unsigned __int8[sz];
		for (unsigned int i = 0; i < sz; i++)
		{
			sk[i] = 0;
		}*/
	}
	else return;
	for (unsigned int i = 0; i < x.sz; i++)
	{
		if (x.sk[i] > 0)
		{
			unsigned __int16 p = 0;
			for (unsigned int j = 0; j < t.sz; j++)
			{
				p += sk[j + i] + (t.sk[j] * x.sk[i]);
				sk[j + i] = p % 256;
				p /= 256;
			}
			if (p > 0)
			{
				sk[i + t.sz] += p;
			}
		}
	}
	Shorten();
}
DN DN::operator / (DN x)
{
	DN a = this[0];
	a /= x;
	return a;
}
DN DN::operator /= (DN x)
{
	if (sz == 0 || x.sz == 0)
	{
		return (DN)0;
	}
	if (x.sz == 1)
	{
		unsigned __int8 k = 0;
		for (unsigned int i = sz; i > 0; i--)
		{
			unsigned __int16 n = (256 * k) + sk[i - 1];
			k = n % x.sk[0];
			sk[i - 1] = n / x.sk[0];
		}
		Shorten();
		return (DN)k;
	}
	if (bigger(x))
	{
		DN l = this[0];
		delete[] sk;
		sk = nullptr;
		csz = sz = 0;
		return l;
	}
	int n = log2() - x.log2();
	DN t(this[0]), *v = new DN[8];
	v[0] = x;
	for (unsigned int i = 1; i < ((n < 8) ? n + 1 : 8); i++)
	{
		v[i] = v[i - 1];
		v[i].multx2();
	}
	delete[] sk;
	sk = nullptr;
	csz = sz = 0;
	while (n >= 0)
	{
		multx2();
		if (!t.bigger(v[n % 8], n / 8))
		{
			//t -= v[n % 8];
			{
				bool mintyje = false;
				for (unsigned int i = n / 8; i < v[n % 8].sz + (n / 8) || mintyje; i++)
				{
					unsigned __int8 xsk = (i - (n / 8) < v[n % 8].sz) ? v[n % 8].sk[i - (n / 8)] : 0;
					bool nmintyje = ((int)t.sk[i] < (int)xsk + (int)mintyje);
					t.sk[i] = (unsigned __int8)(((int)t.sk[i] + (int)256 - (int)xsk - (int)mintyje) % 256);
					mintyje = nmintyje;
				}
				t.Shorten();
			}
			this[0]++;
		}
		n--;
	}
	delete[] v;
	return t;
}
DN DN::operator % (DN x)
{
	if (sz == 0 || x.sz == 0)
	{
		return DN();
	}
	if (bigger(x))
	{
		return this[0];
	}
	if (x.sz == 1)
	{
		/*if (sz == 1)
		{
			return (DN)(sk[0] % x.sk[0]);
		}*/
		DN t(this[0]);
		unsigned __int8 k = 0;
		for (unsigned int i = sz; i > 0; i--)
		{
			unsigned __int16 n = (256 * k) + t.sk[i - 1];
			k = n % x.sk[0];
			t.sk[i - 1] = n / x.sk[0];
		}
		return k;
	}
	int n = log2() - x.log2();
	if (n < 8)
	{
		x.multx2pow(n);
		DN t(this[0]);
		while (n >= 0)
		{
			if (t >= x)
			{
				t -= x;
			}
			x.divdx2();
			n--;
		}
		return t;
	}
	else
	{
		DN t = this[0], *v = new DN[8];
		v[0] = x;
		for (unsigned int i = 1; i < 8; i++)
		{
			v[i] = v[i - 1];
			v[i].multx2();
		}
		while (n >= 0)
		{
			if (!t.bigger(v[n % 8], n / 8))
			{
				//t -= v[n % 8];
				{
					bool mintyje = false;
					for (unsigned int i = n / 8; i < v[n % 8].sz + (n / 8) || mintyje; i++)
					{
						unsigned __int8 xsk = (i - (n / 8) < v[n % 8].sz) ? v[n % 8].sk[i - (n / 8)] : 0;
						bool nmintyje = ((int)t.sk[i] < (int)xsk + (int)mintyje);
						t.sk[i] = (unsigned __int8)(((int)t.sk[i] + (int)256 - (int)xsk - (int)mintyje) % 256);
						mintyje = nmintyje;
					}
					t.Shorten();
				}
			}
			n--;
		}
		delete[] v;
		return t;
	}
	/*int n = log2() - x.log2();
	x.multx2pow(n);
	DN t(this[0]);
	while (n >= 0)
	{
		if (t >= x)
		{
			t -= x;
		}
		x.divdx2();
		n--;
	}
	return t;*/
}
void DN::operator %= (DN x)
{
	this[0].operator=(this[0] % x);
}
void DN::operator ++ (int)
{
	unsigned int m = 0;
	if (!Positive())
	{
		alloc(1);
	}
	while (sk[m] == 255)
	{
		sk[m] = 0;
		m++;
		if (m == sz)
		{
			alloc(1);
		}
	}
	sk[m]++;
}
void DN::operator -- (int)
{
	if (sz == 0)
	{
		return;
	}
	unsigned int i = 0;
	while (sk[i] == 0)
	{
		sk[i] = 255;
		i++;
	}
	sk[i]--;
}
bool DN::operator < (DN &x)
{
	return bigger(x);
}
bool DN::operator > (DN &x)
{
	return !bigger(x) && !same(x);
}
bool DN::operator >= (DN &x)
{
	return !bigger(x);
}
bool DN::operator <= (DN &x)
{
	return bigger(x) || same(x);
}
bool DN::operator == (DN &x)
{
	return same(x);
}
unsigned int DN::ToInt()
{
	if (sz == 0)
	{
		return 0;
	}
	unsigned int is = 0;
	for (int i = (sz <= 3) ? sz - 1 : 3; i >= 0; i--)
	{
		is *= 256;
		is += sk[i];
	}
	return is;
}
unsigned long long DN::ToIntLow64()
{
	if (sz == 0)
	{
		return 0;
	}
	unsigned long long is = 0;
	for (int i = (sz <= 7) ? sz - 1 : 7; i >= 0; i--)
	{
		is *= 256;
		is += sk[i];
	}
	return is;
}
double DN::ToDouble()
{
	double d = 0;
	for (unsigned int i = sz; i > 0; i--)
	{
		d *= 256.0;
		d += sk[i - 1];
	}
	return d;
}
long double DN::ToLongDouble()
{
	long double d = 0;
	for (unsigned int i = sz; i > 0; i--)
	{
		d *= 256.0;
		d += sk[i - 1];
	}
	return d;
}
void DN::multx2()
{
	if (sz == 0)
	{
		return;
	}
	if (sk[sz - 1] > 127)
	{
		alloc(1);
	}
	bool mintyje = false;
	for (unsigned int i = 0; i < sz; i++)
	{
		bool nmintyje = (sk[i] > 127);
		sk[i] = ((2 * sk[i]) + mintyje) % 256;
		mintyje = nmintyje;
	}
}
void DN::multx2pow(unsigned int x)
{
	if (sz > 0)
	{
		unsigned int a = ((log2() % 8) + x) / 8, b = x / 8;
		if (a > 0)
		{
			alloc(a);
			for (unsigned int i = 0; i < sz; i++)
			{
				sk[i] = (i - b < sz - a) ? ((i >= b) ? sk[i - b] : 0) : 0;
			}
			/*sz += a;
			unsigned __int8 *nsk = new unsigned __int8[sz];
			for (unsigned int i = 0; i < sz; i++)
			{
				nsk[i] = (i - b < sz - a) ? ((i >= b) ? sk[i - b] : 0) : 0;
			}
			delete[] sk;
			sk = nsk;*/
		}
		if (x % 8 > 0)
		{
			unsigned __int8 mintyje = 0, p = 128;
			for (unsigned int i = 1; i < x % 8; i++)
			{
				p /= 2;
			}
			for (unsigned int i = 0; i < sz; i++)
			{
				unsigned __int8 nmintyje = (sk[i] >= p) ? sk[i] / p : 0;
				sk[i] = ((((int)256 / (int)p) * sk[i]) + mintyje) % 256;
				mintyje = nmintyje;
			}
		}
	}
}
bool DN::divdx2()
{
	bool mintyje = false;
	for (int i = sz - 1; i >= 0; i--)
	{
		bool nmintyje = (sk[i] % 2 == 1);
		sk[i] /= 2;
		sk[i] += 128 * (int)mintyje;
		mintyje = nmintyje;
	}
	Shorten();
	return mintyje;
}
bool DN::Positive()
{
	if (sz == 0)
	{
		return false;
	}
	else if (sz == 1 && sk[0] == 0)
	{
		return false;
	}
	return true;
}
void DN::SetPower(DN &x)
{
	if (sz == 0)
	{
		return;
	}
	if (!x.Positive() || (sk[0] == 1 && sz == 1))
	{
		this->operator=(1);
		return;
	}
	DN t = this[0];
	this->operator=(1);
	for (unsigned int i = 0; i < x.sz; i++)
	{
		unsigned __int8 b = x.sk[i];
		for (unsigned int j = 0; ((b > 0) || (i < x.sz - 1)) && (j < 8); j++)
		{
			if (b % 2)
			{
				this->operator *= (t);
			}
			b /= 2;
			if (i < x.sz - 1 || b > 0)
			{
				t *= (DN)t;
			}
		}
	}
	return;
}
bool DN::fitplus(DN b)
{
	if (sz < b.sz)
	{
		return false;
	}
	for (unsigned int i = sz; i > b.sz; i--)
	{
		if (sk[i - 1] < 255)
		{
			return true;
		}
	}
	for (unsigned int i = (sz < b.sz) ? sz : b.sz; i > 0; i--)
	{
		if (sk[i - 1] + b.sk[i - 1] > 255)
		{
			return false;
		}
		else if (sk[i - 1] + b.sk[i - 1] < 255)
		{
			return true;
		}
	}
	return true;
}
bool DN::fitplus1()
{
	if (sz == 0)
	{
		return false;
	}
	for (int i = sz; i > 0; i--)
	{
		if (sk[i - 1] < 255)
		{
			return true;
		}
	}
	return false;
}
void DN::Shorten()
{
	while (sz > 0)
	{
		if (sk[sz - 1] == 0)
		{
			sz--;
		}
		else
		{
			break;
		}
	}
	if (sz == 0)
	{
		csz = 0;
		delete[] sk;
		sk = nullptr;
	}
}
bool DN::bigger(DN &x)
{
	if (x.sz > sz)
	{
		return true;
	}
	if (x.sz < sz)
	{
		return false;
	}
	for (int i = x.sz - 1; i >= 0; i--)
	{
		if (x.sk[i] > sk[i])
		{
			return true;
		}
		if (x.sk[i] < sk[i])
		{
			return false;
		}
	}
	return false;
}
bool DN::bigger(DN & x, unsigned int B)
{
	if (x.sz + B > sz)
	{
		return true;
	}
	if (x.sz + B < sz)
	{
		return false;
	}
	for (int i = sz - 1; i >= 0; i--)
	{
		unsigned __int8 v = (i >= B) ? x.sk[i - B] : 0;
		if (v > sk[i])
		{
			return true;
		}
		if (v < sk[i])
		{
			return false;
		}
	}
	return false;
}
bool DN::same(DN &x)
{
	unsigned int xs = x.sz;
	if (xs > sz)
	{
		return false;
	}
	if (xs < sz)
	{
		return false;
	}
	for (unsigned int i = 0; i < xs; i++)
	{
		if (x.sk[i] != sk[i])
		{
			return false;
		}
	}
	return true;
}
void DN::alloc(unsigned int a)
{
	sz += a;
	if (sz > csz)
	{
		if (csz == 0)
		{
			csz = a;
			sk = new unsigned __int8[csz];
			memset(sk, 0, csz);
			return;
		}
		unsigned __int8 *nsk = new unsigned __int8[sz];
		memcpy(nsk, sk, csz);
		for (unsigned int i = csz; i < sz; i++)
		{
			nsk[i] = 0;
		}
		delete[] sk;
		sk = nsk;
		csz = sz;
	}
}
int DN::log2()
{
	if (sz == 0)
	{
		return -1; // Teoriðkai bûtø minus begalybë.
	}
	int l = (sz - 1) * 8;
	for (unsigned __int8 a = sk[sz - 1]; a > 0; a /= 2)
	{
		l++;
	}
	return l - 1;
}
bool DN::Bit(unsigned int a)
{
	if (a / 8 > sz)
	{
		return false;
	}
	unsigned __int8 b = sk[a / 8];
	for (unsigned int i = 0; i < a % 8; i++)
	{
		b /= 2;
	}
	return (bool)(b % 2);
}
char *DN::TextNumberEx(unsigned int &l, unsigned __int8 s)
{
	if (s > 36 || s < 2)
	{
		l = 1;
		char *ErrMark = new char[2];
		ErrMark[0] = '!';
		ErrMark[1] = '\0';
		return ErrMark;
	}
	if (sz == 0)
	{
		l = 1;
		char *Zero = new char[2];
		Zero[0] = '0';
		Zero[1] = '\0';
		return Zero;
	}
	char *Text;
	if (s == 2 || s == 4 || s == 16)
	{
		unsigned int u = 8;
		for (unsigned int a = 2; a < s; a *= a)
		{
			u /= 2;
		}
		l = sz * u;
		Text = new char[l + 1];
		Text[l] = 0;
		Text[0] = '0';
		unsigned __int8 n = 0, m = sk[0];
		for (unsigned int i = l; i > 0; i--)
		{
			unsigned __int8 k = m % s;
			m /= s;
			Text[i - 1] = (char)(k + 48);
			if ((i - 1) % u == 0)
			{
				n++;
				m = sk[n];
			}
		}
	}
	else
	{
		const long double clog2 = clog2list(s);
		l = (unsigned int)((long double)(log2() + 1) / (long double)clog2) + 1;
		Text = new char[l + 1];
		Text[l] = 0;
		Text[0] = '0';
		DN a = this[0];
		for (unsigned int i = l; a.Positive(); i--)
		{
			DN c = (a /= s);
			Text[i - 1] = (char)(c.ToInt() + 48);
		}
	}
	if (s > 10)
	{
		for (unsigned int i = 0; i < l; i++)
		{
			if (Text[i] >= 58)
			{
				Text[i] += 7;
			}
		}
	}
	unsigned int h = 0;
	while (Text[h] == '0')
	{
		h++;
	}
	if (h > 0)
	{
		const unsigned int nsz = (l - h) + 1;
		char *sText = new char[nsz];
		for (unsigned int i = 0; i < nsz; i++)
		{
			sText[i] = Text[i + h];
		}
		delete[] Text;
		l = nsz - 1;
		return sText;
	}
	return Text;
}
char *DN::TextNumberEx(unsigned __int8 s)
{
	unsigned int Length;
	return TextNumberEx(Length, s);
}
char *DN::TextNumber()
{
	unsigned int l = 0;
	return TextNumberEx(l, 10);
}

DZ::DZ()
{
	Pos = false;
}
DZ::DZ(int x)
{
	Pos = true;
	if (x < 0)
	{
		Pos = false;
		x *= -1;
	}
	dn = DN(x);
}
DZ::DZ(DN &x)
{
	dn = DN(x);
	Pos = true;
}
void DZ::operator += (DZ &x)
{
	if (Pos == x.Pos)
	{
		Add(x.dn);
	}
	else
	{
		Subtract(x.dn);
	}
}
DZ DZ::operator + (DZ &x)
{
	DZ a = this[0];
	a += x;
	return a;
}
void DZ::operator -= (DZ &x)
{
	if (Pos == x.Pos)
	{
		Subtract(x.dn);
	}
	else
	{
		Add(x.dn);
	}
}
DZ DZ::operator - (DZ &x)
{
	DZ a = this[0];
	a -= x;
	return a;
}
void DZ::operator *= (DZ &x)
{
	Pos = (Pos == x.Pos);
	dn *= x.dn;
	if (!dn.Positive())
	{
		Pos = true;
	}
}
DZ DZ::operator * (DZ &x)
{
	DZ a = this[0];
	a *= x;
	return a;
}
void DZ::operator /= (DZ &x)
{
	Pos = (Pos == x.Pos);
	dn /= x.dn;
	if (!dn.Positive())
	{
		Pos = true;
	}
}
DZ DZ::operator / (DZ &x)
{
	DZ a = this[0];
	a /= x;
	return a;
}
void DZ::operator %= (DZ &x)
{
	dn %= x.dn;
	if (!dn.Positive())
	{
		Pos = true;
	}
}
DZ DZ::operator % (DZ &x)
{
	DZ a = this[0];
	a %= x;
	return a;
}
bool DZ::operator == (DZ &x)
{
	return (Pos == x.Pos) && (dn == x.dn);
}
void DZ::SetPower(DZ &x)
{
	if (!Pos)
	{
		if (x % (DZ)2 == (DZ)0)
		{
			Pos = true;
		}
	}
	if (!x.Pos)
	{
		dn = 0;
	}
	dn.SetPower(x.dn);
}
int DZ::ToInt()
{
	return (int)dn.ToInt() * (1 - (2 * (!Pos)));
}
void DZ::Add(DN &x)
{
	dn += x;
	if (!dn.Positive())
	{
		Pos = true;
	}
}
void DZ::Subtract(DN &x)
{
	if (dn < x)
	{
		Pos = !Pos;
		dn = x - dn;
	}
	else
	{
		dn -= x;
	}
	if (!dn.Positive())
	{
		Pos = true;
	}
}

DQ::DQ()
{
}
DQ::DQ(unsigned int x)
{
	skai = x;
	vard = 1;
	return;
}
DQ::DQ(const char * x, unsigned __int8 s)
{
	vard = 1;
	if (s > 10)
	{
		skai = 1;
		return;
	}
	DN S = s;
	unsigned int l = 0, m = 0;
	bool b = false;
	while (x[l] >= 48 && x[l] < (s + 48))
	{
		skai *= S;
		skai += (DN)(x[l] - 48);
		l++;
		if (b)
		{
			vard *= S;
			m++;
		}
		else if (x[l] == ',' || x[l] == '.')
		{
			b = true;
			l++;
		}
	}
	Bendrav();
	if (x[l] == '(')
	{
		l++;
		unsigned int psz = 0;
		DQ p;
		p.vard = S;
		while (x[l] >= 48 && x[l] < (s + 48))
		{
			DN n = x[l] - 48;
			p.skai.operator += (n);
			p.skai *= S;
			psz++;
			l++;
		}
		p.vard.SetPower((DN)(psz));
		p.vard--;
		DN M = S;
		M.SetPower((DN)(m + 1));
		p.vard *= M;
		this[0] += p;
	}
}
DQ::DQ(DN &x)
{
	skai = x;
	vard = 1;
}
DQ::DQ(DQ &x)
{
	skai = x.skai;
	vard = x.vard;
}
void DQ::operator = (int x)
{
	skai = x;
	vard = 1;
	return;
}
void DQ::operator = (DN x)
{
	skai = x;
	vard = 1;
	return;
}
void DQ::operator = (DQ x)
{
	skai = x.skai;
	vard = x.vard;
	return;
}
DQ DQ::operator+(DQ x)
{
	DQ a = this[0];
	a += x;
	return a;
}
void DQ::operator += (DQ x)
{
	/*x.skai *= vard;
	skai *= x.vard;
	vard *= x.vard;
	skai += x.skai;
	Bendrav();*/
	DN mbk = vard, dbd = DBD(vard, x.vard);
	mbk *= x.vard;
	mbk /= dbd;
	DN xv = x.skai;
	xv *= vard;
	skai *= x.vard;
	skai += xv;
	skai /= dbd;
	vard = mbk;
	return;
}
DQ DQ::operator-(DQ x)
{
	DQ a = this[0];
	a -= x;
	return a;
}
void DQ::operator -= (DQ x)
{
	/*x.skai *= vard;
	skai *= x.vard;
	if (skai < x.skai)
	{
		Nullify();
		return;
	}
	vard *= x.vard;
	skai -= x.skai;
	Bendrav();*/
	DN mbk = vard * x.vard, dbd = DBD(vard, x.vard);
	mbk /= dbd;
	DN xv = x.skai * vard;
	skai *= x.vard;
	skai -= xv;
	if (skai.Positive())
	{
		skai /= dbd;
		vard = mbk;
	}
	else
	{
		vard = 1;
	}
	return;
}
DQ DQ::operator*(DQ x)
{
	DQ a = this[0];
	a *= x;
	return a;
}
void DQ::operator *= (DQ x)
{
	DN dbd = DBD(skai, x.vard);
	dbd *= DBD(vard, x.skai);
	skai *= x.skai;
	vard *= x.vard;
	skai /= dbd;
	vard /= dbd;
	return;
}
DQ DQ::operator / (DQ x)
{
	DQ a = this[0];
	a *= x;
	return a;
}
void DQ::operator /= (DQ x)
{
	DN xs = x.skai;
	x.skai = x.vard;
	x.vard = xs;
	this->operator *= (x);
	return;
}
bool DQ::operator > (DQ x)
{
	x.skai *= vard;
	DN nskai = skai;
	nskai *= x.vard;
	int i = 0;
	return nskai > x.skai;
}
bool DQ::operator < (DQ x)
{
	x.skai *= vard;
	DN nskai = skai;
	nskai *= x.vard;
	return nskai < x.skai;
}
void DQ::SetPower(int x)
{
	if (x < 0)
	{
		DN xs = skai;
		skai = vard;
		vard = xs;
		x *= -1;
	}
	DN m = x;
	skai.SetPower(m);
	vard.SetPower(m);
	return;
}
void DQ::Inv()
{
	DN t = skai;
	skai = vard;
	vard = t;
}
double DQ::DoubleApprox()
{
	return (double)skai.ToInt() / (double)vard.ToInt();
}
char * DQ::TextNumberEx(unsigned int & l, unsigned int m, unsigned __int8 s)
{
	if (s > 36 || s < 2)
	{
		return nullptr;
	}
	if (m == 0)
	{
		DN a = skai / vard;
		return a.TextNumberEx(l, s);
	}
	DN a = skai, S = s;
	DN b;
	b = (a /= vard);
	unsigned int c = 0;
	char *First = a.TextNumberEx(c, s);
	if (!b.Positive())
	{
		return First;
	}
	char *Second = new char[m], *Period = nullptr;
	DN *v = new DN[m];
	unsigned int n = 0;
	while (n < m && b.Positive() && !Period)
	{
		v[n] = b;
		b *= S;
		a = (b /= vard);
		Second[n] = b.ToInt() + 48;
		if (Second[n] >= 58)
		{
			Second[n] += 7;
		}
		for (unsigned int i = 0; i <= n; i++)
		{
			if (a == v[i])
			{
				m = n - i + 1;
				Period = new char[m];
				n = i;
				for (unsigned int j = 0; j < m; j++)
				{
					Period[j] = Second[j + i];
				}
				break;
			}
		}
		if (!Period)
		{
			b = a;
			n++;
		}
	}
	delete[] v;
	l = c + n + 1;
	if (Period)
	{
		l += m + 2;
	}
	char *Full = new char[l + 1];
	Full[l] = 0;
	for (unsigned int i = 0; i < c; i++)
	{
		Full[i] = First[i];
	}
	Full[c] = ',';
	for (unsigned int i = 0; i < n; i++)
	{
		Full[i + c + 1] = Second[i];
	}
	if (Period)
	{
		Full[n + c + 1] = '(';
		for (unsigned int i = 0; i < m; i++)
		{
			Full[i + n + c + 2] = Period[i];
		}
		Full[m + n + c + 2] = ')';
		delete[] Period;
	}
	delete[] First;
	delete[] Second;
	return Full;
}
char * DQ::TextNumberEx(unsigned int max, unsigned __int8 s)
{
	unsigned int x = 0;
	return TextNumberEx(x, max, s);
}
void DQ::Bendrav()
{
	DN dbd = DBD(skai, vard);
	skai /= dbd;
	vard /= dbd;
}
DN DQ::DBD(DN a, DN b)
{
	while (a.Positive() && b.Positive())
	{
		a %= b;
		if (b.Positive() && a.Positive())
		{
			b %= a;
		}
		if (!(b.Positive()))
		{
			return a;
		}
	}
	return b;
}
void DQ::Nullify()
{
	skai = 0;
	vard = 0;
	return;
}

#ifdef _STRING_
std::string StrNumber(DN &Number, unsigned __int8 System)
{
	char *c = Number.TextNumberEx(System);
	string str(c);
	delete[] c;
	return str;
}
std::string StrNumber(DN &Number)
{
	char *c = Number.TextNumber();
	string str(c);
	delete[] c;
	return str;
}
std::string StrNumber(DQ &Number, unsigned int Max, unsigned __int8 System)
{
	char *c = Number.TextNumberEx(Max, System);
	string str(c);
	delete[] c;
	return str;
}
std::string StrNumber(DQ &Number, unsigned int Max)
{
	char *c = Number.TextNumberEx(Max, 10);
	string str(c);
	delete[] c;
	return str;
}
#endif
long double clog2list(unsigned __int8 e)
{
	switch (e)
	{
	case 1: return 0.0;
	case 2: return 1.0;
	case 3: return 1.5849625007211561;
	case 4: return 2.0;
	case 5: return 2.3219280948873622;
	case 6: return 2.5849625007211561;
	case 7: return 2.8073549220576042;
	case 8: return 3.0;
	case 9: return 3.1699250014423122;
	case 10: return 3.3219280948873622;
	case 11: return 3.4594316186372973;
	case 12: return 3.5849625007211561;
	case 13: return 3.7004397181410922;
	case 14: return 3.8073549220576042;
	case 15: return 3.9068905956085187;
	case 16: return 4.0;
	case 17: return 4.0874628412503391;
	case 18: return 4.1699250014423122;
	case 19: return 4.2479275134435852;
	case 20: return 4.3219280948873626;
	case 21: return 4.3923174227787607;
	case 22: return 4.4594316186372973;
	case 23: return 4.5235619560570131;
	case 24: return 4.5849625007211561;
	case 25: return 4.6438561897747244;
	case 26: return 4.7004397181410926;
	case 27: return 4.7548875021634682;
	case 28: return 4.8073549220576037;
	case 29: return 4.8579809951275719;
	case 30: return 4.9068905956085187;
	case 31: return 4.9541963103868749;
	case 32: return 5.0;
	case 33: return 5.0443941193584534;
	case 34: return 5.0874628412503391;
	case 35: return 5.1292830169449664;
	case 36: return 5.1699250014423122;
	default: return 0.0;
	}
}