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
	// �ʱ�ȭ �Լ�
	HRESULT	Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	// ���� Ÿ���� �߰����ִ� �Լ�
	HRESULT	Add_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pRenderTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, _float4 vClearColor);
	HRESULT Add_MRT(const _tchar* pMRTTag, const _tchar* pRenderTargetTag);

	// ��ġ�� ��Ƽ����Ÿ�� ���ε�
	HRESULT Begin(ID3D11DeviceContext* pDeviceContext, const _tchar* pMRTTag);
	HRESULT Begin(ID3D11DeviceContext* pDeviceContext, const _tchar* pMRTTag, ID3D11DepthStencilView * pDSV);
	HRESULT End(ID3D11DeviceContext* pDeviceContext, const _tchar* pMRTTag);
	HRESULT End(ID3D11DeviceContext* pDeviceContext, const _tchar* pMRTTag, ID3D11DepthStencilView* pDSV);

	//�ؽ��� ������� �������� �Լ�
	_float4x4	Get_TextureProjMatrix(const _tchar* pRenderTargetTag);

//#ifdef _DEBUG

public:
	// ����� �뵵�� ȭ�鿡 ����� ������ ������ �����ϴ� �Լ�
	HRESULT Ready_DebugDesc(const _tchar* pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	// ����� �뵵�� ȭ�鿡 ���۸� ���
	HRESULT Render_DebugBuffer(const _tchar* pMRTTag);
	void	Update();
//#endif

private:
	// ����Ÿ�ٵ��� �����ϴ� �� �����̳�
	map<const _tchar*, class CRenderTarget*>			m_RenderTargets;
	typedef map<const _tchar*, class CRenderTarget*>	RENDERTARGETS;

private:
	map<const _tchar*, list<class CRenderTarget*>>			m_MRTs;
	typedef map<const _tchar*, list<class CRenderTarget*>>	MRTs;

private:
	// ������ ���� Ÿ�� �並 ������ �������
	ID3D11RenderTargetView*		m_pOldRTV = nullptr;
	// �������� ����ؾ��� �X�� ���ٽǺ�� ���� Ÿ�ٰ� ����� ���ƾ��Ѵ�.
	// ���ο� ���� Ÿ���� ���ε��� �� ����ϱ� ���� �ʿ��� ������ �X�� ���ٽǺ並 �޾ƿ� �������
	ID3D11DepthStencilView*		m_pOriginalDSV = nullptr;

//#ifdef _DEBUG

private:	// ������ ���ۿ� ���̴�
	class CVIBuffer_Rect*		m_pVIBuffer = nullptr;
	class CShader*				m_pShader = nullptr;
	_bool						m_bRender = true;
//#endif


public:
	// ����Ÿ���� ã���ִ� �Լ�
	class CRenderTarget*		Find_RenderTarget(const _tchar* pRenderTargetTag);
	// ��Ƽ ����Ÿ���� ã���ִ� �Լ�
	list<class CRenderTarget*>*	Find_MRT(const _tchar* pMRTTag);

public:
	virtual	void	Free() override;
};

END