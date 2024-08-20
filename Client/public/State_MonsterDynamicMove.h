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
	_bool TurnRound(_double MaxTime, _double TimeDelta); // �÷��̾ �Ĵٺ��� ���ۺ��� ��
	_bool TurnRoundAndBack(_double Speed, _double fDistDiff, _double TimeAcc, _double TimeDelta); // �÷��̾ �Ĵٺ��� ���鼭 fDistDiff ��ŭ �־���
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