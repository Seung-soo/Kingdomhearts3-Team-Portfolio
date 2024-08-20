#pragma once

#include "Base.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{
public:
	enum TEXTURE_TYPE { TYPE_SRV, TYPE_UAV, TYPE_END };
private:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRenderTarget() = default;
public:
	ID3D11RenderTargetView*		Get_RTV() { return m_pRTV; }
	ID3D11ShaderResourceView*	Get_SRV() { return m_pSRV; }
public:
	HRESULT	NativeConstruct(_uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, _float4 vClearColor);
	HRESULT	NativeConstruct_Cube(_uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, _float4 vClearColor);
	// ���� ������ Ŭ����
	HRESULT Clear();

	_float4x4	Get_TextureProjMatrix() { return m_RTProjMatrix;}

//#ifdef _DEBUG
public:
	// ����� �뵵�� ȭ�鿡 ����� ������ ������ �����ϴ� �Լ�
	HRESULT Ready_DebugDesc(_float fX, _float fY, _float fSizeX, _float fSizeY);
	// ����� �뵵�� ȭ�鿡 ���۸� ���
	HRESULT Render_DebugBuffer(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
//#endif
private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;
	ID3D11Texture2D*			m_pTexture2D = nullptr;
	ID3D11RenderTargetView*		m_pRTV = nullptr;
	ID3D11ShaderResourceView*	m_pSRV = nullptr;
private:
	_float4						m_vClearColor;
	_uint						m_iPass = 0;

private:
	_float4x4					m_WorldMatrix;
	_float4x4					m_ViewMatrix;
	_float4x4					m_ProjMatrix;
	_float4x4					m_RTProjMatrix; //���� �ؽ��� �����������

public:
	static CRenderTarget*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, _float4 vClearColor);
	static CRenderTarget*	CreateCube(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, _float4 vClearColor);
	virtual void			Free() override;
};

END