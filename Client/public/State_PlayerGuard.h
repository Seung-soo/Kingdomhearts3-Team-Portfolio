#pragma once

#include "Client_Defines.h"
#include "State.h"

#include "Player.h"

BEGIN(Client)

class CState_PlayerGuard final : public CState
{
	enum RAGE_GUARD_EFFECT {
		RAGE_EFFECT_GUARD,
		RAGE_EFFECT_GUARD_SUCCEEDED,
		RAGE_EFFECT_COUNTER_ATTACK,
		RAGE_EFFECT_END
	};

public:
	explicit CState_PlayerGuard() = default;
	virtual ~CState_PlayerGuard() = default;

private:
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool	KeyInput(_double TimeDelta) override;

public:
	virtual _bool Action(_double TimeDelta) override;

	virtual HRESULT EnterState() override;
	virtual HRESULT ExitState() override;
	virtual HRESULT ClearState() override;

private:
	class CPlayer*	m_pPlayer = nullptr;
	class CCombat_Assistant* m_pCombat_Assistant = nullptr;
	class CAction_Assistant* m_pAction_Assistant = nullptr;
	class CPlayer_Weapon* m_pPlayer_Weapon = nullptr;

	_double m_dRageForm_Guard_Effect_CreateTime[RAGE_EFFECT_END];
	_bool m_bRageForm_Guard_Effect_Created[RAGE_EFFECT_END];

public:
	static CState_PlayerGuard* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END