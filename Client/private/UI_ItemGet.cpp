#include "stdafx.h"
#include "..\public\UI_ItemGet.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "InvenManager.h"
#include "SoundManager.h"

#include "Item.h"

CUI_ItemGet::CUI_ItemGet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_ItemGet::CUI_ItemGet(const CUI_ItemGet & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_ItemGet::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_ItemGet::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));
	CHECK_FAILED(SetUp_Components());

	// UI 기본 Setting ===
	m_fSizeX = g_iWinCX;
	m_fSizeY = g_iWinCY;
	m_fX = 0.f;
	m_fY = 0.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, 0.f, 1.f)));

	// Setting =========

	iPattern_Cnt = PATTERN_1;
	iPosition_Cnt = 0;

	SizeX = 256.f;
	SizeY = 128.f;
	SettingX = m_fX - (g_iWinCX*0.5f);
	SettingY = m_fY - (g_iWinCY*0.5f);
	SetX = -256.f;
	SetY = 600.f;

	m_fDepth = 0.1f; // 깊이

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));


	SM->Sound_Command(L"UI_ItemGet.wav", 0.5f);
	return S_OK;
}

_int CUI_ItemGet::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	if (false == GM->Get_UIRender())
		return 0;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	switch (iPattern_Cnt)
	{
	case Client::CUI_ItemGet::PATTERN_1:

		fSpeedX *= 1.55f;
		SetX += fSpeedX;


		if (128.f < SetX)
		{
			iItemCnt = rand() % 3;

			switch (iItemCnt) // 3 4 5 는 나중에 상점 구매
			{
			case 0:
				Inven->Math_Potion(1);
				Inven->Inven_sort(SLOT_POTION);
				break;
			case 1:
				Inven->Math_High_Potion(1);
				Inven->Inven_sort(SLOT_HIGH_POTION);
				break;
			case 2:
				Inven->Math_Elixir(1);
				Inven->Inven_sort(SLOT_ELIXIR);
				break;
			default:
				break;
			}

			m_UI.pSetX = &SetX;
			m_UI.pSetY = &SetY;
			//m_UI.p_iProperties = &iItemCnt;
			Inven->Set_Frame(iItemCnt);

			pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Item"), TEXT("Prototype_GameObject_UI_ItemIcon"), &m_UI);

			iPattern_Cnt = PATTERN_2;
		}
		break;
	case Client::CUI_ItemGet::PATTERN_2:
		
		SetX = 128.f;
		dGetTime += TimeDelta;

		if (1.3 < dGetTime)
			iPattern_Cnt = PATTERN_3;

		break;
	case Client::CUI_ItemGet::PATTERN_3:

		SetY *= 1.05f;

		if (900.f < SetY)
			m_bDead = true;

		break;
	default:
		break;
	}

	if (true == GM->Get_bItem() && 1 <= GM->Get_ItemCnt())
	{
		GM->Set_bItem(false);
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	return RESULT_NOPROBLEM;
}

_int CUI_ItemGet::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return RESULT_NOPROBLEM;
}

HRESULT CUI_ItemGet::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (false == GM->Get_UIRender())
		return S_OK;

	CHECK_FAILED(SetUp_ConstantTable());

	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT CUI_ItemGet::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_Get"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	return S_OK;
}

HRESULT CUI_ItemGet::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture"));

	return S_OK;
}


void CUI_ItemGet::Creat_Item()
{
}

CUI_ItemGet * CUI_ItemGet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_ItemGet*	pInstance = new CUI_ItemGet(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_ItemGet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_ItemGet::Clone(void * pArg)
{
	CUI_ItemGet*	pInstance = new CUI_ItemGet(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_ItemGet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_ItemGet::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
