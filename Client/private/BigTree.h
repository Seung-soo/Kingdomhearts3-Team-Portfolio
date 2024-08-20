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
class CNavigation;
END

BEGIN(Client)

class CBigTree final : public CMonster
{
public:
	enum BIGTREE_SKILL
	{
		BTSKILL_SUMMONCROW,
		BTSKILL_HUG,
		BTSKILL_GROUNDHEADBUTT,
		BTSKILL_POWERHEADBUTT,
		BTSKILL_SCREWDASH,
		BTSKILL_BUILDINGCLIMB,
		BTSKILL_SUMMONFRUITS,
		BTSKILL_END
	};

private:
	CBigTree(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBigTree(const CBigTree& rhs);
	virtual ~CBigTree() = default;

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
	virtual CCollider*	Get_HitCollider() { return m_pHitColliderCom[0]; }
	virtual _bool		Damaged(CCollider* pAtkCollider, _float fDamaged);

public:
	_float4*		Get_EffectBonePos()			{ return m_pEffectBone_Pos; }
	_float4x4*		Get_EffectBoneWorldMatrix() { return m_pEffectBone_WorldMatrix; }

public:
	void ResetAtkCool() { m_bCanAtk = true; m_AtkCool = 0.0; }

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
	CStateMachine*		m_pStateMachineCom  = nullptr;
	CShader*			m_pShaderCom        = nullptr;
	CRenderer*			m_pRendererCom      = nullptr;
	CModel*				m_pModelCom         = nullptr;
	CStatus*			m_pStatusCom        = nullptr;
	CNavigation*		m_pNavigationCom    = nullptr;

	CCollider*			m_pHitColliderCom[4] = { nullptr, };
	CCollider*			m_pAtkColliderCom[5] = { nullptr, };

	_double				m_OnDamagedAnimTimeAcc = 0.0;					// �ǰ� �ִϸ��̼� ��� ��Ÿ��
	BIGTREE_SKILL		m_eSkill               = BTSKILL_SUMMONCROW;	// ���� Atk ���� �� ����� ��ų
	BIGTREE_SKILL		m_ePrevSkill           = BTSKILL_END;

	_double m_AtkCool = 0.0;
	_bool	m_bCanAtk = true;

	class CTerrain*		m_pCurLevelTerrain = nullptr;

private: // �ݶ��̴� ���� ����
	// �ǰ�
	CModel::BONEMATRIX_PTR	m_HitCollider_BoneMatrixPtr[4];			// Collider �� ���� "��"�� ����(������, �Ĺ��ε�)
	_float4x4				m_HitCollider_SocketPivotmMatrix[4];	// Collider �� ���� ���� "�ǹ�"��Ʈ����
	_matrix					m_HitCollider_TransformMatrix[4];		// Collider �� "����"������ ������ ��Ʈ����
	_float4					m_pEffectBone_Pos[4];					// Effect_Manager �� ����
	_float4x4				m_pEffectBone_WorldMatrix[4];			// Effect_Manager �� ����
	// ����
	CModel::BONEMATRIX_PTR	m_AtkCollider_BoneMatrixPtr[5];
	_float4x4				m_AtkCollider_SocketPivotmMatrix[5];
	_matrix					m_AtkCollider_TransformMatrix[5];

public:
	static	CBigTree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END