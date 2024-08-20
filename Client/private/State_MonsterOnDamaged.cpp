#include "stdafx.h"
#include "State_MonsterOnDamaged.h"
#include "GameInstance.h"
#include "Monster.h"
#include "Terrain.h"

CState_MonsterOnDamaged::CState_MonsterOnDamaged()
{
}

HRESULT CState_MonsterOnDamaged::Initialize(void* pArg)
{
	if (pArg == nullptr)
	{
		BREAKPOINT;
	}

	memcpy(&m_iAnimIndex, pArg, sizeof(_uint));

	return S_OK;
}

HRESULT CState_MonsterOnDamaged::EnterState()
{
	// 몬스터 타입 설정
	m_eMonsterType = (MONSTERTYPE)dynamic_cast<CMonster*>(m_pGameObject)->Get_MonsterType();

	// 몬스터 설정
	m_pMonster           = dynamic_cast<CMonster*>(m_pGameObject);
	m_pMonsterNavigation = static_cast<CNavigation*>(m_pGameObject->Get_Component(TEXT("Com_Navigation")));

	// 터레인 설정
	if (m_eMonsterType == MONSTERTYPE_BIGTREE)
		m_pRapunzelBossTerrain = static_cast<CTerrain*>(CGameInstance::GetInstance()->Get_Front(LEVEL_RAPUNZEL_BOSS, TEXT("Layer_Terrain")));

	// 림라이트
	m_pGameObject->StartRimLight(12);

	if (m_pMonsterNavigation == nullptr)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// 현재 레벨에 있는 Player 의 Transform 컴포넌트를 얻어옴
	m_pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_LevelIndex(), TEXT("Layer_Player"), TEXT("Com_Transform"), 0));

	// 애니메이션 설정
	m_pMonster->Set_AnimAndReset(m_pModelCom, m_iAnimIndex);

	// 공격 콜라이더 끄기
	m_pMonster->Set_IsAttacking(false);
	
	// Goat 일 경우 피격 사운드 재생
	if (m_eMonsterType == MONSTERTYPE_GOAT)
	{
		_uint iRandNum = rand() % 4;
		if (iRandNum == 0)
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Goat_7_OnDamaged.wav"), 0.2f, true, m_pTransformCom->Get_Position());
		else if (iRandNum == 1)
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Goat_8_OnDamaged.wav"), 0.2f, true, m_pTransformCom->Get_Position());
		else if (iRandNum == 2)
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Goat_9_OnDamaged.wav"), 0.2f, true, m_pTransformCom->Get_Position());
		else
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Goat_10_OnDamaged.wav"), 0.2f, true, m_pTransformCom->Get_Position());
	}

	return S_OK;
}

HRESULT CState_MonsterOnDamaged::ClearState()
{
	// 공격 콜라이더 끄기
	m_pMonster->Set_IsAttacking(false);

	// 림라이트
	m_pGameObject->EndRimLight();

	return S_OK;
}

_bool CState_MonsterOnDamaged::Action(_double TimeDelta)
{
	_bool bStateEnd = false;

	// 몬스터 타입에 따라 Action 분기
	switch (m_eMonsterType)
	{
	case MONSTERTYPE_SKULLSOLDIER:
		m_pGameObject->Set_IsAttacking(false);
		bStateEnd = (m_pTransformCom->SetHeightZero(TimeDelta, m_pMonsterNavigation) && m_pModelCom->Get_CurrFinished());
		break;
	case MONSTERTYPE_CROWSOLDIER:
		m_pGameObject->Set_IsAttacking(false);
		bStateEnd = m_pModelCom->Get_CurrFinished();
		break;
	case MONSTERTYPE_BOWMAN:
		m_pGameObject->Set_IsAttacking(false);
		bStateEnd = (m_pTransformCom->SetHeightZero(TimeDelta, m_pMonsterNavigation) && m_pModelCom->Get_CurrFinished());
		break;
	case MONSTERTYPE_CANNON:
		m_pGameObject->Set_IsAttacking(false);
		bStateEnd = (m_pModelCom->Get_CurrFinished());
		break;
	case MONSTERTYPE_SHININGEGG:
		m_pGameObject->Set_IsAttacking(false);
		bStateEnd = (m_pTransformCom->SetHeightZero(TimeDelta, m_pMonsterNavigation) && m_pModelCom->Get_CurrFinished());
		break;
	case MONSTERTYPE_MONKEY:
		m_pGameObject->Set_IsAttacking(false);
		bStateEnd = (m_pTransformCom->SetHeightZero(TimeDelta, m_pMonsterNavigation) && m_pModelCom->Get_CurrFinished());
		break;
	case MONSTERTYPE_BIGTREE:
		m_pGameObject->Set_IsAttacking(false);
		bStateEnd = (m_pTransformCom->SetHeight(TimeDelta, vecy(m_pRapunzelBossTerrain->Compute_PositionOnTerrain(m_pTransformCom->Get_Position()))) && m_pModelCom->Get_CurrFinished());
		break;
	case MONSTERTYPE_MOWER:
		m_pGameObject->Set_IsAttacking(false);
		bStateEnd = (m_pModelCom->Get_CurrFinished());
		break;
	case MONSTERTYPE_NIGHTMARE:
		m_pGameObject->Set_IsAttacking(false);
		bStateEnd = m_pModelCom->Get_CurrFinished();
		break;
	case MONSTERTYPE_NOBODY:
		m_pGameObject->Set_IsAttacking(false);
		bStateEnd = (m_pModelCom->Get_CurrFinished());
		break;
	case MONSTERTYPE_THINSOLDIER:
		m_pGameObject->Set_IsAttacking(false);
		bStateEnd = (m_pTransformCom->SetHeightZero(TimeDelta, m_pMonsterNavigation) && m_pModelCom->Get_CurrFinished());
		break;
	case MONSTERTYPE_ICEMAN:
		m_pGameObject->Set_IsAttacking(false);
		bStateEnd = (m_pTransformCom->SetHeightZero(TimeDelta * 5.0, m_pMonsterNavigation) && m_pModelCom->Get_CurrFinished());
		break;
	case MONSTERTYPE_FROZENWOLF:
		m_pGameObject->Set_IsAttacking(false);
		bStateEnd = (m_pTransformCom->SetHeightZero(TimeDelta * 3.0, m_pMonsterNavigation) && m_pModelCom->Get_CurrFinished());
		break;
	case MONSTERTYPE_GOAT:
		if (m_bActionEnd == false)
		{
			m_pGameObject->Set_IsAttacking(false);
			m_bActionEnd = true;
		}
		m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);
		m_pTransformCom->Set_LookY_Zero(TimeDelta);
		bStateEnd = m_pModelCom->Get_CurrFinished();
		break;
	default:
		break;
	}

	if (bStateEnd == true)
	{
		// 림라이트
		m_pGameObject->EndRimLight();
		return DELETE_STATE;
	}

	return KEEP_STATE;
}

CState_MonsterOnDamaged* CState_MonsterOnDamaged::Create(void* pArg)
{
	CState_MonsterOnDamaged* pInstance = new CState_MonsterOnDamaged;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

void CState_MonsterOnDamaged::Free()
{
	__super::Free();
}