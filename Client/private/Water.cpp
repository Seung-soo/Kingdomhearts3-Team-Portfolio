#include "stdafx.h"
#include "..\public\Water.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "GameManager.h"
#include "ImGui_Manager.h"

CWater::CWater(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CWater::CWater(const CWater & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWater::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWater::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CWater::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CWater::NativeConstruct");
		return E_FAIL;
	}

	_vector vPosition = XMLoadFloat4((_float4*)pArg);

	m_pTransformCom->Set_Position(vPosition);

	m_iPass = 3;
	m_iTexIndex = 89;

	// For Shadow
	m_isStaticShadow = true;

	//m_bImGuiController = true;

	return S_OK;
}

_int CWater::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CWater::Tick");
		return RESULT_ERROR;
	}

	if (m_pGameInstance->Get_KeyEnter(DIK_RIGHT))
	{
		m_iTexIndex++;
		if (99 < m_iTexIndex)
			m_iTexIndex = 0;
	}

	if (m_pGameInstance->Get_KeyEnter(DIK_LEFT))
	{
		m_iTexIndex--;
		if (0 > m_iTexIndex)
			m_iTexIndex = 99;
	}

	m_fNoiseUV += (_float)TimeDelta * 0.1f;

	return _int();
}

_int CWater::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CWater::LateTick");
		return RESULT_ERROR;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	return _int();
}

HRESULT CWater::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CWater::Render");
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, m_iPass)))
	{
		MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CWater::Render");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CWater::Render_Shadow()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_LightViewMatrix", &pGameInstance->Get_LightMatrix(CLight_Manager::SHADOW_STATIC).LightViewMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_LightProjMatrix", &pGameInstance->Get_LightMatrix(CLight_Manager::SHADOW_STATIC).LightProjMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 4)))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	return S_OK;
}

void CWater::PostRender(_double TimeDelta)
{
	ImGui::Begin("WaterColor");

	static _float fColor[4] = { 0.29f,0.29f,0.41f,0.f };

	if (ImGui::DragFloat4("Color", fColor, 0.01f, -1.f, 1.f, "%.2f"))
		memcpy(&m_vWaterColor, fColor, sizeof(_float4));


	ImGui::End();
}


HRESULT CWater::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CWater::SetUp_Components(Renderer)");
		return E_FAIL;
	}
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CWater::SetUp_Components(Shader");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_RAPUNZEL, TEXT("Prototype_Component_VIBuffer_Terrain"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CWater::SetUp_Components(VIBuffer_Rect)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Water"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_NoiseTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Noise"), TEXT("Com_NoiseTexture"), (CComponent**)&m_pNoiseTextureCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_NormalTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Water_Normal"), TEXT("Com_NormalTexture"), (CComponent**)&m_pNormalTextureCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CWater::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CWater::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CWater::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CWater::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CWater::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture")))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pNoiseTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourDiffuseTexture", m_iTexIndex)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_fWaveValue", &m_fNoiseUV, sizeof(float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CWater::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_vWaterColor", &m_vWaterColor, sizeof(_float4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CWater::SetUp_ConstantTable(g_vWaterColor)");
		return E_FAIL;
	}
	
	return S_OK;
}



CWater * CWater::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CWater*	pInstance = new CWater(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CWater");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CWater::Clone(void * pArg)
{
	CWater*	pInstance = new CWater(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CWater");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWater::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pNormalTextureCom);
	Safe_Release(m_pNoiseTextureCom);
}
