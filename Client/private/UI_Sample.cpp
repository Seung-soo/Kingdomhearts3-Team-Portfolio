#include "stdafx.h"
#include "..\public\UI_Sample.h"
#include "GameInstance.h"

UI_Sample::UI_Sample(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

UI_Sample::UI_Sample(const UI_Sample & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT UI_Sample::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT UI_Sample::NativeConstruct(void * pArg)
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

	/* //Sample
		m_UI.SizeX = 64.f;
		m_UI.SizeY = 64.f;
		m_UI.SettingX = m_fX - (g_iWinCX*0.5f);
		m_UI.SettingY = m_fY - (g_iWinCY*0.5f);
		m_UI.SetX = 1200.f;
		m_UI.SetY = 50.f;

		m_fDepth = 0.f; // 깊이

		m_pTransformCom->Scaled(XMVectorSet(m_UI.SizeX, m_UI.SizeY, 1.f, 0.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UI.SettingX + m_UI.SetX , m_UI.SettingY + m_UI.SetY, 0.f , 1.f)); // UI에도 적용가능 
	*/

	//===============

	return S_OK;
}

_int UI_Sample::Tick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	return RESULT_NOPROBLEM;
}

_int UI_Sample::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return RESULT_NOPROBLEM;
}

HRESULT UI_Sample::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT UI_Sample::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */ //====================

	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Sample"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	//Sample	
	//if (UI_NAME::UI_BLACK == iCom_Texture)
	//	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BlackBoard"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	//======================================

	return S_OK;
}

HRESULT UI_Sample::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	// TextureCom Set ======


	/* // Sample
	if (UI_NAME::UI_LOADING == iCom_Texture)
		CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_fFrame));

	if (UI_NAME::UI_BLACK == iCom_Texture)
		CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture"));
	*/
	//====================

	return S_OK;
}

UI_Sample * UI_Sample::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	UI_Sample*	pInstance = new UI_Sample(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created UI_Sample");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * UI_Sample::Clone(void * pArg)
{
	UI_Sample*	pInstance = new UI_Sample(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created UI_Sample");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void UI_Sample::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
