#include "stdafx.h"
#include "..\public\UI_Command_Weapon.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "SoundManager.h"


CUI_Command_Weapon::CUI_Command_Weapon(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_Command_Weapon::CUI_Command_Weapon(const CUI_Command_Weapon & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_Command_Weapon::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_Command_Weapon::NativeConstruct(void * pArg)
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

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, -150.f, 150.f)));
	// Setting =========

	SettingX = m_fX - (g_iWinCX*0.5f);
	SettingY = m_fY - (g_iWinCY*0.5f);


	if (0 == iCom_Texture)
	{
		SizeX = 288.f;
		SizeY = 48.f;;
		SetX = 354.f;
		SetY = 104.f;
		m_fFrame = 7.f;
		m_fDepth = 0.f; // ±íÀÌ
	}	
	if (1 == iCom_Texture)
	{
		SizeX = 288.f;
		SizeY = 48.f;;
		SetX = 354.f;
		SetY = 104.f;
		m_fFrame = 8.f;
		m_fDepth = 0.f; // ±íÀÌ
	}
	if (2 == iCom_Texture)
	{
		SizeX = 228.f;
		SizeY = 30.f;;
		SetX = 334.f;
		SetY = 104.f;
		m_fFrame = 9.f;
		m_fDepth = 0.4f; // ±íÀÌ
	}

	

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	return S_OK;
}

_int CUI_Command_Weapon::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	dTime += TimeDelta;
	if (0 == iCom_Texture
		|| 1 == iCom_Texture)
	{
		if (0.2 > dTime)
		{
			m_pTransformCom->Go_Right(TimeDelta * 7.0);
		}
		if (0.2 <= dTime && 0.7 > dTime)
		{

		}
		if (0.7 <= dTime)
		{
			m_pTransformCom->Go_Right(-TimeDelta * 10.0);
		}
		if (1.0 < dTime)
		{
			m_bDead = true;
		}
	}

	if (2 == iCom_Texture)
	{
		if (1.0 < dTime)
		{
			m_bDead = true;
		}
	}


	return RESULT_NOPROBLEM;
}

_int CUI_Command_Weapon::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	return RESULT_NOPROBLEM;
}

HRESULT CUI_Command_Weapon::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());
	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT CUI_Command_Weapon::SetUp_Components()
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

HRESULT CUI_Command_Weapon::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_fFrame));

	return S_OK;
}

CUI_Command_Weapon * CUI_Command_Weapon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Command_Weapon*	pInstance = new CUI_Command_Weapon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_Command_Weapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Command_Weapon::Clone(void * pArg)
{
	CUI_Command_Weapon*	pInstance = new CUI_Command_Weapon(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_Command_Weapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Command_Weapon::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
