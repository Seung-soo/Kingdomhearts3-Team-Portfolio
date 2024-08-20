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

class CState_MonsterSpawn final : public CState
{
private:
	explicit CState_MonsterSpawn();
	virtual ~CState_MonsterSpawn() = default;

private:
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT EnterState();
	virtual _bool	Action(_double TimeDelta);

private:
	_bool NightmareSpawn(_double TimeDelta);
	_bool NobodySpawn(_double TimeDelta);
	_bool ThinSoldierSpawn(_double TimeDelta);

private:
	CTransform*		m_pPlayerTransform   = nullptr;
	MONSTERTYPE		m_eMonsterType       = MONSTERTYPE_END;
	CNavigation*	m_pMonsterNavigation = nullptr;

	class CTerrain* m_pRapunzelBossTerrain = nullptr;
	class CMonster* m_pMonster = nullptr;

	_bool	m_bSoundEnd  = false;
	_bool	m_bEffect[3] = { false, };
	_double m_TimeAcc    = 0.0;

	_uint m_iSoundChannel = 0;

public:
	static CState_MonsterSpawn* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END