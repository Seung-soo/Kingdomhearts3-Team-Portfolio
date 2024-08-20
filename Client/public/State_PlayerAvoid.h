#pragma once

#include "Client_Defines.h"
#include "State.h"

#include "Player.h"

BEGIN(Client)

class CState_PlayerAvoid final : public CState
{
public:
	explicit CState_PlayerAvoid() = default;
	virtual ~CState_PlayerAvoid() = default;

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

	_bool m_bAvoidSoundPlayed = false;
	_uint m_iSoundPlayedFrame = UINT_MAX;

public:
	static CState_PlayerAvoid* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END