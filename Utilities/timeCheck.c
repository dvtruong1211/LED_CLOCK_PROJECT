#include "timeCheck.h"

/*hàm timeIsAfter return true neu elapsedTime(uint32_t newTime,uint32_t oldTime) < 100000
 * giá tri 100000 là du nho de biet rang gia tri newTime vua moi chay qua oldTime
 * vd: timeOffBuzzer = g_sysTime+ 100ms.
 * trong vài ms tiep theo thì giá tri g_sysTime(new) < timeOffBuzzer (gan ngay sát timeOffBuzzer)
 * thì  elapsedTime( g_sysTime(newTime),timeOffBuzzer) >> 100, nhu vay là chua chay qua,
 * giá tri 100000 là tuong doi, chi can >= 100ms (theo vd này) và du nho de tránh truong hop chua chay qua */



uint32_t elapsedTime(uint32_t newTime,uint32_t oldTime)
{
	if(newTime >= oldTime)
	{
		return (newTime - oldTime);
	}else
	{
		return (newTime + (0xffffffff - oldTime)+1);
	}
}

bool timeIsAfter(uint32_t newTime,uint32_t oldTime)
{
	return (elapsedTime(newTime , oldTime) < 100000)? true:false;
}
