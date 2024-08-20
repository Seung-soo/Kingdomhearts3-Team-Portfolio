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

class CState_MonsterDead final : public CState
{
private:
	explicit CState_MonsterDead();
	virtual ~CState_MonsterDead() = default;

private:
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT EnterState();
	virtual _bool	Action(_double TimeDelta);

public:
	CTransform*		m_pPlayerTransform   = nullptr;
	MONSTERTYPE		m_eMonsterType       = MONSTERTYPE_END;

	class CMonster* m_pMonster           = nullptr;
	CNavigation*	m_pMonsterNavigation = nullptr;

	_uint m_iAnimIndex = 0;

	_bool m_bEffectEnd = false;

public:
	static CState_MonsterDead* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END