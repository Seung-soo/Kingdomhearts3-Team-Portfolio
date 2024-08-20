#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

class CDonald_Ice final : public CGameObject
{
public:
	CDonald_Ice(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CDonald_Ice(const CDonald_Ice& rhs);
	virtual ~CDonald_Ice() = default;

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
	CModel*					m_pModelCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;

private:
	_float4					m_vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	_float					m_fScale = 0.5f;
	_float					m_fEndScale = 1.5f;
	class CDonald*			m_pDonald = nullptr;
public:
	static	CDonald_Ice* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END