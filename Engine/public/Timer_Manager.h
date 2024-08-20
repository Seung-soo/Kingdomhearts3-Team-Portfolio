#pragma once

#include "Base.h"
#include "Timer.h"

BEGIN(Engine)

class CTimer_Manager final : public CBase
{
	MAKE_SINGLETON(CTimer_Manager)
private:
	CTimer_Manager();
	virtual ~CTimer_Manager() = default;
public:
	_double			Get_TimeDelta(const _tchar* pTimerTag);
public:
	HRESULT			Add_Timer(const _tchar* pTimerTag);
private:
	map<const _tchar*, CTimer*>			m_Timers;
	typedef map<const _tchar*, CTimer*>	TIMERS;
private:
	CTimer*	Find_Timer(const _tchar* pTimerTag);
public:
	virtual void	Free() override;
};

END