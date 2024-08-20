#include "stdafx.h"
#include "..\public\UI_Mouse.h"
#include "GameInstance.h"

CUI_Mouse::CUI_Mouse(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_Mouse::CUI_Mouse(const CUI_Mouse & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_Mouse::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_Mouse::NativeConstruct(void * pArg)
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

	m_UI.SizeX = 16.f;
	m_UI.SizeY = 16.f;
	m_UI.SettingX = m_fX - (g_iWinCX*0.5f);
	m_UI.SettingY = m_fY - (g_iWinCY*0.5f);

	m_fDepth = 0.0f; // 깊이
	m_fFrame = 0.f;

	m_pTransformCom->Scaled(XMVectorSet(m_UI.SizeX, m_UI.SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UI.SettingX + m_UI.SetX, m_UI.SettingY + m_UI.SetY, 0.f, 1.f)); // UI에도 적용가능 

	return S_OK;
}

_int CUI_Mouse::Tick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	POINT	ptMouse;
	GetCursorPos(&ptMouse);

	m_UI.SetX = (_float)ptMouse.x;
	m_UI.SetY = (_float)ptMouse.y;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UI.SettingX + m_UI.SetX, m_UI.SettingY + m_UI.SetY, 0.f, 1.f)); // UI에도 적용가능 

	return RESULT_NOPROBLEM;
}

_int CUI_Mouse::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return RESULT_NOPROBLEM;
}

HRESULT CUI_Mouse::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT CUI_Mouse::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */ //====================
	CHECK_FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Copmonent_Texture_Logo"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	return S_OK;
}

HRESULT CUI_Mouse::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture"));

	return S_OK;
}

CUI_Mouse * CUI_Mouse::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Mouse*	pInstance = new CUI_Mouse(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_Mouse");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Mouse::Clone(void * pArg)
{
	CUI_Mouse*	pInstance = new CUI_Mouse(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_Mouse");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Mouse::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
