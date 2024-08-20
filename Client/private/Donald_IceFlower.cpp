#include "stdafx.h"
#include "..\public\Donald_IceFlower.h"
#include "GameInstance.h"
#include "Easing_Function.h"
#include "Effect_Particle.h"
#include "GameManager.h"
#include "Donald.h"
#include "Monster.h"
#include "Effect_Manager.h"
#include "Player.h"

CDonald_IceFlower::CDonald_IceFlower(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CDonald_IceFlower::CDonald_IceFlower(const CDonald_IceFlower & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDonald_IceFlower::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDonald_IceFlower::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 10.f;
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

	memcpy(&m_tIceFlowerDesc, pArg, sizeof(ICEFLOWERDESC));

	m_pTransformCom->Set_WorldMatrix(m_tIceFlowerDesc.WorldMatrix);

	m_tIceFlowerDesc.WorldMatrix.m[3][1] += 0.5f;
	memcpy(&m_vPosition, &m_tIceFlowerDesc.WorldMatrix.m[3], sizeof(_float4));

	m_fScale = 0.01f;
	m_pTransformCom->Scaled(__vector(m_fScale, m_fScale, m_fScale, 0.f));


	// 파티클 생성
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase = false;
	tParticleDesc.bLoop = false;
	tParticleDesc.bTrail = false;
	tParticleDesc.bUseEasing = true;
	tParticleDesc.eEasing = CEffect_Particle::OUTCIRC;
	tParticleDesc.eDir = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_100;

	_float	fAddColor[4] = { -1.f, -0.18f, -0.56f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 0.03f, 0.63f, 5.47f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fLiveTime = 0.5f;
	tParticleDesc.fMakeRange = 0.5f;
	tParticleDesc.fMaxDistance = 300.f;
	tParticleDesc.fMinSpeed = 60.f;
	tParticleDesc.fMaxSpeed = 100.f;
	tParticleDesc.fScaleSpeed = 1.f;
	tParticleDesc.fOriginScale = 0.5f;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.iLevelIndex = 0;
	tParticleDesc.iTexIndex = 2;
	tParticleDesc.pDead = &m_bDead;
	tParticleDesc.pParentPosition = &m_vPosition;
	tParticleDesc.vDir = __vector(0.f, 1.f, 0.f, 0.f);

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

	m_pDonald = static_cast<CDonald*>(m_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Donald")));

	
	m_pGameInstance->LetsPlaySound_Monster(L"Donald_Skill2_21.wav", 0.5f);
	return S_OK;
}

_int CDonald_IceFlower::Tick(_double TimeDelta)
{
	if (true == *m_tIceFlowerDesc.pDead || true == m_bDead)
	{
		auto AttackMonsters = m_pDonald->Get_AttackMonsterContainer();

		for (_uint i = 0; i < AttackMonsters->size(); i++)
		{
			(*AttackMonsters)[i]->Set_Slow(1.0);
			(*AttackMonsters)[i]->Set_Stun(0.0);
			(*AttackMonsters)[i]->Damaged(m_pColliderCom, 10.f);
			if (true == static_cast<CMonster*>((*AttackMonsters)[i])->Get_ReadyToDie())
				GM->Get_Player()->Eliminate_DeadTarget((*AttackMonsters)[i]);

			CEffect_Manager::GetInstance()->Create_HitStarDonald((*AttackMonsters)[i]->Get_Transform(), 0.7f);

		}

		return RESULT_DEAD;
	}

	if (0 > __super::Tick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	m_fScale = EaseOutExpo(4.f, m_fLerpTime, 0.5f) + 0.01f;

	m_pTransformCom->Scaled(__vector(m_fScale, m_fScale, m_fScale, 0.f));

	if (0.5f > m_fLerpTime)
		m_fLerpTime += (_float)TimeDelta;

	if (0.5f < m_fLerpTime)
	{
		m_bCollision = false;
		m_fLerpTime = 0.5f;
	}

	m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	return _int();
}

_int CDonald_IceFlower::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	if (true == m_bCollision)
	{
		//	몬스터 콜라이더 리스트
		auto pDestColliderList = m_pGameInstance->Get_ColliderList(string("Monster_Hit"));
		//	한번 충돌한 몬스터 벡터 컨테이너
		auto pAttackMonsterContainer = m_pDonald->Get_AttackMonsterContainer();

		//	몬스터 콜라이더 리스트를 순회
		for (auto& pDestCollider : *pDestColliderList)
		{
			//	충돌 검사
			if (true == m_pGameInstance->Collision_Enter_Sphere(m_pColliderCom, pDestCollider))
			{

				CGameObject*	pDestParent = pDestCollider->Get_Parent();
				_bool			bInSameMonster = false;
				//	이미 같은 몬스터를 얼렸는지 검사
				for (size_t i = 0; i < pAttackMonsterContainer->size(); i++)
				{
					if ((*pAttackMonsterContainer)[i] == pDestParent)
					{
						bInSameMonster = true;
						break;
					}
				}
				//	같은 몬스터가 없다면 충돌한 몬스터 벡터 컨테이너에 집어 넣어준다.
				if (false == bInSameMonster)
				{
					// 슬로우
					static_cast<CMonster*>(pDestParent)->Set_Slow(0.1);
					static_cast<CMonster*>(pDestParent)->Damaged(m_pColliderCom, 10.f);
					if (true == static_cast<CMonster*>(pDestParent)->Get_ReadyToDie())
						GM->Get_Player()->Eliminate_DeadTarget(pDestParent);
					CEffect_Manager::GetInstance()->Create_HitStarDonald(pDestParent->Get_Transform(), 0.7f);
					m_pDonald->Add_AttackMonster(static_cast<CMonster*>(pDestParent));
				}
			}
		}
	}

	if (m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_Position(), 10.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
#ifdef _DEBUG
		m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
	}

	return RESULT_NOPROBLEM;
}

HRESULT CDonald_IceFlower::Render()
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

void CDonald_IceFlower::PostRender(_double TimeDelta)
{
}

HRESULT CDonald_IceFlower::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_DonaldFlower"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(3.5f, 3.5f, 3.5f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tColliderDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDonald_IceFlower::SetUp_ConstantTable()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_bool bBlur = 1;
	if (FAILED(m_pShaderCom->Set_RawValue("g_iBlur", &bBlur, sizeof(_bool))))
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

CDonald_IceFlower * CDonald_IceFlower::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CDonald_IceFlower*	pInstance = new CDonald_IceFlower(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CDonald_IceFlower");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDonald_IceFlower::Clone(void * pArg)
{
	CDonald_IceFlower*	pInstance = new CDonald_IceFlower(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CDonald_IceFlower");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDonald_IceFlower::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
