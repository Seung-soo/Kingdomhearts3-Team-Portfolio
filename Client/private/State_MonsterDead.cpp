#include "stdafx.h"
#include "State_MonsterDead.h"
#include "GameInstance.h"
#include "Monster.h"
#include "Effect_Manager.h"

CState_MonsterDead::CState_MonsterDead()
{
}

HRESULT CState_MonsterDead::Initialize(void* pArg)
{
	if (pArg == nullptr)
	{
		BREAKPOINT;
	}

	memcpy(&m_iAnimIndex, pArg, sizeof(_uint));

	return S_OK;
}

HRESULT CState_MonsterDead::EnterState()
{
	// ���� Ÿ�� ����
	m_eMonsterType = (MONSTERTYPE)dynamic_cast<CMonster*>(m_pGameObject)->Get_MonsterType();

	// ���� ����
	m_pMonster           = dynamic_cast<CMonster*>(m_pGameObject);
	m_pMonsterNavigation = static_cast<CNavigation*>(m_pGameObject->Get_Component(TEXT("Com_Navigation")));

	if (m_pMonsterNavigation == nullptr)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// ���� ���� ����
	m_pGameObject->Set_IsAttacking(false);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// ���� ������ �ִ� Player �� Transform ������Ʈ�� ����
	m_pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_LevelIndex(), TEXT("Layer_Player"), TEXT("Com_Transform"), 0));

	// �ִϸ��̼� ����
	m_pMonster->Set_AnimAndReset(m_pModelCom, m_iAnimIndex);

	switch (m_eMonsterType)
	{
	case MONSTERTYPE_SKULLSOLDIER:
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("SkullSoldier_Dead.wav"), 1.f, true, m_pTransformCom->Get_Position());
		break;
	case MONSTERTYPE_CROWSOLDIER:
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("CrowSoldier_Dead.wav"), 1.f, true, m_pTransformCom->Get_Position());
		break;
	case MONSTERTYPE_THINSOLDIER:
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("ThinSoldier_Dead.wav"), 1.f, true, m_pTransformCom->Get_Position());
		break;
	case MONSTERTYPE_SHININGEGG:
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("ShiningEgg_Dead.wav"), 1.5f, true, m_pTransformCom->Get_Position());
		break;
	case MONSTERTYPE_MONKEY:
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Monkey_Dead.wav"), 1.f, true, m_pTransformCom->Get_Position());
		break;
	case MONSTERTYPE_MOWER:
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Mower_Dead.wav"), 1.f, true, m_pTransformCom->Get_Position());
		break;
	case MONSTERTYPE_NOBODY:
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Nobody_Dead.wav"), 1.f, true, m_pTransformCom->Get_Position());
		break;
	default:
		break;
	}

	return S_OK;
}

_bool CState_MonsterDead::Action(_double TimeDelta)
{
	_bool bStateEnd = false;

	// ���� Ÿ�Կ� ���� Action �б�
	switch (m_eMonsterType)
	{
	case MONSTERTYPE_SKULLSOLDIER:
		bStateEnd = m_pModelCom->Get_CurrFinished();
		if (bStateEnd)
			CEffect_Manager::GetInstance()->Create_DeadEffect(m_pTransformCom, 2.f, 1.f);
		break;
	case MONSTERTYPE_CROWSOLDIER:
		bStateEnd = m_pModelCom->Get_CurrFinished();
		if (bStateEnd)
			CEffect_Manager::GetInstance()->Create_DeadEffect(m_pTransformCom, 2.f, 1.f);
		break;
	case MONSTERTYPE_BOWMAN:
		bStateEnd = m_pModelCom->Get_CurrFinished();
		if (bStateEnd)
			CEffect_Manager::GetInstance()->Create_DeadEffect(m_pTransformCom, 2.f, 1.f);
		break;
	case MONSTERTYPE_CANNON:
		bStateEnd = m_pModelCom->Get_CurrFinished();
		break;
	case MONSTERTYPE_SHININGEGG:
		bStateEnd = m_pModelCom->Get_CurrFinished();
		if (bStateEnd)
			CEffect_Manager::GetInstance()->Create_DeadEffect(m_pTransformCom, 1.5f, 1.f);
		break;
	case MONSTERTYPE_MONKEY:
		bStateEnd = m_pModelCom->Get_CurrFinished();
		if (bStateEnd)
			CEffect_Manager::GetInstance()->Create_DeadEffect(m_pTransformCom, 2.f, 1.f);
		break;
	case MONSTERTYPE_BIGTREE:
		if (m_pModelCom->Get_TargetTimeAcc(m_iAnimIndex) >= 30.0)
			m_pGameObject->StartDissolve(0.5f); 
		bStateEnd = (m_pTransformCom->SetHeightZero(TimeDelta, m_pMonsterNavigation) && m_pModelCom->Get_CurrFinished());
		break;
	case MONSTERTYPE_MOWER:
		bStateEnd = m_pModelCom->Get_CurrFinished();
		if (bStateEnd)
			CEffect_Manager::GetInstance()->Create_DeadEffect(m_pTransformCom, 2.f, 1.f);
		break;
	case MONSTERTYPE_THINSOLDIER:
		bStateEnd = m_pModelCom->Get_CurrFinished();
		if (bStateEnd)
			CEffect_Manager::GetInstance()->Create_DeadEffect(m_pTransformCom, 3.f, 1.f);
		break;
	case MONSTERTYPE_NIGHTMARE:
		if (m_pModelCom->Get_TargetTimeAcc(m_iAnimIndex) >= 12.0)
			m_pGameObject->StartDissolve(0.85f);
		bStateEnd = m_pModelCom->Get_CurrFinished();
		break;
	case MONSTERTYPE_NOBODY:
		if (m_bEffectEnd == false)
		{
			CEffect_Manager::GetInstance()->Create_Flowers(m_pTransformCom);
			m_bEffectEnd = true;
		}
		if (m_pModelCom->Get_TargetTimeAcc(m_iAnimIndex) >= 12.0)
			m_pGameObject->StartDissolve(0.85f);
		bStateEnd = m_pModelCom->Get_CurrFinished();
		break;
	case MONSTERTYPE_ICEMAN:
		if (m_pModelCom->Get_CurrFinished() == true)
		{
			m_pGameObject->StartDissolve(0.5f); // ������ ����
			m_pStateMachine->ClearState(CStateMachine::STATETYPE_IDLE); // Dead �ִϸ��̼��� ���� �Ŀ� �� ���� �״�� �ֵ��� Idle ���´� �����
		}
		if (m_pModelCom->Get_TargetTimeAcc(24) >= 60.0 && m_bEffectEnd == false)
		{
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("IceMan_Dead_23.wav"), 0.7f, true, m_pTransformCom->Get_Position());

			CEffect_Manager::GetInstance()->Create_HitStar(m_pTransformCom, 4.f);
			m_bEffectEnd = true;
		}
		m_pTransformCom->SetHeightZero(TimeDelta * 5.0, m_pMonsterNavigation);
		if (m_pGameObject->GetDissolveAmount() >= 1.f)
			bStateEnd = true; // ������ ������ Dead
		break;
	case MONSTERTYPE_FROZENWOLF:
		if (m_pModelCom->Get_CurrFinished() == true)
		{
			m_pGameObject->StartDissolve(0.5f); // ������ ����
			m_pStateMachine->ClearState(CStateMachine::STATETYPE_IDLE); // Dead �ִϸ��̼��� ���� �Ŀ� �� ���� �״�� �ֵ��� Idle ���´� �����
		}
		if (m_pModelCom->Get_TargetTimeAcc(24) >= 60.0 && m_bEffectEnd == false)
		{
			CEffect_Manager::GetInstance()->Create_HitStar(m_pTransformCom, 4.f);
			m_bEffectEnd = true;
		}
		m_pTransformCom->SetHeightZero(TimeDelta * 5.0, m_pMonsterNavigation);
		if (m_pGameObject->GetDissolveAmount() >= 1.f)
			bStateEnd = true; // ������ ������ Dead
		break;
	case MONSTERTYPE_GOAT:
		bStateEnd = m_pModelCom->Get_CurrFinished();
		break;
	default:
		break;
	}

	if (bStateEnd == true)
	{
		m_pGameObject->Set_Dead(true);
		return DELETE_STATE;
	}

	return KEEP_STATE;
}

CState_MonsterDead* CState_MonsterDead::Create(void* pArg)
{
	CState_MonsterDead* pInstance = new CState_MonsterDead;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

void CState_MonsterDead::Free()
{
	__super::Free();
}
