#include "stdafx.h"
#include "..\public\UI_StageLogo.h"
#include "GameInstance.h"
#include "GameManager.h"

CUI_StageLogo::CUI_StageLogo(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_StageLogo::CUI_StageLogo(const CUI_StageLogo & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_StageLogo::NativeConstruct_Prototype()
{

	return S_OK;
}

HRESULT CUI_StageLogo::NativeConstruct(void * pArg)
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));

	iCom_Texture = (*(_int*)pArg); // 텍스쳐 정보
	CHECK_FAILED(SetUp_Components());

	// UI 기본 Setting ===

	m_fSizeX = g_iWinCX;
	m_fSizeY = g_iWinCY;
	m_fX = 0.f;
	m_fY = 0.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, 0.f, 1.f)));

	// Setting =========

	SettingX = m_fX - (g_iWinCX*0.5f);
	SettingY = m_fY - (g_iWinCY*0.5f);
	SetX = 640.f;
	SetY = 360.f;
	
	SizeX = 512.f;
	SizeY = 512.f;


	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	return S_OK;
}

_int CUI_StageLogo::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (false == bFrame)
	{
		m_fFrame += 8.f * (_float)TimeDelta;

		if (11.f < m_fFrame)
		{
			m_fFrame = 11.f;
			bFrame = true;
		}
	}
	//	UI가 다 드러나면 일정 대기시간 이후에 사라지게
	else if (0.f < m_fWaitingTime)
	{
		m_fWaitingTime -= (_float)TimeDelta;
		if (0.f >= m_fWaitingTime)
		{
			SizeSpeed = 1.f;
			bLogoPatten = true;
		}
	}

	dTime += TimeDelta;
	if (false == bLogoPatten && 0.01 < dTime)
	{
		SizeSpeed *= 1.05f;
		dTime = 0.0;

		if (256.f < SizeSpeed)
		{
			SizeSpeed = 256.f;
		}
	}
	
	if (true == bLogoPatten && 0.01 < dTime)
	{
		SizeSpeed *= 1.1f;
		fFade += 0.05f;
		dTime = 0.0;

		if (4096.f < SizeSpeed)
		{
			SizeSpeed = 4096.f;
		}
	}



	if (false == bLogoPatten)
	{
		SizeX = 1024.f - SizeSpeed;
		SizeY = 1024.f - SizeSpeed;
	}
	else
	{
		SizeX = 768.f + SizeSpeed;
		SizeY = 768.f + SizeSpeed;

		if (4096.f <= SizeSpeed)
			m_bDead = true;
	}

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	return _int();
}

_int CUI_StageLogo::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CUI_StageLogo::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	CHECK_FAILED(m_pShaderCom->Set_RawValue("StageLogo", &fFade, sizeof(_float)))

		m_pVIBufferCom->Render(m_pShaderCom, 13); 

	return S_OK;
}

HRESULT CUI_StageLogo::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	switch (iCom_Texture)
	{
	case 0:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_tt_Logo"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;

	case 1:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_ra_Logo"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;

	}
		
	

	return S_OK;
}

HRESULT CUI_StageLogo::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_fFrame));

	return S_OK;
}

CUI_StageLogo * CUI_StageLogo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_StageLogo*	pInstance = new CUI_StageLogo(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_StageLogo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_StageLogo::Clone(void * pArg)
{
	CUI_StageLogo*	pInstance = new CUI_StageLogo(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_StageLogo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_StageLogo::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
