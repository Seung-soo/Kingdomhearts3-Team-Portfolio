#include "stdafx.h"
#include "..\public\UI_TBoxOpen.h"
#include "GameInstance.h"
#include "GameManager.h"

#include "Player.h"

CUI_TBoxOpen::CUI_TBoxOpen(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_TBoxOpen::CUI_TBoxOpen(const CUI_TBoxOpen & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_TBoxOpen::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_TBoxOpen::NativeConstruct(void * pArg)
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

	_int aaa = 1;
	switch (iCom_Texture)
	{
	case 0:
		if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_TBoxOpen"), &aaa)))
			return E_FAIL;
		m_pLightUI = pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_UI"));
		
		m_fFrame = 0.f;
		break;
	case 1:

		break;
	case 2:
		if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_TBoxOpen"), &aaa)))
			return E_FAIL;
		m_pLightUI = pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_UI"));

		m_fFrame = 1.f;
		break;
	}

	SettingX = m_fX - (g_iWinCX*0.5f);
	SettingY = m_fY - (g_iWinCY*0.5f);

	SizeX = 464.f * 0.65f;
	SizeY = 48.f * 0.65f;
	SetX = -252.f * 0.65f;
	SetY = 140.f;

	if (0 == iCom_Texture)
		m_fDepth = 0.19f;
	
	if (1 == iCom_Texture)
		m_fDepth = 0.18f;
	
	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	m_pPlayer = GM->Get_Player();

	if (nullptr == m_pPlayer)
		_breakpoint;

	return S_OK;
}

_int CUI_TBoxOpen::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	switch (iCom_Texture)
	{
	case 0:
	case 2:
		dTime += TimeDelta;
		if (false == bCase0)
		{
			if (0.01 < dTime && 454.f > fSpeedX)
			{
				fSpeedX *= 1.25f;
				dTime = 0.0;
			}

			if (454.f <= fSpeedX)
				fSpeedX = 454.f;

			SetX = -252.f + fSpeedX;
		}
		else
		{
			if (nullptr != m_pLightUI)
			{
				m_pLightUI->Set_Dead(true);
				m_pLightUI = nullptr;

				m_pPlayer->Play_FormChangeUI();
			}
			if (0.01<dTime && 600.f>fSpeedX)
			{
				fSpeedX *= 1.4f;
				dTime = 0.0;
			}

			if (600.f <= fSpeedX)
				m_bDead = true;

			SetX = 202.f - fSpeedX;
		}

		if (m_pGameInstance->Get_KeyEnter(DIK_F))
		{
			bCase0 = true;
			fSpeedX = 10.f;
		}
			
		break;
	case 1:
		dTime += TimeDelta;
		if (0.01 < dTime && 5.f > fSize)
		{
			fSize += 0.1f;
			fFade += 0.02f;

			if (5.f <= fSize)
			{
				fSize = 1.f;
				fFade = 1.f;
			}
				
			dTime = 0.0;
		}

		SizeX = 464.f * fSize * 0.5f;
		SizeY = 48.f * fSize;
		SetX = 226.f;
		break;
	default:
		break;
	}


	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX*0.65f, SettingY + SetY, 0.f, 1.f));



	return _int();
}

_int CUI_TBoxOpen::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CUI_TBoxOpen::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());
	
	switch (iCom_Texture)
	{
	case 0:
	case 2:
		m_pVIBufferCom->Render(m_pShaderCom, 0);
		break;
	case 1:
		CHECK_FAILED(m_pShaderCom->Set_RawValue("TBoxOpen", &fFade, sizeof(_float)))
		m_pVIBufferCom->Render(m_pShaderCom, 14);
		break;
	default:
		break;
	}
	
	return S_OK;
}

HRESULT CUI_TBoxOpen::SetUp_Components()
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
	case 2:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Open_Button"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case 1:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Open_Button_Light"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	}

	return S_OK;
}

HRESULT CUI_TBoxOpen::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	switch (iCom_Texture)
	{
	case 0:
	case 2:
		CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_fFrame));
		break;
	case 1:
		CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture"));
		break;
	}

	return S_OK;
}

CUI_TBoxOpen * CUI_TBoxOpen::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_TBoxOpen*	pInstance = new CUI_TBoxOpen(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_TBoxOpen");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_TBoxOpen::Clone(void * pArg)
{
	CUI_TBoxOpen*	pInstance = new CUI_TBoxOpen(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_TBoxOpen");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_TBoxOpen::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
