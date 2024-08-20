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
	virtual void	PostRender(_double TimeDelta); // ImGui 디버깅용 (GameObject 에 정의된 m_bImGuiController 이 true 면 항상 실행, false 면 ImGui 창에서 직접 켜줘야함)

public:
	virtual HRESULT		Save_ObjectData(HANDLE hFile) override; // 맵툴용 : 보스의 월드매트릭스를 저장해두기 위해 필요(복붙하면 됨)
	virtual HRESULT		Load_ObjectData(HANDLE hFile) override; // 맵툴용 : 보스의 월드매트릭스를 로드하기 위해 필요(복붙하면 됨)
	virtual CCollider*	Get_HitCollider() { return m_pHitColliderCom; } // 맵툴용 : 맵툴에서 피킹용으로 사용함, 아무 콜라이더나 return 시켜주면 됨

	// Player_Weapon 에서 호출하는 함수, 몬스터가 플레이어한테 피격됐을 때 처리해야할 데이터들을 이 함수 안에서 처리
	virtual _bool		Damaged(CCollider* pAtkCollider, _float fDamaged);


private: // Collider 관련 함수
	HRESULT Ready_Collider_Info(); // 콜라이더를 붙일 뼈의 정보 초기화
	HRESULT	Collider_Transform_Update(); // 콜라이더 위치 업데이트

private:
	HRESULT SetUp_Components();		// 컴포넌트 초기화
	HRESULT SetUp_RootNodeIndex();	// 루트노드 초기화(움직임이 있는 애니메이션과 Transform 의 포지션을 동기화하기 위해)
	HRESULT SetUp_Animations();		// 애니메이션 초기화
	HRESULT SetUp_ConstantTable();

private:
	HRESULT OnCollision(_double TimeDelta); // 충돌 처리 함수
	HRESULT ChangeState(_double TimeDelta); // 상태 변경 함수

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


	// 몬스터의 콜라이더는 두 종류(공격 콜라이더, 피격 콜라이더)로 나누어져 있음
	CCollider* m_pHitColliderCom = nullptr; // 피격 콜라이더
	CCollider* m_pAtkColliderCom = nullptr; // 공격 콜라이더

	// 피격 애니메이션을 매번 재생하면 이상하니까 피격 애니메이션 재생에 쿨타임을 주기 위해 만든 변수
	_double		m_OnDamagedAnimTimeAcc = 0.0;

	_bool		m_bOnlyIdle = false; // 디버깅용 (무조건 Idle 상태로 만들기)

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

private: // 콜라이더 관련 변수
	// 피격
	CModel::BONEMATRIX_PTR	m_HitCollider_BoneMatrixPtr;
	_float4x4				m_HitCollider_SocketPivotMatrix;
	_matrix					m_HitCollider_SocketMatrix;
	_matrix					m_HitCollider_TransformMatrix;

	// 공격
	CModel::BONEMATRIX_PTR	m_AtkCollider_BoneMatrixPtr;
	_float4x4				m_AtkCollider_SocketPivotMatrix;
	_matrix					m_AtkCollider_SocketMatrix;
	_matrix					m_AtkCollider_TransformMatrix;
};

