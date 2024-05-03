#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>

using namespace std;

template <class T = int> class FractionBase //分数基类（T 可以是 char, int ,long long, 或者自定义类（如大数等类，需要重载=,+,-,*,/,%,>,>=,<,<=,==,!=运算符以及<<输出流运算符）,并支持类似 T num = 1; 的写法）
{
public:
	T n, d; //分子，分母
	static bool autoRed; //自动约分

	FractionBase(); //初始化
	FractionBase(T num);
	FractionBase(T num, T den);

	T gcd(T a, T b); //求最大公约数
	void red(); //约分

	void add(FractionBase<T> num); //加法
	void sub(FractionBase<T> num); //减法
	void mul(FractionBase<T> num); //乘法
	void div(FractionBase<T> num); //除法

	T cmp(FractionBase<T> num); //比较大小（正数 为 this > num, 0 为 this = num, 负数 为 this < num）
	static void setAutoRed(bool bl); //设置自动约分
};

enum class FractionStyle {
	fractional = 0, //分数形式（分子分母都在一行）
	threeLines = 1, //分数形式（有三行，第一行为分子，第二行为分数符号，第三行为分母）
	fixed = 2, //小数形式（固定小数位数）
	scientific = 3, //小数形式（使用科学计数法，显示有效数字）
};
struct FractionOutputFormat { //分数输出格式
	FractionStyle style; //格式
	int precision; //精度
};

template <class T = int> class Fraction : public FractionBase<T> //分数类
{
public:
	static FractionOutputFormat format; //输出格式

	Fraction(); //初始化
	Fraction(T num);
	Fraction(T num, T den);
	Fraction(string str);

	T ton(string str);

	void operator=(Fraction<T> num); //赋值

	Fraction<T> operator+(Fraction<T> num); //加法
	Fraction<T> operator-(Fraction<T> num); //减法
	Fraction<T> operator*(Fraction<T> num); //乘法
	Fraction<T> operator/(Fraction<T> num); //除法

	void operator+=(Fraction<T> num);
	void operator-=(Fraction<T> num);
	void operator*=(Fraction<T> num);
	void operator/=(Fraction<T> num);

	bool operator>(Fraction<T> num); //大于
	bool operator>=(Fraction<T> num); //大于等于
	bool operator<(Fraction<T> num); //小于
	bool operator<=(Fraction<T> num); //小于等于
	bool operator==(Fraction<T> num); //等于
	bool operator!=(Fraction<T> num); //不等于

	static void setFormat(FractionStyle style = FractionStyle::fractional, int precision = 8); //设置输出格式
};


//模板类只能在头文件中定义，在cpp文件中定义会提示无法解析的外部符号


/*********** FractionBase ***********/


template <class T> bool FractionBase<T>::autoRed = true; //默认开启自动约分

template <class T> FractionBase<T>::FractionBase() { //初始化
	n = 0; //初始化为0
	d = 1;
}

template <class T> FractionBase<T>::FractionBase(T num) {
	this->n = num;
	this->d = 1;
}

template <class T> FractionBase<T>::FractionBase(T num, T den) {
	if (den == 0) throw "分母不能为0"; //分母不能为0

	this->n = num;
	this->d = den;

	if (autoRed) red(); //约分
}

template <class T> T FractionBase<T>::gcd(T a, T b) { //求最大公约数

	//辗转相除法：
	//假设有两个数 d, n, 且 d > n, 其中 d = a * b, n = a * c, a 为 d 和 n 的最大公约数。
	//设 t = d % n, 则 t = a * (b % c), t 和 n 的最大公约数仍为 a, 而 n > t。
	//令 d = n, n = t, 则又有 d > n, 且 a 仍为 d 和 n 的最大公约数。
	//如此往复计算，d和n会越来越小，但 a 仍然是其最大公约数，直到 d % n = 0。
	//只有当满足 b 为 c 的倍数时，才能满足 d % n = 0, 又因为若 c != 1, 则 d 和 n 的最大公约数为 a * c, 与假设矛盾
	//故此时 c 必为 1, n 必为 a, 故此时的 n 为最开始的两数的最大公约数

	T t;
	while (a % b != 0) { //辗转相除法（b > a 时对结果无影响，不用排除）
		t = a % b;
		a = b;
		b = t;
	}

	return b;
}

template <class T> void FractionBase<T>::red() { //约分
	if (n == 0) { //分子为0则直接将分母变为1
		d = 1;
		return;
	}

	T t = gcd(d, n); //约分数字
	n /= t;
	d /= t;

	if (d < 0) { //约分符号, 分母不能是负数
		d *= -1;
		n *= -1;
	}
}

template <class T> void FractionBase<T>::add(FractionBase<T> num) { //加法
	n = n * num.d + num.n * d; //通分后相加
	d *= num.d;
	if (autoRed) red(); //约分
}

template <class T> void FractionBase<T>::sub(FractionBase<T> num) { //减法
	n = n * num.d - num.n * d; //通分后相减
	d *= num.d;
	if (autoRed) red(); //约分
}

template <class T> void FractionBase<T>::mul(FractionBase<T> num) { //乘法
	n *= num.n; //分子分母分别相乘
	d *= num.d;
	if (autoRed) red(); //约分
}

template <class T> void FractionBase<T>::div(FractionBase<T> num) { //除法
	n *= num.d; //相当于分子乘分母，分母乘分子
	d *= num.n;
	if (autoRed) red(); //约分
}
template <class T> T FractionBase<T>::cmp(FractionBase<T> num) { //比较大小
	T tn = n * num.d; //分子通分即可
	num.n *= d;
	return tn - num.n; //正数 为 this > num, 0 为 this = num, 负数 为 this < num
}

template <class T> void FractionBase<T>::setAutoRed(bool bl) { //设置自动约分
	autoRed = bl;
}

template <class T> ostream& operator<<(ostream& out, FractionBase<T> num) { //输出
	out << num.n << '/' << num.d;
	return out;
}


/*********** Fraction ***********/


template <class T> FractionOutputFormat Fraction<T>::format = { FractionStyle::fractional, 8 }; //默认使用一行的分数形式，若使用的是小数形式，则精度默认为8位

template <class T> Fraction<T>::Fraction() : FractionBase<T>::FractionBase() {} //使用基类的构造函数
template <class T> Fraction<T>::Fraction(T num) : FractionBase<T>::FractionBase(num) {}
template <class T> Fraction<T>::Fraction(T num, T den) : FractionBase<T>::FractionBase(num, den) {}
template <class T> Fraction<T>::Fraction(string str) {
	int idx = 0; //删除多余的空格
	for (int i = 0; i < str.size(); i++) {
		if (str[i] != ' ') str[idx++] = str[i];
	}
	str.resize(idx);

	T ta, tb; //字符串转为T类型
	string ds = "1";
	stringstream sa, sb;

	idx = str.find('.');
	if (idx != -1) { //有小数点
		ds += string(str.size() - idx - 1, '0'); //分母加上对应的0
		str.erase(idx, 1); //去掉小数点, 作分子
	}

	sa << str; //转T类型
	sa >> ta;
	sb << ds;
	sb >> tb;

	this->n = ta;
	this->d = tb;
}

template <class T> T Fraction<T>::ton(string str) { //字符串转数字
}


template <class T> void Fraction<T>::operator=(Fraction<T> num) { //赋值
	this->n = num.n;
	this->d = num.d;
}

template <class T> Fraction<T> Fraction<T>::operator+(Fraction<T> num) { //加法
	Fraction<T> t = *this;
	t.add(num);
	return t;
}
template <class T> Fraction<T> Fraction<T>::operator-(Fraction<T> num) { //减法
	Fraction<T> t = *this;
	t.sub(num);
	return t;
}
template <class T> Fraction<T> Fraction<T>::operator*(Fraction<T> num) { //乘法
	Fraction<T> t = *this;
	t.mul(num);
	return t;
}
template <class T> Fraction<T> Fraction<T>::operator/(Fraction<T> num) { //除法
	Fraction<T> t = *this;
	t.div(num);
	return t;
}

template <class T> void Fraction<T>::operator+=(Fraction<T> num) {
	this->add(num); //不懂为什么要加 this->
}
template <class T> void Fraction<T>::operator-=(Fraction<T> num) {
	this->sub(num);
}
template <class T> void Fraction<T>::operator*=(Fraction<T> num) {
	this->mul(num);
}
template <class T> void Fraction<T>::operator/=(Fraction<T> num) {
	this->div(num);
}

template <class T> bool Fraction<T>::operator>(Fraction<T> num) { //大于
	return this->cmp(num) > 0;
}
template <class T> bool Fraction<T>::operator>=(Fraction<T> num) { //大于等于
	return this->cmp(num) >= 0;
}
template <class T> bool Fraction<T>::operator<(Fraction<T> num) { //小于
	return this->cmp(num) < 0;
}
template <class T> bool Fraction<T>::operator<=(Fraction<T> num) { //小于等于
	return this->cmp(num) <= 0;
}
template <class T> bool Fraction<T>::operator==(Fraction<T> num) { //等于
	return this->cmp(num) == 0;
}
template <class T> bool Fraction<T>::operator!=(Fraction<T> num) { //不等于
	return this->cmp(num) != 0;
}

template <class T> void Fraction<T>::setFormat(FractionStyle style, int precision) { //设置输出格式
	format.style = style;
	format.precision = precision;
}

template <class T> ostream& operator<<(ostream& out, Fraction<T> num) { //输出
	if (num.n == 0) { //分子为0就直接输出0
		out << num.n;
	}
	else if (Fraction<T>::format.style == FractionStyle::fractional) { //一行的分数形式
		out << num.n << '/' << num.d;
	}
	else if (Fraction<T>::format.style == FractionStyle::threeLines) { //三行的分数形式
		stringstream oa; //获取符号以及n的绝对值
		oa << num.n;
		string a = oa.str();
		int sign = 1;
		if (a[0] == '-') {
			sign = -1;
			a.erase(0, 1);
		}

		if(sign == 1) out << endl << a << endl << "---" << endl << num.d << endl; //正数
		else out << endl << "  " << a << endl << "- ---" << endl << "  " << num.d << endl; //负数
	}
	else if (Fraction<T>::format.style == FractionStyle::fixed) { //固定小数位数 (保留 precision 位小数)
		int precision = Fraction<T>::format.precision; //分子需要加 precision+1 个0（这里多加了一个0方便四舍五入）
		T number = 1;
		for (int i = 0; i <= precision; i++) {
			number *= 10;
		}
		number *= num.n;

		number /= num.d; //分子除分母
		if(number >= 0) number = (number + 5) / 10; //四舍五入（1.49 + 0.5 = 1.99，取整数部分则为1；1.50 + 0.5 = 2.00，取整数部分则为2；由此可实现四舍五入）
		else number = (number - 5) / 10;

		stringstream oa; //获取字符串
		oa << number;
		string a = oa.str();
		if (precision > 0) a.insert(a.size() - precision, "."); //插入小数点(precision为0表示只要整数，因此不需要小数点)

		out << a; //输出
	}
	else if (Fraction<T>::format.style == FractionStyle::scientific) { //使用科学计数法 (保留 precision 位有效数字)
		int precision = Fraction<T>::format.precision;

		stringstream oa, ob; //获取分子分母绝对值的字符串以及整个式子的符号
		oa << num.n;
		ob << num.d;
		string a = oa.str(), b = ob.str();
		int sign = 1;
		if (a[0] == '-') {
			sign = -1;
			a.erase(0, 1);
		}

		T number = sign; //符号为正
		number *= num.n;
		int k;
		if (a < b) k = precision + b.size() - a.size(); //若 a<b(在这里比较的是字典序), 则将a(分子)的长度变得和b(分母)一样（a 加上 b.size() - a.size() 个0）后，a/b = 0.几，因此a需要再乘precision个0。a>=b 时可以少乘个0。又因为需要四舍五入，还有再加一个0
		else k = precision + b.size() - a.size() -1;
		if (k >= 0) for (int i = 0; i <= k; i++) {
			number *= 10;
		}
		else for (int i = 0; i < -k - 1; i++) {
			number /= 10;
		}

		k = -(k-(precision-1)); //算指数
		number /= num.d; //分子除分母
		number = (number + 5) / 10; //四舍五入

		stringstream oc; //获取字符串
		oc << number;
		string c = oc.str();
		if (c.size() != precision) { //四舍五入后数字多了一个，则删去最末尾一个数，指数加1
			c.erase(c.size() - 1);
			k++;
		}
		if (precision > 1) c.insert(1, "."); //插入小数点(precision为1表示只要整数，因此不需要小数点)

		out << (sign == -1 ? "-" : "") << c << "e" << k; //输出符号，数字和指数
	}
	return out;
}

template <class T> istream& operator>>(istream& in, Fraction<T>& num) { //输入
	string str;
	in >> str;
	num = Fraction<T>(str);
	return in;
}
