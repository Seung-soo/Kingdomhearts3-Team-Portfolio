#include "stdafx.h"
#include "..\public\Goofy_Weapon.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Goofy.h"
#include "Effect_Mesh.h"
#include "GameManager.h"
#include "Effect_Particle.h"
#include "Monster.h"
#include "Effect_Manager.h"
#include "Player.h"

#define	Weapon_Speed 3.f

CGoofy_Weapon::CGoofy_Weapon(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CGoofy_Weapon::CGoofy_Weapon(const CGoofy_Weapon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CGoofy_Weapon::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGoofy_Weapon::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(180.0f);
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

	m_pGoofy = static_cast<CGoofy*>(m_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Goofy")));

	m_ColliderList = *(m_pGameInstance->Get_ColliderList(string("Monster_Hit")));

	return S_OK;
}

_int CGoofy_Weapon::Tick(_double TimeDelta)
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

	if (MODE_RUN == m_tWeaponDesc.eMode)
	{
		if (true == m_bScaleUp)
		{
			m_fScale += (_float)TimeDelta * 40.f;

			if (20.f < m_fScale)
			{
				m_fScale = 20.f;
				m_pColliderCom->Set_Scale(_float3(0.6f));
			}

			m_iPass = 20;
		}

		if (true == m_bScaleDown)
		{
			m_fScale -= (_float)TimeDelta * 40.f;
			m_bRun = false;
			if (1.f > m_fScale)
			{
				m_fScale = 1.f;
				m_iPass = 0;
				m_pColliderCom->Set_Scale(_float3(1.f));
			}
		}

		m_pTransformCom->Scaled(__vector(m_fScale, m_fScale, m_fScale, 0.f));

		if (FAILED(Run_Effect()))
		{
			BREAKPOINT;
			return RESULT_ERROR;
		}
	}

	m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	return _int();
}

_int CGoofy_Weapon::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	if (false == m_bFirstLateTick)
	{
		m_ColliderList = *(m_pGameInstance->Get_ColliderList(string("Monster_Hit")));

		m_bFirstLateTick = true;
	}

	// 구피 돌진 충돌 함수
	Run_Attack();

	if (true == m_bComeBack)
	{
		if (false == m_bComeBackStart)
		{
			m_vLerpStart = m_pTransformCom->Get_Position();
			m_bComeBackStart = true;
		}
	}

	// 던지는 상태가 아닐 때
	if (false == m_bThrow)
	{
		// 무기를 팔에 붙혀줌
		_matrix WorldMatrix = XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(180.f)) * m_pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_tWeaponDesc.pModel->Get_TransformMatrix()) * m_tWeaponDesc.pTransform->Get_WorldMatrix();

		_vector vScale = m_pTransformCom->Get_AllScale();
		m_pTransformCom->Set_WorldMatrix(WorldMatrix);
		m_pTransformCom->Scaled(vScale);

		if (true == m_bTurn)
		{
			// 콜라이더 리스트 받기
			//auto ColliderList = m_pGameInstance->Get_ColliderList(string("Monster_Hit"));
			if (0 < m_ColliderList.size())
			{
				CCollider* pTarget = m_ColliderList.front();

				if (nullptr != pTarget)
				{
					_vector vTargetPos = XMLoadFloat3(&pTarget->Get_Sphere()->Center);

					static_cast<CTransform*>(m_tWeaponDesc.pParent->Get_Component(TEXT("Com_Transform")))->Turn_To_Direction(vTargetPos - static_cast<CTransform*>(m_tWeaponDesc.pParent->Get_Component(TEXT("Com_Transform")))->Get_Position(), TimeDelta * Weapon_Speed);
				}
			}
		}
	}
	// 던지는 상태일 때
	else
	{
		// 회수 상태가 아니고 타겟 설정이 안되어있을 때
		if (false == m_bComeBack && nullptr == m_pTargetCollider)
		{
			auto iter = m_ColliderList.begin();

			// 더 맞출 적이 없다면 구피가 던지는 상태를 해제하고 회수함
			if (m_iColliderIndex >= (_uint)m_ColliderList.size())
			{
				m_bComeBack = true;
				static_cast<CGoofy*>(m_tWeaponDesc.pParent)->Set_ThrowEnd(true);

				m_bEffect = false;
			}
			// 있으면 다시 타겟 설정
			else
			{
				for (_uint i = 0; i < (_uint)m_iColliderIndex; i++)
					++iter;

				m_pTargetCollider = *iter;
			}

		}
		// 회수 상태일 때
		else if (true == m_bComeBack)
		{
			// 구피 뼈와 보간하면서 복귀
			_matrix WorldMatrix = XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f)
				, XMConvertToRadians(180.f)) * m_pNode->Get_CombinedTransformationMatrix() 
				* XMLoadFloat4x4(&m_tWeaponDesc.pModel->Get_TransformMatrix()) 
				* m_tWeaponDesc.pTransform->Get_WorldMatrix();
			_vector vGoofyHand = WorldMatrix.r[3];

			_vector vPos = XMVectorLerp(m_vLerpStart, vGoofyHand, m_fComeBackLerp);
			m_pTransformCom->Set_Position(vPos);
			if (1.f <= m_fComeBackLerp)
				m_bThrow = false;
		}
		// 회수 상태가 아니고 타겟 설정이 되어있을 때
		else
		{
			if (false == m_bPaticle)
			{
				if (FAILED(Throw_Particle()))
				{
					BREAKPOINT;
					return RESULT_ERROR;
				}

				m_bPaticle = true;
			}
			// 타겟을 향해서 날아간다.
			_vector vTargetPos = XMLoadFloat3(&m_pTargetCollider->Get_Sphere()->Center);
			vecw(vTargetPos) = 1.f;
			m_pTransformCom->Chase(vTargetPos, TimeDelta * Weapon_Speed);

			m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());
#ifdef _DEBUG
			m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
			// 충돌 검사 후 충돌했으면 타겟을 비워주고 다음 콜라이더 순번으로 이동
			if (true == m_pGameInstance->Collision_Enter_Sphere(m_pColliderCom, m_pTargetCollider))
			{
				static_cast<CMonster*>(m_pTargetCollider->Get_Parent())->Damaged(m_pColliderCom, 20.f);
				if (true == static_cast<CMonster*>(m_pTargetCollider->Get_Parent())->Get_ReadyToDie())
					GM->Get_Player()->Eliminate_DeadTarget(m_pTargetCollider->Get_Parent());

				_vector vCenter = (m_pColliderCom->Get_Parent()->Get_Transform()->Get_Position() 
					+ m_pTargetCollider->Get_Parent()->Get_Transform()->Get_Position()) * 0.5f;
				_vector vLook = XMVector3Normalize(m_pTargetCollider->Get_Parent()->Get_Transform()
					->Get_Position() - m_pColliderCom->Get_Parent()->Get_Transform()->Get_Position());
				CEffect_Manager::GetInstance()->Create_GoofyHitEffect(vCenter, vLook);
				CEffect_Manager::GetInstance()->Create_HitStarGoofy(m_pTargetCollider->Get_Parent()->Get_Transform(), 0.7f);

				m_pTargetCollider = nullptr;
				++m_iColliderIndex;
			}

			static_cast<CTransform*>(m_tWeaponDesc.pParent->Get_Component(TEXT("Com_Transform")))->Turn_To_Direction(vTargetPos - static_cast<CTransform*>(m_tWeaponDesc.pParent->Get_Component(TEXT("Com_Transform")))->Get_Position(), TimeDelta * Weapon_Speed);
		}

		if (FAILED(Throw_Effect()))
		{
			BREAKPOINT;
			return RESULT_ERROR;
		}
	}

	if (m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_Position(), 1.5f))
	{
		if (0 == m_iPass)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		else
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
#ifdef _DEBUG
		m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
	}

	return RESULT_NOPROBLEM;
}

HRESULT CGoofy_Weapon::Render()
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

void CGoofy_Weapon::PostRender(_double TimeDelta)
{
}

HRESULT CGoofy_Weapon::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_GoofyWeapon"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tColliderDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGoofy_Weapon::SetUp_ConstantTable()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_bool bBlur = false;

	if (0 != m_iPass)
		bBlur = true;

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

HRESULT CGoofy_Weapon::Run_Effect()
{
	m_vPosition = XMStoreFloat4RT(m_pTransformCom->Get_Position());

	if (true == m_bEffect && false == m_bEffectStart)
	{
		CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;
		ZeroMemory(&tEffectMeshDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

		tEffectMeshDesc.bBillboarding = false;
		lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Tornado12"));
		tEffectMeshDesc.iTextureIndex = 16;
		tEffectMeshDesc.iScondTextureIndex = 0;
		tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
		tEffectMeshDesc.fLifeTime = 100.f;
		tEffectMeshDesc.pParentPos = &m_vPosition;
		tEffectMeshDesc.iPassIndex = 0;
		tEffectMeshDesc.fTransparency = 0.f;
		tEffectMeshDesc.fTransparecySpeed = 0.f;
		tEffectMeshDesc.fMaxTransparecy = 0.f;
		tEffectMeshDesc.bSpinning = false;
		tEffectMeshDesc.fSpinningSpeed = 0.f;
		tEffectMeshDesc.vSpinAxis = __vector(0.f, 0.f, 0.f, 0.f);
		tEffectMeshDesc.bBillboarding = false;
		tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
		tEffectMeshDesc.bUseEasing_Scaling = false;
		tEffectMeshDesc.vStartScale = __vector(0.01f, 0.01f, 0.01f, 0.f);
		tEffectMeshDesc.vMaxScale = __vector(1.f, 1.f, 1.f, 0.f);
		tEffectMeshDesc.fScaleSpeed = 0.f;
		tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
		tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;
		tEffectMeshDesc.bMovingUV = true;
		tEffectMeshDesc.vUVMovingSpeed = _float2(0.f, 6.f);
		tEffectMeshDesc.bUsingEmissive = false;
		tEffectMeshDesc.bUsingBlur = true;
		tEffectMeshDesc.bUsingDistortion = false;
		tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
		tEffectMeshDesc.vColor = _float4(1.f, 1.f, -0.5f, 0.f);
		tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);

		if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		m_pEffect[0] = static_cast<CEffect_Mesh*>(m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Effect")));

		m_bEffectStart = true;
	}

	if (nullptr != m_pEffect[0])
	{
		_matrix	WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		WorldMatrix.r[0] = XMVector3Normalize(WorldMatrix.r[0]) * 0.6f;
		WorldMatrix.r[1] = XMVector3Normalize(WorldMatrix.r[1]) * 0.6f;
		WorldMatrix.r[2] = XMVector3Normalize(WorldMatrix.r[2]) * 0.6f;

		CTransform*	pEffectTransform = static_cast<CTransform*>(m_pEffect[0]->Get_Component(TEXT("Com_Transform")));
		pEffectTransform->Set_WorldMatrix(WorldMatrix);
		pEffectTransform->Set_MoveBasedOnCurrentLook(-8.f);

		if (true == m_pEffect[0]->Get_Dead())
			m_pEffect[0] = nullptr;
	}

	if (false == m_bEffect && true == m_bEffectStart)
	{
		m_pEffect[0]->Set_MeshLifeType(CEffect_Mesh::LOOP_LIMIT);
		m_pEffect[0]->Set_PlayType(CEffect_Mesh::PLAY_ONCE);

		m_bEffectStart = false;
	}


	return S_OK;
}

void CGoofy_Weapon::Run_Attack()
{
	if (true == m_bRun)
	{
		//	몬스터 콜라이더 리스트
		auto pDestColliderList = m_pGameInstance->Get_ColliderList(string("Monster_Hit"));
		//	한번 충돌한 몬스터 벡터 컨테이너
		auto pAttackMonsterContainer = m_pGoofy->Get_AttackMonsterContainer();

		_vector vMyPosition = XMVectorSetY(m_pTransformCom->Get_Position(), 0.f);
		_vector	vGoofyLook = m_pGoofy->Get_Transform()->Get_NLook();

		for (auto& pDestCollider : *pDestColliderList)
		{
			_vector vMonsterPosition = XMVectorSetY(pDestCollider->Get_Parent()
				->Get_Transform()->Get_Position(), 0.f);
			_vector	vDir = XMVector3Normalize(vMonsterPosition - vMyPosition);
			_float	fDot = DOT3(vDir, vGoofyLook);

			if (0.f > fDot)
				continue;

			//	충돌 검사
			if (true == m_pGameInstance->Collision_Enter_Sphere(m_pColliderCom, pDestCollider))
			{

				CGameObject*	pDestParent = pDestCollider->Get_Parent();
				_bool			bInSameMonster = false;
				//	이미 같은 몬스터와 충돌했는지 검사
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
					// 데미지주고 넉백시키기
					static_cast<CMonster*>(pDestParent)->Damaged(m_pColliderCom, 40.f);
					if (true == static_cast<CMonster*>(pDestParent)->Get_ReadyToDie())
						GM->Get_Player()->Eliminate_DeadTarget(pDestParent);
					_vector vCenter = (m_pColliderCom->Get_Parent()->Get_Transform()->Get_Position() + pDestCollider->Get_Parent()->Get_Transform()->Get_Position()) * 0.5f;
					_vector vLook = XMVector3Normalize(pDestCollider->Get_Parent()->Get_Transform()->Get_Position() - m_pColliderCom->Get_Parent()->Get_Transform()->Get_Position());
					CEffect_Manager::GetInstance()->Create_GoofyHitEffect(vCenter, vLook);
					CEffect_Manager::GetInstance()->Create_HitStarGoofy(pDestCollider->Get_Parent()->Get_Transform(), 0.7f);
					m_pGoofy->Add_AttackMonster(static_cast<CMonster*>(pDestParent));
				}
			}

		}
	}
}

HRESULT CGoofy_Weapon::Throw_Effect()
{
	m_vPosition = XMStoreFloat4RT(m_pTransformCom->Get_Position());
	m_vPaticlePosition[0] = XMStoreFloat4RT(m_pTransformCom->Get_Position() + m_pTransformCom->Get_Up() * 2.f);
	m_vPaticlePosition[1] = XMStoreFloat4RT(m_pTransformCom->Get_Position() - m_pTransformCom->Get_Up() * 2.f);
	m_vPaticlePosition[2] = XMStoreFloat4RT(m_pTransformCom->Get_Position() + m_pTransformCom->Get_Right() * 2.f);
	m_vPaticlePosition[3] = XMStoreFloat4RT(m_pTransformCom->Get_Position() - m_pTransformCom->Get_Right() * 2.f);


	if (true == m_bEffect && false == m_bEffectStart)
	{
		CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

		tEffectMeshDesc.bBillboarding = false;
		lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Tornado11"));
		tEffectMeshDesc.iTextureIndex = 72;
		tEffectMeshDesc.iScondTextureIndex = 0;
		tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
		tEffectMeshDesc.fLifeTime = 100.f;
		tEffectMeshDesc.pParentPos = &m_vPosition;
		tEffectMeshDesc.iPassIndex = 0;
		tEffectMeshDesc.fTransparency = 0.f;
		tEffectMeshDesc.fTransparecySpeed = 0.f;
		tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, 1.f, 0.f);
		tEffectMeshDesc.fMaxTransparecy = 0.f;
		tEffectMeshDesc.bSpinning = true;
		tEffectMeshDesc.fSpinningSpeed = 60.f;
		tEffectMeshDesc.vSpinAxis = __vector(0.f, -1.f, 0.f, 0.f);
		tEffectMeshDesc.bBillboarding = false;
		tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
		tEffectMeshDesc.bUseEasing_Scaling = false;
		tEffectMeshDesc.vStartScale = __vector(0.01f, 0.01f, 0.01f, 0.f);
		tEffectMeshDesc.vMaxScale = __vector(1.f, 1.f, 1.f, 0.f);
		tEffectMeshDesc.fScaleSpeed = 0.f;
		tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
		tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;
		tEffectMeshDesc.bMovingUV = true;
		tEffectMeshDesc.vUVMovingSpeed = _float2(0.f, 2.f);
		tEffectMeshDesc.bUsingEmissive = false;
		tEffectMeshDesc.bUsingBlur = true;
		tEffectMeshDesc.bUsingDistortion = false;
		tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
		tEffectMeshDesc.vColor = _float4(0.3f, 1.f, 0.f, 0.f);
		tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);

		if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		m_pEffect[1] = static_cast<CEffect_Mesh*>(m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Effect")));

		m_bEffectStart = true;
	}

	if (nullptr != m_pEffect[1])
	{
		CTransform*	pEffectTransform = static_cast<CTransform*>(m_pEffect[1]->Get_Component(TEXT("Com_Transform")));
		pEffectTransform->Set_Position(m_pTransformCom->Get_Position());

		if (true == m_pEffect[1]->Get_Dead())
		{
			m_pEffect[1] = nullptr;
		}
	}

	if (false == m_bEffect && true == m_bEffectStart)
	{
		m_pEffect[1]->Set_MeshLifeType(CEffect_Mesh::LOOP_LIMIT);
		m_pEffect[1]->Set_PlayType(CEffect_Mesh::PLAY_ONCE);

		m_bEffectStart = false;
	}

	return S_OK;
}

HRESULT CGoofy_Weapon::Throw_Particle()
{
	// 파티클 생성
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase = false;
	tParticleDesc.bLoop = false;
	tParticleDesc.bTrail = true;
	tParticleDesc.bUseEasing = false;
	tParticleDesc.eDir = CEffect_Particle::DIR_ONE;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_30;

	_float	fAddColor[4] = { -0.25f, -0.22f, -0.38f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fLiveTime = 100.f;
	tParticleDesc.fMakeRange = 0.5f;
	tParticleDesc.fMaxDistance = 300.f;
	tParticleDesc.fMinSpeed = 0.3f;
	tParticleDesc.fMaxSpeed = 0.3f;
	tParticleDesc.fScaleSpeed = 2.f;
	tParticleDesc.fOriginScale = 0.5f;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.iLevelIndex = 0;
	tParticleDesc.iTexIndex = 2;
	tParticleDesc.pDead = m_pEffect[1]->Get_DeadPointer();
	tParticleDesc.pParentPosition = &m_vPaticlePosition[0];
	tParticleDesc.vDir = __vector(0.f, -1.f, 0.f, 0.f);

	for (_uint i = 0; i < 2; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		m_Particle.push_back(m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Effect")));
	}

	tParticleDesc.pParentPosition = &m_vPaticlePosition[1];
	for (_uint i = 0; i < 2; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		m_Particle.push_back(m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Effect")));
	}

	tParticleDesc.pParentPosition = &m_vPaticlePosition[2];
	for (_uint i = 0; i < 2; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		m_Particle.push_back(m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Effect")));
	}

	tParticleDesc.pParentPosition = &m_vPaticlePosition[3];
	for (_uint i = 0; i < 2; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		m_Particle.push_back(m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Effect")));
	}
	return S_OK;
}

CGoofy_Weapon * CGoofy_Weapon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CGoofy_Weapon*	pInstance = new CGoofy_Weapon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CGoofy_Weapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGoofy_Weapon::Clone(void * pArg)
{
	CGoofy_Weapon*	pInstance = new CGoofy_Weapon(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CGoofy_Weapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGoofy_Weapon::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
