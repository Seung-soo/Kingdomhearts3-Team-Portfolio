#include "..\public\Renderer.h"
#include "GameObject.h"
#include "Target_Manager.h"
#include "Light_Manager.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_OrthoWin.h"
#include "GameInstance.h"
#include "RenderTarget.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
	, m_pTarget_Manager(CTarget_Manager::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
{
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTarget_Manager);
}

ID3D11ShaderResourceView* CRenderer::Get_SRV(_tchar* pRenderTargetTag)
{
	return m_pTarget_Manager->Get_SRV(pRenderTargetTag);
}

_float RandomRange(_float r1, _float r2)
{
	_float random = ((_float)rand()) / (_float)RAND_MAX;
	_float diff = r2 - r1;
	_float val = random * diff;

	return r1 + val;
}

HRESULT CRenderer::NativeConstruct_Prototype()
{
	if (nullptr == m_pTarget_Manager)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	//	랜더 타겟을 추가
	if (FAILED(Add_RenderTargets()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 객체들을 그릴 때 바인드하는 멀티랜더타겟을 추가
	if (FAILED(Add_MultiRenderTargets()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 랜더러에서 사용할 컴포넌트들 세팅
	if (FAILED(SetUp_Components()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 뷰포트를 얻어온다.
	// 행렬 세팅
	_uint		iNumViewports = 1;

	D3D11_VIEWPORT		Viewport;

	m_pDeviceContext->RSGetViewports(&iNumViewports, &Viewport);

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_WorldMatrix._11 = Viewport.Width;
	m_WorldMatrix._22 = Viewport.Height;
	m_WorldMatrix._33 = 1.f;
	memcpy(&m_WorldMatrix.m[3][0], &_float4(0.f, 0.f, 0.f, 1.f), sizeof(_float4));

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix)));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f)));

//#ifdef _DEBUG
	//	랜더할 버퍼의 상태를 세팅해줌
	if (FAILED(Ready_DebugDesc()))
	{
		BREAKPOINT;
		return E_FAIL;
	}
//#endif
	
	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	iNumViewports = 1;

	m_pDeviceContext->RSGetViewports(&iNumViewports, &Viewport);

	TextureDesc.Width = (_uint)(Viewport.Width * 0.5f);
	TextureDesc.Height = (_uint)(Viewport.Height * 0.5f);
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

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDownDSV)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);


	// 다운/업 스케일을 위한 창 세팅하기
	m_pScaleWIn[DOWNSCALE] = CVIBuffer_OrthoWin::Create(m_pDevice, m_pDeviceContext, (_int)(Viewport.Width * 0.5f), (_int)(Viewport.Height * 0.5f));
	m_pScaleWIn[UPSACLE] = CVIBuffer_OrthoWin::Create(m_pDevice, m_pDeviceContext, (_int)(Viewport.Width), (_int)(Viewport.Height));


	if (FAILED(Make_HDRDepthStencil(_uint(Viewport.Width * 0.5f), _uint(Viewport.Height * 0.5f), 0)))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(Make_HDRDepthStencil(256, 256, 1)))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(Make_HDRDepthStencil(64, 64, 2)))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(Make_HDRDepthStencil(16, 16, 3)))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(Make_HDRDepthStencil(4, 4, 4)))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(Make_HDRDepthStencil(1, 1, 5)))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(Make_HDRDepthStencil(_uint(Viewport.Width * 0.25f), _uint(Viewport.Height * 0.25f), 6)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	//if (FAILED(Make_CubeDepthStencil()))
	//{
	//	BREAKPOINT;
	//	return E_FAIL;
	//}

	// 랜덤 방향 벡터
	for (_uint i = 0; i < 64; i++)
	{
		_float	t = (_float)i / 64.f;
		_float	t2 = t * t;
		_float	fScale = (1.f - t2) * 0.1f + t2 * 1.f;

		m_vRandomDir[i] = _float4(RandomRange(-1.f, 1.f), RandomRange(-1.f, 1.f), RandomRange(0.f, 1.f), 0.f);
		XMStoreFloat4(&m_vRandomDir[i], XMVector3Normalize(XMLoadFloat4(&m_vRandomDir[i])) * RandomRange(0.f, 1.f) * fScale);
	}



	return S_OK;
}

HRESULT CRenderer::NativeConstruct(void* pArg)
{

	
	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject * pRenderObject)
{
	if (eRenderGroup >= RENDER_END)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (nullptr == pRenderObject)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_RenderObjects[eRenderGroup].push_back(pRenderObject);

	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CRenderer::Clear_Renderer()
{
	for (auto& RenderObjects : m_RenderObjects)
	{
		for (auto& pGameObject : RenderObjects)
			Safe_Release(pGameObject);
		RenderObjects.clear();
	}

	return S_OK;
}

HRESULT CRenderer::Reset_Values(_bool isRenderShadow)
{
	m_isStaticShadowRender = false;
	m_isRenderShadow = isRenderShadow;

	m_pLight_Manager->Set_RenderShadow(isRenderShadow);

	if (false == m_isRenderShadow)
	{
		auto pTarget = m_pTarget_Manager->Find_RenderTarget(TEXT("Target_Shadow"));
	
		pTarget->Clear();

		pTarget = m_pTarget_Manager->Find_RenderTarget(TEXT("Target_Shadow_Static"));

		pTarget->Clear();

		pTarget = m_pTarget_Manager->Find_RenderTarget(TEXT("Target_UpSample_Shadow"));

		pTarget->Clear();
	}

	return S_OK;
}

void CRenderer::Set_LUTDesc(_bool isUsing, _uint TexIndex)
{
	Set_LUTTexIndex(TexIndex);
	Set_UsingLUT(isUsing);
}

//#ifdef _DEBUG

HRESULT CRenderer::Add_Debug(CComponent* pComponent)
{
	if (true == m_bDebugRenderController)
	{
		m_DebugObject.push_back(pComponent);
		Safe_AddRef(pComponent);
	}

	return S_OK;
}
//#endif

HRESULT CRenderer::Render()
{
	//if (FAILED(Render_Priority()))
	//{
	//	BREAKPOINT;
	//	return E_FAIL;
	//}

	if (FAILED(Render_Shadow()))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	if (FAILED(Render_NonAlpha()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(Render_SSAO()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(Render_Effect()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(Render_Lights()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 논블랜드만 찍혀있는 텍스쳐 생성
	if (FAILED(Render_Calculation()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (true == m_bFog)
	{
		if (FAILED(Render_Fog()))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	if (true == m_bHDR)
	{
		if (FAILED(Render_HDRDownScale()))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		if (FAILED(CalcAverLum()))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		if (FAILED(Render_ToneMapping()))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		// 이전 평균휘도값 기록
		if (FAILED(Render_PrevAverLum()))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}
	
	if (true == m_bBloom)
	{
		if (FAILED(Render_Bloom()))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	// LUT
	if (FAILED(LUT_Filtering()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// MotionBlur
	if (FAILED(Render_MotionBlur()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// DOF
	if (true == m_bDOF)
	{
		if (FAILED(Render_DOF()))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}


	// 알파만 모여있는 텍스쳐 생성
	if (FAILED(Render_NonLight()))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(Render_Alpha()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(Render_Effect()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 블러까지 합한 텍스쳐 생성
	if (FAILED(Render_Combine()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 왜곡을 먹인 텍스쳐 생성
	if (FAILED(Distortion()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (true == m_bRadialBlur)
	{
		// 레디얼 블러를 먹인 텍스쳐 생성
		if (FAILED(Render_RadialBlur()))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	// 마지막으로 백버퍼에 그림
	if (FAILED(Render_Final()))
	{
		BREAKPOINT;
		return E_FAIL;
	}


#ifdef _DEBUG
	if (FAILED(Render_Debug()))
	{
		BREAKPOINT;
		return E_FAIL;
	}
#endif
	if (FAILED(Render_UI()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

#ifdef _DEBUG

	

	if (m_bDebugRenderController == true)
	{
		if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_Deferred"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_LightAcc"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_DownSample"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_UpSample"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_BlurX"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_BlurY"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_Combine"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_Distortion"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_NonBlendTexture"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_AlphaTexture"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_HDR_SixthDownScale"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_ToneMapping"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_BloomMask"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_BloomMapping"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_DOF"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_SSAO_BlurY"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_Shadow"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_Shadow_Static"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_UpSample_Shadow"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_LUT"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		if (FAILED(m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_Fog"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}
#endif

	return S_OK;
}

HRESULT CRenderer::Render_Shadow()
{
	if(false == m_isRenderShadow)
		return S_OK;

	// 뷰포트 쉐도우용으로 세팅
	CGraphic_Device::GetInstance()->Set_Viewport(CGraphic_Device::SHADOW);

	for (auto& pRenderObject : m_RenderObjects[RENDER_NONBLEND])
	{
		if (true == pRenderObject->Is_ShadowStatic())
		{
			if (false == m_isStaticShadowRender)
			{
				m_ShadowObject[SHADOW_STATIC].push_back(pRenderObject);
				Safe_AddRef(pRenderObject);
			}
		}

		else
		{
			m_ShadowObject[SHADOW_DYNAMIC].push_back(pRenderObject);
			Safe_AddRef(pRenderObject);
		}
	}
	
	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_Shadow"), CGraphic_Device::GetInstance()->Get_ShadowDepthStencilView())))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Dynamic Shadow
	for (auto& pRenderObject : m_ShadowObject[SHADOW_DYNAMIC])
	{
		if (nullptr != pRenderObject)
		{
			if (FAILED(pRenderObject->Render_Shadow()))
			{
				BREAKPOINT;
				return E_FAIL;
			}
			Safe_Release(pRenderObject);
		}
	}
	m_ShadowObject[SHADOW_DYNAMIC].clear();

	// 다시 백버퍼를 바인딩
	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_Shadow"), CGraphic_Device::GetInstance()->Get_ShadowDepthStencilView())))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (false == m_isStaticShadowRender && m_ShadowObject[SHADOW_STATIC].size() != 0)
	{
		// Static Shadow
		if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_Shadow_Static"), CGraphic_Device::GetInstance()->Get_ShadowDepthStencilView())))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		for (auto& pRenderObject : m_ShadowObject[SHADOW_STATIC])
		{
			if (nullptr != pRenderObject)
			{
				if (FAILED(pRenderObject->Render_Shadow()))
				{
					BREAKPOINT;
					return E_FAIL;
				}

				Safe_Release(pRenderObject);
			}
		}
		m_ShadowObject[SHADOW_STATIC].clear();

		// 다시 백버퍼를 바인딩
		if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_Shadow_Static"), CGraphic_Device::GetInstance()->Get_ShadowDepthStencilView())))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		m_isStaticShadowRender = true;
	}

	ID3D11ShaderResourceView* pSRV = NULL;

	m_pDeviceContext->PSSetShaderResources(0, 1, &pSRV);

	// 다시 기본으로 세팅
	CGraphic_Device::GetInstance()->Set_Viewport(CGraphic_Device::DEFAULT);

	return S_OK;
}

HRESULT CRenderer::Shadow_Blur()
{
	if (FAILED(Shadow_DownSample()))
		return E_FAIL;

	_float		fAlpha = 1.f;

	if (FAILED(m_pShader->Set_RawValue("g_fAlpha", &fAlpha, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(Blur()))
		return E_FAIL;

	if (FAILED(Shadow_UpSample()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Shadow_DownSample()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_DownSample"), m_pDownDSV)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	//SetUp_ConstantTable ================================================================
	_float4x4		WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	D3D11_VIEWPORT		Viewport;
	_uint				iNumViewports = 1;

	m_pDeviceContext->RSGetViewports(&iNumViewports, &Viewport);

	_float		fScreenX = Viewport.Width * 0.5f;
	_float		fScreenY = Viewport.Height * 0.5f;

	WorldMatrix._11 = fScreenX;
	WorldMatrix._22 = fScreenY;
	WorldMatrix._33 = 1.f;

	memcpy(&WorldMatrix.m[3][0], &_float4(-fScreenX * 0.5f, fScreenY * 0.5f, 0.f, 1.f), sizeof(_float4));

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	_float4x4		ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float4x4		ProjMatrix;
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f)));

	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Shadow_Final"))))
	//====================================================================================================================

	m_pVIBuffer->Render(m_pShader, 0);

	// 다시 백버퍼를 바인딩
	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_DownSample"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	ID3D11ShaderResourceView* pSRV = NULL;
	m_pDeviceContext->PSSetShaderResources(0, 1, &pSRV);

	return S_OK;
}

HRESULT CRenderer::Shadow_UpSample()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_UpSample_Shadow"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	//SetUp_ConstantTable ================================================================
	_float4x4	WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &WorldMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Verti"))))

	//====================================================================================================================

	m_pScaleWIn[UPSACLE]->Render(m_pShader, 0);

	// 다시 백버퍼를 바인딩
	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_UpSample_Shadow"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	ID3D11ShaderResourceView* pSRV = NULL;
	m_pDeviceContext->PSSetShaderResources(0, 1, &pSRV);

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if (nullptr != pRenderObject)
		{
			if (FAILED(pRenderObject->Render()))
			{
				BREAKPOINT;
				return E_FAIL;
			}
		}
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonAlpha()
{
	// 디퍼드 멀티랜더타겟을 장치에 바인딩
	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_Deferred"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	for (auto& pRenderObject : m_RenderObjects[RENDER_NONBLEND])
	{
		if (nullptr != pRenderObject)
		{

			if (FAILED(pRenderObject->Render()))
			{
				BREAKPOINT;
				return E_FAIL;
			}

		}
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_NONBLEND].clear();

	// 다시 백버퍼를 바인딩
	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_Deferred"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	ID3D11ShaderResourceView* pSRV = NULL;

	m_pDeviceContext->PSSetShaderResources(0, 1, &pSRV);

	return S_OK;
}

HRESULT CRenderer::Render_Effect()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_LevelIndex() < 3)
	{
		return S_OK;
	}

	if (FAILED(DownSampling()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float		fAlpha = 0.f;

	if (FAILED(m_pShader->Set_RawValue("g_fAlpha", &fAlpha, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(Blur()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(UpSampling()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
	if (nullptr == m_pTarget_Manager)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_LightAcc"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	m_pLight_Manager->Set_SSAO(m_bSSAO);
	m_pLight_Manager->Render();

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_LightAcc"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	ID3D11ShaderResourceView* pSRV = NULL;
	m_pDeviceContext->PSSetShaderResources(0, 1, &pSRV);

	Shadow_Blur();

	return S_OK;
}

HRESULT CRenderer::Render_Calculation()
{
	RETURN_FAIL(if (nullptr == m_pVIBuffer || nullptr == m_pShader));

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_NonBlendTexture"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	Render_Priority();

	CHECK_FAILED(m_pShader->Set_Texture("g_BlurTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_UpSample"))));

	CHECK_FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Diffuse"))));

	CHECK_FAILED(m_pShader->Set_Texture("g_ShadeTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Shade"))));

	CHECK_FAILED(m_pShader->Set_Texture("g_ShadowTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_UpSample_Shadow"))));

	CHECK_FAILED(m_pShader->Set_Texture("g_SpecularTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Specular"))));

	CHECK_FAILED(m_pShader->Set_Texture("g_ThirdTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Emissive"))));

	CHECK_FAILED(m_pShader->Set_Texture("g_RimTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_RimLight"))));

	CHECK_FAILED(m_pShader->Set_Texture("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Outline"))));

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	if (false == m_bOutline)
		m_pVIBuffer->Render(m_pShader, 3);
	else
		m_pVIBuffer->Render(m_pShader, 22);

	ID3D11ShaderResourceView* pSRV = NULL;
	m_pDeviceContext->PSSetShaderResources(4, 1, &pSRV);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_NonBlendTexture"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}



	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_AlphaTexture"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 논블랜드 텍스쳐를 미리 랜더타겟에 깔아놓음
	if (true == m_bDOF)
	{
		CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_DOF"))));
	}
	else
	{
		CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_MotionBlur"))));
	}

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));


	m_pVIBuffer->Render(m_pShader, 0);


	for (auto& pRenderObject : m_RenderObjects[RENDER_NONLIGHT])
	{
		if (nullptr != pRenderObject)
		{
			if (FAILED(pRenderObject->Render()))
			{
				BREAKPOINT;
				return E_FAIL;
			}
		}
		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Alpha()
{
	m_RenderObjects[RENDER_BLEND].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
		{
			return pSour->Get_AlphaDepth() > pDest->Get_AlphaDepth();
		});

	for (auto& pRenderObject : m_RenderObjects[RENDER_BLEND])
	{
		if (nullptr != pRenderObject)
		{
			if (FAILED(pRenderObject->Render()))
			{
				BREAKPOINT;
				return E_FAIL;
			}
		}
		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RENDER_BLEND].clear();

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_AlphaTexture"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_Combine()
{
	RETURN_FAIL(if (nullptr == m_pVIBuffer || nullptr == m_pShader));

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_Combine"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CHECK_FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_AlphaTexture"))));
	
	CHECK_FAILED(m_pShader->Set_Texture("g_NormalTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_UpSample"))));

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	m_pVIBuffer->Render(m_pShader, 9);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_Combine"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	m_RenderObjects[RENDER_UI].sort(DepthSort<CGameObject*>);
	for (auto& pRenderObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pRenderObject)
		{
			if (FAILED(pRenderObject->Render()))
			{
				BREAKPOINT;
				return E_FAIL;
			}
		}
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::HDR_FirstDownScale()
{
	RETURN_FAIL(if (nullptr == m_pVIBuffer || nullptr == m_pShader));

	_uint		iNumViewports = 1;

	D3D11_VIEWPORT		Viewport;
	Viewport.Height = 360.f;
	Viewport.Width = 640.f;
	Viewport.MinDepth = 0.f;
	Viewport.MaxDepth = 1.f;
	Viewport.TopLeftX = 0.f;
	Viewport.TopLeftY = 0.f;

	m_pDeviceContext->RSSetViewports(iNumViewports, &Viewport);

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_HDR_FirstDownScale"), m_pHDRDownDSV[0])))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (true == m_bFog)
	{
		CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Fog"))));
	}
	else
	{
		CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_NonBlendTexture"))));
	}

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	_float fWidth = 640.f;
	_float fHeight = 360.f;
	CHECK_FAILED(m_pShader->Set_RawValue("ScreenWidth", &fWidth, sizeof(_float)));
	CHECK_FAILED(m_pShader->Set_RawValue("ScreenHeight", &fHeight, sizeof(_float)));

	m_pVIBuffer->Render(m_pShader, 4);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_HDR_FirstDownScale"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_HDR_FirstDownScaleBlur"), m_pHDRDownDSV[0])))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_HDR_FirstDownScale"))));

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	m_pVIBuffer->Render(m_pShader, 5);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_HDR_FirstDownScale"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::HDR_SecondDownScale()
{
	RETURN_FAIL(if (nullptr == m_pVIBuffer || nullptr == m_pShader));

	_uint		iNumViewports = 1;

	D3D11_VIEWPORT		Viewport;
	Viewport.Height = 256.f;
	Viewport.Width = 256.f;
	Viewport.MinDepth = 0.f;
	Viewport.MaxDepth = 1.f;
	Viewport.TopLeftX = 0.f;
	Viewport.TopLeftY = 0.f;

	m_pDeviceContext->RSSetViewports(iNumViewports, &Viewport);


	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_HDR_SecondDownScale"), m_pHDRDownDSV[1])))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_HDR_FirstDownScaleBlur"))));

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	_float fWidth = 256.f;
	_float fHeight = 256.f;
	CHECK_FAILED(m_pShader->Set_RawValue("ScreenWidth", &fWidth, sizeof(_float)));
	CHECK_FAILED(m_pShader->Set_RawValue("ScreenHeight", &fHeight, sizeof(_float)));

	m_pVIBuffer->Render(m_pShader, 4);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_HDR_SecondDownScale"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_HDR_SecondDownScaleBlur"), m_pHDRDownDSV[1])))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_HDR_SecondDownScale"))));

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	m_pVIBuffer->Render(m_pShader, 5);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_HDR_SecondDownScaleBlur"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::HDR_ThirdDownScale()
{
	RETURN_FAIL(if (nullptr == m_pVIBuffer || nullptr == m_pShader));

	_uint		iNumViewports = 1;

	D3D11_VIEWPORT		Viewport;
	Viewport.Height = 64.f;
	Viewport.Width = 64.f;
	Viewport.MinDepth = 0.f;
	Viewport.MaxDepth = 1.f;
	Viewport.TopLeftX = 0.f;
	Viewport.TopLeftY = 0.f;

	m_pDeviceContext->RSSetViewports(iNumViewports, &Viewport);

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_HDR_ThirdDownScale"), m_pHDRDownDSV[2])))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_HDR_SecondDownScaleBlur"))));

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	_float fWidth = 64.f;
	_float fHeight = 64.f;
	CHECK_FAILED(m_pShader->Set_RawValue("ScreenWidth", &fWidth, sizeof(_float)));
	CHECK_FAILED(m_pShader->Set_RawValue("ScreenHeight", &fHeight, sizeof(_float)));

	m_pVIBuffer->Render(m_pShader, 4);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_HDR_ThirdDownScale"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_HDR_ThirdDownScaleBlur"), m_pHDRDownDSV[2])))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_HDR_ThirdDownScale"))));

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	m_pVIBuffer->Render(m_pShader, 5);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_HDR_ThirdDownScaleBlur"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::HDR_FourthDownScale()
{
	RETURN_FAIL(if (nullptr == m_pVIBuffer || nullptr == m_pShader));

	_uint		iNumViewports = 1;

	D3D11_VIEWPORT		Viewport;
	Viewport.Height = 16.f;
	Viewport.Width = 16.f;
	Viewport.MinDepth = 0.f;
	Viewport.MaxDepth = 1.f;
	Viewport.TopLeftX = 0.f;
	Viewport.TopLeftY = 0.f;

	m_pDeviceContext->RSSetViewports(iNumViewports, &Viewport);

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_HDR_FourthDownScale"), m_pHDRDownDSV[3])))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_HDR_ThirdDownScaleBlur"))));

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	_float fWidth = 16.f;
	_float fHeight = 16.f;
	CHECK_FAILED(m_pShader->Set_RawValue("ScreenWidth", &fWidth, sizeof(_float)));
	CHECK_FAILED(m_pShader->Set_RawValue("ScreenHeight", &fHeight, sizeof(_float)));

	m_pVIBuffer->Render(m_pShader, 4);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_HDR_FourthDownScale"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_HDR_FourthDownScaleBlur"), m_pHDRDownDSV[3])))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_HDR_FourthDownScale"))));

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	m_pVIBuffer->Render(m_pShader, 5);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_HDR_FourthDownScaleBlur"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::HDR_FifthDownScale()
{
	RETURN_FAIL(if (nullptr == m_pVIBuffer || nullptr == m_pShader));

	_uint		iNumViewports = 1;

	D3D11_VIEWPORT		Viewport;
	Viewport.Height = 4.f;
	Viewport.Width = 4.f;
	Viewport.MinDepth = 0.f;
	Viewport.MaxDepth = 1.f;
	Viewport.TopLeftX = 0.f;
	Viewport.TopLeftY = 0.f;

	m_pDeviceContext->RSSetViewports(iNumViewports, &Viewport);

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_HDR_FifthDownScale"), m_pHDRDownDSV[4])))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_HDR_FourthDownScaleBlur"))));

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	_float fWidth = 4.f;
	_float fHeight = 4.f;
	CHECK_FAILED(m_pShader->Set_RawValue("ScreenWidth", &fWidth, sizeof(_float)));
	CHECK_FAILED(m_pShader->Set_RawValue("ScreenHeight", &fHeight, sizeof(_float)));

	m_pVIBuffer->Render(m_pShader, 4);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_HDR_FifthDownScale"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_HDR_FifthDownScaleBlur"), m_pHDRDownDSV[4])))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_HDR_FifthDownScale"))));

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	m_pVIBuffer->Render(m_pShader, 5);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_HDR_FifthDownScaleBlur"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::HDR_SixthDownScale()
{
	RETURN_FAIL(if (nullptr == m_pVIBuffer || nullptr == m_pShader));

	_uint		iNumViewports = 1;

	D3D11_VIEWPORT		Viewport;
	Viewport.Height = 1.f;
	Viewport.Width = 1.f;
	Viewport.MinDepth = 0.f;
	Viewport.MaxDepth = 1.f;
	Viewport.TopLeftX = 0.f;
	Viewport.TopLeftY = 0.f;

	m_pDeviceContext->RSSetViewports(iNumViewports, &Viewport);

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_HDR_SixthDownScale"), m_pHDRDownDSV[5])))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_HDR_FifthDownScaleBlur"))));

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	m_pVIBuffer->Render(m_pShader, 13);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_HDR_SixDownScale"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::CalcAverLum()
{

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_CalcAverLum"), m_pHDRDownDSV[5])))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_HDR_SixthDownScale"))));
	if (1 >= m_iStartLum)
	{
		CHECK_FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_HDR_SixthDownScale"))));
	}
	else
	{
		CHECK_FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_PrevAverLum"))));
	}

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	m_pVIBuffer->Render(m_pShader, 14);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_CalcAverLum"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	m_iStartLum++;

	_uint		iNumViewports = 1;

	D3D11_VIEWPORT		Viewport;

	Viewport.Height = 720.f;
	Viewport.Width = 1280.f;
	Viewport.MinDepth = 0.f;
	Viewport.MaxDepth = 1.f;
	Viewport.TopLeftX = 0.f;
	Viewport.TopLeftY = 0.f;

	m_pDeviceContext->RSSetViewports(iNumViewports, &Viewport);

	return S_OK;
}

HRESULT CRenderer::Render_HDRDownScale()
{
	if (FAILED(HDR_FirstDownScale()))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(HDR_SecondDownScale()))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(HDR_ThirdDownScale()))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(HDR_FourthDownScale()))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(HDR_FifthDownScale()))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(HDR_SixthDownScale()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_ToneMapping()
{

	RETURN_FAIL(if (nullptr == m_pVIBuffer || nullptr == m_pShader));

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_ToneMapping"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (true == m_bFog)
	{
		CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Fog"))));
	}
	else
	{
		CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_NonBlendTexture"))));
	}
	CHECK_FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_CalcAverLum"))));

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_fGrayScale", &m_fHDR_GrayScale, sizeof(_float)));
	CHECK_FAILED(m_pShader->Set_RawValue("g_fWhite", &m_fHDE_White, sizeof(_float)));

	m_pVIBuffer->Render(m_pShader, 10);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_ToneMapping"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_PrevAverLum()
{
	_uint		iNumViewports = 1;

	D3D11_VIEWPORT		Viewport;
	Viewport.Height = 1.f;
	Viewport.Width = 1.f;
	Viewport.MinDepth = 0.f;
	Viewport.MaxDepth = 1.f;
	Viewport.TopLeftX = 0.f;
	Viewport.TopLeftY = 0.f;

	m_pDeviceContext->RSSetViewports(iNumViewports, &Viewport);

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_PrevAverLum"), m_pHDRDownDSV[5])))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_CalcAverLum"))));

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	m_pVIBuffer->Render(m_pShader, 0);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_PrevAverLum"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	Viewport.Height = 720.f;
	Viewport.Width = 1280.f;
	Viewport.MinDepth = 0.f;
	Viewport.MaxDepth = 1.f;
	Viewport.TopLeftX = 0.f;
	Viewport.TopLeftY = 0.f;

	m_pDeviceContext->RSSetViewports(iNumViewports, &Viewport);

	return S_OK;
}

HRESULT CRenderer::Bloom_Mask()
{
	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_BloomMask"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (true == m_bHDR)
	{
		CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_ToneMapping"))));
	}
	else
	{
		if (true == m_bFog)
		{
			CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Fog"))));
		}
		else
		{
			CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_NonBlendTexture"))));
		}
	}

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	m_pVIBuffer->Render(m_pShader, 11);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_BloomMask"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Bloom_Down()
{
	RETURN_FAIL(if (nullptr == m_pVIBuffer || nullptr == m_pShader));

	_uint		iNumViewports = 1;

	D3D11_VIEWPORT		Viewport;
	Viewport.Height = 180.f;
	Viewport.Width = 320.f;
	Viewport.MinDepth = 0.f;
	Viewport.MaxDepth = 1.f;
	Viewport.TopLeftX = 0.f;
	Viewport.TopLeftY = 0.f;

	m_pDeviceContext->RSSetViewports(iNumViewports, &Viewport);

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_BloomDown"), m_pHDRDownDSV[6])))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_BloomMask"))));

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	m_pVIBuffer->Render(m_pShader, 0);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_BloomDown"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Bloom_BlurX()
{
	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_BloomBlurX"), m_pHDRDownDSV[6])))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_BloomDown"))));

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	_float fWidth = 320.f;
	_float fHeight = 180.f;
	CHECK_FAILED(m_pShader->Set_RawValue("ScreenWidth", &fWidth, sizeof(_float)));
	CHECK_FAILED(m_pShader->Set_RawValue("ScreenHeight", &fHeight, sizeof(_float)));

	m_pVIBuffer->Render(m_pShader, 4);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_BloomBlurX"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Bloom_BlurY()
{
	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_BloomBlurY"), m_pHDRDownDSV[6])))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_BloomBlurX"))));

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	_float fWidth = 320.f;
	_float fHeight = 180.f;
	CHECK_FAILED(m_pShader->Set_RawValue("ScreenWidth", &fWidth, sizeof(_float)));
	CHECK_FAILED(m_pShader->Set_RawValue("ScreenHeight", &fHeight, sizeof(_float)));

	m_pVIBuffer->Render(m_pShader, 5);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_BloomBlurY"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Bloom_Up()
{
	_uint		iNumViewports = 1;

	D3D11_VIEWPORT		Viewport;
	Viewport.Height = 720.f;
	Viewport.Width = 1280.f;
	Viewport.MinDepth = 0.f;
	Viewport.MaxDepth = 1.f;
	Viewport.TopLeftX = 0.f;
	Viewport.TopLeftY = 0.f;

	m_pDeviceContext->RSSetViewports(iNumViewports, &Viewport);

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_BloomUp"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_BloomBlurY"))));

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));


	m_pVIBuffer->Render(m_pShader, 0);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_BloomUp"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Bloom_Mapping()
{
	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_BloomMapping"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (true == m_bHDR)
	{
		CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_ToneMapping"))));
	}
	else
	{
		if (true == m_bFog)
		{
			CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Fog"))));
		}
		else
		{
			CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_NonBlendTexture"))));
		}
	}
	CHECK_FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_BloomUp"))));
	CHECK_FAILED(m_pShader->Set_Texture("g_NormalTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_BloomMask"))));

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));


	m_pVIBuffer->Render(m_pShader, 12);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_BloomMapping"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_Bloom()
{
	if (FAILED(Bloom_Mask()))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if(FAILED(Bloom_Down()))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if(FAILED(Bloom_BlurX()))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if(FAILED(Bloom_BlurY()))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if(FAILED(Bloom_Up()))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if(FAILED(Bloom_Mapping()))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CRenderer::DOF_BlurX()
{
	_uint		iNumViewports = 1;

	D3D11_VIEWPORT		Viewport;
	Viewport.Height = 180.f;
	Viewport.Width = 320.f;
	Viewport.MinDepth = 0.f;
	Viewport.MaxDepth = 1.f;
	Viewport.TopLeftX = 0.f;
	Viewport.TopLeftY = 0.f;

	m_pDeviceContext->RSSetViewports(iNumViewports, &Viewport);

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_DOF_BlurX"), m_pHDRDownDSV[6])))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	//CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_BloomMapping"))));


	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_MotionBlur"))));


	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	_float fWidth = 320.f;
	_float fHeight = 180.f;
	CHECK_FAILED(m_pShader->Set_RawValue("ScreenWidth", &fWidth, sizeof(_float)));
	CHECK_FAILED(m_pShader->Set_RawValue("ScreenHeight", &fHeight, sizeof(_float)));


	m_pVIBuffer->Render(m_pShader, 16);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_DOF_BlurX"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::DOF_BlurY()
{
	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_DOF_BlurY"), m_pHDRDownDSV[6])))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_DOF_BlurX"))));

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	_float fWidth = 320.f;
	_float fHeight = 180.f;
	CHECK_FAILED(m_pShader->Set_RawValue("ScreenWidth", &fWidth, sizeof(_float)));
	CHECK_FAILED(m_pShader->Set_RawValue("ScreenHeight", &fHeight, sizeof(_float)));


	m_pVIBuffer->Render(m_pShader, 17);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_DOF_BlurY"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_uint		iNumViewports = 1;

	D3D11_VIEWPORT		Viewport;
	Viewport.Height = 720.f;
	Viewport.Width = 1280.f;
	Viewport.MinDepth = 0.f;
	Viewport.MaxDepth = 1.f;
	Viewport.TopLeftX = 0.f;
	Viewport.TopLeftY = 0.f;

	m_pDeviceContext->RSSetViewports(iNumViewports, &Viewport);

	return S_OK;
}

HRESULT CRenderer::Render_DOF()
{
	if (FAILED(DOF_BlurX()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(DOF_BlurY()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_DOF"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	//CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_BloomMapping"))));

	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_MotionBlur"))));

	CHECK_FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_DOF_BlurY"))));
	CHECK_FAILED(m_pShader->Set_Texture("g_NormalTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth"))));

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));



	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	_float4x4		ViewMatrixInv, ProjMatrixInv;
	XMStoreFloat4x4(&ViewMatrixInv, XMMatrixTranspose(XMMatrixInverse(nullptr, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))));
	XMStoreFloat4x4(&ProjMatrixInv, XMMatrixTranspose(XMMatrixInverse(nullptr, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))));


	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrixInv", &ViewMatrixInv, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrixInv", &ProjMatrixInv, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	CHECK_FAILED(m_pShader->Set_RawValue("g_vCamPosition", &m_vFocusPosition, sizeof(_float4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_fMinDistance", &m_fDOF_MinDistance, sizeof(_float)));
	CHECK_FAILED(m_pShader->Set_RawValue("g_fMaxDistance", &m_fDOF_MaxDistance, sizeof(_float)));


	m_pVIBuffer->Render(m_pShader, 15);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_DOF"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_SSAO()
{
	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_SSAO"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	//CHECK_FAILED(m_pSSAOShader->Set_Texture("g_SSAOTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Position"))));

	CHECK_FAILED(m_pSSAOShader->Set_Texture("g_SSAONormalTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Normal"))));
	CHECK_FAILED(m_pSSAOShader->Set_Texture("g_SSAODepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth"))));	

	CHECK_FAILED(m_pSSAOShader->Set_RawValue("g_SSAOWorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pSSAOShader->Set_RawValue("g_SSAOViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pSSAOShader->Set_RawValue("g_SSAOProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	CPipeLine*	pPipeLine = GET_INSTANCE(CPipeLine);

	_float4x4 ProjMatrixInv;
	XMStoreFloat4x4(&ProjMatrixInv, XMMatrixTranspose(XMMatrixInverse(nullptr, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))));
	CHECK_FAILED(m_pSSAOShader->Set_RawValue("g_SSAOProjMatrixInv", &ProjMatrixInv, sizeof(_float4x4)));

	CHECK_FAILED(m_pSSAOShader->Set_RawValue("g_RealViewMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4)));
	CHECK_FAILED(m_pSSAOShader->Set_RawValue("g_RealProjMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4)));

	
	CHECK_FAILED(m_pSSAOShader->Set_RawValue("g_fWinSizeX", &m_fWinCX, sizeof(_float)));
	CHECK_FAILED(m_pSSAOShader->Set_RawValue("g_fWinSizeY", &m_fWinCY, sizeof(_float)));

	CHECK_FAILED(m_pSSAOShader->Set_RawValue("g_fRadius", &m_fSSAO_Radius, sizeof(_float)));
	CHECK_FAILED(m_pSSAOShader->Set_RawValue("g_fBias", &m_fSSAO_Bias, sizeof(_float)));

	CHECK_FAILED(m_pSSAOShader->Set_RawValue("vSamples", m_vRandomDir, sizeof(_float4) * 64));

	m_pVIBuffer->Render(m_pSSAOShader, 3);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_SSAO"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(SSAO_BlurX()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(SSAO_BlurY()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::SSAO_BlurX()
{
	_uint		iNumViewports = 1;

	D3D11_VIEWPORT		Viewport;
	Viewport.Height = 360.f;
	Viewport.Width = 640.f;
	Viewport.MinDepth = 0.f;
	Viewport.MaxDepth = 1.f;
	Viewport.TopLeftX = 0.f;
	Viewport.TopLeftY = 0.f;

	m_pDeviceContext->RSSetViewports(iNumViewports, &Viewport);

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_SSAO_BlurX"), m_pDownDSV)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CHECK_FAILED(m_pSSAOShader->Set_Texture("g_SSAOTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_SSAO"))));

	CHECK_FAILED(m_pSSAOShader->Set_RawValue("g_SSAOWorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pSSAOShader->Set_RawValue("g_SSAOViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pSSAOShader->Set_RawValue("g_SSAOProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	_float fWidth = 640.f;
	_float fHeight = 360.f;
	CHECK_FAILED(m_pSSAOShader->Set_RawValue("g_fWinSizeX", &fWidth, sizeof(_float)));
	CHECK_FAILED(m_pSSAOShader->Set_RawValue("g_fWinSizeY", &fHeight, sizeof(_float)));


	m_pVIBuffer->Render(m_pSSAOShader, 1);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_SSAO_BlurX"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::SSAO_BlurY()
{
	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_SSAO_BlurY"), m_pDownDSV)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CHECK_FAILED(m_pSSAOShader->Set_Texture("g_SSAOTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_SSAO_BlurX"))));

	CHECK_FAILED(m_pSSAOShader->Set_RawValue("g_SSAOWorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pSSAOShader->Set_RawValue("g_SSAOViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pSSAOShader->Set_RawValue("g_SSAOProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	_float fWidth = 640.f;
	_float fHeight = 360.f;
	CHECK_FAILED(m_pSSAOShader->Set_RawValue("g_fWinSizeX", &fWidth, sizeof(_float)));
	CHECK_FAILED(m_pSSAOShader->Set_RawValue("g_fWinSizeY", &fHeight, sizeof(_float)));


	m_pVIBuffer->Render(m_pSSAOShader, 2);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_SSAO_BlurY"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_uint		iNumViewports = 1;

	D3D11_VIEWPORT		Viewport;
	Viewport.Height = 720.f;
	Viewport.Width = 1280.f;
	Viewport.MinDepth = 0.f;
	Viewport.MaxDepth = 1.f;
	Viewport.TopLeftX = 0.f;
	Viewport.TopLeftY = 0.f;

	m_pDeviceContext->RSSetViewports(iNumViewports, &Viewport);

	return S_OK;
}

HRESULT CRenderer::Render_MotionBlur()
{
	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_MotionBlur"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (true == m_isUsingLUT)
	{
		CHECK_FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_LUT"))));
	}

	else
	{
		if (true == m_bHDR && false == m_bBloom)
		{
			CHECK_FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_ToneMapping"))));
		}
		else if (true == m_bBloom)
		{
			CHECK_FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_BloomMapping"))));
		}
		else if (false == m_bHDR && false == m_bBloom)
		{
			if (true == m_bFog)
			{
				CHECK_FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Fog"))));
			}
			else
			{
				CHECK_FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_NonBlendTexture"))));
			}
		}
	}
	CHECK_FAILED(m_pShader->Set_Texture("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth"))));
	CHECK_FAILED(m_pShader->Set_Texture("g_NormalTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Velocity"))));

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	m_pVIBuffer->Render(m_pShader, 19);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_MotionBlur"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_RadialBlur()
{
	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_RadialBlur"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Distortion"))));

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_fBlurDist", &m_fRadialBlurDist, sizeof(_float)));

	m_pVIBuffer->Render(m_pShader, 20);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_RadialBlur"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_Fog()
{
	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_Fog"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);
	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_NonBlendTexture"))));
	CHECK_FAILED(m_pShader->Set_Texture("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth"))));

	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_fFogStart", &m_fFogStart, sizeof(_float)));
	CHECK_FAILED(m_pShader->Set_RawValue("g_fFogEnd", &m_fFogEnd, sizeof(_float)));
	CHECK_FAILED(m_pShader->Set_RawValue("g_vFogColor", &m_vFogColor, sizeof(_float4)));
	CHECK_FAILED(m_pShader->Set_RawValue("g_fMaxFogFactor", &m_fMaxFogFactor, sizeof(_float)));

	_float4x4		ViewMatrixInv, ProjMatrixInv;
	_float4			vCamPosition;

	XMStoreFloat4x4(&ViewMatrixInv, XMMatrixTranspose(XMMatrixInverse(nullptr, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))));
	XMStoreFloat4x4(&ProjMatrixInv, XMMatrixTranspose(XMMatrixInverse(nullptr, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))));
	XMStoreFloat4(&vCamPosition, pPipeLine->Get_CamPosition());

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrixInv", &ViewMatrixInv, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrixInv", &ProjMatrixInv, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pShader->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	
	m_pVIBuffer->Render(m_pShader, 21);

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_Fog"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::DownSampling()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_DownSample"), m_pDownDSV)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	//SetUp_ConstantTable ================================================================
	_float4x4		WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	D3D11_VIEWPORT		Viewport;
	_uint				iNumViewports = 1;

	m_pDeviceContext->RSGetViewports(&iNumViewports, &Viewport);

	_float		fScreenX = Viewport.Width * 0.5f;
	_float		fScreenY = Viewport.Height * 0.5f;

	WorldMatrix._11 = fScreenX;
	WorldMatrix._22 = fScreenY;
	WorldMatrix._33 = 1.f;

	memcpy(&WorldMatrix.m[3][0], &_float4(-fScreenX * 0.5f, fScreenY * 0.5f, 0.f, 1.f), sizeof(_float4));
	//memcpy(&WorldMatrix.m[3][0], &_float4(0.f, 0.f, 0.f, 1.f), sizeof(_float4));

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	_float4x4		ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float4x4		ProjMatrix;
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f)));

	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_ForDownSample"))))
	//====================================================================================================================

	m_pVIBuffer->Render(m_pShader, 0);

	// 다시 백버퍼를 바인딩
	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_DownSample"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	ID3D11ShaderResourceView* pSRV = NULL;
	m_pDeviceContext->PSSetShaderResources(0, 1, &pSRV);

	return S_OK;
}

HRESULT CRenderer::UpSampling()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_UpSample"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	//SetUp_ConstantTable ================================================================
	_float4x4	WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &WorldMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;

		return E_FAIL;
	}

	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Verti"))))
	
	//====================================================================================================================

	m_pScaleWIn[UPSACLE]->Render(m_pShader, 0);

	// 다시 백버퍼를 바인딩
	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_UpSample"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	ID3D11ShaderResourceView* pSRV = NULL;
	m_pDeviceContext->PSSetShaderResources(0, 1, &pSRV);

	return S_OK;
}

HRESULT CRenderer::Blur()
{
	if (FAILED(Blur_X()))
	{
		BREAKPOINT;
		return E_FAIL;;
	}

	if (FAILED(Blur_Y()))
	{
		BREAKPOINT;
		return E_FAIL;;
	}

	return S_OK;
}

HRESULT CRenderer::Blur_X()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_BlurX"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	//SetUp_ConstantTable ================================================================
	_float4x4	WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;

		return E_FAIL;
	}

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &WorldMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;

		return E_FAIL;
	}

	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;

		return E_FAIL;
	}


	// 뷰포트를 얻어온다.
	_uint				iNumViewports = 1;
	D3D11_VIEWPORT		Viewport;

	m_pDeviceContext->RSGetViewports(&iNumViewports, &Viewport);

	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_DownSample"))))

		//====================================================================================================================

		m_pScaleWIn[DOWNSCALE]->Render(m_pShader, 4);



	// 다시 백버퍼를 바인딩
	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_BlurX"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	ID3D11ShaderResourceView* pSRV = NULL;
	m_pDeviceContext->PSSetShaderResources(0, 1, &pSRV);

	return S_OK;
}

HRESULT CRenderer::Blur_Y()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_BlurY"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float4x4	WorldMatrix;
	//SetUp_ConstantTable ================================================================
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;

		return E_FAIL;
	}

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &WorldMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;

		return E_FAIL;
	}

	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;

		return E_FAIL;
	}

	CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Hori"))))


	//====================================================================================================================

	m_pScaleWIn[DOWNSCALE]->Render(m_pShader, 5);

	// 다시 백버퍼를 바인딩
	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_BlurY"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	ID3D11ShaderResourceView* pSRV = NULL;
	m_pDeviceContext->PSSetShaderResources(0, 1, &pSRV);

	return S_OK;
}

HRESULT CRenderer::Distortion()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_Distortion"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float4x4	WorldMatrix;
	//SetUp_ConstantTable ================================================================
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;

		return E_FAIL;
	}

	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;

		return E_FAIL;
	}

	CHECK_FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Combine"))));
	CHECK_FAILED(m_pShader->Set_Texture("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_NonLightDepth"))));

	if (FAILED(m_pNoiseTexture->SetUp_OnShader(m_pShader, "g_BlurTexture", 36)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pShader->Set_RawValue("g_DistortionUV", &m_fDistortionUV, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	//====================================================================================================================

	m_pVIBuffer->Render(m_pShader, 7);

	m_fDistortionUV += 0.02f;

	// 다시 백버퍼를 바인딩
	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_Distortion"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::LUT_Filtering()
{
	RETURN_FAIL(if (nullptr == m_pVIBuffer || nullptr == m_pShader));

	if (FAILED(m_pTarget_Manager->Begin(m_pDeviceContext, TEXT("MRT_LUT"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	
	if (true == m_isUsingLUT)
	{
		if (true == m_bHDR && false == m_bBloom)
		{
			CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_ToneMapping"))));
		}
		else if (true == m_bBloom)
		{
			CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_BloomMapping"))));
		}
		else if (false == m_bHDR && false == m_bBloom)
		{
			if (true == m_bFog)
			{
				CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Fog"))));
			}
			else
			{
				CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_NonBlendTexture"))));
			}
		}
		if (FAILED(m_pLUTTexture->SetUp_OnShader(m_pShader, "g_LUTTexture", m_iLUTTexIndex)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		if (FAILED(m_pLUTTexture->SetUp_OnShader(m_pShader, "g_DiffuseTexture", m_iLUTTexIndexSecond)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		CHECK_FAILED(m_pShader->Set_Texture("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth"))));


		CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

		CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

		CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));


		m_pVIBuffer->Render(m_pShader, 18);
	}

	if (FAILED(m_pTarget_Manager->End(m_pDeviceContext, TEXT("MRT_LUT"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_Final()
{
	RETURN_FAIL(if (nullptr == m_pVIBuffer || nullptr == m_pShader));

	if (true == m_bRadialBlur)
	{
		CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_RadialBlur"))));
	}
	else
	{
		CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Distortion"))));
	}

	if (true == m_bRenderDiffuse)
		CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Diffuse"))));
	
	//CHECK_FAILED(m_pShader->Set_Texture("g_TargetTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Combine"))));
	
	CHECK_FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4)));

	CHECK_FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));


	m_pVIBuffer->Render(m_pShader, 8);


	return S_OK;
}

HRESULT CRenderer::Add_RenderTargets()
{
	// 뷰포트를 얻어온다.
	_uint		iNumViewports = 1;

	D3D11_VIEWPORT		Viewport = CGraphic_Device::GetInstance()->Get_Viewport(CGraphic_Device::DEFAULT);

	// 디퓨즈 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Diffuse")
		, (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM
		, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 노말 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Normal"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 스펙큘러 타겟 추가
	CHECK_FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Specular"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)));


	// 깊이 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Depth"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 셰이드 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Shade"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 다운스케일링 이전 처리 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_ForDownSample"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 다운샘플링 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_DownSample"), (_uint)(Viewport.Width * 0.5f), (_uint)(Viewport.Height * 0.5f), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 업스케일링 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_UpSample"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 수평 블러 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Blur_Hori"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 수직 블러 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Blur_Verti"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 논블랜드 텍스쳐 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_NonBlendTexture"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 알파 텍스쳐 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_AlphaTexture"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 논블랜드 + 알파 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Combine"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 왜곡 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Distortion"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 논라이트 뎊스 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_NonLightDepth"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 이미시브 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Emissive"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	//	HDR DownScale 타겟들 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_HDR_FirstDownScale"), (_uint)(Viewport.Width * 0.5f), (_uint)(Viewport.Height * 0.5f), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_HDR_FirstDownScaleBlur"), (_uint)(Viewport.Width * 0.5f), (_uint)(Viewport.Height * 0.5f), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_HDR_SecondDownScale"), (_uint)(256), (_uint)(256), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_HDR_SecondDownScaleBlur"), (_uint)(256), (_uint)(256), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_HDR_ThirdDownScale"), (_uint)(64), (_uint)(64), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_HDR_ThirdDownScaleBlur"), (_uint)(64), (_uint)(64), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_HDR_FourthDownScale"), (_uint)(16), (_uint)(16), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_HDR_FourthDownScaleBlur"), (_uint)(16), (_uint)(16), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_HDR_FifthDownScale"), (_uint)(4), (_uint)(4), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_HDR_FifthDownScaleBlur"), (_uint)(4), (_uint)(4), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_HDR_SixthDownScale"), (_uint)(1), (_uint)(1), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_CalcAverLum"), (_uint)(1), (_uint)(1), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 이전 평균휘도 랜더타겟
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_PrevAverLum"), (_uint)(1), (_uint)(1), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 톤매핑 랜더타겟
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_ToneMapping"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	// 블룸 마스크 랜더타겟
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_BloomMask"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 블룸 마스크 다운샘플링 랜더타겟
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_BloomDown"), (_uint)(Viewport.Width * 0.25f), (_uint)(Viewport.Height * 0.25f), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 블룸 마스크 BlurX 랜더타겟
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_BloomBlurX"), (_uint)(Viewport.Width * 0.25f), (_uint)(Viewport.Height * 0.25f), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 블룸 마스크 BlurY 랜더타겟
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_BloomBlurY"), (_uint)(Viewport.Width * 0.25f), (_uint)(Viewport.Height * 0.25f), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 블룸 마스크 업샘플링 랜더타겟
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_BloomUp"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 블룸 매핑 랜더 타겟
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_BloomMapping"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// DOF 블러X 랜더 타겟
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_DOF_BlurX"), (_uint)(Viewport.Width * 0.25f), (_uint)(Viewport.Height * 0.25f), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// DOF 블러Y 랜더 타겟
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_DOF_BlurY"), (_uint)(Viewport.Width * 0.25f), (_uint)(Viewport.Height * 0.25f), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// DOF 랜더 타겟
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_DOF"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	// SSAO 랜더 타겟
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_SSAO"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// SSAO BlurX랜더 타겟
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_SSAO_BlurX"), (_uint)(Viewport.Width * 0.5f), (_uint)(Viewport.Height * 0.5f), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// SSAO BlurY랜더 타겟
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_SSAO_BlurY"), (_uint)(Viewport.Width * 0.5f), (_uint)(Viewport.Height * 0.5f), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 림라이트 랜더 타겟
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_RimLight"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 그림자 랜더 타겟
	D3D11_VIEWPORT		ShadowViewport = CGraphic_Device::GetInstance()->Get_Viewport(CGraphic_Device::SHADOW);
	
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Shadow"), (_uint)(ShadowViewport.Width), (_uint)(ShadowViewport.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 0.f, 0.f, 1.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Shadow_Static"), (_uint)(ShadowViewport.Width), (_uint)(ShadowViewport.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 0.f, 0.f, 1.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Shadow_Final"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 업스케일링_Shadow 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_UpSample_Shadow"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// LUT 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_LUT"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Velocity 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Velocity"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// MotionBlur 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_MotionBlur"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// RadialBlur 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_RadialBlur"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Fog 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Fog"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.5f, 0.5f, 0.5f, 1.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Outline 타겟 추가
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Outline"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// SSAO 끄는용도 타겟
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_NotSSAO"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Add_MultiRenderTargets()
{
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Diffuse"))))	// 디퓨즈
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Normal"))))	// 노말
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Depth"))))	// 깊이
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_ForDownSample"))))	// 다운샘플링하기 위한 텍스쳐
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Emissive"))))	// 다운샘플링하기 위한 텍스쳐
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_RimLight"))))	// 림라이트
	{
		BREAKPOINT;
		return E_FAIL;
	} 

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Velocity"))))	// Velocity
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_DownSample"), TEXT("Target_DownSample"))))	// 다운스케일
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_UpSample"), TEXT("Target_UpSample"))))	// 다운스케일
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BlurX"), TEXT("Target_Blur_Hori"))))	// 다운스케일
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BlurY"), TEXT("Target_Blur_Verti"))))	// 다운스케일
	{
		BREAKPOINT;
		return E_FAIL;
	}


	// 빛을 그릴 때 바인드하는 멀티랜더타겟을 추가
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))	// 명암
	{
		BREAKPOINT;
		return E_FAIL;
	}		

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shadow_Final"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Outline"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 논블랜드 텍스쳐 멀티랜더타겟
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_NonBlendTexture"), TEXT("Target_NonBlendTexture"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 알파 텍스쳐 멀티랜더타겟
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_AlphaTexture"), TEXT("Target_AlphaTexture"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_AlphaTexture"), TEXT("Target_NonLightDepth"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_AlphaTexture"), TEXT("Target_ForDownSample"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 논블랜드 + 알파 멀티랜더타겟
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Combine"), TEXT("Target_Combine"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	// 왜곡 멀티랜더타겟
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// HDR 다운스케일 멀티랜더타겟
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_HDR_FirstDownScale"), TEXT("Target_HDR_FirstDownScale"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_HDR_FirstDownScaleBlur"), TEXT("Target_HDR_FirstDownScaleBlur"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_HDR_SecondDownScale"), TEXT("Target_HDR_SecondDownScale"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_HDR_SecondDownScaleBlur"), TEXT("Target_HDR_SecondDownScaleBlur"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_HDR_ThirdDownScale"), TEXT("Target_HDR_ThirdDownScale"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_HDR_ThirdDownScaleBlur"), TEXT("Target_HDR_ThirdDownScaleBlur"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_HDR_FourthDownScale"), TEXT("Target_HDR_FourthDownScale"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_HDR_FourthDownScaleBlur"), TEXT("Target_HDR_FourthDownScaleBlur"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_HDR_FifthDownScale"), TEXT("Target_HDR_FifthDownScale"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_HDR_FifthDownScaleBlur"), TEXT("Target_HDR_FifthDownScaleBlur"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_HDR_SixthDownScale"), TEXT("Target_HDR_SixthDownScale"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 평균 휘도값 계산 멀티랜더타겟
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_CalcAverLum"), TEXT("Target_CalcAverLum"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 톤매핑 멀티랜더 타겟
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_ToneMapping"), TEXT("Target_ToneMapping"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 이전 평균 휘도 멀티 랜더타겟
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_PrevAverLum"), TEXT("Target_PrevAverLum"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 블룸 멀티 랜더 타겟
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BloomMask"), TEXT("Target_BloomMask"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BloomDown"), TEXT("Target_BloomDown"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BloomBlurX"), TEXT("Target_BloomBlurX"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BloomBlurY"), TEXT("Target_BloomBlurY"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BloomUp"), TEXT("Target_BloomUp"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BloomMapping"), TEXT("Target_BloomMapping"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// DOF BlurX 멀티 랜더 타겟
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_DOF_BlurX"), TEXT("Target_DOF_BlurX"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// DOF BlurY 멀티 랜더 타겟
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_DOF_BlurY"), TEXT("Target_DOF_BlurY"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// DOF 멀티 랜더 타겟
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_DOF"), TEXT("Target_DOF"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// SSAO 멀티 랜더 타겟
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_SSAO"), TEXT("Target_SSAO"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// SSAO_BlurX 멀티 랜더 타겟
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_SSAO_BlurX"), TEXT("Target_SSAO_BlurX"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// SSAO_BlurY 멀티 랜더 타겟
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_SSAO_BlurY"), TEXT("Target_SSAO_BlurY"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Shadow 멀티 랜더 타겟
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_Shadow"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Shadow_Static"), TEXT("Target_Shadow_Static"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_UpSample_Shadow"), TEXT("Target_UpSample_Shadow"))))	// 다운스케일
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// LUT 멀티 랜더 타겟
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LUT"), TEXT("Target_LUT"))))	// 다운스케일
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// MotionBlur 멀티 랜더 타겟
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_MotionBlur"), TEXT("Target_MotionBlur"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// RadialBlur 멀티 랜더 타겟
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_RadialBlur"), TEXT("Target_RadialBlur"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Fog 멀티 랜더 타겟
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Fog"), TEXT("Target_Fog"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	
	return S_OK;
}

HRESULT CRenderer::SetUp_Components()
{
	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext);
	if (nullptr == m_pVIBuffer)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_pShader = CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../ShaderFiles/Shader_Deferred.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_pSSAOShader = CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../ShaderFiles/Shader_SSAO.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (nullptr == m_pSSAOShader)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_pNoiseTexture = CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Noise/noise (%d).png"), 99);
	if (nullptr == m_pNoiseTexture)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_pLUTTexture = CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/LUT/LUT0%d.png"), 97);
	if (nullptr == m_pLUTTexture)
	{
		BREAKPOINT;
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CRenderer::Make_HDRDepthStencil(_uint iWidth, _uint iHeight, _uint iIndex)
{
	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
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
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pHDRDownDSV[iIndex])))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

HRESULT CRenderer::Make_CubeDepthStencil()
{
	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC TextureDesc;
	TextureDesc.Width = 256;
	TextureDesc.Height = 256;
	TextureDesc.Format = DXGI_FORMAT_R32_FLOAT;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 6;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	CHECK_FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture));

	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc;
	DSVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	DSVDesc.Texture2DArray.FirstArraySlice = 0;
	DSVDesc.Texture2DArray.ArraySize = 6;
	DSVDesc.Texture2DArray.MipSlice = 0;

	CHECK_FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, &DSVDesc, &m_pCubeDSV));


	return S_OK;
}

//#ifdef _DEBUG
HRESULT CRenderer::Ready_DebugDesc()
{
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Diffuse")
		, 50.f, 50.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Normal")
		, 50.f, 150.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Depth"), 50.f, 250.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Shade"), 150.f, 50.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Specular"), 150.f, 150.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_ForDownSample"), 150.f, 250.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_DownSample"), 250.f, 50.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_UpSample"), 250.f, 150.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Blur_Hori"), 250.f, 250.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Blur_Verti"), 350.f, 50.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Combine"), 350.f, 150.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Distortion"), 350.f, 250.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_NonLightDepth"), 450.f, 50.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_NonBlendTexture"), 450.f, 150.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_AlphaTexture"), 450.f, 250.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_HDR_SixthDownScale"), 550.f, 50.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_ToneMapping"), 550.f, 150.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_BloomMask"), 550.f, 250.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_BloomMapping"), 650.f, 50.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_DOF"), 650.f, 150.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_SSAO_BlurY"), 650.f, 250.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Shadow"), 750.f, 50.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Shadow_Static"), 750.f, 150.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Shadow_Final"), 750.f, 250.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_UpSample_Shadow"), 850.f, 50.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_LUT"), 850.f, 150.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Velocity"), 850.f, 250.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Fog"), 950.f, 50.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->Ready_DebugDesc(TEXT("Target_Outline"), 950.f, 150.f, 100.f, 100.f)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}


HRESULT CRenderer::Render_Debug()
{
	for (auto& pComponent : m_DebugObject)
	{
		if (nullptr != pComponent)
			pComponent->Render();

		Safe_Release(pComponent);
	}
	m_DebugObject.clear();

	return S_OK;
}
//#endif

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CRenderer::Clone(void* pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

	Safe_Release(m_pNoiseTexture);
	Safe_Release(m_pLUTTexture);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pSSAOShader);
	Safe_Release(m_pShader);
	Safe_Release(m_pSSAOShader);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pDownDSV);
	Safe_Release(m_pSSAOShader);

	for (_uint i = 0; i < 7; i++)
	{
		Safe_Release(m_pHDRDownDSV[i]);
	}

	for (_uint i = 0; i < SCALE_END; ++i)
		Safe_Release(m_pScaleWIn[i]);

	for (auto& RenderObjects : m_RenderObjects)
	{
		for (auto& pGameObject : RenderObjects)
			Safe_Release(pGameObject);
		RenderObjects.clear();
	}

	for (auto& RenderObjects : m_ShadowObject)
	{
		for (auto& pGameObject : RenderObjects)
			Safe_Release(pGameObject);
		RenderObjects.clear();
	}
}
