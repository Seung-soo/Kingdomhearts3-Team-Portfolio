#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CDropFruitZone final : public CGameObject
{
private:
	CDropFruitZone(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CDropFruitZone(const CDropFruitZone& rhs);
	virtual ~CDropFruitZone() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CTexture*			m_pNoiseTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;

private:
	_float	m_fRotation = 0.f;
	_double	m_TimeAcc   = 0.0;

	_double	m_PtclTimeAcc = 0.0;
	_bool	m_bPlayParticle = false;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CDropFruitZone*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END