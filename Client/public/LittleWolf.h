#pragma once

#include "Client_Defines.h"
#include "MonsterDefines.h"
#include "Monster.h"
#include "Model.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CStateMachine;
class CCollider;
class CStatus;
class CMonsterCooler;
class CNavigation;
END

class CLittleWolf final : public CMonster
{
public:
	typedef struct tagWOLFDESC
	{
		_vector		vStartPos;
		_vector		vStartDir;
		_bool		bRain = false;
	}WOLFDESC;

private:
	CLittleWolf(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CLittleWolf(const CLittleWolf& rhs);
	virtual ~CLittleWolf() = default;

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
	virtual CCollider*	Get_HitCollider() { return m_pHitColliderCom; } // ������ : �������� ��ŷ������ �����, �ƹ� �ݶ��̴��� return �����ָ� ��

	// Player_Weapon ���� ȣ���ϴ� �Լ�, ���Ͱ� �÷��̾����� �ǰݵ��� �� ó���ؾ��� �����͵��� �� �Լ� �ȿ��� ó��
	virtual _bool		Damaged(CCollider* pAtkCollider, _float fDamaged);


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

public:
	static	CLittleWolf* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

private:
	CStateMachine*	m_pStateMachineCom     = nullptr;
	CShader*		m_pShaderCom           = nullptr;
	CRenderer*		m_pRendererCom         = nullptr;
	CModel*			m_pModelCom            = nullptr;
	CStatus*		m_pStatusCom           = nullptr;
	CMonsterCooler* m_pMonsterCoolerCom    = nullptr;
	CNavigation*	m_pNavigationCom       = nullptr;


	// ������ �ݶ��̴��� �� ����(���� �ݶ��̴�, �ǰ� �ݶ��̴�)�� �������� ����
	CCollider* m_pHitColliderCom = nullptr; // �ǰ� �ݶ��̴�
	CCollider* m_pAtkColliderCom = nullptr; // ���� �ݶ��̴�

	// �ǰ� �ִϸ��̼��� �Ź� ����ϸ� �̻��ϴϱ� �ǰ� �ִϸ��̼� ����� ��Ÿ���� �ֱ� ���� ���� ����
	_double		m_OnDamagedAnimTimeAcc = 0.0;

	_bool		m_bOnlyIdle = false; // ������ (������ Idle ���·� �����)

	_vector		m_vFlyUpDir;

	_vector		m_vTargetPos;

	_bool		m_bReadyToAttack = false;

	_double		m_fCoolTime = 0.5;

	_double		m_TimeAccum = 0.0;

	_float		m_fAnimOriginSpeed = 0.f;

	_bool		m_isBiting = false;

	_bool		m_isTurnFinish = false;
	_bool		m_isClosed = false;

	_bool		m_bRain = false;

	_double		m_TimeAccum_Life = 0.0;

	_double		m_TimeAccum_Turn = 0.0;
	_double		m_TimeAccum_ChasePlayer = 0.0;

	_bool		m_isChasingPlayer = false;
	_bool		m_isFocusingOnPlayer = false;

private: // �ݶ��̴� ���� ����
	// �ǰ�
	CModel::BONEMATRIX_PTR	m_HitCollider_BoneMatrixPtr;
	_float4x4				m_HitCollider_SocketPivotMatrix;
	_matrix					m_HitCollider_SocketMatrix;
	_matrix					m_HitCollider_TransformMatrix;

	// ����
	CModel::BONEMATRIX_PTR	m_AtkCollider_BoneMatrixPtr;
	_float4x4				m_AtkCollider_SocketPivotMatrix;
	_matrix					m_AtkCollider_SocketMatrix;
	_matrix					m_AtkCollider_TransformMatrix;
};

