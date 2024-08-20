#include "stdafx.h"
#include "..\public\UI_Minigame.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "UI_Num_Manager.h"
#include "Progress_Manager.h"

CUI_Minigame::CUI_Minigame(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_Minigame::CUI_Minigame(const CUI_Minigame & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_Minigame::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_Minigame::NativeConstruct(void * pArg)
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
	switch (iCom_Texture)
	{
	case 0: /* Target */
		m_fFrame = 0.f;

		SizeX = 1280.f;
		SizeY = 720.f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 640.f;
		SetY = 360.f;

		m_fDepth = 0.02f; // 깊이
		break;
	case 1: /* Frame */
		m_fFrame = 2.f;

		SizeX = 1280.f;
		SizeY = 720.f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 640.f;
		SetY = 360.f;

		m_fDepth = 0.02f; // 깊이
		break;
	default:
		break;
	}

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));
	return S_OK;
}

_int CUI_Minigame::Tick(_double TimeDelta)
{
	
	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	if (false == PM->Get_Minigame())
	{
		m_pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_Minigame"));
		return RESULT_DEAD;
	}
			
	if (true == PM->Get_CrowTarget())
		bAni = true;

	switch (iCom_Texture)
	{
	case 0:  /* Target */

		m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), TimeDelta);

		/* Frame */
		if (true == PM->Get_CrowTarget())
		{
			m_fFrame = 1.f;
		}
		else if (false == PM->Get_CrowTarget())
		{
			m_fFrame = 0.f;
		}
		/* ============================== */
		if (true == bAni)
		{
			dTime += TimeDelta;

			if (0.2 > dTime)
			{
				dSpeedTime += TimeDelta;
				if (0.001 < dSpeedTime)
				{
					SizeX += 0.5f;
					SizeY += 0.5f;

					m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
					dSpeedTime = 0;
				}

				dTime = 0;
				bAni = false;
			}
			else
			{
				dTime = 0;
				bAni = false;
			}
		}

		if (false == bAni)
		{
			SizeX = 1280.f;
			SizeY = 720.f;
		}

		m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));
		break;
	case 1:
		Score = GM->Get_Minigame_Score();
		if (0 >= Score)
			Score = 0;

		NM->Setting_Minigame(Score, 32.f);
		/* ============================== */
		if (true == bAni)
		{
			dTime += TimeDelta;

			if (0.2 > dTime)
			{
				dSpeedTime += TimeDelta;
				if (0.001 < dSpeedTime)
				{
					SizeX += 1.f;
					SizeY += 1.f;

					if (true == bShake)
					{
						SetX += 20.f;
						bShake = false;
					}
					else if (false == bShake)
					{
						SetX -= 20.f;
						bShake = true;
					}
						
					m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
					m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

					dSpeedTime = 0;
				}
			}
			else
			{
				dTime = 0;
				bAni = false;
			}
		}

		if (false == bAni)
		{
			SizeX = 1280.f;
			SizeY = 720.f;
			SetX = 640.f;
			PM->Set_CrowTarget(false);
		}


		m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));
		break;
	default:
		break;
	}

	if (false == m_bEnd && 5000 <= GM->Get_Minigame_Score())
	{
		PM->Set_ReadyWhiteFadeOut(true);
		PM->Set_MinigameWhiteFadeOut(true);
		m_bEnd = true;
	}

	return RESULT_NOPROBLEM;
}

_int CUI_Minigame::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return RESULT_NOPROBLEM;
}

HRESULT CUI_Minigame::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT CUI_Minigame::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MiniGame"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	return S_OK;
}

HRESULT CUI_Minigame::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_fFrame));

	return S_OK;
}

CUI_Minigame * CUI_Minigame::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Minigame*	pInstance = new CUI_Minigame(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_Minigame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Minigame::Clone(void * pArg)
{
	CUI_Minigame*	pInstance = new CUI_Minigame(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_Minigame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Minigame::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
