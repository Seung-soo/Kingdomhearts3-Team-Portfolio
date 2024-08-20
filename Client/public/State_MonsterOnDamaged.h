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

class CState_MonsterOnDamaged final : public CState
{
private:
	explicit CState_MonsterOnDamaged();
	virtual ~CState_MonsterOnDamaged() = default;

private:
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT EnterState();
	virtual HRESULT ClearState();
	virtual _bool	Action(_double TimeDelta);

public:
	CTransform*		m_pPlayerTransform   = nullptr;
	MONSTERTYPE		m_eMonsterType       = MONSTERTYPE_END;
	CNavigation*	m_pMonsterNavigation = nullptr;

	class CTerrain* m_pRapunzelBossTerrain = nullptr;
	class CMonster* m_pMonster             = nullptr;

	_uint m_iAnimIndex = 0;

	_bool m_bActionEnd = false;

public:
	static CState_MonsterOnDamaged* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END