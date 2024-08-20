#pragma once
// 모션 블러 키려면 패스 15번으로

#include "Client_Defines.h"
#include "GameObject.h"

#include "Model.h"
#include "StateMachine.h"
#include "Effect_VISwordTrail.h"
#include "Player_Weapon.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CCollider;
class CNavigation;
class CStatus;
class CCell;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	enum SORA_ANIM
	{
		/* 000 */ SORA_ANIM_IDLE, SORA_ANIM_BORING, SORA_ANIM_DRAW_BLADE,
		/* 003 */ SORA_ANIM_IDLE_BLADE, SORA_ANIM_IDLE_BLADE_END,
		/* 005 */ SORA_ANIM_BORING_BLADE, SORA_ANIM_RUN_FORWARD, SORA_ANIM_RUN_FORWARD_BLADE, 
		/* 008 */ SORA_ANIM_STEPPING_LTOR, SORA_ANIM_STEPPING_RTOL,
		/* 010 */ SORA_ANIM_JUMP_UP, SORA_ANIM_FALL_START, SORA_ANIM_FALL_LOOP, SORA_ANIM_FALL_END,

		// Default Attack Animations
		/* 014 */ SORA_ANIM_GROUND_ATTACK1, SORA_ANIM_GROUND_ATTACK2, SORA_ANIM_GROUND_ATTACK3, SORA_ANIM_GROUND_ATTACK4,
		/* 018 */ SORA_ANIM_AIR_ATTACK1, SORA_ANIM_AIR_ATTACK2, SORA_ANIM_AIR_ATTACK3, SORA_ANIM_AIR_ATTACK4,
		/* 022 */ SORA_ANIM_BLOCKED_BY_ENV, SORA_ANIM_AIR_BLOCKED_BY_ENV,

		// Guard and Avoid Animations
		/* 024 */ SORA_ANIM_GUARD, SORA_ANIM_GUARD_SUCCEEDED,
		/* 026 */ SORA_ANIM_AIR_GUARD, SORA_ANIM_AIR_GUARD_SUCCEEDED,
		/* 028 */ SORA_ANIM_ROLL, SORA_ANIM_AIR_DASH, 

		// Damaged Animations
		/* 030 */ SORA_ANIM_DAMAGED_NORMAL_FU, SORA_ANIM_DAMAGED_NORMAL_BU, SORA_ANIM_DAMAGED_NORMAL_R, SORA_ANIM_DAMAGED_NORMAL_L, SORA_ANIM_DAMAGED_NORMAL_FD, SORA_ANIM_DAMAGED_NORMAL_BD,
		/* 036 */ SORA_ANIM_AIR_DAMAGED_NORMAL_F, SORA_ANIM_AIR_DAMAGED_NORMAL_B, SORA_ANIM_AIR_DAMAGED_NORMAL_R, SORA_ANIM_AIR_DAMAGED_NORMAL_L,
		/* 040 */ SORA_ANIM_AIR_DAMAGED_AERIAL_F_ENTER, SORA_ANIM_AIR_DAMAGED_AERIAL_F_LOOP, SORA_ANIM_AIR_DAMAGED_AERIAL_F_EXIT,
		/* 043 */ SORA_ANIM_AIR_DAMAGED_AERIAL_B_ENTER, SORA_ANIM_AIR_DAMAGED_AERIAL_B_LOOP, SORA_ANIM_AIR_DAMAGED_AERIAL_B_EXIT,
		/* 046 */ SORA_ANIM_AIR_DAMAGED_AERIAL_R_ENTER, SORA_ANIM_AIR_DAMAGED_AERIAL_R_LOOP, SORA_ANIM_AIR_DAMAGED_AERIAL_R_EXIT,
		/* 049 */ SORA_ANIM_AIR_DAMAGED_AERIAL_L_ENTER, SORA_ANIM_AIR_DAMAGED_AERIAL_L_LOOP, SORA_ANIM_AIR_DAMAGED_AERIAL_L_EXIT,
		/* 052 */ SORA_ANIM_DUMP_BACK, SORA_ANIM_DUMP_FRONT, SORA_ANIM_DUMP_LEFT, SORA_ANIM_DUMP_RIGHT,
		/* 056 */ SORA_ANIM_STUN_ENTER, SORA_ANIM_STUN_LOOP, SORA_ANIM_STUN_EXIT,

		// DynamicMove Animations
		/* 059 */ SORA_ANIM_HANG_ENTER, SORA_ANIM_HANG_LOOP, SORA_ANIM_RISE, SORA_ANIM_RUN_WALL, SORA_ANIM_JUMP_AT_WALL, SORA_ANIM_SOMERSAULT,

		// Extra Attack Animations_1
		/* 065 */ SORA_ANIM_UPPER_ATTACK1, SORA_ANIM_UPPER_ATTACK2, SORA_ANIM_UPPER_ATTACK3, SORA_ANIM_UPPER_ATTACK4, 
		/* 069 */ SORA_ANIM_LOWER_ATTACK1, SORA_ANIM_LOWER_ATTACK2, SORA_ANIM_LOWER_ATTACK3, SORA_ANIM_LOWER_ATTACK4,

		// Skill Animations
		/* 073 */ SORA_ANIM_SKILL_LEVEL1, SORA_ANIM_SKILL_LEVEL2, SORA_ANIM_SKILL_MOVE, 
		/* 076 */ SORA_ANIM_AIR_SKILL_LEVEL1, SORA_ANIM_AIR_SKILL_LEVEL2, SORA_ANIM_AIR_SKILL_MOVE,

		// Chest Animations
		/* 079 */ SORA_ANIM_OPEN_SMALL, SORA_ANIM_OPEN_BIG, 

		// Rage Form Animations
		/* 081 */ SORA_ANIM_RAGE_IDLE, SORA_ANIM_RAGE_RUN,
		/* 083 */ SORA_ANIM_RAGE_JUMP_UP, SORA_ANIM_RAGE_FALL_START, SORA_ANIM_RAGE_FALL_LOOP, SORA_ANIM_RAGE_FALL_END,
		/* 087 */ SORA_ANIM_RAGE_AIR_DASH, SORA_ANIM_RAGE_GUARD_SUCCEEDED, SORA_ANIM_RAGE_ROLL,
		/* 090 */ SORA_ANIM_RAGE_DAMAGED_F, SORA_ANIM_RAGE_DAMAGED_B, SORA_ANIM_RAGE_DAMAGED_L, SORA_ANIM_RAGE_DAMAGED_R,
		/* 094 */ SORA_ANIM_RAGE_AIR_DAMAGED_F, SORA_ANIM_RAGE_AIR_DAMAGED_B, SORA_ANIM_RAGE_AIR_DAMAGED_L, SORA_ANIM_RAGE_AIR_DAMAGED_R,
		/* 098 */ SORA_ANIM_RAGE_DAMAGED_SLIDE_F, SORA_ANIM_RAGE_DAMAGED_SLIDE_B, SORA_ANIM_RAGE_DAMAGED_SLIDE_L, SORA_ANIM_RAGE_DAMAGED_SLIDE_R,
		/* 102 */ SORA_ANIM_RAGE_BLOCKED, SORA_ANIM_RAGE_AIR_BLOCKED, SORA_ANIM_RAGE_GUARD, SORA_ANIM_RAGE_AIR_GUARD,
		/* 106 */ SORA_ANIM_RAGE_FORM, SORA_ANIM_RAGE_SHOUT,
		/* 108 */ SORA_ANIM_RAGE_DOUBLE_TAP, SORA_ANIM_RAGE_TRIPLE_TAP,
		/* 110 */ SORA_ANIM_RAGE_ATTACK1, SORA_ANIM_RAGE_ATTACK2, SORA_ANIM_RAGE_ATTACK3, SORA_ANIM_RAGE_ATTACK4, SORA_ANIM_RAGE_ATTACK5,
		/* 115 */ SORA_ANIM_RAGE_FINISH, SORA_ANIM_RAGE_JUMP_SHOUT, SORA_ANIM_RAGE_COUNTER_ATTACK,

		// Hammer Form Animations
		/* 118 */ SORA_ANIM_HAMMER_TAKE_OUT, SORA_ANIM_HAMMER_PUT_DOWN,
		/* 120 */ SORA_ANIM_HAMMER_IDLE_LONG, SORA_ANIM_HAMMER_IDLE_SHORT,
		/* 122 */ SORA_ANIM_HAMMER_RUN,
		/* 123 */ SORA_ANIM_HAMMER_JUMP_UP, SORA_ANIM_HAMMER_FALL_START, SORA_ANIM_HAMMER_FALL_LOOP, SORA_ANIM_HAMMER_FALL_END,
		/* 127 */ SORA_ANIM_HAMMER_SECOND_JUMP_UP, SORA_ANIM_HAMMER_SECOND_JUMP,
		/* 129 */ SORA_ANIM_HAMMER_DASH,
		/* 130 */ SORA_ANIM_HAMMER_DAMAGED_F, SORA_ANIM_HAMMER_DAMAGED_B, SORA_ANIM_HAMMER_DAMAGED_R, SORA_ANIM_HAMMER_DAMAGED_L,
		/* 134 */ SORA_ANIM_HAMMER_AIR_DAMAGED_F, SORA_ANIM_HAMMER_AIR_DAMAGED_B, SORA_ANIM_HAMMER_AIR_DAMAGED_R, SORA_ANIM_HAMMER_AIR_DAMAGED_L,
		/* 138 */ SORA_ANIM_HAMMER_DAMAGED_SLIDE_F, SORA_ANIM_HAMMER_DAMAGED_SLIDE_B, SORA_ANIM_HAMMER_DAMAGED_SLIDE_R, SORA_ANIM_HAMMER_DAMAGED_SLIDE_L,
		/* 142 */ SORA_ANIM_HAMMER_GUARD, SORA_ANIM_HAMMER_GUARD_SUCCEEDED,
		/* 144 */ SORA_ANIM_HAMMER_AIR_GUARD, SORA_ANIM_HAMMER_AIR_GUARD_SUCCEEDED,
		/* 146 */ SORA_ANIM_HAMMER_SKILL_LEVEL01, SORA_ANIM_HAMMER_SKILL_LEVEL02,
		/* 148 */ SORA_ANIM_HAMMER_AIR_SKILL_LEVEL01, SORA_ANIM_HAMMER_AIR_SKILL_LEVEL02,
		/* 150 */ SORA_ANIM_HAMMER_BUFF, SORA_ANIM_HAMMER_AIR_BUFF,
		/* 152 */ SORA_ANIM_HAMMER_FINISH_READY, SORA_ANIM_HAMMER_AIR_FINISH_READY,
		/* 155 */ SORA_ANIM_HAMMER_ATTACK_01, SORA_ANIM_HAMMER_ATTACK_02, SORA_ANIM_HAMMER_ATTACK_03,
		/* 158 */ SORA_ANIM_HAMMER_ATTACK_04, SORA_ANIM_HAMMER_ATTACK_05, SORA_ANIM_HAMMER_ATTACK_06,
		/* 160 */ SORA_ANIM_HAMMER_GROUND_LAST_ATTACK, SORA_ANIM_HAMMER_AIR_LAST_ATTACK,
		/* 162 */ SORA_ANIM_HAMMER_FORM, SORA_ANIM_HAMMER_FINISH,
		/* 164 */ SORA_ANIM_AIR_IDLE, SORA_ANIM_AIR_MOVE, SORA_ANIM_SECOND_JUMP_UP, SORA_ANIM_SECOND_JUMP_FALL_START, SORA_ANIM_STRAIGHTEN, 
		/* 000 */ SORA_ANIM_END
	};

	enum SORA_BODY
	{
		SB_BODY,
		SB_FACE,
		SB_END
	};

	enum SORA_INPUT
	{
		SI_WASD, SI_SPACE, 
		SI_LBUTTON, SI_RBUTTON, 
		SI_RIGHT, 
		SI_R, SI_E, SI_F, 
		SI_SUMMON, 
		SI_EVENT, 
		SI_END
	};

	enum SORA_MAGIC
	{
		SM_DONALD1, 
		SM_DONALD2, 
		SM_GOOFY1, 
		SM_GOOFY2, 
		SM_END
	};

	enum SORA_FORM
	{
		SF_NORMAL, 
		SF_HAMMER, 
		SF_RAGE, 
		SF_END
	};

	enum SORA_OUTFIT
	{
		SO_HAMMER,
		SO_RAGE,
		SO_END
	};

	enum FORM_FUNCTION
	{
		FORM_FUNC_RAGEFORM,
		FORM_FUNC_RAGE_FINISH,
		FORM_FUNC_HAMMERFORM,
		FORM_FUNC_HAMMER_FINISH,
		FORM_FUNC_COUNTER_ATTACK,
		FORM_FUNC_END
	};

	typedef enum PLAYER_SOUND_TYPE
	{
		PLAYER_SOUND_ACTION,
		PLAYER_SOUND_ATTACK,
		PLAYER_SOUND_LAST_AND_FINISH,
		PLAYER_SOUND_DAMAGED,
		PLAYER_SOUND_EFFECT,
		PLAYER_SOUND_END

	} P_SOUND;

	enum PLAYER_ITEM
	{
		PLAYER_ITEM_DAMAGE,
		PLAYER_ITEM_ATTACK_SPEED,
		PLAYER_ITEM_MOVE_SPEED,
		PLAYER_ITEM_END
	};

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	class CCamera_Perspective* Get_LinkedCamera() { return m_pCamera; }

	// Get Components
	CModel* Get_Model() { return m_pModelCom; }
	CNavigation* Get_Navigation() { return m_pNavigationCom; }
	CStateMachine* Get_StateMachine() { return m_pStateMachineCom; }

	_float Get_Angle() { return m_fAngle_Between_CameraD; }
	_float Get_AngleR() { return m_fAngle_Between_CameraR; }

	SORA_ANIM Get_AnimIndex_EachBody(SORA_BODY eBodyEnum) {
		return m_eSoraAnim[eBodyEnum];	}

	void Set_AnimIndex_EachBody(SORA_BODY eBodyEnum, SORA_ANIM eSoraAnim) {
		m_eSoraAnim[eBodyEnum] = eSoraAnim;	}

	_bool Get_ControlMode() { return m_bControlMode; }
	void Set_ControlMode(_bool bControlMode) { m_bControlMode = bControlMode; }

	_bool Get_HoldingWeapon() { return m_bHoldingWeapon; }
	void Set_HoldingWeapon(_bool bHoldingWeapon) { m_bHoldingWeapon = bHoldingWeapon; }

	_bool Get_Teleport() { return m_bTeleport; }
	void Set_Teleport(_bool bTeleport) { m_bTeleport = bTeleport; }

	_bool Get_Hovering() { return m_bHovering; }
	void Set_Hovering(_bool bHovering) { m_bHovering = bHovering; }

	_bool Get_Hovering_MoveAnimation() { return m_bHovering_MoveAnimation; }
	void Set_Hovering_MoveAnimation(_bool bMoveAnimation) { m_bHovering_MoveAnimation = bMoveAnimation; }

	_uint Get_Combo() { return m_iCombo; }
	void Set_Combo(_uint iCombo) { m_iCombo = iCombo; }

	_double Get_ComboTimer() { return m_dComboTimer; }
	void Set_ComboTimer(_double dTimer) { m_dComboTimer = dTimer; }

	_double Get_ComboTimer_Limit() { return m_dComboTimer_Limit; }
	void Set_ComboTimer_Limit(_double dTimer) { m_dComboTimer_Limit = dTimer; }

	_float Get_MoveAmountY() { return m_fMoveAmountY_OnAttack; }
	void Set_MoveAmountY(_float fAmountY) { m_fMoveAmountY_OnAttack = fAmountY; }

	_double Get_ChasingSpeedY() { return m_dAxisY_ChasingSpeed; }
	void Set_ChasingSpeedY(_double dSpeed) { m_dAxisY_ChasingSpeed = dSpeed; }

	_bool Get_Input(SORA_INPUT _input) { return m_bInput[_input]; }
	void Set_Input(SORA_INPUT _input, _bool _val) { m_bInput[_input] = _val; }

	SORA_MAGIC Get_Magic() { return m_eMagic; }
	void Set_Magic(SORA_MAGIC _magic) { m_eMagic = _magic; }

	SORA_FORM Get_Form() { return m_eForm; }
	void Set_Form(SORA_FORM _form) { m_eForm = _form; }

	_bool Get_AbleToFinish() { return m_bForm_AbleToFinish; }
	void Set_AbleToFinish(_bool bAble) { m_bForm_AbleToFinish = bAble; }

	_bool Get_FormTexture() { return m_bFormTexture; }
	void Set_FormTexture(_bool bFormTexture) { m_bFormTexture = bFormTexture; }

	_vector Get_MoveDir() { return m_vMoveDir; }
	void Set_MoveDir(_fvector vDirection) { m_vMoveDir = vDirection; }

	_vector Get_MoveDir_OnWall() { return m_vMoveDir_OnWall; }
	void Set_MoveDir_OnWall(_fvector vDirection) { m_vMoveDir_OnWall = vDirection; }

	_vector Get_AvoidingDir() { return m_vAvoidingDirection; }
	void Set_AvoidingDir(_fvector vDirection) { m_vAvoidingDirection = vDirection; }

	_double Get_GuardTimer() { return m_dGuardTimer; }
	void Set_GuardTimer(_double dTimer) { m_dGuardTimer = dTimer; }

	_double Get_GuardTimer_Max() { return m_dGuardTimer_Max; }
	void Set_GuardTimer_Max(_double dTimer) { m_dGuardTimer_Max = dTimer; }

	_double Get_InvincibilityTimer() { return m_dInvincibilityTimer; }
	void Set_InvincibilityTimer(_double dTimer) { m_dInvincibilityTimer = dTimer; }

	_double Get_InvincibilityTimer_Max() { return m_dInvincibilityTimer_Max; }
	void Set_InvincibilityTimer_Max(_double dTimer) { m_dInvincibilityTimer_Max = dTimer; }

	_float	Get_Dissolve() { return m_fTime; }
	_bool	Get_TP() { return m_bTP; }

	class CPlayer_Weapon* Get_Weapon() { return m_pWeapon; }
	class CCombat_Assistant* Get_CombatAssistant() { return m_pCombat_Assistant; }
	class CAction_Assistant* Get_ActionAssistant() { return m_pAction_Assistant; }

	_vector Get_BodyCollider_Center() { return m_pColliderCom->Get_Center(); }
	virtual CCollider* Get_HitCollider() { return m_pColliderCom; }

	_bool Set_ItemValues(PLAYER_ITEM eItemType, _double dDuration = 30.0);
	void Apply_Item(PLAYER_ITEM eItemType);

	// 이동 사운드
	void Play_MoveSound(_float fVolum = 1.f); // 이동할 때 나오는 사운드
	void Play_LandSound(_float fVolum = 1.f); // 착지 사운드
	void Play_RiseSound(_float fVolum = 1.f); // 벽 다 올라가서 회전할 때 나오는 사운드

	// 목소리
	void Play_AttackVoiceSound(_bool bLastCombo, _float fVolum = 1.f); // 공격 기합 사운드
	void Play_LastAttackVoiceSound(_float fVolum = 1.f); // 피니시, 라스트 어택
	void Play_ActionVoiceSound(_float fVolum = 1.f); // 동작들 재생시 나오는 보이스 사운드
	void Play_DamagedVoiceSound(_float fVolum = 1.f); // 피격시 재생되는 사운드

	// 일반 좌클릭 공격 히트 사운드
	void Play_AttackHitSound(_bool bLastCombo, _float fVolum = 1.f);

	// 노멀 폼
	void Play_NormalFormSwing(_float fVolum = 1.f);

	// 해머 폼
	void Play_HammerFormSound(_float fVolum = 1.f);
	void Play_HammerFormSwing(_float fVolum = 1.f);
	void Play_HammerFormWaveSound(SORA_ANIM eCurrentnAnim, _float fVolum = 1.f);
	void Play_HammerFormLastSound(_float fVolum = 1.f);
	
	void Play_GuardSucceededSound(_float fVolum = 1.f); // 가드 성공시 재생되는 사운드
	_bool Play_AvoidSound(SORA_ANIM eCurrentnAnim, _float fVolum = 1.f); // 구르기, 에어대시 시에 작게 깔리는 사운드
	void Play_SummonSound(_float fVolum = 1.f); // 도널드, 구피 소환 사운드
	void Play_FormChangeUI(_float fVolum = 1.f); // UI_FormChange 클릭 사운드

	// 레이지 폼
	void Play_RageFormSound(_uint iSoundIndex, _float fVolum = 1.f); // 레이지 폼 변신 사운드
	void Play_RageFormSwing(_float fVolum = 1.f); // 레이지 폼 공격 스윙 사운드
	void Play_RageWind(_float fVolum = 1.f); // 레이지 폼 공격 5번 바람소리 + 피니시 어택
	void Play_RageTeleport(_float fVolum = 1.f); // 레이지 폼 대시 순간이동 때 나오는 사운드(순간이동 때 사운드 2번씩 나온다)
	void Play_RageGuard(_float fVolum = 1.f); // 레이지 폼은 일반 가드 때 사운드가 존재함
	void Play_RageFormLastSound(_float fVolum = 1.f);
	void Play_RageFormFinish(_float fVolum = 1.f);

	// 상자열기
	void Play_OpenChest(_uint iSoundIndex, _float fVolum = 1.f);

	// UI
	void Play_SideUI_PopUp_Sound(_float fVolum = 1.f);
	
	void Update_SoundChannel(PLAYER_SOUND_TYPE eSoundType);

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void	PostRender(_double TimeDelta);

	// For Shadow
	virtual HRESULT Render_Shadow() override;

public:
	virtual HRESULT Save_ObjectData(HANDLE hFile) override;
	virtual HRESULT Load_ObjectData(HANDLE hFile) override;

	void Change_PlayerAnimation(SORA_ANIM eSoraAnim);
	_bool Check_OnFinish(); // Finish Attack 애니메이션을 재생 중인지 확인하는 함수

	void GetOnNavigation(); // 네비게이션 강제로 태우는 함수
	_bool Check_OnGround(); // JumpBoolean 값에 상관없이 Y값이 터레인 또는 네비의 Y값과 일치하는지 확인하는 함수

	// StateMachine =============================================================
	void Create_PlayerState(_uint iStateType, void* pArg = nullptr);
	_bool Check_PlayerState(CStateMachine::STATETYPE _stateType);

	HRESULT MakeState_Idle();

	_uint Get_AttackCount();
	void Add_Combo();

	_bool Anim_Check_PlusJumpChain(_uint iAnimIndex);
	_bool Anim_Check_HoveringStart(_uint iAnimIndex);
	_bool Anim_Check_AbleToSecondJump(_uint iAnimIndex);
	_bool Anim_Check_LandOnGround(_uint iAnimIndex);
	_bool Anim_Check_GuardSucceeded(_uint iAnimIndex);
	_bool Anim_Check_MoveOnAxisY(_uint iAnimIndex);
	_bool Anim_Check_LastCombo(_uint iAnimIndex);
	_bool Anim_Check_NormalAttacks(_uint iAnimIndex);

	// 플레이어의 중점과 플레이어의 포지션(바닥) 간의 높이 차이값을 반환하는 함수
	_float Calculate_GapY_Between_PositionAndCenter();

	// 무적 상태 입력
	void Invincibility_Input(_double dDuration, _double dSpeed);

	// 최종 보스 전용 함수
	void Attacked_By_PillarOfFire(_fvector vPillaorPosition);
	void Attacked_By_Teleport();

	// Weapon ===================================================================
	void Draw_Weapon();
	void Sheathe_Weapon();

	_bool Register_WeaponCollider();
	void Register_WeaponCollider(_bool bRegister);
	// ==========================================================================

	// combat_Assistant =========================================================
	_bool Eliminate_DeadTarget(CGameObject* _deadTarget);
	CGameObject* Get_Target();

	void Set_AttackTarget(CCollider* _attackTarget);
	// ==========================================================================

	void Set_Trail_Active(_bool isActive) { m_pWeapon->Set_Trail_Active(isActive); }

	void Dead_FormUI(FORM_FUNCTION _uiType);
	void Null_FormUI(FORM_FUNCTION _uiType);
	CGameObject* Get_FormUI(FORM_FUNCTION _uiType);

	void Raise_RageGauge();
	void Empty_RageGauge();
	_float Get_RageForm_GaugeRate();

	void Raise_HammerCombo();
	void Empty_HammerCombo();

	// ==========================================================================

	void SetUp_BoxOpenSoundValues();
	_float4* Get_BonePosition(); // center의 포지션 담아둔 멤버변수 m_vCenterPosition 를 행렬연산하여 반환한다

public:
	static _bool g_RatioControl;
	static _float g_RatioXZ, g_RatioY;

	static _bool g_SoldierType;
	static _float g_PlayerDefaultMoveSpeed;

private:
	class CCamera_Perspective* m_pCamera = nullptr;
	CTransform* m_pCameraTransform = nullptr;

	_float m_fAngle_Between_CameraD = 0.0f;
	_float m_fAngle_Between_CameraR = 0.0f;

	class CPlayer_Weapon* m_pWeapon = nullptr;
	class CCombat_Assistant* m_pCombat_Assistant = nullptr;
	class CAction_Assistant* m_pAction_Assistant = nullptr;

	SORA_ANIM m_eSoraAnim[SORA_BODY::SB_END];
	_uint m_iPlayerSound[P_SOUND::PLAYER_SOUND_END];

	list<SORA_ANIM> m_WeaponDrawAnim;
	_bool m_bHoldingWeapon = false;
	_bool m_bTeleport = false;

	_bool m_bHovering = false;
	_bool m_bHovering_MoveAnimation = false;
	vector<SORA_ANIM> m_Animation_PlusJumpChain;
	vector<SORA_ANIM> m_Animation_HoveringStart;
	vector<SORA_ANIM> m_Animation_AbleToSecondJump;
	vector<SORA_ANIM> m_Animation_LandOnGround;
	vector<SORA_ANIM> m_Animation_GuardSucceeded;
	vector<SORA_ANIM> m_Animation_LastCombo;
	vector<SORA_ANIM> m_Animation_NormalAttacks;

	// 공격 관련 변수
	_uint m_iCombo = 0;
	_double m_dComboTimer = 0.0;
	_double m_dComboTimer_Limit = 4.0;

	_int m_iHammerForm_ComboStep = 1;
	_int m_iHammerForm_Combo = 0;
	_int m_iHammerForm_ComboMax = 10;

	_int m_iRageForm_GaugeStep = 1;
	_int m_iRageForm_Gauge = 0;
	_int m_iRageForm_GaugeMax = 12;

	_double m_dRageForm_GaugeTimer = 0.0;
	_double m_dRageForm_GaugeTimer_Limit = 0.0;

	CGameObject* m_pFormFunctionUI[FORM_FUNC_END] = { nullptr };

	// 아이템 지속시간 관련 변수
	_bool m_bItem[PLAYER_ITEM_END];
	_double m_dItemTimer[PLAYER_ITEM_END];
	_float4 m_vRimColorEachItem[PLAYER_ITEM_END];

	// 상호작용 관련 변수(박스 오픈 사운드 재생)
	_double m_dSoundTimer_BoxOpen = 0.0;
	_double m_dSoundTimer_BoxOpen_Duration = 90.0;
	_uint m_iRecorded_SoundPlayedFrame = UINT_MAX;

	// Y이동을 시키는 공격 관련 변수
	_float m_fMoveAmountY_OnAttack = 0.f;
	vector<SORA_ANIM> m_Animation_MoveOnAxisY;
	_double m_dAxisY_ChasingSpeed = 0.5;

	// 플레이어 행동 제어 변수
	_bool m_bControlMode = false;
	
	// 사운드 제어 변수
	_bool m_bLandSoundPlayed = false;
	_bool m_bGuardSucceededPlayed = false;

	// 마법(스킬)사용 관련 변수
	_bool m_bInput[SORA_INPUT::SI_END];
	SORA_MAGIC m_eMagic = SORA_MAGIC::SM_END;

	// 폼 체인지 관련 변수
	SORA_FORM m_eForm = SORA_FORM::SF_END;
	_bool m_bForm_AbleToFinish = false;
	_bool m_bAbleToChange_Form[SORA_OUTFIT::SO_END];

	_bool m_bFormTexture = false;

	// 가드 관련 변수
	_double m_dGuardTimer = 0.0;
	_double m_dGuardTimer_Max = 0.0;

	// 무적 관련 변수
	_double m_dInvincibilityTimer = 0.0;
	_double m_dInvincibilityTimer_Max = 0.0;
	_double m_dInvincibilityTimer_Speed = 1.0;

	// 이동 변수
	_vector m_vMoveDir = XMVectorZero();
	_vector m_vMoveDir_OnWall = XMVectorZero();
	_vector m_vAvoidingDirection = XMVectorZero();

	// 벽(TYPE_RUNNING) 충돌 감지 변수
	_float3 m_vWallFinder;
	_float m_fFinderDistance = 0.5f;
	_float m_fAngle_Between_Wall = 0.f;
	_double m_dTimer_ToStartFreeRunning = 0.0;

	// 네비게이션 관련 변수
	_float m_fDistance_To_Closest_Line = 0.f;
	_uint m_iClosest_Line_Type = UINT_MAX;

	CModel::BONEMATRIX_PTR m_BoneMatrixPtr;
	_float4x4 m_SocketTransformMatrix;

	// ImGuI
	_bool m_bFormChangeLock = true;
	MONSTERTYPE m_eMonsterType = MONSTERTYPE::MONSTERTYPE_END;
	_float creation_point[3]{};
	_int m_iDamagedAnim = 0;

	_bool m_bInfinityEnergy = false;

	// Effect 변수
	_float4	m_vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	_int	m_iEffectCount = 0;

private:
	// NativeConstruct //
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	HRESULT Separate_Channels();

	void Adjust_Animation_Details();
	HRESULT SetUp_RootNodeIndex();
	void MakeList_Once();
	void MakeList_Lerp();
	void MakeList_Draw();
	void SetUp_StateAnimations();

	void MakeList_PlusJumpChain();
	void MakeList_HoveringStart();
	void MakeList_AbleToSecondJump();
	void MakeList_LandOnGround();
	void MakeList_GuardSucceeded();
	void MakeList_LastCombo();
	void MakeList_MoveOnAxisY();
	void MakeList_NormalAttacks();

	HRESULT Create_Weapon();
	void SetUp_Assistant();

	// Tick //
	void Work_On_FirstTick();
	void Calculate_AngleValues();
	void Calculate_CellValues();

	void Update_Input(_double Timedelta);
	void Update_Terms(_double Timedelta);
	void Calculate_AvoidingDirection();

	void ChangeState();
	void ChangeForm();

	void Effect_Control();
	void CellEntry_Processing();

	void Set_LocationAssistance_MoveAmountY(_fvector vPoint);
	void Set_LocationAssistance_InputValues(SORA_ANIM eTempIndex);

	void LocationAssistance_Tick(_double TimeDelta);

	void Show_UI_FormChange();

	void Teleport_Dissolve(_double TimeDelta);

	// LateTick //
	void OnCollision();
	void Select_DamagedAnimation(CTransform::DAMAGEDDIR eDirection);

	// Render //
	void Overwrite_OutfitTexture(_uint iMaterialIndex);

	// PostRender //
	void Control_Animation();
	void Control_Collider();
	void Control_Jump();
	void Control_StateMachine();
	void Control_DamageState();
	void Control_CombatAssistant();
	void Control_ActionAssistant();

	_bool Check_DrawAnim(SORA_ANIM eSoraAnim);
	_bool Check_DrawAnim();

private:
	CStateMachine*		m_pStateMachineCom				= nullptr;
	CShader*			m_pShaderCom					= nullptr;
	CRenderer*			m_pRendererCom					= nullptr;
	CModel*				m_pModelCom						= nullptr;
	CCollider*			m_pColliderCom					= nullptr;
	CNavigation*		m_pNavigationCom				= nullptr;
	CStatus*			m_pStatusCom					= nullptr;

	CCollider*			m_pWallFinderCom				= nullptr;

	CTexture*			m_pTextureCom					= nullptr;
	CTexture*			m_pDiffuseTexture[SO_END]		= { nullptr };

	// Blur와 Rim 컨트롤 하는 변수(사용하고 싶을때 true로 변환시켜주면 됨)
	_bool				m_isUsingBlur		= false;
	_bool				m_isUsingRim		= false;

	_float				m_fRimPower = 2.f;
	_float				m_fRimRange = 1.2f;
	_float4				m_vRimColor = _float4(1.f);

	// For Test
	class CEffect_Trail* m_pTrail[4]		= { nullptr };
	_bool				m_bPlay				= false;
	_float				m_fTime				= 0.f;

	// For Effect
	_bool				m_RageFormEffect[3] = {false, false, false};
	_float4				m_vCenterPosition;
	///////////////////////////////////////////////////////

	// 모션 트레일 사용 변수
	_bool				m_bUseMotionTrail = false;

	_bool				m_bTeleportDissolve = false;
	_bool				m_bTP = false;
public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END