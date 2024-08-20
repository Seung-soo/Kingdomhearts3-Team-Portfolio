#include "stdafx.h"
#include "State_MonsterIdle.h"
#include "GameInstance.h"
#include "Monster.h"
#include "Terrain.h"
#include "Effect_Manager.h"
#include "Goat.h"

CState_MonsterIdle::CState_MonsterIdle()
{
}

HRESULT CState_MonsterIdle::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CState_MonsterIdle::EnterState()
{
	// 몬스터 타입 설정
	m_eMonsterType = (MONSTERTYPE)dynamic_cast<CMonster*>(m_pGameObject)->Get_MonsterType();

	// 몬스터 설정
	m_pMonster           = dynamic_cast<CMonster*>(m_pGameObject);	
	m_pMonsterNavigation = static_cast<CNavigation*>(m_pGameObject->Get_Component(TEXT("Com_Navigation")));

	return S_OK;
}

_bool CState_MonsterIdle::Action(_double TimeDelta)
{
	// 상태머신에 Idle 만 있다면 애니메이션 설정하고 Action
	if (m_pStateMachine->Get_NumberOfStates() != 1)
		return KEEP_STATE;

	// 플레이어 설정
	if (m_pPlayerTransform == nullptr)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();

		CGameObject* pPlayer = pGameInstance->Get_GameObject(pGameInstance->Get_LevelIndex(), TEXT("Layer_Player"), 0);

		if (pPlayer == nullptr)
			return KEEP_STATE;

		m_pPlayerTransform = pPlayer->Get_Transform();

		if (m_pPlayerTransform == nullptr)
			return KEEP_STATE;
	}

	// 시간누적
	m_TimeAcc += TimeDelta;

	switch (m_eMonsterType)
	{
	case MONSTERTYPE_SKULLSOLDIER:
		m_pModelCom->Set_TempIndex(2);
		LookPlayer(TimeDelta);
		break;
	case MONSTERTYPE_CROWSOLDIER:
		m_pModelCom->Set_TempIndex(2);
		LookPlayer(TimeDelta);
		if (m_bIsFinishedFlyUpper == false)
			m_bIsFinishedFlyUpper = FlyUpperPlayer(3.f, 0.1, TimeDelta);
		FlyLRMove(0.1, 10.0, TimeDelta);
		break;
	case MONSTERTYPE_BOWMAN:
		m_pModelCom->Set_TempIndex(1);
		LookPlayer(TimeDelta);
		break;
	case MONSTERTYPE_CANNON:
		m_pModelCom->Set_TempIndex(0);
		LookPlayer(TimeDelta);
		break;
	case MONSTERTYPE_SHININGEGG:
		if (m_TimeAcc >= 1.0)
		{
			CEffect_Manager::GetInstance()->Create_ShineParticle(m_pTransformCom, __vector(0.f, 0.5f, 0.f, 0.f));
			m_TimeAcc = 0.0;
		}
		m_pModelCom->Set_TempIndex(1);
		LookPlayer(TimeDelta);
		break;
	case MONSTERTYPE_MONKEY:
		m_pModelCom->Set_TempIndex(1);
		LookPlayer(TimeDelta);
		break;
	case MONSTERTYPE_BIGTREE:
		if (CGameInstance::GetInstance()->Get_LevelIndex() != LEVEL_MAPTOOL)
			m_pTransformCom->SetHeight(TimeDelta,
				vecy(static_cast<CTerrain*>(CGameInstance::GetInstance()->Get_Front(LEVEL_RAPUNZEL_BOSS, TEXT("Layer_Terrain")))->Compute_PositionOnTerrain(m_pTransformCom->Get_Position())));
		m_pModelCom->Set_TempIndex(0);
		break;
	case MONSTERTYPE_MOWER:
		m_pModelCom->Set_TempIndex(1);
		LookPlayer(TimeDelta);
		break;
	case MONSTERTYPE_NIGHTMARE:
		m_pModelCom->Set_TempIndex(1);
		LookPlayer(TimeDelta);
		break;
	case MONSTERTYPE_NOBODY:
		m_pModelCom->Set_TempIndex(1);
		LookPlayer(TimeDelta);
		if (m_bIsFinishedFlyUpper == false)
			m_bIsFinishedFlyUpper = FlyUpperPlayer(2.f, 0.2, TimeDelta);
		FlyLRMove(0.35, 10.0, TimeDelta);
		break;
	case MONSTERTYPE_THINSOLDIER:
		m_pModelCom->Set_TempIndex(0);
		LookPlayer(TimeDelta);
		break;
	case MONSTERTYPE_ICEMAN:
		// 계속해서 재생시킬 LOOP 애니메이션은 Set_TempIndex 로 설정
		m_pModelCom->Set_TempIndex(0);
		break;
	case MONSTERTYPE_FROZENWOLF:
		m_pModelCom->Set_TempIndex(0);
		m_pModelCom->Set_TargetPlaySpeed(0, 50.0);
		break;
	case MONSTERTYPE_GOAT:
		m_pModelCom->Set_TempIndex(0);
		FlyLRMove(0.075, 10.0, TimeDelta);
		FlyFBMove(0.05, 6.0, TimeDelta);
		FlyUDMove(0.025, 4.0, TimeDelta);
		Teleport(TimeDelta);
		m_pTransformCom->Go_Up(TimeDelta * 0.01);
		m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);
		m_pTransformCom->Set_LookY_Zero(TimeDelta);
		break;
	default:
		break;
	}

	return KEEP_STATE;
}

void CState_MonsterIdle::LookPlayer(_double TimeDelta)
{
	m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);
}

_bool CState_MonsterIdle::FlyUpperPlayer(_float fHeight, _double Speed, _double TimeDelta)
{
	_float fCurHeight    = vecy(m_pTransformCom->Get_Position());
	_float fTargetHeight = vecy(m_pPlayerTransform->Get_Position()) + fHeight;

	if (fTargetHeight - fCurHeight <= 0.5f)
		return true; 

	// 플레이어보다 높게 날기
	if (fTargetHeight > fCurHeight)
		m_pTransformCom->Go_Up(TimeDelta * Speed, m_pMonsterNavigation);

	// 목표 높이보다 더 높으면 아래로
	if (fCurHeight >= fTargetHeight)
		m_pTransformCom->Go_Down(TimeDelta * Speed, m_pMonsterNavigation);

	return false;
}

void CState_MonsterIdle::FlyLRMove(_double Speed, _double MaxFlyTime, _double TimeDelta)
{
	if (m_TimeAcc >= MaxFlyTime)
		m_TimeAcc = 0.0;

	// 번갈아가면서 좌우로 이동
	if (m_TimeAcc >= MaxFlyTime * 0.5)
		m_pTransformCom->Go_Right(TimeDelta * Speed, m_pMonsterNavigation);
	else
		m_pTransformCom->Go_Left(TimeDelta * Speed, m_pMonsterNavigation);
}

void CState_MonsterIdle::FlyFBMove(_double Speed, _double MaxFlyTime, _double TimeDelta)
{
	if (m_TimeAcc >= MaxFlyTime)
		m_TimeAcc = 0.0;

	// 번갈아가면서 앞뒤로 이동
	if (m_TimeAcc >= MaxFlyTime * 0.5)
		m_pTransformCom->Go_Straight(TimeDelta * Speed, m_pMonsterNavigation);
	else
		m_pTransformCom->Go_Backward(TimeDelta * Speed, m_pMonsterNavigation);
}

void CState_MonsterIdle::FlyUDMove(_double Speed, _double MaxFlyTime, _double TimeDelta)
{
	if (m_TimeAcc >= MaxFlyTime)
		m_TimeAcc = 0.0;

	// 번갈아가면서 위아래로 이동
	if (m_TimeAcc >= MaxFlyTime * 0.5)
		m_pTransformCom->Go_Up(TimeDelta * Speed, m_pMonsterNavigation);
	else
		m_pTransformCom->Go_Down(TimeDelta * Speed, m_pMonsterNavigation);
}

void CState_MonsterIdle::Teleport(_double TimeDelta)
{
	m_TeleportTimeAcc += TimeDelta;

	if (m_TeleportTimeAcc >= 3.0)
	{
		if (rand() % 3 == 0)
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

				_uint iRandNum = rand() % (vDir.size() - 1);
				if (iLoopAcc == 10)
					iRandNum = (_uint)(vDir.size() - 1);
				if (m_pTransformCom->Teleport(vDir[iRandNum], 3.5f, m_pMonsterNavigation) == true)
				{
					CEffect_Manager::GetInstance()->Create_GoatTeleport(m_pTransformCom);
					static_cast<CGoat*>(m_pGameObject)->StartTeleportDissolve();
					break;
				}
			}
		}

		m_TeleportTimeAcc = 0.0;
	}
}

CState_MonsterIdle* CState_MonsterIdle::Create(void* pArg)
{
	CState_MonsterIdle* pInstance = new CState_MonsterIdle;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

void CState_MonsterIdle::Free()
{
	__super::Free();
}