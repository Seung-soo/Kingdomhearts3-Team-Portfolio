#pragma once

#include "Client_Defines.h"
#include "MonsterDefines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CNavigation;
class CMonster;
class CNavigation;
END

BEGIN(Client)

class CState_MonsterKnockBack final : public CState
{
private:
	explicit CState_MonsterKnockBack();
	virtual ~CState_MonsterKnockBack() = default;

private:
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT EnterState();
	virtual _bool	Action(_double TimeDelta);

private:
	_bool SkullSoldierAnim(_double TimeDelta);
	_bool ThinSoldierAnim(_double TimeDelta);
	_bool MonkeyAnim(_double TimeDelta);

public:
	CTransform*		m_pPlayerTransform   = nullptr;
	MONSTERTYPE		m_eMonsterType       = MONSTERTYPE_END;
	CNavigation*	m_pMonsterNavigation = nullptr;

	class CMonster* m_pMonster = nullptr;

	_bool m_bEffectEnd = false;

	_uint m_iAnimIndex = 0;

public:
	static CState_MonsterKnockBack* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END