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
		������ ������ ���
		Ű ���̵� ���� �� ���� ��� ���·� ��ȯ�Ѵ�.

		���� UI�� ���õǾ��ִ� ���.
		���� ���Ⱑ �� ���� �ƴ� ���. ( ���� ����� ��ȯ�ϸ鼭 ���� ��ȯ ���°� �߻��ϰ� �ڿ������� ���ο� ���⸦ ������ �ִϸ��̼��� ����� )
	*/

	// KeyBlade Lock�� �ɷȴٸ� return
	if (true == GM->Get_KeyBladeLock())
		return true;

	// Ű���� ������ ��ư �Է��� ������ return
	if (false == m_pPlayer->Get_Input(CPlayer::SI_RIGHT))
		return true;

	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
	switch (curr_form)
	{
	case Client::CPlayer::SF_NORMAL:

		// ��� ���� �ܿ� �ٸ� ���°� �ִٸ� return
		if (1 != m_pStateMachine->Get_NumberOfStates())
			return true;

		m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_DRAW_BLADE);

		break;
	case Client::CPlayer::SF_HAMMER:

		// ��� ���� �ܿ� �ٸ� ���°� �ִٸ� return
		if (2 != m_pStateMachine->Get_NumberOfStates())
			return true;

		m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_HAMMER_TAKE_OUT);

		break;
	case Client::CPlayer::SF_RAGE:

		// ��� ���� �ܿ� �ٸ� ���°� �ִٸ� return
		if (2 != m_pStateMachine->Get_NumberOfStates())
			return true;

		// ������ ���� �׻� KINGDOM_KEY�� �������. ó���� ���� ����.

		break;
	}

	return true;
}

_bool CState_PlayerIdle::Action(_double TimeDelta)
{
	// ��� ���´� �Է��� ��� ���¸� ������ �ʴ´�.
	KeyInput(TimeDelta);

	// ���߿� �ִٸ� ��� ���� �ƴ�
	if (true == m_pTransformCom->Get_JumpBoolean())
		return KEEP_STATE;

	// �� ��ȭ�� �������� Ȯ��
	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
	switch (curr_form)
	{
	case Client::CPlayer::SF_NORMAL:

		// ���°� Idle�ܿ��� �����Ѵٸ� : ���� Idle ���°� �ƴ�
		if (1 != m_pStateMachine->Get_NumberOfStates())
			return KEEP_STATE;

		break;
	case Client::CPlayer::SF_HAMMER:

		// ���°� Idle, Form�ܿ��� �����Ѵٸ� �� ��ȭ�� ������ Idle ���°� �ƴ�
		if (2 != m_pStateMachine->Get_NumberOfStates())
			return KEEP_STATE;

		break;
	case Client::CPlayer::SF_RAGE:

		// ���°� Idle, Form�ܿ��� �����Ѵٸ� �� ��ȭ�� ������ Idle ���°� �ƴ�
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

		// Rage : ������ ���� ��� ���¿����� ó���� ���� ����
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

		// ��� ���� �ִϸ��̼��� �ƴ� �ٸ� �ִϸ��̼��� ���� ���
		// �ִϸ��̼��� ����Ǵ� ���� �ѹ��� ���� �� �ִ�
		switch (curr_anim_type)
		{
		case Engine::CAnimation::ANIM_ONCE:

			// �ش� �ִϸ��̼��� �̹� �����ӿ� �ٲ� ���� �ƴϰ� �ִϸ��̼��� ����Ǿ��� ��
			if (true == anim_done && true == curr_and_temp)
				Change_To_IdleAnimation();

			break;
		case Engine::CAnimation::ANIM_LOOP:

			// Loop �ִϸ��̼��� �ٷ� ��ȯ��Ų��
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
