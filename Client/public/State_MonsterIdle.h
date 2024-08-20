#pragma once

#include "Client_Defines.h"
#include "MonsterDefines.h"
#include "State.h"

BEGIN(Engine)
class CMonster;
class CNavigation;
END

BEGIN(Client)

class CState_MonsterIdle final : public CState
{
private:
	explicit CState_MonsterIdle();
	virtual ~CState_MonsterIdle() = default;

private:
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT EnterState();
	virtual _bool	Action(_double TimeDelta);

private:
	void	LookPlayer(_double TimeDelta);
	_bool	FlyUpperPlayer(_float fHeight, _double Speed, _double TimeDelta);
	void	FlyLRMove(_double Speed, _double MaxFlyTime, _double TimeDelta);
	void	FlyFBMove(_double Speed, _double MaxFlyTime, _double TimeDelta);
	void	FlyUDMove(_double Speed, _double MaxFlyTime, _double TimeDelta);
	void	Teleport(_double TimeDelta);

private:
	CTransform*		m_pPlayerTransform   = nullptr;
	MONSTERTYPE		m_eMonsterType       = MONSTERTYPE_END;
	CNavigation*	m_pMonsterNavigation = nullptr;

	class CMonster* m_pMonster = nullptr;

	_double m_TimeAcc         = 0.0;
	_double m_TeleportTimeAcc = 0.0;

	_bool m_bIsFinishedFlyUpper = false;

public:
	static CState_MonsterIdle* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END