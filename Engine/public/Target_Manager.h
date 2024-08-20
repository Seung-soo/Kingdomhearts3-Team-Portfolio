#pragma once

#include "Base.h"


BEGIN(Engine)

class CTarget_Manager final : public CBase
{
	MAKE_SINGLETON(CTarget_Manager)
private:
	CTarget_Manager();
	virtual ~CTarget_Manager() = default;

public:
	ID3D11ShaderResourceView*	Get_SRV(const _tchar* pTargetTag) const;

public:
	// 초기화 함수
	HRESULT	Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	// 랜더 타겟을 추가해주는 함수
	HRESULT	Add_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pRenderTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, _float4 vClearColor);
	HRESULT Add_MRT(const _tchar* pMRTTag, const _tchar* pRenderTargetTag);

	// 장치에 멀티랜더타겟 바인드
	HRESULT Begin(ID3D11DeviceContext* pDeviceContext, const _tchar* pMRTTag);
	HRESULT Begin(ID3D11DeviceContext* pDeviceContext, const _tchar* pMRTTag, ID3D11DepthStencilView * pDSV);
	HRESULT End(ID3D11DeviceContext* pDeviceContext, const _tchar* pMRTTag);
	HRESULT End(ID3D11DeviceContext* pDeviceContext, const _tchar* pMRTTag, ID3D11DepthStencilView* pDSV);

	//텍스쳐 투영행렬 가져오는 함수
	_float4x4	Get_TextureProjMatrix(const _tchar* pRenderTargetTag);

//#ifdef _DEBUG

public:
	// 디버깅 용도로 화면에 출력할 버퍼의 정보를 셋팅하는 함수
	HRESULT Ready_DebugDesc(const _tchar* pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	// 디버깅 용도로 화면에 버퍼를 출력
	HRESULT Render_DebugBuffer(const _tchar* pMRTTag);
	void	Update();
//#endif

private:
	// 랜더타겟들을 보관하는 맵 컨테이너
	map<const _tchar*, class CRenderTarget*>			m_RenderTargets;
	typedef map<const _tchar*, class CRenderTarget*>	RENDERTARGETS;

private:
	map<const _tchar*, list<class CRenderTarget*>>			m_MRTs;
	typedef map<const _tchar*, list<class CRenderTarget*>>	MRTs;

private:
	// 기존의 랜더 타겟 뷰를 저장할 멤버변수
	ID3D11RenderTargetView*		m_pOldRTV = nullptr;
	// 랜더링에 사용해야할 뎊스 스텐실뷰는 랜더 타겟과 사이즈가 같아야한다.
	// 새로울 랜더 타겟을 바인딩할 때 사용하기 위해 필요한 기존의 뎊스 스텐실뷰를 받아올 멤버변수
	ID3D11DepthStencilView*		m_pOriginalDSV = nullptr;

//#ifdef _DEBUG

private:	// 디버깅용 버퍼와 셰이더
	class CVIBuffer_Rect*		m_pVIBuffer = nullptr;
	class CShader*				m_pShader = nullptr;
	_bool						m_bRender = true;
//#endif


public:
	// 랜더타겟을 찾아주는 함수
	class CRenderTarget*		Find_RenderTarget(const _tchar* pRenderTargetTag);
	// 멀티 랜더타겟을 찾아주는 함수
	list<class CRenderTarget*>*	Find_MRT(const _tchar* pMRTTag);

public:
	virtual	void	Free() override;
};

END