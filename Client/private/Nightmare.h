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

BEGIN(Client)

class CNightmare final : public CMonster
{
private:
	CNightmare(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CNightmare(const CNightmare& rhs);
	virtual ~CNightmare() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual HRESULT NativeConstruct_Pool(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow() override;
	virtual void	PostRender(_double TimeDelta);

public:
	virtual HRESULT		Save_ObjectData(HANDLE hFile) override;
	virtual HRESULT		Load_ObjectData(HANDLE hFile) override;
	virtual CCollider*	Get_HitCollider() { return m_pHitColliderCom; }
	virtual _bool		Damaged(CCollider* pAtkCollider, _float fDamaged);

private: // Collider ���� �Լ�
	void Ready_Collider_Info();
	void Collider_Transform_Update();

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RootNodeIndex();
	HRESULT SetUp_Animations();
	HRESULT SetUp_ConstantTable();

private:
	void OnCollision(_double TimeDelta); // �浹 ó�� �Լ�
	void ChangeState(_double TimeDelta); // ���� ���� �Լ�

private:
	CStateMachine*	m_pStateMachineCom    = nullptr;
	CShader*		m_pShaderCom          = nullptr;
	CRenderer*		m_pRendererCom        = nullptr;
	CModel*			m_pModelCom           = nullptr;
	CStatus*		m_pStatusCom          = nullptr;
	CMonsterCooler* m_pMonsterCoolerCom   = nullptr;
	CNavigation*	m_pNavigationCom      = nullptr;

	CCollider*		m_pHitColliderCom     = nullptr;
	CCollider*		m_pAtkColliderCom[4]  = { nullptr, };

	_double			m_OnDamagedAnimTimeAcc = 0.0;	// �ǰ� �ִϸ��̼� ��� ��Ÿ��

	_double			m_MotionTrailInterval = 0.0;

private: // �ݶ��̴� ���� ����
	// �ǰ�
	CModel::BONEMATRIX_PTR	m_HitCollider_BoneMatrixPtr;		// Collider �� ���� "��"�� ����(������, �Ĺ��ε�)
	_float4x4				m_HitCollider_SocketPivotmMatrix;	// Collider �� ���� ���� "�ǹ�"��Ʈ����
	_matrix					m_HitCollider_SocketMatrix;			// Collider �� ���� "��"�� ���� ��Ʈ����(������ * �Ĺ��ε� * �ǹ�)
	_matrix					m_HitCollider_TransformMatrix;		// Collider �� "����"������ ������ ��Ʈ����
	// ����
	CModel::BONEMATRIX_PTR	m_AtkCollider_BoneMatrixPtr[4];
	_float4x4				m_AtkCollider_SocketPivotmMatrix[4];
	_matrix					m_AtkCollider_SocketMatrix[4];
	_matrix					m_AtkCollider_TransformMatrix[4];
	
public:
	static	CNightmare* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END