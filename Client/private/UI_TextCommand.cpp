#include "stdafx.h"
#include "..\public\UI_TextCommand.h"
#include "GameInstance.h"
#include "GameManager.h"


CUI_TextCommand::CUI_TextCommand(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_TextCommand::CUI_TextCommand(const CUI_TextCommand & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_TextCommand::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_TextCommand::NativeConstruct(void * pArg)
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

	SettingX = m_fX - (g_iWinCX*0.5f);
	SettingY = m_fY - (g_iWinCY*0.5f);

	m_fDepth = 0.19f;

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::UI_TEXT_ATTACK:
		SizeX = 288.f;
		SizeY = 48.f;

		SetX = 121.f;
		SetY = 102.f;
		m_fFrame = 0.f;
		break;
	case Engine::CUI_Parents::UI_TEXT_SKILL:
		SizeX = 288.f;
		SizeY = 48.f;

		SetX = 121.f;
		SetY = 80.f;
		m_fFrame = 1.f;
		break;
	case Engine::CUI_Parents::UI_TEXT_ITEM:
		SizeX = 288.f;
		SizeY = 48.f;

		SetX = 121.f;
		SetY = 58.f;
		m_fFrame = 2.f;
		break;
	case Engine::CUI_Parents::UI_TEXT_DONALD1:
		SizeX = 288.f;
		SizeY = 48.f;

		SetX = 384.f;
		SetY = 104.f;
		m_fFrame = 5.f;
		break;
	case Engine::CUI_Parents::UI_TEXT_DONALD2:
		SizeX = 288.f;
		SizeY = 48.f;

		SetX = 384.f;
		SetY = 82.f;
		m_fFrame = 6.f;
		break;
	case Engine::CUI_Parents::UI_TEXT_GOOFY1:
		SizeX = 288.f;
		SizeY = 48.f;

		SetX = 384.f;
		SetY = 60.f;
		m_fFrame = 3.f;
		break;
	case Engine::CUI_Parents::UI_TEXT_GOOFY2:
		SizeX = 288.f;
		SizeY = 48.f;

		SetX = 384.f;
		SetY = 38.f;
		m_fFrame = 4.f;
		break;
	default:
		break;
	}

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	return S_OK;
}

_int CUI_TextCommand::Tick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));
	
	if (false == GM->Get_UIRender())
		return 0;
	
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* 
	First X: 121 <-> 143  //  Y: 102 80 58 36
	Second X: 384 <-> 399  //  Y: 104 82 60 38
	*/

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::UI_TEXT_ATTACK:
		if (0 == GM->Get_First_Frame_Num())
		{
			SetX = 143.f;
		}
		else
		{
			SetX = 121.f;
		}
		break;
	case Engine::CUI_Parents::UI_TEXT_SKILL:
		if (1 == GM->Get_First_Frame_Num())
		{
			SetX = 143.f;
		}
		else
		{
			SetX = 121.f;
		}
		break;
	case Engine::CUI_Parents::UI_TEXT_ITEM:
		if (2 == GM->Get_First_Frame_Num())
		{
			SetX = 143.f;
		}
		else
		{
			SetX = 121.f;
		}
		break;
	case Engine::CUI_Parents::UI_TEXT_DONALD1:
		if (0 == GM->Get_Second_Frame_Num())
		{
			SetX = 399.f;
		}
		else
		{
			SetX = 384.f;
		}
		break;
	case Engine::CUI_Parents::UI_TEXT_DONALD2:
		if (1 == GM->Get_Second_Frame_Num())
		{
			SetX = 399.f;
		}
		else
		{
			SetX = 384.f;
		}
		break;
	case Engine::CUI_Parents::UI_TEXT_GOOFY1:
		if (2 == GM->Get_Second_Frame_Num())
		{
			SetX = 399.f;
		}
		else
		{
			SetX = 384.f;
		}
		break;
	case Engine::CUI_Parents::UI_TEXT_GOOFY2:
		if (3 == GM->Get_Second_Frame_Num())
		{
			SetX = 399.f;
		}
		else
		{
			SetX = 384.f;
		}
		break;
	default:
		break;
	}

	/*if (pGameInstance->Get_KeyPressing(DIK_I))
		SetY += 1.f;

	if (pGameInstance->Get_KeyPressing(DIK_K))
		SetY -= 1.f;

	if (pGameInstance->Get_KeyPressing(DIK_J))
		SetX -= 1.f;

	if (pGameInstance->Get_KeyPressing(DIK_L))
		SetX += 1.f;*/

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	return RESULT_NOPROBLEM;
}

_int CUI_TextCommand::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::UI_TEXT_ATTACK:
	case Engine::CUI_Parents::UI_TEXT_SKILL:
	case Engine::CUI_Parents::UI_TEXT_ITEM:
		if (false == GM->Get_bCameraMove())
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
		break;
	case Engine::CUI_Parents::UI_TEXT_DONALD1:
	case Engine::CUI_Parents::UI_TEXT_DONALD2:
		if (false == GM->Get_bCameraMove() && true == GM->Get_Slot_On_Donald())
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
		break;
	case Engine::CUI_Parents::UI_TEXT_GOOFY1:
	case Engine::CUI_Parents::UI_TEXT_GOOFY2:
		if (false == GM->Get_bCameraMove() && true == GM->Get_Slot_On_Goofy())
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
		break;
	default:
		break;
	}

	return RESULT_NOPROBLEM;
}

HRESULT CUI_TextCommand::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (false == GM->Get_UIRender())
		return S_OK;

	CHECK_FAILED(SetUp_ConstantTable());

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::UI_TEXT_ATTACK:
		if (0 == GM->Get_First_Frame_Num())
		{
			m_pVIBufferCom->Render(m_pShaderCom, 0);
		}
		else
		{
			m_pVIBufferCom->Render(m_pShaderCom, 9);
		}
		break;
	case Engine::CUI_Parents::UI_TEXT_SKILL:
		if (1 == GM->Get_First_Frame_Num())
		{
			m_pVIBufferCom->Render(m_pShaderCom, 0);
		}
		else
		{
			m_pVIBufferCom->Render(m_pShaderCom, 9);
		}
		break;
	case Engine::CUI_Parents::UI_TEXT_ITEM:
		if (2 == GM->Get_First_Frame_Num())
		{
			m_pVIBufferCom->Render(m_pShaderCom, 0);
		}
		else
		{
			m_pVIBufferCom->Render(m_pShaderCom, 9);
		}
		
		break;
	case Engine::CUI_Parents::UI_TEXT_DONALD1:
		if (true == GM->Get_bFriendIcon())
		{
			if (0 == GM->Get_Second_Frame_Num())
			{
				m_pVIBufferCom->Render(m_pShaderCom, 0);
			}
			else
			{
				m_pVIBufferCom->Render(m_pShaderCom, 9);
			}
			break;
		}
		break;
	case Engine::CUI_Parents::UI_TEXT_DONALD2:
		if (true == GM->Get_bFriendIcon())
		{
			if (1 == GM->Get_Second_Frame_Num())
			{
				m_pVIBufferCom->Render(m_pShaderCom, 0);
			}
			else
			{
				m_pVIBufferCom->Render(m_pShaderCom, 9);
			}
			break;
		}
		break;
	case Engine::CUI_Parents::UI_TEXT_GOOFY1:
		if (true == GM->Get_bFriendIcon())
		{
			if (2 == GM->Get_Second_Frame_Num())
			{
				m_pVIBufferCom->Render(m_pShaderCom, 0);
			}
			else
			{
				m_pVIBufferCom->Render(m_pShaderCom, 9);
			}
			break;
		}
		break;
	case Engine::CUI_Parents::UI_TEXT_GOOFY2:
		if (true == GM->Get_bFriendIcon())
		{
			if (3 == GM->Get_Second_Frame_Num())
			{
				m_pVIBufferCom->Render(m_pShaderCom, 0);
			}
			else
			{
				m_pVIBufferCom->Render(m_pShaderCom, 9);
			}
			break;
		}
		break;
	default:
		break;
	}
		

	return S_OK;
}

HRESULT CUI_TextCommand::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_CommandText"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	return S_OK;
}

HRESULT CUI_TextCommand::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_fFrame));

	return S_OK;
}

CUI_TextCommand * CUI_TextCommand::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_TextCommand*	pInstance = new CUI_TextCommand(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_TextCommand");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_TextCommand::Clone(void * pArg)
{
	CUI_TextCommand*	pInstance = new CUI_TextCommand(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_TextCommand");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_TextCommand::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
