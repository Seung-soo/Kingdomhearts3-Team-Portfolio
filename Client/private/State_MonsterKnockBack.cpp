#include "stdafx.h"
#include "State_MonsterKnockBack.h"
#include "GameInstance.h"
#include "Monster.h"
#include "Effect_Manager.h"

CState_MonsterKnockBack::CState_MonsterKnockBack()
{
}

HRESULT CState_MonsterKnockBack::Initialize(void* pArg)
{
	if (pArg == nullptr)
	{
		BREAKPOINT;
	}

	memcpy(&m_iAnimIndex, pArg, sizeof(_uint));

	return S_OK;
}

HRESULT CState_MonsterKnockBack::EnterState()
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

	// 애니메이션 설정
	m_pMonster->Set_AnimAndReset(m_pModelCom, m_iAnimIndex);

	return S_OK;
}

_bool CState_MonsterKnockBack::Action(_double TimeDelta)
{
	_bool bStateEnd = false;

	// 몬스터 타입에 따라 Action 분기
	switch (m_eMonsterType)
	{
	case MONSTERTYPE_SKULLSOLDIER:
		bStateEnd = SkullSoldierAnim(TimeDelta);
		break;
	case MONSTERTYPE_THINSOLDIER:
		bStateEnd = ThinSoldierAnim(TimeDelta);
		break;
	case MONSTERTYPE_MONKEY:
		bStateEnd = MonkeyAnim(TimeDelta);
		break;
	case MONSTERTYPE_ICEMAN:
		m_pModelCom->Set_TargetPlaySpeed(24, 35.0);
		if (m_pModelCom->Get_TargetTimeAcc(24) >= 60.0 && m_bEffectEnd == false)
		{
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("IceMan_CollisionWall_44.wav"), 0.7f, true, m_pTransformCom->Get_Position());

			CEffect_Manager::GetInstance()->Create_HitStar(m_pTransformCom, 4.f);
			m_bEffectEnd = true;
		}
		bStateEnd = m_pMonster->PlayAnimations(m_pModelCom, vector<_uint>{ 24, 25, 26 }, vector<_uint>{ 25 });
		break;
	}

	if (bStateEnd == true)
		return DELETE_STATE;

	return KEEP_STATE;
}

_bool CState_MonsterKnockBack::SkullSoldierAnim(_double TimeDelta)
{
	// 18, 19, 20 Front (19 제외)
	if (m_pModelCom->Check_CurAnim_And_Finished(18))
		m_pMonster->Set_AnimAndReset(m_pModelCom, 20);

	if (m_pModelCom->Get_CurrAnimIndex() == 18)
		m_pTransformCom->Go_Backward(TimeDelta, nullptr);

	if (m_pModelCom->Check_CurAnim_And_Finished(20))
		return true;

	// 21, 22, 23 Back (22 제외)
	if (m_pModelCom->Check_CurAnim_And_Finished(21))
		m_pMonster->Set_AnimAndReset(m_pModelCom, 23);

	if (m_pModelCom->Get_CurrAnimIndex() == 21)
		m_pTransformCom->Go_Straight(TimeDelta, nullptr);

	if (m_pModelCom->Check_CurAnim_And_Finished(23))
		return true;

	return false;
}

_bool CState_MonsterKnockBack::ThinSoldierAnim(_double TimeDelta)
{
	if (m_pModelCom->Get_CurrFinished() == true)
		return true;

	return false;
}

_bool CState_MonsterKnockBack::MonkeyAnim(_double TimeDelta)
{
	if (m_pModelCom->Get_CurrFinished() == true)
		return true;

	return false;
}

CState_MonsterKnockBack* CState_MonsterKnockBack::Create(void* pArg)
{
	CState_MonsterKnockBack* pInstance = new CState_MonsterKnockBack;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

void CState_MonsterKnockBack::Free()
{
	__super::Free();
}