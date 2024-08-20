#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CRenderer;
class CModel;
class CTexture;
END

BEGIN(Client)

class CNightmare_Blade final : public CGameObject
{
public:
	typedef struct tagBladeDesc
	{
		_matrix		LocalMatrix         = XMMatrixIdentity();
		CTransform*	pParentTransform	= nullptr;
	}BLADEDESC;
public:
	CNightmare_Blade(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CNightmare_Blade(const CNightmare_Blade& rhs);
	virtual ~CNightmare_Blade() = default;

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
	HRESULT Make_Particle();
private:
	CModel*					m_pModelCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
private:
	_float					m_fLifeTime;
	_float					m_fMovingV = 1.f;
	_float					m_fMovingUV_Power = -2.f;
	_bool					m_bBlur = true;
	BLADEDESC				m_tBladeDesc;
	_float4					m_vColor = _float4(0.f, 0.f, 0.f, 0.f);
	_float4					m_vPosition = _float4(0.f, 0.f, 0.f, 1.f);
public:
	static	CNightmare_Blade* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END