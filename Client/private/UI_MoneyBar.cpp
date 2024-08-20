#include "stdafx.h"
#include "..\public\UI_MoneyBar.h"
#include "GameInstance.h"
#include "UI_Num_Manager.h"
#include "GameManager.h"

CUI_MoneyBar::CUI_MoneyBar(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_MoneyBar::CUI_MoneyBar(const CUI_MoneyBar & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_MoneyBar::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_MoneyBar::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));

	//iCom_Texture = (*(_int*)pArg); // 텍스쳐 정보
	CHECK_FAILED(SetUp_Components());

	// UI 기본 Setting ===

	m_fSizeX = g_iWinCX;
	m_fSizeY = g_iWinCY;
	m_fX = 0.f;
	m_fY = 0.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, 0.f, 1.f)));

	// Setting =========

	SizeX = 256.f;
	SizeY = 48.f;
	SettingX = m_fX - (g_iWinCX*0.5f);
	SettingY = m_fY - (g_iWinCY*0.5f);

	SetX = 128.f;
	SetY = 180.f; // 280.f
	m_fDepth = 0.19f; // 깊이

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	return S_OK;
}

_int CUI_MoneyBar::Tick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	if (false == GM->Get_UIRender())
		return 0;

	//SetX = 1100.f + NM->Get_ComboPos();
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == NM->Get_MoneyBar_Render())
	{
		if (280.f > SetY)
		{
			fSpeed_A *= 0.97f;
			SetY += fSpeed_A;

			if (280.f < SetY)
			{
				SetY = 280.f;
				fSpeed_B = 10.f;
			}
				
			
			NM->Set_MoneyBarPos(SetY);
		}
	}
	if (false == NM->Get_MoneyBar_Render())
	{
		if (180.f < SetY)
		{
			fSpeed_B *= 0.97f;
			SetY -= fSpeed_B;

			if (180.f > SetY)
			{
				SetY = 180.f;
				fSpeed_A = 10.f;
			}
				

			NM->Set_MoneyBarPos(SetY);
		}
	}

	fMoneyFadeOut = (280.f - SetY) * 0.01f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	return RESULT_NOPROBLEM;
}

_int CUI_MoneyBar::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return RESULT_NOPROBLEM;
}

HRESULT CUI_MoneyBar::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (false == GM->Get_UIRender())
		return S_OK;

	CHECK_FAILED(SetUp_ConstantTable());

	CHECK_FAILED(m_pShaderCom->Set_RawValue("MoneyBar", &fMoneyFadeOut, sizeof(_float)))

	m_pVIBufferCom->Render(m_pShaderCom, 12);

	return S_OK;
}

HRESULT CUI_MoneyBar::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_MoneyBar"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	return S_OK;
}

HRESULT CUI_MoneyBar::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture"));

	return S_OK;
}

CUI_MoneyBar * CUI_MoneyBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_MoneyBar*	pInstance = new CUI_MoneyBar(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_MoneyBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_MoneyBar::Clone(void * pArg)
{
	CUI_MoneyBar*	pInstance = new CUI_MoneyBar(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_MoneyBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_MoneyBar::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
