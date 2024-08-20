#include "stdafx.h"
#include "State_MonsterDynamicMove.h"
#include "GameInstance.h"
#include "Monster.h"
#include "Effect_Manager.h"
#include "Terrain.h"
#include "Goat.h"

CState_MonsterDynamicMove::CState_MonsterDynamicMove()
{
}

HRESULT CState_MonsterDynamicMove::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CState_MonsterDynamicMove::EnterState()
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
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("SkullSoldier_DynamicMove.wav"), 0.7f, true, m_pTransformCom->Get_Position());
		m_iNextAnimation = rand() % 2 + 4;
		m_pMonster->Set_AnimAndReset(m_pModelCom, m_iNextAnimation);
		break;
	case MONSTERTYPE_BOWMAN:
		m_pMonster->Set_AnimAndReset(m_pModelCom, 2);
		m_iRandNum = rand() % 2;
		break;
	case MONSTERTYPE_SHININGEGG:
		m_iNextAnimation = rand() % 3 + 4;
		m_pMonster->Set_AnimAndReset(m_pModelCom, m_iNextAnimation);
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("ShiningEgg_DynamicMove.wav"), 0.6f, true, m_pTransformCom->Get_Position());
		break;
	case MONSTERTYPE_MONKEY:
		m_iNextAnimation = 2;
		m_pMonster->Set_AnimAndReset(m_pModelCom, m_iNextAnimation);
		break;
	case MONSTERTYPE_BIGTREE:
		// 터레인 설정
		m_pRapunzelBossTerrain = static_cast<CTerrain*>(pGameInstance->Get_Front(LEVEL_RAPUNZEL_BOSS, TEXT("Layer_Terrain")));
		// 애니메이션 설정
		m_pMonster->Set_AnimAndReset(m_pModelCom, 2);
		m_iRandNum = rand() % 3;
		break;
	case MONSTERTYPE_THINSOLDIER:
		m_iNextAnimation = rand() % 3 + 3;
		m_iRandNum       = rand() % 2;
		m_pMonster->Set_AnimAndReset(m_pModelCom, m_iNextAnimation);
		break;
	case MONSTERTYPE_GOAT:
		m_pModelCom->Set_TempIndex(1);
		m_TimeAcc = 1.0;
		break;
	default:
		break;
	}

	return S_OK;
}

_bool CState_MonsterDynamicMove::Action(_double TimeDelta)
{
	_bool bStateEnd = false;
	// 몬스터 타입에 따라 Action 분기
	switch (m_eMonsterType)
	{
	case MONSTERTYPE_SKULLSOLDIER:
		m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);
		bStateEnd = m_pModelCom->CheckFinished_Input(m_iNextAnimation);
		break;
	case MONSTERTYPE_BOWMAN:
		bStateEnd = TurnRound(2.0, TimeDelta);
		break;
	case MONSTERTYPE_SHININGEGG:
		m_EffectInterval += TimeDelta;
		m_RandInterval = (rand() % 5 + 1) * 0.1;
		if (m_EffectInterval >= m_RandInterval)
		{
			CEffect_Manager::GetInstance()->Create_Cloud(m_pTransformCom->Get_Position());
			m_EffectInterval = 0.0;
		}
		m_pTransformCom->SetHeightNavi(TimeDelta, m_pMonsterNavigation);
		bStateEnd = m_pModelCom->CheckFinished_Input(m_iNextAnimation);
		break;
	case MONSTERTYPE_MONKEY:
		if (m_pModelCom->Get_TargetTimeAcc(2) >= 25.0 && m_bEffectEnd[0] == false)
		{
			CEffect_Manager::GetInstance()->Create_ShockWave(m_pTransformCom->Get_WorldFloat4x4_Pointer());
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Monkey_Provoke.wav"), 0.7f, true, m_pTransformCom->Get_Position());
			m_bEffectEnd[0] = true;
		}
		if (m_pModelCom->Get_TargetTimeAcc(2) >= 35.0 && m_bEffectEnd[1] == false)
		{
			CEffect_Manager::GetInstance()->Create_ShockWave(m_pTransformCom->Get_WorldFloat4x4_Pointer());
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Monkey_Provoke.wav"), 0.7f, true, m_pTransformCom->Get_Position());
			m_bEffectEnd[1] = true;
		}
		bStateEnd = m_pModelCom->CheckFinished_Input(m_iNextAnimation);
		break;
	case MONSTERTYPE_BIGTREE:
		bStateEnd = BigTreeDynamicMove(m_TimeAcc, TimeDelta);
		break;
	case MONSTERTYPE_THINSOLDIER:
		bStateEnd = TurnRoundAndBack(0.2, 7.f, 2.5, TimeDelta);
		break;
	case MONSTERTYPE_GOAT:
		m_TimeAcc += TimeDelta;
		if (m_TimeAcc >= 1.0)
		{
			// 텔레포트 하려는 위치들
			vector<_vector> vDir = { m_pTransformCom->Get_Up() + m_pTransformCom->Get_Right() - m_pTransformCom->Get_Look(),
									 m_pTransformCom->Get_Up() - m_pTransformCom->Get_Right() - m_pTransformCom->Get_Look(),
									 m_pTransformCom->Get_Up() - m_pTransformCom->Get_Look(),
									 m_pTransformCom->Get_Up() + m_pTransformCom->Get_Look(),
									 m_pTransformCom->Get_Up() };

			// 본인 주변으로 텔레포트
			_uint iLoopAcc = 0;
			while (true)
			{
				// 무한루프 탈출
				++iLoopAcc;

				// 플레이어를 쳐다보게 하기
				m_pTransformCom->LookAt(m_pPlayerTransform);

				_uint iRandNum = rand() % (vDir.size() - 1);
				if (iLoopAcc == 10)
					iRandNum = (_uint)(vDir.size() - 1);
				if (m_pTransformCom->Teleport(vDir[iRandNum], 3.5f, m_pMonsterNavigation) == true)
				{
					CEffect_Manager::GetInstance()->Create_GoatTeleport(m_pTransformCom);
					static_cast<CGoat*>(m_pMonster)->StartTeleportDissolve();
					m_iTeleportAcc++;
					break;
				}
			}

			m_TimeAcc = 0.0;
		}

		// 3번 텔레포트 하면 종료
		if (m_iTeleportAcc == 3)
			bStateEnd = true;
		break;
	}

	if (bStateEnd == true)
		return DELETE_STATE;

	return KEEP_STATE;
}

_bool CState_MonsterDynamicMove::TurnRound(_double MaxTime, _double TimeDelta)
{
	m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);

	if (m_iRandNum == 0)
		m_pTransformCom->Go_Left(TimeDelta, m_pMonsterNavigation);
	else
		m_pTransformCom->Go_Right(TimeDelta, m_pMonsterNavigation);

	m_TimeAcc += TimeDelta;
	if (m_TimeAcc >= MaxTime)
		return true;

	return false;
}

_bool CState_MonsterDynamicMove::TurnRoundAndBack(_double Speed, _double fDistDiff, _double TimeAcc, _double TimeDelta)
{
	m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);

	_double SpeedPerTime = TimeDelta * Speed;

	if (m_iRandNum == 0)
		m_pTransformCom->Go_Left(SpeedPerTime, m_pMonsterNavigation);
	else
		m_pTransformCom->Go_Right(SpeedPerTime, m_pMonsterNavigation);

	if (m_pTransformCom->Get_Distance(m_pPlayerTransform) < fDistDiff)
		m_pTransformCom->Go_Backward(SpeedPerTime, m_pMonsterNavigation);

	m_TimeAcc += TimeDelta;
	if (m_TimeAcc >= TimeAcc)
		return true;

	return false;
}

_bool CState_MonsterDynamicMove::BigTreeDynamicMove(_double TimeAcc, _double TimeDelta)
{
	m_TimeAcc += TimeDelta;

	_double MoveSpeed = TimeDelta * 0.35;

	m_pTransformCom->Go_Straight(MoveSpeed, m_pMonsterNavigation);

	if (m_iRandNum == 0)
		m_pTransformCom->Turn_Right(__vector(0.f, 1.f, 0.f, 0.f), MoveSpeed);
	else if (m_iRandNum == 1)
		m_pTransformCom->Turn_Left(__vector(0.f, 1.f, 0.f, 0.f), MoveSpeed);

	// 터레인태우기
	m_pTransformCom->Set_Position(m_pRapunzelBossTerrain->Compute_PositionOnTerrain(m_pTransformCom->Get_Position()));

	// 일정 시간 이상 움직였으면 true 리턴
	if (m_TimeAcc >= 6.5)
		return true;

	return false;
}

_bool CState_MonsterDynamicMove::GoatDynamicMove(_double TiemDelta)
{
	return _bool();
}

CState_MonsterDynamicMove* CState_MonsterDynamicMove::Create(void* pArg)
{
	CState_MonsterDynamicMove* pInstance = new CState_MonsterDynamicMove;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

void CState_MonsterDynamicMove::Free()
{
	__super::Free();
}