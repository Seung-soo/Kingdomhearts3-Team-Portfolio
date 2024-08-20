#include "stdafx.h"
#include "..\public\UI_Normal.h"
#include "GameInstance.h"
#include "MiniGame_Manager.h"

CUI_Normal::CUI_Normal(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice,pDeviceContext)
{
}

CUI_Normal::CUI_Normal(const CUI_Normal & rhs)
	:CUI_Parents(rhs)
{
}

HRESULT CUI_Normal::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_Normal::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));

	iCom_Texture = (*(_int*)pArg); // ÅØ½ºÃÄ Á¤º¸
	CHECK_FAILED(SetUp_Components());

	// UI ±âº» Setting ===

	m_fSizeX = g_iWinCX;
	m_fSizeY = g_iWinCY;
	m_fX = 0.f;
	m_fY = 0.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, 0.f, 1.f)));

	// Setting =========

	if (UI_NAME::UI_LOADING == iCom_Texture)
	{
		SizeX = 64.f;
		SizeY = 64.f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 1200.f;
		SetY = 50.f;

		m_fDepth = 0.f; // ±íÀÌ
	}

	if (UI_NAME::UI_BLACK == iCom_Texture)
	{
		SizeX = g_iWinCX;
		SizeY = g_iWinCY;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 640.f;
		SetY = 360.f;

		m_fDepth = 1.f; // ±íÀÌ
	}

	if (UI_NAME::UI_NOW_LOADING == iCom_Texture)
	{
		SizeX = 256.f;
		SizeY = 64.f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 1030.f;
		SetY = 50.f;

		m_fDepth = 0.1f; // ±íÀÌ
	}

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));


	//===============

	return S_OK;
}

_int CUI_Normal::Tick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	if (UI_NAME::UI_LOADING == iCom_Texture)
	{
		m_fFrame += (_float)TimeDelta * 15.f;
		if (15.f < m_fFrame)
			m_fFrame = 0.f;	
	}

	if (UI_NAME::UI_BLACK == iCom_Texture
		|| UI_NAME::UI_NOW_LOADING == iCom_Texture)
	{

	}

	return RESULT_NOPROBLEM;
}

_int CUI_Normal::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return RESULT_NOPROBLEM;
}

HRESULT CUI_Normal::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT CUI_Normal::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */ //====================

	if (UI_NAME::UI_LOADING == iCom_Texture)
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Loading"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	if (UI_NAME::UI_BLACK == iCom_Texture && false == MINIGAME->Get_ENDLoading())
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BlackBoard"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
	
	if (UI_NAME::UI_BLACK == iCom_Texture && true == MINIGAME->Get_ENDLoading())
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Logo"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));


	if (UI_NAME::UI_NOW_LOADING == iCom_Texture)
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Now_Loading"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	//======================================

	return S_OK;
}

HRESULT CUI_Normal::SetUp_ConstantTable()
{

	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	// TextureCom Set ======
	if (UI_NAME::UI_LOADING == iCom_Texture)
		CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_fFrame));

	if (UI_NAME::UI_BLACK == iCom_Texture
		|| UI_NAME::UI_NOW_LOADING == iCom_Texture)
		CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture"));
	//====================

	return S_OK;
}

CUI_Normal * CUI_Normal::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Normal*	pInstance = new CUI_Normal(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_Normal");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Normal::Clone(void * pArg)
{
	CUI_Normal*	pInstance = new CUI_Normal(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_Normal");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Normal::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
