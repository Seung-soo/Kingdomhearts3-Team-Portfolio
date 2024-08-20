#include "stdafx.h"
#include "..\public\Nightmare_Appear.h"
#include "GameInstance.h"
#include "Easing_Function.h"
#include "Effect_Particle.h"
#include "GameManager.h"
#include "Donald.h"
#include "Monster.h"
#include "Camera_Perspective.h"

CNightmare_Appear::CNightmare_Appear(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CNightmare_Appear::CNightmare_Appear(const CNightmare_Appear & rhs)
	: CGameObject(rhs)
{
}

HRESULT CNightmare_Appear::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNightmare_Appear::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 35.f;
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

	CTransform*	pNightmareTransform = (CTransform*)pArg;

	m_pTransformCom->Set_WorldMatrix(pNightmareTransform->Get_WorldFloat4x4());

	//m_pTransformCom->Set_MoveBasedOnCurrentUp(1.3f);
	m_pTransformCom->Set_MoveBasedOnCurrentLook(2.f);

	m_pTransformCom->Scaled(__vector(1.f, 2.f, 1.f, 0.f));

	if (GM->Get_CurrentLevel() != LEVEL_TEST)
		GM->Get_Camera()->Set_HorizonShake(0.2f, 0.f, 1.f);
	m_iPass = 27;

	return S_OK;
}

_int CNightmare_Appear::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	if (0 > __super::Tick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}
	m_fLifeTime -= (_float)TimeDelta;

	if (0 > m_fLifeTime)
		m_bDead = true;

	m_pTransformCom->Go_Straight(TimeDelta);


	return _int();
}

_int CNightmare_Appear::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);


	return RESULT_NOPROBLEM;
}

HRESULT CNightmare_Appear::Render()
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

void CNightmare_Appear::PostRender(_double TimeDelta)
{
}

HRESULT CNightmare_Appear::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Slash04"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CNightmare_Appear::SetUp_ConstantTable()
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

	_bool	bDistortion = 1.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_bDistortion", &bDistortion, sizeof(_bool))))
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

	_float4 vColor = _float4(0.f, 0.f, 0.f, 0.f);
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &vColor, sizeof(_float4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	
	return S_OK;
}

CNightmare_Appear * CNightmare_Appear::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CNightmare_Appear*	pInstance = new CNightmare_Appear(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CNightmare_Appear");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CNightmare_Appear::Clone(void * pArg)
{
	CNightmare_Appear*	pInstance = new CNightmare_Appear(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CNightmare_Appear");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNightmare_Appear::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
