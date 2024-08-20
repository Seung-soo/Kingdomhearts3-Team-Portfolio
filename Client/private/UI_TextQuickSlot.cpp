#include "stdafx.h"
#include "..\public\UI_TextQuickSlot.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "InvenManager.h"

CUI_TextQuickSlot::CUI_TextQuickSlot(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_TextQuickSlot::CUI_TextQuickSlot(const CUI_TextQuickSlot & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_TextQuickSlot::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_TextQuickSlot::NativeConstruct(void * pArg)
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
	SizeX = 288.f;
	SizeY = 48.f;

	SetX = 384.f;


	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::SLOT_1:
		SetY = 82.f;

		break;
	case Engine::CUI_Parents::SLOT_2:
		SetY = 60.f;

		break;
	case Engine::CUI_Parents::SLOT_3:
		SetY = 38.f;

		break;
	default:
		break;
	}

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	return S_OK;
}

_int CUI_TextQuickSlot::Tick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/*
	Add_GameObject 3번 생성
	Slot으로 switch 지정 후 각 Slot마다 frame으로 속성 지정
	===

	1. 포션 포션 포션 으로 띄운 뒤 특정 키입력으로 Frame을 변경해 다른 글자가 뜨는지 확인.
	3. Slot이 Null 일떄 Render Frame 0(빈 슬롯) , 작동 X

	*/
	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::SLOT_1:

		Slot_State(Inven->Get_Slot1_Name());
		
		if (0 == Inven->Get_QuickItem_Cnt())
		{
			SetX = 399.f;
		}
		else
		{
			SetX = 384.f;
		}

		break;
	case Engine::CUI_Parents::SLOT_2:

		Slot_State(Inven->Get_Slot2_Name());

		if (1 == Inven->Get_QuickItem_Cnt())
		{
			SetX = 399.f;
		}
		else
		{
			SetX = 384.f;
		}

		break;
	case Engine::CUI_Parents::SLOT_3:

		Slot_State(Inven->Get_Slot3_Name());

		if (2 == Inven->Get_QuickItem_Cnt())
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


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	return RESULT_NOPROBLEM;
}

_int CUI_TextQuickSlot::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::SLOT_1:
		break;
	case Engine::CUI_Parents::SLOT_2:
		break;
	case Engine::CUI_Parents::SLOT_3:
		break;
	default:
		break;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return RESULT_NOPROBLEM;
}

HRESULT CUI_TextQuickSlot::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;
	CHECK_FAILED(SetUp_ConstantTable());

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::SLOT_1:
		if (true == Inven->Get_QuickItem_Text())
		{
			if (0 == Inven->Get_QuickItem_Cnt())
			{
				m_pVIBufferCom->Render(m_pShaderCom, 0);
			}
			else
			{
				m_pVIBufferCom->Render(m_pShaderCom, 9);
			}
		}
		break;
	case Engine::CUI_Parents::SLOT_2:
		if (true == Inven->Get_QuickItem_Text())
		{
			if (1 == Inven->Get_QuickItem_Cnt())
			{
				m_pVIBufferCom->Render(m_pShaderCom, 0);
			}
			else
			{
				m_pVIBufferCom->Render(m_pShaderCom, 9);
			}
		}
		break;
	case Engine::CUI_Parents::SLOT_3:
		if (true == Inven->Get_QuickItem_Text())
		{
			if (2 == Inven->Get_QuickItem_Cnt())
			{
				m_pVIBufferCom->Render(m_pShaderCom, 0);
			}
			else
			{
				m_pVIBufferCom->Render(m_pShaderCom, 9);
			}
		}
		break;
	default:
		break;
	}
	return S_OK;
}

HRESULT CUI_TextQuickSlot::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ItemText"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	return S_OK;
}

HRESULT CUI_TextQuickSlot::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_fFrame));

	return S_OK;
}

void CUI_TextQuickSlot::Slot_State(_int _Name)
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

CUI_TextQuickSlot * CUI_TextQuickSlot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_TextQuickSlot*	pInstance = new CUI_TextQuickSlot(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_TextQuickSlot");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_TextQuickSlot::Clone(void * pArg)
{
	CUI_TextQuickSlot*	pInstance = new CUI_TextQuickSlot(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_TextQuickSlot");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_TextQuickSlot::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
