#include "stdafx.h"
#include "..\public\Menu_ItemText.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "InvenManager.h"
#include "UI_Num_Manager.h"

CMenu_ItemText::CMenu_ItemText(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CMenu_ItemText::CMenu_ItemText(const CMenu_ItemText & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CMenu_ItemText::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CMenu_ItemText::NativeConstruct(void * pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

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

	SettingX = m_fX - (g_iWinCX*0.5f);
	SettingY = m_fY - (g_iWinCY*0.5f);



	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::ITEMWINDOWQUICK_SLOT1:
		SizeX = 200.f * 0.5f;
		SizeY = 50.f * 0.5f;
		SetX = 675.f;
		SetY = 594.f;

		m_fDepth = 0.f; // ±íÀÌ
		m_fFrame = 0.f;

		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));
		break;
	case Engine::CUI_Parents::ITEMWINDOWQUICK_SLOT2:
		SizeX = 200.f * 0.5f;
		SizeY = 50.f * 0.5f;
		SetX = 680.f;
		SetY = 559.f;

		m_fDepth = 0.f; // ±íÀÌ
		m_fFrame = 0.f;

		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));
		break;
	case Engine::CUI_Parents::ITEMWINDOWQUICK_SLOT3:
		SizeX = 200.f * 0.5f;
		SizeY = 50.f * 0.5f;
		SetX = 685.f;
		SetY = 524.f;

		m_fDepth = 0.f; // ±íÀÌ
		m_fFrame = 0.f;

		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));
		break;
	case Engine::CUI_Parents::ITEMWINDOWQUICK_SLOT4:
		SizeX = 200.f * 0.5f;
		SizeY = 50.f * 0.5f;
		SetX = 690.f;
		SetY = 489.f;

		m_fDepth = 0.f; // ±íÀÌ
		m_fFrame = 7.f;

		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));
		break;

	case Engine::CUI_Parents::ITEMWINDOW_SLOT1: /* Inven SLOT */
		SizeX = 200.f * 0.8f;
		SizeY = 50.f * 0.8f;

		/* inven */
		if (true == Inven->Get_ShopItemText())
		{
			SetX = 260.f;
			SetY = 510.f;
		}
		else
		{
			SetX = 240.f;
			SetY = 510.f;
		}

		/* ===== */

		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));
		m_fDepth = 0.f; // ±íÀÌ
		m_fFrame = 0.f;

		break;
	case Engine::CUI_Parents::ITEMWINDOW_SLOT2:
		SizeX = 200.f * 0.8f;
		SizeY = 50.f * 0.8f;

		/* inven */
		if (true == Inven->Get_ShopItemText())
		{
			SetX = 268.f;
			SetY = 455.f;
		}
		else
		{
			SetX = 250.f;
			SetY = 454.f;
		}
		/* ===== */

		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));
		m_fDepth = 0.f; // ±íÀÌ
		m_fFrame = 0.f;
		break;
	case Engine::CUI_Parents::ITEMWINDOW_SLOT3:
		SizeX = 200.f * 0.8f;
		SizeY = 50.f * 0.8f;

		/* inven */
		if (true == Inven->Get_ShopItemText())
		{
			SetX = 273.f;
			SetY = 402.f;
		}
		else
		{
			SetX = 260.f;
			SetY = 397.f;
		}
		/* ===== */

		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));
		m_fDepth = 0.f; // ±íÀÌ
		m_fFrame = 0.f;
		break;
	case Engine::CUI_Parents::ITEMWINDOW_SLOT4:
		SizeX = 200.f * 0.8f;
		SizeY = 50.f * 0.8f;

		/* inven */
		if (true == Inven->Get_ShopItemText())
		{
			SetX = 276.f;
			SetY = 344.f;
		}
		else
		{
			SetX = 270.f;
			SetY = 342.f;
		}
		/* ===== */

		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));
		m_fDepth = 0.f; // ±íÀÌ
		m_fFrame = 0.f;
		break;
	case Engine::CUI_Parents::ITEMWINDOW_SLOT5:
		SizeX = 200.f * 0.8f;
		SizeY = 50.f * 0.8f;

		/* inven */
		if (true == Inven->Get_ShopItemText())
		{
			SetX = 284.f;
			SetY = 287.f;
		}
		else
		{
			SetX = 280.f;
			SetY = 285.f;
		}
		/* ===== */

		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));
		m_fDepth = 0.f; // ±íÀÌ
		m_fFrame = 0.f;
		break;
	case Engine::CUI_Parents::ITEMWINDOW_SLOT6:
		SizeX = 200.f * 0.8f;
		SizeY = 50.f * 0.8f;

		/* inven */
		if (true == Inven->Get_ShopItemText())
		{
			SetX = 292.f;
			SetY = 230.f;
		}
		else
		{
			SetX = 290.f;
			SetY = 228.f;
		}
		/* ===== */

		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));
		m_fDepth = 0.f; // ±íÀÌ
		m_fFrame = 0.f;
		break;
	default:
		break;
	}

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	return S_OK;
}

_int CMenu_ItemText::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	_float PlusX = 200.f;
	_float PlusY = 30.f;

	/* TEST === */
	//if (m_pGameInstance->Get_KeyPressing(DIK_UP))
	//	SetY += 1.f;

	//if (m_pGameInstance->Get_KeyPressing(DIK_DOWN))
	//	SetY -= 1.f;
	//
	//if (m_pGameInstance->Get_KeyPressing(DIK_LEFT))
	//	SetX -= 1.f;

	//if (m_pGameInstance->Get_KeyPressing(DIK_RIGHT))
	//	SetX += 1.f;
	/* ========= */

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::ITEMWINDOWQUICK_SLOT1:

		Slot_State(Inven->Get_Slot1_Name());

		if (true == Inven->Get_InvenDelete())
			m_bDead = true;

		break;
	case Engine::CUI_Parents::ITEMWINDOWQUICK_SLOT2:

		Slot_State(Inven->Get_Slot2_Name());

		if (true == Inven->Get_InvenDelete())
			m_bDead = true;

		break;
	case Engine::CUI_Parents::ITEMWINDOWQUICK_SLOT3:

		Slot_State(Inven->Get_Slot3_Name());

		if (true == Inven->Get_InvenDelete())
			m_bDead = true;

		break;
	case Engine::CUI_Parents::ITEMWINDOWQUICK_SLOT4: /* QUICKSLOT Á¾·á */

		if (true == Inven->Get_InvenDelete())
			m_bDead = true;

		break;
	case Engine::CUI_Parents::ITEMWINDOW_SLOT1: /* Inven SLOT */

		Slot_State(Inven->Get_InvenSlot1_Name());
		if (true == Inven->Get_InvenDelete_List())
			m_bDead = true;
		
		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		XMStoreFloat4(&fPosition, vPosition);
		NM->Setting_InvenNumber1(Inven->Get_InvenSlot1(), 22.f, fPosition.x + 640.f + PlusX, fPosition.y + 360.f + PlusY);
		break;
	case Engine::CUI_Parents::ITEMWINDOW_SLOT2:

		Slot_State(Inven->Get_InvenSlot2_Name());
		if (true == Inven->Get_InvenDelete_List())
			m_bDead = true;

		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		XMStoreFloat4(&fPosition, vPosition);
		NM->Setting_InvenNumber2(Inven->Get_InvenSlot2(), 22.f, fPosition.x + 640.f + PlusX, fPosition.y + 360.f + PlusY);
		break;
	case Engine::CUI_Parents::ITEMWINDOW_SLOT3:

		Slot_State(Inven->Get_InvenSlot3_Name());
		if (true == Inven->Get_InvenDelete_List())
			m_bDead = true;

		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		XMStoreFloat4(&fPosition, vPosition);
		NM->Setting_InvenNumber3(Inven->Get_InvenSlot3(), 22.f, fPosition.x + 640.f + PlusX, fPosition.y + 360.f + PlusY);
		break;
	case Engine::CUI_Parents::ITEMWINDOW_SLOT4:

		Slot_State(Inven->Get_InvenSlot4_Name());
		if (true == Inven->Get_InvenDelete_List())
			m_bDead = true;

		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		XMStoreFloat4(&fPosition, vPosition);
		NM->Setting_InvenNumber4(Inven->Get_InvenSlot4(), 22.f, fPosition.x + 640.f + PlusX, fPosition.y + 360.f + PlusY);
		break;
	case Engine::CUI_Parents::ITEMWINDOW_SLOT5:

		Slot_State(Inven->Get_InvenSlot5_Name());
		if (true == Inven->Get_InvenDelete_List())
			m_bDead = true;

		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		XMStoreFloat4(&fPosition, vPosition);
		NM->Setting_InvenNumber5(Inven->Get_InvenSlot5(), 22.f, fPosition.x + 640.f + PlusX, fPosition.y + 360.f + PlusY);
		break;
	case Engine::CUI_Parents::ITEMWINDOW_SLOT6:

		Slot_State(Inven->Get_InvenSlot6_Name());
		if (true == Inven->Get_InvenDelete_List())
			m_bDead = true;

		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		XMStoreFloat4(&fPosition, vPosition);
		NM->Setting_InvenNumber6(Inven->Get_InvenSlot6(), 22.f, fPosition.x + 640.f + PlusX, fPosition.y + 360.f + PlusY);
		break;
	default:
		break;
	}

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(SettingX + SetX + Inven->Get_MovePositionX() ,SettingY + SetY + Inven->Get_MovePositionY(), 0.f, 1.f));

	return _int();
}

_int CMenu_ItemText::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::ITEMWINDOWQUICK_SLOT1:

		break;
	case Engine::CUI_Parents::ITEMWINDOWQUICK_SLOT2:

		break;
	case Engine::CUI_Parents::ITEMWINDOWQUICK_SLOT3:

		break;
	case Engine::CUI_Parents::ITEMWINDOWQUICK_SLOT4:

		break;
	case Engine::CUI_Parents::ITEMWINDOW_SLOT1: /* Inven SLOT */

		break;
	case Engine::CUI_Parents::ITEMWINDOW_SLOT2:

		break;
	case Engine::CUI_Parents::ITEMWINDOW_SLOT3:

		break;
	case Engine::CUI_Parents::ITEMWINDOW_SLOT4:

		break;
	case Engine::CUI_Parents::ITEMWINDOW_SLOT5:

		break;
	case Engine::CUI_Parents::ITEMWINDOW_SLOT6:

		break;
	default:
		break;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);


	return S_OK;
}

HRESULT CMenu_ItemText::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;


	CHECK_FAILED(SetUp_ConstantTable());

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::ITEMWINDOWQUICK_SLOT1:
	case Engine::CUI_Parents::ITEMWINDOWQUICK_SLOT2:
	case Engine::CUI_Parents::ITEMWINDOWQUICK_SLOT3:
	case Engine::CUI_Parents::ITEMWINDOWQUICK_SLOT4:

		m_pVIBufferCom->Render(m_pShaderCom, 0);

		break;
	case Engine::CUI_Parents::ITEMWINDOW_SLOT1: /* Inven SLOT */
	case Engine::CUI_Parents::ITEMWINDOW_SLOT2:
	case Engine::CUI_Parents::ITEMWINDOW_SLOT3:
	case Engine::CUI_Parents::ITEMWINDOW_SLOT4:
	case Engine::CUI_Parents::ITEMWINDOW_SLOT5:
	case Engine::CUI_Parents::ITEMWINDOW_SLOT6:
		//if (true == Inven->Get_InvenList_Render())
		m_pVIBufferCom->Render(m_pShaderCom, 0);
		break;

	default:
		break;
	}

	return S_OK;
}

HRESULT CMenu_ItemText::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_ItemText"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	return S_OK;
}

HRESULT CMenu_ItemText::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_fFrame));

	return S_OK;
}

void CMenu_ItemText::Slot_State(_int _Name)
{
	switch (_Name)
	{
	case SLOT_EMPTY:
		m_fFrame = 0;
		break;
	case SLOT_POTION:
		m_fFrame = 1;
		break;
	case SLOT_HIGH_POTION:
		m_fFrame = 2;
		break;
	case SLOT_ELIXIR:
		m_fFrame = 3;
		break;
	case SLOT_ATKUP:
		m_fFrame = 4;
		break;
	case SLOT_ATKSPEEDUP:
		m_fFrame = 5;
		break;
	case SLOT_SPEEDUP:
		m_fFrame = 6;
		break;
	default:
		break;
	}
}

CMenu_ItemText * CMenu_ItemText::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMenu_ItemText*	pInstance = new CMenu_ItemText(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CMenu_ItemText");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMenu_ItemText::Clone(void * pArg)
{
	CMenu_ItemText*	pInstance = new CMenu_ItemText(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMenu_ItemText");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMenu_ItemText::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
