#include "stdafx.h"
#include "..\public\Donald_Icicle.h"
#include "GameInstance.h"
#include "Easing_Function.h"
#include "Effect_Particle.h"
#include "GameManager.h"
#include "Monster.h"
#include "Effect_Texture.h"
#include "Effect_Manager.h"
#include "Player.h"

CDonald_Icicle::CDonald_Icicle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CDonald_Icicle::CDonald_Icicle(const CDonald_Icicle & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDonald_Icicle::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDonald_Icicle::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec    = 10.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec  = 5.f;

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

	memcpy(&m_tIcicleDesc, pArg, sizeof(ICICLEDESC));

	// 타겟의 위치
	m_vTargetPosition = XMVectorSetW(XMLoadFloat3(&m_tIcicleDesc.pTargetCollider->Get_Sphere()->Center), 1.f);
	m_fDist = XMVectorGetX(XMVector4Length(m_vTargetPosition - m_tIcicleDesc.vPosition));

	m_pTransformCom->Set_Position(m_tIcicleDesc.vPosition);



	// 파티클 생성
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase          = false;
	tParticleDesc.bLoop           = false;
	tParticleDesc.bTrail          = true;
	tParticleDesc.bUseEasing      = false;
	tParticleDesc.eDir            = CEffect_Particle::DIR_ONE;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_30;

	_float	fAddColor[4] = { -1.f, -0.18f, -0.56f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 0.03f, 0.63f, 5.47f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower   = 0.f;
	tParticleDesc.fLiveTime       = 300.f;
	tParticleDesc.fMakeRange      = 0.5f;
	tParticleDesc.fMaxDistance    = 300.f;
	tParticleDesc.fMinSpeed       = 0.1f;
	tParticleDesc.fMaxSpeed       = 0.3f;
	tParticleDesc.fScaleSpeed     = 1.f;
	tParticleDesc.fOriginScale    = 0.5f;
	tParticleDesc.fTrailInterval  = 0.f;
	tParticleDesc.iLevelIndex     = 0;
	tParticleDesc.iTexIndex       = 2;
	tParticleDesc.pDead           = &m_bDead;
	tParticleDesc.pParentPosition = &m_vPosition;
	tParticleDesc.vDir            = __vector(0.f, -1.f, 0.f, 0.f);

	if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}



	_float	AddColor[4] = { -0.43f, -0.35f, 1.f, 0.f };
	memcpy(tParticleDesc.fAddColor, AddColor, sizeof(_float) * 4);

	_float	MulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, MulColor, sizeof(_float) * 4);

	if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_iPass = 33;

	return S_OK;
}

_int CDonald_Icicle::Tick(_double TimeDelta)
{
	// 타겟이 사라지면 사라짐
	if (true == m_tIcicleDesc.pTargetCollider->Get_Parent()->Get_Dead() || true == m_bDead)
	{
		return RESULT_DEAD;
	}

	if (0 > __super::Tick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	// 타겟의 위치
	m_vTargetPosition = XMVectorSetW(XMLoadFloat3(&m_tIcicleDesc.pTargetCollider->Get_Sphere()->Center), 1.f);
	m_fDist = XMVectorGetX(XMVector4Length(m_vTargetPosition - m_tIcicleDesc.vPosition));

	_vector vRight = CROSS3(XMVector3Normalize(m_vTargetPosition - m_tIcicleDesc.vPosition)
		, __vector(0.f, 1.f, 0.f, 0.f));

	// 베지에 곡선을 활용할 점
	m_vCenterPosition  = (m_tIcicleDesc.vPosition * 0.7f + m_vTargetPosition * 0.3f);
	m_vCenterPosition  = XMVectorSetY(m_vCenterPosition, XMVectorGetY(m_vCenterPosition) + m_fDist * 0.4f);
	m_vCenterPosition += vRight * m_fDist * 0.4f;

	m_fLerpTime += (_float)TimeDelta / m_fDist * 30.f;

	_float fRealLerpTime = EaseInQuad(1.f, m_fLerpTime, 1.f);

	// 베지에 곡선
	_vector vPos = XMVectorLerp(XMVectorLerp(m_tIcicleDesc.vPosition, m_vCenterPosition, (_float)fRealLerpTime)
		, XMVectorLerp(m_vCenterPosition, m_vTargetPosition, (_float)fRealLerpTime), (_float)fRealLerpTime);

	// 진행 방향으로 룩을 설정
	_vector	vLook = XMVector3Normalize(vPos - m_pTransformCom->Get_Position());

	// 회전과 포지션 적용
	m_pTransformCom->MakeSameDirection(vLook);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_vPosition = XMStoreFloat4RT(vPos);


	// 지나갔을 경우 다시 베지에 곡선
	if (1.05f < m_fLerpTime)
	{
		m_fLerpTime             = 0.f;
		m_tIcicleDesc.vPosition = m_pTransformCom->Get_Position();
		m_fDist                 = XMVectorGetX(XMVector4Length(m_vTargetPosition - m_tIcicleDesc.vPosition));
	}

	m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());
#ifdef _DEBUG
	m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
	return _int();
}

_int CDonald_Icicle::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	if (true == m_pGameInstance->Collision_Sphere(m_pColliderCom, m_tIcicleDesc.pTargetCollider))
	{
		// 데미지 + 터지는 이펙트
		static_cast<CMonster*>(m_tIcicleDesc.pTargetCollider->Get_Parent())->Damaged(m_pColliderCom, 10.f);
		if (true == static_cast<CMonster*>(m_tIcicleDesc.pTargetCollider->Get_Parent())->Get_ReadyToDie())
			GM->Get_Player()->Eliminate_DeadTarget(m_tIcicleDesc.pTargetCollider->Get_Parent());
		CEffect_Manager::GetInstance()->Create_HitStarDonald(m_tIcicleDesc.pTargetCollider->Get_Parent()->Get_Transform(), 0.7f);
		m_bDead = true;
	}

	if (m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_Position(), 1.5f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	return RESULT_NOPROBLEM;
}

HRESULT CDonald_Icicle::Render()
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
		m_pModelCom->Render(m_pShaderCom, m_iPass, i);
	}

	return S_OK;
}

void CDonald_Icicle::PostRender(_double TimeDelta)
{
}

HRESULT CDonald_Icicle::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_DonaldIcicle"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Collider */
	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale       = _float3(0.5f, 0.5f, 0.5f);
	tColliderDesc.vRotation    = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.3f, 1.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tColliderDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDonald_Icicle::SetUp_ConstantTable()
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


	_float4 vColor = _float4(0.7f, 0.8f, 1.f, 1.f);
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &vColor, sizeof(_float4))))
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

	_bool  bDistortion= 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_bDistortion", &bDistortion, sizeof(_bool))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_bool bBlur = true;
	if (FAILED(m_pShaderCom->Set_RawValue("g_iBlur", &bBlur, sizeof(_bool))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

CDonald_Icicle * CDonald_Icicle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CDonald_Icicle*	pInstance = new CDonald_Icicle(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CDonald_Icicle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDonald_Icicle::Clone(void * pArg)
{
	CDonald_Icicle*	pInstance = new CDonald_Icicle(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CDonald_Icicle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDonald_Icicle::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
