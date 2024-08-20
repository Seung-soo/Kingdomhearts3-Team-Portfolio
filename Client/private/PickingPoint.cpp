#include "stdafx.h"
#include "PickingPoint.h"
#include "GameInstance.h"

CPickingPoint::CPickingPoint(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CPickingPoint::CPickingPoint(const CPickingPoint& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPickingPoint::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPickingPoint::NativeConstruct(void* pArg)
{
	CTransform::TRANSFORMDESC TransformDesc;
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = 15.f;
	TransformDesc.fScalingPerSec = 5.f;
	if (FAILED(__super::NativeConstruct(&TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		_vector vPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		memcpy(&vPos, pArg, sizeof(_vector));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}

	m_pTransformCom->Scaled(XMVectorSet(0.3f, 0.3f, 0.3f, 0.f));
	m_pTransformCom->Set_Speed(5.f);

	return S_OK;
}

_int CPickingPoint::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	if (true == m_bDead)
		return RESULT_DEAD;

	return RESULT_NOPROBLEM;
}

_int CPickingPoint::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return	RESULT_ERROR;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	return RESULT_NOPROBLEM;
}

HRESULT CPickingPoint::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 2)))
		return E_FAIL;

	return S_OK;
}

void CPickingPoint::FollowMouse(_float fPosY)
{
	pair<_bool, _vector> CollisionCheck = CGameInstance::GetInstance()->Check_Ray_VirtualPlane_Collision(fPosY);

	if (TRUE == CollisionCheck.first)
	{
		// 피킹이 제대로 됐다면 그 위치로 이동
		_vector vFollowPosition = XMVectorSetY(CollisionCheck.second, fPosY);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vFollowPosition);
	}
}

HRESULT CPickingPoint::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Cube"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPickingPoint::SetUp_ConstantTable()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	 

	return S_OK;
}

CPickingPoint* CPickingPoint::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPickingPoint* pInstance = new CPickingPoint(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CPickingPoint");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPickingPoint::Clone(void* pArg)
{
	CPickingPoint* pInstance = new CPickingPoint(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CPickingPoint");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPickingPoint::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}