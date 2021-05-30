#include "time.hpp"

#include <windows.h>

#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */

namespace basic
{

double get_milliseconds()
{
	LARGE_INTEGER frequency; 
	LARGE_INTEGER t1;

	double elapsedTime;
	QueryPerformanceFrequency(&frequency);

	QueryPerformanceCounter(&t1);

    elapsedTime = t1.QuadPart * 1000 / frequency.QuadPart; 

	return elapsedTime;
}

}
