#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Terrain;
class CTexture;
END

BEGIN(Client)

class CWater final : public CGameObject
{
private:
	CWater(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CWater(const CWater& rhs);
	virtual ~CWater() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void	PostRender(_double TimeDelta);
	virtual HRESULT Render_Shadow();
private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Terrain*	m_pVIBufferCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CTexture*			m_pNormalTextureCom = nullptr;
	CTexture*			m_pNoiseTextureCom = nullptr;
private:
	_float				m_fNoiseUV = 0.f;
	_int				m_iTexIndex = 0;
	_float4				m_vWaterColor = _float4(0.29f, 0.29f, 0.41f, 0.f);
	_bool				m_bRenderShadow = false;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static	CWater*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END