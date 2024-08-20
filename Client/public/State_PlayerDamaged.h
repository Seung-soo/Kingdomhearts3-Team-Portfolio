#pragma once

#include "Client_Defines.h"
#include "State.h"

#include "Player.h"

BEGIN(Client)

class CState_PlayerDamaged final : public CState
{
public:
	typedef struct tagPlayerDamagedStateStruct
	{
		CPlayer::SORA_ANIM sora_anim;

	} PLAYER_DAMAGED_DESC;

public:
	explicit CState_PlayerDamaged() = default;
	virtual ~CState_PlayerDamaged() = default;

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

	PLAYER_DAMAGED_DESC m_DamagedDesc;
	_double m_dTestTimer = 0.0;

public:
	static CState_PlayerDamaged* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END