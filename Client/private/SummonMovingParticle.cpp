#include "stdafx.h"
#include "..\public\SummonMovingParticle.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "Effect_Particle.h"


CSummonMovingParticle::CSummonMovingParticle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CSummonMovingParticle::CSummonMovingParticle(const CSummonMovingParticle & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSummonMovingParticle::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSummonMovingParticle::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(360.0f);
	tTransformDesc.fScalingPerSec = 4.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	SUMPARTDESC tSummonParticleDesc;
	memcpy(&tSummonParticleDesc, pArg, sizeof(SUMPARTDESC));

	m_pTargetTransform = tSummonParticleDesc.pTargetTransform;

	m_pTransformCom->Set_WorldMatrix(XMMatrixTranslation(tSummonParticleDesc.vLocalPosition.x, tSummonParticleDesc.vLocalPosition.y, tSummonParticleDesc.vLocalPosition.z) * m_pTargetTransform->Get_WorldMatrix());


	// 파티클 생성
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase = false;
	tParticleDesc.bLoop = false;
	tParticleDesc.bTrail = true;
	tParticleDesc.bUseEasing = false;
	tParticleDesc.eDir = CEffect_Particle::DIR_ONE;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_10;
	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fLiveTime = 3.f;
	tParticleDesc.fMakeRange = 0.3f;
	tParticleDesc.fMaxDistance = 300.f;
	tParticleDesc.fMinSpeed = 0.f;
	tParticleDesc.fMaxSpeed = 0.f;
	tParticleDesc.fScaleSpeed = 2.f;
	tParticleDesc.fOriginScale = 0.5f;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.iLevelIndex = 0;
	tParticleDesc.iTexIndex = 2;
	tParticleDesc.pDead = &m_bDead;
	tParticleDesc.pParentPosition = m_pTransformCom->Get_PositionFloat4_Pointer();
	tParticleDesc.vDir = __vector(0.f, 1.f, 0.f, 0.f);

	_float	AddColor0[4] = { -0.43f, -0.35f, 1.f, 0.f };
	memcpy(tParticleDesc.fAddColor, AddColor0, sizeof(_float) * 4);
	_float	MulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, MulColor, sizeof(_float) * 4);

	if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float	AddColor1[4] = { 0.f, -0.41f, 0.f, 0.f };
	memcpy(tParticleDesc.fAddColor, AddColor1, sizeof(_float) * 4);
	 
	if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float	AddColor2[4] = { 0.f, -0.41f, 0.f, 0.f };
	memcpy(tParticleDesc.fAddColor, AddColor2, sizeof(_float) * 4);

	if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float	AddColor3[4] = { 0.f, 0.f, -0.2f, 0.f };
	memcpy(tParticleDesc.fAddColor, AddColor3, sizeof(_float) * 4);

	if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float	AddColor4[4] = { 0.f, -0.45f, -0.38f, 0.f };
	memcpy(tParticleDesc.fAddColor, AddColor4, sizeof(_float) * 4);

	if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float	AddColor5[4] = { -0.45f, 1.f, -0.38f, 0.f };
	memcpy(tParticleDesc.fAddColor, AddColor5, sizeof(_float) * 4);

	if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

_int CSummonMovingParticle::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	if (0 > __super::Tick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	m_pTransformCom->Go_VectorXZ(m_pTargetTransform->Get_Position() - m_pTransformCom->Get_Position(), TimeDelta * 0.5f);
	m_pTransformCom->Revolution(__vector(0.f, 1.f, 0.f, 0.f), m_pTargetTransform->Get_Position(), TimeDelta * 2.f);
	m_pTransformCom->Go_Up(TimeDelta * 1.5f);
	m_pTransformCom->Get_PositionFloat4_Pointer();
	if (vecy(m_pTransformCom->Get_Position()) > vecy(m_pTargetTransform->Get_Position()) + 2.5f)
		m_bDead = true;


	return _int();
}

_int CSummonMovingParticle::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}


	return RESULT_NOPROBLEM;
}

HRESULT CSummonMovingParticle::Render()
{

	return S_OK;
}

CSummonMovingParticle * CSummonMovingParticle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSummonMovingParticle*	pInstance = new CSummonMovingParticle(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CSummonMovingParticle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSummonMovingParticle::Clone(void * pArg)
{
	CSummonMovingParticle*	pInstance = new CSummonMovingParticle(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CSummonMovingParticle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSummonMovingParticle::Free()
{
	__super::Free();

}
