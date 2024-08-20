#pragma once

#include "Client_Defines.h"
#include "State.h"

#include "Player.h"

BEGIN(Engine)
END

BEGIN(Client)

class CCamera_Perspective;
class CState_PlayerInteraction final : public CState
{
public:
	explicit CState_PlayerInteraction() = default;
	virtual ~CState_PlayerInteraction() = default;

private:
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool	KeyInput(_double TimeDelta) override;

public:
	virtual _bool Action(_double TimeDelta) override;

	virtual HRESULT EnterState() override;
	virtual HRESULT ExitState() override;

private:
	class CPlayer*	m_pPlayer = nullptr;
	class CCombat_Assistant* m_pCombat_Assistant = nullptr;
	class CAction_Assistant* m_pAction_Assistant = nullptr;
	class CPlayer_Weapon* m_pPlayer_Weapon = nullptr;

	CPlayer::SORA_ANIM m_eInitialAnim = CPlayer::SORA_ANIM::SORA_ANIM_END;

public:
	static CState_PlayerInteraction* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END