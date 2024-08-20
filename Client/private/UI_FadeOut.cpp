#include "stdafx.h"
#include "..\public\UI_FadeOut.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "Camera_Perspective.h"
#include "Progress_Manager.h"

UI_FadeOut::UI_FadeOut(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

UI_FadeOut::UI_FadeOut(const UI_FadeOut & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT UI_FadeOut::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT UI_FadeOut::NativeConstruct(void * pArg)
{
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

	SizeX = 1280.f;
	SizeY = 720.f;
	SettingX = m_fX - (g_iWinCX*0.5f);
	SettingY = m_fY - (g_iWinCY*0.5f);
	SetX = 640.f;
	SetY = 360.f;

	m_fDepth = -5.f; // 깊이
	/*====*/

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	return S_OK;
}

_int UI_FadeOut::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	dFadeTime += TimeDelta;

	if (0 == iCom_Texture
		|| 2 == iCom_Texture)
	{
		if (0.01 < dFadeTime)
		{
			if (2 == iCom_Texture)
				fFadeOut -= 0.01f;
			else
				fFadeOut -= 0.01f;
			dFadeTime = 0.0;
		}

		if (0.f > fFadeOut)
		{
			fFadeOut = 0.f;
			m_bFadeOutMax = true;
		}
	}

	/* ========================= */

	if (1 == iCom_Texture
		|| 3 == iCom_Texture)
	{
		if (0.01 < dFadeTime)
		{
			fFadeIn += 0.01f;
			dFadeTime = 0.0;
		}

		if (1.f < fFadeIn)
		{
			m_bDead = true;
			if (LEVEL_TWILIGHT == GM->Get_CurrentLevel())
			{
				if (true == GM->Get_OnTrigger(2))
				{
					// UI 생성
					_int iComTex = 0;
					if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_UI_Tip_Down"), &iComTex)))
					{
						BREAKPOINT;
						return RESULT_ERROR;
					}

					iComTex = 5;
					if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_UI_Tip_Down"), &iComTex)))
					{
						BREAKPOINT;
						return RESULT_ERROR;
					}
					//m_pGameInstance->PlayBGM(TEXT("TwilightTownBGM.mp3"), 0.f);
				}
			}

			fFadeIn = 1.f;
		}

	}

	/* ========================= */

	if (4 == iCom_Texture)
	{
		
	}

	return RESULT_NOPROBLEM;
}

_int UI_FadeOut::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return RESULT_NOPROBLEM;
}

HRESULT UI_FadeOut::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	m_pVIBufferCom->Render(m_pShaderCom, 10);

	return S_OK;
}

HRESULT UI_FadeOut::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	if (0 == iCom_Texture
		|| 1 == iCom_Texture)
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Black_FadeOut"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
	
	if (2 == iCom_Texture
		|| 3 == iCom_Texture
		|| 4 == iCom_Texture)
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_White_FadeOut"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	
	return S_OK;
}

HRESULT UI_FadeOut::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture"));
	if (0 == iCom_Texture
		|| 2 == iCom_Texture)
		CHECK_FAILED(m_pShaderCom->Set_RawValue("FadeOut", &fFadeOut, sizeof(_float)));

	if (1 == iCom_Texture
		|| 3 == iCom_Texture
		|| 4 == iCom_Texture)
		CHECK_FAILED(m_pShaderCom->Set_RawValue("FadeOut", &fFadeIn, sizeof(_float)));

	return S_OK;
}

UI_FadeOut * UI_FadeOut::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	UI_FadeOut*	pInstance = new UI_FadeOut(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created UI_FadeOut");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * UI_FadeOut::Clone(void * pArg)
{
	UI_FadeOut*	pInstance = new UI_FadeOut(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created UI_FadeOut");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void UI_FadeOut::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
