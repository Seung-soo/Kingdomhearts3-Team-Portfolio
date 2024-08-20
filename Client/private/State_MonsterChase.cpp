#include "stdafx.h"
#include "State_MonsterChase.h"
#include "GameInstance.h"
#include "Monster.h"
#include "Effect_Manager.h"

CState_MonsterChase::CState_MonsterChase()
{
}

HRESULT CState_MonsterChase::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CState_MonsterChase::EnterState()
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

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// ���� ������ �ִ� Player �� Transform ������Ʈ�� ����
	m_pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_LevelIndex(), TEXT("Layer_Player"), TEXT("Com_Transform"), 0));

	// �ִϸ��̼� ����
	switch (m_eMonsterType)
	{
	case MONSTERTYPE_SKULLSOLDIER:
		m_pMonster->Set_AnimAndReset(m_pModelCom, 6);
		break;
	case MONSTERTYPE_CROWSOLDIER:
		m_pMonster->Set_AnimAndReset(m_pModelCom, 20);
		break;
	case MONSTERTYPE_SHININGEGG:
		m_pMonster->Set_AnimAndReset(m_pModelCom, 3);
		break;
	case MONSTERTYPE_MONKEY:
		m_pMonster->Set_AnimAndReset(m_pModelCom, 4);
		break;
	case MONSTERTYPE_NIGHTMARE:
		m_pMonster->Set_MotionTrail(true);
		m_pMonster->Set_AnimAndReset(m_pModelCom, 2);
		break;
	case MONSTERTYPE_ICEMAN:
		m_pModelCom->Set_TempIndex(9); // �ȴ� �ִϸ��̼����� ����
	default:
		break;
	}

	return S_OK;
}

_bool CState_MonsterChase::Action(_double TimeDelta)
{	
	_bool bStateEnd = false;
	_bool bTurn     = false;
	_bool bMove     = false;

	// ���� Ÿ�Կ� ���� Action �б�
	switch (m_eMonsterType)
	{
	case MONSTERTYPE_SKULLSOLDIER:
		m_pTransformCom->SetHeightNavi(TimeDelta, m_pMonsterNavigation);
		bStateEnd = Chase(5.f, TimeDelta, m_pMonsterNavigation);
		break;
	case MONSTERTYPE_CROWSOLDIER:
		bStateEnd = CrowSoldierChase(3.5f, TimeDelta);
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
		bStateEnd = Chase(3.5f, TimeDelta, m_pMonsterNavigation);
		break;
	case MONSTERTYPE_MONKEY:
		m_pTransformCom->SetHeightNavi(TimeDelta, m_pMonsterNavigation);
		bStateEnd = Chase(9.f, TimeDelta, m_pMonsterNavigation);
		break;
	case MONSTERTYPE_NIGHTMARE:
		m_EffectInterval += TimeDelta;
		if (m_EffectInterval >= 0.1)
		{
			CEffect_Manager::GetInstance()->Create_NightmareRunEffect(m_pTransformCom, &m_iEffectCount);
			m_EffectInterval = 0.0;
		}
		bStateEnd = Chase(5.f, TimeDelta, m_pMonsterNavigation);
		if (bStateEnd == true)
			m_pMonster->Set_MotionTrail(false);
		break;
	case MONSTERTYPE_ICEMAN:
		m_pModelCom->Set_CurrPlaySpeed(45.0);
		if (m_pModelCom->Get_CurrTimeAcc() >= 15.0 && m_bSoundEnd == false)
		{
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("IceMan_Walk_1.wav"), 0.7f, true, m_pTransformCom->Get_Position());
			m_bSoundEnd = true;
		}
		if (m_pModelCom->Get_CurrFinished())
		{
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("IceMan_Walk_2.wav"), 0.7f, true, m_pTransformCom->Get_Position());
			m_bSoundEnd = false;
		}
		m_TimeAcc += TimeDelta;
		bTurn      = m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta * 3.0);
		bMove      = m_pTransformCom->Go_To_TargetXZ(m_pPlayerTransform, 10.f, TimeDelta * 3.0, m_pMonsterNavigation);
		bStateEnd  = (m_TimeAcc >= 3.5 || (bTurn && bMove)); // �ð��� �ʰ��ǰų� �Ÿ�+���� ������ �����Ǹ� ������
		break;
	default:
		break;
	}

	if (bStateEnd == true)
		return DELETE_STATE;

	return KEEP_STATE;
}

_bool CState_MonsterChase::Chase(_float fDistDiff, _double TimeDelta, CNavigation* pNavigation)
{
	// �÷��̾������� ȸ��
	m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);

	// �÷��̾� ���� fDistDiff �Ÿ���ŭ �ٰ����� ��ǥ �Ÿ��� �����ϸ� true ����
	return m_pTransformCom->Go_To_TargetXZ(m_pPlayerTransform, fDistDiff, TimeDelta, pNavigation);
}

_bool CState_MonsterChase::CrowSoldierChase(_float fDistDiff, _double TimeDelta)
{
	// 20: ���� �غ�, 21: ����, 19: ���� ������

	// �÷��̾������� ȸ��
	m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);

	if (m_pModelCom->Get_TargetTimeAcc(20) >= 20.0 && m_bEffectEnd == false)
	{
		CEffect_Manager::GetInstance()->Create_CrowAttack_Roaring(m_pTransformCom->Get_Position()); // ��ȿ����Ʈ
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("CrowSoldier_Roar.wav"), 1.f, true, m_pTransformCom->Get_Position());
		m_bEffectEnd = true;
	}

	if (m_pModelCom->Check_CurAnim_And_Finished(20))
	{
		m_pMonster->Set_AnimAndReset(m_pModelCom, 21);
		m_bEffectEnd = false;
	}

	if (m_pModelCom->Get_CurrAnimIndex() == 21)
		m_EffectInterval += TimeDelta;

	if (m_pModelCom->Get_CurrAnimIndex() == 21 || m_pModelCom->Get_PrevAnimIndex() == 21 && m_pModelCom->Get_CurrAnimIndex() != 19)
	{		
		if (m_pTransformCom->Go_To_Target(m_pPlayerTransform, fDistDiff, TimeDelta * 1.5, m_pMonsterNavigation))
		{
			m_pMonster->Set_AnimAndReset(m_pModelCom, 19);
			m_pGameObject->Set_IsAttacking(true); // ���ݻ��� �ѱ�
		}
	}

	if (m_pModelCom->Get_TargetTimeAcc(19) >= 15.0 && m_bEffectEnd == false)
	{
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("CrowSoldier_AtkKick.wav"), 1.f, true, m_pTransformCom->Get_Position());
		m_bEffectEnd = true;
	}

	if (m_pModelCom->Check_CurAnim_And_Finished(19))
	{
		m_pGameObject->Set_IsAttacking(false); // ���ݻ��� ����
		return true;
	}

	return false;
}

CState_MonsterChase* CState_MonsterChase::Create(void* pArg)
{
	CState_MonsterChase* pInstance = new CState_MonsterChase;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

void CState_MonsterChase::Free()
{
	__super::Free();
}