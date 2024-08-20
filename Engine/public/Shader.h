#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader : public CComponent
{
public:
	typedef struct tagPassDesc
	{
		ID3DX11EffectPass*		pPass;
		ID3D11InputLayout*		pInputLayout;
	} PASSDESC;

protected:
	explicit CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual HRESULT NativeConstruct(void* pArg);

public:
	HRESULT Apply(_uint iPassIndex);
	HRESULT Set_InputLayout(_uint iPassIndex);
	HRESULT Set_RawValue(const char* pValueName, const void* pData, _uint iLength);
	HRESULT Set_Texture(const char* pValueName, ID3D11ShaderResourceView* pShaderResourceView);
	_uint	Get_NumberOfPasses() { return (_uint)(m_Passes.size()); }

protected:
	ID3DX11Effect*		m_pEffect = nullptr;
	vector<PASSDESC*>	m_Passes;

public:
	static CShader*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void		Free() override;
};

END