#include "stdafx.h"
#include "State_MonsterAtk.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "Monster.h"
#include "MonsterSkill.h"
#include "ImGui_Manager.h"
#include "Effect_Manager.h"

CState_MonsterAtk::CState_MonsterAtk()
{
}

HRESULT CState_MonsterAtk::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CState_MonsterAtk::EnterState()
{
	// 몬스터 타입 설정
	m_eMonsterType = (MONSTERTYPE)dynamic_cast<CMonster*>(m_pGameObject)->Get_MonsterType();

	// 몬스터 설정
	m_pMonster           = dynamic_cast<CMonster*>(m_pGameObject);
	m_pMonsterNavigation = static_cast<CNavigation*>(m_pGameObject->Get_Component(TEXT("Com_Navigation")));

	if (m_pMonsterNavigation == nullptr)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// 현재 레벨에 있는 Player 의 Transform 컴포넌트를 얻어옴
	m_pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_LevelIndex(), TEXT("Layer_Player"), TEXT("Com_Transform"), 0));

	// 애니메이션 설정
	switch (m_eMonsterType)
	{
	case MONSTERTYPE_SKULLSOLDIER:
		m_pMonster->Set_AnimAndReset(m_pModelCom, 13);
		break;
	case MONSTERTYPE_CROWSOLDIER:
		m_pMonster->Set_AnimAndReset(m_pModelCom, 18);
		break;
	case MONSTERTYPE_BOWMAN:
		m_pMonster->Set_AnimAndReset(m_pModelCom, 7);
		break;
	case MONSTERTYPE_CANNON:
		m_pMonster->Set_AnimAndReset(m_pModelCom, 1);
		break;
	case MONSTERTYPE_SHININGEGG:
		m_pMonster->Set_AnimAndReset(m_pModelCom, 7);
		break;
	case MONSTERTYPE_MONKEY:
		if (m_pTransformCom->Get_Distance(m_pPlayerTransform) > 7.f)
			m_iNextAnimation = 11;
		else
			m_iNextAnimation = 10;
		m_pMonster->Set_AnimAndReset(m_pModelCom, m_iNextAnimation);
		break;
	case MONSTERTYPE_MOWER:
		if (m_pTransformCom->Get_Distance(m_pPlayerTransform) > 7.f)
			m_iNextAnimation = 2;
		else
			m_iNextAnimation = 3;
		m_pMonster->Set_AnimAndReset(m_pModelCom, m_iNextAnimation);
		break;
	case MONSTERTYPE_NIGHTMARE:
		if (m_pTransformCom->Get_Distance(m_pPlayerTransform) > 10.f)
			m_iNextAnimation = 4;
		else
		{
			if (rand() % 2 == 0)
				m_iNextAnimation = 8;
			else
				m_iNextAnimation = 11;
		}
		m_pMonster->Set_AnimAndReset(m_pModelCom, m_iNextAnimation);
		break;
	case MONSTERTYPE_NOBODY:
		if (rand() % 2 == 0)
			m_iNextAnimation = 2; // 날아서 가야하니까 2번으로 설정
		else
			m_iNextAnimation = 5;
		m_pMonster->Set_AnimAndReset(m_pModelCom, m_iNextAnimation);
		break;
	case MONSTERTYPE_THINSOLDIER:
		m_iNextAnimation = rand() % 2 + 14;
		if (m_iNextAnimation == 14)
			m_pMonster->Set_AnimAndReset(m_pModelCom, 6);
		else
			m_pMonster->Set_AnimAndReset(m_pModelCom, m_iNextAnimation);
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CState_MonsterAtk::ExitState()
{
	switch (m_eMonsterType)
	{
	case MONSTERTYPE_THINSOLDIER:
		m_pGameObject->Set_Pass(0);
		break;
	}

	return S_OK;
}

_bool CState_MonsterAtk::Action(_double TimeDelta)
{
	_bool bStateEnd = false;

	// 몬스터 타입에 따라 Action 분기
	switch (m_eMonsterType)
	{
	case MONSTERTYPE_SKULLSOLDIER:
		if (m_pModelCom->Get_TargetTimeAcc(13) < 20.0)
			JustLook(TimeDelta);
		if (m_pModelCom->Get_TargetTimeAcc(13) >= 22.0)
		{
			if (m_bSoundEnd == false)
			{
				CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("SkullSoldier_Atk.wav"), 0.7f, true, m_pTransformCom->Get_Position());
				m_bSoundEnd = true;
			}

			m_pGameObject->Set_IsAttacking(true);
		}
		if (m_pModelCom->Get_TargetTimeAcc(13) >= 32.0)
			m_pGameObject->Set_IsAttacking(false);
		if (m_pModelCom->Get_TargetTimeAcc(13) >= 31.5 && m_bEffectEnd[0] == false)
		{
			CEffect_Manager::GetInstance()->Create_Landing(m_pTransformCom->Get_Position());
			m_bEffectEnd[0] = true;
		}
		bStateEnd = (m_pModelCom->Get_CurrFinished() && m_pTransformCom->SetHeightZero(TimeDelta, m_pMonsterNavigation));
		if (bStateEnd == true)
			m_pGameObject->Set_IsAttacking(false);
		break;
	case MONSTERTYPE_CROWSOLDIER:
		JustLook(TimeDelta);
		CrowSoldierAtk(TimeDelta);
		bStateEnd = AtkAcc(2, TimeDelta);
		break;
	case MONSTERTYPE_BOWMAN:
		JustLook(TimeDelta);
		bStateEnd = BowManAtk(TimeDelta);
		break;
	case MONSTERTYPE_CANNON:
		JustLook(TimeDelta);
		bStateEnd = CannonAtk(TimeDelta);
		break;
	case MONSTERTYPE_SHININGEGG:
		JustLook(TimeDelta);
		bStateEnd = (ShiningEggAtk(TimeDelta) && m_pModelCom->Get_CurrFinished());
		break;
	case MONSTERTYPE_MONKEY:
		JustLook(TimeDelta);
		bStateEnd = MonkeyAtk(TimeDelta);
		if (bStateEnd == true)
			m_pGameObject->Set_IsAttacking(false);
		break;
	case MONSTERTYPE_MOWER:
		JustLook(TimeDelta);
		bStateEnd = MowerAtk(TimeDelta);
		if (bStateEnd == true)
			m_pGameObject->Set_IsAttacking(false);
		break;
	case MONSTERTYPE_NIGHTMARE:
		bStateEnd = NightmareAtk(TimeDelta);
		if (bStateEnd == true)
		{
			m_pGameObject->Set_IsAttacking(false);
			m_pModelCom->Set_TempIndex(1);
		}
		break;
	case MONSTERTYPE_NOBODY:
		bStateEnd = NobodyAtk(TimeDelta);
		if (bStateEnd == true)
			m_pGameObject->Set_IsAttacking(false);
		break;
	case MONSTERTYPE_THINSOLDIER:
		bStateEnd = ThinSoldierAtk(TimeDelta);
		if (bStateEnd == true)
			m_pGameObject->Set_IsAttacking(false);
		break;
	default:
		break;
	}

	if (bStateEnd == true)
		return DELETE_STATE;

	return KEEP_STATE;
}

void CState_MonsterAtk::JustLook(_double TimeDelta)
{
	// Player 쪽으로 회전
	m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);
}

_bool CState_MonsterAtk::AtkAcc(_uint MaxAtkNum, _double TimeDelta)
{
	// 애니메이션이 끝나면 공격횟수를 누적시킴
	if (m_pModelCom->Get_CurrFinished())
	{
		++m_iAtkNum;
		if (m_iAtkNum > MaxAtkNum)
			return true;
	}

	return false;
}

_bool CState_MonsterAtk::Chase(_float fDistDiff, _double TimeDelta)
{
	// 플레이어쪽으로 회전
	m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);

	// 플레이어 한테 fDistDiff 거리만큼 다가가고 목표 거리에 도달하면 true 리턴
	return m_pTransformCom->Go_To_TargetXZ(m_pPlayerTransform, fDistDiff, TimeDelta, m_pMonsterNavigation);
}

void CState_MonsterAtk::CrowSoldierAtk(_double TimeDelta)
{
	// 18: 공중에서 발로 내려찍기 동작
	if (m_pModelCom->Get_CurrAnimIndex() == 18)
	{
		if (m_pModelCom->Get_TargetTimeAcc(18) >= 5.0 && m_bIsSkillEnd[0] == false)
		{
			CEffect_Manager::GetInstance()->Create_CrowAttack_LongDi_Wind(m_pTransformCom->Get_WorldFloat4x4_Pointer());
			m_bIsSkillEnd[0] = true;
		}

		if (m_pModelCom->Get_TargetTimeAcc(18) >= 20.0 && m_bIsSkillEnd[1] == false)
		{
			_vector vPos = m_pTransformCom->Get_Position() + __vector(0.f, 1.f, 0.f, 0.f);

			// 몬스터 스킬 생성
			CMonsterSkill::MONSTERSKILLDESC MonsterSkillDesc;
			MonsterSkillDesc.bNoModel = true;
			MonsterSkillDesc.vPos     = vPos;
			MonsterSkillDesc.vDir     = m_pTransformCom->Get_Dir(m_pPlayerTransform);
			MonsterSkillDesc.fSpeed   = 20.f;
			MonsterSkillDesc.fDamage  = 5.f;
			MonsterSkillDesc.Duration = 2.0f;
			MonsterSkillDesc.fSize    = 1.5f;

			CGameObject* pSkill = CGameInstance::GetInstance()->Add_GameObject_And_Return(CGameInstance::GetInstance()->Get_LevelIndex(), TEXT("Layer_MonsterSkill"), TEXT("Prototype_GameObject_MonsterSkill"), &MonsterSkillDesc);

			// 이펙트 생성
			CEffect_Manager::GetInstance()->Create_CrowAttack_LongDi_Moon(pSkill->Get_Transform(), pSkill->Get_DeadPointer());

			// 사운드
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("CrowSoldier_AtkMoon.wav"), 0.4f, true, m_pTransformCom->Get_Position());

			m_bIsSkillEnd[1] = true;
		}

		if (m_pModelCom->Check_CurAnim_And_Finished(18))
		{
			m_bIsSkillEnd[0] = false;
			m_bIsSkillEnd[1] = false;
		}
	}	
}

_bool CState_MonsterAtk::BowManAtk(_double TimeDelta)
{
	// 7:조준, 8:공격
	if (m_pModelCom->Check_CurAnim_And_Finished(7))
		m_pMonster->Set_AnimAndReset(m_pModelCom, 8);

	if (m_pModelCom->Get_CurrAnimIndex() == 8)
	{
		m_TimeAcc += TimeDelta;

		if (m_bIsSkillEnd[m_iAtkNum] == false)
		{
			_vector vPos = m_pTransformCom->Get_Position();
			vPos += XMVector4Normalize(m_pTransformCom->Get_Look()) * 0.5f + __vector(0.f, 0.8f, 0.f, 0.f);

			// 몬스터 스킬 생성
			CMonsterSkill::MONSTERSKILLDESC MonsterSkillDesc;
			lstrcpy(MonsterSkillDesc.ModelTag, TEXT("Prototype_Component_Model_Apple"));
			MonsterSkillDesc.ModelPrototypeLevel = LEVEL_STATIC;
			MonsterSkillDesc.vPos                = vPos;
			MonsterSkillDesc.vDir                = m_pTransformCom->Get_Dir(m_pPlayerTransform);
			MonsterSkillDesc.fSpeed              = 30.f; 
			MonsterSkillDesc.fDamage             = 5.f;
			MonsterSkillDesc.Duration            = 3.5f;
			MonsterSkillDesc.fSize               = 0.3f;

			CGameInstance::GetInstance()->Add_GameObject(CGameInstance::GetInstance()->Get_LevelIndex(), TEXT("Layer_MonsterSkill"), TEXT("Prototype_GameObject_MonsterSkill"), &MonsterSkillDesc);
			m_bIsSkillEnd[m_iAtkNum] = true;
		}
		
		if (m_iAtkNum == 0 && m_pModelCom->Get_CurrAnimIndex() == 8 && m_pModelCom->Check_ProgressRate(0.5f))
			m_iAtkNum = 1;

		if (m_TimeAcc >= 1.0)
			return true;
	}

	if (m_pModelCom->Check_CurAnim_And_Finished(8))
	{
		m_iAtkNum = 0;
		m_bIsSkillEnd[0] = false;
		m_bIsSkillEnd[1] = false;
		m_pMonster->Set_AnimAndReset(m_pModelCom, 7);
	}

	return false;
}

_bool CState_MonsterAtk::CannonAtk(_double TimeDelta)
{
	if (m_pModelCom->Check_CurAnim_And_Finished(1))
		m_pMonster->Set_AnimAndReset(m_pModelCom, 2);

	if (m_pModelCom->Get_CurrAnimIndex() == 2 && m_pModelCom->Check_ProgressRate(0.5f) && m_bIsSkillEnd[0] == false)
	{ 
		_vector vPos = m_pTransformCom->Get_Position();

		// 몬스터 스킬 생성
		CMonsterSkill::MONSTERSKILLDESC MonsterSkillDesc;
		lstrcpy(MonsterSkillDesc.ModelTag, TEXT("Prototype_Component_Model_Apple"));
		MonsterSkillDesc.ModelPrototypeLevel = LEVEL_STATIC;
		MonsterSkillDesc.vPos                = vPos;
		MonsterSkillDesc.vDir                = m_pTransformCom->Get_Dir(m_pPlayerTransform);
		MonsterSkillDesc.fSpeed              = 30.f;
		MonsterSkillDesc.fDamage             = 5.f;
		MonsterSkillDesc.Duration            = 3.5f;
		MonsterSkillDesc.fSize               = 0.3f;

		CGameInstance::GetInstance()->Add_GameObject(CGameInstance::GetInstance()->Get_LevelIndex(), TEXT("Layer_MonsterSkill"), TEXT("Prototype_GameObject_MonsterSkill"), &MonsterSkillDesc);
		m_bIsSkillEnd[0] = true;
	}

	if (m_pModelCom->Check_CurAnim_And_Finished(2))
		return true;

	return false;
}

_bool CState_MonsterAtk::MonkeyAtk(_double TimeDelta)
{
	if (m_iNextAnimation == 10)
	{
		if (m_pModelCom->Get_TargetTimeAcc(10) >= 12.0)
			m_pGameObject->Set_IsAttacking(true);

		_double effectTime[6] = { 12.0, 16.0, 20.0, 22.0, 25.0, 42.0 };

		for (_uint i = 0; i < 6; ++i)
		{
			if (m_pModelCom->Get_TargetTimeAcc(10) >= effectTime[i] && m_bEffectEnd[i] == false)
			{
				CEffect_Manager::GetInstance()->Create_Monkey_AttackSlash(m_pTransformCom->Get_WorldFloat4x4_Pointer());
				CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Monkey_AtkSlash.wav"), 1.f, true, m_pTransformCom->Get_Position());
				m_bEffectEnd[i] = true;
			}
		}

		return m_pModelCom->CheckFinished_Input(m_iNextAnimation);
	}
	else
	{
		if (m_pModelCom->Get_CurrAnimIndex() == 11 && m_pModelCom->Check_ProgressRate(0.75f) && m_bIsSkillEnd[0] == false)
		{
			_vector vPos = m_pTransformCom->Get_Position();
			vPos        += XMVector4Normalize(m_pTransformCom->Get_Look());
			vPos        += __vector(0.f, 0.5f, 0.f, 0.f);

			CMonsterSkill::MONSTERSKILLDESC MonsterSkillDesc;
			lstrcpy(MonsterSkillDesc.ModelTag, TEXT("Prototype_Component_Model_Banana"));
			MonsterSkillDesc.ModelPrototypeLevel = LEVEL_STATIC;
			MonsterSkillDesc.vPos                = vPos;
			MonsterSkillDesc.vDir                = m_pTransformCom->Get_Dir(m_pPlayerTransform);
			MonsterSkillDesc.fSpeed              = 15.f;
			MonsterSkillDesc.fDamage             = 5.f;
			MonsterSkillDesc.Duration            = 3.5f;
			MonsterSkillDesc.fSize               = 0.5f;

			MonsterSkillDesc.eSkillType = CMonsterSkill::MONSTERSKILLTYPE_BEZIERROTATION;
			MonsterSkillDesc.vDestPos   = m_pPlayerTransform->Get_Position();
			MonsterSkillDesc.vViaPos    = vPos + XMVector4Normalize(m_pTransformCom->Get_Look()) * 5.f + __vector(0.f, 5.f, 0.f, 0.f);

			CGameInstance::GetInstance()->Add_GameObject(CGameInstance::GetInstance()->Get_LevelIndex(), TEXT("Layer_MonsterSkill"), TEXT("Prototype_GameObject_MonsterSkill"), &MonsterSkillDesc);

			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Monkey_AtkBanana.wav"), 1.f, true, m_pTransformCom->Get_Position());

			m_bIsSkillEnd[0] = true;
		}

		if (m_pModelCom->CheckFinished_Input(m_iNextAnimation) == true)
		{
			m_bIsSkillEnd[0] = false;
			return true;
		}
	}

	return false;
}

_bool CState_MonsterAtk::MowerAtk(_double TimeDelta)
{
	if (m_iNextAnimation == 2) // 씨뿌리기
	{
		if (m_pModelCom->Get_TargetTimeAcc(2) >= 52.0 && m_bIsSkillEnd[0] == false)
		{
			// 스킬생성
			_vector vPos = m_pTransformCom->Get_Position() + __vector(0.f, 1.f, 0.f, 0.f);

			CMonsterSkill::MONSTERSKILLDESC MonsterSkillDesc;
			MonsterSkillDesc.bNoModel = true;
			MonsterSkillDesc.vPos     = vPos;
			MonsterSkillDesc.fSpeed   = 3.f;
			MonsterSkillDesc.fDamage  = 5.f;
			MonsterSkillDesc.Duration = 2.f;
			MonsterSkillDesc.fSize    = 0.3f;

			MonsterSkillDesc.eSkillType   = CMonsterSkill::MONSTERSKILLTYPE_SPREAD;
			MonsterSkillDesc.SpreadTime   = 0.0;
			MonsterSkillDesc.fOriginY     = XMVectorGetY(vPos);

			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Mower_AtkSeed.wav"), 0.6f, true, m_pTransformCom->Get_Position());
			for (_uint i = 0; i < 15; ++i)
			{
				MonsterSkillDesc.vDir         = XMVector4Normalize(__vector((_float)(rand() % 9 - 4.f), 0.f, (_float)(rand() % 9 - 4.f), 0.f));
				MonsterSkillDesc.fSpreadPower = (_float)(rand() % 3) + 5.f;

				CGameObject* pSkill = CGameInstance::GetInstance()->Add_GameObject_And_Return(CGameInstance::GetInstance()->Get_LevelIndex(), TEXT("Layer_MonsterSkill"), TEXT("Prototype_GameObject_MonsterSkill"), &MonsterSkillDesc);

				// 이펙트 생성
				CEffect_Manager::GetInstance()->Create_Poison(pSkill->Get_Transform()->Get_WorldFloat4x4_Pointer());
			}

			m_bIsSkillEnd[0] = true;
		}

		if (m_pModelCom->CheckFinished_Input(m_iNextAnimation) == true)
		{
			m_bIsSkillEnd[0] = false;
			return true;
		}
	}
	else if (m_iNextAnimation == 3) // 회전베기
	{
		if (m_pModelCom->Get_TargetTimeAcc(3) >= 30.0 && m_bEffectEnd[0] == false)
		{
			CEffect_Manager::GetInstance()->Create_BigTornado(m_pTransformCom->Get_Position());
			m_pGameObject->Set_IsAttacking(true);
			m_bEffectEnd[0] = true;
		}

		if (m_pModelCom->Get_TargetTimeAcc(3) >= 35.0 && m_bEffectEnd[1] == false) 
		{
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Mower_AtkTornado.wav"), 0.6f, true, m_pTransformCom->Get_Position());
			CEffect_Manager::GetInstance()->Create_BigTornado(m_pTransformCom->Get_Position());
			m_bEffectEnd[1] = true;
		}

		if (m_pModelCom->Get_TargetTimeAcc(3) >= 47.0)
			m_pGameObject->Set_IsAttacking(false);

		return m_pModelCom->CheckFinished_Input(m_iNextAnimation);
	}

	return false;
}

_bool CState_MonsterAtk::NightmareAtk(_double TimeDelta)
{
	if (m_iNextAnimation == 4) // 4: 날아가기 공격 준비
	{
		if (m_pModelCom->Get_CurrAnimIndex() == 4)
		{
			m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);
		}

		if (m_pModelCom->Check_CurAnim_And_Finished(4))
		{
			m_pModelCom->Set_TempIndex(5); // 5: 날아가기
			m_pMonster->Set_MotionTrail(true);
		}

		if (m_pModelCom->Get_CurrAnimIndex() == 5)
		{
			if (Chase(10.f, TimeDelta * 5.f) == true)
			{
				m_pMonster->Set_AnimAndReset(m_pModelCom, 6); // 6: 팽이 공격
				m_pMonster->Set_MotionTrail(false);
			}
		}

		if (m_pModelCom->Get_CurrAnimIndex() == 6 && m_pModelCom->Get_TargetTimeAcc(6) <= 27.0)
		{
			m_pTransformCom->Go_Straight(TimeDelta, m_pMonsterNavigation); 

			if (m_pModelCom->Get_TargetTimeAcc(6) >= 8.0 && m_bEffectEnd[0] == false)
			{
				CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Nightmare_AtkRotation.wav"), 1.f, true, m_pTransformCom->Get_Position());
				CEffect_Manager::GetInstance()->Create_NightmareRotationAtk(m_pTransformCom);
				m_bEffectEnd[0] = true;
			}
		}

		if (m_pModelCom->Get_CurrAnimIndex() == 6 && m_pModelCom->Check_ProgressRate(0.2f))
			m_pGameObject->Set_IsAttacking(true);

		if (m_pModelCom->Check_CurAnim_And_Finished(6))
			m_pMonster->Set_AnimAndReset(m_pModelCom, 3); // 3: 점프 동작 마무리

		if (m_pModelCom->Get_CurrAnimIndex() == 3)
		{
			m_pGameObject->Set_IsAttacking(false);
			m_pTransformCom->SetHeightZero(TimeDelta, m_pMonsterNavigation);
		}

		if (m_pModelCom->Check_CurAnim_And_Finished(3))
			return true;
	}
	else if (m_iNextAnimation == 8) // 공중 발차기
	{
		if (m_pModelCom->Get_CurrAnimIndex() == 8)
		{
			if (m_bSoundEnd == false)
			{
				CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Nightmare_AtkJump.wav"), 1.f, true, m_pTransformCom->Get_Position());
				m_bSoundEnd = true;
			}

			m_pModelCom->Set_CurrPlaySpeed(40.0);
			m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta * 2.0);
			m_pTransformCom->Go_Up(TimeDelta * 0.5, m_pMonsterNavigation);
			m_pTransformCom->Go_Backward(TimeDelta * 0.4, m_pMonsterNavigation);
		}

		if (m_pModelCom->Check_CurAnim_And_Finished(8))
		{
			m_pGameObject->Set_IsAttacking(true);
			m_pMonster->Set_AnimAndReset(m_pModelCom, 10); // 발차기
			CEffect_Manager::GetInstance()->Create_NightmareFlyKick_Before(m_pTransformCom);
			CEffect_Manager::GetInstance()->Create_NightmareBlade(m_pTransformCom);
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Nightmare_AtkKickAfterJump.wav"), 1.f, true, m_pTransformCom->Get_Position());
		}

		if (m_pModelCom->Get_CurrAnimIndex() == 10)
		{
			_vector vTarget = m_pPlayerTransform->Get_Position();

			if (m_pModelCom->Get_TargetTimeAcc(10) <= 20.0)
			{
				m_pModelCom->Set_CurrPlaySpeed(40.0);
				m_pTransformCom->Go_Down(TimeDelta * 0.5, m_pMonsterNavigation);
				m_pTransformCom->Go_Straight(TimeDelta, m_pMonsterNavigation);
			}
		}

		if (m_pModelCom->Check_CurAnim_And_Finished(10))
		{
			m_pMonster->Set_AnimAndReset(m_pModelCom, 3); // 점프 동작
			m_pGameObject->Set_IsAttacking(false);
		}

		if (m_pModelCom->Get_CurrAnimIndex() == 3)
		{
			m_pTransformCom->SetHeightZero(TimeDelta, m_pMonsterNavigation);
		}

		if (m_pModelCom->Check_CurAnim_And_Finished(3))
			return true;
	}
	else if (m_iNextAnimation == 11) // 평지에서 연속공격
	{
		m_pGameObject->Set_IsAttacking(false);

		if (m_pModelCom->Get_TargetTimeAcc(11) >= 18.0 && m_bEffectEnd[0] == false)
		{
			CEffect_Manager::GetInstance()->Create_NightmareBeforeNails_Rings(m_pTransformCom, 0);
			m_bEffectEnd[0] = true;
		}

		if (m_pModelCom->Get_TargetTimeAcc(11) >= 25.0 && m_bEffectEnd[1] == false)
		{
			CEffect_Manager::GetInstance()->Create_NightmareBeforeNails_Rings(m_pTransformCom, 1);
			m_bEffectEnd[1] = true;
		}

		if (m_pModelCom->Get_TargetTimeAcc(11) >= 32.0 && m_bEffectEnd[2] == false)
		{
			CEffect_Manager::GetInstance()->Create_NightmareBeforeNails_Rings(m_pTransformCom, 2);
			m_bEffectEnd[2] = true;
		}

		if (m_pModelCom->Get_TargetTimeAcc(11) >= 34.0 && m_bIsSkillEnd[0] == false)
		{
			// 몬스터 스킬 생성
			CMonsterSkill::MONSTERSKILLDESC MonsterSkillDesc;
			MonsterSkillDesc.bNoModel = true;
			MonsterSkillDesc.vPos     = m_pTransformCom->Get_Position() + __vector(0.f, 2.f, 0.f, 0.f);
			MonsterSkillDesc.vDir     = XMVector4Normalize(m_pTransformCom->Get_Look());
			MonsterSkillDesc.fSpeed   = 30.f;
			MonsterSkillDesc.fDamage  = 5.f;
			MonsterSkillDesc.Duration = 1.5f;
			MonsterSkillDesc.fSize    = 3.f;

			CGameObject* pSkill = CGameInstance::GetInstance()->Add_GameObject_And_Return(CGameInstance::GetInstance()->Get_LevelIndex(), TEXT("Layer_MonsterSkill"), TEXT("Prototype_GameObject_MonsterSkill"), &MonsterSkillDesc);

			CEffect_Manager::GetInstance()->Create_NightmareNails_New(pSkill->Get_Transform(), pSkill->Get_DeadPointer(), 6);
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Nightmare_AtkNail.wav"), 1.f, true, m_pTransformCom->Get_Position());

			CEffect_Manager::GetInstance()->Create_NightmareNailTrail(m_pTransformCom, 0);

			m_bIsSkillEnd[0] = true;
		}

		if (m_pModelCom->Get_TargetTimeAcc(11) >= 43.0 && m_bIsSkillEnd[1] == false)
		{
			// 몬스터 스킬 생성
			CMonsterSkill::MONSTERSKILLDESC MonsterSkillDesc;
			MonsterSkillDesc.bNoModel = true;
			MonsterSkillDesc.vPos     = m_pTransformCom->Get_Position() + __vector(0.f, 2.f, 0.f, 0.f);
			MonsterSkillDesc.vDir     = XMVector4Normalize(m_pTransformCom->Get_Look());
			MonsterSkillDesc.fSpeed   = 30.f;
			MonsterSkillDesc.fDamage  = 5.f;
			MonsterSkillDesc.Duration = 1.5f;
			MonsterSkillDesc.fSize    = 3.f;

			CGameObject* pSkill = CGameInstance::GetInstance()->Add_GameObject_And_Return(CGameInstance::GetInstance()->Get_LevelIndex(), TEXT("Layer_MonsterSkill"), TEXT("Prototype_GameObject_MonsterSkill"), &MonsterSkillDesc);

			CEffect_Manager::GetInstance()->Create_NightmareNails_New(pSkill->Get_Transform(), pSkill->Get_DeadPointer(), 5);

			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Nightmare_AtkNail.wav"), 1.f, true, m_pTransformCom->Get_Position());

			CEffect_Manager::GetInstance()->Create_NightmareNailTrail(m_pTransformCom, 1);

			m_bIsSkillEnd[1] = true;
		}

		if (m_pModelCom->Get_TargetTimeAcc(11) <= 62.0)
			m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);

		if (m_pModelCom->Get_TargetTimeAcc(11) > 62.0 && m_pModelCom->Get_TargetTimeAcc(11) < 72.0)
		{
			if (m_bIsSkillEnd[2] == false)
			{
				CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Nightmare_AtkKick.wav"), 1.f, true, m_pTransformCom->Get_Position());
				//CEffect_Manager::GetInstance()->Create_NightmareRush(m_pTransformCom);

				// 몬스터 스킬 생성
				CMonsterSkill::MONSTERSKILLDESC MonsterSkillDesc;
				MonsterSkillDesc.eSkillType       = CMonsterSkill::MONSTERSKILLTYPE_SHURIKEN;
				MonsterSkillDesc.bNoModel         = true;
				MonsterSkillDesc.vPos             = m_pTransformCom->Get_Position() + XMVector4Normalize(m_pTransformCom->Get_Right()) * 3.f;
				MonsterSkillDesc.vDir             = XMVector4Normalize(m_pTransformCom->Get_Look());
				MonsterSkillDesc.fSpeed           = 50.f;
				MonsterSkillDesc.fDamage          = 5.f;
				MonsterSkillDesc.Duration         = 5.f;
				MonsterSkillDesc.fSize            = 2.f; 
				MonsterSkillDesc.PauseTime        = 1.0;
				MonsterSkillDesc.pPlayerTransform = m_pPlayerTransform;

				CGameObject* pSkill = CGameInstance::GetInstance()->Add_GameObject_And_Return(CGameInstance::GetInstance()->Get_LevelIndex(), TEXT("Layer_MonsterSkill"), TEXT("Prototype_GameObject_MonsterSkill"), &MonsterSkillDesc);
				CEffect_Manager::GetInstance()->Create_NightmareShuriken(pSkill->Get_Transform(), pSkill->Get_DeadPointer(), true);

				// 몬스터 스킬 생성
				MonsterSkillDesc;
				MonsterSkillDesc.eSkillType       = CMonsterSkill::MONSTERSKILLTYPE_SHURIKEN;
				MonsterSkillDesc.bNoModel         = true;
				MonsterSkillDesc.vPos             = m_pTransformCom->Get_Position() + XMVector4Normalize(m_pTransformCom->Get_Right()) * -3.f;
				MonsterSkillDesc.vDir             = XMVector4Normalize(m_pTransformCom->Get_Look());
				MonsterSkillDesc.fSpeed           = 50.f;
				MonsterSkillDesc.fDamage          = 5.f;
				MonsterSkillDesc.Duration         = 5.f;
				MonsterSkillDesc.fSize            = 2.f;
				MonsterSkillDesc.PauseTime        = 1.5;
				MonsterSkillDesc.pPlayerTransform = m_pPlayerTransform;

				pSkill = CGameInstance::GetInstance()->Add_GameObject_And_Return(CGameInstance::GetInstance()->Get_LevelIndex(), TEXT("Layer_MonsterSkill"), TEXT("Prototype_GameObject_MonsterSkill"), &MonsterSkillDesc);
				CEffect_Manager::GetInstance()->Create_NightmareShuriken(pSkill->Get_Transform(), pSkill->Get_DeadPointer());

				m_bIsSkillEnd[2] = true;
			}

			_vector vTargetPos = m_pPlayerTransform->Get_Position();

			//m_pTransformCom->Go_Straight(TimeDelta * 1.5, m_pMonsterNavigation);
		}

		if (m_pModelCom->Check_CurAnim_And_Finished(11))
		{
			m_pMonster->Set_AnimAndReset(m_pModelCom, 3); // 점프 동작
		}

		if (m_pModelCom->Get_CurrAnimIndex() == 3)
		{
			m_pTransformCom->SetHeightZero(TimeDelta, m_pMonsterNavigation);
		}			

		if (m_pModelCom->Check_CurAnim_And_Finished(3))
			return true;
	}

	return false;
}

_bool CState_MonsterAtk::ShiningEggAtk(_double TimeDelta)
{
	if (m_pModelCom->Get_TargetTimeAcc(7) >= 20.0 && m_bEffectEnd[0] == false)
	{
		_vector vPos = m_pTransformCom->Get_Position();
		vPos += XMVector4Normalize(m_pTransformCom->Get_Look()) * 0.5f;

		// 몬스터 스킬 생성
		CMonsterSkill::MONSTERSKILLDESC MonsterSkillDesc;
		MonsterSkillDesc.eSkillType   = CMonsterSkill::MONSTERSKILLTYPE_SPIRAL;
		MonsterSkillDesc.bNoModel     = true;
		MonsterSkillDesc.vPos         = vPos + XMVector4Normalize(m_pTransformCom->Get_Look()) * 0.5f;
		MonsterSkillDesc.vDir         = m_pTransformCom->Get_Dir(m_pPlayerTransform);
		MonsterSkillDesc.fSpeed       = 3.f;
		MonsterSkillDesc.fDamage      = 5.f;
		MonsterSkillDesc.Duration     = 1.5f;
		MonsterSkillDesc.fSize        = 1.f;
		MonsterSkillDesc.vColliderPos = _float4(0.f, 1.f, 0.f, 1.f);

		CGameObject* pSkill = CGameInstance::GetInstance()->Add_GameObject_And_Return(CGameInstance::GetInstance()->Get_LevelIndex(), TEXT("Layer_MonsterSkill"), TEXT("Prototype_GameObject_MonsterSkill"), &MonsterSkillDesc);
		
		CEffect_Manager::GetInstance()->Create_MagicGrass(m_pTransformCom);
		CEffect_Manager::GetInstance()->Create_Tornado(pSkill->Get_Transform()->Get_WorldFloat4x4_Pointer(), pSkill->Get_DeadPointer());
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("ShiningEgg_Tornado.wav"), 0.6f, true, m_pTransformCom->Get_Position());
		m_bEffectEnd[0] = true;
	}

	return true;
}

_bool CState_MonsterAtk::NobodyAtk(_double TimeDelta)
{
	// 2, 3, 4	날아가기
	// 8, 9		박치기공격
	if (m_iNextAnimation == 2) // 박치기 공격하러 날아가는 모션 시작
	{
		if (m_pModelCom->Get_CurrAnimIndex() == 2)
			m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta * 2.5);

		if (m_pModelCom->Check_CurAnim_And_Finished(2))
			m_pModelCom->Set_TempIndex(3);

		if (m_pModelCom->Get_CurrAnimIndex() == 3)
		{
			_vector vTargetPos    = m_pPlayerTransform->Get_Position();
			_float	fTargetHeight = XMVectorGetY(vTargetPos) - 1.5f;

			m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta * 2.0);

			if (m_pTransformCom->Go_To_TargetXZ_SetHeight(vTargetPos, 10.f, fTargetHeight, 0.5, TimeDelta) == true)
			{
				m_pGameObject->Set_IsAttacking(true);
				CEffect_Manager::GetInstance()->Create_NobodyChargeRush(m_pTransformCom);
				m_pMonster->Set_AnimAndReset(m_pModelCom, 8);
			}
		}

		if (m_pModelCom->Check_CurAnim_And_Finished(8))
		{
			m_pMonster->Set_AnimAndReset(m_pModelCom, 9);
			CEffect_Manager::GetInstance()->Create_NobodyRush(m_pTransformCom);
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Nobody_AtkHeadButt.wav"), 1.f, true, m_pTransformCom->Get_Position());
		}

		if (m_pModelCom->Get_CurrAnimIndex() == 9) 
		{
			if (m_pModelCom->Get_TargetTimeAcc(9) <= 5.0)
			{
				m_pTransformCom->Go_Straight(TimeDelta * 1.5);
			}
			else if (m_bEffectEnd[0] == false)
			{
				m_pGameObject->Set_IsAttacking(false);
				m_bEffectEnd[0] = true;
			}
		}

		if (m_pModelCom->Check_CurAnim_And_Finished(9))
			return true;
	}

	// 5, 6, 7	회전공격
	if (m_iNextAnimation == 5)
	{
		if (m_pModelCom->Get_CurrAnimIndex() == 5)
		{
			if (vecy(m_pTransformCom->Get_Position()) <= 0.5f)
				m_pTransformCom->Go_Up(TimeDelta * 0.1);
		}

		if (m_pModelCom->Check_CurAnim_And_Finished(5))
		{
			m_pGameObject->Set_IsAttacking(true);
			m_pModelCom->Set_TempIndex(6);
		}

		if (m_pModelCom->Get_CurrAnimIndex() == 6)
		{
			if (m_bEffectEnd[0] == false)
			{
				CEffect_Manager::GetInstance()->Create_NobodyRotationAtk(m_pTransformCom);
				CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Nobody_AtkRotation.wav"), 0.9f, true, m_pTransformCom->Get_Position());
				m_bEffectEnd[0] = true;
			}

			m_TimeAcc += TimeDelta;

			if (m_TimeAcc >= 2.5 && m_bSoundEnd == false)
			{
				CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Nobody_AtkRotationEnd.wav"), 0.9f, true, m_pTransformCom->Get_Position());
				m_bSoundEnd = true;
			}

			_vector vTargetPos    = m_pPlayerTransform->Get_Position();
			_float	fTargetHeight = vecy(vTargetPos) - 1.f;

			m_pTransformCom->Go_To_TargetXZ(vTargetPos, 3.f, TimeDelta * 0.45, m_pMonsterNavigation); 

			if (m_bChaseEnd == false)
				m_bChaseEnd = m_pTransformCom->SetHeight(TimeDelta * 0.15, fTargetHeight);

			m_pTransformCom->Turn(m_pTransformCom->Get_Up(), TimeDelta * 10.0);

			if (m_TimeAcc >= 2.5)
			{
				m_pMonster->Set_AnimAndReset(m_pModelCom, 7);
			}
		}

		// 마무리 동작
		if (m_pModelCom->Get_CurrAnimIndex() == 7 && m_pModelCom->Get_TargetTimeAcc(7) >= 20.0)
		{
			m_pGameObject->Set_IsAttacking(false);
			m_pModelCom->Set_CurrPlaySpeed(40.0);
		} 

		if (m_pModelCom->Check_CurAnim_And_Finished(7))
			return true;
	}

	return false;
}

_bool CState_MonsterAtk::ThinSoldierAtk(_double TimeDelta)
{
	if (m_iNextAnimation == 14) // 손톱공격
	{
		if (m_pModelCom->Get_CurrAnimIndex() != 14)
		{
			_bool bTurn = m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta * 5.f);

			if (m_bChaseEnd == false)
				m_bChaseEnd = m_pTransformCom->Go_To_TargetXZ(m_pPlayerTransform, 5.f, TimeDelta, m_pMonsterNavigation);

			if (bTurn && m_bChaseEnd)
				m_pMonster->Set_AnimAndReset(m_pModelCom, 14);
		}

		if (m_pModelCom->Get_CurrAnimIndex() == 14 && m_pModelCom->Get_TargetTimeAcc(14) < 9.0)
		{
			m_pGameObject->Set_IsAttacking(true);
			m_pModelCom->Set_TargetTimeAcc(14, 9.0);
		}

		if (m_pModelCom->Get_TargetTimeAcc(14) >= 13.0 && m_pModelCom->Get_TargetTimeAcc(14) <= 16.0)
			m_pTransformCom->Go_Straight(TimeDelta * 3.0, m_pMonsterNavigation);

		if (m_pModelCom->Get_TargetTimeAcc(14) >= 18.0 && m_bEffectEnd[0] == false)
		{
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("ThinSoldier_AtkSmallSlash.wav"), 1.f, true, m_pTransformCom->Get_Position());
			CEffect_Manager::GetInstance()->Create_AttackSlash_Short(m_pTransformCom->Get_WorldFloat4x4_Pointer(), m_pGameObject->Get_DeadPointer());
			m_bEffectEnd[0] = true;
		}

		if (m_pModelCom->Get_TargetTimeAcc(14) >= 23.0 && m_bEffectEnd[1] == false)
		{
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("ThinSoldier_AtkSmallSlash.wav"), 1.f, true, m_pTransformCom->Get_Position());
			CEffect_Manager::GetInstance()->Create_AttackSlash_Short(m_pTransformCom->Get_WorldFloat4x4_Pointer(), m_pGameObject->Get_DeadPointer());
			m_bEffectEnd[1] = true;
		}

		if (m_pModelCom->Get_TargetTimeAcc(14) >= 25.0)
		{
			m_pGameObject->Set_IsAttacking(false);
			return true;
		}
	}
	else
	{
		_bool bZeroHeight =	m_pTransformCom->SetHeightZero(TimeDelta, m_pMonsterNavigation);

		if (m_pModelCom->Check_CurAnim_And_Finished(15))
		{
			CEffect_Manager::GetInstance()->Create_Teleport(m_pTransformCom->Get_Position());
			m_pMonster->Set_AnimAndReset(m_pModelCom, 16);

			_vector vOriginPos = m_pTransformCom->Get_Position();
			_vector vTargetPos;
			_bool	bTeleport = false;

			// 오른쪽 앞
			if (bTeleport == false)
			{
				vTargetPos = m_pPlayerTransform->Get_Position() + m_pPlayerTransform->Get_Look() * 2.f + m_pPlayerTransform->Get_Right() * 2.f;
				if (m_pMonsterNavigation->Move_OnNavigation(vTargetPos, vOriginPos))
				{
					m_pTransformCom->Set_Position(vTargetPos);
					m_pMonsterNavigation->Find_Cell(m_pTransformCom->Get_Position());
					bTeleport = true;
				}
			}

			// 왼쪽 앞
			if (bTeleport == false)
			{
				vTargetPos = m_pPlayerTransform->Get_Position() + m_pPlayerTransform->Get_Look() * 2.f - m_pPlayerTransform->Get_Right() * 2.f;
				if (m_pMonsterNavigation->Move_OnNavigation(vTargetPos, vOriginPos))
				{
					m_pTransformCom->Set_Position(vTargetPos);
					m_pMonsterNavigation->Find_Cell(m_pTransformCom->Get_Position());
					bTeleport = true;
				}
			}

			// 오른쪽 뒤
			if (bTeleport == false)
			{
				vTargetPos = m_pPlayerTransform->Get_Position() - m_pPlayerTransform->Get_Look() * 2.f + m_pPlayerTransform->Get_Right() * 2.f;
				if (m_pMonsterNavigation->Move_OnNavigation(vTargetPos, vOriginPos))
				{
					m_pTransformCom->Set_Position(vTargetPos);
					m_pMonsterNavigation->Find_Cell(m_pTransformCom->Get_Position());
					bTeleport = true;
				}
			}

			// 왼쪽 뒤
			if (bTeleport == false)
			{
				vTargetPos = m_pPlayerTransform->Get_Position() - m_pPlayerTransform->Get_Look() * 2.f - m_pPlayerTransform->Get_Right() * 2.f;
				if (m_pMonsterNavigation->Move_OnNavigation(vTargetPos, vOriginPos))
				{
					m_pTransformCom->Set_Position(vTargetPos);
					m_pMonsterNavigation->Find_Cell(m_pTransformCom->Get_Position());
					bTeleport = true;
				}
			}
			
			if (bTeleport == true)
				m_pMonsterNavigation->Find_Cell(m_pTransformCom->Get_Position());

			m_pTransformCom->LookAt(m_pPlayerTransform->Get_Position());
			m_pGameObject->Set_Pass(10);
			CEffect_Manager::GetInstance()->Create_Teleport(m_pTransformCom->Get_Position());
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("ThinSoldier_Teleport.wav"), 1.f, true, m_pTransformCom->Get_Position());
		}

		if (m_pModelCom->Check_CurAnim_And_Finished(16))
		{
			m_pGameObject->Set_IsAttacking(true);
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("ThinSoldier_AtkBigSlash.wav"), 1.f, true, m_pTransformCom->Get_Position());
			m_pMonster->Set_AnimAndReset(m_pModelCom, 17);
		}

		if (m_pModelCom->Get_TargetTimeAcc(17) >= 3.0 && m_bEffectEnd[0] == false)
		{
			CEffect_Manager::GetInstance()->Create_AttackSlash(m_pTransformCom->Get_WorldFloat4x4_Pointer(), m_pGameObject->Get_DeadPointer());
			m_bEffectEnd[0] = true;
		}

		if (m_pModelCom->Get_TargetTimeAcc(17) >= 10.0)
			m_pGameObject->Set_IsAttacking(false);

		if (m_pModelCom->Get_TargetTimeAcc(17) >= 15.0)
			m_pGameObject->Set_Pass(0);

		if (m_pModelCom->Check_CurAnim_And_Finished(17) && bZeroHeight)
			return true;
	}

	return false;
}

CState_MonsterAtk* CState_MonsterAtk::Create(void* pArg)
{
	CState_MonsterAtk* pInstance = new CState_MonsterAtk;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

void CState_MonsterAtk::Free()
{
	__super::Free();
}