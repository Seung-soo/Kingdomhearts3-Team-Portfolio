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

class CGoat final : public CMonster
{
public:
	enum GOAT_ATK
	{
		GOAT_ATK_SPEAR,					// ���������� ��� ����
		GOAT_ATK_TELEPORTSPEAR,			// (�����̵�) ���������� ��� ����
		GOAT_ATK_CONSECUTIVE_SPEAR,		// (�����̵�) ��� �����̵� �ϸ鼭 ���� ��� ����
		GOAT_ATK_FLYSWING,				// ���� ��¦ ���Ҵٰ� �밢�� �Ʒ��� ���� ����
		GOAT_ATK_TELEPORTFLYSWING,		// (�����̵�) ���� ��¦ ���Ҵٰ� �밢�� �Ʒ��� ���� ����
		GOAT_ATK_DASHSWING,				// ���������� ���Ҵٰ� ���� ����
		GOAT_ATK_TELEPORTDASHSWING,		// (�����̵�) ���������� ���Ҵٰ� ���� ����
		GOAT_ATK_SPINDASHSWING,			// Į �����鼭 ���ƿ��ٰ� �������� ũ�� ����
		GOAT_ATK_TELEPORTSPINDASHSWING,	// (�����̵�) Į �����鼭 ���ƿ��ٰ� �������� ũ�� ����
		GOAT_ATK_JUMPDASHSWING,			// �ڷ� ��¦ ���ٰ� ������ ���ƿ��鼭 ���� Į �����鼭 ����
		GOAT_ATK_UNDERSWING,			// ���� ���� ������ ���Ҵٰ� �Ʒ��� ��� ����
		GOAT_ATK_TELEPORTUNDERSWING,	// (�����̵�) ���� ���� ������ ���Ҵٰ� �Ʒ��� ��� ����
		GOAT_ATK_LONGUNDERATK,			// ũ�� ���� ���Ƽ� �Ʒ��� ��� ������� ����
		GOAT_ATK_TELEPORTLONGUNDERATK,	// (�����̵�) ũ�� ���� ���Ƽ� �Ʒ��� ��� ������� ����
		GOAT_ATK_CONSECUTIVE_SWING,		// ���ƿ��鼭 ���� ���� ����(1�� 58��)
		GOAT_ATK_MAGICSHOT,				// ��������(����, ���Ÿ�, ����)
		GOAT_ATK_SHORTPILLAROFFIRE,		// ª�� �ұ��
		GOAT_ATK_GRAVITYPILLAROFFIRE,	// �÷��̾� ������� ū �ұ��
		GOAT_ATK_PILLAROFFIRE,			// �ұ�� ����
		GOAT_ATK_END
	};

private:
	CGoat(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGoat(const CGoat& rhs);
	virtual ~CGoat() = default;

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

public:
	void CreateWeapon();
	void SetWeaponAttackTime(CModel* pModel, _uint iAnimIndex, _double startTime, _double endTime);
	void SetIsWeaponAttacking(_bool bOption);
	void StartTeleportDissolve();

private: // Collider ���� �Լ�
	void Ready_Collider_Info();
	void Collider_Transform_Update();

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RootNodeIndex();
	HRESULT SetUp_Animations();
	HRESULT SetUp_ConstantTable();

private:
	void OnCollision(_double TimeDelta);
	void ChangeState(_double TimeDelta);

private:
	void TeleportDissolveUpdate(_double TimeDelta);

private:
	CStateMachine*	m_pStateMachineCom  = nullptr;
	CShader*		m_pShaderCom        = nullptr;
	CRenderer*		m_pRendererCom      = nullptr;
	CModel*			m_pModelCom         = nullptr;
	CStatus*		m_pStatusCom        = nullptr;
	CNavigation*	m_pNavigationCom    = nullptr;
	CCollider*		m_pHitColliderCom   = nullptr;

	// �븻, �����, ����
	GOAT_ATK m_ePrevGoatAtk[3] = { GOAT_ATK_END, };
	GOAT_ATK m_eGoatAtk[3]     = { GOAT_ATK_END, };

	_bool m_bOnlyIdle = false;
	
	// ���� ���� ����
	_double m_DamagedTimeAcc   = 0.0;
	_uint	m_iDamagedCountAcc = 0; // �ǰ� Ƚ�� (���� �ð� ���� ���� �ǰ� �� ȸ�� �ߵ�)

	_double m_MaxAttackCool  = 4.5;
	_double m_AttackCool     = 0.0;
	_bool	m_bCanAttack     = true;
	_uint	m_iNormalAtkAcc  = 0;
	_uint	m_iSpecialAtkAcc = 0;

	_uint	m_iCurrentPhase    = 0; // ���� ������
	_bool	m_bPhaseChanged[2] = { false, };

	class CGoatWeapon* m_pWeapon = nullptr;

	_double m_MotionTrailInterval = 0.0;

private: // �ǰ� �ݶ��̴� ���� ����
	CModel::BONEMATRIX_PTR	m_HitCollider_BoneMatrixPtr;
	_float4x4				m_HitCollider_SocketPivotmMatrix;
	_matrix					m_HitCollider_TransformMatrix;

private: // �����̵� ����Ʈ�� ����
	_float	m_fDissolveAlpha = 0.f;
	_bool	m_bDissolve      = false;
	_bool	m_bDissolvePlus  = true;
	_float	m_fDistortion    = 0.f;

public:
	static	CGoat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END