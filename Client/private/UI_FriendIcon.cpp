#include "stdafx.h"
#include "..\public\UI_FriendIcon.h"
#include "GameInstance.h"
#include "GameManager.h"

CUI_FriendIcon::CUI_FriendIcon(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_FriendIcon::CUI_FriendIcon(const CUI_FriendIcon & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_FriendIcon::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_FriendIcon::NativeConstruct(void * pArg)
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

	_float	tp = 0.25f;

	SettingX = m_fX - (g_iWinCX*0.5f);
	SettingY = m_fY - (g_iWinCY*0.5f);

	//m_fDepth = 0.19f; // 깊이
	m_fDepth = 0.1f;

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::DONALD_01:
		SizeX = 128.f*tp;
		SizeY = 128.f*tp;

		SetX = 302.f;
		SetY = 109.f;
		break;
	case Engine::CUI_Parents::DONALD_02:
		SizeX = 128.f*tp;
		SizeY = 128.f*tp;

		SetX = 289.f;
		SetY = 85.f;
		break;
	case Engine::CUI_Parents::GOOFY_01:
		SizeX = 128.f*tp;
		SizeY = 144.f*tp;

		SetX = 289.f;
		SetY = 61.f;
		break;
	case Engine::CUI_Parents::GOOFY_02:
		SizeX = 128.f*tp;
		SizeY = 144.f*tp;

		SetX = 289.f;
		SetY = 37.f;
		break;
	default:
		break;
	}

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	return S_OK;
}

_int CUI_FriendIcon::Tick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	if (false == GM->Get_UIRender())
		return 0;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::DONALD_01:
		if (0 == GM->Get_iFriendIcon_Num())
		{
			SetX = 302.f;
		}
		else
		{
			SetX = 289.f;
		}
		break;
	case Engine::CUI_Parents::DONALD_02:
		if (1 == GM->Get_iFriendIcon_Num())
		{
			SetX = 302.f;
		}
		else
		{
			SetX = 289.f;
		}
		break;
	case Engine::CUI_Parents::GOOFY_01:
		if (2 == GM->Get_iFriendIcon_Num())
		{
			SetX = 302.f;
		}
		else
		{
			SetX = 289.f;
		}
		break;
	case Engine::CUI_Parents::GOOFY_02:
		if (3 == GM->Get_iFriendIcon_Num())
		{
			SetX = 302.f;
		}
		else
		{
			SetX = 289.f;
		}
		break;
	default:
		break;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));


	return RESULT_NOPROBLEM;
}

_int CUI_FriendIcon::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::DONALD_01:
		break;
	case Engine::CUI_Parents::DONALD_02:
		break;
	case Engine::CUI_Parents::GOOFY_01:
		break;
	case Engine::CUI_Parents::GOOFY_02:
		break;
	default:
		break;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return RESULT_NOPROBLEM;
}

HRESULT CUI_FriendIcon::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (false == GM->Get_UIRender())
		return S_OK;

	CHECK_FAILED(SetUp_ConstantTable());

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::DONALD_01:
	case Engine::CUI_Parents::DONALD_02:
		if (true == GM->Get_bFriendIcon() && true == GM->Get_Slot_On_Donald())
			m_pVIBufferCom->Render(m_pShaderCom, 0);
		break;
	case Engine::CUI_Parents::GOOFY_01:
	case Engine::CUI_Parents::GOOFY_02:
		if (true == GM->Get_bFriendIcon() && true == GM->Get_Slot_On_Goofy())
			m_pVIBufferCom->Render(m_pShaderCom, 0);
		break;
	default:
		break;
	}



	return S_OK;
}

HRESULT CUI_FriendIcon::SetUp_Components()
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
	case Engine::CUI_Parents::DONALD_01:
	case Engine::CUI_Parents::DONALD_02:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Goofy_Icon"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::GOOFY_01:
	case Engine::CUI_Parents::GOOFY_02:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Donald_Icon"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	default:
		break;
	}
	
	
	return S_OK;
}

HRESULT CUI_FriendIcon::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture"));

	return S_OK;
}

CUI_FriendIcon * CUI_FriendIcon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_FriendIcon*	pInstance = new CUI_FriendIcon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_FriendIcon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_FriendIcon::Clone(void * pArg)
{
	CUI_FriendIcon*	pInstance = new CUI_FriendIcon(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_FriendIcon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_FriendIcon::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
