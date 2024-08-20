#pragma once

#include "Client_Defines.h"
#include "State.h"

#include "Player.h"

BEGIN(Client)

class CState_PlayerForm final : public CState
{
	explicit CState_PlayerForm() = default;
	virtual ~CState_PlayerForm() = default;

	enum RAGE_FORM_EFFECT {
		RAGE_EFFECT_PEARL,
		RAGE_EFFECT_RIPPLE,
		RAGE_EFFECT_MARBLE,
		RAGE_EFFECT_PARTICLE,
		RAGE_EFFECT_ROAR,
		RAGE_EFFECT_END
	};

private:
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool	KeyInput(_double TimeDelta) override;

public:
	virtual _bool Action(_double TimeDelta) override;

	virtual HRESULT EnterState() override;
	virtual HRESULT ExitState() override;
	virtual HRESULT ClearState() override;

public:
	_double Get_FormTimer() { return m_dFormTimer; }
	void Set_FormTimer(_double _formTime) { m_dFormTimer = _formTime; }

	_double Get_FormDuration() { return g_Form_Duration; }
	void Set_FormDuration(_double _formDuration) { g_Form_Duration = _formDuration; }

private:
	void Create_RageForm_Effects();
	void Play_RageForm_Sounds();

	void Play_HammerForm_Sounds();
	void Create_HammerForm_ChangingEffect();
	
public:
	static _double g_Form_Duration; // ÆûÀÇ Áö¼Ó½Ã°£

private:
	class CPlayer*	m_pPlayer = nullptr;
	class CCombat_Assistant* m_pCombat_Assistant = nullptr;
	class CAction_Assistant* m_pAction_Assistant = nullptr;
	class CPlayer_Weapon* m_pPlayer_Weapon = nullptr;

	CPlayer::SORA_FORM m_eInitialForm = CPlayer::SORA_FORM::SF_END;
	_double m_dFormTimer = 0.0; // Æû Å¸ÀÌ¸Ó
	
	_bool m_bHammerForm_Sound_Played = false;
	_uint m_iCreatedFrame_HammerForm_Changing_Effect = UINT_MAX;

	_double m_dRageForm_Effect_CreateTime[RAGE_EFFECT_END];
	_bool m_bRageForm_Effect_Created[RAGE_EFFECT_END];
	_bool m_bRageForm_Sound_Played[4];

	_bool m_bFormEnd = false;

public:
	static CState_PlayerForm* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END