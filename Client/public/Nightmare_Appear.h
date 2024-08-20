#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CRenderer;
class CModel;

END

BEGIN(Client)

class CNightmare_Appear final : public CGameObject
{
public:
	CNightmare_Appear(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CNightmare_Appear(const CNightmare_Appear& rhs);
	virtual ~CNightmare_Appear() = default;

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

private:
	_float					m_fLifeTime = 2.f;
public:
	static	CNightmare_Appear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END