#include <iostream>
#include "Timer.h"
#include "BigNum.h"
#include "Fraction.h"
#include "Expression.h"

#define N Fraction<BigNum>
#define E Expression<N>::calculate

using namespace std;

int main() {

	Timer timer;
	timer.begin();

	N res = E("-1 + 3 * (4 + 5 / 2) + 0.12"); //输入表达式：-1 + 3 * (4 + 5 / 2) + 0.12

	N::setFormat(FractionStyle::fractional); //输出一行分数
	cout << res << endl; //输出：931/50

	N::setFormat(FractionStyle::threeLines); //分数分成三行输出
	cout << res << endl; //第一行输出分子。第二行输出分数符号，如果是负数，还会输出负号。第三行输出分母

	N::setFormat(FractionStyle::fixed, 8); //保留8位小数
	cout << res << endl << endl; //输出：18.62000000

	N::setFormat(FractionStyle::scientific, 8); //使用科学计数法，并保留8位有效数字
	cout << res << endl; //输出：1.8620000e1

	cout << endl << "Time: " << timer.delta() << " s" << endl; //输出耗时
	return 0;
}