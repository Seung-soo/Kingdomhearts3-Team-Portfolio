#pragma once

#include "Client_Defines.h"
#include "MonsterDefines.h"
#include "Monster.h"
#include "Model.h"
#include "Effect_Trail.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CStateMachine;
class CCollider;
class CStatus;
class CMonsterCooler;
class CNavigation;
END

class CFrozenWolf final : public CMonster
{
public:
	enum WOLF_SKILL
	{
		WOLF_DASH,
		WOLF_SCRATCH,
		WOLF_SUMMON,
		WOLF_THROWFIRE,
		WOLF_SPINATTACK,
		WOLF_PURPLERAIN,
		WOLF_SKILL_END
	};

private:
	CFrozenWolf(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CFrozenWolf(const CFrozenWolf& rhs);
	virtual ~CFrozenWolf() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual HRESULT NativeConstruct_Pool(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow() override;
	virtual void	PostRender(_double TimeDelta); // ImGui ������ (GameObject �� ���ǵ� m_bImGuiController �� true �� �׻� ����, false �� ImGui â���� ���� �������)

public:
	virtual HRESULT		Save_ObjectData(HANDLE hFile) override; // ������ : ������ �����Ʈ������ �����صα� ���� �ʿ�(�����ϸ� ��)
	virtual HRESULT		Load_ObjectData(HANDLE hFile) override; // ������ : ������ �����Ʈ������ �ε��ϱ� ���� �ʿ�(�����ϸ� ��)
	virtual CCollider*  Get_HitCollider() { return m_pHitColliderCom[0]; } // ������ : �������� ��ŷ������ �����, �ƹ� �ݶ��̴��� return �����ָ� ��

	// Player_Weapon ���� ȣ���ϴ� �Լ�, ���Ͱ� �÷��̾����� �ǰݵ��� �� ó���ؾ��� �����͵��� �� �Լ� �ȿ��� ó��
	virtual _bool		Damaged(CCollider* pAtkCollider, _float fDamaged);
	

	void				Set_GravitySwitch(_bool isOn) { m_GravitySwitch = isOn; }
	void				Set_Force(_float fForce) { m_fForce = fForce; }

	_matrix				Get_AtkBoneMatrix(_uint iIndex = 2);
	_float4*			Get_BonePos_Pointer(_uint iIndex);
	_float4x4*			Get_CenterBoneMatrix();

	_vector				Get_TailPos();

	void				MoveToNextSkill();

	void				Set_TrailOn(_bool isActive);

	void				Set_MotionTrail(_bool OnOff) {m_bUseMotionTrail = OnOff;}


private: // Collider ���� �Լ�
	HRESULT Ready_Collider_Info(); // �ݶ��̴��� ���� ���� ���� �ʱ�ȭ
	HRESULT	Collider_Transform_Update(); // �ݶ��̴� ��ġ ������Ʈ

private:
	HRESULT SetUp_Components();		// ������Ʈ �ʱ�ȭ
	HRESULT SetUp_RootNodeIndex();	// ��Ʈ��� �ʱ�ȭ(�������� �ִ� �ִϸ��̼ǰ� Transform �� �������� ����ȭ�ϱ� ����)
	HRESULT SetUp_Animations();		// �ִϸ��̼� �ʱ�ȭ
	HRESULT SetUp_ConstantTable();

private:
	HRESULT OnCollision(_double TimeDelta); // �浹 ó�� �Լ�
	HRESULT ChangeState(_double TimeDelta); // ���� ���� �Լ�

	void    Gravity(_double TimeDelta);

	void	Change_AttackPattern();


public:
	static	CFrozenWolf* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

private:
	CStateMachine* m_pStateMachineCom   = nullptr;
	CShader* m_pShaderCom               = nullptr;
	CRenderer* m_pRendererCom           = nullptr;
	CModel* m_pModelCom                 = nullptr;
	CStatus* m_pStatusCom               = nullptr;
	CMonsterCooler* m_pMonsterCoolerCom = nullptr;
	CNavigation* m_pNavigationCom       = nullptr;

	WOLF_SKILL	 m_eWolfSkill = WOLF_SKILL_END;

	// ������ �ݶ��̴��� �� ����(���� �ݶ��̴�, �ǰ� �ݶ��̴�)�� �������� ����
	CCollider* m_pHitColliderCom[3] = { nullptr, }; // �ǰ� �ݶ��̴�
	CCollider* m_pAtkColliderCom[3] = { nullptr, }; // ���� �ݶ��̴�

	// �ǰ� �ִϸ��̼��� �Ź� ����ϸ� �̻��ϴϱ� �ǰ� �ִϸ��̼� ����� ��Ÿ���� �ֱ� ���� ���� ����
	_double	   m_OnDamagedAnimTimeAcc = 0.0;

	_bool	   m_bOnlyIdle = false; // ������ (������ Idle ���·� �����)

	_bool	   m_GravitySwitch = false;

	_float	   m_fForce = 0.f;
	_float	   m_fGravity = 0.5f;

	_double	   m_CoolTime = 0.0;

	WOLF_SKILL m_SkillSequence[10];

	_uint		m_iCurSkill = 0;

	_vector     m_vTailPos;

	_uint		m_iHitCnt = 0;

	_float4     m_vBonePos[3];

	_bool		m_isWaiting = true;

	_double		m_TimeAccum = 0.0;

	_double		m_TimeAccum_MotionTrail = 0.0;

	_float		m_fMoveUV = 0.f;
	_float		m_fMoveUVSpeed = 0.1f;
	_float		m_fNoiseX = 0.f;
	_float		m_fNoiseXSpeed = 0.f;
	_float		m_fNoiseY = 0.f;
	_float		m_fNoiseYSpeed = 0.1f;
	_float		m_fNoisePower = -0.26f;

	_int		m_iTexIndex = 99;
private: // �ݶ��̴� ���� ����
	// �ǰ�
	CModel::BONEMATRIX_PTR	m_HitCollider_BoneMatrixPtr[3];
	_float4x4				m_HitCollider_SocketPivotMatrix[3];
	_matrix					m_HitCollider_TransformMatrix[3];

	// ����
	CModel::BONEMATRIX_PTR	m_AtkCollider_BoneMatrixPtr[3];
	_float4x4				m_AtkCollider_SocketPivotMatrix[3];
	_matrix					m_AtkCollider_TransformMatrix[3];

	// Center
	CModel::BONEMATRIX_PTR	m_Center_BoneMatrixPtr;

	_float4x4				m_Center_TransformMatrix;

	CEffect_Trail*			m_pTrail[5] = { nullptr, };

	_bool					m_bSpecialMove[2] = {false, };


};

