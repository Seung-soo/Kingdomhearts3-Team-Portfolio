#include "stdafx.h"
#include "..\public\DropFruitZone.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Level_Loading.h"
#include "GameManager.h"
#include "Effect_Manager.h"

CDropFruitZone::CDropFruitZone(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CDropFruitZone::CDropFruitZone(const CDropFruitZone & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDropFruitZone::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDropFruitZone::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec    = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec  = 4.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	
	_vector vPosition;
	memcpy(&vPosition, pArg, sizeof(_vector));
	m_pTransformCom->Set_Position(vPosition);

	m_pTransformCom->Rotation(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	m_pTransformCom->Scaled(__vector(1.f, 1.f, 1.f, 0.f));

	return S_OK;
}

_int CDropFruitZone::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	if (m_TimeAcc < 1.0)
	{
		m_pTransformCom->Scaling(TimeDelta);

		if (false == m_bPlayParticle)
		{
			m_PtclTimeAcc += TimeDelta;

			if (m_PtclTimeAcc >= 0.9)
			{
				CEffect_Manager::GetInstance()->Create_Leaves(m_pTransformCom->Get_Position(), __vector(0.f, 0.f, 0.f, 0.f));

				m_PtclTimeAcc = 0;
				m_bPlayParticle = true;

			}
		}
	
	}

	else if (m_TimeAcc >= 2.0)
	{ 
		CTransform::TRANSFORMDESC TransformDesc;
		TransformDesc.fSpeedPerSec    = 20.f;
		TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
		TransformDesc.fScalingPerSec  = -2.f;

		m_pTransformCom->Set_TransformDesc(TransformDesc);
		m_pTransformCom->Scaling(TimeDelta);
	}

	m_TimeAcc += TimeDelta;

	if (m_TimeAcc >= 4.5f)
		return RESULT_DEAD;

	m_fRotation += (_float)TimeDelta * 90.f;

	return _int();
}

_int CDropFruitZone::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	return RESULT_NOPROBLEM;
}

HRESULT CDropFruitZone::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 18)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDropFruitZone::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
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

	return S_OK;
}

HRESULT CDropFruitZone::SetUp_ConstantTable()
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
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 339)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pNoiseTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourTexture", 0)))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	
	_bool bBlur = true;
	_float fDistortion = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_iBlur", &bBlur, sizeof(_bool))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fDistortion", &fDistortion, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float fRot = XMConvertToRadians(m_fRotation);
	if (FAILED(m_pShaderCom->Set_RawValue("g_fNoiseRot", &fRot, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	
	return S_OK;
}

CDropFruitZone * CDropFruitZone::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CDropFruitZone*	pInstance = new CDropFruitZone(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CDropFruitZone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDropFruitZone::Clone(void * pArg)
{
	CDropFruitZone*	pInstance = new CDropFruitZone(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CDropFruitZone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDropFruitZone::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pNoiseTextureCom);
	Safe_Release(m_pShaderCom);
}
