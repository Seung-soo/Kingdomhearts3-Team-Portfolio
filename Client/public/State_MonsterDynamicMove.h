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

class CState_MonsterDynamicMove final : public CState
{
private:
	explicit CState_MonsterDynamicMove();
	virtual ~CState_MonsterDynamicMove() = default;

private:
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT EnterState();
	virtual _bool	Action(_double TimeDelta);

private:
	_bool TurnRound(_double MaxTime, _double TimeDelta); // 플레이어를 쳐다보고 빙글빙글 돎
	_bool TurnRoundAndBack(_double Speed, _double fDistDiff, _double TimeAcc, _double TimeDelta); // 플레이어를 쳐다보고 돌면서 fDistDiff 만큼 멀어짐
	_bool BigTreeDynamicMove(_double TimeAcc, _double TimeDelta);
	_bool GoatDynamicMove(_double TiemDelta);

private:
	CTransform*		m_pPlayerTransform   = nullptr;
	MONSTERTYPE		m_eMonsterType       = MONSTERTYPE_END;
	CNavigation*	m_pMonsterNavigation = nullptr;

	class CTerrain* m_pRapunzelBossTerrain = nullptr;
	class CMonster* m_pMonster             = nullptr;

	_uint	m_iNextAnimation = 0;
	_uint	m_iRandNum       = 0;
	_double m_TimeAcc        = 0.0;

	_double m_EffectInterval = 0.0;
	_bool	m_bEffectEnd[2]  = { false, };
	_double m_RandInterval   = 0.0;

	_uint	m_iTeleportAcc = 0;

public:
	static CState_MonsterDynamicMove* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END