#include "stdafx.h"
#include "..\public\State_PlayerInteraction.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Player_Weapon.h"

HRESULT CState_PlayerInteraction::Initialize(void * pArg)
{
	return S_OK;
}

_bool CState_PlayerInteraction::KeyInput(_double TimeDelta)
{
	// 딱히 입력받을 것 없음

	return true;
}

_bool CState_PlayerInteraction::Action(_double TimeDelta)
{
	if (false == KeyInput(TimeDelta))
	{
		ExitState();
		return DELETE_STATE;
	}

	// 애니메이션 끝났을 경우
	if (true == m_pModelCom->CheckStop_EachBody(CPlayer::SB_BODY))
	{
		ExitState();
		return DELETE_STATE;
	}

	return KEEP_STATE;
}

HRESULT CState_PlayerInteraction::EnterState()
{
	m_pPlayer = static_cast<CPlayer*>(m_pGameObject);
	m_pCombat_Assistant = m_pPlayer->Get_CombatAssistant();
	m_pAction_Assistant = m_pPlayer->Get_ActionAssistant();
	m_pPlayer_Weapon = m_pPlayer->Get_Weapon();

	m_eInitialAnim = CPlayer::SORA_ANIM::SORA_ANIM_OPEN_BIG;
	m_pPlayer->Change_PlayerAnimation(m_eInitialAnim);

	return S_OK;
}

HRESULT CState_PlayerInteraction::ExitState()
{
	m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_IDLE);

	return S_OK;
}

CState_PlayerInteraction * CState_PlayerInteraction::Create(void * pArg)
{
	CState_PlayerInteraction* pInstance = new CState_PlayerInteraction;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

void CState_PlayerInteraction::Free()
{
	__super::Free();
}
