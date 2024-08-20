#pragma once

#include "Client_Defines.h"
#include "Base.h"

#include "Player.h"

BEGIN(Engine)
class CTransform;
class CModel;
class CGameObject;
END

BEGIN(Client)

typedef class CAction_Assistant final : public CBase
{
public:
	enum ASSIST_TYPE
	{
		ASSIST_CANCLE,		// 컴뱃 애니메이션에만 적용되는 조건
		ASSIST_ATTACK,		// 공격 애니메이션에만 적용되는 조건
		ASSIST_MOVE,		// 특정 애니메이션에만 적용되는 조건
		ASSIST_GRAVITY,		// 공중 애니메이션에만 적용되는 조건
		ASSIST_END
	};

public:
	explicit CAction_Assistant() = default;
	virtual ~CAction_Assistant() = default;

public:
	HRESULT Initialize(CGameObject* _player);
	void Run(_double Timedelta);

	void SetUp_Weapon();

public:
	// Get, Set
	_float CancleAction_Progress() { return g_CancleAction_ProgressRate; }
	void CancleAction_Progress(_float _progress) { g_CancleAction_ProgressRate = _progress; }

	_float AttackNormal_Progress() { return g_AttackNormal_ProgressRate; }
	void AttackNormal_Progress(_float _progress) { g_AttackNormal_ProgressRate = _progress; }

	_bool Power() { return m_bPower; }
	void Power(_bool _power) { m_bPower = _power; }

	_bool Work(ASSIST_TYPE _actionType) { return m_bWork[_actionType]; }
	void Work(ASSIST_TYPE _actionType, _bool _val) { m_bWork[_actionType] = _val; }

	_bool Able(ASSIST_TYPE _actionType) { return m_bAble[_actionType]; }
	void Able(ASSIST_TYPE _actionType, _bool _val) { m_bAble[_actionType] = _val; }

	_bool Add_AssistAnim(ASSIST_TYPE _actionType, CPlayer::SORA_ANIM _soraAnim);
	_bool Rmv_AssistAnim(ASSIST_TYPE _actionType, CPlayer::SORA_ANIM _soraAnim);
	void FindOut_Type(CPlayer::SORA_ANIM _soraAnim, vector<ASSIST_TYPE>* _types);

	CPlayer::SORA_ANIM Dice_NextAttack(_uint _attackCount);
	CPlayer::SORA_ANIM Next_AttackAnimation_Normal(_uint _attackCount);
	CPlayer::SORA_ANIM Next_AttackAnimation_Hammer(_uint _attackCount);
	CPlayer::SORA_ANIM Next_AttackAnimation_Rage(_uint _attackCount);

	void Clear_AttackAnimOrder();

public:
	static _float g_CancleAction_ProgressRate;
	static _float g_CancleAction_AirDash_ProgressRate;
	static _float g_CancleAction_GuardSucceeded_ProgressRate;
	static _float g_AttackNormal_ProgressRate;
	static _float g_MovingNormal_ProgressRate;
	static _float g_GravityDefiance_ProgressRate;
	static _float g_Range_Search_Enemy;
	static _uint g_Conditional_Figures_To_Change_Attack_Animation_On_HammerForm;

private:
	CPlayer*		m_pPlayer = nullptr;
	CTransform*		m_pPlayerTransformCom = nullptr;
	CModel*			m_pPlayerModelCom = nullptr;

	class CPlayer_Weapon* m_pPlayerWeapon = nullptr;
	class CCombat_Assistant* m_pCombat_Assistant = nullptr;

	_bool m_bPower = true;
	_bool m_bWork[ASSIST_END];

	_bool m_bAble[ASSIST_END];
	list<CPlayer::SORA_ANIM> m_AssistAnim[ASSIST_END];

	CPlayer::SORA_ANIM m_ePrevAttackAnim = CPlayer::SORA_ANIM::SORA_ANIM_END;
	list<CPlayer::SORA_ANIM> m_AttackAnimOrder;

private:
	void Cancle_Form_Normal(CPlayer::SORA_ANIM _current_anim);
	void Cancle_Form_Hammer(CPlayer::SORA_ANIM _current_anim);
	void Cancle_Form_Rage(CPlayer::SORA_ANIM _current_anim);

	void Attack_Form_Normal(CPlayer::SORA_ANIM _current_anim);
	void Attack_Form_Hammer(CPlayer::SORA_ANIM _current_anim);
	void Attack_Form_Rage(CPlayer::SORA_ANIM _current_anim);

	void Gravity_Form_Normal(CPlayer::SORA_ANIM _current_anim);
	void Gravity_Form_Hammer(CPlayer::SORA_ANIM _current_anim);
	void Gravity_Form_Rage(CPlayer::SORA_ANIM _current_anim);

public:
	static CAction_Assistant* Create(CGameObject* _player);
	virtual void Free() override;

} A_ASSIST;

END