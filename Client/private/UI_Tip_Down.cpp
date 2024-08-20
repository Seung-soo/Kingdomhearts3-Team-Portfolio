#include "stdafx.h"
#include "..\public\UI_Tip_Down.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"
#include "Easing_Function.h"

CUI_Tip_Down::CUI_Tip_Down(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_Tip_Down::CUI_Tip_Down(const CUI_Tip_Down & rhs)
	:CUI_Parents(rhs)
{
}

HRESULT CUI_Tip_Down::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_Tip_Down::NativeConstruct(void * pArg)
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

	SizeX = 400.f;
	SizeY = 120.f;

	/*====*/

	if (0 == iCom_Texture)
	{
		SetX = 1480.f;
		SetY = 600.f;
		m_fDepth = 0.2f;

		m_pGameInstance->StopSound(CSoundMgr::SYSTEM_EFFECT2);
		m_pGameInstance->LetsPlaySound(TEXT("Information.wav"), CSoundMgr::SYSTEM_EFFECT2, 0.7f);
	}
	else if (1 == iCom_Texture)
	{
		SetX = 1080.f;
		SetY = 600.f;

		m_fFrame = 0.f;
		m_fDepth = 0.19f;
	}
	else if (2 == iCom_Texture)
	{
		SetX = 1080.f;
		SetY = 600.f;

		m_fFrame = 1.f;
		m_fDepth = 0.19f;
	}
	else if (3 == iCom_Texture)
	{
		SetX = 1080.f;
		SetY = 600.f;

		m_fFrame = 2.f;
		m_fDepth = 0.19f;
	}
	else if (4 == iCom_Texture)
	{
		SetX = 1080.f;
		SetY = 600.f;

		m_fFrame = 3.f;
		m_fDepth = 0.19f;
	}
	else if (5 == iCom_Texture)
	{
		SetX = 1080.f;
		SetY = 600.f;

		m_fFrame = 4.f;
		m_fDepth = 0.19f;
	}
	else if (6 == iCom_Texture)
	{
		SetX = 1080.f;
		SetY = 600.f;

		m_fFrame = 5.f;
		m_fDepth = 0.19f;
	}
	else if (7 == iCom_Texture)
	{
		SetX = 1080.f;
		SetY = 600.f;

		m_fFrame = 6.f;
		m_fDepth = 0.19f;
	}

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	return S_OK;
}

_int CUI_Tip_Down::Tick(_double TimeDelta)
{

	if (true == m_bDead)
	{
		if (1 == iCom_Texture)
		{
			_int	iBackUI     = 0;
			_int	iTutorialUI = 3;

			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Tutorial"), TEXT("Prototype_GameObject_UI_Tip"), &iBackUI)))
			{
				BREAKPOINT;
				return E_FAIL;
			}

			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Tutorial"), TEXT("Prototype_GameObject_UI_Tip"), &iTutorialUI)))
			{
				BREAKPOINT;
				return E_FAIL;
			}
		}


		return RESULT_DEAD;
	}

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_fWaitingTime += (_float)TimeDelta;
	dTime += TimeDelta;

	if (1.0 < m_fWaitingTime)
	{
		m_bWaiting = true;
		m_fWaitingTime = 0;
	}
		
	if (false == m_bWaiting)
	{
		SetX = 1480.f - EaseOutQuart(400.f, m_fWaitingTime, 1.0);
	}
	else
	{
		m_fWaitingTime2 += (_float)TimeDelta;

		if(4.0 <= m_fWaitingTime2)
		{
			if (0.01 < dTime)
			{
				SetX += 30.f;
				dTime = 0;
			}

			if (1480.f <= SetX)
				m_bDead = true;
		}
	}



	// 튜토리얼 UI 띄우기
	Tutorial(TimeDelta);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));
	return _int();
}

_int CUI_Tip_Down::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CUI_Tip_Down::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT CUI_Tip_Down::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	if (0 == iCom_Texture)
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Tip_Down"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	if (0 != iCom_Texture)
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Tip_Down_Text"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	return S_OK;
}

HRESULT CUI_Tip_Down::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	if (0 == iCom_Texture)
		CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture"));
	if (0 != iCom_Texture)
		CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_fFrame));

	return S_OK;
}

void CUI_Tip_Down::Tutorial(_double TimeDelta)
{
	if (LEVEL_TWILIGHT == GM->Get_CurrentLevel() && 0 == iCom_Texture)
	{
		if (true == GM->Get_OnTrigger(0) && false == GM->Get_OnTrigger(1) && false == m_bTuto)
		{
			m_fTutoWait += (_float)TimeDelta;
			if (1.f < m_fTutoWait)
			{
				_int	iBackUI = 0;
				_int	iTutorialUI = 2;

				if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Tutorial"), TEXT("Prototype_GameObject_UI_Tip"), &iBackUI)))
				{
					BREAKPOINT;
					return;
				}

				if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Tutorial"), TEXT("Prototype_GameObject_UI_Tip"), &iTutorialUI)))
				{
					BREAKPOINT;
					return;
				}

				m_bTuto = true;
			}
		}

		if (true == GM->Get_OnTrigger(1) && false == GM->Get_OnTrigger(2) && false == m_bTuto)
		{
			m_fTutoWait += (_float)TimeDelta;
			if (1.f < m_fTutoWait)
			{
				_int	iBackUI = 0;
				_int	iTutorialUI = 4;

				if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Tutorial"), TEXT("Prototype_GameObject_UI_Tip"), &iBackUI)))
				{
					BREAKPOINT;
					return;
				}

				if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Tutorial"), TEXT("Prototype_GameObject_UI_Tip"), &iTutorialUI)))
				{
					BREAKPOINT;
					return;
				}
				m_bTuto = true;
			}
		}
	}
}

CUI_Tip_Down * CUI_Tip_Down::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Tip_Down*	pInstance = new CUI_Tip_Down(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_Tip_Down");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Tip_Down::Clone(void * pArg)
{
	CUI_Tip_Down*	pInstance = new CUI_Tip_Down(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_Tip_Down");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Tip_Down::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
