#include "stdafx.h"
#include "State_MonsterWait.h"
#include "GameInstance.h"
#include "Monster.h"

CState_MonsterWait::CState_MonsterWait()
{
}

HRESULT CState_MonsterWait::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CState_MonsterWait::EnterState()
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
		m_pModelCom->Set_TempIndex(2);
		break;
	case MONSTERTYPE_CROWSOLDIER:
		m_pModelCom->Set_TempIndex(2);
		break;
	case MONSTERTYPE_BOWMAN:
		m_pModelCom->Set_TempIndex(1);
		break;
	case MONSTERTYPE_CANNON:
		m_pModelCom->Set_TempIndex(0);
		break;
	case MONSTERTYPE_SHININGEGG:
		m_pModelCom->Set_TempIndex(1);
		break;
	case MONSTERTYPE_MONKEY:
		m_pModelCom->Set_TempIndex(1);
		break;
	case MONSTERTYPE_BIGTREE:
		m_pModelCom->Set_TempIndex(0);
		break;
	case MONSTERTYPE_MOWER:
		m_pModelCom->Set_TempIndex(1);
		break;
	case MONSTERTYPE_NIGHTMARE:
		m_pModelCom->Set_TempIndex(1);
		break;
	case MONSTERTYPE_NOBODY:
		m_pModelCom->Set_TempIndex(1);
		break;
	default:
		break;
	}

	return S_OK;
}

_bool CState_MonsterWait::Action(_double TimeDelta)
{
	return S_OK;
}

_bool CState_MonsterWait::WaitTime(_double WaitTime, _double TimeDelta)
{
	m_TimeAcc += TimeDelta;

	if (m_TimeAcc >= WaitTime)
		return S_OK;

	return false;
}

CState_MonsterWait* CState_MonsterWait::Create(void* pArg)
{
	CState_MonsterWait* pInstance = new CState_MonsterWait;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

void CState_MonsterWait::Free()
{
	__super::Free();
}