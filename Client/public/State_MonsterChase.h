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

class CState_MonsterChase final : public CState
{
private:
	explicit CState_MonsterChase();
	virtual ~CState_MonsterChase() = default;

private:
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT EnterState();
	virtual _bool	Action(_double TimeDelta);

private:
	_bool Chase(_float fDistDiff, _double TimeDelta, CNavigation* pNavigation = nullptr); // Player 쳐다보면서 fDistDiff 만큼 직선으로 쫓아옴
	_bool CrowSoldierChase(_float fDistDiff, _double TimeDelta);

private:
	CTransform*		m_pPlayerTransform   = nullptr;
	MONSTERTYPE		m_eMonsterType       = MONSTERTYPE_END;
	CNavigation*	m_pMonsterNavigation = nullptr;

	class CMonster* m_pMonster = nullptr;

	_double m_TimeAcc        = 0.0;
	_double m_EffectInterval = 0.0;
	_uint	m_iEffectCount   = 0;
	_bool	m_bEffectEnd     = false;
	_double m_RandInterval   = 0.0;

	_bool m_bSoundEnd = false;

public:
	static CState_MonsterChase* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END