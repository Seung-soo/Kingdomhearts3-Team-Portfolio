#include "stdafx.h"
#include "State_PlayerIdle.h"
#include "GameInstance.h"
#include "GameManager.h"

_double CState_PlayerIdle::g_TimeLength_To_Boring = 5.0;

HRESULT CState_PlayerIdle::Initialize(void * pArg)
{
	return S_OK;
}

_bool CState_PlayerIdle::KeyInput(_double TimeDelta)
{
	/*
		조건이 충족될 경우
		키 블레이드 뽑은 후 무기 대기 상태로 전환한다.

		공격 UI가 선택되어있는 경우.
		현재 무기가 한 개가 아닐 경우. ( 다음 무기로 전환하면서 무기 전환 상태가 발생하고 자연스럽게 새로운 무기를 꺼내는 애니메이션이 재생됨 )
	*/

	// KeyBlade Lock이 걸렸다면 return
	if (true == GM->Get_KeyBladeLock())
		return true;

	// 키보드 오른쪽 버튼 입력이 없으면 return
	if (false == m_pPlayer->Get_Input(CPlayer::SI_RIGHT))
		return true;

	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
	switch (curr_form)
	{
	case Client::CPlayer::SF_NORMAL:

		// 대기 상태 외에 다른 상태가 있다면 return
		if (1 != m_pStateMachine->Get_NumberOfStates())
			return true;

		m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_DRAW_BLADE);

		break;
	case Client::CPlayer::SF_HAMMER:

		// 대기 상태 외에 다른 상태가 있다면 return
		if (2 != m_pStateMachine->Get_NumberOfStates())
			return true;

		m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_HAMMER_TAKE_OUT);

		break;
	case Client::CPlayer::SF_RAGE:

		// 대기 상태 외에 다른 상태가 있다면 return
		if (2 != m_pStateMachine->Get_NumberOfStates())
			return true;

		// 레이지 폼은 항상 KINGDOM_KEY를 들고있음. 처리할 것이 없다.

		break;
	}

	return true;
}

_bool CState_PlayerIdle::Action(_double TimeDelta)
{
	// 대기 상태는 입력이 없어도 상태를 지우지 않는다.
	KeyInput(TimeDelta);

	// 공중에 있다면 대기 상태 아님
	if (true == m_pTransformCom->Get_JumpBoolean())
		return KEEP_STATE;

	// 폼 변화된 상태인지 확인
	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
	switch (curr_form)
	{
	case Client::CPlayer::SF_NORMAL:

		// 상태가 Idle외에도 존재한다면 : 순수 Idle 상태가 아님
		if (1 != m_pStateMachine->Get_NumberOfStates())
			return KEEP_STATE;

		break;
	case Client::CPlayer::SF_HAMMER:

		// 상태가 Idle, Form외에도 존재한다면 폼 변화된 상태의 Idle 상태가 아님
		if (2 != m_pStateMachine->Get_NumberOfStates())
			return KEEP_STATE;

		break;
	case Client::CPlayer::SF_RAGE:

		// 상태가 Idle, Form외에도 존재한다면 폼 변화된 상태의 Idle 상태가 아님
		if (2 != m_pStateMachine->Get_NumberOfStates())
			return KEEP_STATE;

		break;
	}

	_bool curr_and_temp = m_pModelCom->Check_TempAndCurr_EachBody(CPlayer::SB_BODY);

	if (false == curr_and_temp)
		return KEEP_STATE;

	CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);
	CPlayer::SORA_ANIM prev_anim = (CPlayer::SORA_ANIM)m_pModelCom->Get_PrevAnimIndex_EachBody(CPlayer::SB_BODY);
	_bool anim_done = m_pModelCom->Get_TargetStopped(curr_anim);

	_bool boring = g_TimeLength_To_Boring <= m_dTimer ? true : false;
	_bool hovering = m_pPlayer->Get_Hovering();

	CAnimation::ANIM_TYPE curr_anim_type = m_pModelCom->Get_TargetAnimType(curr_anim);

	switch (curr_anim)
	{
	case CPlayer::SORA_ANIM_IDLE:

		if (true == hovering)
		{
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_AIR_IDLE);
			break;
		}

		if (true == boring)
		{
			_bool holding_weapon = m_pPlayer->Get_HoldingWeapon();

			if (false == holding_weapon)
			{
				m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_BORING);
				m_dTimer = 0.0;
			}

			else
			{
				m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_BORING_BLADE);
				m_dTimer = 0.0;
			}
		}

		m_dTimer += TimeDelta;

		break;
	case CPlayer::SORA_ANIM_BORING:

		if (true == anim_done)
		{
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_IDLE);
			m_dTimer = 0.0;
		}

		break;
	case CPlayer::SORA_ANIM_DRAW_BLADE:

		if (true == anim_done)
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_IDLE_BLADE);

		break;
	case CPlayer::SORA_ANIM_IDLE_BLADE:

		if (true == anim_done)
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_IDLE_BLADE_END);

		break;
	case CPlayer::SORA_ANIM_IDLE_BLADE_END:

		if (true == anim_done)
		{
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_IDLE);
			m_dTimer = 0.0;
		}

		break;
	case CPlayer::SORA_ANIM_BORING_BLADE:

		if (true == anim_done)
		{
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_IDLE);
			m_dTimer = 0.0;

			m_pPlayer->Sheathe_Weapon();
		}

		break;

		// Rage : 레이지 폼의 대기 상태에서는 처리할 것이 없음
	case CPlayer::SORA_ANIM_RAGE_IDLE:

		if (true == hovering)
		{
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_AIR_IDLE);
			break;
		}

		break;

		// Hammer
	case CPlayer::SORA_ANIM_HAMMER_IDLE_SHORT:

		if (true == hovering)
		{
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_AIR_IDLE);
			break;
		}

		if (true == anim_done)
		{
			if(CPlayer::SORA_ANIM_HAMMER_IDLE_LONG == prev_anim)
				m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_HAMMER_PUT_DOWN);
			else
				m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_HAMMER_IDLE_LONG);
		}

		break;
	case CPlayer::SORA_ANIM_HAMMER_IDLE_LONG:

		if (true == hovering)
		{
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_AIR_IDLE);
			break;
		}

		if (true == anim_done)
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_HAMMER_IDLE_SHORT);

		break;
	case CPlayer::SORA_ANIM_HAMMER_TAKE_OUT:

		if (true == anim_done)
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_HAMMER_IDLE_SHORT);

		break;
	case CPlayer::SORA_ANIM_HAMMER_PUT_DOWN:

		if (true == anim_done)
		{
			m_pPlayer->Sheathe_Weapon();
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_IDLE);
		}

		break;
	default:

		// 대기 관련 애니메이션이 아닌 다른 애니메이션이 들어온 경우
		// 애니메이션이 변경되는 순간 한번씩 들어올 수 있다
		switch (curr_anim_type)
		{
		case Engine::CAnimation::ANIM_ONCE:

			// 해당 애니메이션이 이번 프레임에 바뀐 것이 아니고 애니메이션이 종료되었을 때
			if (true == anim_done && true == curr_and_temp)
				Change_To_IdleAnimation();

			break;
		case Engine::CAnimation::ANIM_LOOP:

			// Loop 애니메이션은 바로 전환시킨다
			Change_To_IdleAnimation();
			
			break;
		}

		break;
	}

	return KEEP_STATE;
}

HRESULT CState_PlayerIdle::EnterState()
{
	m_pPlayer = static_cast<CPlayer*>(m_pGameObject);
	m_pCombat_Assistant = m_pPlayer->Get_CombatAssistant();
	m_pAction_Assistant = m_pPlayer->Get_ActionAssistant();
	m_pPlayer_Weapon = m_pPlayer->Get_Weapon();

	return S_OK;
}

HRESULT CState_PlayerIdle::ExitState()
{
	return S_OK;
}

void CState_PlayerIdle::Change_To_IdleAnimation()
{
	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
	switch (curr_form)
	{
	case Client::CPlayer::SF_NORMAL:
		m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_IDLE);

		break;
	case Client::CPlayer::SF_HAMMER:
		m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_HAMMER_IDLE_SHORT);

		break;
	case Client::CPlayer::SF_RAGE:
		m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_IDLE);

		break;
	}

	if(true == m_pPlayer->Get_Hovering())
		m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_AIR_IDLE);

	m_dTimer = 0.0;
}

CState_PlayerIdle * CState_PlayerIdle::Create(void * pArg)
{
	CState_PlayerIdle* pInstance = new CState_PlayerIdle;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

void CState_PlayerIdle::Free()
{
	__super::Free();
}
