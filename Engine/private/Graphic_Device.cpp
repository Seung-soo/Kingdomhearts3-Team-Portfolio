#include "..\public\Graphic_Device.h"

READY_SINGLETON(CGraphic_Device)

CGraphic_Device::CGraphic_Device()
	: m_pDevice(nullptr), m_pDeviceContext(nullptr), m_pSwapChain(nullptr), m_pBackBufferRTV(nullptr),m_pDepthStencilView(nullptr)
{
}

HRESULT CGraphic_Device::Ready_Graphic_Device(HWND hWnd, WINMODE WinMode, _uint iWinCX, _uint iWinCY, ID3D11Device ** ppDeviceOut, ID3D11DeviceContext ** ppDeviceContextOut)
{
	_uint		iFlag = 0;


	// 디버그 과정에서 뭐가 문젠지 알려주는 기능
#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

	// 버전을 사용할 수 있는지 장치 기능 수준을 받아오는 구조체
	D3D_FEATURE_LEVEL		FeatureLV;

	// 그래픽 장치 초기화
	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, nullptr, 0, D3D11_SDK_VERSION, &m_pDevice, &FeatureLV, &m_pDeviceContext)))
		return E_FAIL;

	// 백버퍼에 정보를 셋팅하여 스왑체인 객체를 생성한다 : 백버퍼를 만든다.
	if (FAILED(Ready_SwapChain(hWnd, WinMode, iWinCX, iWinCY)))
		return E_FAIL;

	if (FAILED(Ready_BackBufferRenderTargetView()))
		return E_FAIL;

	if (FAILED(Ready_DepthStencilRenderTargetView(iWinCX, iWinCY)))
		return E_FAIL;

	// 장치에 바인드해놓을 렌더 타겟들과 뎁스스텐실뷰를 셋팅한다.
	m_pDeviceContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pDepthStencilView);

	// 뷰포트 셋팅
	ZeroMemory(&m_ViewPortDesc, sizeof(D3D11_VIEWPORT) * 2);

	m_ViewPortDesc[DEFAULT].TopLeftX  = 0;
	m_ViewPortDesc[DEFAULT].TopLeftY  = 0;
	m_ViewPortDesc[DEFAULT].Width     = (_float)iWinCX;
 	m_ViewPortDesc[DEFAULT].Height    = (_float)iWinCY;
 	m_ViewPortDesc[DEFAULT].MinDepth  = 0.f;
 	m_ViewPortDesc[DEFAULT]. MaxDepth = 1.f;

	// Shadow Viewport 생성
	m_ViewPortDesc[SHADOW].TopLeftX = 0;
	m_ViewPortDesc[SHADOW].TopLeftY = 0;
	m_ViewPortDesc[SHADOW].Width    = 4069.f;
	m_ViewPortDesc[SHADOW].Height   = 4069.f;
	m_ViewPortDesc[SHADOW].MinDepth = 0.f;
	m_ViewPortDesc[SHADOW].MaxDepth = 1.f;

	m_pDeviceContext->RSSetViewports(2, m_ViewPortDesc);

	*ppDeviceOut = m_pDevice;
	*ppDeviceContextOut = m_pDeviceContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	return S_OK;
}

HRESULT CGraphic_Device::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	// 백버퍼를 초기화 한다.
	m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV, (_float*)&vClearColor);

	return S_OK;
}

HRESULT CGraphic_Device::Clear_DepthStencil_View()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	return S_OK;
}

HRESULT CGraphic_Device::Present()
{
	if (nullptr == m_pSwapChain)
		return E_FAIL;

	return m_pSwapChain->Present(0, 0);
}

void CGraphic_Device::Set_DepthStencilView()
{
	m_pDeviceContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pDepthStencilView);
}

void CGraphic_Device::Set_Viewport(VIEWPORT eViewport)
{
	m_pDeviceContext->RSSetViewports(1, &m_ViewPortDesc[eViewport]);
}

D3D11_VIEWPORT CGraphic_Device::Get_Viewport(VIEWPORT eViewport)
{
	D3D11_VIEWPORT	Viewport[VIEWPORT_END];
	ZeroMemory(&Viewport, sizeof(D3D11_VIEWPORT) * 2);
	
	_uint			iViewportNum = 2;

	m_pDeviceContext->RSGetViewports(&iViewportNum, Viewport);

	return Viewport[eViewport];
}

HRESULT CGraphic_Device::Ready_SwapChain(HWND hWnd, WINMODE eWinMode, _uint iWinCX, _uint iWinCY)
{
	IDXGIDevice*			pDevice = nullptr;
	m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDevice);

	IDXGIAdapter*			pAdapter = nullptr;
	pDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);

	IDXGIFactory*			pFactory = nullptr;
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

	DXGI_SWAP_CHAIN_DESC		SwapChain;
	ZeroMemory(&SwapChain, sizeof(DXGI_SWAP_CHAIN_DESC));

	// 백버퍼 정보들 입력
	SwapChain.BufferDesc.Width = iWinCX;
	SwapChain.BufferDesc.Height = iWinCY;
	SwapChain.BufferDesc.RefreshRate.Numerator = 60;
	SwapChain.BufferDesc.RefreshRate.Denominator = 1;
	SwapChain.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	SwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	SwapChain.SampleDesc.Quality = 0;
	SwapChain.SampleDesc.Count = 1;
	SwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChain.BufferCount = 1;
	SwapChain.OutputWindow = hWnd;
	SwapChain.Windowed = eWinMode;
	SwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	if (FAILED(pFactory->CreateSwapChain(m_pDevice, &SwapChain, &m_pSwapChain)))
		return E_FAIL;

	Safe_Release(pFactory);
	Safe_Release(pAdapter);
	Safe_Release(pDevice);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_BackBufferRenderTargetView()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	ID3D11Texture2D*	pBackBufferTexture = nullptr;

	if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(pBackBufferTexture, nullptr, &m_pBackBufferRTV)))
		return E_FAIL;

	Safe_Release(pBackBufferTexture);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY)
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	ID3D11Texture2D*		pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWinCX;
	TextureDesc.Height = iWinCY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDepthStencilView)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);


	// Create Shadow Depth StencilView
	ID3D11Texture2D* pDepthStencilTexture_Shadow = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc_Shadow;
	ZeroMemory(&TextureDesc_Shadow, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc_Shadow.Width = (_uint)4069;
	TextureDesc_Shadow.Height = (_uint)4069;
	TextureDesc_Shadow.MipLevels = 1;
	TextureDesc_Shadow.ArraySize = 1;
	TextureDesc_Shadow.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc_Shadow.SampleDesc.Quality = 0;
	TextureDesc_Shadow.SampleDesc.Count = 1;

	TextureDesc_Shadow.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc_Shadow.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc_Shadow.CPUAccessFlags = 0;
	TextureDesc_Shadow.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc_Shadow, nullptr, &pDepthStencilTexture_Shadow)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture_Shadow, nullptr, &m_pShadowDepthStencilView)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture_Shadow);

	return S_OK;
}

void CGraphic_Device::Free()
{
	Safe_Release(m_pSwapChain);
	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pShadowDepthStencilView);
	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pDeviceContext);


#if defined(DEBUG) || defined(_DEBUG)
	ID3D11Debug* d3dDebug;
	HRESULT hr = m_pDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
	if (SUCCEEDED(hr))
	{
		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r\n");
		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker \r\n");
		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r\n");

		hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r\n");
		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker END \r\n");
		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r\n");
	}
	if (d3dDebug != nullptr)            d3dDebug->Release();
#endif



	Safe_Release(m_pDevice);
}