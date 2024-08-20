#include "stdafx.h"
#include "..\public\UI_Minigame_Num.h"
#include "GameInstance.h"
#include "UI_Num_Manager.h"

CUI_Minigame_Num::CUI_Minigame_Num(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_Minigame_Num::CUI_Minigame_Num(const CUI_Minigame_Num & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_Minigame_Num::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_Minigame_Num::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));

	//iCom_Texture = (*(_int*)pArg);
	CHECK_FAILED(SetUp_Components());

	// UI ±âº» Setting ===

	m_fSizeX = g_iWinCX;
	m_fSizeY = g_iWinCY;
	m_fX = 0.f;
	m_fY = 0.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, 0.f, 1.f)));

	// Setting =========

	iPush_Num = (*(FONT_SET*)pArg).Number;
	fPush_Size = (*(FONT_SET*)pArg).Size;
	fGap = (*(FONT_SET*)pArg).Gap;

	fSize_Position = fPush_Size * 0.2f;

	//===============
	m_fFrame = (_float)iPush_Num;

	SizeX = fPush_Size;
	SizeY = fPush_Size;
	SettingX = m_fX - (g_iWinCX*0.5f);
	SettingY = m_fY - (g_iWinCY*0.5f);

	SetX = 1200.f + fGap;
	SetY = 100.f;
	m_fDepth = 0.f; // ±íÀÌ

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));
	return S_OK;
}

_int CUI_Minigame_Num::Tick(_double TimeDelta)
{
	if (true == NM->Get_ComboRender())
		return RESULT_DEAD;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	return RESULT_NOPROBLEM;
}

_int CUI_Minigame_Num::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return RESULT_NOPROBLEM;
}

HRESULT CUI_Minigame_Num::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT CUI_Minigame_Num::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Num_Font"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	return S_OK;
}

HRESULT CUI_Minigame_Num::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_fFrame));

	return S_OK;
}

CUI_Minigame_Num * CUI_Minigame_Num::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Minigame_Num*	pInstance = new CUI_Minigame_Num(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_Minigame_Num");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Minigame_Num::Clone(void * pArg)
{
	CUI_Minigame_Num*	pInstance = new CUI_Minigame_Num(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_Minigame_Num");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Minigame_Num::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
