#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CPortal final : public CGameObject
{
private:
	CPortal(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPortal(const CPortal& rhs);
	virtual ~CPortal() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void	PostRender(_double TimeDelta);

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CTexture*			m_pNoiseTextureCom = nullptr;

private:
	_float				m_fNoiseUV = 0.f;

	CGameObject*		m_pFadeOut = nullptr;
	_bool				m_bGoNextLevel = false;

	_float				m_fAlpha = 1.f;
	_uint				m_iTexIndex = 0;

	CGameObject*		m_pButtonUI = nullptr;
	_bool				m_bButton = false;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static	CPortal*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END