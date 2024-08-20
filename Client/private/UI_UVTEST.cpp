#include "stdafx.h"
#include "..\public\UI_UVTEST.h"
#include "GameInstance.h"

CUI_UVTEST::CUI_UVTEST(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_UVTEST::CUI_UVTEST(const CUI_UVTEST & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_UVTEST::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_UVTEST::NativeConstruct(void * pArg)
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

	m_UI.SizeX = 400.f;
	m_UI.SizeY = 400.f;
	m_UI.SettingX = m_fX - (g_iWinCX*0.5f);
	m_UI.SettingY = m_fY - (g_iWinCY*0.5f);
	m_UI.SetX = 600.f;
	m_UI.SetY = 300.f;

	m_fDepth = 0.f; // 깊이
	m_fFrame = 0.f;

	m_pTransformCom->Scaled(XMVectorSet(m_UI.SizeX, m_UI.SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UI.SettingX + m_UI.SetX, m_UI.SettingY + m_UI.SetY, 0.f, 1.f)); // UI에도 적용가능 


	return S_OK;
}

_int CUI_UVTEST::Tick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_KeyPressing(DIK_TAB))
	{
		//RGBA
		//if (pGameInstance->Get_KeyPressing(DIK_1)) /* R */
		//{
		//	fUVR += 0.01f;
		//}
		//if (pGameInstance->Get_KeyPressing(DIK_2))
		//{
		//	fUVR -= 0.01f;
		//}
		//if (pGameInstance->Get_KeyPressing(DIK_3)) /* G */
		//{
		//	fUVG += 0.01f;
		//}
		//if (pGameInstance->Get_KeyPressing(DIK_4))
		//{
		//	fUVG -= 0.01f;
		//}
		//if (pGameInstance->Get_KeyPressing(DIK_5)) /* B */
		//{
		//	fUVB += 0.01f;
		//}
		//if (pGameInstance->Get_KeyPressing(DIK_6))
		//{
		//	fUVB -= 0.01f;
		//}
		//if (pGameInstance->Get_KeyPressing(DIK_7)) /* A */
		//{
		//	fUVA += 0.01f;
		//}
		//if (pGameInstance->Get_KeyPressing(DIK_8))
		//{
		//	fUVA -= 0.01f;
		//}

		//XY

		if (pGameInstance->Get_KeyPressing(DIK_RIGHT)) /* X */
		{
			fUVX += 0.01f;

			if (fUVX > 1.f)
				fUVX = 1.f;
			if (fUVX < 0.f)
				fUVX = 0.f;
		}
		if (pGameInstance->Get_KeyPressing(DIK_LEFT))
		{
			fUVX -= 0.01f;
			if (fUVX > 1.f)
				fUVX = 1.f;
			if (fUVX < 0.f)
				fUVX = 0.f;
		}
		if (pGameInstance->Get_KeyPressing(DIK_UP)) /* Y */
		{
			fUVY -= 0.01f;
			if (fUVY > 1.f)
				fUVY = 1.f;
			if (fUVY < 0.f)
				fUVY = 0.f;
		}
		if (pGameInstance->Get_KeyPressing(DIK_DOWN))
		{
			fUVY += 0.01f;
			if (fUVY > 1.f)
				fUVY = 1.f;
			if (fUVY < 0.f)
				fUVY = 0.f;
		}

	}


	return RESULT_NOPROBLEM;
}

_int CUI_UVTEST::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	//===
	//CHECK_FAILED(m_pShaderCom->Set_RawValue("HP", &fSetMP_Second, sizeof(_float)))

	//CHECK_FAILED(m_pShaderCom->Set_RawValue("fUV_R", &fUVR, sizeof(_float)));
	//CHECK_FAILED(m_pShaderCom->Set_RawValue("fUV_G", &fUVG, sizeof(_float)));
	//CHECK_FAILED(m_pShaderCom->Set_RawValue("fUV_B", &fUVB, sizeof(_float)));
	//CHECK_FAILED(m_pShaderCom->Set_RawValue("fUV_A", &fUVA, sizeof(_float)));

	CHECK_FAILED(m_pShaderCom->Set_RawValue("fUV_Y", &fUVY, sizeof(_float)));

	return RESULT_NOPROBLEM;
}

HRESULT CUI_UVTEST::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	m_pVIBufferCom->Render(m_pShaderCom, 2);

	return S_OK;
}

HRESULT CUI_UVTEST::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */ //====================
	CHECK_FAILED(__super::Add_Component(LEVEL_TWILIGHT, TEXT("Prototype_Component_Texture_UVTEST"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	return S_OK;
}

HRESULT CUI_UVTEST::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture"));

	return S_OK;
}

CUI_UVTEST * CUI_UVTEST::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_UVTEST*	pInstance = new CUI_UVTEST(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_UVTEST");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_UVTEST::Clone(void * pArg)
{
	CUI_UVTEST*	pInstance = new CUI_UVTEST(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_UVTEST");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_UVTEST::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
