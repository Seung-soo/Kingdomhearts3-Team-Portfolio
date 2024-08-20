#pragma once

#include "Client_Defines.h"
#include "State.h"

#include "Player.h"

BEGIN(Client)

class CState_PlayerAttack final : public CState
{
	// 여기에 어택타입 열거체가 필요할 수도 있다.
	// 혹은 다이나믹 어택, 스페셜 어택같은 스테이트가 추가될 수도 있음

	enum HIT_NUMBER { HIT_ONE, HIT_TWO, HIT_THREE, HIT_FOUR, HIT_FIVE, HIT_END  };

public:
	explicit CState_PlayerAttack() = default;
	virtual ~CState_PlayerAttack() = default;

private:
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool	KeyInput(_double TimeDelta) override;

public:
	virtual _bool Action(_double TimeDelta) override;

	virtual HRESULT EnterState() override;
	virtual HRESULT ExitState() override;
	virtual HRESULT ClearState() override;

	_uint AttackCount() { return m_iAttackCount; }
	void AttackCount(_uint _attackCount) { m_iAttackCount = _attackCount; }

	_bool EffectCreated(HIT_NUMBER _hitNumber) { return m_bEffectCreated[_hitNumber]; }
	void EffectCreated(HIT_NUMBER _hitNumber, _bool _val) { m_bEffectCreated[_hitNumber] = _val; }

	_bool FinishAttack() { return m_bFinishAttack; }
	void FinishAttack(_bool _finish) { m_bFinishAttack = _finish; }

private:
	void Register_Colliders_On_Normal(_double TimeDelta);
	void Register_Colliders_On_Hammer(_double TimeDelta);
	void Register_Colliders_On_RageForm(_double TimeDelta);

	void Reset_EffectCreated_Boolean_For_RageForm();

private:
	class CPlayer*	m_pPlayer = nullptr;
	class CCombat_Assistant* m_pCombat_Assistant = nullptr;
	class CAction_Assistant* m_pAction_Assistant = nullptr;
	class CPlayer_Weapon* m_pPlayer_Weapon = nullptr;

	_uint m_iAttackCount = 0;
	_bool m_bEffectCreated[HIT_END];

	_uint m_iVoiceSound_PlayedFrame[CPlayer::SORA_FORM::SF_END];
	_uint m_iLastAttackVoiceSound_PlayedFrame = UINT_MAX;

	_uint m_iSwingSound_PlayedFrame[CPlayer::SORA_FORM::SF_END];
	_uint m_iGrounHitSound_PlayedFrame = UINT_MAX;
	_uint m_iLastAttackSound_PlayedFrame = UINT_MAX; // 해머폼 : 라스트 어택, 레이지 폼 : RAGE_ATTACK5 마지막 구슬 공격
	_uint m_iFinishSound_PlayedFrame = UINT_MAX;
	_bool m_bFinishAttack = false;

	_bool m_bEarlyInput = false;
	CPlayer::SORA_ANIM m_eNextAttackAnim = CPlayer::SORA_ANIM_END;

	CGameObject* m_pTrailMeshEffect = nullptr;
	_bool m_bAttackReadyEffectCreated = false;

	// HammerForm
	_bool m_bHammerForm_Attack_Trail_Effect_Created = false;
	_bool m_bHammerForm_LastAttack_Effect_UpStream = false;
	_bool m_bHammerForm_LastAttack_Effect_AirHit = false;
	_bool m_bHammerForm_Effect_GroundHit_Created = false;
	_bool m_bHammerForm_Effect_Ember_Created = false;
	_bool m_bHammerForm_Effect_AirCut_Created = false;
	_uint m_iEmberCreation_FrameCount = 0;

	vector<CGameObject*> m_EmberEffects;

	// RageForm
	_bool  m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_END];
	_uint  m_iRageFormSequence = 0;

	_bool  m_bRageForm_Attack_Effect_WaveCreation = false;
	_double m_dRageForm_Attack_Effect_WaveTimer = 0.0;
	_uint m_iRageForm_Attack_Effect_WaveNumber = 3;

public:
	static CState_PlayerAttack* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END