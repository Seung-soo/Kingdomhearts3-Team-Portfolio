#include "stdafx.h"
#include "..\public\Grid.h"
#include "GameInstance.h"
#include "HierarchyNode.h"

CGrid::CGrid(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CGrid::CGrid(const CGrid & rhs)
	: CGameObject(rhs)
{
}

HRESULT CGrid::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGrid::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		BREAKPOINT;
		return E_FAIL;
	}



	return S_OK;
}

_int CGrid::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}


	return _int();
}

_int CGrid::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}
	if (m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_Position(), 1.5f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	return RESULT_NOPROBLEM;
}

HRESULT CGrid::Render()
{
	if (nullptr == m_pModelCom)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumberOfMaterials();

	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, m_iPass, i);
	}

	return S_OK;
}

void CGrid::PostRender(_double TimeDelta)
{
}

HRESULT CGrid::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_FROZEN, TEXT("Prototype_Component_Model_Grid"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGrid::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float fTransparency = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fTransparency", &fTransparency, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

CGrid * CGrid::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CGrid*	pInstance = new CGrid(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CGrid");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGrid::Clone(void * pArg)
{
	CGrid*	pInstance = new CGrid(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CGrid");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGrid::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
