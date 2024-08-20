#pragma once

#include "Client_Defines.h"
#include "MonsterDefines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader; 
class CRenderer;
class CModel;
class CCollider;
class CStatus;
class CNavigation;
END

BEGIN(Client)

class CCrow final : public CMonster
{
public:
	typedef struct tagCrowDesc
	{
		_vector vPos;
		_vector vRandomDir;
	}CROWDESC;

private:
	CCrow(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CCrow(const CCrow& rhs);
	virtual ~CCrow() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual HRESULT NativeConstruct_Pool(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void	PostRender(_double TimeDelta);

public:
	virtual _bool	Damaged(CCollider* pAtkCollider, _float fDamaged);
	virtual CCollider* Get_HitCollider() { return m_pHitColliderCom; }

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RootNodeIndex();
	HRESULT SetUp_Animations();
	HRESULT SetUp_ConstantTable();

private:
	void OnCollision(_double TimeDelta); // 충돌 처리 함수
	void ChasePlayer(_double TimeDelta); // 플레이어 추적

private:
	CShader*		m_pShaderCom      = nullptr;
	CRenderer*		m_pRendererCom    = nullptr;
	CModel*			m_pModelCom       = nullptr;
	CStatus*		m_pStatusCom      = nullptr;
	CNavigation*	m_pNavigationCom  = nullptr;

	CCollider*		m_pHitColliderCom = nullptr;
	CCollider*		m_pAtkColliderCom = nullptr;

	class CTerrain* m_pTerrain = nullptr;

	_double m_TimeAcc = 0.0;

	CROWDESC m_CrowDesc;

private:
	CGameObject*	m_pBigTree = nullptr;
public:
	static	CCrow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END