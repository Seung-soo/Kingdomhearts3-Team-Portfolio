#include "..\public\Timer_Manager.h"

READY_SINGLETON(CTimer_Manager)

CTimer_Manager::CTimer_Manager()
{
}

_double CTimer_Manager::Get_TimeDelta(const _tchar * pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);

	if (nullptr == pTimer)
		return 0.0;

	return pTimer->Get_TimeDelta();
}

HRESULT CTimer_Manager::Add_Timer(const _tchar * pTimerTag)
{
	// 타이머를 추가하려는데 이미 태그가 있으면 추가X
	if (nullptr != Find_Timer(pTimerTag))
	{
		MSGBOX("Timer already exists and cannot be added.");
		return E_FAIL;
	}

	// 동적 할당이 되지 않았으면 추가X
	CTimer* pTimer = CTimer::Create();
	if (nullptr == pTimer)
	{
		MSGBOX("Timer is nullptr and cannot be added.");
		return E_FAIL;
	}

	m_Timers.emplace(pTimerTag, pTimer);

	return S_OK;
}

CTimer * CTimer_Manager::Find_Timer(const _tchar * pTimerTag)
{
	auto iter = find_if(m_Timers.begin(), m_Timers.end(), CTagFinder(pTimerTag));

	if (iter == m_Timers.end())
		return nullptr;

	return iter->second;
}

void CTimer_Manager::Free()
{
	for (auto& Pair : m_Timers)
		Safe_Release(Pair.second);

	m_Timers.clear();
}
