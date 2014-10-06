/**************************
2014 adocilesloth@gmail.com
***************************/
#include "timer.h"

using namespace std;

ending closed;

DWORD WINAPI StartTimer(LPVOID t)
{
	long int loops = *static_cast<long int*>(t);
	delete t;
	long int i = 1;

	while(!closed.state() && i < loops)
	{
		Sleep(100);
		i++;
	}

	if(!closed.state())
	{
		OBSStartStopRecording();
		//Sleep(50);
		OBSStartStopRecording();
	}
	return 0;
}

void StopTimer()
{
	closed.now();
	return;
}

void ResetTimer()
{
	closed.nolonger();
	return;
}