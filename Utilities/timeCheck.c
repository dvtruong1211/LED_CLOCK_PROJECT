#include "timeCheck.h"

/*h�m timeIsAfter return true neu elapsedTime(uint32_t newTime,uint32_t oldTime) < 100000
 * gi� tri 100000 l� du nho de biet rang gia tri newTime vua moi chay qua oldTime
 * vd: timeOffBuzzer = g_sysTime+ 100ms.
 * trong v�i ms tiep theo th� gi� tri g_sysTime(new) < timeOffBuzzer (gan ngay s�t timeOffBuzzer)
 * th�  elapsedTime( g_sysTime(newTime),timeOffBuzzer) >> 100, nhu vay l� chua chay qua,
 * gi� tri 100000 l� tuong doi, chi can >= 100ms (theo vd n�y) v� du nho de tr�nh truong hop chua chay qua */



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
