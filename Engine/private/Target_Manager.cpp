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
	 //버퍼와 셰이더를 할당
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
	// 해당 랜더타겟태그가 이미 생성되어 있으면 에러메세지를 낸다.
	if (nullptr != Find_RenderTarget(pRenderTargetTag))
	{
		MSGBOX("nullptr != Find_RenderTarget in CTarget_Manager::Add_RenderTarget");
		return E_FAIL;
	}

	// 랜더 타겟 생성
	CRenderTarget*	pRenderTarget = CRenderTarget::Create(pDevice, pDeviceContext, iSizeX, iSizeY, eFormat, vClearColor);
	if (nullptr == pRenderTarget)
	{
		MSGBOX("nullptr == pRenderTarget in CTarget_Manager::Add_RenderTarget");
		return E_FAIL;
	}

	// 랜더타겟을 컨테이너에 집어 넣는다.
	m_RenderTargets.emplace(pRenderTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const _tchar * pMRTTag, const _tchar * pRenderTargetTag)
{
	// 멀티랜더 타겟에 넣을 랜더타겟을 찾아서 가져옴
	CRenderTarget*	pRenderTarget = Find_RenderTarget(pRenderTargetTag);
	if (nullptr == pRenderTarget)
	{
		MSGBOX("nullptr == pRenderTarget in CTarget_Manager::Add_MRT");
		return E_FAIL;
	}
	
	// 멀티 랜더타겟을 찾는다.
	list<CRenderTarget*>*	pMRTList = Find_MRT(pMRTTag);

	// 해당 태그를 가진 멀티 랜더타겟이 없을 경우 만들어서 랜더 타겟을 넣는다.
	if (nullptr == pMRTList)
	{
		list<CRenderTarget*>	MRTList;
		MRTList.push_back(pRenderTarget);
		m_MRTs.emplace(pMRTTag, MRTList);
	}
	// 있을 경우 그냥 넣음
	else
		pMRTList->push_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Begin(ID3D11DeviceContext * pDeviceContext, const _tchar * pMRTTag)
{
	// 멀티랜더타겟 받아옴
	list<CRenderTarget*>*	pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
	{
		MSGBOX("nullptr == pMRTList in CTarget_Manager::Begin");
		return E_FAIL;
	}
	// 기존의 랜더타겟과 뎊스스텐실을 받아온다.
	pDeviceContext->OMGetRenderTargets(1, &m_pOldRTV, &m_pOriginalDSV);

	_uint iNumView = 0;

	// 8개의 랜더타겟을 받을 수 있는 배열 생성
	ID3D11RenderTargetView*		pRenderTargets[8] = { nullptr };

	// 멀티 랜더 타겟 안에 있는 랜더 타겟들을 지정된 색상으로 클리어 하고 위 랜더타겟 배열에 랜더타겟 뷰를 넣어준다.
	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();		
		pRenderTargets[iNumView++] = pRenderTarget->Get_RTV();
	}

	// 지정한 멀티 랜더 타겟뷰들을 장치에 바인딩한다.
	pDeviceContext->OMSetRenderTargets(iNumView, pRenderTargets, m_pOriginalDSV);

	return S_OK;
}

HRESULT CTarget_Manager::Begin(ID3D11DeviceContext * pDeviceContext, const _tchar * pMRTTag, ID3D11DepthStencilView * pDSV)
{
	// 멀티랜더타겟 받아옴
	list<CRenderTarget*>*	pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
	{
		MSGBOX("nullptr == pMRTList in CTarget_Manager::Begin");
		return E_FAIL;
	}
	// 기존의 랜더타겟과 뎊스스텐실을 받아온다.
	pDeviceContext->OMGetRenderTargets(1, &m_pOldRTV, &m_pOriginalDSV);

	_uint iNumView = 0;

	// 8개의 랜더타겟을 받을 수 있는 배열 생성
	ID3D11RenderTargetView*		pRenderTargets[8] = { nullptr };

	// 멀티 랜더 타겟 안에 있는 랜더 타겟들을 지정된 색상으로 클리어 하고 위 랜더타겟 배열에 랜더타겟 뷰를 넣어준다.
	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		pRenderTargets[iNumView++] = pRenderTarget->Get_RTV();
	}

	// 지정한 멀티 랜더 타겟뷰들을 장치에 바인딩한다.
	pDeviceContext->OMSetRenderTargets(iNumView, pRenderTargets, pDSV);

	return S_OK;
}

HRESULT CTarget_Manager::End(ID3D11DeviceContext * pDeviceContext, const _tchar * pMRTTag)
{
	ID3D11ShaderResourceView* pSRV[8] = { nullptr };
	pDeviceContext->PSSetShaderResources(0, 8, pSRV);
	ID3D11RenderTargetView* nullRTV[8] = { nullptr };
	pDeviceContext->OMSetRenderTargets(8, nullRTV, nullptr);

	// 다시 기존의 백버퍼를 바인딩한다.
	_uint		iNumViews = 1;

	pDeviceContext->OMSetRenderTargets(iNumViews, &m_pOldRTV, m_pOriginalDSV);

	// OMGetRenderTargets를 호출하면 랜더타겟뷰와 뎊스스텐실뷰의
	// 레퍼런스 카운트가 무조건 증가하기 때문에 레퍼런스 카운트를 낮춰준다.
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

	// DSV 클리어
	pDeviceContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	// 다시 기존의 백버퍼를 바인딩한다.
	_uint		iNumViews = 1;
	pDeviceContext->OMSetRenderTargets(iNumViews, &m_pOldRTV, m_pOriginalDSV);


	// OMGetRenderTargets를 호출하면 랜더타겟뷰와 뎊스스텐실뷰의
	// 레퍼런스 카운트가 무조건 증가하기 때문에 레퍼런스 카운트를 낮춰준다.
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
	// 원하는 멀티 랜더 타겟을 찾아주고
	list<CRenderTarget*>*	pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
	{
		MSGBOX("nullptr == pMRTList in CTarget_Manager::Render_DebugBuffer");
		return E_FAIL;
	}

	// 각각 랜더 타겟의 랜더 함수를 호출
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