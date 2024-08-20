#include "..\Public\SpriteFont.h"
#include "PipeLine.h"

CSpriteFont::CSpriteFont(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CSpriteFont::NativeConstruct(const _tchar * pFontFilePath)
{
	m_pBatch = new SpriteBatch(m_pDeviceContext);
	RETURN_FAIL(if (nullptr == m_pBatch))

	m_pFont = new SpriteFont(m_pDevice, pFontFilePath);
	RETURN_FAIL(if (nullptr == m_pFont))

	return S_OK;
}

HRESULT CSpriteFont::Render(const _tchar* pText, _float2 vPosition, _fvector vColor)
{
	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	_matrix			ViewMatrix = pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	_matrix			ProjMatrix = pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);

	m_pBatch->Begin();

	m_pFont->DrawString(m_pBatch, pText, XMVectorSet(0.f, 5.f, 0.f, 1.f), vColor, 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 0.f));

	m_pBatch->End();

	 

	return S_OK;
}

CSpriteFont * CSpriteFont::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pFontFilePath)
{
	CSpriteFont*	pInstance = new CSpriteFont(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(pFontFilePath)))
	{
		MSGBOX("Failed to Created CFont");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSpriteFont::Free()
{

	Safe_Delete(m_pBatch);
	Safe_Delete(m_pFont);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
