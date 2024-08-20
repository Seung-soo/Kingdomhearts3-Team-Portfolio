#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CRenderer;
class CModel;

END

BEGIN(Client)

class CBigTree_Howling final : public CGameObject
{
public:
	CBigTree_Howling(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBigTree_Howling(const CBigTree_Howling& rhs);
	virtual ~CBigTree_Howling() = default;

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
	_float					m_fLifeTime = 3.0f;
public:
	static	CBigTree_Howling* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END