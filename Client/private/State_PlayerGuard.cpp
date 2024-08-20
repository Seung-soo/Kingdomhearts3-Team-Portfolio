#include "stdafx.h"
#include "State_PlayerGuard.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "StateMachine.h"
#include "State_PlayerMove.h"
#include "State_Jump.h"

#include "Effect_Manager.h"
#include "Combat_Assistant.h"
#include "Effect_Manager.h"

HRESULT CState_PlayerGuard::Initialize(void * pArg)
{
	for (_uint i = 0; i < RAGE_EFFECT_END; ++i)
	{
		m_bRageForm_Guard_Effect_Created[i] = false;

		switch (RAGE_GUARD_EFFECT(i))
		{
		case Client::CState_PlayerGuard::RAGE_EFFECT_GUARD:
			m_dRageForm_Guard_Effect_CreateTime[i] = 6.0;
			break;
		case Client::CState_PlayerGuard::RAGE_EFFECT_GUARD_SUCCEEDED:
			m_dRageForm_Guard_Effect_CreateTime[i] = 0.0;
			break;
		case Client::CState_PlayerGuard::RAGE_EFFECT_COUNTER_ATTACK:
			m_dRageForm_Guard_Effect_CreateTime[i] = 12.0;
			break;
		}
	}

	return S_OK;
}

_bool CState_PlayerGuard::KeyInput(_double TimeDelta)
{
	/*
		input���� �� ���� ����.
		�׻� true�� ��ȯ�ϰԲ� �����

		�׼ǿ��� ���ư��� ������ ���ؼ�
		������Ʈ�� �����Ѵ�.		
	*/

	// �켱 ������ �ӽ÷� ������� ��ȯ�ϴ� �Է��� �ִ´�.
	CGameInstance* inst = CGameInstance::GetInstance();

	
	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
	_bool air = m_pTransformCom->Get_JumpBoolean();

	// ��� �ߵ� ����
	_bool guard_activated = inst->Get_KeyEnter(DIK_F5);

	// ��� �ߵ�
	if (true == guard_activated)
	{
		// ������ ������ ������ �˹� ȿ���� �߻���Ű���� �����д�
		_bool knockback_activated = true;

		// ���� ���� ���
		if (false == air)
		{
			switch (curr_form)
			{
			case Client::CPlayer::SF_NORMAL:
				m_pModelCom->ResetAnimation(CPlayer::SORA_ANIM::SORA_ANIM_GUARD_SUCCEEDED);
				m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_GUARD_SUCCEEDED);

				CEffect_Manager::GetInstance()->Create_Guard(m_pTransformCom);
				break;
			case Client::CPlayer::SF_HAMMER:
				m_pModelCom->ResetAnimation(CPlayer::SORA_ANIM::SORA_ANIM_HAMMER_GUARD_SUCCEEDED);
				m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_HAMMER_GUARD_SUCCEEDED);

				CEffect_Manager::GetInstance()->Create_Guard(m_pTransformCom);
				break;
			case Client::CPlayer::SF_RAGE:
				
				// ������ �������� ����� ������ ���߻��·� �̾�����
				{
					m_pTransformCom->BasicJump(CTransform::JUMP_NORMAL);

					CState_Jump::JUMP_STATE_DESC jump_state_desc;
					jump_state_desc.eObjectType = m_pPlayer->Get_ObjectType();
					jump_state_desc.iJumpAnimIndex = 0;
					jump_state_desc.iNumberOfJumpAnim = 4;

					m_pPlayer->Create_PlayerState(CStateMachine::STATETYPE_JUMP, &jump_state_desc);
				}

				// Create_PlayerState() �ȿ��� ���� ���°� ������ �� �ִϸ��̼��� �ٲ�� ������ ���⼭ �ٽ� ���´�
				m_pModelCom->ResetAnimation(CPlayer::SORA_ANIM::SORA_ANIM_RAGE_GUARD_SUCCEEDED);
				m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_GUARD_SUCCEEDED);

				// ������ �������� �߰����� �˹� ���¸� �ο����� �ʴ´�
				knockback_activated = false;

				// ������ ���� ���� ����Ʈ
				CEffect_Manager::GetInstance()->Create_RageForm_Guard_Success(m_pTransformCom);
				break;
			}
		}

		// ���߿� ���� ���
		else
		{
			switch (curr_form)
			{
			case Client::CPlayer::SF_NORMAL:
				m_pModelCom->ResetAnimation(CPlayer::SORA_ANIM::SORA_ANIM_AIR_GUARD_SUCCEEDED);
				m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_AIR_GUARD_SUCCEEDED);

				CEffect_Manager::GetInstance()->Create_Guard(m_pTransformCom);
				break;
			case Client::CPlayer::SF_HAMMER:
				m_pModelCom->ResetAnimation(CPlayer::SORA_ANIM::SORA_ANIM_HAMMER_AIR_GUARD_SUCCEEDED);
				m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_HAMMER_AIR_GUARD_SUCCEEDED);

				CEffect_Manager::GetInstance()->Create_Guard(m_pTransformCom);
				break;
			case Client::CPlayer::SF_RAGE:
				// ������ �������� ����� ������ ���߻��·� �̾�����
				{
					m_pTransformCom->BasicJump(CTransform::JUMP_NORMAL);

					CState_Jump::JUMP_STATE_DESC jump_state_desc;
					jump_state_desc.eObjectType = m_pPlayer->Get_ObjectType();
					jump_state_desc.iJumpAnimIndex = 0;
					jump_state_desc.iNumberOfJumpAnim = 4;

					m_pPlayer->Create_PlayerState(CStateMachine::STATETYPE_JUMP, &jump_state_desc);
				}

				// Create_PlayerState() �ȿ��� ���� ���°� ������ �� �ִϸ��̼��� �ٲ�� ������ ���⼭ �ٽ� ���´�
				m_pModelCom->ResetAnimation(CPlayer::SORA_ANIM::SORA_ANIM_RAGE_GUARD_SUCCEEDED);
				m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_GUARD_SUCCEEDED);

				// ������ �������� �߰����� �˹� ���¸� �ο����� �ʴ´�
				knockback_activated = false;

				// ������ ���� ���� ����Ʈ
				CEffect_Manager::GetInstance()->Create_RageForm_Guard_Success(m_pTransformCom);
				break;
			case Client::CPlayer::SF_END:
				_breakpoint;
				break;
			}
		}

		if (true == knockback_activated)
		{
			// �˹� ���� �ڵ�

			// �˹� _float3
			/*_float3 direction;
			XMStoreFloat3(&direction, _vec3norm(m_pTransformCom->Get_Look()) * -1.f);
			m_pTransformCom->KnockBackInput(direction, 10.f, 0.25);*/

			// �˹� _vector
			_vector direction_v = _vec3norm(m_pTransformCom->Get_Look()) * -1.f;
			m_pTransformCom->KnockBackInput_v(direction_v, 1.f, 0.5);
		}
	}

	
	// ī���� �ߵ� ���θ� Ȯ���ϱ� ���ؼ� ���� �ִϸ��̼��� �޾ƿ´�
	CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);

	// ī���� �ߵ� ����
	_bool counter_activated = m_pPlayer->Get_Input(CPlayer::SI_LBUTTON);

	// ī���� �ߵ�
	if (true == counter_activated && CPlayer::SORA_ANIM_RAGE_GUARD_SUCCEEDED == curr_anim)
	{
		if (nullptr != m_pPlayer->Get_FormUI(CPlayer::FORM_FUNCTION::FORM_FUNC_COUNTER_ATTACK))
		{
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_COUNTER_ATTACK);
			m_pCombat_Assistant->Auto_Rotation();

			_double counter_duration = m_pModelCom->Get_TargetDuration(CPlayer::SORA_ANIM_RAGE_COUNTER_ATTACK);
			_double counter_play_speed = m_pModelCom->Get_TargetPlaySpeed(CPlayer::SORA_ANIM_RAGE_COUNTER_ATTACK);

			m_pPlayer->Invincibility_Input(counter_duration, counter_play_speed);
			m_pPlayer->Dead_FormUI(CPlayer::FORM_FUNCTION::FORM_FUNC_COUNTER_ATTACK);
		}
	}

	return true;
}

_bool CState_PlayerGuard::Action(_double TimeDelta)
{
	_bool input = KeyInput(TimeDelta);

	// Ű �Է��� ���� ���
	if (false == input)
	{
		ExitState();
		return DELETE_STATE;
	}

	CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);

	// ���� ���� �ִϸ��̼��� ���
	if (true == m_pStateMachine->Find_AnimIndex(m_iStateType, curr_anim))
	{
		// �ִϸ��̼��� �����ٸ�
		if (true == m_pModelCom->CheckStop_EachBody(CPlayer::SB_BODY))
		{
			ExitState();
			return DELETE_STATE;
		}

		// ī���� �ִϸ��̼��� ��
		_double time_acc = m_pModelCom->Get_TargetTimeAcc(curr_anim);
		if (CPlayer::SORA_ANIM_RAGE_COUNTER_ATTACK == curr_anim)
		{
			if ((14.0 <= time_acc && 16.0 > time_acc) ||
				(18.0 <= time_acc && 20.0 > time_acc) ||
				(22.0 <= time_acc && 24.0 > time_acc) ||
				(26.0 <= time_acc && 28.0 > time_acc) ||
				(30.0 <= time_acc && 32.0 > time_acc))
			{
				m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_BLADE, true);
			}
			else
			{
				m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_BLADE, false);
			}

			// ������ �� - ī���� ���� ����Ʈ ����
			if (false == m_bRageForm_Guard_Effect_Created[RAGE_EFFECT_COUNTER_ATTACK] &&
				time_acc >= m_dRageForm_Guard_Effect_CreateTime[RAGE_EFFECT_COUNTER_ATTACK])
			{
				CEffect_Manager* em = CEffect_Manager::GetInstance();
				em->Create_RageForm_CounterAttack_Weapon(m_pPlayer_Weapon->Get_Transform()->Get_WorldFloat4x4_Pointer());

				m_bRageForm_Guard_Effect_Created[RAGE_EFFECT_COUNTER_ATTACK] = true;
			}
			
		}
		
		// ������ �� - ���� ����Ʈ ����
		if (CPlayer::SORA_ANIM_RAGE_GUARD == curr_anim ||
			CPlayer::SORA_ANIM_RAGE_AIR_GUARD == curr_anim)
		{
			if (false == m_bRageForm_Guard_Effect_Created[RAGE_EFFECT_GUARD] && 
				time_acc >= m_dRageForm_Guard_Effect_CreateTime[RAGE_EFFECT_GUARD])
			{
				CEffect_Manager* em = CEffect_Manager::GetInstance();

				_vector offset = m_pPlayer->Get_BodyCollider_Center() - m_pTransformCom->Get_Position();
				vecx(offset) = -0.15f;
				vecz(offset) = +0.0f;
				vecw(offset) = +1.0f;

				em->Create_RageForm_Guard(m_pTransformCom, offset);

				m_bRageForm_Guard_Effect_Created[RAGE_EFFECT_GUARD] = true;
			}
		}
	}

	// ���� ���� �ִϸ��̼��� �ƴ� ���
	else
	{
		if (CPlayer::SORA_ANIM::SORA_ANIM_FALL_END == curr_anim ||
			CPlayer::SORA_ANIM::SORA_ANIM_RAGE_FALL_END == curr_anim ||
			CPlayer::SORA_ANIM::SORA_ANIM_HAMMER_FALL_END == curr_anim)
		{
			ExitState();
			return DELETE_STATE;
		}
	}

	return KEEP_STATE;
}

HRESULT CState_PlayerGuard::EnterState()
{
	// �÷��̾� ����
	m_pPlayer = static_cast<CPlayer*>(m_pGameObject);

	// �÷��̾� ���� ���� ����
	m_pCombat_Assistant = m_pPlayer->Get_CombatAssistant();
	m_pAction_Assistant = m_pPlayer->Get_ActionAssistant();
	m_pPlayer_Weapon = m_pPlayer->Get_Weapon();

	// ���� ���� ü�� ���¸� �޾ƿ´�
	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
	_bool air = m_pTransformCom->Get_JumpBoolean();

	// ���� ���� ���
	if (false == air)
	{
		switch (curr_form)
		{
		case Client::CPlayer::SF_NORMAL:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_GUARD);
			break;
		case Client::CPlayer::SF_HAMMER:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_HAMMER_GUARD);
			break;
		case Client::CPlayer::SF_RAGE:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_GUARD);
			break;
		}
	}

	// ���߿� ���� ���
	else
	{
		switch (curr_form)
		{
		case Client::CPlayer::SF_NORMAL:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_AIR_GUARD);
			break;
		case Client::CPlayer::SF_HAMMER:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_HAMMER_AIR_GUARD);
			break;
		case Client::CPlayer::SF_RAGE:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_AIR_GUARD);
			break;
		case Client::CPlayer::SF_END:
			_breakpoint;
			break;
		}
	}

	// �̵�, ���� ������Ʈ�� ���� ��� �ش� ������Ʈ�� ����
	m_pStateMachine->ClearState(CStateMachine::STATETYPE_NORMALMOVE);
	m_pStateMachine->ClearState(CStateMachine::STATETYPE_ATK);

	// ���� ���(������ ���� ���� ����ó�� �ɷ�����)
	m_pPlayer->Play_ActionVoiceSound(0.75f);

	// �������� ��� ���� ���� ���
	if (CPlayer::SF_RAGE == curr_form)
		m_pPlayer->Play_RageGuard();

	return S_OK;
}

HRESULT CState_PlayerGuard::ExitState()
{
	/*
		�ִϸ��̼��� ��ȯ�ؾ��Ѵ�.

		1. ���� �ִٸ�
		-> Ű �Է��� �ִ� : �̵�
			-> ���⸦ ����ִ�
			-> ���⸦ ������� �ʴ�
		-> Ű �Է��� ���� : ���
			-> ����t�� ����ִ�
			-> ���⸦ ������� �ʴ�

		=> ����� ���⸦ �׻� ������� �� �ۿ� ���� �� ��û�� �ڽľ�.

		2. ���߿� �ִٸ�
		 -> ���� �ִϸ��̼�

		�̹� ����������Ʈ �����Ǿ�����.
	*/

	_bool air = m_pTransformCom->Get_JumpBoolean();
	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();

	// ���� ���� ���
	if (false == air)
	{
		switch (curr_form)
		{
		case Client::CPlayer::SF_NORMAL:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_IDLE_BLADE);
			break;
		case Client::CPlayer::SF_HAMMER:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_HAMMER_IDLE_SHORT);
			break;
		case Client::CPlayer::SF_RAGE:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_IDLE);
			break;
		case Client::CPlayer::SF_END:
		default:

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
			break;
		case Client::CPlayer::SF_HAMMER:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_HAMMER_FALL_LOOP);
			break;
		case Client::CPlayer::SF_RAGE:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_FALL_LOOP);
			break;
		case Client::CPlayer::SF_END:
		default:

			_breakpoint;

			break;
		}
	}

	// ���� Ÿ�̸� �ʱ�ȭ
	m_pPlayer->Set_GuardTimer(0.0);

	return S_OK;
}

HRESULT CState_PlayerGuard::ClearState()
{
	// ���� Ÿ�̸� �ʱ�ȭ
	m_pPlayer->Set_GuardTimer(0.0);

	return S_OK;
}

CState_PlayerGuard * CState_PlayerGuard::Create(void * pArg)
{
	CState_PlayerGuard* pInstance = new CState_PlayerGuard;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

void CState_PlayerGuard::Free()
{
	__super::Free();
}
