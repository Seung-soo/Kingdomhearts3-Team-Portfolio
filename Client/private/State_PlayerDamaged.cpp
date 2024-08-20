#include "stdafx.h"
#include "State_PlayerDamaged.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "StateMachine.h"
#include "Action_Assistant.h"

HRESULT CState_PlayerDamaged::Initialize(void * pArg)
{
	if (nullptr == pArg)
	{
		BREAKPOINT;
	}

	memcpy(&m_DamagedDesc, pArg, sizeof(PLAYER_DAMAGED_DESC));

	return S_OK;
}

_bool CState_PlayerDamaged::KeyInput(_double TimeDelta)
{
	/*
		input���� �� ���� ����.
		�׻� true�� ��ȯ�ϰԲ� �����

		�׼ǿ��� ���ư��� ������ ���ؼ�
		������Ʈ�� �����Ѵ�.

		===============================

		������ζ�� �ǰ� ���� �߿���
		���� ��ų�̳� ������ �� ���� ���� �ߵ��� �� �ִ�.

		�ش� input�� ���⼭ ó���ϴ°� �������� ����.
	*/

	return true;
}

_bool CState_PlayerDamaged::Action(_double TimeDelta)
{
	_bool input = KeyInput(TimeDelta);

	// Ű �Է� ���ο� ���� ������ �߻�
	if (false == input)
	{
		ExitState();
		return DELETE_STATE;
	}

	// �ִϸ��̼� ����� �Ϸ���� �ʾҴٸ� return
	if (false == m_pModelCom->CheckStop_Input(m_DamagedDesc.sora_anim))
	{
		if (CPlayer::SORA_ANIM_AIR_DAMAGED_AERIAL_F_LOOP != m_DamagedDesc.sora_anim &&
			CPlayer::SORA_ANIM_AIR_DAMAGED_AERIAL_B_LOOP != m_DamagedDesc.sora_anim &&
			CPlayer::SORA_ANIM_AIR_DAMAGED_AERIAL_R_LOOP != m_DamagedDesc.sora_anim &&
			CPlayer::SORA_ANIM_AIR_DAMAGED_AERIAL_L_LOOP != m_DamagedDesc.sora_anim &&
			CPlayer::SORA_ANIM_STUN_LOOP != m_DamagedDesc.sora_anim)
		{
			return KEEP_STATE;
		}
	}

	_uint anim_index = m_DamagedDesc.sora_anim;

	// �ִϸ��̼� ����� �Ϸ�� ���
	switch (m_DamagedDesc.sora_anim)
	{
	case Client::CPlayer::SORA_ANIM_DAMAGED_NORMAL_FU:
	case Client::CPlayer::SORA_ANIM_DAMAGED_NORMAL_BU:
	case Client::CPlayer::SORA_ANIM_DAMAGED_NORMAL_R:
	case Client::CPlayer::SORA_ANIM_DAMAGED_NORMAL_L:
	case Client::CPlayer::SORA_ANIM_DAMAGED_NORMAL_FD:
	case Client::CPlayer::SORA_ANIM_DAMAGED_NORMAL_BD:
	case Client::CPlayer::SORA_ANIM_AIR_DAMAGED_NORMAL_F:
	case Client::CPlayer::SORA_ANIM_AIR_DAMAGED_NORMAL_B:
	case Client::CPlayer::SORA_ANIM_AIR_DAMAGED_NORMAL_R:
	case Client::CPlayer::SORA_ANIM_AIR_DAMAGED_NORMAL_L:

	case Client::CPlayer::SORA_ANIM_AIR_DAMAGED_AERIAL_F_EXIT:
	case Client::CPlayer::SORA_ANIM_AIR_DAMAGED_AERIAL_B_EXIT:
	case Client::CPlayer::SORA_ANIM_AIR_DAMAGED_AERIAL_R_EXIT:
	case Client::CPlayer::SORA_ANIM_AIR_DAMAGED_AERIAL_L_EXIT:

	case Client::CPlayer::SORA_ANIM_DUMP_BACK:
	case Client::CPlayer::SORA_ANIM_DUMP_FRONT:
	case Client::CPlayer::SORA_ANIM_DUMP_LEFT:
	case Client::CPlayer::SORA_ANIM_DUMP_RIGHT:

	case Client::CPlayer::SORA_ANIM_STUN_EXIT:

		// RAGE
	case Client::CPlayer::SORA_ANIM_RAGE_DAMAGED_F:
	case Client::CPlayer::SORA_ANIM_RAGE_DAMAGED_B:
	case Client::CPlayer::SORA_ANIM_RAGE_DAMAGED_R:
	case Client::CPlayer::SORA_ANIM_RAGE_DAMAGED_L:

	case Client::CPlayer::SORA_ANIM_RAGE_AIR_DAMAGED_F:
	case Client::CPlayer::SORA_ANIM_RAGE_AIR_DAMAGED_B:
	case Client::CPlayer::SORA_ANIM_RAGE_AIR_DAMAGED_R:
	case Client::CPlayer::SORA_ANIM_RAGE_AIR_DAMAGED_L:

	case Client::CPlayer::SORA_ANIM_RAGE_DAMAGED_SLIDE_F:
	case Client::CPlayer::SORA_ANIM_RAGE_DAMAGED_SLIDE_B:
	case Client::CPlayer::SORA_ANIM_RAGE_DAMAGED_SLIDE_R:
	case Client::CPlayer::SORA_ANIM_RAGE_DAMAGED_SLIDE_L:

		// HAMMER
	case Client::CPlayer::SORA_ANIM_HAMMER_DAMAGED_F:
	case Client::CPlayer::SORA_ANIM_HAMMER_DAMAGED_B:
	case Client::CPlayer::SORA_ANIM_HAMMER_DAMAGED_R:
	case Client::CPlayer::SORA_ANIM_HAMMER_DAMAGED_L:

	case Client::CPlayer::SORA_ANIM_HAMMER_AIR_DAMAGED_F:
	case Client::CPlayer::SORA_ANIM_HAMMER_AIR_DAMAGED_B:
	case Client::CPlayer::SORA_ANIM_HAMMER_AIR_DAMAGED_R:
	case Client::CPlayer::SORA_ANIM_HAMMER_AIR_DAMAGED_L:

	case Client::CPlayer::SORA_ANIM_HAMMER_DAMAGED_SLIDE_F:
	case Client::CPlayer::SORA_ANIM_HAMMER_DAMAGED_SLIDE_B:
	case Client::CPlayer::SORA_ANIM_HAMMER_DAMAGED_SLIDE_R:
	case Client::CPlayer::SORA_ANIM_HAMMER_DAMAGED_SLIDE_L:

		ExitState();
		return DELETE_STATE;

	case Client::CPlayer::SORA_ANIM_AIR_DAMAGED_AERIAL_F_ENTER:
	case Client::CPlayer::SORA_ANIM_AIR_DAMAGED_AERIAL_B_ENTER:
	case Client::CPlayer::SORA_ANIM_AIR_DAMAGED_AERIAL_R_ENTER:
	case Client::CPlayer::SORA_ANIM_AIR_DAMAGED_AERIAL_L_ENTER:

	case Client::CPlayer::SORA_ANIM_STUN_ENTER:

		anim_index++;
		m_DamagedDesc.sora_anim = (CPlayer::SORA_ANIM)anim_index;

		m_pPlayer->Change_PlayerAnimation((CPlayer::SORA_ANIM)m_DamagedDesc.sora_anim);

		break;
	case Client::CPlayer::SORA_ANIM_AIR_DAMAGED_AERIAL_F_LOOP:
	case Client::CPlayer::SORA_ANIM_AIR_DAMAGED_AERIAL_B_LOOP:
	case Client::CPlayer::SORA_ANIM_AIR_DAMAGED_AERIAL_R_LOOP:
	case Client::CPlayer::SORA_ANIM_AIR_DAMAGED_AERIAL_L_LOOP:

	case Client::CPlayer::SORA_ANIM_STUN_LOOP:

		m_dTestTimer -= TimeDelta;

		if (0.0 >= m_dTestTimer)
		{
			anim_index++;
			m_DamagedDesc.sora_anim = (CPlayer::SORA_ANIM)anim_index;

			m_pPlayer->Change_PlayerAnimation((CPlayer::SORA_ANIM)m_DamagedDesc.sora_anim);
		}

		break;
	}

	return KEEP_STATE;
}

HRESULT CState_PlayerDamaged::EnterState()
{
	// �÷��̾� �ڷ������� ����ȯ�ؼ� �ּ�����
	m_pPlayer = static_cast<CPlayer*>(m_pGameObject);
	m_pCombat_Assistant = m_pPlayer->Get_CombatAssistant();
	m_pAction_Assistant = m_pPlayer->Get_ActionAssistant();
	m_pPlayer_Weapon = m_pPlayer->Get_Weapon();

	m_pPlayer->Change_PlayerAnimation((CPlayer::SORA_ANIM)m_DamagedDesc.sora_anim);
	m_dTestTimer = 1.5;

	// ���⿡�� �ǰ� ��ġ, ���� Ÿ�Կ� ���� �ٸ� �ִϸ��̼����� ��ȯ������Ѵ�.
	m_pStateMachine->Clear_CrashTypes((CStateMachine::STATETYPE)m_iStateType);
	m_pStateMachine->ClearState(CStateMachine::STATETYPE_SUMMON);

	return S_OK;
}
HRESULT CState_PlayerDamaged::ExitState()
{
	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
	_bool air = m_pTransformCom->Get_JumpBoolean();

	// ���� ���� ���
	if (false == air)
	{
		switch (curr_form)
		{
		case Client::CPlayer::SF_NORMAL:

			if (false == m_pPlayer->Get_HoldingWeapon())
				m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_IDLE);
			else
				m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_IDLE_BLADE);

			break;
		case Client::CPlayer::SF_HAMMER:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_HAMMER_IDLE_SHORT);
			break;
		case Client::CPlayer::SF_RAGE:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_IDLE);
			break;
		case Client::CPlayer::SF_END:
			_breakpoint;
			break;
		}
	}

	// ���߿� ���� ���
	else
	{
		switch (curr_form)
		{
		case Client::CPlayer::SF_NORMAL:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_FALL_LOOP);
			m_pTransformCom->BasicJump(CTransform::JUMP_FALL);
			break;
		case Client::CPlayer::SF_HAMMER:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_HAMMER_FALL_LOOP);
			m_pTransformCom->BasicJump(CTransform::JUMP_FALL);
			break;
		case Client::CPlayer::SF_RAGE:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_FALL_LOOP);
			m_pTransformCom->BasicJump(CTransform::JUMP_FALL);
			break;
		case Client::CPlayer::SF_END:
			_breakpoint;
			break;
		}
	}

	m_pPlayer->Invincibility_Input(0.5, 1.0);

	return S_OK;
}

HRESULT CState_PlayerDamaged::ClearState()
{
	for (_uint i = 0; i < A_ASSIST::ASSIST_TYPE::ASSIST_END; ++i)
		m_pAction_Assistant->Able((A_ASSIST::ASSIST_TYPE)i, true);

	return S_OK;
}

CState_PlayerDamaged * CState_PlayerDamaged::Create(void * pArg)
{
	CState_PlayerDamaged* pInstance = new CState_PlayerDamaged;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

void CState_PlayerDamaged::Free()
{
	__super::Free();
}
