#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_OrthoWin final: public CVIBuffer
{
public:
	explicit CVIBuffer_OrthoWin(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_OrthoWin(const CVIBuffer_OrthoWin& rhs);
	virtual ~CVIBuffer_OrthoWin() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(_int iWidth, _int iHeight);
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	virtual HRESULT Render(CShader* pShader, _uint iPassIndex) override;

public:
	static CVIBuffer_OrthoWin*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _int iWidth, _int iHeight);
	virtual CComponent*			Clone(void* pArg);
	virtual void				Free() override;

private:
	_float4x4					m_WorldMatrix;

	_float						m_fWidth = 0.f;
	_float						m_fHeight = 0.f;
};

END