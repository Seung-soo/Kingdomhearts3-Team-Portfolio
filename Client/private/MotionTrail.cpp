#include "stdafx.h"
#include "..\public\MotionTrail.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "GameManager.h"

CMotionTrail::CMotionTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CMotionTrail::CMotionTrail(const CMotionTrail & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMotionTrail::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMotionTrail::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 15.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	
	memcpy(&m_tMotionTrailDesc, pArg, sizeof(MOTIONTRAILDESC));
	m_MeshDescVector = *m_tMotionTrailDesc.pMeshDescVector;

	if (FAILED(SetUp_Components()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_iPass = 16;

	return S_OK;
}

HRESULT CMotionTrail::NativeConstruct_Pool(void * pArg)
{
	return S_OK;
}

_int CMotionTrail::Tick(_double TimeDelta)
{
	if (true == m_bDead)
	{
		m_bDead = false;
		return RESULT_DEAD;	
	}
	if (0 > __super::Tick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}
	m_fLiveTime += (_float)TimeDelta * 6.f;
	if (1.f < m_fLiveTime)
	{
		m_fLiveTime = 1.f;
		m_bDead = true;
	}

	return _int();
}

_int CMotionTrail::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}
	if (m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_Position(), 1.5f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	return RESULT_NOPROBLEM;
}

HRESULT CMotionTrail::Render()
{
	if (FAILED(SetUp_ConstantTable()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	for (size_t i = 0; i < m_MeshDescVector.size(); i++)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_BoneMatrices", m_MeshDescVector[i]->pBoneMatrices, sizeof(_float4x4) * m_tMotionTrailDesc.iSize)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		m_MeshDescVector[i]->pMeshContainer->Render(m_pShaderCom, m_iPass);
	}

	return S_OK;
}

HRESULT CMotionTrail::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (280 >= m_tMotionTrailDesc.iSize)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimHeavyModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CMotionTrail::SetUp_ConstantTable()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_fRimPower", &m_tMotionTrailDesc.fRimPower, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_vRimColor", &m_tMotionTrailDesc.vColor, sizeof(_float4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}	

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fLiveTime, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	

	return S_OK;
}


CMotionTrail * CMotionTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMotionTrail*	pInstance = new CMotionTrail(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CMotionTrail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMotionTrail::Clone(void * pArg)
{
	CMotionTrail*	pInstance = new CMotionTrail(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMotionTrail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMotionTrail::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_MeshDescVector.size(); i++)
	{
		Safe_Delete_Array(m_MeshDescVector[i]->pBoneMatrices);
		Safe_Delete(m_MeshDescVector[i]);
	}
	m_MeshDescVector.clear();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
