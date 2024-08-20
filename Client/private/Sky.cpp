#include "stdafx.h"
#include "..\Public\Sky.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "ImGui_Manager.h"

CSky::CSky(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CSky::CSky(const CSky & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSky::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSky::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	// 여기서 레벨에 맞게 스카이박스 인덱스 변경

	switch (GM->Get_CurrentLevel())
	{
	case LEVEL_TWILIGHT:
		m_vColor = _float4(-0.1f, -0.1f, -0.1f, 1.f);
		m_iTexIndex = 0;
		break;
	case LEVEL_RAPUNZEL:
		m_vColor = _float4(-0.31f, -0.3f, -0.2f, 1.f);
		m_iTexIndex = 6;
		break;
	case LEVEL_RAPUNZEL_BOSS:
		m_iTexIndex = 10;
		break;
	case LEVEL_FROZEN:
		m_iTexIndex = 9;
		break;
	case LEVEL_KEYGRAVE:
		m_iTexIndex = 47;
		break;
	case LEVEL_FINALBOSS:
		m_vColor = _float4(0.3f, 0.3f, 0.3f, 1.f);
		m_iTexIndex = 8;
		break;
	default:
		m_iTexIndex = 5;
		break;
	}

	if (m_bEffectLevel == true)
		m_iTexIndex = 47;

	//m_bImGuiController = true;

	return S_OK;
}

_int CSky::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	m_fNoiseX += (_float)TimeDelta * 0.2f;

	//if (LEVEL_KEYGRAVE == GM->Get_CurrentLevel() || m_bEffectLevel == true)
	//{
	//	if (m_pGameInstance->Get_KeyEnter(DIK_RIGHT))
	//	{
	//		++m_iTexIndex;
	//		if (100 == m_iTexIndex)
	//			m_iTexIndex = 0;
	//	}
	//	else if (m_pGameInstance->Get_KeyEnter(DIK_LEFT))
	//	{
	//		--m_iTexIndex;
	//		if (0 == m_iTexIndex)
	//			m_iTexIndex = 99;
	//	}
	//}

	return RESULT_NOPROBLEM;
}

_int CSky::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return	RESULT_ERROR;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);

	return RESULT_NOPROBLEM;
}

HRESULT CSky::Render()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pGameInstance->Get_CamPosition());

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (LEVEL_KEYGRAVE != GM->Get_CurrentLevel() && m_bEffectLevel == false)
	{
		if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 3)))
			return E_FAIL;
	}
	else
	{
		_uint iNumMaterials = m_pModelCom->Get_NumberOfMaterials();

		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Render(m_pShaderCom, 46, i);
		}
	}



	return S_OK;
}

void CSky::PostRender(_double TimeDelta)
{
	ImGui::Begin("SkyColor");

	_float fColor[4];
	memcpy(fColor, &m_vColor, sizeof(_float4));

	if (ImGui::DragFloat4("AddColor", fColor, 0.01f, -1.f, 1.f, "%.2f"))
		memcpy(&m_vColor, fColor, sizeof(_float4));

	ImGui::DragInt("SkyIndex", &m_iTexIndex, 1, 0, 10);

	ImGui::End();
}

HRESULT CSky::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (LEVEL_KEYGRAVE == GM->Get_CurrentLevel() || m_bEffectLevel == true)
	{
		/* For.Com_Shader */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;

		if (FAILED(__super::Add_Component(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_Skydome"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Noise"), TEXT("Com_Texture"), (CComponent**)&m_pNoiseTextureCom)))
			return E_FAIL;
	}
	else
	{
		/* For.Com_Shader */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Cube"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;

		/* For.Com_VIBuffer */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
			return E_FAIL;

		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}



	return S_OK;
}

HRESULT CSky::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;	
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (LEVEL_KEYGRAVE != GM->Get_CurrentLevel() && m_bEffectLevel == false)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", m_iTexIndex)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveNoise_X", &m_fNoiseX, sizeof(_float))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveNoise_Y", &m_fNoiseY, sizeof(_float))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_X", &m_fNoiseX, sizeof(_float))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_Y", &m_fNoiseY, sizeof(_float))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		if (FAILED(m_pNoiseTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourTexture", m_iTexIndex)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	return S_OK;
}

CSky* CSky::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSky* pInstance = new CSky(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CSky");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSky::Clone(void * pArg)
{
	CSky* pInstance = new CSky(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CSky");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSky::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pNoiseTextureCom);
	Safe_Release(m_pShaderCom);
}
