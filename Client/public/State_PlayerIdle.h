#pragma once

#include "Client_Defines.h"
#include "State.h"

#include "Player.h"

BEGIN(Client)

class CState_PlayerIdle final : public CState
{
public:
	explicit CState_PlayerIdle() = default;
	virtual ~CState_PlayerIdle() = default;

private:
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool	KeyInput(_double TimeDelta) override;

public:
	virtual _bool Action(_double TimeDelta);

	virtual HRESULT EnterState() override;
	virtual HRESULT ExitState() override;

public:
	static _double g_TimeLength_To_Boring;

private:
	CPlayer* m_pPlayer = nullptr;
	class CCombat_Assistant* m_pCombat_Assistant = nullptr;
	class CAction_Assistant* m_pAction_Assistant = nullptr;
	class CPlayer_Weapon* m_pPlayer_Weapon = nullptr;

	_double m_dTimer = 0.0;
	_bool m_bBoringDone = false;

private:
	void Change_To_IdleAnimation();

public:
	static CState_PlayerIdle* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END