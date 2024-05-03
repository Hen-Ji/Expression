#pragma once
#include <string>
#include <vector>
#include <stack>
#include <iostream>
#include <algorithm>
#include <sstream>

using namespace std;

template <class T = int> class Expression //表达式
{
public:
	static vector<pair<string, bool>> parse(string str); //中缀解析为后缀
	static T calculate(string str); //计算后缀表达式
};

template <class T> vector<pair<string, bool>> Expression<T>::parse(string str) {
	int idx = 0; //删去空格
	for (int i = 0; i < str.size(); i++) {
		if (str[i] != ' ') str[idx++] = str[i];
	}
	str.resize(idx);

	vector<pair<string, bool>> arr; //分离数字与运算符
	char ope[6] = { '+', '-', '*', '/', '(', ')' }; //运算符
	string opeStr[6] = { "+", "-", "*", "/", "(", ")" }; //运算符
	string tmp;
	for (int i = 0; i < str.size(); i++) {
		tmp += str[i];
		for (int j = 0; j < 6; j++) {
			if (str[i] == ope[j]) { //是运算符
				if (
					str[i] == '-'
					&& (i == 0 || !((str[i - 1] >= '0' && str[i - 1] <= '9') || str[i - 1] == ')'))
					&& (str[i + 1] >= '0' && str[i + 1] <= '9')
					) break; //如果是负号 且 出现在str[0]或负号前不是数字/右括号 且 负号后是数字，则这个负号应是表示后面的数字是个负数，不处理

				if (tmp.size() > 1) arr.push_back({ tmp.substr(0, tmp.size() - 1), false }); //放数字
				arr.push_back({ opeStr[j], true }); //放运算符
				tmp = ""; //清空
				break;
			}
		}
	}
	if (tmp.size() > 0) arr.push_back({ tmp, false }); //放最后一个数字

	vector<pair<string, bool>> res; //开始转为后缀表达式
	stack<string> stk;

	idx = 0;
	for (auto& [word, is] : arr) {
		if (!is) { //是数字则直接进入res
			res.push_back({ word , false });
		}
		else { //是运算符
			if (stk.empty()) { //栈为空则直接入栈
				stk.push(word);
			}
			else if (word == "(") { //左括号直接进栈
				stk.push(word);
			}
			else if (word == ")") { //stk一直出栈，直到找到左括号
				while (stk.top() != "(") {
					res.push_back({ stk.top() , true });
					stk.pop();
				}
				stk.pop(); //左括号出栈
			}
			else if (word == "+" || word == "-") {
				if (stk.top() == "(") { //是左括号可以直接进栈
					stk.push(word);
				}
				else { //不是的话就要一直出栈，直到栈空或者碰到左括号
					while (!stk.empty() && stk.top() != "(") {
						res.push_back({ stk.top() , true });
						stk.pop();
					}
					stk.push(word); //运算符进栈
				}
			}
			else if (word == "*" || word == "/") {
				if (stk.top() == "(" || stk.top() == "+" || stk.top() == "-") { //是左括号或者加减号可以直接进栈
					stk.push(word);
				}
				else { //不是的话就要一直出栈，直到栈空或者碰到左括号或者加减号
					while (!stk.empty() && stk.top() != "(" && stk.top() != "+" && stk.top() != "-") {
						res.push_back({ stk.top() , true });
						stk.pop();
					}
					stk.push(word); //运算符进栈
				}
			}
		}
	}
	while (!stk.empty()) { //剩余运算符全部出栈（由于左括号和右括号是成对出现的，上面的过程执行完后stk里面不会再有左括号）
		res.push_back({ stk.top() , true });
		stk.pop();
	}

	return res;
}

template <class T> T Expression<T>::calculate(string str) {
	vector<pair<string, bool>> exp = Expression<T>::parse(str); //解析字符串

	stack<T> res; //开始计算
	for (auto& [word, is] : exp) {
		if (!is) { //是数字
			T tmp; //字符串转为T类型后放进去
			stringstream ss;
			ss << word;
			ss >> tmp;
			res.push(tmp);
		}
		else { //是运算符
			T a, b; //取出栈顶两个数
			a = res.top();
			res.pop();
			b = res.top();
			res.pop();

			if (word == "+") b += a; //b和a不能调换
			else if (word == "-") b -= a;
			else if (word == "*") b *= a;
			else if (word == "/") b /= a;

			res.push(b); //算完后再放进去
		}
	}

	return res.top(); //最后在栈里的就是结果，返回
}
