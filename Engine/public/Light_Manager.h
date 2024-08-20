#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	MAKE_SINGLETON(CLight_Manager)

public:
	enum SHADOWTYPE {SHADOW_DYNAMIC, SHADOW_STATIC, SHADOW_END};

	typedef struct tagLightMatrixDesc
	{
		_float4x4	LightViewMatrix;
		_float4x4	LightProjMatrix;
	}LIGHTMATRIX_DESC;

private:
	CLight_Manager() = default;
	virtual ~CLight_Manager() = default;
public:
	LIGHTDESC*	Get_LightDesc(_uint iIndex);

	size_t		Get_LightSize() {return m_Lights.size(); }

	void		Set_SSAO(_bool bSSAO) { m_bSSAO = bSSAO; }

	// For Shadow
	LIGHTMATRIX_DESC Get_LightMatrix(SHADOWTYPE eType) { return m_tLightMatrix[eType]; };

	void Set_StaticLightViewMatrix(_fvector vLightPos);
	void Set_RenderShadow(_bool isRenderShadow) {m_isRenderShadow = isRenderShadow;}

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	HRESULT	Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const LIGHTDESC& LightDesc);
	void	Update();
	HRESULT Render();
	HRESULT	Release_Light();

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
private: 
	list<class CLight*>			m_Lights;
	typedef list<class CLight*>	LIGHTS;
private:
	class CVIBuffer_Rect*		m_pVIBuffer = nullptr;
	class CShader*				m_pShader = nullptr;
private:
	_float4x4					m_WorldMatrix;
	_float4x4					m_ViewMatrix;
	_float4x4					m_ProjMatrix;

	// For Shadow
	LIGHTMATRIX_DESC			m_tLightMatrix[SHADOW_END];
	_bool						m_isRenderShadow = true;
	_bool						m_bSSAO = true;
public:
	virtual void	Free() override;
};

END