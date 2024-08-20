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
		input에서 할 것이 없다.
		항상 true를 반환하게끔 만들고

		액션에서 돌아가는 로직에 의해서
		스테이트를 삭제한다.		
	*/

	// 우선 지금은 임시로 방어모션을 전환하는 입력을 넣는다.
	CGameInstance* inst = CGameInstance::GetInstance();

	
	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
	_bool air = m_pTransformCom->Get_JumpBoolean();

	// 방어 발동 조건
	_bool guard_activated = inst->Get_KeyEnter(DIK_F5);

	// 방어 발동
	if (true == guard_activated)
	{
		// 조건이 충족될 때에만 넉백 효과를 발생시키위해 만들어둔다
		_bool knockback_activated = true;

		// 지상에 있을 경우
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
				
				// 레이지 폼에서의 가드는 무조건 공중상태로 이어진다
				{
					m_pTransformCom->BasicJump(CTransform::JUMP_NORMAL);

					CState_Jump::JUMP_STATE_DESC jump_state_desc;
					jump_state_desc.eObjectType = m_pPlayer->Get_ObjectType();
					jump_state_desc.iJumpAnimIndex = 0;
					jump_state_desc.iNumberOfJumpAnim = 4;

					m_pPlayer->Create_PlayerState(CStateMachine::STATETYPE_JUMP, &jump_state_desc);
				}

				// Create_PlayerState() 안에서 점프 상태가 생성될 때 애니메이션이 바뀌기 때문에 여기서 다시 덮는다
				m_pModelCom->ResetAnimation(CPlayer::SORA_ANIM::SORA_ANIM_RAGE_GUARD_SUCCEEDED);
				m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_GUARD_SUCCEEDED);

				// 레이지 폼에서는 추가적인 넉백 상태를 부여하지 않는다
				knockback_activated = false;

				// 레이지 가드 성공 이펙트
				CEffect_Manager::GetInstance()->Create_RageForm_Guard_Success(m_pTransformCom);
				break;
			}
		}

		// 공중에 있을 경우
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
				// 레이지 폼에서의 가드는 무조건 공중상태로 이어진다
				{
					m_pTransformCom->BasicJump(CTransform::JUMP_NORMAL);

					CState_Jump::JUMP_STATE_DESC jump_state_desc;
					jump_state_desc.eObjectType = m_pPlayer->Get_ObjectType();
					jump_state_desc.iJumpAnimIndex = 0;
					jump_state_desc.iNumberOfJumpAnim = 4;

					m_pPlayer->Create_PlayerState(CStateMachine::STATETYPE_JUMP, &jump_state_desc);
				}

				// Create_PlayerState() 안에서 점프 상태가 생성될 때 애니메이션이 바뀌기 때문에 여기서 다시 덮는다
				m_pModelCom->ResetAnimation(CPlayer::SORA_ANIM::SORA_ANIM_RAGE_GUARD_SUCCEEDED);
				m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_GUARD_SUCCEEDED);

				// 레이지 폼에서는 추가적인 넉백 상태를 부여하지 않는다
				knockback_activated = false;

				// 레이지 가드 성공 이펙트
				CEffect_Manager::GetInstance()->Create_RageForm_Guard_Success(m_pTransformCom);
				break;
			case Client::CPlayer::SF_END:
				_breakpoint;
				break;
			}
		}

		if (true == knockback_activated)
		{
			// 넉백 예제 코드

			// 넉백 _float3
			/*_float3 direction;
			XMStoreFloat3(&direction, _vec3norm(m_pTransformCom->Get_Look()) * -1.f);
			m_pTransformCom->KnockBackInput(direction, 10.f, 0.25);*/

			// 넉백 _vector
			_vector direction_v = _vec3norm(m_pTransformCom->Get_Look()) * -1.f;
			m_pTransformCom->KnockBackInput_v(direction_v, 1.f, 0.5);
		}
	}

	
	// 카운터 발동 여부를 확인하기 위해서 현재 애니메이션을 받아온다
	CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);

	// 카운터 발동 조건
	_bool counter_activated = m_pPlayer->Get_Input(CPlayer::SI_LBUTTON);

	// 카운터 발동
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

	// 키 입력이 없을 경우
	if (false == input)
	{
		ExitState();
		return DELETE_STATE;
	}

	CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);

	// 가드 관련 애니메이션일 경우
	if (true == m_pStateMachine->Find_AnimIndex(m_iStateType, curr_anim))
	{
		// 애니메이션이 끝났다면
		if (true == m_pModelCom->CheckStop_EachBody(CPlayer::SB_BODY))
		{
			ExitState();
			return DELETE_STATE;
		}

		// 카운터 애니메이션일 때
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

			// 레이지 폼 - 카운터 어택 이펙트 생성
			if (false == m_bRageForm_Guard_Effect_Created[RAGE_EFFECT_COUNTER_ATTACK] &&
				time_acc >= m_dRageForm_Guard_Effect_CreateTime[RAGE_EFFECT_COUNTER_ATTACK])
			{
				CEffect_Manager* em = CEffect_Manager::GetInstance();
				em->Create_RageForm_CounterAttack_Weapon(m_pPlayer_Weapon->Get_Transform()->Get_WorldFloat4x4_Pointer());

				m_bRageForm_Guard_Effect_Created[RAGE_EFFECT_COUNTER_ATTACK] = true;
			}
			
		}
		
		// 레이지 폼 - 가드 이펙트 생성
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

	// 가드 관련 애니메이션이 아닐 경우
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
	// 플레이어 참조
	m_pPlayer = static_cast<CPlayer*>(m_pGameObject);

	// 플레이어 관련 변수 참조
	m_pCombat_Assistant = m_pPlayer->Get_CombatAssistant();
	m_pAction_Assistant = m_pPlayer->Get_ActionAssistant();
	m_pPlayer_Weapon = m_pPlayer->Get_Weapon();

	// 현재 폼과 체공 상태를 받아온다
	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
	_bool air = m_pTransformCom->Get_JumpBoolean();

	// 지상에 있을 경우
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

	// 공중에 있을 경우
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

	// 이동, 공격 스테이트가 있을 경우 해당 스테이트를 삭제
	m_pStateMachine->ClearState(CStateMachine::STATETYPE_NORMALMOVE);
	m_pStateMachine->ClearState(CStateMachine::STATETYPE_ATK);

	// 사운드 재생(레이지 폼에 대한 예외처리 걸려있음)
	m_pPlayer->Play_ActionVoiceSound(0.75f);

	// 레이지일 경우 가드 사운드 재생
	if (CPlayer::SF_RAGE == curr_form)
		m_pPlayer->Play_RageGuard();

	return S_OK;
}

HRESULT CState_PlayerGuard::ExitState()
{
	/*
		애니메이션을 전환해야한다.

		1. 지상에 있다면
		-> 키 입력이 있다 : 이동
			-> 무기를 들고있다
			-> 무기를 들고있지 않다
		-> 키 입력이 없다 : 대기
			-> 무기t를 들고있다
			-> 무기를 들고있지 않다

		=> 가드는 무기를 항상 들고있을 수 밖에 없다 이 멍청한 자식아.

		2. 공중에 있다면
		 -> 낙하 애니메이션

		이미 점프스테이트 생성되어있음.
	*/

	_bool air = m_pTransformCom->Get_JumpBoolean();
	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();

	// 지상에 있을 경우
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

	// 공중에 있을 경우
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

	// 가드 타이머 초기화
	m_pPlayer->Set_GuardTimer(0.0);

	return S_OK;
}

HRESULT CState_PlayerGuard::ClearState()
{
	// 가드 타이머 초기화
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
