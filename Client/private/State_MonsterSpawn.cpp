#include "stdafx.h"
#include "State_MonsterSpawn.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "Monster.h"
#include "Effect_Manager.h"
#include "Terrain.h"
#include "Goat.h"

CState_MonsterSpawn::CState_MonsterSpawn()
{
}

HRESULT CState_MonsterSpawn::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CState_MonsterSpawn::EnterState()
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
		m_pMonster->Set_AnimAndReset(m_pModelCom, 0);
		pGameInstance->LetsPlaySound_Monster(TEXT("SkullSoldier_Spawn.wav"), 0.6f, true, m_pTransformCom->Get_Position());
		break;
	case MONSTERTYPE_CROWSOLDIER:
		m_pMonster->Set_AnimAndReset(m_pModelCom, 0);
		pGameInstance->LetsPlaySound_Monster(TEXT("CrowSoldier_Spawn.wav"), 0.6f, true, m_pTransformCom->Get_Position());
		break;
	case MONSTERTYPE_BOWMAN:
		m_pMonster->Set_AnimAndReset(m_pModelCom, 0);
		break;
	case MONSTERTYPE_SHININGEGG:
		m_pMonster->Set_AnimAndReset(m_pModelCom, 0);
		CEffect_Manager::GetInstance()->Create_ShineParticle(m_pTransformCom, __vector(0.f, 1.f, 0.f, 0.f));
		pGameInstance->LetsPlaySound_Monster(TEXT("ShiningEgg_Spawn.wav"), 0.3f, true, m_pTransformCom->Get_Position());
		break;
	case MONSTERTYPE_MONKEY:
		m_pMonster->Set_AnimAndReset(m_pModelCom, 0);
		pGameInstance->LetsPlaySound_Monster(TEXT("Monkey_Spawn.wav"), 0.6f, true, m_pTransformCom->Get_Position());
		break;
	case MONSTERTYPE_BIGTREE:
		// 터레인 설정
		m_pRapunzelBossTerrain = static_cast<CTerrain*>(pGameInstance->Get_Front(LEVEL_RAPUNZEL_BOSS, TEXT("Layer_Terrain")));
		m_pMonster->Set_AnimAndReset(m_pModelCom, 13);
		break;
	case MONSTERTYPE_THINSOLDIER:
		m_pMonster->Set_AnimAndReset(m_pModelCom, 16);
		pGameInstance->LetsPlaySound_Monster(TEXT("ThinSoldier_Spawn.wav"), 0.6f, true, m_pTransformCom->Get_Position());
		break;
	case MONSTERTYPE_MOWER:
		m_pMonster->Set_AnimAndReset(m_pModelCom, 0);
		break;
	case MONSTERTYPE_NIGHTMARE:
		m_pMonster->Set_AnimAndReset(m_pModelCom, 0);
		break;
	case MONSTERTYPE_NOBODY:
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Flower_Open.wav"), 0.5f, true, m_pTransformCom->Get_Position());
		m_pMonster->Set_AnimAndReset(m_pModelCom, 0);
		break;
	case MONSTERTYPE_ICEMAN:
		m_pModelCom->Set_TempIndex(25);
		m_iSoundChannel = CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("IceMan_Snoring.wav"), 1.0f, true, m_pTransformCom->Get_Position());
		m_pTransformCom->LookAt(m_pPlayerTransform->Get_Position());
		break;
	case MONSTERTYPE_FROZENWOLF:
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("DarkWolf_Howl.wav"), 0.5f, true, m_pTransformCom->Get_Position());
		m_pMonster->Set_AnimAndReset(m_pModelCom, 38);
		break;
	case MONSTERTYPE_GOAT:
		m_pMonster->Set_AnimAndReset(m_pModelCom, 0);
		m_pTransformCom->LookAtXZ(m_pPlayerTransform);
		m_pModelCom->Set_TargetPlaySpeed_Rate(0, 0.8);
		m_pModelCom->Set_TargetPlaySpeed_Rate(40, 0.8);
		static_cast<CGoat*>(m_pMonster)->CreateWeapon();
		break;
	default:
		break;
	}

	return S_OK;
}

_bool CState_MonsterSpawn::Action(_double TimeDelta)
{
	_bool bStateEnd = false;

	// 몬스터 타입에 따라 Action 분기
	switch (m_eMonsterType)
	{
	case MONSTERTYPE_SKULLSOLDIER:
		if (m_bEffect[0] == false)
		{
			CEffect_Manager::GetInstance()->Create_AppearAction(m_pTransformCom->Get_Position(), CEffect_Manager::SKULL);
			m_bEffect[0] = true;
		}
		m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);
		bStateEnd = m_pModelCom->Get_CurrFinished();
		break;
	case MONSTERTYPE_CROWSOLDIER:
		if (m_bEffect[0] == false)
		{
			CEffect_Manager::GetInstance()->Create_AppearAction(m_pTransformCom->Get_Position(), CEffect_Manager::CROW);
			m_bEffect[0] = true;
		}
		m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);
		bStateEnd = m_pModelCom->Get_CurrFinished();
		break;
	case MONSTERTYPE_BOWMAN:
		m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);
		bStateEnd = m_pModelCom->Get_CurrFinished();
		break;
	case MONSTERTYPE_THINSOLDIER:
		m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);
		bStateEnd = ThinSoldierSpawn(TimeDelta);
		break;
	case MONSTERTYPE_SHININGEGG:
		if (m_bEffect[0] == false)
		{
			CEffect_Manager::GetInstance()->Create_AppearAction(m_pTransformCom->Get_Position(), CEffect_Manager::SHININGEGG);
			m_bEffect[0] = true;
		}
		m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);
		bStateEnd = m_pModelCom->Get_CurrFinished();
		break;
	case MONSTERTYPE_MONKEY:
		if (m_bEffect[0] == false)
		{
			CEffect_Manager::GetInstance()->Create_AppearAction(m_pTransformCom->Get_Position(), CEffect_Manager::MONKEY);
			m_bEffect[0] = true;
		}
		m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);
		bStateEnd = m_pModelCom->Get_CurrFinished();
		break;
	case MONSTERTYPE_BIGTREE:
		if (m_bEffect[0] == false)
		{
			CEffect_Manager::GetInstance()->Create_BigTreeSpawn(m_pTransformCom);
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("BigTree_SpawnEffectSound.wav"), 1.0f);
			m_bEffect[0] = true;
		}

		if (m_pModelCom->Get_TargetTimeAcc(13) >= 65.0 && m_bEffect[1] == false)
		{
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("BigTree_SpawnRoar.wav"), 0.4f);
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("BigTree_SpawnHowling.wav"), 0.4f);
			CEffect_Manager::GetInstance()->Create_BigTreeHowling(m_pTransformCom);
			m_bEffect[1] = true;
		}
		m_pTransformCom->LookAtXZ(m_pPlayerTransform);
		bStateEnd = (m_pTransformCom->SetHeight(TimeDelta * 0.5, vecy(m_pRapunzelBossTerrain->Compute_PositionOnTerrain(m_pTransformCom->Get_Position()))) && m_pModelCom->Get_CurrFinished());
		break;
	case MONSTERTYPE_MOWER:
		m_pTransformCom->SetHeightNavi(TimeDelta, m_pMonsterNavigation);
		bStateEnd = m_pModelCom->Get_CurrFinished();
		break;
	case MONSTERTYPE_NIGHTMARE:		
		m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);
		bStateEnd = NightmareSpawn(TimeDelta);
		break;
	case MONSTERTYPE_NOBODY:
		if (m_pModelCom->Get_TargetTimeAcc(0) >= 5.0 && m_bSoundEnd == false)
		{
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Nobody_Spawn.wav"), 0.5f, true, m_pTransformCom->Get_Position());
			m_bSoundEnd = true;
		}
		bStateEnd = (NobodySpawn(TimeDelta) && m_pModelCom->Get_CurrFinished());
		break;
	case MONSTERTYPE_ICEMAN:
		if (m_pModelCom->Check_CurAnim_And_Finished(25))
		{
			CGameInstance::GetInstance()->StopMonsterSound(m_iSoundChannel);
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("IceMan_5_Awake.wav"), 1.0f, true, m_pTransformCom->Get_Position());
		}

		if (m_pModelCom->Get_TargetTimeAcc(1) >= 20.0 && m_bEffect[0] == false)
		{
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("IceMan_Spawn_23.wav"), 0.7f, true, m_pTransformCom->Get_Position());

			CEffect_Manager::GetInstance()->Create_IceManRoar(m_pTransformCom);
			m_bEffect[0] = true;
		}
		bStateEnd = m_pMonster->PlayAnimations(m_pModelCom, vector<_uint>{ 25, 26, 1 });
		break;
	case MONSTERTYPE_FROZENWOLF:
		if (m_bEffect[0] == false && m_pModelCom->Get_TargetTimeAcc(38) >= 25.0)
		{
			GM->Use_RadialBlur(true, 1.2f, 2.f, 1.5f, 1.f);
			m_bEffect[0] = true;
		}
		bStateEnd = m_pMonster->PlayAnimations(m_pModelCom, vector<_uint>{38});
		if (m_pModelCom->Get_CurrTimeAcc() >= 25.0 && m_pModelCom->Get_CurrTimeAcc() <= 40.0)
			m_pModelCom->Set_CurrPlaySpeed(10.0);
		else
			m_pModelCom->Set_CurrPlaySpeed(50.0);

		if (m_bEffect[1] == false && m_pModelCom->Get_TargetTimeAcc(38) >= 73.0)
		{
			CEffect_Manager::GetInstance()->Create_Wolf_SpawnEffect(m_pTransformCom);
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("DarkWolf_Summon.wav"), 0.5f);
			m_bEffect[1] = true;
		}

		break;
	case MONSTERTYPE_GOAT:
		m_TimeAcc += TimeDelta;
		if (m_TimeAcc >= 3.0 && m_pModelCom->Get_CurrAnimIndex() != 40)
			m_pMonster->Set_AnimAndReset(m_pModelCom, 40);
		if (m_pModelCom->Get_TargetTimeAcc(40) >= 10.0 && m_pModelCom->Get_TargetTimeAcc(40) <= 35.0)
			m_pTransformCom->Go_Up(TimeDelta * 0.25);
		if (m_pModelCom->Get_TargetTimeAcc(40) >= 10.0 && m_bSoundEnd == false)
		{
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Goat_25_Spawn.wav"), 0.7f);
			m_bSoundEnd = true;
		}
		bStateEnd = m_pModelCom->Check_CurAnim_And_Finished(40);
		if (bStateEnd == true)
		{
			m_pModelCom->Set_TargetPlaySpeed_Rate(0, 1.0);
			m_pModelCom->Set_TargetPlaySpeed_Rate(40, 1.0);
		}
		break;
	default:
		break;
	}

	if (bStateEnd == true)
		return DELETE_STATE;

	return KEEP_STATE;
}

_bool CState_MonsterSpawn::NightmareSpawn(_double TimeDelta)
{
	if (m_bEffect[0] == false)
	{
		CEffect_Manager::GetInstance()->Create_NightmareSpawnTornado(m_pTransformCom);
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Nightmare_SpawnTornado.wav"), 1.f, true, m_pTransformCom->Get_Position());
		m_bEffect[0] = true;
	}

	m_TimeAcc += TimeDelta;
	if (m_TimeAcc <= 2.0)
		m_pModelCom->Set_TargetTimeAcc(0, 1.0);

	if (m_pModelCom->Get_TargetTimeAcc(0) < 25.0)
		m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta * 1.5);

	if (m_pModelCom->Get_TargetTimeAcc(0) >= 25.0 && m_bEffect[1] == false)
	{
		CEffect_Manager::GetInstance()->Create_NightmareSpawnXCross(m_pTransformCom);
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Nightmare_SpawnX.wav"), 1.f, true, m_pTransformCom->Get_Position());
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Nightmare_Spawn.wav"), 1.f, true, m_pTransformCom->Get_Position());
		m_bEffect[1] = true;
	}

	return m_pModelCom->Get_CurrFinished();
}

_bool CState_MonsterSpawn::NobodySpawn(_double TimeDelta)
{
	m_TimeAcc += TimeDelta;

	if (m_TimeAcc >= 1.0)
		m_TimeAcc = 1.0;
	
	_bool bIsScaleFinished = m_pTransformCom->ScaledLerp(__vector(0.1f, 0.1f, 0.1f, 0.f), __vector(1.f, 1.f, 1.f, 0.f), (_float)m_TimeAcc, TimeDelta);

	if (m_pModelCom->Get_TargetTimeAcc(0) >= 8.0 && m_bEffect[0] == false)
	{
		CEffect_Manager::GetInstance()->Create_Flowers(m_pTransformCom);
		m_bEffect[0] = true;
	}

	if (m_pModelCom->Get_TargetTimeAcc(0) >= 20.0 && m_bEffect[1] == false)
	{
		CEffect_Manager::GetInstance()->Create_NobodySpawnMagicCircle(m_pTransformCom);
		m_bEffect[1] = true;
	}

	if (m_pModelCom->Get_TargetTimeAcc(0) >= 30.0 && m_bEffect[2] == false)
	{
		CEffect_Manager::GetInstance()->Create_NobodySpawnTurn(m_pTransformCom);
		m_bEffect[2] = true;
	}

	if (m_bEffect[0] == true && m_bEffect[1] == true && m_bEffect[2] == true && bIsScaleFinished)
		return true;

	return false;
}

_bool CState_MonsterSpawn::ThinSoldierSpawn(_double TimeDelta)
{
	if (m_bEffect[0] == false)
	{
		CEffect_Manager::GetInstance()->Create_AppearAction(m_pTransformCom->Get_Position(), CEffect_Manager::THIN);
		m_bEffect[0] = true;
	}

	if (m_pModelCom->Check_CurAnim_And_Finished(16))
		m_pMonster->Set_AnimAndReset(m_pModelCom, 17);
	if (m_pModelCom->Check_CurAnim_And_Finished(17))
		return true;

	return false;
}

CState_MonsterSpawn* CState_MonsterSpawn::Create(void* pArg)
{
	CState_MonsterSpawn* pInstance = new CState_MonsterSpawn;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

void CState_MonsterSpawn::Free()
{
	__super::Free();
}