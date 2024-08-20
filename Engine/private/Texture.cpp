#include "..\public\Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CTexture::CTexture(const CTexture & rhs)
	: CComponent(rhs)
	, m_Textures(rhs.m_Textures)
{
	for (auto& pTexture : m_Textures)
		Safe_AddRef(pTexture);
}

HRESULT CTexture::SetUp_OnShader(CShader * pShader, const char * pValueName, _uint iTextureIndex)
{
	if (nullptr == pShader || iTextureIndex >= m_Textures.size())
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return pShader->Set_Texture(pValueName, m_Textures[iTextureIndex]);
}

HRESULT CTexture::NativeConstruct_Prototype(const _tchar * pTexturePath, _uint iNumTextures)
{
	CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

	_tchar		szTextureFilePath[MAX_PATH] = TEXT("");
	_tchar		szExt[MAX_PATH] = TEXT("");

	m_Textures.reserve(iNumTextures);

	for (_uint i = 0; i < iNumTextures; i++)
	{
		wsprintf(szTextureFilePath, pTexturePath, i);
		ID3D11ShaderResourceView*	pSRV = nullptr;
		ID3D11Texture2D*			pTexture2D = nullptr;
		_wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

		if (!lstrcmp(szExt, TEXT(".dds")))
		{
			if (FAILED(CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, (ID3D11Resource**)&pTexture2D, &pSRV)))
				return E_FAIL;
		}
		else if (!lstrcmp(szExt, TEXT(".tga")))
			return E_FAIL;
		else
		{
			if (FAILED(CreateWICTextureFromFile(m_pDevice, szTextureFilePath, (ID3D11Resource**)&pTexture2D, &pSRV)))
				return E_FAIL;
		}
		m_Textures.push_back(pSRV);
		Safe_Release(pTexture2D);
	}


	return S_OK;
}

HRESULT CTexture::NativeConstruct(void * pArg)
{
	return S_OK;
}

CTexture * CTexture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pTextureFilePath, _uint iNumTextures)
{
	CTexture*	pInstance = new CTexture(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pTextureFilePath, iNumTextures)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTexture::Clone(void * pArg)
{
	CTexture*	pInstance = new CTexture(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (auto& pTexture : m_Textures)
		Safe_Release(pTexture);
	m_Textures.clear();
}
