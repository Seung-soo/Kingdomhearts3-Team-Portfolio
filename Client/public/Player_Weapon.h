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
		WEAPON_TYPE_KINGDOM_KEY,			// ŷ�� Ű
		WEAPON_TYPE_FAVORITE_DEPUTY,		// �����ǻƼ
		WEAPON_TYPE_HYPER_HAMMER,			// �������ظ�
		WEAPON_TYPE_HYPER_HAMMER_CHANGING,	// �������ظ� ���� �� ������ ����
		WEAPON_TYPE_END
	};

	enum WEAPON_STATE
	{
		WEAPON_STATE_DRAW,					// ���� ���� ����
		WEAPON_STATE_SHEATHE,				// ���� ������� ����
		WEAPON_STATE_END
	};

	enum RAGE_COLLIDER
	{
		RAGE_COLLIDER_BLADE,				// ��
		RAGE_COLLIDER_NAIL,					// �޼�
		RAGE_COLLIDER_FRONT,				// ���濡 ���� ���� �Ǵ� ����ũ����
		RAGE_COLLIDER_AROUND,				// ������ ���к� ����
		RAGE_COLLIDER_FINISH,				// ������ ����
		RAGE_COLLIDER_BLADEAREA,			// ī���� ����
		RAGE_COLLIDER_END
	};

	enum HAMMER_COLLIDER
	{
		HAMMER_COLLIDER_HEAD,				// �ظ� ��� ���� Ÿ���� �����ϴ� �浹ü
		HAMMER_COLLIDER_WAVE,				// ���� ���ݽ� ����� �����ϴ� �浹ü
		HAMMER_COLLIDER_LAST,				// �޺��� ��Ʈ ���� �����ϴ� �浹ü
		HAMMER_COLLIDER_FINISH,				// �ظ� �� �ǴϽ� ����
		HAMMER_COLLIDER_END
	};

	enum HAMMER_SPOT
	{
		HAMMER_SPOT_HEAD,					// �ظ� ����� �� �κ�
		HAMMER_SPOT_BODY,					// �ظ� ����� ���� �κ�
		HAMMER_SPOT_BOOSTER,				// �ظ� ����� �ν��� �κ�
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
	// �÷��̾�
	CPlayer* m_pPlayer = nullptr;

	// �÷��̾� ������Ʈ
	CStateMachine*		m_pPlayerStateMachine = nullptr;
	CTransform*			m_pPlayerTransform = nullptr;
	CModel*				m_pPlayerModel = nullptr;

	// �÷��̾� �� �ǹ�
	_float4x4 m_PlayerModelTransformMatrix; 

	// Ŭ���� ���� =============================================================

	WEAPON_TYPE m_eWeaponType = WEAPON_TYPE::WEAPON_TYPE_END;
	WEAPON_STATE m_eWeaponState = WEAPON_STATE::WEAPON_STATE_END;
	const _tchar* m_pModelComponentTag[WEAPON_TYPE_END] = { nullptr };

	// �ߺ� �˻縦 ���� ��ü�� �޾Ƶ� stl
	vector<CGameObject*> m_CollisionParents;
	vector<CGameObject*> m_CollisionParents_Hammer[HAMMER_COLLIDER_END];
	vector<CGameObject*> m_CollisionParents_Rage[RAGE_COLLIDER_END];

	// �ظ� ȸ����
	_float m_fRotationValue = 150.f; 

	// ImGuI
	_bool m_bRender_All_Colliders = false; 

	// �˹� ���� ����
	_float m_fKnockBackPower = 5.f;
	_double m_dKnockBackTimer = 0.25;




	// Normal Form �ݶ��̴� ������ =============================================

	// �ݶ��̴��� ���� Bone�� ���õ� �������
	CModel::BONEMATRIX_PTR m_BoneMatrixPtr_Normal;

	// �ݶ��̴� ��� ����
	_bool m_bRegisterCollider = false;
	
	// NormalCollider Local ���� ���� ����
	_float m_fNormalCollider_Scale;
	_float4 m_fNormalCollider_Position;

	// NormalCollider ������Ʈ �±�
	const _tchar* m_pNormalCollider_ComponentTag = nullptr;




	// Hammer Form �ݶ��̴� ������ =============================================

	// �ݶ��̴��� ���� Bone�� ���õ� �������
	CModel::BONEMATRIX_PTR m_BoneMatrixPtr_Hammer[RAGE_COLLIDER_END];

	// �ݶ��̴� ��� ����
	_bool m_bRegisterCollider_Hammer[HAMMER_COLLIDER_END];

	// HammerCollider Local ���� ���� ����
	_float m_fHammerCollider_LocalScale[HAMMER_COLLIDER_END];
	_float4 m_vHammerCollider_LocalPosition[HAMMER_COLLIDER_END];

	// HammerCollider ������Ʈ �±�
	const _tchar* m_pHammerCollider_ComponentTag[HAMMER_COLLIDER_END] = { nullptr };

	// -------------------------------------------------------------------------

	/*
		����Ʈ�� ����ϱ� ���� ��ġ�� ��� ���ϰ� �ݶ��̴��� ����ϱ�� ����,
		���ϴ� �ش� �ݶ��̴��� ���õ� ������.
	*/

	// �ݶ��̴��� ���� Bone�� ���õ� ��� ����
	CModel::BONEMATRIX_PTR m_BoneMatrixPtr_Hammer_HeadPoint[HAMMER_SPOT_END];

	// HeadPointCollider Local ���� ���� ���� -> ȸ�� ���� �߰��Ǿ���� �� �ִ�.
	_float m_fHammer_HeadPoint_LocalScale;
	_float4 m_vHammer_HeadPoint_LocalPosition[HAMMER_SPOT_END];

	// HeadPointCollider ������Ʈ �±�
	const _tchar* m_pHammer_HeadPoint_ComponentTag[HAMMER_SPOT_END] = { nullptr };

	// HeadPointCollider �� ��Ŀ����� ���� ��ġ, ����� �����ϱ� ���� ������
	_float4 m_vHammer_HeadPoint_Position[HAMMER_SPOT_END];
	_float4x4 m_Hammer_HeadPoint_Matrix[HAMMER_SPOT_END];


	// Rage Form �ݶ��̴� ������ ===============================================

	// �ݶ��̴��� ���� Bone�� ���õ� ��� ����
	CModel::BONEMATRIX_PTR m_BoneMatrixPtr_Rage[RAGE_COLLIDER_END];

	// �ݶ��̴� ��� ����
	_bool m_bRegisterCollider_Rage[RAGE_COLLIDER_END];

	// RageCollider Local ���� ���� ����
	_float m_fRageCollider_LocalScale[RAGE_COLLIDER_END];

	// RageCollider Local ������Ʈ �±�
	const _tchar* m_pRageCollider_ComponentTag[RAGE_COLLIDER_END] = { nullptr };

	_float4 m_vRageCollider_Position[RAGE_COLLIDER_END];
	_float4x4 m_RageCollider_Front_Matrix;

	// ������ ��� ���� ����
	_bool m_isUsingBlur = false;
	_bool m_isUsingRim = false;

	_float m_fRimPower = 0.f;
	_float m_fRimRange = 0.f;
	_float4 m_vRimColor = _float4(1.f);
	
	// ����� ����
	_bool m_bAppear = false;
	_double m_dTimer_Control_RimBrightness = 0.0;
	_double m_dTimer_Control_RimBrightness_Max = 0.0;

public:
	void Change_Animation(_uint iIndex);
	void Reset_Animation();

	_vector Calculate_Point_On_Ground(_uint iColliderKind, _uint iNumber);

	CGameObject* Create_Effect_HammerEmbers(); // �ð� ������ ���� ����
	HRESULT Create_Effect_HammerSwing(); // �ð� ������ ���� ����
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

	_bool Check_Anim_NonAnim(); // ���� ���Ⱑ AnimModel(true)���� NonAnimModel(false)���� Ȯ���ϴ� �Լ�

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