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
		input에서 할 것이 없다.
		항상 true를 반환하게끔 만들고

		액션에서 돌아가는 로직에 의해서
		스테이트를 삭제한다.

		===============================

		원래대로라면 피격 상태 중에도
		연계 스킬이나 세컨드 폼 같은 것을 발동할 수 있다.

		해당 input을 여기서 처리하는게 좋을지는 배제.
	*/

	return true;
}

_bool CState_PlayerDamaged::Action(_double TimeDelta)
{
	_bool input = KeyInput(TimeDelta);

	// 키 입력 여부에 의한 삭제가 발생
	if (false == input)
	{
		ExitState();
		return DELETE_STATE;
	}

	// 애니메이션 재생이 완료되지 않았다면 return
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

	// 애니메이션 재생이 완료된 경우
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
	// 플레이어 자료형으로 형변환해서 주소저장
	m_pPlayer = static_cast<CPlayer*>(m_pGameObject);
	m_pCombat_Assistant = m_pPlayer->Get_CombatAssistant();
	m_pAction_Assistant = m_pPlayer->Get_ActionAssistant();
	m_pPlayer_Weapon = m_pPlayer->Get_Weapon();

	m_pPlayer->Change_PlayerAnimation((CPlayer::SORA_ANIM)m_DamagedDesc.sora_anim);
	m_dTestTimer = 1.5;

	// 여기에서 피격 위치, 공격 타입에 따라서 다른 애니메이션으로 전환해줘야한다.
	m_pStateMachine->Clear_CrashTypes((CStateMachine::STATETYPE)m_iStateType);
	m_pStateMachine->ClearState(CStateMachine::STATETYPE_SUMMON);

	return S_OK;
}
HRESULT CState_PlayerDamaged::ExitState()
{
	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
	_bool air = m_pTransformCom->Get_JumpBoolean();

	// 지상에 있을 경우
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

	// 공중에 있을 경우
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
