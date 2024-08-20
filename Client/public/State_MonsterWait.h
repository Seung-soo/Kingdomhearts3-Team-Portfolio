#pragma once

#include "Client_Defines.h"
#include "MonsterDefines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CMonster;
class CNavigation;
END

BEGIN(Client)

class CState_MonsterWait final : public CState
{
private:
	explicit CState_MonsterWait();
	virtual ~CState_MonsterWait() = default;

private:
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT EnterState();
	virtual _bool	Action(_double TimeDelta);

private:
	_bool WaitTime(_double WaitTime, _double TimeDelta);

private:
	CTransform*		m_pPlayerTransform    = nullptr;
	MONSTERTYPE		m_eMonsterType        = MONSTERTYPE_END;
	CNavigation*	m_pMonsterNavigation  = nullptr;

	class CMonster* m_pMonster = nullptr;

	_double m_TimeAcc = 0.0;

public:
	static CState_MonsterWait* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END