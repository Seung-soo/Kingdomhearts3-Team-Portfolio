#pragma once

#include "Client_Defines.h"
#include "MonsterDefines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CMonster;
END

BEGIN(Client)

class CState_MonsterAtk final : public CState
{
private:
	explicit CState_MonsterAtk();
	virtual ~CState_MonsterAtk() = default;

private:
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT EnterState();
	virtual HRESULT ExitState();
	virtual _bool	Action(_double TimeDelta);

private:
	void	JustLook(_double TimeDelta); // Player 쪽으로 회전
	_bool	AtkAcc(_uint MaxAtkNum, _double TimeDelta); // 공격 애니메이션 횟수 누적
	_bool	Chase(_float fDistDiff, _double TimeDelta); // Player 쳐다보면서 직선으로 쫓아옴

	// 몬스터별 공격처리
	void	CrowSoldierAtk(_double TimeDelta);
	_bool	BowManAtk(_double TimeDelta);
	_bool	CannonAtk(_double TimeDelta);
	_bool	MonkeyAtk(_double TimeDelta);
	_bool	MowerAtk(_double TimeDelta);
	_bool	NightmareAtk(_double TimeDelta);
	_bool	ShiningEggAtk(_double TimeDelta);
	_bool	NobodyAtk(_double TimeDelta);
	_bool	ThinSoldierAtk(_double TimeDelta);

private:
	CTransform*		m_pPlayerTransform   = nullptr;
	MONSTERTYPE		m_eMonsterType       = MONSTERTYPE_END;
	CNavigation*	m_pMonsterNavigation = nullptr;

	class CMonster* m_pMonster = nullptr;

	_double m_TimeAcc = 0.0;
	_uint	m_iAtkNum = 0;

	_uint m_iNextAnimation = 0;

	_bool m_bIsSkillEnd[5] = { false, };
	_bool m_bChaseEnd      = false;
	_bool m_bEffectEnd[6]  = { false, };

	_bool m_bSoundEnd = false;

public:
	static CState_MonsterAtk* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END