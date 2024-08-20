#pragma once
#include "Shader.h"

BEGIN(Engine)

class CShader_Blur final : public CShader
{
private:
	explicit CShader_Blur(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CShader_Blur(const CShader_Blur& rhs);
	virtual ~CShader_Blur() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual HRESULT NativeConstruct(void* pArg);

public:
	bool SetShaderParameters(ID3D11DeviceContext* pDeviceContext, _matrix World, _matrix View, _matrix Proj, ID3D11ShaderResourceView*, float fValue);
	void RenderShader(ID3D11DeviceContext*, int);

public:
	static CShader*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual void		Free() override;

private:
	ID3D11Buffer*		m_pMatrixBuffer = nullptr;
};

END