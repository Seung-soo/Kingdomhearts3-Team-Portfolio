#include "..\public\Timer.h"

CTimer::CTimer()
	: m_dTimeDelta(0.0)
{
	QueryPerformanceCounter(&m_CurrentTime);
	QueryPerformanceCounter(&m_OldTime);
	QueryPerformanceCounter(&m_OriginTime);
	QueryPerformanceFrequency(&m_CpuTick);
}

_double CTimer::Get_TimeDelta()
{
	QueryPerformanceCounter(&m_CurrentTime);

	if (m_CurrentTime.QuadPart - m_OriginTime.QuadPart > m_CpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CpuTick);
		m_OriginTime = m_CurrentTime;
	}

	m_dTimeDelta = _double(m_CurrentTime.QuadPart - m_OldTime.QuadPart) / m_CpuTick.QuadPart;

	m_OldTime = m_CurrentTime;

	return m_dTimeDelta;
}

CTimer * CTimer::Create()
{
	CTimer* pInstance = new CTimer();

	return pInstance;
}

void CTimer::Free()
{
}