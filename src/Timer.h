#pragma once
#include <chrono> //获取时间的头文件
using namespace std;

class Timer //计时器类
{
private:
	chrono::steady_clock::time_point timerStart;

public:
	void begin()
	{
		timerStart = chrono::steady_clock::now(); //记录开始时间
	}
	double delta()
	{
		auto timeNow = chrono::steady_clock::now(); //记录现在时间
		return chrono::duration<double>(timeNow - timerStart).count(); //计算时间间隔(纳秒级)
	}
};

