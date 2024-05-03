#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

class BigNumBase { //大数基类(只能算非负数)
public:
	vector<char> n; //存数字

	BigNumBase(); //初始化
	BigNumBase(const char* str);
	BigNumBase(string str);

	int cmp(BigNumBase &num); //比较大小（正数 为 this > num, 0 为 this = num, 负数 为 this < m

	void shl(int i); //左移（乘i个10）
	void shr(int i); //右移（除i个10）

	void add(BigNumBase num); //加法
	void sub(BigNumBase num); //减法(大减小)
	void mul(BigNumBase num); //乘法
	void div(BigNumBase num); //除法
	void mod(BigNumBase num); //取模

	string tos(); //转为string
};

struct BigNumOutputFormat { //大数输出格式
	char sign;
	int gap;
};

class BigNum : public BigNumBase { //大数类
public:
	int s; //符号(1为正，2为负)

	static BigNumOutputFormat format;
	static bool optimize; //优化（针对加减个位数，乘除10^n，模10，模1）

	BigNum(); //初始化
	BigNum(const char* str);
	BigNum(string str);
	BigNum(int num);

	void addProxy(BigNumBase &num); //加法代理
	void subProxy(BigNumBase &num); //减法代理(大减小)
	void mulProxy(BigNumBase &num); //乘法代理
	void divProxy(BigNumBase &num); //除法代理
	void modProxy(BigNumBase &num); //取模代理

	void operator=(BigNum num); //赋值

	BigNum operator+(BigNum num); //加法
	BigNum operator-(BigNum num); //减法
	BigNum operator*(BigNum num); //乘法
	BigNum operator/(BigNum num); //除法
	BigNum operator%(BigNum num); //取模
	BigNum operator<<(int i); //左移
	BigNum operator>>(int i); //右移

	void operator+=(BigNum num);
	void operator-=(BigNum num);
	void operator*=(BigNum num);
	void operator/=(BigNum num);
	void operator%=(BigNum num);
	void operator<<=(int i);
	void operator>>=(int i);

	BigNum operator++(); //前置自增
	BigNum operator--(); //前置自减

	BigNum operator++(int); //后置
	BigNum operator--(int); //后置

	bool operator>(BigNum num); //大于
	bool operator>=(BigNum num); //大于等于
	bool operator<(BigNum num); //小于
	bool operator<=(BigNum num); //小于等于
	bool operator==(BigNum num); //等于
	bool operator!=(BigNum num); //不等于

	BigNum abs(); //绝对值
	char getNum(int i); //获取某一位的数值
	int getDigits(); //获取位数
	int getSign(); //判断正负
	int is0(); //判断是否为0
	string toString(); //转为字符串（带格式）

	static void setFormat(char sign = ',', int gap = 3); //设置输出格式
	static void setOptimize(bool bl); //设置是否优化

	friend ostream& operator<<(ostream& out, BigNum num); //输出
	friend istream& operator>>(istream& out, BigNum &num); //输入

};


//实现


/*********** BigNumBase ***********/


BigNumBase::BigNumBase() {}
BigNumBase::BigNumBase(const char* str) : BigNumBase(string(str)) {}
BigNumBase::BigNumBase(string str) { //初始化
	n = vector<char>(str.size());
	for (int i = 0; i < str.size(); i++) n[i] = str[str.size() - 1 - i] - '0'; //倒着存（如 12345 在数组中存为 [5, 4, 3, 2, 1]）
}

int BigNumBase::cmp(BigNumBase& num) { //比较大小（正数 为 this > num, 0 为 this = num, 负数 为 this < m）
	//比较位数，位数多的肯定更大
	if (n.size() != num.n.size()) return n.size() - num.n.size();

	//位数相同，则从右向左遍历，直到遍历到第一个不相同的两个数
	else for (int i = n.size() - 1; i >= 0; i--) if (n[i] != num.n[i]) return n[i] - num.n[i];

	//完全相同则输出0
	return 0;
}

void BigNumBase::shl(int i) { //左移（乘i个10）
	int a = n.size();
	n.resize(n.size() + i); //预留空间
	while (a--) n[a + i] = n[a];
	while (i--) n[i] = 0;
}

void BigNumBase::shr(int i) { //右移（除i个10）
	int a = -1;
	while (++a < n.size() - i) n[a] = n[a + i];
	if (n.size() > 1) n.resize(n.size() - i); //删去空间
	else n[0] = 0; //不能再删了就为0
}

void BigNumBase::add(BigNumBase num) { //加法
	int s = max(n.size(), num.n.size());
	num.n.resize(s);
	n.resize(s + 1); //先多出一位，防止溢出

	for (int i = 0; i < s; i++) { //按照竖式加法算
		n[i] += num.n[i];
		if (n[i] >= 10) { //判断进位
			n[i + 1]++;
			n[i] -= 10;
		}
	}
	if (n[s]) s++; //判断位数

	n.resize(s); //保存位数
}

void BigNumBase::sub(BigNumBase num) { //减法(大减小)
	int s = max(n.size(), num.n.size());
	num.n.resize(s);
	n.resize(s + 1); //虽然没必要有这句，但和加法计算的代码类似比较好记

	for (int i = 0; i < s; i++) { //按竖式减法算
		n[i] -= num.n[i];
		if (n[i] < 0) { //判断退位
			n[i + 1]--;
			n[i] += 10;
		}
	}
	while (s > 1 && n[s - 1] == 0) s--; //判断位数（删除前导0, 直到有一个数不为0或只剩下一个0）

	n.resize(s); //保存位数
}

void BigNumBase::mul(BigNumBase num) { //乘法
	int rs = n.size() + num.n.size();
	vector<char> res(rs); //创建中间变量, 放结果

	for (int i = 0; i < n.size(); i++) { //按竖式乘法算，按顺序取一个数
		for (int j = 0; j < num.n.size(); j++) { //与num的每一个数相乘
			res[i + j] += n[i] * num.n[j]; //结果按照竖式乘法规律放到tmp的对应位置
			if (res[i + j] >= 10) { //判断进位
				res[i + j + 1] += res[i + j] / 10;
				res[i + j] %= 10;
			}
		}
	}
	while (rs > 1 && res[rs - 1] == 0) rs--; //判断位数
	res.resize(rs); //保存位数并复制
	n = res;
}

void BigNumBase::div(BigNumBase num) { //除法
	int rs = n.size();
	vector<char> res(rs); //商
	BigNumBase tmp("0"); //初始化中间变量

	int i = rs; //现在正在求res（商）的第i位
	while (i--) { //从高位求到低位
		if (!(tmp.n.size() == 1 && tmp.n[0] == 0)) tmp.shl(1); //tmp不为0时左移1（乘10）,防止出现前导0
		tmp.n[0] = n[i]; //加上n[i]

		while (tmp.cmp(num) >= 0) { //若 tmp >= num 则一直减, res[i]+1, 直到 tmp < num, 此时就求出商的第i位了
			tmp.sub(num);
			res[i]++;
		}
	}
	while (rs > 1 && res[rs - 1] == 0) rs--; //判断位数
	res.resize(rs); //保存位数并复制
	n = res;
}

void BigNumBase::mod(BigNumBase num) { //取模
	BigNumBase res("0"); //初始化res, 记录每次计算的余数

	int i = n.size();
	while (i--) { //从高位求到低位
		if (!(res.n.size() == 1 && res.n[0] == 0)) res.shl(1); //res不为0时左移1（乘10）,防止出现前导0
		res.n[0] = n[i]; //加上n[i]
		while (res.cmp(num) >= 0) res.sub(num); //若 res >= num 则一直减, 直到 res < num
	}
	n = res.n; //可以直接保存结果
}

string BigNumBase::tos() { //转为string
	string str;
	for (int i = n.size() - 1; i >= 0; i--) str += n[i] + '0'; //倒回来
	return str;
}


/*********** BigNum ***********/


BigNumOutputFormat BigNum::format = { ',', 0 };
bool BigNum::optimize = true; //默认打开优化

BigNum::BigNum() { //初始化
	n.push_back(0); //初始化为+0
	s = 1;
}
BigNum::BigNum(const char* str) : BigNum(string(str)) {}
BigNum::BigNum(string str) {
	if (str[0] == '-') { //判断符号
		s = -1;
		str.erase(0, 1);
	}
	else s = 1;

	int idx = 0; //删除除数字外的所有字符
	for (int i = 0; i < str.size(); i++) if (str[i] >= '0' && str[i] <= '9') str[idx++] = str[i];
	str.erase(idx);

	if (str == "") { //空字符串为0
		s = 1;
		n.push_back(0);
		return;
	}

	idx = 0; //删除前导0
	while (idx < str.size() - 1 && str[idx] == '0') idx++;
	str.erase(0, idx);

	n = vector<char>(str.size());
	for (int i = 0; i < str.size(); i++) n[i] = str[str.size() - 1 - i] - '0'; //倒着存
}
BigNum::BigNum(int num) {
	if (num == 0) { //等于0直接走
		n.push_back(0);
		s = 1;
		return;
	}

	if (num < 0) { //判断符号
		s = -1;
		num *= -1;
	}
	else s = 1;

	while (num > 0) { //输入到数组
		n.push_back(num % 10);
		num /= 10;
	}
}

void BigNum::addProxy(BigNumBase& num) { //加法代理
	if (optimize && num.n.size() == 1) { //打开优化，且加个位数
		if (num.n[0] == 0) return; //加0等于什么都没加

		int i = 1;
		n[0] += num.n[0]; //把这个数加上去
		while (i < n.size() && n[i - 1] >= 10) { //进位
			n[i - 1] -= 10;
			n[i] += 1;
			i++;
		}
		if (n[n.size() - 1] >= 10) { //增加空间
			n[n.size() - 1] -= 10;
			n.push_back(1);
		}
		return;
	}

	add(num); //没有进行优化则正常执行
}
void BigNum::subProxy(BigNumBase& num) { //减法代理(大减小)
	if (optimize && num.n.size() == 1) { //打开优化，且加个位数
		if (num.n[0] == 0) return; //减0等于什么都没减

		int i = 1;
		n[0] -= num.n[0]; //把这个数减掉
		while (i < n.size() && n[i - 1] < 0) { //退位
			n[i - 1] += 10;
			n[i] -= 1;
			i++;
		}
		if (n[n.size() - 1] == 0 && n.size() > 1) { //压缩空间
			n.resize(n.size() - 1);
		}
		return;
	}

	sub(num); //没有进行优化则正常执行
}
void BigNum::mulProxy(BigNumBase& num) { //乘法代理
	if (optimize) {
		if (num.n.size() == 1 && num.n[0] == 1) return; //乘1相当于没乘
		if (num.n.size() > 1 && num.n[0] == 0 && num.n[num.n.size() - 1] == 1) {
			int i = 1;
			while (num.n[i] == 0) i++;
			if (i == num.n.size() - 1) { //确实是10^i(有i个0)
				shl(i); //直接左移
				return;
			}
		}
	}

	mul(num); //没有进行优化则正常执行
}
void BigNum::divProxy(BigNumBase& num) { //除法代理
	if (optimize) {
		if (num.n.size() == 1 && num.n[0] == 1) return; //除1相当于没除
		if (num.n.size() > 1 && num.n[0] == 0 && num.n[num.n.size() - 1] == 1) {
			int i = 1;
			while (num.n[i] == 0) i++;
			if (i == num.n.size() - 1) { //确实是10^i(有i个0)
				shr(i); //直接右移
				return;
			}
		}
	}

	div(num); //没有进行优化则正常执行
}
void BigNum::modProxy(BigNumBase& num) { //取模代理
	if (optimize) {
		if (num.n.size() == 1 && num.n[0] == 1) { //模1必为0
			n.resize(1);
			n[0] = 0;
			return;
		}
		if (num.n.size() == 2 && num.n[0] == 0 && num.n[1] == 1) { //num==10
			n.resize(1); //只保留个位数
			return;
		}
	}

	mod(num); //没有进行优化则正常执行
}

void BigNum::operator=(BigNum num) { //赋值
	n = num.n;
	s = num.s;
}

BigNum BigNum::operator+(BigNum num) { //加法
	BigNum tmp = *this;
	tmp += num;
	return tmp;
}
BigNum BigNum::operator-(BigNum num) { //减法
	BigNum tmp = *this;
	tmp -= num;
	return tmp;
}
BigNum BigNum::operator*(BigNum num) { //乘法
	BigNum tmp = *this;
	tmp *= num;
	return tmp;
}
BigNum BigNum::operator/(BigNum num) { //除法
	BigNum tmp = *this;
	tmp /= num;
	return tmp;
}
BigNum BigNum::operator%(BigNum num) { //取模
	BigNum tmp = *this;
	tmp %= num;
	return tmp;
}
BigNum BigNum::operator<<(int i) { //左移
	BigNum tmp = *this;
	tmp <<= i;
	return tmp;
}
BigNum BigNum::operator>>(int i) { //右移
	BigNum tmp = *this;
	tmp >>= i;
	return tmp;
}

void BigNum::operator+=(BigNum num) {
	if (s == num.s) addProxy(num); //同号情况直接加
	else { //异号情况就乘个负一变成同号算减法，算完后再乘个负一还原，乘两个负一相当于没乘
		int res = cmp(num);
		if (res == 0) *this = 0; //相等为0
		else if (res > 0) { //大减小
			subProxy(num);
		}
		else { //小减大
			num.subProxy(*this);
			*this = num;
		}
	}
}
void BigNum::operator-=(BigNum num) {
	num.s *= -1; //负号放进去，算加法
	operator+=(num);
}
void BigNum::operator*=(BigNum num) {
	s *= num.s; //符号相乘
	if ((n.size() == 1 && n[0] == 0) || (n.size() == 1 && n[0] == 0)) *this = 0; //任何数乘0都等于0
	else mulProxy(num); //数字相乘
}
void BigNum::operator/=(BigNum num) {
	s *= num.s; //符号相乘
	if (num.n.size() == 1 && num.n[0] == 0) throw "不能除0"; //任何数都不能除0
	if (n.size() == 1 && n[0] == 0) *this = 0; //0除任何数都为0
	else divProxy(num); //数字相除
}
void BigNum::operator%=(BigNum num) {
	s *= num.s; //符号相乘
	if (num.n.size() == 1 && num.n[0] == 0) throw "不能模0"; //任何数都不能模0
	else {
		modProxy(num); //数字取模
		if (n.size() == 1 && n[0] == 0) s = 1; //取完模为0则符号改为正
	}
}
void BigNum::operator<<=(int i) {
	if (i > 0) shl(i); //左移
	else if (i < 0) shr(-i); //i为负数相当于右移
}
void BigNum::operator>>=(int i) {
	if (i > 0) shr(i); //右移
	else if (i < 0) shl(-i); //i为负数相当于左移
}

BigNum BigNum::operator++() { //前置自增
	operator+=(1);
	return *this;
}
BigNum BigNum::operator--() { //前置自减
	operator-=(1);
	return *this;
}

BigNum BigNum::operator++(int) { //后置自增
	BigNum tmp = *this;
	operator+=(1);
	return tmp;
}
BigNum BigNum::operator--(int) { //后置自减
	BigNum tmp = *this;
	operator-=(1);
	return tmp;
}

bool BigNum::operator>(BigNum num) { //大于
	if (s > num.s) return true;
	if (s < num.s) return false;
	if (s == 1) return cmp(num) > 0;
	return !(cmp(num) > 0);
}
bool BigNum::operator>=(BigNum num) { //大于等于
	if (s > num.s) return true;
	if (s < num.s) return false;
	if (s == 1) return cmp(num) >= 0;
	return !(cmp(num) >= 0);
}
bool BigNum::operator<(BigNum num) { //小于
	if (s < num.s) return true;
	if (s > num.s) return false;
	if (s == 1) return cmp(num) < 0;
	return !(cmp(num) < 0);
}
bool BigNum::operator<=(BigNum num) { //小于等于
	if (s < num.s) return true;
	if (s > num.s) return false;
	if (s == 1) return cmp(num) <= 0;
	return !(cmp(num) <= 0);
}
bool BigNum::operator==(BigNum num) { //等于
	if (s != num.s) return false;
	return cmp(num) == 0;
}
bool BigNum::operator!=(BigNum num) { //不等于
	if (s != num.s) return true;
	return cmp(num) != 0;
}

BigNum BigNum::abs() { //绝对值
	if (s == 1) return *this;
	BigNum res = *this;
	res.s = 1;
	return res;
}
char BigNum::getNum(int i) { //获取某一位的数值
	return n[i + 1];
}
int BigNum::getDigits() { //获取位数
	return n.size();
}
int BigNum::getSign() { //判断正负
	return s;
}
int BigNum::is0() { //判断是否为0
	return (n.size() == 1 && n[0] == 0);
}

void BigNum::setFormat(char sign, int gap) { //设置输出格式
	format.sign = sign;
	format.gap = gap;
}

void BigNum::setOptimize(bool bl) { //设置是否优化
	optimize = bl;
}

string BigNum::toString() { //转为字符串（带格式）
	string str;
	if (s == -1) str += '-'; //输出符号

	for (int i = n.size() - 1; i >= 0; i--) {
		str += n[i] + '0'; //倒回来
		if (i != 0 && format.gap != 0 && i % format.gap == 0) str += format.sign; //判断是否需要加间隔符
	}
	return str;
}

ostream& operator<<(ostream& out, BigNum num) { //输出
	out << num.toString(); //输出
	return out;
}
istream& operator>>(istream& in, BigNum& num) { //输入
	string str;
	in >> str;
	num = BigNum(str);
	return in;
}

