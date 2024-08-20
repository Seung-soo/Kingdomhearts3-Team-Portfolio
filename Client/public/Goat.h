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
		GOAT_ATK_SPEAR,					// 일직선으로 찌르기 공격
		GOAT_ATK_TELEPORTSPEAR,			// (순간이동) 일직선으로 찌르기 공격
		GOAT_ATK_CONSECUTIVE_SPEAR,		// (순간이동) 계속 순간이동 하면서 연속 찌르기 공격
		GOAT_ATK_FLYSWING,				// 위로 살짝 날았다가 대각선 아래로 스윙 공격
		GOAT_ATK_TELEPORTFLYSWING,		// (순간이동) 위로 살짝 날았다가 대각선 아래로 스윙 공격
		GOAT_ATK_DASHSWING,				// 일직선으로 날았다가 스윙 공격
		GOAT_ATK_TELEPORTDASHSWING,		// (순간이동) 일직선으로 날았다가 스윙 공격
		GOAT_ATK_SPINDASHSWING,			// 칼 돌리면서 날아오다가 마지막에 크게 공격
		GOAT_ATK_TELEPORTSPINDASHSWING,	// (순간이동) 칼 돌리면서 날아오다가 마지막에 크게 공격
		GOAT_ATK_JUMPDASHSWING,			// 뒤로 살짝 갔다가 앞으로 날아오면서 위로 칼 돌리면서 공격
		GOAT_ATK_UNDERSWING,			// 위로 조금 빠르게 날았다가 아래로 찍는 공격
		GOAT_ATK_TELEPORTUNDERSWING,	// (순간이동) 위로 조금 빠르게 날았다가 아래로 찍는 공격
		GOAT_ATK_LONGUNDERATK,			// 크게 위로 날아서 아래로 길게 내려찍는 공격
		GOAT_ATK_TELEPORTLONGUNDERATK,	// (순간이동) 크게 위로 날아서 아래로 길게 내려찍는 공격
		GOAT_ATK_CONSECUTIVE_SWING,		// 날아오면서 연속 스윙 공격(1분 58초)
		GOAT_ATK_MAGICSHOT,				// 마법공격(근접, 원거리, 원격)
		GOAT_ATK_SHORTPILLAROFFIRE,		// 짧은 불기둥
		GOAT_ATK_GRAVITYPILLAROFFIRE,	// 플레이어 끌어당기는 큰 불기둥
		GOAT_ATK_PILLAROFFIRE,			// 불기둥 공격
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

private: // Collider 관련 함수
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

	// 노말, 스페셜, 연속
	GOAT_ATK m_ePrevGoatAtk[3] = { GOAT_ATK_END, };
	GOAT_ATK m_eGoatAtk[3]     = { GOAT_ATK_END, };

	_bool m_bOnlyIdle = false;
	
	// 패턴 관련 변수
	_double m_DamagedTimeAcc   = 0.0;
	_uint	m_iDamagedCountAcc = 0; // 피격 횟수 (일정 시간 내에 연속 피격 시 회피 발동)

	_double m_MaxAttackCool  = 4.5;
	_double m_AttackCool     = 0.0;
	_bool	m_bCanAttack     = true;
	_uint	m_iNormalAtkAcc  = 0;
	_uint	m_iSpecialAtkAcc = 0;

	_uint	m_iCurrentPhase    = 0; // 현재 페이즈
	_bool	m_bPhaseChanged[2] = { false, };

	class CGoatWeapon* m_pWeapon = nullptr;

	_double m_MotionTrailInterval = 0.0;

private: // 피격 콜라이더 관련 변수
	CModel::BONEMATRIX_PTR	m_HitCollider_BoneMatrixPtr;
	_float4x4				m_HitCollider_SocketPivotmMatrix;
	_matrix					m_HitCollider_TransformMatrix;

private: // 순간이동 이펙트용 변수
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