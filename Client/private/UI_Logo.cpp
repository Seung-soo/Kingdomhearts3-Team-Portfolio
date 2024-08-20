#include "stdafx.h"
#include "..\public\UI_Logo.h"
#include "GameInstance.h"
#include "GameManager.h"

CUI_Logo::CUI_Logo(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_Logo::CUI_Logo(const CUI_Logo & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_Logo::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_Logo::NativeConstruct(void * pArg)
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

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, -150.f, 150.f)));
	// Setting =========

	_float qwe = 1.2f;

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::LOGO_MAIN:
		SizeX = g_iWinCX;
		SizeY = g_iWinCY;;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 640.f;
		SetY = 360.f;

		m_fDepth = 1.f; // ±íÀÌ
		m_fFrame = 0.f;
		break;
	case Engine::CUI_Parents::LOGO_BUTTON0:
		SizeX = 220.f;
		SizeY = 34.f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 400.f;
		SetY = 280.f;

		m_fDepth = 0.31f; // ±íÀÌ
		m_fFrame = 0.f;
		break;
	case Engine::CUI_Parents::LOGO_BUTTON1:
		SizeX = 90.f;
		SizeY = 34.f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 335.f;
		SetY = 230.f;

		m_fDepth = 0.31f; // ±íÀÌ
		m_fFrame = 0.f;
		break;
	case Engine::CUI_Parents::LOGO_TITLE:
		SizeX = 512.f * qwe;
		SizeY = 168.f * qwe;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 400.f;
		SetY = 530.f;

		m_fDepth = 0.31f; // ±íÀÌ
		m_fFrame = 0.f;
		break;
	case Engine::CUI_Parents::LOGO_HEART:
		SizeX = 232.f * qwe;
		SizeY = 256.f * qwe;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 400.f;
		SetY = 520.f;

		m_fDepth = 0.33f; // ±íÀÌ
		m_fFrame = 0.f;
		break;
	case Engine::CUI_Parents::LOGO_TRD:
		SizeX = 208.f * qwe;
		SizeY = 256.f * qwe;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 400.f;
		SetY = 520.f;

		m_fDepth = 0.32f; // ±íÀÌ
		m_fFrame = 0.f;
		break;
	case Engine::CUI_Parents::LOGO_CROWN:
		SizeX = 64.f * qwe;
		SizeY = 40.f * qwe;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 400.f;
		SetY = 670.f;

		m_fDepth = 0.30f; // ±íÀÌ
		m_fFrame = 0.f;
		break;
	case Engine::CUI_Parents::LOGO_SORA:
		SizeX = 512.f * 1.3f;
		SizeY = 512.f * 1.3f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 950.f;
		SetY = 350.f;

		m_fDepth = 0.31f; // ±íÀÌ
		m_fFrame = 0.f;
		break;

	case Engine::CUI_Parents::LOGO_POINT:
		SizeX = (512.f - 192.f) * 1.6f;
		SizeY = 32.f * 1.7f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 250.f;
		SetY = 350.f;

		m_fDepth = 0.5f; // ±íÀÌ
		m_fFrame = 0.f;
		break;
	default:
		break;
	}

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	return S_OK;
}

_int CUI_Logo::Tick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::LOGO_MAIN:
		break;
	case Engine::CUI_Parents::LOGO_BUTTON0:
		if (true == GM->Get_bLoadingStart())
		{
			fSpeed *= 1.2f;
			SetY -= fSpeed;
		}
		break;
	case Engine::CUI_Parents::LOGO_BUTTON1:
		if (true == GM->Get_bLoadingStart())
		{
			fSpeed *= 1.2f;
			SetY -= fSpeed;
		}
		break;
	case Engine::CUI_Parents::LOGO_TITLE:
		
		break;
	case Engine::CUI_Parents::LOGO_HEART:
		if (true == GM->Get_bLoadingStart())
		{
			fSpeed *= 1.01f;
			m_pTransformCom->NowRotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fSpeed));

			if (20.f < fSpeed)
			{
				fSpeed = 20.f;
			}


				
		}
			
		break;
	case Engine::CUI_Parents::LOGO_TRD:
		if (true == GM->Get_bLoadingStart())
		{
			fSpeed *= 1.2f;
			SetY += fSpeed;
		}
		break;
	case Engine::CUI_Parents::LOGO_CROWN:
		if (true == GM->Get_bLoadingStart())
		{
			fSpeed *= 1.2f;
			SetY += fSpeed;
		}
		break;
	case Engine::CUI_Parents::LOGO_SORA:
		break;
	case Engine::CUI_Parents::LOGO_POINT:

		if (pGameInstance->Get_KeyEnter(DIK_UP))
			iSelect_Point += 1;
		
		if (pGameInstance->Get_KeyEnter(DIK_DOWN))
			iSelect_Point -= 1;
		
		if (2 <= iSelect_Point)
			iSelect_Point = 0;

		if (-1 >= iSelect_Point)
			iSelect_Point = 1;

		//==========================================
		//==========================================

		if (0 == iSelect_Point)
		{
			SetX = 545.f;
			SetY = 280.f;
		}
		else if (1 == iSelect_Point)
		{
			SetX = 545.f;
			SetY = 230.f;
		}

		
		break;
	default:
		break;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));


	return RESULT_NOPROBLEM;
}

_int CUI_Logo::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return RESULT_NOPROBLEM;
}

HRESULT CUI_Logo::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::LOGO_MAIN:
	case Engine::CUI_Parents::LOGO_BUTTON0:
	case Engine::CUI_Parents::LOGO_BUTTON1:
	case Engine::CUI_Parents::LOGO_TITLE:
	case Engine::CUI_Parents::LOGO_HEART:
	case Engine::CUI_Parents::LOGO_TRD:
	case Engine::CUI_Parents::LOGO_CROWN:
	case Engine::CUI_Parents::LOGO_SORA:
		m_pVIBufferCom->Render(m_pShaderCom, 11);
		break;
	case Engine::CUI_Parents::LOGO_POINT:
		if (false == GM->Get_bLoadingStart())
			m_pVIBufferCom->Render(m_pShaderCom, 11);
		break;
	default:
		break;
	}
	

	return S_OK;
}

HRESULT CUI_Logo::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */ //====================
	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::LOGO_MAIN:
		CHECK_FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Copmonent_Texture_Logo"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::LOGO_BUTTON0:
		CHECK_FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Copmonent_Texture_Button0"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::LOGO_BUTTON1:
		CHECK_FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Copmonent_Texture_Button1"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::LOGO_TITLE:
		CHECK_FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Copmonent_Texture_Title"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::LOGO_HEART:
		CHECK_FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Copmonent_Texture_Title_Heart"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::LOGO_TRD:
		CHECK_FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Copmonent_Texture_Title_Trd"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::LOGO_CROWN:
		CHECK_FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Copmonent_Texture_Title_Crown"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::LOGO_SORA:
		CHECK_FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Copmonent_Texture_Title_Sora"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::LOGO_POINT:
		CHECK_FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Copmonent_Texture_Point"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CUI_Logo::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture"));

	return S_OK;
}

CUI_Logo * CUI_Logo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Logo*	pInstance = new CUI_Logo(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_Logo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Logo::Clone(void * pArg)
{
	CUI_Logo*	pInstance = new CUI_Logo(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_Logo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Logo::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
