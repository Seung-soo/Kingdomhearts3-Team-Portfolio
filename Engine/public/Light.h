#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight final : public CBase
{
private:
	CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLight() = default;
public:
	LIGHTDESC* Get_LightDesc() { return &m_LightDesc; }
	void	   Set_SSAO(_bool bSSAO) { m_bSSAO = bSSAO; }
public:
	HRESULT	NativeConstruct(const LIGHTDESC& LightDesc);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	HRESULT Render_DirectionalLight(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer, _bool isRenderShadow);

	// For Shadow
	_float4x4	Get_LightViewMatrix();

public:
	// For Shadow
	void		Set_LightDistance(_float fDistance) { m_fLightDistance = fDistance; };

private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;
	LIGHTDESC					m_LightDesc;
	class CTarget_Manager*		m_pTarget_Manager = nullptr;

	// For Shadow
	_matrix						m_LightViewMatrix;
	_float						m_fLightDistance = 6.f;

	// 라이트 투영행렬은 한번 세팅하면 변하지 않기때문애 라이트 매니저에서 생성

	_bool						m_bSSAO = true;
public:
	static CLight*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const LIGHTDESC& LightDesc);
	virtual void	Free() override;
};

END