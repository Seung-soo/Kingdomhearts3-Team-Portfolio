#include "stdafx.h"
#include "State_IceManAtk.h"
#include "GameInstance.h"
#include "Monster.h"
#include "MonsterCooler.h"
#include "MonsterSkill.h"
#include "Effect_Manager.h"

CState_IceManAtk::CState_IceManAtk()
{
}

HRESULT CState_IceManAtk::Initialize(void* pArg)
{
	if (pArg == nullptr)
	{
		BREAKPOINT;
	}

	// IceMan 이 사용할 공격을 설정해줌
	memcpy(&m_eIceManSkill, pArg, sizeof(CIceMan::ICEMAN_SKILL));

	return S_OK;
}

HRESULT CState_IceManAtk::EnterState()
{
	// IceMan 설정
	m_pIceMan            = static_cast<CIceMan*>(m_pGameObject);
	m_pMonsterNavigation = static_cast<CNavigation*>(m_pGameObject->Get_Component(TEXT("Com_Navigation")));

	if (m_pMonsterNavigation == nullptr)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 플레이어 트랜스폼 설정
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_LevelIndex(), TEXT("Layer_Player"), TEXT("Com_Transform"), 0));

	// AnimPlayIndex 초기화
	m_pIceMan->ResetAnimPlayIndex();

	// 애니메이션 설정 및 초기값 설정
	switch (m_eIceManSkill)
	{
	case CIceMan::ICEMAN_SKILL_WALKSLASH:
		m_pIceMan->Set_AnimAndReset(m_pModelCom, 9); // 걷는 애니메이션으로 변경
		break;
	case CIceMan::ICEMAN_SKILL_CLAP:
		m_pIceMan->Set_AnimAndReset(m_pModelCom, 9); // 걷는 애니메이션으로 변경
		break;
	case CIceMan::ICEMAN_SKILL_LONGSLASH:
		m_pIceMan->Set_AnimAndReset(m_pModelCom, 9); // 걷는 애니메이션으로 변경
		break;
	case CIceMan::ICEMAN_SKILL_JUMPDOWNWARDBLOW:
		m_pIceMan->Set_AnimAndReset(m_pModelCom, 9); // 걷는 애니메이션으로 변경
		// 점프관련 변수 초기화
		CTransform::MONSTERJUMPDESC MonsterJumpDesc;
		MonsterJumpDesc.fOriginY    = vecy(m_pTransformCom->Get_Position());
		MonsterJumpDesc.fJumpSpeed  = 5.f;
		MonsterJumpDesc.fJumpPower  = 20.f;
		MonsterJumpDesc.JumpTimeAcc = 0.0;
		m_pTransformCom->SetMonsterJumpDesc(MonsterJumpDesc);
		break;
	default:
		break;
	}

	return S_OK;
}

_bool CState_IceManAtk::Action(_double TimeDelta)
{
	_bool bStateEnd = false;

	switch (m_eIceManSkill)
	{
	case CIceMan::ICEMAN_SKILL_WALKSLASH:
		bStateEnd = Atk_WalkSlash(TimeDelta);
		break;
	case CIceMan::ICEMAN_SKILL_CLAP:
		bStateEnd = Atk_Clap(TimeDelta);
		break;
	case CIceMan::ICEMAN_SKILL_LONGSLASH:
		bStateEnd = Atk_LongSlash(TimeDelta);
		break;
	case CIceMan::ICEMAN_SKILL_JUMPDOWNWARDBLOW:
		bStateEnd = Atk_JumpDownwardBlow(TimeDelta);
		break;
	default:
		break;
	}

	if (bStateEnd == true)
	{
		// 공격 상태 끄기
		m_pGameObject->Set_IsAttacking(false);
		return DELETE_STATE;
	}

	return KEEP_STATE;
}

HRESULT CState_IceManAtk::ClearState()
{
	if (m_pSkill != nullptr)
	{
		m_pSkill->Set_Dead(true);
		m_pSkill = nullptr;
	}

	return S_OK;
}

_bool CState_IceManAtk::Atk_WalkSlash(_double TimeDelta)
{
	// 공격 콜라이더 타이밍 설정
	m_pIceMan->SetAttackTime(m_pModelCom, 29, 24.0, 27.0);
	m_pIceMan->SetAttackTime(m_pModelCom, 29, 39.0, 42.0);

	// 이펙트
	if (m_pModelCom->Get_TargetTimeAcc(29) >= 24.0 && m_bEffectEnd[0] == false)
	{
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("IceMan_Slash_24.wav"), 0.7f, true, m_pTransformCom->Get_Position());

		CEffect_Manager::GetInstance()->Create_IceManSlash(m_pTransformCom, true);
		m_bEffectEnd[0] = true;
	}
	if (m_pModelCom->Get_TargetTimeAcc(29) >= 39.0 && m_bEffectEnd[1] == false)
	{
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("IceMan_Slash_24.wav"), 0.7f, true, m_pTransformCom->Get_Position());

		CEffect_Manager::GetInstance()->Create_IceManSlash(m_pTransformCom, false);
		m_bEffectEnd[1] = true;
	}

	// 플레이어를 바라보면서 접근하다가 공격 애니메이션으로 변경
	if (m_pModelCom->Get_CurrAnimIndex() == 9)
	{
		if (ChasePlayer(TimeDelta) == true)
			m_pIceMan->Set_AnimAndReset(m_pModelCom, m_WalkSlash[0]);

		return false;
	}

	// 애니메이션 속도 설정
	m_pModelCom->Set_TargetPlaySpeed(29, 55.0);

	// 애니메이션이 끝나면 true 리턴해서 상태 종료되게 함
	return m_pIceMan->PlayAnimations(m_pModelCom, m_WalkSlash);
}

_bool CState_IceManAtk::Atk_Clap(_double TimeDelta) 
{
	// 공격 콜라이더 타이밍 설정
	m_pIceMan->SetAttackTime(m_pModelCom, 30, 39.0, 42.0);

	// 이펙트
	if (m_pModelCom->Get_TargetTimeAcc(30) >= 41.0 && m_bEffectEnd[0] == false)
	{
		CEffect_Manager::GetInstance()->Create_IceManClap(m_pTransformCom);
		m_bEffectEnd[0] = true;
	}
	if (m_pModelCom->Check_CurAnim_And_Finished(30))
	{
		m_bEffectEnd[0] = false;
	}

	// 플레이어를 바라보면서 접근하다가 공격 애니메이션으로 변경
	if (m_pModelCom->Get_CurrAnimIndex() == 9)
	{
		if (ChasePlayer(TimeDelta) == true)
			m_pIceMan->Set_AnimAndReset(m_pModelCom, m_Clap[0]);

		return false;
	}

	// 애니메이션 속도 설정
	m_pModelCom->Set_TargetPlaySpeed(30, 55.0);

	// 밀어내기
	if (m_bPushEnd == false && m_pModelCom->Get_TargetTimeAcc(30) >= 45.0)
	{
		CNavigation* pPlayerNavigation  = static_cast<CNavigation*>(CGameInstance::GetInstance()->Get_Component(CGameInstance::GetInstance()->Get_LevelIndex(), TEXT("Layer_Player"), TEXT("Com_Navigation"), 0));
		m_bPushEnd = m_pTransformCom->PushObject(m_pPlayerTransform, TimeDelta, 1.0, 5.0, pPlayerNavigation);
	}

	// 아이스볼스킬 생성
	if (m_pModelCom->Get_TargetTimeAcc(30) >= 45.0 && m_bEffectEnd[1] == false)
	{
		Create_IceBall(TimeDelta, m_pTransformCom->Get_Up());
		Create_IceBall(TimeDelta, m_pTransformCom->Get_Up() + m_pTransformCom->Get_Right());
		Create_IceBall(TimeDelta, m_pTransformCom->Get_Up() - m_pTransformCom->Get_Right());
		Create_IceBall(TimeDelta, m_pTransformCom->Get_Up() + m_pTransformCom->Get_Look());
		Create_IceBall(TimeDelta, m_pTransformCom->Get_Up() - m_pTransformCom->Get_Look());
		Create_IceBall(TimeDelta, m_pTransformCom->Get_Up() + m_pTransformCom->Get_Right() + m_pTransformCom->Get_Look());
		Create_IceBall(TimeDelta, m_pTransformCom->Get_Up() + m_pTransformCom->Get_Right() - m_pTransformCom->Get_Look());
		Create_IceBall(TimeDelta, m_pTransformCom->Get_Up() - m_pTransformCom->Get_Right() + m_pTransformCom->Get_Look());
		Create_IceBall(TimeDelta, m_pTransformCom->Get_Up() - m_pTransformCom->Get_Right() - m_pTransformCom->Get_Look());
		m_bEffectEnd[1] = true;
	}
	if (m_pModelCom->Check_CurAnim_And_Finished(30))
		m_bEffectEnd[1] = false;

	// 애니메이션이 끝나면 true 리턴해서 상태 종료되게 함
	return m_pIceMan->PlayAnimations(m_pModelCom, m_Clap);
}

_bool CState_IceManAtk::Atk_LongSlash(_double TimeDelta)
{	
	// 공격 콜라이더 타이밍 설정
	m_pIceMan->SetAttackTime(m_pModelCom, 32, 9.0, 20.0);
	m_pIceMan->SetAttackTime(m_pModelCom, 34, 16.0, 19.0);

	// 플레이어를 바라보면서 접근하다가 공격 애니메이션으로 변경
	if (m_pModelCom->Get_CurrAnimIndex() == 9)
	{
		if (ChasePlayer(TimeDelta) == true)
			m_pIceMan->Set_AnimAndReset(m_pModelCom, m_LongSlashIndices[0]);

		return false;
	}

	// 이펙트
	if (m_pModelCom->Get_TargetTimeAcc(32) >= 8.0 && m_bEffectEnd[0] == false)
	{
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("IceMan_Slash_24.wav"), 0.7f, true, m_pTransformCom->Get_Position());

		CEffect_Manager::GetInstance()->Create_IceManSlash(m_pTransformCom, true);
		m_bEffectEnd[0] = true;
	}
	if (m_pModelCom->Get_TargetTimeAcc(32) >= 18.0 && m_bEffectEnd[1] == false)
	{
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("IceMan_Slash_24.wav"), 0.7f, true, m_pTransformCom->Get_Position());

		CEffect_Manager::GetInstance()->Create_IceManSlash(m_pTransformCom, false);
		m_bEffectEnd[1] = true;
	}
	if (m_pModelCom->Check_CurAnim_And_Finished(32))
	{
		m_bEffectEnd[0] = false;
		m_bEffectEnd[1] = false;
	}
	if (m_pModelCom->Get_TargetTimeAcc(34) >= 16.0 && m_bEffectEnd[0] == false)
	{
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("IceMan_Slash_24.wav"), 0.7f, true, m_pTransformCom->Get_Position());

		CEffect_Manager::GetInstance()->Create_IceManSlash(m_pTransformCom, true);
		m_bEffectEnd[0] = true;
	}


	if (m_pModelCom->Get_CurrAnimIndex() == 32)
	{
		// 애니메이션 속도 설정
		m_pModelCom->Set_CurrPlaySpeed(40.0);

		// 손톱 휘두르면서 앞으로 가다가 네비게이션의 끝에 도착하면 벽이랑 충돌상태로 만들고 상태 종료
		if (m_pTransformCom->Go_Straight_Check_NaviEnd(TimeDelta * 2.5, m_pMonsterNavigation) == true)
		{
			m_pIceMan->SetCollisionWall(true);
			return true;
		}
	}

	// 애니메이션이 끝나면 true 리턴해서 상태 종료되게 함
	return m_pIceMan->PlayAnimations(m_pModelCom, m_LongSlashIndices, vector<_uint>{ 32 });
}

_bool CState_IceManAtk::Atk_JumpDownwardBlow(_double TimeDelta)
{
	// 공격 콜라이더 타이밍 설정
	m_pIceMan->SetAttackTime(m_pModelCom, 39, 1.0, 2.0);

	// 플레이어를 바라보면서 접근하다가 둘러보는 애니메이션(2)으로 변경
	if (m_pModelCom->Get_CurrAnimIndex() == 9)
	{
		if (ChasePlayer(TimeDelta) == true)
		{
			m_pIceMan->Set_AnimAndReset(m_pModelCom, 2);
			m_pModelCom->Set_TargetPlaySpeed(2, 35.0);
		}

		return false;
	}

	// 둘러보는 애니메이션이 끝나면 소리지르는 애니메이션(1)으로 변경하고 점프가이드 생성
	if (m_pModelCom->Check_CurAnim_And_Finished(2))
	{
		m_pIceMan->Set_AnimAndReset(m_pModelCom, 1);

		// 몬스터 스킬(점프가이드) 생성
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		CMonsterSkill::MONSTERSKILLDESC MonsterSkillDesc;
		MonsterSkillDesc.eSkillType       = CMonsterSkill::MONSTERSKILLTYPE_ICEMANTRACEPLAYER;
		MonsterSkillDesc.bNoModel         = true;
		MonsterSkillDesc.bNoCollider      = true;
		MonsterSkillDesc.vPos             = m_pTransformCom->Get_Position();
		MonsterSkillDesc.pPlayerTransform = m_pPlayerTransform;
		MonsterSkillDesc.fSpeed           = 15.f;
		MonsterSkillDesc.Duration         = 10.f;
		MonsterSkillDesc.fSize            = 1.f;

		m_pSkill = pGameInstance->Add_GameObject_And_Return(pGameInstance->Get_LevelIndex(), TEXT("Layer_MonsterSkill"), TEXT("Prototype_GameObject_MonsterSkill"), &MonsterSkillDesc);
		CEffect_Manager::GetInstance()->Create_IceManJumpAtkGuide(m_pSkill->Get_Transform(), m_pSkill->Get_DeadPointer());
	}

	if (m_pModelCom->Get_TargetTimeAcc(1) >= 20.0 && m_bSoundEnd[0] == false)
	{
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("IceMan_Spawn_23.wav"), 0.7f, true, m_pTransformCom->Get_Position());

		m_bSoundEnd[0] = true;
	}

	// 소리지르는 애니메이션이 끝나면 m_vJumpDir, m_fGoPower 계산하고 점프 시작 애니메이션으로 변경
	if (m_pModelCom->Check_CurAnim_And_Finished(1))
	{
		// 점프해서 가야할 방향(m_vJumpDir) 설정
		m_vJumpDir = m_pTransformCom->Get_Dir(m_pPlayerTransform);

		// 점프해서 앞으로 가야할 파워(m_GoPower) 설정 (m_GoPower 가 5.0 일때 30.f 만큼 이동)
		_float fDistance = m_pTransformCom->Get_Distance(m_pPlayerTransform);
		m_GoPower        = fDistance * 5.0 * 0.0315;

		m_pIceMan->Set_AnimAndReset(m_pModelCom, m_JumpDownwardBlowIndices[0]);

		// 점프가이드 추적 종료
		if (m_pSkill != nullptr)
			static_cast<CMonsterSkill*>(m_pSkill)->SetChaseState(false);
	}

	// 이펙트
	if (m_pModelCom->Get_TargetTimeAcc(1) >= 20.0 && m_bEffectEnd[0] == false)
	{
		CEffect_Manager::GetInstance()->Create_IceManRoar(m_pTransformCom);
		m_bEffectEnd[0] = true;
	}
	if (m_pModelCom->Check_CurAnim_And_Finished(35))
	{
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("IceMan_Jump_17.wav"), 0.7f, true, m_pTransformCom->Get_Position());

		CEffect_Manager::GetInstance()->Create_IceManJump(m_pTransformCom); // 점프
	}
	if (m_pModelCom->Check_CurAnim_And_Finished(38))
	{
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("IceMan_Landing_14.wav"), 0.7f, true, m_pTransformCom->Get_Position());

		CEffect_Manager::GetInstance()->Create_IceManJumpLanding(m_pTransformCom); // 착지
	}
	if (m_pModelCom->Get_TargetTimeAcc(39) >= 1.0 && m_bEffectEnd[0] == false)
	{
		CEffect_Manager::GetInstance()->Create_IceManJumpAtkLanding(m_pTransformCom); // 내려찍기
		CEffect_Manager::GetInstance()->Create_IceManJumpAtkLanding(m_pTransformCom, false);
		m_bEffectEnd[0] = true;
	}
	if (m_pModelCom->Check_CurAnim_And_Finished(39))
	{
		m_bEffectEnd[0] = false;
	}

	// 36, 37, 38 애니메이션 실행 중일 때 점프
	if (m_pModelCom->Get_CurrAnimIndex() == 36 || m_pModelCom->Get_CurrAnimIndex() == 37 || m_pModelCom->Get_CurrAnimIndex() == 38)
	{
		// 점프
		m_pTransformCom->MonsterJump(TimeDelta, 0.f);

		// 플레이어쪽으로 회전
		m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta * 5.0);

		// m_vJumpDir 쪽으로 이동
		m_pTransformCom->Go_Vector(m_vJumpDir, TimeDelta * m_GoPower, m_pMonsterNavigation);
	}

	// 착지하면 점프가이드 종료
	if (m_pModelCom->Check_CurAnim_And_Finished(38))
	{
		if (m_pSkill != nullptr)
		{
			m_pSkill->Set_Dead(true);
			m_pSkill = nullptr;
		}
	}

	// 마지막 애니메이션 때 높이가 네비보다 높으면 네비높이로 맞춰주기
	if (m_pModelCom->Get_CurrAnimIndex() == 39)
		m_pTransformCom->SetHeightZero(TimeDelta * 5.0, m_pMonsterNavigation);

	// 애니메이션이 끝나면 true 리턴해서 상태 종료되게 함
	return m_pIceMan->PlayAnimations(m_pModelCom, m_JumpDownwardBlowIndices, vector<_uint>{ 36, 38 });
}

_bool CState_IceManAtk::ChasePlayer(_double TimeDelta)
{
	if (m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta * 3.0, 0.05f) == false)
	{
		m_pTransformCom->Go_Straight(TimeDelta, m_pMonsterNavigation);
		return false;
	}

	return true;
}

void CState_IceManAtk::Create_IceBall(_double TimeDelta, _fvector vFirstDir)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// 몬스터 스킬 생성
	CMonsterSkill::MONSTERSKILLDESC MonsterSkillDesc;
	MonsterSkillDesc.eSkillType       = CMonsterSkill::MONSTERSKILLTYPE_AWAIT_STRAIGHT;
	MonsterSkillDesc.bNoModel         = true;
	MonsterSkillDesc.vFirstDir        = XMVector3Normalize(vFirstDir);
	MonsterSkillDesc.vPos             = m_pTransformCom->Get_Position() + __vector(0.f, 3.f, 0.f, 0.f);
	MonsterSkillDesc.pPlayerTransform = m_pPlayerTransform;
	MonsterSkillDesc.fSpeed           = (_float)(rand() % 55) + 35.f;
	MonsterSkillDesc.fDamage          = 5.f;
	MonsterSkillDesc.Duration         = 3.5f;
	MonsterSkillDesc.fSize            = 3.f;

	CGameObject* pSkill = pGameInstance->Add_GameObject_And_Return(pGameInstance->Get_LevelIndex(), TEXT("Layer_MonsterSkill"), TEXT("Prototype_GameObject_MonsterSkill"), &MonsterSkillDesc);
	CEffect_Manager::GetInstance()->Create_IceBall(pSkill->Get_Transform(), pSkill->Get_DeadPointer());
}

CState_IceManAtk* CState_IceManAtk::Create(void* pArg)
{
	CState_IceManAtk* pInstance = new CState_IceManAtk;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

void CState_IceManAtk::Free()
{
	__super::Free();
}