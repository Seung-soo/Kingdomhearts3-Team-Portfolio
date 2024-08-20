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

class CState_MonsterRunAway final : public CState
{
private:
	explicit CState_MonsterRunAway();
	virtual ~CState_MonsterRunAway() = default;

private:
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT EnterState();
	virtual _bool	Action(_double TimeDelta);

private:
	_bool TurnBackAndRunAway(_double MaxTime, _double TimeDelta);		// 플레이어한테 뒤돌아 MaxTime 만큼 도망감
	_bool TurnBackAndRunAway(_float fDiff, _double TimeDelta);			// 플레이어한테 뒤돌아 fDiff 만큼 도망감
	_bool LookPlayerAndRunAway(_double MaxTime, _double TimeDelta);		// 플레이어를 바라보고 MaxTime 만큼 도망감
	_bool FlyAway(_float fHeight, _double Speed, _double TimeDelta);	// 날아서 뒤로 도망감

private: // IceMan 전용
	_bool IceManRunAway(_double TimeDelta);
	_bool CreateIcicle(_double TimeDelta);

private:
	CTransform*		m_pPlayerTransform   = nullptr;
	MONSTERTYPE		m_eMonsterType       = MONSTERTYPE_END;
	CNavigation*	m_pMonsterNavigation = nullptr;

	class CMonster* m_pMonster = nullptr;

	_double m_TimeAcc        = 0.0;

	// 이펙트 관련
	_uint	m_iEffectCount   = 0;
	_double m_EffectInterval = 0.0;

public:
	static CState_MonsterRunAway* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END