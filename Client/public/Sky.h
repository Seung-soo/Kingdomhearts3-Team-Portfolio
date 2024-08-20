#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Cube;
class CModel;
END

BEGIN(Client)

class CSky final : public CGameObject
{
private:
	CSky(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CSky(const CSky& rhs);
	virtual ~CSky() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void	PostRender(_double TimeDelta);

private:
	CShader*			m_pShaderCom   = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CTexture*			m_pNoiseTextureCom  = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Cube*		m_pVIBufferCom = nullptr;	
	CModel*				m_pModelCom = nullptr;
private:
	_int				m_iTexIndex = 0;
	_float4				m_vColor = _float4(0.f, 0.f, 0.f, 1.f);

	_float				m_fNoiseX = 0.f;
	_float				m_fNoiseY = 0.f;

private:
	_bool m_bEffectLevel = false;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
public:
	static CSky* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END