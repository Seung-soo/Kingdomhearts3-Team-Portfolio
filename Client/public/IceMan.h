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

class CIceMan final : public CMonster
{
public:
	enum ICEMAN_SKILL // IceMan �� ����� ���� ��ϵ�(�� �� �ϳ��� ���� State �� �ѱ�� �ش� ������ ������)
	{
		ICEMAN_SKILL_WALKSLASH,
		ICEMAN_SKILL_CLAP,
		ICEMAN_SKILL_LONGSLASH,
		ICEMAN_SKILL_JUMPDOWNWARDBLOW,
		ICEMAN_SKILL_END
	};

private:
	CIceMan(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CIceMan(const CIceMan& rhs);
	virtual ~CIceMan() = default;

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
	virtual CCollider*	Get_HitCollider() { return m_pHitColliderCom[2]; } // ������ : �������� ��ŷ������ �����, �ƹ� �ݶ��̴��� return �����ָ� ��

	// Player_Weapon ���� ȣ���ϴ� �Լ�, ���Ͱ� �÷��̾����� �ǰݵ��� �� ó���ؾ��� �����͵��� �� �Լ� �ȿ��� ó��
	virtual _bool		Damaged(CCollider* pAtkCollider, _float fDamaged);

public: // IceMan �� ���� �ھ��� �� m_bCollisionWall ���� ������ �Լ�
	void SetCollisionWall(_bool bOption) { m_bCollisionWall = bOption; }
	ICEMAN_SKILL Get_IceManSkill() { return m_eIceManSkill; }

private: // Collider ���� �Լ�
	void Ready_Collider_Info(); // �ݶ��̴��� ���� ���� ���� �ʱ�ȭ
	void Collider_Transform_Update(); // �ݶ��̴� ��ġ ������Ʈ

private:
	HRESULT SetUp_Components();		// ������Ʈ �ʱ�ȭ
	HRESULT SetUp_RootNodeIndex();	// ��Ʈ��� �ʱ�ȭ(�������� �ִ� �ִϸ��̼ǰ� Transform �� �������� ����ȭ�ϱ� ����)
	HRESULT SetUp_Animations();		// �ִϸ��̼� �ʱ�ȭ
	HRESULT SetUp_ConstantTable();

private:
	void OnCollision(_double TimeDelta); // �浹 ó�� �Լ�
	void ChangeState(_double TimeDelta); // ���� ���� �Լ�

private:
	CStateMachine*	m_pStateMachineCom  = nullptr;
	CShader*		m_pShaderCom        = nullptr;
	CRenderer*		m_pRendererCom      = nullptr;
	CModel*			m_pModelCom         = nullptr;
	CStatus*		m_pStatusCom        = nullptr;
	CMonsterCooler* m_pMonsterCoolerCom = nullptr;
	CNavigation*	m_pNavigationCom    = nullptr;

	// ������ �ݶ��̴��� �� ����(���� �ݶ��̴�, �ǰ� �ݶ��̴�)�� �������� ����
	CCollider*		m_pHitColliderCom[4] = { nullptr, }; // �ǰ� �ݶ��̴�
	CCollider*		m_pAtkColliderCom[2] = { nullptr, }; // ���� �ݶ��̴�

	// �ǰ� �ִϸ��̼��� �Ź� ����ϸ� �̻��ϴϱ� �ǰ� �ִϸ��̼� ����� ��Ÿ���� �ֱ� ���� ���� ����
	_double			m_OnDamagedAnimTimeAcc = 0.0;

	ICEMAN_SKILL m_eIceManSkill = ICEMAN_SKILL_END; // ���� ���¸� ������ �� � ������ �ϰ� ������ �����ϴ� ����

	ICEMAN_SKILL m_eSpecialSkill     = ICEMAN_SKILL_END;
	ICEMAN_SKILL m_ePrevSpecialSkill = ICEMAN_SKILL_JUMPDOWNWARDBLOW;

	_bool m_bCollisionWall = false; // ���̶� �浹�ߴ��� �Ǻ�

	_bool m_bOnlyIdle = false; // ������ (������ Idle ���·� �����)

private: // �ݶ��̴� ���� ����
	// �ǰ�
	CModel::BONEMATRIX_PTR	m_HitCollider_BoneMatrixPtr[4];
	_float4x4				m_HitCollider_SocketPivotmMatrix[4];
	_matrix					m_HitCollider_TransformMatrix[4];
	// ����
	CModel::BONEMATRIX_PTR	m_AtkCollider_BoneMatrixPtr[2];
	_float4x4				m_AtkCollider_SocketPivotmMatrix[2];
	_matrix					m_AtkCollider_TransformMatrix[2];

public:
	static	CIceMan* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END