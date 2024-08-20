#include "stdafx.h"
#include "..\public\CellPoint.h"
#include "GameInstance.h"

CCellPoint::CCellPoint(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CCellPoint::CCellPoint(const CCellPoint & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCellPoint::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCellPoint::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pPoint = (_float3*)pArg;

	return S_OK;
}

_int CCellPoint::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;
	if (0 > __super::Tick(TimeDelta))
		return -1;
	_vector vPos = XMLoadFloat3(m_pPoint);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vPos, 1.f));

	m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	m_pGameInstance->Add_Collider(string("CellPoint"), m_pColliderCom);

	return _int();
}

_int CCellPoint::LateTick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	if (0 > __super::LateTick(TimeDelta))
		return -1;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	return _int();
}

HRESULT CCellPoint::Render()
{
	if (DISJOINT != m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.f))
	{
#ifdef _DEBUG
		m_pColliderCom->Render();
#endif
	}
	return S_OK;
}

void CCellPoint::IsPickCell(_bool bPick)
{
	if (true == bPick)
	{
		m_pTransformCom->Scaled(XMVectorSet(1.f, 1.f, 1.f, 0.f));
		m_pColliderCom->Set_IsCollision(true);
	}
	else
	{
		m_pTransformCom->Scaled(XMVectorSet(0.3f, 0.3f, 0.3f, 0.f));
		m_pColliderCom->Set_IsCollision(false);
	}
}

HRESULT CCellPoint::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(0.5f, 0.5f, 0.5f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tColliderDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	m_pColliderCom->Set_Parent(this);

	return S_OK;
}

CCellPoint * CCellPoint::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCellPoint*	pInstance = new CCellPoint(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCellPoint::Clone(void * pArg)
{
	CCellPoint*	pInstance = new CCellPoint(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCellPoint::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
}
