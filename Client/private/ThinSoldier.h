#pragma once

#include "Client_Defines.h"
#include "MonsterDefines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader; 
class CRenderer;
class CModel;
class CStateMachine;
class CCollider;
class CStatus;
class CMonsterCooler;
class CNavigation;
END

BEGIN(Client)

class CThinSoldier final : public CMonster
{
private:
	CThinSoldier(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CThinSoldier(const CThinSoldier& rhs);
	virtual ~CThinSoldier() = default;

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

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RootNodeIndex();
	HRESULT SetUp_Animations();
	HRESULT SetUp_ConstantTable();

private:
	void OnCollision(_double TimeDelta);	// 충돌 처리 함수
	void ChangeState(_double TimeDelta);	// 상태 변경 함수
	void ResetHitCount(_double TimeDelta);	// 피격카운트 초기화

private:
	CStateMachine*	m_pStateMachineCom  = nullptr;
	CShader*		m_pShaderCom        = nullptr;
	CRenderer*		m_pRendererCom      = nullptr;
	CModel*			m_pModelCom         = nullptr;
	CStatus*		m_pStatusCom        = nullptr;
	CMonsterCooler* m_pMonsterCoolerCom = nullptr;
	CNavigation*	m_pNavigationCom    = nullptr;

	CCollider*		m_pHitColliderCom	= nullptr;
	CCollider*		m_pAtkColliderCom	= nullptr;

	_bool			bCreate = false;
	CGameObject*	m_pMinimapIcon = nullptr;

public:
	static	CThinSoldier* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END