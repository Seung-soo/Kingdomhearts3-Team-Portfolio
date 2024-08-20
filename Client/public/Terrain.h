#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "VIBuffer_Terrain.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
END

BEGIN(Client)

class CTerrain final : public CGameObject
{
public:
	explicit CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void	PostRender(_double TimeDelta);

private:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ConstantTable();

public:
	_uint Get_Width() { return m_iWidth; }
	_uint Get_Depth() { return m_iDepth; }

public:
	_vector Compute_PositionOnTerrain(_fvector _position);

	HRESULT Load_TerrainData(const _tchar* pFilePath);

private:
	class CPlayer* m_pPlayer = nullptr;
	class CTransform* m_pPlayerTransform = nullptr;

	_uint m_iWidth;
	_uint m_iDepth;

private:
	CShader*			m_pShaderCom		= nullptr;
	CTexture*			m_pBrushTextureCom	= nullptr;
	CTexture*			m_pFieldTextureCom	= nullptr;
	CRenderer*			m_pRendererCom		= nullptr;
	CVIBuffer_Terrain*	m_pVIBufferCom		= nullptr;

public:
	static	CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END