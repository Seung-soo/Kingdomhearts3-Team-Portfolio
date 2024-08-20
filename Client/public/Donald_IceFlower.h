#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CRenderer;
class CModel;

END

BEGIN(Client)

class CDonald_IceFlower final : public CGameObject
{
public:
	typedef struct tagIceFlowerDesc
	{
		_float4x4	WorldMatrix;
		_bool*		pDead;
	}ICEFLOWERDESC;
public:
	void	Set_Color(_float4 vColor) { m_vColor = vColor; }
public:
	CDonald_IceFlower(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CDonald_IceFlower(const CDonald_IceFlower& rhs);
	virtual ~CDonald_IceFlower() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void	PostRender(_double TimeDelta);

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	CModel*					m_pModelCom    = nullptr;
	CShader*				m_pShaderCom   = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
private:
	ICEFLOWERDESC			m_tIceFlowerDesc;
	_float4					m_vPosition = _float4(0.f, 0.f, 0.f, 1.f);

	_float					m_fScale    = 1.f;
	_float					m_fLerpTime = 0.f;
	_float4					m_vColor    = _float4(0.5f, 0.6f, 1.f, 0.8f);

	_bool					m_bCollision = true;
	class CDonald*			m_pDonald    = nullptr;
public:
	static	CDonald_IceFlower* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END