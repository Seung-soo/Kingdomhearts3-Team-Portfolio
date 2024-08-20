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
	// ���� Ÿ�� ����
	m_eMonsterType = (MONSTERTYPE)dynamic_cast<CMonster*>(m_pGameObject)->Get_MonsterType();

	// ���� ����
	m_pMonster           = dynamic_cast<CMonster*>(m_pGameObject);
	m_pMonsterNavigation = static_cast<CNavigation*>(m_pGameObject->Get_Component(TEXT("Com_Navigation")));

	// �ͷ��� ����
	if (m_eMonsterType == MONSTERTYPE_BIGTREE)
		m_pRapunzelBossTerrain = static_cast<CTerrain*>(CGameInstance::GetInstance()->Get_Front(LEVEL_RAPUNZEL_BOSS, TEXT("Layer_Terrain")));

	// ������Ʈ
	m_pGameObject->StartRimLight(12);

	if (m_pMonsterNavigation == nullptr)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// ���� ������ �ִ� Player �� Transform ������Ʈ�� ����
	m_pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_LevelIndex(), TEXT("Layer_Player"), TEXT("Com_Transform"), 0));

	// �ִϸ��̼� ����
	m_pMonster->Set_AnimAndReset(m_pModelCom, m_iAnimIndex);

	// ���� �ݶ��̴� ����
	m_pMonster->Set_IsAttacking(false);
	
	// Goat �� ��� �ǰ� ���� ���
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
	// ���� �ݶ��̴� ����
	m_pMonster->Set_IsAttacking(false);

	// ������Ʈ
	m_pGameObject->EndRimLight();

	return S_OK;
}

_bool CState_MonsterOnDamaged::Action(_double TimeDelta)
{
	_bool bStateEnd = false;

	// ���� Ÿ�Կ� ���� Action �б�
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
		// ������Ʈ
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