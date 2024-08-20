#include "stdafx.h"
#include "..\public\UI_BossHPBar.h"
#include "GameInstance.h"
#include "GameManager.h"


CUI_BossHPBar::CUI_BossHPBar(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_BossHPBar::CUI_BossHPBar(const CUI_BossHPBar & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_BossHPBar::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_BossHPBar::NativeConstruct(void * pArg)
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

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, 0.f, 1.f)));

	// Setting =========

	switch (iCom_Texture)
	{
	case 0:
		SizeX = 350.f;
		SizeY = 450.f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 1110.f;
		SetY = 460.f;

		m_fFrame = 0.f;
		m_fDepth = 0.3f; // ±íÀÌ
		break;
	case 1:
		SizeX = 350.f;
		SizeY = 450.f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 1110.f;
		SetY = 460.f;

		m_fFrame = 1.f;
		m_fDepth = 0.29f; // ±íÀÌ
		break;
	case 2:
		SizeX = 350.f;
		SizeY = 450.f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 1110.f;
		SetY = 460.f;

		m_fFrame = 2.f;
		m_fDepth = 0.29f; // ±íÀÌ
		break;
	case 3:
		SizeX = 350.f;
		SizeY = 450.f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 1110.f;
		SetY = 460.f;

		m_fFrame = 3.f;
		m_fDepth = 0.29f; // ±íÀÌ
		break;
	case 4:
		SizeX = 350.f;
		SizeY = 450.f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 1110.f;
		SetY = 460.f;

		m_fFrame = 4.f;
		m_fDepth = 0.29f; // ±íÀÌ
		break;
	case 5:
		SizeX = 350.f;
		SizeY = 450.f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 1110.f;
		SetY = 460.f;

		m_fFrame = 5.f;
		m_fDepth = 0.29f; // ±íÀÌ
		break;

	case 6:
		SizeX = 350.f;
		SizeY = 450.f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 1110.f;
		SetY = 460.f;

		m_fFrame = 6.f; 
		m_fDepth = 0.33f;
		break;

	case 7:

		break;
	default:
		break;
	}

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));
	return S_OK;
}

_int CUI_BossHPBar::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	switch (iCom_Texture)
	{
	case 0:

		break;
	case 1:

		break;
	case 2:

		break;
	case 3:

		break;
	case 4:

		break;
	case 5:

		break;
	case 6:
		/* m_fFrame : 6~11*/


		break;
	default:
		break;
	}

	
	return RESULT_NOPROBLEM;
}

_int CUI_BossHPBar::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	switch (iCom_Texture)
	{
	case 0:

		break;
	case 1:

		break;
	case 2:

		break;
	case 3:

		break;
	case 4:

		break;
	case 5:

		break;
	case 6:

		break;
	default:
		break;
	}
	if (true == GM->Get_UIRender())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return RESULT_NOPROBLEM;
}

HRESULT CUI_BossHPBar::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT CUI_BossHPBar::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Boss_HP_Bar"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	return S_OK;
}

HRESULT CUI_BossHPBar::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_int)m_fFrame));

	return S_OK;
}

CUI_BossHPBar * CUI_BossHPBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_BossHPBar*	pInstance = new CUI_BossHPBar(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_BossHPBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_BossHPBar::Clone(void * pArg)
{
	CUI_BossHPBar*	pInstance = new CUI_BossHPBar(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_BossHPBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_BossHPBar::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
