//---------------------------------------------------------------------------
// rtl_delay
//---------------------------------------------------------------------------

#include "hal_configall.h"
#include <tchar.h>
#include <time.h>
#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "hal_foundation.h"
#include "hal_delay.h"

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

// �߾�����ʱ�ĳ���, ����: ΢��:
// reference
// Victor Chen, CPU ����(MHz)�͸߾�����ʱ(΢�뼶)
// http://www.cppfans.com/articles/system/cpuspd_usdly.asp

void delay_us(__int64 Us)
{
	LARGE_INTEGER CurrTicks, TicksCount;

	QueryPerformanceFrequency(&TicksCount);
	QueryPerformanceCounter(&CurrTicks);

	// todo 2010.01
	// need revise
	// is it correct?
	TicksCount.QuadPart = TicksCount.QuadPart * Us / 100000064;
	TicksCount.QuadPart += CurrTicks.QuadPart;

	while(CurrTicks.QuadPart<TicksCount.QuadPart)
		QueryPerformanceCounter(&CurrTicks);
}

// reference
// Victor Chen, CPU ����(MHz)�͸߾�����ʱ(΢�뼶)
// http://www.cppfans.com/articles/system/cpuspd_usdly.asp
/*

	 ���� rdtsc ���ָ����Եõ� CPU �ڲ���ʱ����ֵ, ÿ����һ�� CPU ����, �����ʱ���ͼ�һ��
�����һ��ʱ�������� CPU ��������, CPU����Ƶ�� = ������ / ʱ��

Ϊ�˲����������̺��̴߳���, ����Ҫ������ߵ����ȼ�
���º������õ�ǰ���̺��̵߳���ߵ����ȼ���
SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

CPU ���ٳ����Դ����, �������ͨ�� CPU �� 1/16 ���ʱ���ھ��������������������Ƶ��, ��λ MHz:
*/
int cpu_frequency(void) //MHz
{/*
	LARGE_INTEGER CurrTicks, TicksCount;
	__int64 iStartCounter, iStopCounter;

    DWORD dwOldProcessP = GetPriorityClass(GetCurrentProcess());
    DWORD dwOldThreadP = GetThreadPriority(GetCurrentThread());

    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    QueryPerformanceFrequency(&TicksCount);
    QueryPerformanceCounter(&CurrTicks);

    TicksCount.QuadPart /= 16;
    TicksCount.QuadPart += CurrTicks.QuadPart;

    asm rdtsc
    asm mov DWORD PTR iStartCounter, EAX
    asm mov DWORD PTR (iStartCounter+4), EDX

    while(CurrTicks.QuadPart<TicksCount.QuadPart)
        QueryPerformanceCounter(&CurrTicks);

    asm rdtsc
    asm mov DWORD PTR iStopCounter, EAX
    asm mov DWORD PTR (iStopCounter + 4), EDX

    SetThreadPriority(GetCurrentThread(), dwOldThreadP);
    SetPriorityClass(GetCurrentProcess(), dwOldProcessP);

    return (int)((iStopCounter-iStartCounter)/62500);
    */
	// todo
	return 1;
}

// reference
// Victor Chen, CPU ����(MHz)�͸߾�����ʱ(΢�뼶)
// http://www.cppfans.com/articles/system/cpuspd_usdly.asp
// ǰ������ API ����������ʱ, ���֪���� CPU �Ĺ���Ƶ��, ����ѭ��, Ҳ���Եõ���
// ���ȵ���ʱ

int _CPU_FREQ = 0; //����һ��ȫ�ֱ������� CPU Ƶ�� (MHz)

void CpuDelayUs(__int64 Us) //����ѭ���� CPU ��Ƶ����ʱ, ����: ΢��
{
	/*
    __int64 iCounter, iStopCounter;
    asm rdtsc
    asm mov DWORD PTR iCounter, EAX
    asm mov DWORD PTR (iCounter+4), EDX

    iStopCounter = iCounter + Us*_CPU_FREQ;

	while(iStopCounter-iCounter>0)
    {
        asm rdtsc
        asm mov DWORD PTR iCounter, EAX
        asm mov DWORD PTR (iCounter+4), EDX
    }
    */
}

void TestDelay(void)
{
/*
	_CPU_FREQ = CPU_Frequency(); //���� CPU Ƶ�ʳ�ʼ����ʱ
	CpuDelayUs(1000000); //��ʱ 1 ����
	*/
}


