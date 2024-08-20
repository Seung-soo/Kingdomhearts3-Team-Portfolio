#include "..\public\Shader_Blur.h"

CShader_Blur::CShader_Blur(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CShader(pDevice, pDeviceContext)
{
}

CShader_Blur::CShader_Blur(const CShader_Blur & rhs)
	: CShader(rhs)
{
}

HRESULT CShader_Blur::NativeConstruct_Prototype(const _tchar * pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC * pElements, _uint iNumElements)
{
	if (FAILED(__super::NativeConstruct_Prototype(pShaderFilePath, pElements, iNumElements)))
		return E_FAIL;


	return S_OK;
}

HRESULT CShader_Blur::NativeConstruct(void * pArg)
{
	return S_OK;
}

bool CShader_Blur::SetShaderParameters(ID3D11DeviceContext * pDeviceContext, _matrix World, _matrix View, _matrix Proj, ID3D11ShaderResourceView *, float fValue)
{
	return false;
}

void CShader_Blur::RenderShader(ID3D11DeviceContext *, int)
{
}

CShader * CShader_Blur::Create(ID3D11Device* pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC * pElements, _uint iNumElements)
{
	CShader_Blur*	pInstance = new CShader_Blur(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath, pElements, iNumElements)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShader_Blur::Free()
{
	__super::Free();

	Safe_Release(m_pMatrixBuffer);
}
