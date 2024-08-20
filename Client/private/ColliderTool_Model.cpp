#include "stdafx.h"
#include "..\public\ColliderTool_Model.h"
#include "GameInstance.h"
#include "Collider.h"
#include "HierarchyNode.h"


CColliderTool_Model::CColliderTool_Model(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CColliderTool_Model::CColliderTool_Model(const CColliderTool_Model & rhs)
	: CGameObject(rhs)
{
}

CModel * CColliderTool_Model::Get_Model()
{
	return m_pModelCom;
}

HRESULT CColliderTool_Model::Set_Model(_tchar * pPrototypeTag)
{
	m_Components.erase(TEXT("Com_Model"));

	if (nullptr != m_pModelCom)
	{
		Safe_Release(m_pModelCom);
		Safe_Release(m_pModelCom);
	}

	if (FAILED(__super::Add_Component(LEVEL_COLLIDERTOOL, pPrototypeTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_pModelCom->Set_TempIndex(0);
	m_pModelCom->Add_OnceListAll();
	memcpy(m_szBoneName, m_pModelCom->Get_RootName(), MAX_PATH);

	return S_OK;
}

void CColliderTool_Model::Set_ColliderRadius(_float fRadius)
{
	m_fColliderRadius = fRadius;

	m_pColliderCom->Set_Scale(_float3(m_fColliderRadius, m_fColliderRadius, m_fColliderRadius));
}

HRESULT CColliderTool_Model::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CColliderTool_Model::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 3.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CColliderTool_Model::NativeConstruct");
		return E_FAIL;
	}

	m_tColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	m_tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	m_tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CMonster::NativeConstruct");
		return E_FAIL;
	}


	m_pModelCom->Set_TempIndex(0);
	m_pModelCom->Add_OnceListAll();
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

_int CColliderTool_Model::Tick(_double dTimeDelta)
{
	if (0 > __super::Tick(dTimeDelta))
	{
		MSGBOX("0 > __super::Tick in CColliderTool_Model::Tick");
		return -1;
	}

	CHierarchyNode* pNode = m_pModelCom->Find_HierarchyNode(m_szBoneName);

	_matrix			TransformMatrix = pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix())  * m_pTransformCom->Get_WorldMatrix();
	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	m_pColliderCom->Update_Transform(TransformMatrix);

	return _int();
}

_int CColliderTool_Model::LateTick(_double dTimeDelta)
{

	if (0 > __super::LateTick(dTimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CColliderTool_Model::LateTick");
		return -1;
	}

	m_pModelCom->Update_CombinedTransformationMatrices(dTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	return _int();
}




HRESULT CColliderTool_Model::Render()
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
	 
	for (_uint i = 0; i < iNumMaterials; i++)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render_Heavy(m_pShaderCom, 7, i, "g_BoneMatrices");
	}
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif
	return S_OK;
}

HRESULT CColliderTool_Model::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimHeavyModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sora"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Collder */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &m_tColliderDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CColliderTool_Model::SetUp_ConstantTable()
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

	return S_OK;
}

CColliderTool_Model * CColliderTool_Model::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CColliderTool_Model*	pInstance = new CColliderTool_Model(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CColliderTool_Model::Clone(void * pArg)
{
	CColliderTool_Model*	pInstance = new CColliderTool_Model(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CColliderTool_Model::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
