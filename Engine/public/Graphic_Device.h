#pragma once

#include "Base.h"

BEGIN(Engine)

class CGraphic_Device final : public CBase
{
	MAKE_SINGLETON(CGraphic_Device)
public:
	enum WINMODE { MODE_FULL, MODE_WIN, MODE_END };

	enum VIEWPORT {DEFAULT, SHADOW, VIEWPORT_END};

public:
	typedef struct tagGraphicDeviceDesc
	{
		HWND		hWnd;
		_uint		iWinCX, iWinCY;
		WINMODE		eWinMode;
	}GRAPHICDESC;
public:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;
public:
	ID3D11Device*			Get_Device()			{ return m_pDevice; }
	ID3D11DeviceContext*	Get_Device_Context()	{ return m_pDeviceContext; }
public:
	HRESULT Ready_Graphic_Device(HWND hWnd, WINMODE WinMode, _uint iWinCX, _uint iWinCY, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut);
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

	void					Set_DepthStencilView();
	void					Set_Viewport(VIEWPORT eViewport);

	D3D11_VIEWPORT			Get_Viewport(VIEWPORT eViewport);
	ID3D11DepthStencilView* Get_ShadowDepthStencilView() { return m_pShadowDepthStencilView; };

private:
	// 메모리 할당 역할 컴객체 (정점버퍼, 인덱스버퍼, 텍스쳐로드)
	ID3D11Device*				m_pDevice = nullptr;

	// 바인딩 작업, 기능 실행 역할 컴객체 (정점 버퍼를 SetStreamSource(), SetIndices(), SetTransform(), SetTexture())
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;

	// 스왑체인 컴객체
	IDXGISwapChain*				m_pSwapChain = nullptr;

	// ID3D11ShaderResourceView*
	ID3D11RenderTargetView*		m_pBackBufferRTV = nullptr;
	ID3D11DepthStencilView*		m_pDepthStencilView = nullptr;

	// For_Shadow
	ID3D11DepthStencilView*		m_pShadowDepthStencilView = nullptr;
	D3D11_VIEWPORT				m_ViewPortDesc[VIEWPORT_END];
	
private:
	HRESULT Ready_SwapChain(HWND hWnd, WINMODE eWinMode, _uint iWinCX, _uint iWinCY);
	HRESULT Ready_BackBufferRenderTargetView();
	HRESULT Ready_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY);
public:
	virtual void Free() override;
};

END