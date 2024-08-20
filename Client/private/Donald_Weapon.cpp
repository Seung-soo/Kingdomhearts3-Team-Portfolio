#include "stdafx.h"
#include "..\public\Donald_Weapon.h"
#include "GameInstance.h"
#include "HierarchyNode.h"

CDonald_Weapon::CDonald_Weapon(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CDonald_Weapon::CDonald_Weapon(const CDonald_Weapon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDonald_Weapon::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDonald_Weapon::NativeConstruct(void * pArg)
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

	memcpy(&m_tWeaponDesc, pArg, sizeof(WEAPON_DESC));
	m_pNode = m_tWeaponDesc.pModel->Find_HierarchyNode(m_tWeaponDesc.pSocketName);

	//_matrix WorldMatrix = XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(180.f)) * m_pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_tWeaponDesc.pModel->Get_TransformMatrix()) * m_tWeaponDesc.pTransform->Get_WorldMatrix();

	//m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	return S_OK;
}

_int CDonald_Weapon::Tick(_double TimeDelta)
{
	if (true == m_tWeaponDesc.pParent->Get_Dead())
	{
		return RESULT_DEAD;
	}
	if (0 > __super::Tick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	_matrix WorldMatrix = XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(180.f)) * m_pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_tWeaponDesc.pModel->Get_TransformMatrix()) * m_tWeaponDesc.pTransform->Get_WorldMatrix();

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);


	return _int();
}

_int CDonald_Weapon::LateTick(_double TimeDelta)
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

HRESULT CDonald_Weapon::Render()
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

void CDonald_Weapon::PostRender(_double TimeDelta)
{
}

HRESULT CDonald_Weapon::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_DonaldWeapon"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDonald_Weapon::SetUp_ConstantTable()
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

CDonald_Weapon * CDonald_Weapon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CDonald_Weapon*	pInstance = new CDonald_Weapon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CDonald_Weapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDonald_Weapon::Clone(void * pArg)
{
	CDonald_Weapon*	pInstance = new CDonald_Weapon(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CDonald_Weapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDonald_Weapon::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
