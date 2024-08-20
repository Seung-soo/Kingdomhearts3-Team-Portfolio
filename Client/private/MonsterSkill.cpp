#include "stdafx.h"
#include "MonsterSkill.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"
#include "Terrain.h"
#include "ImGui_Manager.h"
#include "Effect_Manager.h"

CMonsterSkill::CMonsterSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{
}

CMonsterSkill::CMonsterSkill(const CMonsterSkill& rhs)
	: CMonster(rhs)
{
}

HRESULT CMonsterSkill::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterSkill::NativeConstruct(void* pArg)
{
	if (pArg == nullptr)
	{
		BREAKPOINT;
	}
	else
	{
		memcpy(&m_MonsterSkillDesc, pArg, sizeof(MONSTERSKILLDESC));
	}

	// 부모 초기화할 때 트랜스폼 초기화
	CTransform::TRANSFORMDESC TransformDesc;
	TransformDesc.fSpeedPerSec = m_MonsterSkillDesc.fSpeed;
	TransformDesc.fRotationPerSec = 10.f;
	TransformDesc.fScalingPerSec = 5.f;
	if (FAILED(__super::NativeConstruct(&TransformDesc)))
		return E_FAIL;

	// 위치 설정
	m_pTransformCom->Set_Position(m_MonsterSkillDesc.vPos);

	// 사이즈 설정
	m_pTransformCom->Scaled(__vector(m_MonsterSkillDesc.fSize, m_MonsterSkillDesc.fSize, m_MonsterSkillDesc.fSize, 0.f));

	// 컴포넌트 초기화
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	// 쉐이더패스 설정
	m_iPass = 31;

	// 터레인설정
	if (m_pGameInstance->Get_LevelIndex() == LEVEL_RAPUNZEL_BOSS)
		m_pTerrain = static_cast<CTerrain*>(m_pGameInstance->Get_Front(LEVEL_RAPUNZEL_BOSS, TEXT("Layer_Terrain")));

	if (m_MonsterSkillDesc.eSkillType == MONSTERSKILLTYPE_TRACING)
	{
		m_pTransformCom->MakeSameDirection(m_MonsterSkillDesc.vDir);
	}

	if (m_MonsterSkillDesc.eSkillType == MONSTERSKILLTYPE_GOATMAGICSHOT)
	{
		m_MonsterSkillDesc.RandTime += 1.5;
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Goat_60_FireBall.wav"), 0.5f, true, m_pTransformCom->Get_Position());
	}

	return S_OK;
}

_int CMonsterSkill::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	// 죽으면 RESULT_DEAD 반환
	if (true == m_bDead)
	{
		if (m_MonsterSkillDesc.eSkillType == MONSTERSKILLTYPE_STRAIGHT)
		{
			if (LEVEL_TWILIGHT != GM->Get_CurrentLevel())
				CEffect_Manager::GetInstance()->Create_SkillDeadEffect(m_pTransformCom, 0.f);
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Goat_61_FireBall.wav"), 0.5f, true, m_pTransformCom->Get_Position());
		}
		else if (m_MonsterSkillDesc.eSkillType == MONSTERSKILLTYPE_BOUNCE)
		{
			CEffect_Manager::GetInstance()->Create_Explosion(m_pTransformCom->Get_Position(), 7.f, 1);
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Goat_61_FireBall.wav"), 0.5f, true, m_pTransformCom->Get_Position());
		}
		else if (m_MonsterSkillDesc.eSkillType == MONSTERSKILLTYPE_TRACING)
		{
			CEffect_Manager::GetInstance()->Create_DeadEffect(m_pTransformCom, 4.f, 1.f);
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("DarkWolf_Fire_Explosion.wav"), 0.5f, true, m_pTransformCom->Get_Position());
		}
		else if (m_MonsterSkillDesc.eSkillType == MONSTERSKILLTYPE_GOATMAGICSHOT)
		{
			CEffect_Manager::GetInstance()->Create_Goat_FireBallExplosion(m_pTransformCom);
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Goat_61_FireBall.wav"), 0.5f, true, m_pTransformCom->Get_Position());
		}
		else if (m_MonsterSkillDesc.eSkillType == MONSTERSKILLTYPE_SHURIKEN)
		{
			CEffect_Manager::GetInstance()->Create_SkillDeadEffect_Tornado(m_pTransformCom);
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Goat_61_FireBall.wav"), 0.5f, true, m_pTransformCom->Get_Position());
		}
		else if (m_MonsterSkillDesc.eSkillType == MONSTERSKILLTYPE_AWAIT_STRAIGHT)
		{
			CEffect_Manager::GetInstance()->Create_IceBallBreak(m_pTransformCom);
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Goat_61_FireBall.wav"), 0.5f, true, m_pTransformCom->Get_Position());
		}

		return RESULT_DEAD;
	}

	// 스킬 실행
	Action(TimeDelta);

	if (m_MonsterSkillDesc.bNoCollider == false && m_bFirstTick == false)
	{
		// 콜라이더 위치 업데이트
		m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

		// 콜리전매니저에 콜라이더 등록 (Tick 의 마지막에서 등록해야함)
		m_pGameInstance->Add_Collider(string("Monster_Atk"), m_pColliderCom);
	}

	// 이펙트용 위치갱신
	m_pTransformCom->Get_PositionFloat4_Pointer();

	return RESULT_NOPROBLEM;
}

_int CMonsterSkill::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;

	// 충돌 검사 (LateTick 에서 실행할 것)
	if (m_MonsterSkillDesc.bNoCollider == false && m_bFirstTick == false)
		OnCollision(TimeDelta);

	// 절두체 안에 있다면 + m_bDead 상태가 아니면 렌더그룹에 추가
	if (m_MonsterSkillDesc.bNoModel == false && m_bFirstTick == false)
	{
		if (TRUE == m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fFrustumRange) && FALSE == m_bDead)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	}

#ifdef _DEBUG
	// 콜라이더 디버그 렌더링
	if (m_MonsterSkillDesc.bNoCollider == false && m_bFirstTick == false)
		m_pRendererCom->Add_Debug(m_pColliderCom);
#endif

	return RESULT_NOPROBLEM;
}

HRESULT CMonsterSkill::Render()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (m_MonsterSkillDesc.bNoModel == false)
	{
		_uint iNumMaterials = m_pModelCom->Get_NumberOfMaterials();

		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Render(m_pShaderCom, m_iPass, i);
		}
	}

	return S_OK;
}

void CMonsterSkill::PostRender(_double TimeDelta)
{
	ImGui::Begin("MONSTERSKILL");

	ImGui::Text("%d", m_bDead);

	ImGui::End();
}

HRESULT CMonsterSkill::SetUp_Components()
{
	/* For.Com_Status */
	CStatus::STATUSDESC StatusDesc;
	StatusDesc.fMaxHP = 5.f;
	StatusDesc.fHP = StatusDesc.fMaxHP;
	StatusDesc.fDamaged = m_MonsterSkillDesc.fDamage;
	StatusDesc.iLevel = 1;
	StatusDesc.fEXP = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Status"), TEXT("Com_Status"), (CComponent**)&m_pStatusCom, &StatusDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider */
	if (m_MonsterSkillDesc.bNoCollider == false)
	{
		CCollider::COLLIDERDESC ColliderDesc;
		ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
		ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
		ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
		ColliderDesc.vTranslation = m_MonsterSkillDesc.vColliderPos;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
			return E_FAIL;
	}


	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (m_MonsterSkillDesc.bNoModel == false)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(m_MonsterSkillDesc.ModelPrototypeLevel, m_MonsterSkillDesc.ModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonsterSkill::SetUp_ConstantTable()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (m_iPass == 41)
	{
		_bool bBlur = true;
		if (FAILED(m_pShaderCom->Set_RawValue("g_iBlur", &bBlur, sizeof(_bool))))
			return E_FAIL;

		_float		fRim = 0.8f;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRimPower", &fRim, sizeof(_float))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		_float		fRimRange = 0.6f;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRimRange", &fRimRange, sizeof(_float))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		_float4		vColor = _float4(1.f, 0.f, 0.f, 0.f);
		if (FAILED(m_pShaderCom->Set_RawValue("g_vRimColor", &vColor, sizeof(_float4))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		_float4 vCam;
		XMStoreFloat4(&vCam, pGameInstance->Get_CamPosition());

		if (FAILED(m_pShaderCom->Set_RawValue("g_vWorldCamPos", &vCam, sizeof(_float4))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		_bool isUsingRim = true;
		if (FAILED(m_pShaderCom->Set_RawValue("g_isRim", &isUsingRim, sizeof(_bool))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		_bool isUsingBlur = false;
		if (FAILED(m_pShaderCom->Set_RawValue("g_isBlur", &isUsingBlur, sizeof(_bool))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	return S_OK;
}

void CMonsterSkill::Action(_double TimeDelta)
{
	m_TimeAcc += TimeDelta;

	if (m_MonsterSkillDesc.eSkillType == MONSTERSKILLTYPE_STRAIGHT)
	{
		m_pTransformCom->MakeSameDirection(XMVector3Normalize(m_MonsterSkillDesc.vDir));
		m_pTransformCom->Go_Straight(TimeDelta);
	}
	else if (m_MonsterSkillDesc.eSkillType == MONSTERSKILLTYPE_AWAIT_STRAIGHT) // IceMan 눈덩이 공격에 사용
	{
		if (vecy(m_pTransformCom->Get_Position()) <= 0.5f)
			m_bDead = true;

		if (m_TimeAcc < 1.0)
		{
			_vector vTargetPos = m_MonsterSkillDesc.pPlayerTransform->Get_Position() + __vector((_float)(rand() % 7), 0.f, (_float)(rand() % 7), 0.f);
			m_MonsterSkillDesc.vDir = m_pTransformCom->Get_Dir(vTargetPos);
			m_pTransformCom->Go_Vector(m_MonsterSkillDesc.vFirstDir, TimeDelta * 0.1);
		}
		else
		{
			if (m_bEffectEnd == false)
			{
				CEffect_Manager::GetInstance()->Create_IceBallParticle(m_pTransformCom, &m_bDead);
				m_bEffectEnd = true;
			}

			m_pTransformCom->MakeSameDirection(m_MonsterSkillDesc.vDir);
			m_pTransformCom->Go_Straight(TimeDelta);
		}
	}
	else if (m_MonsterSkillDesc.eSkillType == MONSTERSKILLTYPE_BEZIER)
	{
		m_LerpAcc += TimeDelta;

		_vector vLerpEndPos = XMVectorSetW(BezierCurve(m_MonsterSkillDesc.vPos, m_MonsterSkillDesc.vViaPos, m_MonsterSkillDesc.vDestPos, (_float)m_LerpAcc * m_MonsterSkillDesc.fLerpSpeed), 1.f);
		m_pTransformCom->Set_Position(vLerpEndPos);
	}
	else if (m_MonsterSkillDesc.eSkillType == MONSTERSKILLTYPE_BEZIERROTATION) // Monkey 공격에 사용
	{
		m_LerpAcc += TimeDelta;

		_vector vLerpEndPos = XMVectorSetW(BezierCurve(m_MonsterSkillDesc.vPos, m_MonsterSkillDesc.vViaPos, m_MonsterSkillDesc.vDestPos, (_float)m_LerpAcc * m_MonsterSkillDesc.fLerpSpeed), 1.f);
		m_pTransformCom->Set_Position(vLerpEndPos);
		m_pTransformCom->Random_Rotation(TimeDelta);
	}
	else if (m_MonsterSkillDesc.eSkillType == MONSTERSKILLTYPE_SPREAD) // Mower 공격에 사용
	{
		_vector vPosition = m_pTransformCom->Get_Position();

		m_MonsterSkillDesc.SpreadTime += m_MonsterSkillDesc.fSpeed * TimeDelta;

		_float fY = m_MonsterSkillDesc.fOriginY + ((m_MonsterSkillDesc.fSpreadPower * (_float)m_MonsterSkillDesc.SpreadTime) - ((_float)m_MonsterSkillDesc.SpreadTime * (_float)m_MonsterSkillDesc.SpreadTime * 5.5f * 0.5f));

		vPosition = XMVectorSetY(vPosition, fY) + m_MonsterSkillDesc.vDir * 0.1f;
		m_pTransformCom->Set_Position(vPosition);
	}
	else if (m_MonsterSkillDesc.eSkillType == MONSTERSKILLTYPE_SPIRAL) // ShiningEgg 공격에 사용
	{
		m_pTransformCom->SpiralMoveXZ(TimeDelta, nullptr);
		m_pTransformCom->Set_Position(m_pTransformCom->Get_Position() + m_MonsterSkillDesc.vDir * 0.01f);
	}
	else if (m_MonsterSkillDesc.eSkillType == MONSTERSKILLTYPE_BOUNCE) // BigTree 공격에 사용
	{
		// 1.5 초 지나야 떨어지기 시작
		if (m_TimeAcc >= 1.5)
		{
			if (m_bEffectEnd == false)
			{
				CEffect_Manager::GetInstance()->Create_Apple(m_pTransformCom->Get_WorldFloat4x4_Pointer());
				m_bEffectEnd = true;
			}

			_float fY = 0.f;
			if (m_pGameInstance->Get_LevelIndex() == LEVEL_RAPUNZEL_BOSS)
				fY = vecy(m_pTerrain->Compute_PositionOnTerrain(m_pTransformCom->Get_Position())) + 0.75f;

			m_iPass = 41;

			if (m_bStartBounce == false)
				m_pTransformCom->Go_Vector(m_MonsterSkillDesc.vDir, TimeDelta, nullptr);

			if (vecy(m_pTransformCom->Get_Position()) <= fY)
				m_bStartBounce = true;

			if (m_bStartBounce)
				m_pTransformCom->Go_Up(TimeDelta * 0.3, nullptr);
		}
	}
	else if (m_MonsterSkillDesc.eSkillType == MONSTERSKILLTYPE_SHURIKEN) // Nightmare 공격에 사용
	{
		if (m_TimeAcc <= 0.5)
		{
			m_pTransformCom->Go_Up(TimeDelta * 0.3);
		}
		else if (m_TimeAcc > m_MonsterSkillDesc.PauseTime)
		{
			_vector vTargetDir = m_pTransformCom->Get_Dir(m_MonsterSkillDesc.pPlayerTransform);
			m_pTransformCom->Go_Vector(vTargetDir, TimeDelta);
		}
	}
	else if (m_MonsterSkillDesc.eSkillType == MONSTERSKILLTYPE_TRACING) // ForzenWolf ThorwFire 공격에 사용
	{
		m_pTransformCom->Random_Rotation(TimeDelta);

		// 플레이어 따라가기 전 준비
		if (false == m_bStartChasing)
		{
			_float fDist = m_pTransformCom->Get_Distance(m_MonsterSkillDesc.vDestPos);

			if (fDist > 1.f)
			{
				auto vPos = XMVectorLerp(m_pTransformCom->Get_Position(), m_MonsterSkillDesc.vDestPos, (_float)TimeDelta * 2.f);

				vPos = XMVectorSetW(vPos, 1.f);

				m_pTransformCom->Set_Position(vPos);
			}

			else
			{
				m_bStartChasing = true;
				m_TimeAcc = 0.0;
				m_vTargetPos = m_MonsterSkillDesc.pPlayerTransform->Get_Position() + __vector(0.f, 1.f, 0.f, 0.f);
			}
		}

		// 플레이어 체이싱
		else
		{

			if (false == m_bFinishTurning)
			{
				auto vDir = m_pTransformCom->Get_Dir(m_vTargetPos);

				m_bFinishTurning = m_pTransformCom->Turn_To_Direction(vDir, TimeDelta, 0.5f);
			}


			auto vPos = XMVectorLerp(m_pTransformCom->Get_Position(), m_vTargetPos, (_float)TimeDelta * 5.f);

			vPos = XMVectorSetW(vPos, 1.f);

			m_pTransformCom->Set_Position(vPos);

			auto fDis = DISTANCE(vPos - m_vTargetPos);

			if (fDis < 0.1f)
				m_bDead = true;

		}

	}
	else if (m_MonsterSkillDesc.eSkillType == MONSTERSKILLTYPE_FALLSTRAIGHT) // ForzenWolf Rain 공격에 사용
	{
		_float fY = XMVectorGetY(m_pTransformCom->Get_Position());

		if (fY > 0.f)
		{
			m_pTransformCom->Go_Down(TimeDelta);
			m_TimeAcc = 0.0;
		}

		else
		{
			if (m_TimeAcc < 0.2)
			{
				if (false == m_bEffectEnd)
				{
					CEffect_Manager::GetInstance()->Create_Wolf_Rain_Splash(m_pTransformCom);
					CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("DarkWolf_RainExplosion.wav"), 0.5f, true, m_pTransformCom->Get_Position());

					m_bEffectEnd = true;
				}

				m_pTransformCom->Set_PositionY(0.f);
				m_pTransformCom->Scaling(TimeDelta * 5.0);
			}

			else
			{
				m_bDead = true;
			}
		}

	}
	else if (m_MonsterSkillDesc.eSkillType == MONSTERSKILLTYPE_ICEMANTRACEPLAYER) // IceMan 점프가이드에 사용
	{
		if (m_MonsterSkillDesc.bChase == true)
		{
			m_pTransformCom->Set_Position(XMVectorSetY(m_MonsterSkillDesc.pPlayerTransform->Get_Position(), 0.f));
		}
	}
	else if (m_MonsterSkillDesc.eSkillType == MONSTERSKILLTYPE_GOATMAGICSHOT) // Goat MagicShot에 사용
	{
		if (vecy(m_pTransformCom->Get_Position()) <= 0.7f)
			m_bDead = true;

		m_EffectTimeAcc += TimeDelta;
		if (m_EffectCountAcc < 3 && m_EffectTimeAcc >= 0.6)
		{
			CEffect_Manager::GetInstance()->Create_Goat_FireBallRing(m_pTransformCom, &m_bDead);
			m_EffectTimeAcc = 0.0;
			++m_EffectCountAcc;
		}
		if (m_TimeAcc < 1.5)
		{
			m_pTransformCom->Go_Up(TimeDelta * 0.1);
		}
		if (m_TimeAcc < m_MonsterSkillDesc.RandTime)
		{
			m_MonsterSkillDesc.vDir = m_pTransformCom->Get_Dir(m_MonsterSkillDesc.pPlayerTransform);
		}
		else
		{
			if (m_bEffectEnd == false)
			{
				CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Goat_69_FireBall.wav"), 0.5f, true, m_pTransformCom->Get_Position());
				m_bEffectEnd = true;
			}

			m_pTransformCom->Go_Vector(m_MonsterSkillDesc.vDir, TimeDelta);
		}
	}
}

void CMonsterSkill::OnCollision(_double TimeDelta)
{
	// 플레이어와 충돌 검사
	auto pColliderList = m_pGameInstance->Get_ColliderList(string("Player_Hit"));
	for (auto& pDestCollider : *pColliderList)
	{
		// 일반충돌, Enter, Stay, Exit 충돌별로 각각 다른 함수 실행시킬 수 있음
		if (m_pGameInstance->Collision_Sphere(m_pColliderCom, pDestCollider))
		{
			// 충돌하면 무조건 Dead
			m_bReadyToDie = true;
			m_bDead = true;
		}
	}

	// Duration 초과하면 Dead
	if (m_bReadyToDie == false && m_bDead == false)
	{
		if (m_TimeAcc >= m_MonsterSkillDesc.Duration)
		{
			m_bReadyToDie = true;
			m_bDead = true;
		}
	}
}

CMonsterSkill* CMonsterSkill::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMonsterSkill* pInstance = new CMonsterSkill(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CMonsterSkill");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMonsterSkill::Clone(void* pArg)
{
	CMonsterSkill* pInstance = new CMonsterSkill(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMonsterSkill");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonsterSkill::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pStatusCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}