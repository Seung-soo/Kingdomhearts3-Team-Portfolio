#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Model.h"
#include "Effect_VISwordTrail.h"
#include "MonsterDefines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CCollider;
class CStatus;
class CStateMachine;
END

BEGIN(Client)

class CPlayer;

class CPlayer_Weapon final : public CGameObject
{
public:
	enum WEAPON_TYPE
	{
		WEAPON_TYPE_KINGDOM_KEY,			// 킹덤 키
		WEAPON_TYPE_FAVORITE_DEPUTY,		// 페버디퓨티
		WEAPON_TYPE_HYPER_HAMMER,			// 하이퍼해머
		WEAPON_TYPE_HYPER_HAMMER_CHANGING,	// 하이퍼해머 변신 중 나오는 무기
		WEAPON_TYPE_END
	};

	enum WEAPON_STATE
	{
		WEAPON_STATE_DRAW,					// 무기 뽑은 상태
		WEAPON_STATE_SHEATHE,				// 무기 집어넣은 상태
		WEAPON_STATE_END
	};

	enum RAGE_COLLIDER
	{
		RAGE_COLLIDER_BLADE,				// 검
		RAGE_COLLIDER_NAIL,					// 왼손
		RAGE_COLLIDER_FRONT,				// 전방에 대한 난격 또는 리스크차지
		RAGE_COLLIDER_AROUND,				// 전방위 무분별 공격
		RAGE_COLLIDER_FINISH,				// 마무리 공격
		RAGE_COLLIDER_BLADEAREA,			// 카운터 공격
		RAGE_COLLIDER_END
	};

	enum HAMMER_COLLIDER
	{
		HAMMER_COLLIDER_HEAD,				// 해머 헤드 부위 타격을 판정하는 충돌체
		HAMMER_COLLIDER_WAVE,				// 지상 공격시 충격파 판정하는 충돌체
		HAMMER_COLLIDER_LAST,				// 콤보의 라스트 어택 판정하는 충돌체
		HAMMER_COLLIDER_FINISH,				// 해머 폼 피니시 어택
		HAMMER_COLLIDER_END
	};

	enum HAMMER_SPOT
	{
		HAMMER_SPOT_HEAD,					// 해머 헤드의 앞 부분
		HAMMER_SPOT_BODY,					// 해머 헤드의 몸통 부분
		HAMMER_SPOT_BOOSTER,				// 해머 헤드의 부스터 부분
		HAMMER_SPOT_END
	};

public:
	CPlayer_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPlayer_Weapon(const CPlayer_Weapon& rhs);
	virtual ~CPlayer_Weapon() = default;

public:
	WEAPON_TYPE	WeaponType() { return m_eWeaponType; }
	void WeaponType(WEAPON_TYPE eWeaponType) { m_eWeaponType = eWeaponType; }

	WEAPON_STATE WeaponState() { return m_eWeaponState; }
	void WeaponState(WEAPON_STATE eWeaponState) { m_eWeaponState = eWeaponState; }

	_bool RegisterCollider() { return m_bRegisterCollider; }
	void RegisterCollider(_bool bRegister) { m_bRegisterCollider = bRegister; }

	_bool RegisterCollider_Hammer(HAMMER_COLLIDER eHammerEnum) { return m_bRegisterCollider_Hammer[eHammerEnum]; }
	void RegisterCollider_Hammer(HAMMER_COLLIDER eHammerEnum, _bool bRegister) { m_bRegisterCollider_Hammer[eHammerEnum] = bRegister; }

	_bool RegisterCollider_Rage(RAGE_COLLIDER eRageEnum) { return m_bRegisterCollider_Rage[eRageEnum]; }
	void RegisterCollider_Rage(RAGE_COLLIDER eRageEnum, _bool bRegister) { m_bRegisterCollider_Rage[eRageEnum] = bRegister; }

	_bool Render_All_Colliders() { return m_bRender_All_Colliders; }
	void Render_All_Colliders(_bool bRender) { m_bRender_All_Colliders = bRender; }

	_float Get_KnockBackPower() { return m_fKnockBackPower; }
	void Set_KnockBackPower(_float fPower) { m_fKnockBackPower = fPower; }

	_double Get_KnockBackTimer() { return m_dKnockBackTimer; }
	void Set_KnockBackTimer(_double dTimer) { m_dKnockBackTimer = dTimer; }

	_float4* Rage_Collider_Position(RAGE_COLLIDER eRageEnum) { return &m_vRageCollider_Position[eRageEnum]; }
	_float4x4 Rage_Collider_Front_Matrix() { return m_RageCollider_Front_Matrix; }

	// ImGuI
	void Plus_WeaponType();

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void	PostRender(_double TimeDelta);
	virtual HRESULT Render_Shadow() override;

	void Set_Trail_Active(_bool isActive) { m_pTrail->Set_Active(isActive); }

private:
	// 플레이어
	CPlayer* m_pPlayer = nullptr;

	// 플레이어 컴포넌트
	CStateMachine*		m_pPlayerStateMachine = nullptr;
	CTransform*			m_pPlayerTransform = nullptr;
	CModel*				m_pPlayerModel = nullptr;

	// 플레이어 모델 피벗
	_float4x4 m_PlayerModelTransformMatrix; 

	// 클래스 변수 =============================================================

	WEAPON_TYPE m_eWeaponType = WEAPON_TYPE::WEAPON_TYPE_END;
	WEAPON_STATE m_eWeaponState = WEAPON_STATE::WEAPON_STATE_END;
	const _tchar* m_pModelComponentTag[WEAPON_TYPE_END] = { nullptr };

	// 중복 검사를 위한 객체를 받아둘 stl
	vector<CGameObject*> m_CollisionParents;
	vector<CGameObject*> m_CollisionParents_Hammer[HAMMER_COLLIDER_END];
	vector<CGameObject*> m_CollisionParents_Rage[RAGE_COLLIDER_END];

	// 해머 회전값
	_float m_fRotationValue = 150.f; 

	// ImGuI
	_bool m_bRender_All_Colliders = false; 

	// 넉백 관련 변수
	_float m_fKnockBackPower = 5.f;
	_double m_dKnockBackTimer = 0.25;




	// Normal Form 콜라이더 변수들 =============================================

	// 콜라이더를 붙일 Bone과 관련된 행렬정보
	CModel::BONEMATRIX_PTR m_BoneMatrixPtr_Normal;

	// 콜라이더 등록 여부
	_bool m_bRegisterCollider = false;
	
	// NormalCollider Local 상태 설정 변수
	_float m_fNormalCollider_Scale;
	_float4 m_fNormalCollider_Position;

	// NormalCollider 컴포넌트 태그
	const _tchar* m_pNormalCollider_ComponentTag = nullptr;




	// Hammer Form 콜라이더 변수들 =============================================

	// 콜라이더를 붙일 Bone과 관련된 행렬정보
	CModel::BONEMATRIX_PTR m_BoneMatrixPtr_Hammer[RAGE_COLLIDER_END];

	// 콜라이더 등록 여부
	_bool m_bRegisterCollider_Hammer[HAMMER_COLLIDER_END];

	// HammerCollider Local 상태 설정 변수
	_float m_fHammerCollider_LocalScale[HAMMER_COLLIDER_END];
	_float4 m_vHammerCollider_LocalPosition[HAMMER_COLLIDER_END];

	// HammerCollider 컴포넌트 태그
	const _tchar* m_pHammerCollider_ComponentTag[HAMMER_COLLIDER_END] = { nullptr };

	// -------------------------------------------------------------------------

	/*
		이펙트를 출력하기 위한 위치를 잡기 편하게 콜라이더를 사용하기로 결정,
		이하는 해당 콜라이더와 관련된 변수들.
	*/

	// 콜라이더를 붙일 Bone과 관련된 행렬 정보
	CModel::BONEMATRIX_PTR m_BoneMatrixPtr_Hammer_HeadPoint[HAMMER_SPOT_END];

	// HeadPointCollider Local 상태 설정 변수 -> 회전 값이 추가되어야할 수 있다.
	_float m_fHammer_HeadPoint_LocalScale;
	_float4 m_vHammer_HeadPoint_LocalPosition[HAMMER_SPOT_END];

	// HeadPointCollider 컴포넌트 태그
	const _tchar* m_pHammer_HeadPoint_ComponentTag[HAMMER_SPOT_END] = { nullptr };

	// HeadPointCollider 의 행렬연산이 끝난 위치, 행렬을 저장하기 위한 변수들
	_float4 m_vHammer_HeadPoint_Position[HAMMER_SPOT_END];
	_float4x4 m_Hammer_HeadPoint_Matrix[HAMMER_SPOT_END];


	// Rage Form 콜라이더 변수들 ===============================================

	// 콜라이더를 붙일 Bone과 관련된 행렬 정보
	CModel::BONEMATRIX_PTR m_BoneMatrixPtr_Rage[RAGE_COLLIDER_END];

	// 콜라이더 등록 여부
	_bool m_bRegisterCollider_Rage[RAGE_COLLIDER_END];

	// RageCollider Local 상태 설정 변수
	_float m_fRageCollider_LocalScale[RAGE_COLLIDER_END];

	// RageCollider Local 컴포넌트 태그
	const _tchar* m_pRageCollider_ComponentTag[RAGE_COLLIDER_END] = { nullptr };

	_float4 m_vRageCollider_Position[RAGE_COLLIDER_END];
	_float4x4 m_RageCollider_Front_Matrix;

	// 렌더링 기법 관련 변수
	_bool m_isUsingBlur = false;
	_bool m_isUsingRim = false;

	_float m_fRimPower = 0.f;
	_float m_fRimRange = 0.f;
	_float4 m_vRimColor = _float4(1.f);
	
	// 연출용 변수
	_bool m_bAppear = false;
	_double m_dTimer_Control_RimBrightness = 0.0;
	_double m_dTimer_Control_RimBrightness_Max = 0.0;

public:
	void Change_Animation(_uint iIndex);
	void Reset_Animation();

	_vector Calculate_Point_On_Ground(_uint iColliderKind, _uint iNumber);

	CGameObject* Create_Effect_HammerEmbers(); // 시간 조절할 수도 있음
	HRESULT Create_Effect_HammerSwing(); // 시간 조절할 수도 있음
	HRESULT Create_Effect_AirHit();
	HRESULT Create_Effect_AirCut();

	void Clear_All_Colliding_Data();
	void Appear_Hammer();

	CStatus* Get_StatusCom() { return m_pStatusCom; }

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable(_bool* pAnim);

private:
	void OnCollision(CCollider* pCollider);
	void Decide_KnockBack(CGameObject* pMonster, MONSTERTYPE eMonsterType);

	_bool Check_Anim_NonAnim(); // 현재 무기가 AnimModel(true)인지 NonAnimModel(false)인지 확인하는 함수

private:
	CModel*					m_pModelCom[WEAPON_TYPE_END] = { nullptr };
	CShader*				m_pShaderCom_Anim = nullptr;
	CShader*				m_pShaderCom_NonAnim = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CStatus*				m_pStatusCom = nullptr;

	CCollider*				m_pCollider_RageForm[RAGE_COLLIDER_END] = { nullptr };
	CCollider*				m_pCollider_HammerForm[HAMMER_COLLIDER_END] = { nullptr };
	CCollider*				m_pCollider_NormalAttack = nullptr;

	CCollider*				m_pCollider_HammerHeadPoint[HAMMER_SPOT_END] = { nullptr };

	//Trail
	CEffect_VISwordTrail*	m_pTrail		 = nullptr;

	// RageForm Texture
	CTexture*				m_pRageFormTex	 = nullptr;

private:
	// Motion Trail
	_bool					m_bUseMotionTrail = false;
public:

	static CPlayer_Weapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END