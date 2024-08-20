#include "..\Public\Font_Manager.h"
#include "SpriteFont.h"

READY_SINGLETON(CFont_Manager)

CFont_Manager::CFont_Manager()
{
}


HRESULT CFont_Manager::Add_Font(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pFontTag, const _tchar * pFontFilePath)
{
	RETURN_FAIL(if (nullptr != Find_SpriteFont(pFontTag)))

	CSpriteFont*	pSpriteFont = CSpriteFont::Create(pDevice, pDeviceContext, pFontFilePath);
	RETURN_FAIL(if (nullptr == pSpriteFont))

	m_Fonts.emplace(pFontTag, pSpriteFont);

	return S_OK;
}

HRESULT CFont_Manager::Render_Font(ID3D11DeviceContext* pDeviceContext, const _tchar * pFontTag, const _tchar * pText, _float2 vPosition, _fvector vColor)
{
	CSpriteFont*		pFont = Find_SpriteFont(pFontTag);
	RETURN_FAIL(if (nullptr == pFont))

	pDeviceContext->GSSetShader(nullptr, nullptr, 0);

	return pFont->Render(pText, vPosition, vColor);
}

CSpriteFont * CFont_Manager::Find_SpriteFont(const _tchar * pFontTag)
{
	auto	iter = find_if(m_Fonts.begin(), m_Fonts.end(), CTagFinder(pFontTag));

	RETURN_NULL(if (iter == m_Fonts.end()))

	return iter->second;
}

void CFont_Manager::Free()
{
	for (auto& Pair : m_Fonts)
		Safe_Release(Pair.second);

	m_Fonts.clear();
}
