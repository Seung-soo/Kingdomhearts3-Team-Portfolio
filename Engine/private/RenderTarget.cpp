#include "..\public\RenderTarget.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

CRenderTarget::CRenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CRenderTarget::NativeConstruct(_uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, _float4 vClearColor)
{
	CHECK_NULL(m_pDevice);

	// 랜더타겟을 클리어 시킬 색상을 저장
	m_vClearColor = vClearColor;

	/* 텍스쳐 생성 */
	
	// 텍스쳐 정보 입력
	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iSizeX;
	TextureDesc.Height = iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = eFormat;
	TextureDesc.SampleDesc.Count = 1;	// 픽셀당 멀티 샘플링 수
	TextureDesc.SampleDesc.Quality = 0;	// 이미지 품질
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	// 텍스쳐를 만들고 멤버변수에 저장
	CHECK_FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D));

	// 랜더 타겟 뷰 생성
	CHECK_FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV));

	// 셰이더 리소스 뷰 생성
	CHECK_FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV));


	XMStoreFloat4x4(&m_RTProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)TextureDesc.Width, (_float)TextureDesc.Height, 0.f, 1.f)));
	
	return S_OK;
}

HRESULT CRenderTarget::NativeConstruct_Cube(_uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, _float4 vClearColor)
{
	CHECK_NULL(m_pDevice);

	// 랜더타겟을 클리어 시킬 색상을 저장
	m_vClearColor = vClearColor;

	/* 텍스쳐 생성 */

	// 텍스쳐 정보 입력
	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = 256;
	TextureDesc.Height = 256;
	TextureDesc.MipLevels = 9;
	TextureDesc.ArraySize = 6;
	TextureDesc.Format = eFormat;
	TextureDesc.SampleDesc.Count = 1;	// 픽셀당 멀티 샘플링 수
	TextureDesc.SampleDesc.Quality = 0;	// 이미지 품질
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

	// 텍스쳐를 만들고 멤버변수에 저장
	CHECK_FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D));

	D3D11_RENDER_TARGET_VIEW_DESC RTDesc;
	RTDesc.Format = eFormat;
	RTDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	RTDesc.Texture2DArray.ArraySize = 6;
	RTDesc.Texture2DArray.FirstArraySlice = 0;
	RTDesc.Texture2DArray.MipSlice = 0;

	// 랜더 타겟 뷰 생성
	CHECK_FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, &RTDesc, &m_pRTV));

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	SRVDesc.Format = eFormat;
	SRVDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBE;
	SRVDesc.TextureCube.MipLevels = 9;
	SRVDesc.TextureCube.MostDetailedMip = 0;

	// 셰이더 리소스 뷰 생성
	CHECK_FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, &SRVDesc, &m_pSRV));


	XMStoreFloat4x4(&m_RTProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)TextureDesc.Width, (_float)TextureDesc.Height, 0.f, 1.f)));

	return S_OK;
}

HRESULT CRenderTarget::Clear()
{
	CHECK_NULL(m_pDeviceContext);

	m_pDeviceContext->ClearRenderTargetView(m_pRTV, (_float*)&m_vClearColor);

	return S_OK;
}

//#ifdef _DEBUG

HRESULT CRenderTarget::Ready_DebugDesc(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	//뷰포트 받아와서 직교행렬들을 만들어줌
	_uint		iNumViewports = 1;
	
	D3D11_VIEWPORT	Viewport;

	m_pDeviceContext->RSGetViewports(&iNumViewports, &Viewport);

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_WorldMatrix._11 = fSizeX;
	m_WorldMatrix._22 = fSizeY;
	m_WorldMatrix._33 = 1.f;
	memcpy(&m_WorldMatrix.m[3][0], &_float4(fX - (Viewport.Width * 0.5f), -fY + (Viewport.Height * 0.5f), 0.f, 1.f), sizeof(_float4));

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix)));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f)));


	return S_OK;
}

HRESULT CRenderTarget::Render_DebugBuffer(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	CHECK_FAILED(pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	CHECK_FAILED(pShader->Set_Texture("g_TargetTexture", m_pSRV));

	CHECK_FAILED(pVIBuffer->Render(pShader, m_iPass));

	return S_OK;
}

//#endif

CRenderTarget * CRenderTarget::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, _float4 vClearColor)
{
	CRenderTarget*	pInstance = new CRenderTarget(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(iSizeX, iSizeY, eFormat, vClearColor)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}
	return pInstance;
}

CRenderTarget * CRenderTarget::CreateCube(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, _float4 vClearColor)
{
	CRenderTarget*	pInstance = new CRenderTarget(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Cube(iSizeX, iSizeY, eFormat, vClearColor)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRenderTarget::Free()
{
	Safe_Release(m_pSRV);
	Safe_Release(m_pRTV);
	Safe_Release(m_pTexture2D);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
