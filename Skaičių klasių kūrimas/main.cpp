// main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

bool Prime(DN x)
{
	DN l = 2, p = (DN)x.log2() / (DN)2;
	l.SetPower(p);
	for (DN i = 2; i <= l; i++)
	{
		DN m = (x % i);
		if (m == (DN)0)
		{
			return false;
		}
	}
	return true;
}

// PI(10^2) trunka apie 14 milisek.
// PI(10^3) trunka apie 3 sek.
// PI(10^4) trunka apie 1664 sek. (27 minutes ir 44 sekundes).
DQ PI(unsigned int n)
{
	if (n == 0)
	{
		return 3;
	}
	static DQ *Val = nullptr;
	static unsigned int ArrSize = 0;
	if (n > ArrSize)
	{
		if (!Val)
		{
			Val = new DQ[n];
		}
		else
		{
			DQ *NewVal = new DQ[ArrSize + 1];
			for (unsigned int i = 0; i < ArrSize; i++)
			{
				NewVal[i] = Val[i];
			}
			delete[] Val;
			Val = NewVal;
		}
	}
	else
	{
		return Val[n - 1];
	}
	for (unsigned int i = ArrSize; i < n; i++)
	{
		DQ a = 1;
		//a /= (i + 1);
		a /= (1 + (i * 2));
		if (i > 0)
		{
			//a += Val[i - 1];
			a = (i % 2 == 0) ? (Val[i - 1] + a) : (Val[i - 1] - a);
		}
		Val[i] = a;
	}
	ArrSize = n;
	return Val[n - 1] * 4;
}

void TestMod(DN a, DN b)
{
	ofstream duotis("sk.txt");
	duotis << StrNumber(a) << " / " << StrNumber(b) << " = ";
	DN c = (a /= b);
	duotis << StrNumber(a) << " su " << StrNumber(c) << " liekana. Liekana ";
	duotis << ((c > b) ? "didesnë" : "nedidesnë") << " uþ daliklá.";
	duotis.close();
}

DQ TestPi(unsigned int n)
{
	/*string Number;
	ifstream gautis("sk.txt");
	gautis >> Number;
	gautis.close();
	DQ x(Number.c_str(), 10);*/
	DQ Pi = PI(n) + PI(n - 1);
	DQ Test = 1;
	Test /= n * (n - 1);
	Pi -= Test;
	Pi /= 2;
	/*
	DQ Test2 = PI(n - 1);
	DQ Test3 = PI(n);
	Test2 -= Test3;
	Test2 *= 2;
	DQ Test4(Number.c_str(), 10);
	ofstream duotis("sk.txt");
	Pi -= Test4;
	duotis << StrNumber(Pi, 64) << endl;
	duotis.close();*/
	return Pi;
}

void Doroti()
{
	string Number;
	ifstream gautis("sk.txt");
	gautis >> Number;
	gautis.close();
	DQ x;//(Number.c_str(), 10);
	// VEIKSMAI
	x = 1;
	x /= 998001;
	string str = StrNumber(x, 1000000);
	str.insert(str.size() - 4, "998");
	DQ y(str.c_str(), 10);
	y.Bendrav();
	// --------
	ofstream duotis("sk.txt");
	duotis << StrNumber(y.skai) << ' ' << StrNumber(y.vard);
	duotis.close();
	cout << "Baigta.";
	cin.get();
}

int main()
{
	setlocale(LC_CTYPE, "");
	/*DN a = 2, b = 3;
	ofstream teiktis("sk.txt");
	DN i = 2, m = 13;
	while (i < m)
	{
		teiktis << StrNumber(i) << '\t' << StrNumber(b) << '(' << Prime(b) << ')' << endl;
		b *= a;
		a = b;
		b++;
		do
		{
			i++;
		} while (!Prime(i));
	}
	teiktis.close();*/
	Doroti();
	/*{
		DQ a = TestPi(2000);
		string str = StrNumber(a, 100);
		cout << str << endl;
	}
	DQ e = 1022;
	e /= 299792458;
	e += 1;
	e.SetPower(2);
	ofstream Duotis("sk.txt");
	Duotis << StrNumber(e, 1048576);
	cout << "Baigta." << endl;
	cin.get();*/
    return 0;
}