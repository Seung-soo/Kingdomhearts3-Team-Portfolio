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

private: // Collider 관련 함수
	void Ready_Collider_Info();
	void Collider_Transform_Update();

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RootNodeIndex();
	HRESULT SetUp_Animations();
	HRESULT SetUp_ConstantTable();

private:
	void OnCollision(_double TimeDelta); // 충돌 처리 함수
	void ChangeState(_double TimeDelta); // 상태 변경 함수

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

	_double			m_OnDamagedAnimTimeAcc = 0.0;	// 피격 애니메이션 재생 쿨타임

	_double			m_MotionTrailInterval = 0.0;

private: // 콜라이더 관련 변수
	// 피격
	CModel::BONEMATRIX_PTR	m_HitCollider_BoneMatrixPtr;		// Collider 를 붙일 "뼈"의 정보(오프셋, 컴바인드)
	_float4x4				m_HitCollider_SocketPivotmMatrix;	// Collider 를 붙일 모델의 "피벗"매트릭스
	_matrix					m_HitCollider_SocketMatrix;			// Collider 를 붙일 "뼈"의 최종 매트릭스(오프셋 * 컴바인드 * 피벗)
	_matrix					m_HitCollider_TransformMatrix;		// Collider 에 "최종"적으로 적용할 매트릭스
	// 공격
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