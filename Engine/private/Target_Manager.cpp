#include "..\public\Target_Manager.h"
#include "RenderTarget.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include "Input_Device.h"

READY_SINGLETON(CTarget_Manager)

CTarget_Manager::CTarget_Manager()
{
}

ID3D11ShaderResourceView * CTarget_Manager::Get_SRV(const _tchar * pTargetTag) const
{
	auto iter = find_if(m_RenderTargets.begin(), m_RenderTargets.end(), CTagFinder(pTargetTag));

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second->Get_SRV();
}

HRESULT CTarget_Manager::Initialize(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
//#ifdef _DEBUG
	 //���ۿ� ���̴��� �Ҵ�
	m_pVIBuffer = CVIBuffer_Rect::Create(pDevice, pDeviceContext);
	if (nullptr == m_pVIBuffer)
	{
		MSGBOX("nullptr == m_pVIBuffer in CTarget_Manager::Initialize");
		return E_FAIL;
	}

	m_pShader = CShader::Create(pDevice, pDeviceContext, TEXT("../ShaderFiles/Shader_Deferred.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
	{
		MSGBOX("nullptr == m_pShader in CTarget_Manager::Initialize");
		return E_FAIL;
	}
//#endif
	return S_OK;
}

HRESULT CTarget_Manager::Add_RenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pRenderTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, _float4 vClearColor)
{
	// �ش� ����Ÿ���±װ� �̹� �����Ǿ� ������ �����޼����� ����.
	if (nullptr != Find_RenderTarget(pRenderTargetTag))
	{
		MSGBOX("nullptr != Find_RenderTarget in CTarget_Manager::Add_RenderTarget");
		return E_FAIL;
	}

	// ���� Ÿ�� ����
	CRenderTarget*	pRenderTarget = CRenderTarget::Create(pDevice, pDeviceContext, iSizeX, iSizeY, eFormat, vClearColor);
	if (nullptr == pRenderTarget)
	{
		MSGBOX("nullptr == pRenderTarget in CTarget_Manager::Add_RenderTarget");
		return E_FAIL;
	}

	// ����Ÿ���� �����̳ʿ� ���� �ִ´�.
	m_RenderTargets.emplace(pRenderTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const _tchar * pMRTTag, const _tchar * pRenderTargetTag)
{
	// ��Ƽ���� Ÿ�ٿ� ���� ����Ÿ���� ã�Ƽ� ������
	CRenderTarget*	pRenderTarget = Find_RenderTarget(pRenderTargetTag);
	if (nullptr == pRenderTarget)
	{
		MSGBOX("nullptr == pRenderTarget in CTarget_Manager::Add_MRT");
		return E_FAIL;
	}
	
	// ��Ƽ ����Ÿ���� ã�´�.
	list<CRenderTarget*>*	pMRTList = Find_MRT(pMRTTag);

	// �ش� �±׸� ���� ��Ƽ ����Ÿ���� ���� ��� ���� ���� Ÿ���� �ִ´�.
	if (nullptr == pMRTList)
	{
		list<CRenderTarget*>	MRTList;
		MRTList.push_back(pRenderTarget);
		m_MRTs.emplace(pMRTTag, MRTList);
	}
	// ���� ��� �׳� ����
	else
		pMRTList->push_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Begin(ID3D11DeviceContext * pDeviceContext, const _tchar * pMRTTag)
{
	// ��Ƽ����Ÿ�� �޾ƿ�
	list<CRenderTarget*>*	pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
	{
		MSGBOX("nullptr == pMRTList in CTarget_Manager::Begin");
		return E_FAIL;
	}
	// ������ ����Ÿ�ٰ� �X�����ٽ��� �޾ƿ´�.
	pDeviceContext->OMGetRenderTargets(1, &m_pOldRTV, &m_pOriginalDSV);

	_uint iNumView = 0;

	// 8���� ����Ÿ���� ���� �� �ִ� �迭 ����
	ID3D11RenderTargetView*		pRenderTargets[8] = { nullptr };

	// ��Ƽ ���� Ÿ�� �ȿ� �ִ� ���� Ÿ�ٵ��� ������ �������� Ŭ���� �ϰ� �� ����Ÿ�� �迭�� ����Ÿ�� �並 �־��ش�.
	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();		
		pRenderTargets[iNumView++] = pRenderTarget->Get_RTV();
	}

	// ������ ��Ƽ ���� Ÿ�ٺ���� ��ġ�� ���ε��Ѵ�.
	pDeviceContext->OMSetRenderTargets(iNumView, pRenderTargets, m_pOriginalDSV);

	return S_OK;
}

HRESULT CTarget_Manager::Begin(ID3D11DeviceContext * pDeviceContext, const _tchar * pMRTTag, ID3D11DepthStencilView * pDSV)
{
	// ��Ƽ����Ÿ�� �޾ƿ�
	list<CRenderTarget*>*	pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
	{
		MSGBOX("nullptr == pMRTList in CTarget_Manager::Begin");
		return E_FAIL;
	}
	// ������ ����Ÿ�ٰ� �X�����ٽ��� �޾ƿ´�.
	pDeviceContext->OMGetRenderTargets(1, &m_pOldRTV, &m_pOriginalDSV);

	_uint iNumView = 0;

	// 8���� ����Ÿ���� ���� �� �ִ� �迭 ����
	ID3D11RenderTargetView*		pRenderTargets[8] = { nullptr };

	// ��Ƽ ���� Ÿ�� �ȿ� �ִ� ���� Ÿ�ٵ��� ������ �������� Ŭ���� �ϰ� �� ����Ÿ�� �迭�� ����Ÿ�� �並 �־��ش�.
	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		pRenderTargets[iNumView++] = pRenderTarget->Get_RTV();
	}

	// ������ ��Ƽ ���� Ÿ�ٺ���� ��ġ�� ���ε��Ѵ�.
	pDeviceContext->OMSetRenderTargets(iNumView, pRenderTargets, pDSV);

	return S_OK;
}

HRESULT CTarget_Manager::End(ID3D11DeviceContext * pDeviceContext, const _tchar * pMRTTag)
{
	ID3D11ShaderResourceView* pSRV[8] = { nullptr };
	pDeviceContext->PSSetShaderResources(0, 8, pSRV);
	ID3D11RenderTargetView* nullRTV[8] = { nullptr };
	pDeviceContext->OMSetRenderTargets(8, nullRTV, nullptr);

	// �ٽ� ������ ����۸� ���ε��Ѵ�.
	_uint		iNumViews = 1;

	pDeviceContext->OMSetRenderTargets(iNumViews, &m_pOldRTV, m_pOriginalDSV);

	// OMGetRenderTargets�� ȣ���ϸ� ����Ÿ�ٺ�� �X�����ٽǺ���
	// ���۷��� ī��Ʈ�� ������ �����ϱ� ������ ���۷��� ī��Ʈ�� �����ش�.
	Safe_Release(m_pOldRTV);
	Safe_Release(m_pOriginalDSV);


	return S_OK;
}

HRESULT CTarget_Manager::End(ID3D11DeviceContext* pDeviceContext, const _tchar* pMRTTag, ID3D11DepthStencilView* pDSV)
{
	ID3D11ShaderResourceView* pSRV[8] = { nullptr };
	pDeviceContext->PSSetShaderResources(0, 8, pSRV);
	ID3D11RenderTargetView* nullRTV[8] = { nullptr };
	pDeviceContext->OMSetRenderTargets(8, nullRTV, nullptr);

	// DSV Ŭ����
	pDeviceContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	// �ٽ� ������ ����۸� ���ε��Ѵ�.
	_uint		iNumViews = 1;
	pDeviceContext->OMSetRenderTargets(iNumViews, &m_pOldRTV, m_pOriginalDSV);


	// OMGetRenderTargets�� ȣ���ϸ� ����Ÿ�ٺ�� �X�����ٽǺ���
	// ���۷��� ī��Ʈ�� ������ �����ϱ� ������ ���۷��� ī��Ʈ�� �����ش�.
	Safe_Release(m_pOldRTV);
	Safe_Release(m_pOriginalDSV);


	return S_OK;
}

_float4x4 CTarget_Manager::Get_TextureProjMatrix(const _tchar * pRenderTargetTag)
{
	CRenderTarget*	pRenderTarget = Find_RenderTarget(pRenderTargetTag);

	_float4x4	ProjMatrix;

	if (nullptr == pRenderTarget)
	{
		XMStoreFloat4x4(&ProjMatrix, XMMatrixIdentity());
	}

	else
		ProjMatrix = pRenderTarget->Get_TextureProjMatrix();

	return ProjMatrix;
}

//#ifdef _DEBUG

HRESULT CTarget_Manager::Ready_DebugDesc(const _tchar * pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget*	pRenderTarget = Find_RenderTarget(pTargetTag);

	if (nullptr == pRenderTarget)
	{
		MSGBOX("nullptr == pRenderTarget in CTarget_Manager::Ready_DebugDesc");
		return E_FAIL;
	}

	return pRenderTarget->Ready_DebugDesc(fX, fY, fSizeX, fSizeY);
}

HRESULT CTarget_Manager::Render_DebugBuffer(const _tchar * pMRTTag)
{
	if (false == m_bRender)
		return S_OK;
	// ���ϴ� ��Ƽ ���� Ÿ���� ã���ְ�
	list<CRenderTarget*>*	pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
	{
		MSGBOX("nullptr == pMRTList in CTarget_Manager::Render_DebugBuffer");
		return E_FAIL;
	}

	// ���� ���� Ÿ���� ���� �Լ��� ȣ��
	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Render_DebugBuffer(m_pShader, m_pVIBuffer);
	}
	return S_OK;
}
void CTarget_Manager::Update()
{
	CInput_Device*	pInputDevice = GET_INSTANCE(CInput_Device);


	if (true == pInputDevice->Get_KeyEnter(DIK_U))
	{
		m_bRender = !m_bRender;
	}

	 
}
//#endif

CRenderTarget * CTarget_Manager::Find_RenderTarget(const _tchar * pRenderTargetTag)
{
	auto iter = find_if(m_RenderTargets.begin(), m_RenderTargets.end(), CTagFinder(pRenderTargetTag));
	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;
}

list<CRenderTarget*>* CTarget_Manager::Find_MRT(const _tchar * pMRTTag)
{
	auto iter = find_if(m_MRTs.begin(), m_MRTs.end(), CTagFinder(pMRTTag));
	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

void CTarget_Manager::Free()
{
//#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
//#endif
	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
			Safe_Release(pRenderTarget);
		Pair.second.clear();
	}
	m_MRTs.clear();

	for (auto& Pair : m_RenderTargets)
		Safe_Release(Pair.second);
	m_RenderTargets.clear();
}