#include "stdafx.h"
#include "State_PlayerAttack.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "StateMachine.h"
#include "State_PlayerMove.h"
#include "State_Jump.h"

#include "Player_Weapon.h"
#include "Combat_Assistant.h"
#include "Action_Assistant.h"

#include "Effect_Manager.h"
#include "Camera_Perspective.h"
#include "Monster.h"

HRESULT CState_PlayerAttack::Initialize(void * pArg)
{
	Reset_EffectCreated_Boolean_For_RageForm();

	// 사운드 프레임 변수 초기화
	for (_uint i = 0; i < CPlayer::SORA_FORM::SF_END; ++i)
	{
		m_iVoiceSound_PlayedFrame[i] = UINT_MAX;
		m_iSwingSound_PlayedFrame[i] = UINT_MAX;
	}

	return S_OK;
}

_bool CState_PlayerAttack::KeyInput(_double TimeDelta)
{
	CGameInstance* pGI = CGameInstance::GetInstance();
	CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);

	_bool able_attack = m_pAction_Assistant->Able(CAction_Assistant::ASSIST_ATTACK);
	_bool able_cancle = m_pAction_Assistant->Able(CAction_Assistant::ASSIST_CANCLE);
	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();

	// 전투 애니메이션이 아닐 경우
	if (false == m_pStateMachine->Find_AnimIndex(m_iStateType, curr_anim))

	{
		// 처리가 필요한가?
		// 필요하다면 무엇을, 어떻게 처리해야하는가?

		/*
			공격 스테이트가 처음 생겼을 때
			공격 애니메이션으로 전환하는데
			어떻게 jump_up 상태로 여길 들어오는건가.
			=> tempIndex를 사용하기 때문에.

			아직 모델의 업데이트가 돌지 않아서
			EnterState()에서 넣어준 공격 애니메이션이 적용이 되지 않았다.
		*/

		//return false;
	}

	// 전투 애니메이션일 경우
	else
	{
		// 즉시 공격이 불가능한 상태일 때
		if (false == able_attack || false == able_cancle)
		{
			// 레이지 폼이 아닐 경우 return
			if (CPlayer::SF_RAGE != curr_form)
				return true;
			
			// 선입력 변수가 이미 true라면 return
			if (true == m_bEarlyInput)
				return true;

			// 애니메이션 진행도가 50% 미만이라면 return
			if (false == m_pModelCom->Check_ProgressRate_EachBody(CPlayer::SB_BODY, 50.f))
				return true;

			// LButton 입력이 없다면 return
			if (false == m_pPlayer->Get_Input(CPlayer::SI_LBUTTON))
				return true;

			// 피니시가 발동 중이라면 return
			if (true == m_pPlayer->Check_OnFinish())
				return true;

			// 선입력 처리를 받아둔다
			if (4 > m_iAttackCount)
			{
				m_eNextAttackAnim = m_pAction_Assistant->Dice_NextAttack(m_iAttackCount);
				m_iAttackCount++;

				m_bEarlyInput = true;
			}

			return true;
		}

		// 즉시 공격이 가능한 상태일 때
		// 일반 상태일 경우 키 입력
		switch (curr_form)
		{
		case Client::CPlayer::SF_NORMAL:

			if (4 > m_iAttackCount && pGI->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON))
			{
				CPlayer::SORA_ANIM next_attack_anim = m_pAction_Assistant->Dice_NextAttack(m_iAttackCount);
				m_pPlayer->Change_PlayerAnimation(next_attack_anim);

				m_pCombat_Assistant->Auto_Rotation();

				if (4 > m_iAttackCount)
					m_iAttackCount++;
			}

			break;
		case Client::CPlayer::SF_HAMMER:

			// 해머 공격 6번, GROUND_LAST_ATTAK, AIR_LAST_ATTACK 일 때에는 추가입력 받지 않음
			if (CPlayer::SORA_ANIM_HAMMER_ATTACK_06 == curr_anim ||
				CPlayer::SORA_ANIM_HAMMER_GROUND_LAST_ATTACK == curr_anim ||
				CPlayer::SORA_ANIM_HAMMER_AIR_LAST_ATTACK == curr_anim)
				return true;

			// 피니시가 발동 중이라면 return
			if (true == m_pPlayer->Check_OnFinish())
				return true;

			if (5 > m_iAttackCount && pGI->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON))
			{
				CPlayer::SORA_ANIM next_attack_anim = m_pAction_Assistant->Dice_NextAttack(m_iAttackCount);
				m_pPlayer->Change_PlayerAnimation(next_attack_anim);

				m_pCombat_Assistant->Auto_Rotation();

				if (5 > m_iAttackCount)
					m_iAttackCount++;

				// 카메라워크 시작, 무적시간 설정
				if (CPlayer::SORA_ANIM_HAMMER_GROUND_LAST_ATTACK == next_attack_anim)
				{
					GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_HAMMER_ATTACKEND);

					_double duration = m_pModelCom->Get_TargetDuration(next_attack_anim);
					_double play_speed = m_pModelCom->Get_TargetPlaySpeed(next_attack_anim);

					m_pPlayer->Invincibility_Input(duration, play_speed);
				}
				else if (CPlayer::SORA_ANIM_HAMMER_AIR_LAST_ATTACK == next_attack_anim)
				{
					// 카메라 연출
					GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_HAMMER_AIRATTACKEND);

					// 무적 시간 부여
					_double duration = m_pModelCom->Get_TargetDuration(next_attack_anim);
					_double play_speed = m_pModelCom->Get_TargetPlaySpeed(next_attack_anim);
					m_pPlayer->Invincibility_Input(duration, play_speed);

					// 게임 스피드 제어
					list<CMonster*>* monster_list = GM->Get_MonstersList();
					for (auto pMonster : *monster_list)
					{
						CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), pMonster->Get_LayerTag(), 0.1);
						CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_MonsterSkill"), 0.1);
					}
				}
			}

			break;
		case Client::CPlayer::SF_RAGE:

			// 선 입력이 있다면
			if (true == m_bEarlyInput)
			{
				// 텔레포트를 사용하는 경우
				if (true == m_pPlayer->Get_Teleport())
				{
					m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_JUMP_UP);
					m_pTransformCom->Set_GravityEffect(false);
				}

				// 텔레포트를 사용하지 않는 경우
				else
				{
					// 선 입력받은 변수 처리
					//Reset_EffectCreated_Boolean_For_RageForm();
					m_pPlayer->Change_PlayerAnimation(m_eNextAttackAnim);

					if (CPlayer::SORA_ANIM_RAGE_FINISH != curr_anim)
						m_pCombat_Assistant->Auto_Rotation();

					// 선 입력 변수 리셋
					m_bEarlyInput = false;
					m_eNextAttackAnim = CPlayer::SORA_ANIM_END;
				}
			}

			// 선 입력이 없다면
			else
			{
				// 피니시가 발동 중이라면 return
				if (true == m_pPlayer->Check_OnFinish())
					return true;

				if (false == able_attack || false == able_cancle)
					return true;

				if (4 > m_iAttackCount && pGI->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON))
				{
					m_eNextAttackAnim = m_pAction_Assistant->Dice_NextAttack(m_iAttackCount);

					// 텔레포트를 사용하는 경우
					if (true == m_pPlayer->Get_Teleport())
					{
						m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_JUMP_UP);
						m_pTransformCom->Set_GravityEffect(false);

						if (4 > m_iAttackCount)
							m_iAttackCount++;
					}

					// 텔레포트를 사용하지 않는 경우
					else
					{
						m_pPlayer->Change_PlayerAnimation(m_eNextAttackAnim);
						m_pCombat_Assistant->Auto_Rotation();

						if (4 > m_iAttackCount)
							m_iAttackCount++;
					}
				}
			}

			break;
		}
	}

	return true;
}

_bool CState_PlayerAttack::Action(_double TimeDelta)
{
	_bool input = KeyInput(TimeDelta);

	if (false == input)
	{
		// 키 입력을 못 받았다면 true 리턴해서 StateMachine 에서 지워지게 함
		ExitState();
		return DELETE_STATE;
	}

	else
	{
		CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);

		// 전투 애니메이션일 경우
		if (true == m_pStateMachine->Find_AnimIndex(m_iStateType, curr_anim))
		{
			// 애니메이션이 완전히 종료되었다 = 공격 상태에서 벗어난다.
			if (true == m_pModelCom->CheckStop_EachBody(CPlayer::SB_BODY))
			{
				// RAGE_ATTACK4가 끝났을 경우 RAGE_ATTACK5로 강제전환 시킨다.
				if (CPlayer::SORA_ANIM_RAGE_ATTACK4 == curr_anim)
				{
					// 애니메이션 변경
					m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_ATTACK5);
					m_pCombat_Assistant->Auto_Rotation();

					// 카메라 워크
					GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_RAGE_ATTACKEND);

					return KEEP_STATE;
				}

				// 피니시 어택이 끝났을 경우
				if (CPlayer::SORA_ANIM_RAGE_FINISH == curr_anim ||
					CPlayer::SORA_ANIM_HAMMER_FINISH == curr_anim)
				{
					m_pStateMachine->ClearState(CStateMachine::STATETYPE_FORM);
				}

				if (CPlayer::SORA_ANIM_HAMMER_AIR_LAST_ATTACK == curr_anim)
				{
					// 게임 스피드 제어
					list<CMonster*>* monster_list = GM->Get_MonstersList();
					for (auto pMonster : *monster_list)
					{
						CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), pMonster->Get_LayerTag(), 1.0);
						CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_MonsterSkill"), 1.0);
					}
				}

				// 레이지 폼의 공격이 끝났을 경우
				switch (curr_anim)
				{
				case Client::CPlayer::SORA_ANIM_RAGE_DOUBLE_TAP:
				case Client::CPlayer::SORA_ANIM_RAGE_TRIPLE_TAP:
				case Client::CPlayer::SORA_ANIM_RAGE_ATTACK1:
				case Client::CPlayer::SORA_ANIM_RAGE_ATTACK2:
				case Client::CPlayer::SORA_ANIM_RAGE_ATTACK3:
				case Client::CPlayer::SORA_ANIM_RAGE_ATTACK5:

					// 지상
					if (false == m_pTransformCom->Get_JumpBoolean())
						m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_FALL_END);

					// 공중
					else
					{
						m_pTransformCom->BasicJump(CTransform::JUMP_FALL);

						CState_Jump::JUMP_STATE_DESC jump_state_desc;

						jump_state_desc.eObjectType = m_pPlayer->Get_ObjectType();
						jump_state_desc.iJumpAnimIndex = 2;
						jump_state_desc.iNumberOfJumpAnim = 4;

						m_pPlayer->Create_PlayerState(CStateMachine::STATETYPE_JUMP, &jump_state_desc);
					}

					break;
				}

				ExitState();
				return DELETE_STATE;
			}

			// 애니메이션이 종료되지 않았다면
			CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
			switch (curr_form)
			{
			case Client::CPlayer::SF_NORMAL:
				Register_Colliders_On_Normal(TimeDelta);
				break;
			case Client::CPlayer::SF_HAMMER:
				Register_Colliders_On_Hammer(TimeDelta);
				break;
			case Client::CPlayer::SF_RAGE:
				Register_Colliders_On_RageForm(TimeDelta);
				break;
			case Client::CPlayer::SF_END:
				_breakpoint;
				break;
			}
		}

		else
		{
			if (CPlayer::SORA_ANIM_RAGE_JUMP_UP == curr_anim &&
				true == m_pModelCom->CheckStop_Input(curr_anim) &&
				true == m_pPlayer->Get_Teleport())
			{
				// 순간이동
				CCollider* target_collider = m_pCombat_Assistant->Get_TargetCollider();

				if (nullptr == target_collider)
				{
					// 선 입력받은 변수 처리
					if (CPlayer::SORA_ANIM_END == m_eNextAttackAnim)
						_breakpoint;

					//Reset_EffectCreated_Boolean_For_RageForm();
					m_pPlayer->Change_PlayerAnimation(m_eNextAttackAnim);

					if (CPlayer::SORA_ANIM_RAGE_FINISH != curr_anim)
						m_pCombat_Assistant->Auto_Rotation();

					// 선 입력 변수 리셋
					m_bEarlyInput = false;
					m_eNextAttackAnim = CPlayer::SORA_ANIM_END;

					m_pPlayer->Set_Teleport(false);

					return KEEP_STATE;
				}
				
				_vector collider_position = target_collider->Get_Center();
				_vector player_position = m_pTransformCom->Get_Position();

				vecy(collider_position) = 0.f;
				vecy(player_position) = 0.f;

				// 타겟 콜라이더로부터 떨어질 방향을 찾는 것이기 때문에 player - collider
				_vector direction_xz = _vec3norm(player_position - collider_position);
				_float distance = 0.75f;

				// 텔레포트할 위치를 1차적으로 구함
				collider_position = target_collider->Get_Center();
				_vector teleport_position = collider_position + (direction_xz * distance);

				/*
					:: 텔레포트로 이동할 위치가 정상적으로 이동 가능한 위치인지 판별한다 
					
					이동가능한 위치를 판별하기 위해 정석적으로는 1. 네비게이션 in 검사와 2. 높이 검사를 진행해야하지만
					게임 기획구조상 네비게이션 in 검사는 필요가 없기 때문에 높이 검사만 진행한다
				*/

				_float gap_y_between_position_and_center = m_pPlayer->Calculate_GapY_Between_PositionAndCenter();
				
				// Navigation 검사
				CCell* curr_cell = m_pPlayer->Get_Navigation()->Get_CurrentCell();

				if (nullptr == curr_cell)
					_breakpoint;

				_float height_on_cell = curr_cell->Compute_Y(teleport_position);
				_float target_point_y = vecy(teleport_position) - gap_y_between_position_and_center;

				// 이동하려는 포지션의 높이가 0과 같거나 0보다 낮을 경우
				if (height_on_cell >= target_point_y)
					target_point_y = height_on_cell;

				// 이동하려는 포지션의 높이가 0보다 높을 경우
				else
				{
					m_pTransformCom->BasicJump(CTransform::JUMP_FALL);

					CState_Jump::JUMP_STATE_DESC jump_state_desc;
					jump_state_desc.eObjectType = m_pPlayer->Get_ObjectType();
					jump_state_desc.iJumpAnimIndex = 2;
					jump_state_desc.iNumberOfJumpAnim = 4;

					m_pPlayer->Create_PlayerState(CStateMachine::STATETYPE_JUMP, &jump_state_desc);
				}

				vecy(teleport_position) = target_point_y;
				m_pTransformCom->Set_Position(teleport_position);

				// 자동 회전
				m_pCombat_Assistant->Auto_Rotation();

				// 선 입력받은 변수 처리
				if (CPlayer::SORA_ANIM_END == m_eNextAttackAnim)
					_breakpoint;

				//Reset_EffectCreated_Boolean_For_RageForm();
				m_pPlayer->Change_PlayerAnimation(m_eNextAttackAnim);

				if (CPlayer::SORA_ANIM_RAGE_FINISH != curr_anim)
					m_pCombat_Assistant->Auto_Rotation();

				// 선 입력 변수 리셋
				m_bEarlyInput = false;
				m_eNextAttackAnim = CPlayer::SORA_ANIM_END;

				m_pPlayer->Set_Teleport(false);
			}
		}

		return KEEP_STATE;
	}
}

HRESULT CState_PlayerAttack::EnterState()
{
	/*
		1. 플레이어 객체 참조
		2. 관련 애니메이션 리스트 셋업

		3. 공존할 수 없는 상태제거
			- 공격, 방어, 회피의 3가지는 공존할 수 없다.
			- 또한 위 3가지의 상태에선 일반적인 이동은 작동하지 않는다.

		4. 애니메이션 변경
			첫번째 애니메이션으로 전환한다.
			- 공중일 경우 공중 공격모션
			- 지상일 경우 지상 공격모션

			+ 전환 변경이 일어날 경우 추가적인 검사를 한다.
			- 플레이어의 현재 aerial 상태(지상 OR 공중)
			- 플레이어의 현재 위치(특히 공중일 때)
			- 몬스터(콜라이더)의 현재 위치
			=> 지상에서 공격이 발동되었어도 타겟 콜라이더의 위치가 공중일 경우 공중공격이 발생한다.
			=> 이 때 트랜스폼의 점프 상태를 바꿔줘야한다.
	*/

	// 1. 플레이어 참조
	m_pPlayer = static_cast<CPlayer*>(m_pGameObject);
	m_pCombat_Assistant = m_pPlayer->Get_CombatAssistant();
	m_pAction_Assistant = m_pPlayer->Get_ActionAssistant();
	m_pPlayer_Weapon = m_pPlayer->Get_Weapon();

	// 3. 공존할 수 없는 상태 제거
	m_pStateMachine->ClearState(CStateMachine::STATETYPE_NORMALMOVE);
	m_pStateMachine->ClearState(CStateMachine::STATETYPE_GUARD);
	m_pStateMachine->ClearState(CStateMachine::STATETYPE_AVOID);

	for (_uint i = 0; i < HIT_END; ++i)
		m_bEffectCreated[i] = false;

	// 4. 애니메이션 변경
	m_eNextAttackAnim = m_pAction_Assistant->Dice_NextAttack(m_iAttackCount);

	// 텔레포트가 발동될 경우
	if (true == m_pPlayer->Get_Teleport())
	{
		m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_JUMP_UP);
		m_pTransformCom->Set_GravityEffect(false);

		m_iAttackCount++;
	}

	// 텔레포트 발동 없을 경우
	else
	{
		m_pPlayer->Change_PlayerAnimation(m_eNextAttackAnim);

		m_pCombat_Assistant->Auto_Rotation();
		m_iAttackCount++;
	}
	
	return S_OK;
}

HRESULT CState_PlayerAttack::ExitState()
{
	// 액션 어시스턴트에서 사용한 AnimOrder를 비워준다
	m_pAction_Assistant->Clear_AttackAnimOrder();

	// 지상 공격에 대한 처리는 따로 할 필요가 없다
	// 공중 공격에 대한 처리만 진행한다
	_bool air = m_pTransformCom->Get_JumpBoolean();
	CPlayer::SORA_ANIM temp_anim = (CPlayer::SORA_ANIM)m_pModelCom->Get_TempIndex_EachBody(CPlayer::SB_BODY);
	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();

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

			if (CPlayer::SORA_ANIM_RAGE_FALL_END == temp_anim)
				break;

			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_IDLE);
			break;
		}
	}
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
			_breakpoint;
			break;
		}
	}

	return S_OK;
}

HRESULT CState_PlayerAttack::ClearState()
{
	// 액션 어시스턴트에서 사용한 AnimOrder를 비워준다
	m_pAction_Assistant->Clear_AttackAnimOrder();

	// Able 변수를 켜준다
	m_pAction_Assistant->Able(A_ASSIST::ASSIST_CANCLE, true);
	m_pAction_Assistant->Able(A_ASSIST::ASSIST_ATTACK, true);

	// 받아둔 이펙트 메시를 삭제
	if (nullptr != m_pTrailMeshEffect)
		m_pTrailMeshEffect->Set_Dead(true);

	// 트레일 off
	m_pPlayer_Weapon->Set_Trail_Active(false); 
	 
	// 모든 콜라이더의 충돌상태를 제거
	m_pPlayer_Weapon->Clear_All_Colliding_Data();

	return S_OK;
}

void CState_PlayerAttack::Register_Colliders_On_Normal(_double TimeDelta)
{
	if (false == m_pModelCom->Check_TempAndCurr_EachBody(CPlayer::SB_BODY))
	{
		// 사운드 재생 변수 리셋
		m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = UINT_MAX;
		m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] = UINT_MAX;

		return;
	}

	CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);
	_double time_acc = m_pModelCom->Get_TargetTimeAcc(curr_anim);
	_uint time_acc_in_uint = (_uint)time_acc;

	switch (curr_anim)
	{
	case CPlayer::SORA_ANIM_GROUND_ATTACK1:

		if (6.0 <= time_acc && 14.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider(true);
			m_pPlayer_Weapon->Set_KnockBackPower(8.f);
		}
		else
			m_pPlayer_Weapon->RegisterCollider(false);

		// 짧은 타이밍 안에 반복재생되는 것을 막기위해 정수단위로 재생시간을 기록하여 현재 시간 비교한다
		// 보이스 사운드 재생
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (2 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (6 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		break;
	case CPlayer::SORA_ANIM_GROUND_ATTACK2:

		if (9.0 <= time_acc && 17.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider(true);
			m_pPlayer_Weapon->Set_KnockBackPower(4.f);
		}
		else
			m_pPlayer_Weapon->RegisterCollider(false);

		// 보이스 사운드 재생
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (5 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (9 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		break;
	case CPlayer::SORA_ANIM_GROUND_ATTACK3:

		if (8.0 <= time_acc && 14.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider(true);
			m_pPlayer_Weapon->Set_KnockBackPower(12.f);
		}
		else
			m_pPlayer_Weapon->RegisterCollider(false);

		// 보이스 사운드 재생
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (4 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (8 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		break;
	case CPlayer::SORA_ANIM_GROUND_ATTACK4:

		if (20.0 <= time_acc && 26.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider(true);
			m_pPlayer_Weapon->Set_KnockBackPower(16.f);
		}
		else
			m_pPlayer_Weapon->RegisterCollider(false);

		if (20.0 <= time_acc && false == m_bEffectCreated[HIT_ONE])
		{
			CEffect_Manager::GetInstance()->Create_TrailMesh_Short(m_pTransformCom->Get_WorldFloat4x4_Pointer(), CEffect_Manager::TRAIL_DEFAULT, m_pTransformCom->Get_PositionFloat4_Pointer());
			m_bEffectCreated[HIT_ONE] = true;
		}

		if (false == m_bAttackReadyEffectCreated && time_acc_in_uint)
		{
			CEffect_Manager::GetInstance()->Create_ReadyToAttackEffect(m_pTransformCom);
			m_bAttackReadyEffectCreated = true;
		}

		// 보이스 사운드 재생
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (6 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(true);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (20 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		break;
	case CPlayer::SORA_ANIM_AIR_ATTACK1:

		if (5.0 <= time_acc && 13.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider(true);
			m_pPlayer_Weapon->Set_KnockBackPower(2.f);
		}
		else
			m_pPlayer_Weapon->RegisterCollider(false);

		// 보이스 사운드 재생
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (0 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (5 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		break;
	case CPlayer::SORA_ANIM_AIR_ATTACK2:

		if (4.0 <= time_acc && 12.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider(true);
			m_pPlayer_Weapon->Set_KnockBackPower(2.f);
		}
		else
			m_pPlayer_Weapon->RegisterCollider(false);

		// 보이스 사운드 재생
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (0 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (4 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		break;
	case CPlayer::SORA_ANIM_AIR_ATTACK3:

		if (6.0 <= time_acc && 14.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider(true);
			m_pPlayer_Weapon->Set_KnockBackPower(0.5f);
		}
		else
			m_pPlayer_Weapon->RegisterCollider(false);

		// 보이스 사운드 재생
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (1 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (6 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		break;
	case CPlayer::SORA_ANIM_AIR_ATTACK4:

		if ((6.0 <= time_acc && 11.0 > time_acc) ||
			(13.0 <= time_acc && 17.0 > time_acc) ||
			(19.0 <= time_acc && 23.0 > time_acc))
		{
			if (6.0 <= time_acc && 11.0 > time_acc)
				m_pPlayer_Weapon->Set_KnockBackPower(0.25f);
			else if (13.0 <= time_acc && 17.0 > time_acc)
				m_pPlayer_Weapon->Set_KnockBackPower(0.25f);
			else
				m_pPlayer_Weapon->Set_KnockBackPower(10.f);

			m_pPlayer_Weapon->RegisterCollider(true);
		}
		else
			m_pPlayer_Weapon->RegisterCollider(false);

		if (20.0 <= time_acc && false == m_bEffectCreated[HIT_ONE])
		{
			CEffect_Manager::GetInstance()->Create_TrailMesh(m_pTransformCom);
			m_pTrailMeshEffect = CGameInstance::GetInstance()->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Effect"));

			if (nullptr == m_pTrailMeshEffect)
				_breakpoint;

			m_bEffectCreated[HIT_ONE] = true;
		}

		// 보이스 사운드 재생
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (1 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(true);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (6 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		break;

	case CPlayer::SORA_ANIM_UPPER_ATTACK1:

		if (5.0 <= time_acc && 13.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider(true);
			m_pPlayer_Weapon->Set_KnockBackPower(1.f);
		}
		else
			m_pPlayer_Weapon->RegisterCollider(false);

		// 보이스 사운드 재생
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (0 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (5 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		break;
	case CPlayer::SORA_ANIM_UPPER_ATTACK2:

		if (2.0 <= time_acc && 9.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider(true);
			m_pPlayer_Weapon->Set_KnockBackPower(1.f);
		}
		else
			m_pPlayer_Weapon->RegisterCollider(false);

		// 보이스 사운드 재생
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (0 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (2 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		break;
	case CPlayer::SORA_ANIM_UPPER_ATTACK3:

		if (6.0 <= time_acc && 13.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider(true);
			m_pPlayer_Weapon->Set_KnockBackPower(0.5f);
		}
		else
			m_pPlayer_Weapon->RegisterCollider(false);

		// 보이스 사운드 재생
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (1 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (6 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		break;
	case CPlayer::SORA_ANIM_UPPER_ATTACK4:

		if ((1.0 <= time_acc && 5.0 > time_acc) ||
			(7.0 <= time_acc && 11.0 > time_acc) ||
			(13.0 <= time_acc && 19.0 > time_acc))
		{
			if (1.0 <= time_acc && 5.0 > time_acc)
				m_pPlayer_Weapon->Set_KnockBackPower(0.125f);
			else if (7.0 <= time_acc && 11.0 > time_acc)
				m_pPlayer_Weapon->Set_KnockBackPower(0.125f);
			else
				m_pPlayer_Weapon->Set_KnockBackPower(8.f);

			m_pPlayer_Weapon->RegisterCollider(true);
		}
		else
			m_pPlayer_Weapon->RegisterCollider(false);

		if (13.0 <= time_acc && false == m_bEffectCreated[HIT_ONE])
		{
			CEffect_Manager::GetInstance()->Create_TrailMesh_Spin(m_pTransformCom->Get_WorldFloat4x4_Pointer());
			m_pTrailMeshEffect = CGameInstance::GetInstance()->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Effect"));

			if (nullptr == m_pTrailMeshEffect)
				_breakpoint;

			m_bEffectCreated[HIT_ONE] = true;
		}

		// 보이스 사운드 재생
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (0 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(true);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (1 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		break;

	case CPlayer::SORA_ANIM_LOWER_ATTACK1:

		if (10.0 <= time_acc && 16.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider(true);
			m_pPlayer_Weapon->Set_KnockBackPower(1.f);
		}
		else
			m_pPlayer_Weapon->RegisterCollider(false);

		// 보이스 사운드 재생
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (5 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (10 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		break;
	case CPlayer::SORA_ANIM_LOWER_ATTACK2:

		if (9.0 <= time_acc && 16.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider(true);
			m_pPlayer_Weapon->Set_KnockBackPower(1.f);
		}
		else
			m_pPlayer_Weapon->RegisterCollider(false);

		// 보이스 사운드 재생
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (4 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (9 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		break;
	case CPlayer::SORA_ANIM_LOWER_ATTACK3:

		if (9.0 <= time_acc && 16.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider(true);
			m_pPlayer_Weapon->Set_KnockBackPower(0.25f);
		}
		else
			m_pPlayer_Weapon->RegisterCollider(false);

		// 보이스 사운드 재생
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (4 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (9 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		break;
	case CPlayer::SORA_ANIM_LOWER_ATTACK4:

		if ((6.0 <= time_acc && 11.0 > time_acc) ||
			(13.0 <= time_acc && 18.0 > time_acc))
		{
			if (6.0 <= time_acc && 11.0 > time_acc)
				m_pPlayer_Weapon->Set_KnockBackPower(0.125f);
			else
				m_pPlayer_Weapon->Set_KnockBackPower(8.f);

			m_pPlayer_Weapon->RegisterCollider(true);
		}
		else
			m_pPlayer_Weapon->RegisterCollider(false);

		if (6.0 <= time_acc && false == m_bEffectCreated[HIT_ONE])
		{
			CEffect_Manager::GetInstance()->Create_TrailMesh_Short(m_pTransformCom->Get_WorldFloat4x4_Pointer(), CEffect_Manager::TRAIL_LEFT);
			m_bEffectCreated[HIT_ONE] = true;
		}
		else if (13.0 <= time_acc && false == m_bEffectCreated[HIT_TWO])
		{
			CEffect_Manager::GetInstance()->Create_TrailMesh_Short(m_pTransformCom->Get_WorldFloat4x4_Pointer(), CEffect_Manager::TRAIL_RIGHT);
			m_bEffectCreated[HIT_TWO] = true;
		}

		// 보이스 사운드 재생
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (1 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(true);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (6 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		break;
	}

	// 이펙트 처리
	if (true == m_pPlayer_Weapon->RegisterCollider())
		m_pPlayer_Weapon->Set_Trail_Active(true);
	else
		m_pPlayer_Weapon->Set_Trail_Active(false);
}

void CState_PlayerAttack::Register_Colliders_On_Hammer(_double TimeDelta)
{
	// 애니메이션이 바뀌었으면 값 리셋하고 리턴
	if (false == m_pModelCom->Check_TempAndCurr_EachBody(CPlayer::SB_BODY))
	{
		// 트레일 메시 생성 변수 리셋
		m_bHammerForm_Attack_Trail_Effect_Created = false;

		// 에어 커트 생성 변수 리셋
		m_bHammerForm_Effect_AirCut_Created = false;

		// 그라운드 히트 변수 리셋
		m_bHammerForm_Effect_GroundHit_Created = false;

		// 앰버 파티클 생성 변수 리셋
		m_bHammerForm_Effect_Ember_Created = false;
		m_iEmberCreation_FrameCount = 0;
		m_EmberEffects.clear();

		// 사운드 재생 변수 리셋
		m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] = UINT_MAX;
		m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] = UINT_MAX;

		m_iLastAttackVoiceSound_PlayedFrame = UINT_MAX;
		m_iGrounHitSound_PlayedFrame = UINT_MAX;
		m_iLastAttackSound_PlayedFrame = UINT_MAX;

		return;
	}

	CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);
	_double time_acc = m_pModelCom->Get_TargetTimeAcc(curr_anim);
	_uint time_acc_in_uint = (_uint)time_acc;

	switch (curr_anim)
	{
	case CPlayer::SORA_ANIM_HAMMER_ATTACK_01:

		// HEAD COLLIDER
		if (15.0 <= time_acc && 21.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider_Hammer(CPlayer_Weapon::HAMMER_COLLIDER_HEAD, true);
			m_pPlayer_Weapon->Set_KnockBackPower(1.f);
		}
		else
			m_pPlayer_Weapon->RegisterCollider_Hammer(CPlayer_Weapon::HAMMER_COLLIDER_HEAD, false);

		// WAVE COLLIDER
		if (21.0 <= time_acc && 25.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider_Hammer(CPlayer_Weapon::HAMMER_COLLIDER_WAVE, true);
			m_pPlayer_Weapon->Set_KnockBackPower(4.f);
		}
		else
			m_pPlayer_Weapon->RegisterCollider_Hammer(CPlayer_Weapon::HAMMER_COLLIDER_WAVE, false);

		// 해머 폼 메시 트레일 이펙트 생성
		if (false == m_bHammerForm_Attack_Trail_Effect_Created && time_acc >= 11.0)
		{
			CEffect_Manager::GetInstance()->Create_HammerForm_Attack1(m_pTransformCom, _zeroPoint);
			m_pPlayer_Weapon->Create_Effect_HammerSwing();

			m_bHammerForm_Effect_Ember_Created = true;
			m_iEmberCreation_FrameCount = 6;

			m_bHammerForm_Attack_Trail_Effect_Created = true;
		}

		// 에어 커트 이펙트 생성
		if (false == m_bHammerForm_Effect_AirCut_Created && time_acc >= 14.0)
		{
			m_pPlayer_Weapon->Create_Effect_AirCut();
			m_bHammerForm_Effect_AirCut_Created = true;
		}

		// 그라운드 히트 이펙트 생성
		if (false == m_bHammerForm_Effect_GroundHit_Created && time_acc >= 18.0)
		{
			_vector hit_point = m_pPlayer_Weapon->Calculate_Point_On_Ground(2, 0);

			CEffect_Manager::GetInstance()->Create_HammerForm_Effect_GroundHit_Glow(hit_point, curr_anim);
			CEffect_Manager::GetInstance()->Create_HammerForm_Effect_GroundHit_Rock(hit_point, curr_anim);

			CEffect_Manager::GetInstance()->Create_HammerForm_Effect_GroundHit_Wave(m_pTransformCom, hit_point, curr_anim);
			CEffect_Manager::GetInstance()->Create_HammerForm_Effect_GroundHit_Crown(m_pTransformCom, hit_point, 30.f, curr_anim);

			m_bHammerForm_Effect_GroundHit_Created = true;

			// 카메라 셰이킹
			GM->Get_Camera()->Set_VerticalShake(0.5f, 0.f, 1.f);
		}

		// 보이스 사운드 재생
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (4 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (9 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormSwing();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		// 그라운드 히트 사운드 재생
		if (m_iGrounHitSound_PlayedFrame != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (21 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormWaveSound(curr_anim);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iGrounHitSound_PlayedFrame = time_acc_in_uint;
		}

		break;
	case CPlayer::SORA_ANIM_HAMMER_ATTACK_02:

		// HEAD COLLIDER
		if (17.0 <= time_acc && 23.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider_Hammer(CPlayer_Weapon::HAMMER_COLLIDER_HEAD, true);
			m_pPlayer_Weapon->Set_KnockBackPower(1.f);
		}
		else
			m_pPlayer_Weapon->RegisterCollider_Hammer(CPlayer_Weapon::HAMMER_COLLIDER_HEAD, false);

		// 해머 폼 메시 트레일 이펙트 생성
		if (false == m_bHammerForm_Attack_Trail_Effect_Created && time_acc >= 13.0)
		{
			CEffect_Manager::GetInstance()->Create_HammerForm_Attack2(m_pTransformCom, _zeroPoint);
			m_pPlayer_Weapon->Create_Effect_HammerSwing();

			m_bHammerForm_Effect_Ember_Created = true;
			m_iEmberCreation_FrameCount = 6;

			m_bHammerForm_Attack_Trail_Effect_Created = true;
		}

		// 에어 커트 이펙트 생성
		if (false == m_bHammerForm_Effect_AirCut_Created && time_acc >= 18.0)
		{
			m_pPlayer_Weapon->Create_Effect_AirCut();
			m_bHammerForm_Effect_AirCut_Created = true;
		}

		// 보이스 사운드 재생
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (8 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (10 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormSwing();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		break;
	case CPlayer::SORA_ANIM_HAMMER_ATTACK_03:

		// HEAD COLLIDER
		if (4.0 <= time_acc && 8.0 > time_acc ||
			9.0 <= time_acc && 13.0 > time_acc ||
			14.0 <= time_acc && 18.0 > time_acc ||
			19.0 <= time_acc && 24.0 > time_acc ||
			25.0 <= time_acc && 31.0 > time_acc)
		{
			if (4.0 <= time_acc && 8.0 > time_acc)
				m_pPlayer_Weapon->Set_KnockBackPower(0.125f);
			else if (9.0 <= time_acc && 13.0 > time_acc)
				m_pPlayer_Weapon->Set_KnockBackPower(0.125f);
			else if (14.0 <= time_acc && 18.0 > time_acc)
				m_pPlayer_Weapon->Set_KnockBackPower(0.125f);
			else if (19.0 <= time_acc && 24.0 > time_acc)
				m_pPlayer_Weapon->Set_KnockBackPower(0.125f);
			else
				m_pPlayer_Weapon->Set_KnockBackPower(2.f);

			m_pPlayer_Weapon->RegisterCollider_Hammer(CPlayer_Weapon::HAMMER_COLLIDER_HEAD, true);
		}
		else
			m_pPlayer_Weapon->RegisterCollider_Hammer(CPlayer_Weapon::HAMMER_COLLIDER_HEAD, false);

		// 해머 폼 메시 트레일 이펙트 생성
		if (false == m_bHammerForm_Attack_Trail_Effect_Created && time_acc >= 2.0)
		{
			CEffect_Manager::GetInstance()->Create_HammerForm_Attack3(m_pTransformCom, _zeroPoint);
			m_pPlayer_Weapon->Create_Effect_HammerSwing();

			m_bHammerForm_Effect_Ember_Created = true;
			m_iEmberCreation_FrameCount = 18;

			m_bHammerForm_Attack_Trail_Effect_Created = true;
		}

		// 보이스 사운드 재생
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (0 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (4 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormSwing();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		break;
	case CPlayer::SORA_ANIM_HAMMER_ATTACK_04:

		// HEAD COLLIDER
		if (10.0 <= time_acc && 15.0 > time_acc ||
			16.0 <= time_acc && 20.0 > time_acc ||
			21.0 <= time_acc && 28.0 > time_acc)
		{
			if (10.0 <= time_acc && 15.0 > time_acc)
				m_pPlayer_Weapon->Set_KnockBackPower(0.125f);
			else if (16.0 <= time_acc && 20.0 > time_acc)
				m_pPlayer_Weapon->Set_KnockBackPower(0.125f);
			else
				m_pPlayer_Weapon->Set_KnockBackPower(2.f);

			m_pPlayer_Weapon->RegisterCollider_Hammer(CPlayer_Weapon::HAMMER_COLLIDER_HEAD, true);
		}
		else
			m_pPlayer_Weapon->RegisterCollider_Hammer(CPlayer_Weapon::HAMMER_COLLIDER_HEAD, false);

		// 해머 폼 메시 트레일 이펙트 생성
		if (false == m_bHammerForm_Attack_Trail_Effect_Created && time_acc >= 2.0)
		{
			CEffect_Manager::GetInstance()->Create_HammerForm_Attack4(m_pTransformCom, _zeroPoint);
			m_pPlayer_Weapon->Create_Effect_HammerSwing();

			m_bHammerForm_Effect_Ember_Created = true;
			m_iEmberCreation_FrameCount = 18;

			m_bHammerForm_Attack_Trail_Effect_Created = true;
		}

		// 보이스 사운드 재생
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (6 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (8 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormSwing();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		break;
	case CPlayer::SORA_ANIM_HAMMER_ATTACK_05:

		// HEAD COLLIDER
		if (14.0 <= time_acc && 22.0 > time_acc ||
			33.0 <= time_acc && 41.0 > time_acc)
		{
			if (14.0 <= time_acc && 22.0 > time_acc)
				m_pPlayer_Weapon->Set_KnockBackPower(1.5f);
			else
				m_pPlayer_Weapon->Set_KnockBackPower(1.5f);

			m_pPlayer_Weapon->RegisterCollider_Hammer(CPlayer_Weapon::HAMMER_COLLIDER_HEAD, true);
		}
		else
			m_pPlayer_Weapon->RegisterCollider_Hammer(CPlayer_Weapon::HAMMER_COLLIDER_HEAD, false);

		// WAVE COLLIDER
		if (19.0 <= time_acc && 24.0 > time_acc ||
			38.0 <= time_acc && 43.0 > time_acc)
		{
			if (19.0 <= time_acc && 24.0 > time_acc)
				m_pPlayer_Weapon->Set_KnockBackPower(4.f);
			else
				m_pPlayer_Weapon->Set_KnockBackPower(4.f);

			m_pPlayer_Weapon->RegisterCollider_Hammer(CPlayer_Weapon::HAMMER_COLLIDER_WAVE, true);
		}
		else
			m_pPlayer_Weapon->RegisterCollider_Hammer(CPlayer_Weapon::HAMMER_COLLIDER_WAVE, false);

		// 해머 폼 메시 트레일 이펙트 생성
		if (time_acc >= 7.0 && time_acc < 22.0)
		{
			if (false == m_bHammerForm_Attack_Trail_Effect_Created)
			{
				CEffect_Manager::GetInstance()->Create_HammerForm_Attack5(m_pTransformCom, _zeroPoint, 0);
				m_pPlayer_Weapon->Create_Effect_HammerSwing();

				m_bHammerForm_Effect_Ember_Created = true;
				m_iEmberCreation_FrameCount = 4;

				m_bHammerForm_Attack_Trail_Effect_Created = true;
			}
		}
		else if (time_acc >= 23.0 && time_acc < 43.0)
		{
			if (false == m_bHammerForm_Attack_Trail_Effect_Created)
			{
				CEffect_Manager::GetInstance()->Create_HammerForm_Attack5(m_pTransformCom, _zeroPoint, 1);
				m_pPlayer_Weapon->Create_Effect_HammerSwing();

				m_bHammerForm_Effect_Ember_Created = true;
				m_iEmberCreation_FrameCount = 4;

				m_bHammerForm_Attack_Trail_Effect_Created = true;
			}
		}
		else
		{
			m_bHammerForm_Effect_Ember_Created = false;
			m_iEmberCreation_FrameCount = 0;

			m_bHammerForm_Attack_Trail_Effect_Created = false;
		}

		// 에어 커트 이펙트 생성
		if (time_acc >= 12.0 && time_acc < 18.0 ||
			time_acc >= 30.0 && time_acc < 36.0)
		{
			if (false == m_bHammerForm_Effect_AirCut_Created)
			{
				m_pPlayer_Weapon->Create_Effect_AirCut();
				m_bHammerForm_Effect_AirCut_Created = true;
			}
		}
		else
			m_bHammerForm_Effect_AirCut_Created = false;

		if ((time_acc >= 17.0 && time_acc < 21.0) ||
			(time_acc >= 36.0 && time_acc < 40.0))
		{
			if (false == m_bHammerForm_Effect_GroundHit_Created)
			{
				_vector hit_point = m_pPlayer_Weapon->Calculate_Point_On_Ground(2, 0);

				CEffect_Manager::GetInstance()->Create_HammerForm_Effect_GroundHit_Glow(hit_point, curr_anim);
				CEffect_Manager::GetInstance()->Create_HammerForm_Effect_GroundHit_Rock(hit_point, curr_anim);

				CEffect_Manager::GetInstance()->Create_HammerForm_Effect_GroundHit_Wave(m_pTransformCom, hit_point, curr_anim);
				CEffect_Manager::GetInstance()->Create_HammerForm_Effect_GroundHit_Crown(m_pTransformCom, hit_point, 30.f, curr_anim);

				m_bHammerForm_Effect_GroundHit_Created = true;

				// 카메라 셰이킹
				GM->Get_Camera()->Set_VerticalShake(0.35f, 0.f, 0.5f);
			}
		}
		else
			m_bHammerForm_Effect_GroundHit_Created = false;
		
		// 보이스 사운드 재생
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (6 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (8 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormSwing();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		// 그라운드 히트 사운드 재생
		if (m_iGrounHitSound_PlayedFrame != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (19 == time_acc_in_uint || 38 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormWaveSound(curr_anim);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iGrounHitSound_PlayedFrame = time_acc_in_uint;
		}

		break;
	case CPlayer::SORA_ANIM_HAMMER_ATTACK_06:

		// HEAD COLLIDER
		if (13.0 <= time_acc && 28.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider_Hammer(CPlayer_Weapon::HAMMER_COLLIDER_HEAD, true);
			m_pPlayer_Weapon->Set_KnockBackPower(1.f);
		}
		else
			m_pPlayer_Weapon->RegisterCollider_Hammer(CPlayer_Weapon::HAMMER_COLLIDER_HEAD, false);

		// WAVE COLLIDER
		if (21.0 <= time_acc && 28.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider_Hammer(CPlayer_Weapon::HAMMER_COLLIDER_WAVE, true);
			m_pPlayer_Weapon->Set_KnockBackPower(4.f);
		}
		else
			m_pPlayer_Weapon->RegisterCollider_Hammer(CPlayer_Weapon::HAMMER_COLLIDER_WAVE, false);

		// 카메라 셰이킹
		GM->Get_Camera()->Set_VerticalShake(0.5f, 0.f, 1.f);

		// 보이스 사운드 재생
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (9 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(true);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (11 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormSwing();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		// 그라운드 히트 사운드 재생
		if (m_iGrounHitSound_PlayedFrame != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (21 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormWaveSound(curr_anim);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iGrounHitSound_PlayedFrame = time_acc_in_uint;
		}

		break;
	case CPlayer::SORA_ANIM_HAMMER_GROUND_LAST_ATTACK:

		// LAST COLLIDER
		if (46.0 <= time_acc && 50.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider_Hammer(CPlayer_Weapon::HAMMER_COLLIDER_LAST, true);
			m_pPlayer_Weapon->Set_KnockBackPower(14.f);
		}
		else
			m_pPlayer_Weapon->RegisterCollider_Hammer(CPlayer_Weapon::HAMMER_COLLIDER_LAST, false);
		
		// 해머 폼 라스트 어택 상승기류 이펙트 생성
		if (time_acc >= 17.0)
		{
			if (false == m_bHammerForm_LastAttack_Effect_UpStream)
			{
				CEffect_Manager::GetInstance()->Create_HammerForm_LastAttack_UpStream(m_pTransformCom, _zeroPoint);
				m_bHammerForm_LastAttack_Effect_UpStream = true;
			}
		}
		else
			m_bHammerForm_LastAttack_Effect_UpStream = false;

		// 해머 폼 메시 트레일 이펙트 생성
		if (time_acc >= 42.0)
		{
			if (false == m_bHammerForm_Attack_Trail_Effect_Created)
			{
				CEffect_Manager::GetInstance()->Create_HammerForm_LastAttack_Ground(m_pTransformCom, _zeroPoint);
				m_pPlayer_Weapon->Create_Effect_HammerSwing();

				m_bHammerForm_Effect_Ember_Created = true;
				m_iEmberCreation_FrameCount = 8;

				m_bHammerForm_Attack_Trail_Effect_Created = true;
			}
		}
		else
			m_bHammerForm_Attack_Trail_Effect_Created = false;

		// 에어 커트 이펙트 생성
		if (false == m_bHammerForm_Effect_AirCut_Created && time_acc >= 43.0)
		{
			m_pPlayer_Weapon->Create_Effect_AirCut();
			m_bHammerForm_Effect_AirCut_Created = true;
		}

		if (time_acc >= 46.0)
		{
			if (false == m_bHammerForm_Effect_GroundHit_Created)
			{
				_vector hit_point = m_pPlayer_Weapon->Calculate_Point_On_Ground(2, 0);

				CEffect_Manager::GetInstance()->Create_HammerForm_Effect_GroundHit_Glow(hit_point, curr_anim);
				CEffect_Manager::GetInstance()->Create_HammerForm_Effect_GroundHit_Rock(hit_point, curr_anim);
				
				CEffect_Manager::GetInstance()->Create_HammerForm_Effect_GroundHit_Wave(m_pTransformCom, hit_point, curr_anim);
				CEffect_Manager::GetInstance()->Create_HammerForm_Effect_GroundHit_Crown(m_pTransformCom, hit_point, 30.f, curr_anim);

				m_bHammerForm_Effect_GroundHit_Created = true;

				// 카메라 셰이킹
				GM->Get_Camera()->Set_VerticalShake(0.65f, 0.f, 1.5f);
			}
		}
		else
			m_bHammerForm_Effect_GroundHit_Created = false;

		// 라스트 어택 사운드 재생
		if (m_iLastAttackSound_PlayedFrame != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (7 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormLastSound();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iLastAttackSound_PlayedFrame = time_acc_in_uint;
		}

		// 보이스 사운드 재생
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (16 == time_acc_in_uint)
				m_pPlayer->Play_LastAttackVoiceSound();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (32 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormSwing();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		// 그라운드 히트 사운드 재생
		if (m_iGrounHitSound_PlayedFrame != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (46 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormWaveSound(curr_anim);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iGrounHitSound_PlayedFrame = time_acc_in_uint;
		}

		break;
	case CPlayer::SORA_ANIM_HAMMER_AIR_LAST_ATTACK:

		// LAST COLLIDER
		if (24.0 <= time_acc && 28.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider_Hammer(CPlayer_Weapon::HAMMER_COLLIDER_LAST, true);
			m_pPlayer_Weapon->Set_KnockBackPower(8.f);
		}
		else
			m_pPlayer_Weapon->RegisterCollider_Hammer(CPlayer_Weapon::HAMMER_COLLIDER_LAST, false);

		// 해머 폼 라스트 어택 상승기류 이펙트 생성
		if (time_acc >= 0.0)
		{
			if (false == m_bHammerForm_LastAttack_Effect_UpStream)
			{
				CEffect_Manager::GetInstance()->Create_HammerForm_LastAttack_UpStream(m_pTransformCom, _zeroPoint);
				m_bHammerForm_LastAttack_Effect_UpStream = true;
			}
		}
		else
			m_bHammerForm_LastAttack_Effect_UpStream = false;

		// 해머 폼 라스트 어택 에어 이펙트 생성
		if (time_acc >= 24.0)
		{
			if (false == m_bHammerForm_LastAttack_Effect_AirHit)
			{
				m_pPlayer_Weapon->Create_Effect_AirHit();
				m_bHammerForm_LastAttack_Effect_AirHit = true;
			}
		}
		else
			m_bHammerForm_LastAttack_Effect_AirHit = false;

		// 해머 폼 메시 트레일 이펙트 생성
		if (time_acc >= 20.0)
		{
			if (false == m_bHammerForm_Attack_Trail_Effect_Created)
			{
				CEffect_Manager::GetInstance()->Create_HammerForm_LastAttack_Air(m_pTransformCom, _zeroPoint);
				m_pPlayer_Weapon->Create_Effect_HammerSwing();

				m_bHammerForm_Effect_Ember_Created = true;
				m_iEmberCreation_FrameCount = 8;

				m_bHammerForm_Attack_Trail_Effect_Created = true;
			}
		}
		else
			m_bHammerForm_Attack_Trail_Effect_Created = false;

		// 에어 커트 이펙트 생성
		if (false == m_bHammerForm_Effect_AirCut_Created && time_acc >= 21.0)
		{
			m_pPlayer_Weapon->Create_Effect_AirCut();
			m_bHammerForm_Effect_AirCut_Created = true;
		}

		// 라스트 어택 사운드 재생
		if (m_iLastAttackSound_PlayedFrame != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (2 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormLastSound();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iLastAttackSound_PlayedFrame = time_acc_in_uint;
		}

		// 보이스 사운드 재생
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (13 == time_acc_in_uint)
				m_pPlayer->Play_LastAttackVoiceSound();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (18 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormSwing();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		break;
	case CPlayer::SORA_ANIM_HAMMER_FINISH:

		if (14.0 <= time_acc && 26.0 > time_acc ||
			42.0 <= time_acc && 47.0 > time_acc ||
			53.0 <= time_acc && 58.0 > time_acc ||
			62.0 <= time_acc && 67.0 > time_acc || 
			74.0 <= time_acc && 78.0 > time_acc || 
			81.0 <= time_acc && 86.0 > time_acc || 
			90.0 <= time_acc && 95.0 > time_acc || 
			114.0 <= time_acc && 120.0 > time_acc || 
			140.0 <= time_acc && 144.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider_Hammer(CPlayer_Weapon::HAMMER_COLLIDER_FINISH, true);
			m_pPlayer_Weapon->Set_KnockBackPower(0.15f);

			if(140.0 <= time_acc && 144.0 > time_acc)
				m_pPlayer_Weapon->Set_KnockBackPower(10.f);
		}
		else
			m_pPlayer_Weapon->RegisterCollider_Hammer(CPlayer_Weapon::HAMMER_COLLIDER_FINISH, false);

		break;
	}

	if (true == m_bHammerForm_Effect_Ember_Created && 0 < m_iEmberCreation_FrameCount)
	{
		CGameObject* particle_out = nullptr;

		particle_out = m_pPlayer_Weapon->Create_Effect_HammerEmbers();
		m_iEmberCreation_FrameCount--;

		if (nullptr == particle_out)
		{
			_breakpoint;
			return;
		}

		m_EmberEffects.push_back(particle_out);
	}
	
	if(0 == m_iEmberCreation_FrameCount && 0 != m_EmberEffects.size())
	{
		for (auto particle : m_EmberEffects)
			static_cast<CEffect_Particle*>(particle)->Set_ParticleEnd(true);
	}
}

void CState_PlayerAttack::Register_Colliders_On_RageForm(_double TimeDelta)
{
	if (false == m_pModelCom->Check_TempAndCurr_EachBody(CPlayer::SB_BODY))
	{
		// 이펙트 생성변수 리셋
		Reset_EffectCreated_Boolean_For_RageForm();
		m_iRageFormSequence = 0;

		// 사운드 재생 변수 리셋
		m_iSwingSound_PlayedFrame[CPlayer::SF_RAGE] = UINT_MAX;

		return;
	}

	CPlayer::SORA_ANIM current_anim = (CPlayer::SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);
	_double time_acc = m_pModelCom->Get_TargetTimeAcc(current_anim);
	_uint time_acc_in_uint = (_uint)time_acc;

	switch (current_anim)
	{
	case Client::CPlayer::SORA_ANIM_RAGE_DOUBLE_TAP:

		if (7.0 <= time_acc && 12.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_BLADE, true);
			m_pPlayer_Weapon->Set_KnockBackPower(0.15f);
		}
		else if (14 <= time_acc && 19.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_NAIL, true);
			m_pPlayer_Weapon->Set_KnockBackPower(2.f);
		}
		else
		{
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_BLADE, false);
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_NAIL, false);
		}

		// 레이지 폼 - Double Tap 이펙트 생성
		if (false == m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_BLADE] && time_acc >= 4.0)
		{
			//CEffect_Manager::GetInstance()->Create_RageForm_DoubleTap(m_pPlayer_Weapon->Rage_Collider_Position(CPlayer_Weapon::RAGE_COLLIDER_BLADE), m_pTransformCom->Get_Look());
			//m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_BLADE] = true;
		}

		if (false == m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_NAIL] && time_acc >= 13.0)
		{
			CEffect_Manager::GetInstance()->Create_RageForm_DoubleTap(m_pPlayer_Weapon->Rage_Collider_Position(CPlayer_Weapon::RAGE_COLLIDER_NAIL), m_pTransformCom->Get_Look());
			m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_NAIL] = true;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_RAGE] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (4 == time_acc_in_uint || 14 == time_acc_in_uint)
				m_pPlayer->Play_RageFormSwing(0.65f);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_RAGE] = time_acc_in_uint;
		}

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_TRIPLE_TAP:

		if (9.0 <= time_acc && 13.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_BLADE, true);
			m_pPlayer_Weapon->Set_KnockBackPower(0.25f);
		}
		else if (14.0 <= time_acc && 18.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_NAIL, true);
			m_pPlayer_Weapon->Set_KnockBackPower(0.25f);
		}
		else if (19.0 <= time_acc && 23.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_BLADE, true);
			m_pPlayer_Weapon->Set_KnockBackPower(0.25f);
		}
		else
		{
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_BLADE, false);
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_NAIL, false);
		}

		// 레이지 폼 - Triple Tap 이펙트 생성
		if (false == m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_NAIL] && time_acc >= 8.0)
		{
			CEffect_Manager::GetInstance()->Create_RageForm_TripleTap(m_pPlayer_Weapon->Rage_Collider_Position(CPlayer_Weapon::RAGE_COLLIDER_NAIL), m_pTransformCom->Get_Look());
			m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_NAIL] = true;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_RAGE] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (9 == time_acc_in_uint || 14 == time_acc_in_uint || 19 == time_acc_in_uint)
				m_pPlayer->Play_RageFormSwing(0.65f);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_RAGE] = time_acc_in_uint;
		}

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_ATTACK1:

		if (8.0 <= time_acc && 11.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_NAIL, true);
			m_pPlayer_Weapon->Set_KnockBackPower(0.25f);
		}
		else if (12.0 <= time_acc && 15.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_BLADE, true);
			m_pPlayer_Weapon->Set_KnockBackPower(0.25f);
		}
		else if (16.0 <= time_acc && 19.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_NAIL, true);
			m_pPlayer_Weapon->Set_KnockBackPower(0.25f);
		}
		else if (20.0 <= time_acc && 23.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_BLADE, true);
			m_pPlayer_Weapon->Set_KnockBackPower(0.25f);
		}
		else if (24.0 <= time_acc && 33.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_BLADE, true);
			m_pPlayer_Weapon->Set_KnockBackPower(0.25f);
		}
		else
		{
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_BLADE, false);
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_NAIL, false);
		}

		// 레이지 폼 - SORA_ANIM_RAGE_ATTACK1 이펙트 생성
		if (time_acc >= 7.0 && time_acc < 10.0)
		{
			if (false == m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_NAIL])
			{
				CEffect_Manager::GetInstance()->Create_RageForm_RageAttack1(m_pPlayer_Weapon->Rage_Collider_Position(CPlayer_Weapon::RAGE_COLLIDER_NAIL), m_pTransformCom->Get_Look(), 0);
				m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_NAIL] = true;
			}
		}
		else
		{
			m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_NAIL] = false;
		}

		if (time_acc >= 11.0 && time_acc < 14.0 ||
			time_acc >= 23.0 && time_acc < 26.0)
		{
			if (false == m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_BLADE])
			{
				_uint sequence = 1;
				if (time_acc >= 23.0 && time_acc < 26.0)
					sequence = 2;

				CEffect_Manager::GetInstance()->Create_RageForm_RageAttack1(m_pPlayer_Weapon->Rage_Collider_Position(CPlayer_Weapon::RAGE_COLLIDER_BLADE), m_pTransformCom->Get_Look(), sequence);
				m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_BLADE] = true;
			}
		}
		else
		{
			m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_BLADE] = false;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_RAGE] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (8 == time_acc_in_uint || 12 == time_acc_in_uint || 
				16 == time_acc_in_uint || 20 == time_acc_in_uint || 24 == time_acc_in_uint)
				m_pPlayer->Play_RageFormSwing(0.65f);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_RAGE] = time_acc_in_uint;
		}

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_ATTACK2:

		if (8.0 <= time_acc && 12.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_BLADE, true);
			m_pPlayer_Weapon->Set_KnockBackPower(0.25f);
		}
		else if (14.0 <= time_acc && 18.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_NAIL, true);
			m_pPlayer_Weapon->Set_KnockBackPower(0.25f);
		}
		else if (19.0 <= time_acc && 23.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_NAIL, true);
			m_pPlayer_Weapon->Set_KnockBackPower(0.25f);
		}
		else
		{
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_BLADE, false);
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_NAIL, false);
		}

		// 레이지 폼 - SORA_ANIM_RAGE_ATTACK2 이펙트 생성
		if (false == m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_BLADE] && time_acc >= 7.0)
		{
			CEffect_Manager::GetInstance()->Create_RageForm_RageAttack2(m_pPlayer_Weapon->Rage_Collider_Position(CPlayer_Weapon::RAGE_COLLIDER_BLADE), m_pTransformCom->Get_Look(), 0);
			m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_BLADE] = true;
		}

		if (time_acc >= 13.0 && time_acc < 17.0 ||
			time_acc >= 18.0 && time_acc < 22.0)
		{
			if (false == m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_NAIL])
			{
				_uint sequence = 1;
				if (time_acc >= 18.0 && time_acc < 22.0)
					sequence = 2;

				CEffect_Manager::GetInstance()->Create_RageForm_RageAttack2(m_pPlayer_Weapon->Rage_Collider_Position(CPlayer_Weapon::RAGE_COLLIDER_NAIL), m_pTransformCom->Get_Look(), sequence);
				m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_NAIL] = true;
			}
		}
		else
		{
			m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_NAIL] = false;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_RAGE] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (8 == time_acc_in_uint || 14 == time_acc_in_uint || 19 == time_acc_in_uint)
				m_pPlayer->Play_RageFormSwing(0.65f);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_RAGE] = time_acc_in_uint;
		}

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_ATTACK3:

		if (10.0 <= time_acc && 13.0 > time_acc ||
			14.0 <= time_acc && 17.0 > time_acc ||
			18.0 <= time_acc && 21.0 > time_acc ||
			22.0 <= time_acc && 25.0 > time_acc)
		{
			m_pPlayer_Weapon->Set_KnockBackPower(0.15f);
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_AROUND, true);
		}
		else
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_AROUND, false);

		// 레이지 폼 - SORA_ANIM_RAGE_ATTACK3 이펙트 생성
		if (false == m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_AROUND] && time_acc >= 9.0)
		{
			CEffect_Manager::GetInstance()->Create_RageForm_RageAttack3(m_pPlayer_Weapon->Rage_Collider_Position(CPlayer_Weapon::RAGE_COLLIDER_AROUND), m_pTransformCom->Get_Look());
			m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_AROUND] = true;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_RAGE] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (10 == time_acc_in_uint || 14 == time_acc_in_uint || 18 == time_acc_in_uint || 22 == time_acc_in_uint)
				m_pPlayer->Play_RageFormSwing(0.65f);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_RAGE] = time_acc_in_uint;
		}

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_ATTACK4:

		if (9.0 <= time_acc && 11.0 > time_acc ||
			12.0 <= time_acc && 14.0 > time_acc ||
			15.0 <= time_acc && 17.0 > time_acc ||
			18.0 <= time_acc && 20.0 > time_acc ||
			21.0 <= time_acc && 23.0 > time_acc ||
			24.0 <= time_acc && 26.0 > time_acc)
		{
			m_pPlayer_Weapon->Set_KnockBackPower(0.15f);
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_FRONT, true);

			// RageForm_Effect_Test ====================================================================================================================================

			if (false == m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_FRONT])
			{
				auto BoneMatrixPtr = m_pModelCom->Get_BoneMatrixPtr("R_buki");
				auto World = XMLoadFloat4x4(BoneMatrixPtr.pCombinedMatrix) * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();

				// 레이지 폼 - SORA_ANIM_RAGE_ATTACK4 이펙트 생성
				CEffect_Manager::GetInstance()->Create_RageForm_RageAttack4(World);
				m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_FRONT] = true;
			}

			// =========================================================================================================================================================
		}
		else
		{
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_FRONT, false);
			m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_FRONT] = false;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_RAGE] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (9 == time_acc_in_uint || 12 == time_acc_in_uint || 15 == time_acc_in_uint || 
				18 == time_acc_in_uint || 21 == time_acc_in_uint || 24 == time_acc_in_uint)
				m_pPlayer->Play_RageFormSwing(0.65f);

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_RAGE] = time_acc_in_uint;
		}

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_ATTACK5:

		if (9.0 <= time_acc && 13.0 > time_acc ||
			14.0 <= time_acc && 18.0 > time_acc ||
			19.0 <= time_acc && 23.0 > time_acc ||
			26.0 <= time_acc && 30.0 > time_acc ||
			48.0 <= time_acc && 56.0 > time_acc)
		{
			m_pPlayer_Weapon->Set_KnockBackPower(0.15f);

			if(48.0 <= time_acc && 56.0 > time_acc)
				m_pPlayer_Weapon->Set_KnockBackPower(12.f);

			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_AROUND, true);

			// RageForm_Effect_Test ====================================================================================================================================

			if (false == m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_AROUND])
			{
				// 레이지 폼 - SORA_ANIM_RAGE_ATTACK5 이펙트 생성
				CEffect_Manager::GetInstance()->Create_RageForm_RageAttack5(m_pPlayer->Get_BonePosition(), m_pTransformCom->Get_Look(), m_iRageFormSequence++);

				if (m_iRageFormSequence >= 5)
					m_iRageFormSequence = 0;

				// 레이지 폼 - SORA_ANIM_RAGE_ATTACK4 이펙트 생성 ( 좀 더 화려해보이게 추가한 것 )
				auto BoneMatrixPtr = m_pModelCom->Get_BoneMatrixPtr("R_buki");
				auto World = XMLoadFloat4x4(BoneMatrixPtr.pCombinedMatrix) * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();

				CEffect_Manager::GetInstance()->Create_RageForm_RageAttack4(World);

				m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_AROUND] = true;
				
				if (48.0 <= time_acc && 56.0 > time_acc)
				{
					// 레이지 폼 - SORA_ANIM_RAGE_ATTACK5_FINISH 이펙트 생성
					_vector offset = m_pPlayer->Get_BodyCollider_Center() - m_pTransformCom->Get_Position();
					CEffect_Manager::GetInstance()->Create_RageForm_RageAttack5_Finish(m_pTransformCom, offset);
				}
			}

			// =========================================================================================================================================================

		}
		else
		{
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_AROUND, false);
			m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_AROUND] = false;
		}

		// 스윙 사운드 재생
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_RAGE] != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (9 == time_acc_in_uint || 14 == time_acc_in_uint || 19 == time_acc_in_uint ||
				26 == time_acc_in_uint || 48 == time_acc_in_uint)
				m_pPlayer->Play_RageFormSwing(0.65f);

			if (9 == time_acc_in_uint)
				m_pPlayer->Play_RageWind();

			if (48 == time_acc_in_uint)
				m_pPlayer->Play_RageFormLastSound();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iSwingSound_PlayedFrame[CPlayer::SF_RAGE] = time_acc_in_uint;
		}

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_FINISH:

		if (46.0 <= time_acc && 50.0 > time_acc ||
			51.0 <= time_acc && 55.0 > time_acc ||
			56.0 <= time_acc && 60.0 > time_acc ||
			61.0 <= time_acc && 65.0 > time_acc ||
			66.0 <= time_acc && 70.0 > time_acc ||
			71.0 <= time_acc && 75.0 > time_acc ||
			76.0 <= time_acc && 80.0 > time_acc ||
			81.0 <= time_acc && 85.0 > time_acc)
		{
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_FINISH, true);
			m_pPlayer_Weapon->Set_KnockBackPower(0.15f);
		}
		else
			m_pPlayer_Weapon->RegisterCollider_Rage(CPlayer_Weapon::RAGE_COLLIDER_FINISH, false);

		// 레이지 폼 - SORA_ANIM_RAGE_FINISH_ATTACK 이펙트 생성
		if (time_acc >= 1.0 && time_acc < 4.0 ||
			time_acc >= 28.0 && time_acc < 32.0)
		{
			if (false == m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_FINISH])
			{
				if (time_acc >= 1.0 && time_acc < 4.0)
				{
					CEffect_Manager::GetInstance()->Create_RageForm_FinishAttack_Start(m_pTransformCom);
				}

				if (time_acc >= 28.0 && time_acc < 32.0)
				{
					_vector offset = XMVectorZero();
					if (true == m_pPlayer->Check_OnGround())
						vecy(offset) += 0.1f;

					CEffect_Manager::GetInstance()->Create_RageForm_FinishAttack(m_pTransformCom, offset);
					m_bRageForm_Attack_Effect_WaveCreation = true;
					// m_dRageForm_Attack_Effect_WaveTimer = 0.7;
				}
				
				m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_FINISH] = true;
			}
		}
		else
		{
			m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_FINISH] = false;
		}
		
		// 타이밍 맞추기 위해서 서순으로 타이머 차감 여기서 진행
		m_dRageForm_Attack_Effect_WaveTimer -= TimeDelta;

		// 레이지 폼 - SORA_ANIM_RAGE_FINISH_ATTACK_WAVE 이펙트 생성
		if (true == m_bRageForm_Attack_Effect_WaveCreation && 
			m_dRageForm_Attack_Effect_WaveTimer <= 0.0 && m_iRageForm_Attack_Effect_WaveNumber > 0)
		{
			CEffect_Manager::GetInstance()->Create_RageForm_FinishAttack_Wave(m_pTransformCom);
			m_dRageForm_Attack_Effect_WaveTimer = 0.7;

			m_iRageForm_Attack_Effect_WaveNumber--;
		}

		// 피니시 어택 사운드 재생
		if (m_iFinishSound_PlayedFrame != time_acc_in_uint)
		{
			// 사운드 재생 타이밍
			if (0 == time_acc_in_uint)
				m_pPlayer->Play_RageWind();

			if (24 == time_acc_in_uint)
				m_pPlayer->Play_RageFormLastSound();

			if (36 == time_acc_in_uint || 48 == time_acc_in_uint || 60 == time_acc_in_uint ||
				72 == time_acc_in_uint || 84 == time_acc_in_uint)
				m_pPlayer->Play_RageFormFinish();

			// 사운드가 재생된 프레임을 기록해둔다
			m_iFinishSound_PlayedFrame = time_acc_in_uint;
		}

		break;
	}
}

void CState_PlayerAttack::Reset_EffectCreated_Boolean_For_RageForm()
{
	for (_uint i = 0; i < CPlayer_Weapon::RAGE_COLLIDER_END; ++i)
		m_bRageForm_Attack_Effect_Created[i] = false;
}

CState_PlayerAttack * CState_PlayerAttack::Create(void * pArg)
{
	CState_PlayerAttack* pInstance = new CState_PlayerAttack;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

void CState_PlayerAttack::Free()
{
	__super::Free();
}
