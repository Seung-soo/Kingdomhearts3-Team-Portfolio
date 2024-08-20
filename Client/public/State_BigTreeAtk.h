#pragma once

#include "Client_Defines.h"
#include "MonsterDefines.h"
#include "State.h"
#include "BigTree.h"

BEGIN(Engine)
class CTransform;
class CMonster;
class CNavigation;
END

BEGIN(Client)

class CState_BigTreeAtk final : public CState
{
private:
	explicit CState_BigTreeAtk();
	virtual ~CState_BigTreeAtk() = default;

private:
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT EnterState();
	virtual _bool	Action(_double TimeDelta);

private:
	void	Set_Animation();
	_bool	BigTree_Atk(_double TimeDelta);

private:
	void	SummonCrow(_double TimeInterval, _double TimeDelta);
	void	SummonFruits(_double TimeInterval, _double TimeDelta);
	_bool	Chase(_float fDistDiff, _double TimeDelta);

private:
	CTransform*		m_pPlayerTransform   = nullptr;
	MONSTERTYPE		m_eMonsterType       = MONSTERTYPE_BIGTREE;
	CNavigation*	m_pMonsterNavigation = nullptr;

	class CTerrain* m_pCurTerrain = nullptr;
	class CBigTree* m_pBigTree    = nullptr;

	_double m_TimeAcc         = 0.0;
	_double m_EffectTimeAcc   = 0.0;
	_double m_ParticleTimeAcc = 0.0;

	_uint	m_iAnimFinishedAcc = 0;
	_uint	m_iAnimIndex       = 0;

	_bool m_bSoundEnd[2]  = { false, };
	_bool m_bEffectEnd[5] = { false, };

	CBigTree::BIGTREE_SKILL m_eSkill = CBigTree::BTSKILL_END;

public:
	static CState_BigTreeAtk* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END