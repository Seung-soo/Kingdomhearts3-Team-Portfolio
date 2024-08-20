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

	// ���� ������ ���� �ʱ�ȭ
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

	// ���� �ִϸ��̼��� �ƴ� ���
	if (false == m_pStateMachine->Find_AnimIndex(m_iStateType, curr_anim))

	{
		// ó���� �ʿ��Ѱ�?
		// �ʿ��ϴٸ� ������, ��� ó���ؾ��ϴ°�?

		/*
			���� ������Ʈ�� ó�� ������ ��
			���� �ִϸ��̼����� ��ȯ�ϴµ�
			��� jump_up ���·� ���� �����°ǰ�.
			=> tempIndex�� ����ϱ� ������.

			���� ���� ������Ʈ�� ���� �ʾƼ�
			EnterState()���� �־��� ���� �ִϸ��̼��� ������ ���� �ʾҴ�.
		*/

		//return false;
	}

	// ���� �ִϸ��̼��� ���
	else
	{
		// ��� ������ �Ұ����� ������ ��
		if (false == able_attack || false == able_cancle)
		{
			// ������ ���� �ƴ� ��� return
			if (CPlayer::SF_RAGE != curr_form)
				return true;
			
			// ���Է� ������ �̹� true��� return
			if (true == m_bEarlyInput)
				return true;

			// �ִϸ��̼� ���൵�� 50% �̸��̶�� return
			if (false == m_pModelCom->Check_ProgressRate_EachBody(CPlayer::SB_BODY, 50.f))
				return true;

			// LButton �Է��� ���ٸ� return
			if (false == m_pPlayer->Get_Input(CPlayer::SI_LBUTTON))
				return true;

			// �ǴϽð� �ߵ� ���̶�� return
			if (true == m_pPlayer->Check_OnFinish())
				return true;

			// ���Է� ó���� �޾Ƶд�
			if (4 > m_iAttackCount)
			{
				m_eNextAttackAnim = m_pAction_Assistant->Dice_NextAttack(m_iAttackCount);
				m_iAttackCount++;

				m_bEarlyInput = true;
			}

			return true;
		}

		// ��� ������ ������ ������ ��
		// �Ϲ� ������ ��� Ű �Է�
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

			// �ظ� ���� 6��, GROUND_LAST_ATTAK, AIR_LAST_ATTACK �� ������ �߰��Է� ���� ����
			if (CPlayer::SORA_ANIM_HAMMER_ATTACK_06 == curr_anim ||
				CPlayer::SORA_ANIM_HAMMER_GROUND_LAST_ATTACK == curr_anim ||
				CPlayer::SORA_ANIM_HAMMER_AIR_LAST_ATTACK == curr_anim)
				return true;

			// �ǴϽð� �ߵ� ���̶�� return
			if (true == m_pPlayer->Check_OnFinish())
				return true;

			if (5 > m_iAttackCount && pGI->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON))
			{
				CPlayer::SORA_ANIM next_attack_anim = m_pAction_Assistant->Dice_NextAttack(m_iAttackCount);
				m_pPlayer->Change_PlayerAnimation(next_attack_anim);

				m_pCombat_Assistant->Auto_Rotation();

				if (5 > m_iAttackCount)
					m_iAttackCount++;

				// ī�޶��ũ ����, �����ð� ����
				if (CPlayer::SORA_ANIM_HAMMER_GROUND_LAST_ATTACK == next_attack_anim)
				{
					GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_HAMMER_ATTACKEND);

					_double duration = m_pModelCom->Get_TargetDuration(next_attack_anim);
					_double play_speed = m_pModelCom->Get_TargetPlaySpeed(next_attack_anim);

					m_pPlayer->Invincibility_Input(duration, play_speed);
				}
				else if (CPlayer::SORA_ANIM_HAMMER_AIR_LAST_ATTACK == next_attack_anim)
				{
					// ī�޶� ����
					GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_HAMMER_AIRATTACKEND);

					// ���� �ð� �ο�
					_double duration = m_pModelCom->Get_TargetDuration(next_attack_anim);
					_double play_speed = m_pModelCom->Get_TargetPlaySpeed(next_attack_anim);
					m_pPlayer->Invincibility_Input(duration, play_speed);

					// ���� ���ǵ� ����
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

			// �� �Է��� �ִٸ�
			if (true == m_bEarlyInput)
			{
				// �ڷ���Ʈ�� ����ϴ� ���
				if (true == m_pPlayer->Get_Teleport())
				{
					m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_JUMP_UP);
					m_pTransformCom->Set_GravityEffect(false);
				}

				// �ڷ���Ʈ�� ������� �ʴ� ���
				else
				{
					// �� �Է¹��� ���� ó��
					//Reset_EffectCreated_Boolean_For_RageForm();
					m_pPlayer->Change_PlayerAnimation(m_eNextAttackAnim);

					if (CPlayer::SORA_ANIM_RAGE_FINISH != curr_anim)
						m_pCombat_Assistant->Auto_Rotation();

					// �� �Է� ���� ����
					m_bEarlyInput = false;
					m_eNextAttackAnim = CPlayer::SORA_ANIM_END;
				}
			}

			// �� �Է��� ���ٸ�
			else
			{
				// �ǴϽð� �ߵ� ���̶�� return
				if (true == m_pPlayer->Check_OnFinish())
					return true;

				if (false == able_attack || false == able_cancle)
					return true;

				if (4 > m_iAttackCount && pGI->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON))
				{
					m_eNextAttackAnim = m_pAction_Assistant->Dice_NextAttack(m_iAttackCount);

					// �ڷ���Ʈ�� ����ϴ� ���
					if (true == m_pPlayer->Get_Teleport())
					{
						m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_JUMP_UP);
						m_pTransformCom->Set_GravityEffect(false);

						if (4 > m_iAttackCount)
							m_iAttackCount++;
					}

					// �ڷ���Ʈ�� ������� �ʴ� ���
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
		// Ű �Է��� �� �޾Ҵٸ� true �����ؼ� StateMachine ���� �������� ��
		ExitState();
		return DELETE_STATE;
	}

	else
	{
		CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);

		// ���� �ִϸ��̼��� ���
		if (true == m_pStateMachine->Find_AnimIndex(m_iStateType, curr_anim))
		{
			// �ִϸ��̼��� ������ ����Ǿ��� = ���� ���¿��� �����.
			if (true == m_pModelCom->CheckStop_EachBody(CPlayer::SB_BODY))
			{
				// RAGE_ATTACK4�� ������ ��� RAGE_ATTACK5�� ������ȯ ��Ų��.
				if (CPlayer::SORA_ANIM_RAGE_ATTACK4 == curr_anim)
				{
					// �ִϸ��̼� ����
					m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_ATTACK5);
					m_pCombat_Assistant->Auto_Rotation();

					// ī�޶� ��ũ
					GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_RAGE_ATTACKEND);

					return KEEP_STATE;
				}

				// �ǴϽ� ������ ������ ���
				if (CPlayer::SORA_ANIM_RAGE_FINISH == curr_anim ||
					CPlayer::SORA_ANIM_HAMMER_FINISH == curr_anim)
				{
					m_pStateMachine->ClearState(CStateMachine::STATETYPE_FORM);
				}

				if (CPlayer::SORA_ANIM_HAMMER_AIR_LAST_ATTACK == curr_anim)
				{
					// ���� ���ǵ� ����
					list<CMonster*>* monster_list = GM->Get_MonstersList();
					for (auto pMonster : *monster_list)
					{
						CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), pMonster->Get_LayerTag(), 1.0);
						CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_MonsterSkill"), 1.0);
					}
				}

				// ������ ���� ������ ������ ���
				switch (curr_anim)
				{
				case Client::CPlayer::SORA_ANIM_RAGE_DOUBLE_TAP:
				case Client::CPlayer::SORA_ANIM_RAGE_TRIPLE_TAP:
				case Client::CPlayer::SORA_ANIM_RAGE_ATTACK1:
				case Client::CPlayer::SORA_ANIM_RAGE_ATTACK2:
				case Client::CPlayer::SORA_ANIM_RAGE_ATTACK3:
				case Client::CPlayer::SORA_ANIM_RAGE_ATTACK5:

					// ����
					if (false == m_pTransformCom->Get_JumpBoolean())
						m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_FALL_END);

					// ����
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

			// �ִϸ��̼��� ������� �ʾҴٸ�
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
				// �����̵�
				CCollider* target_collider = m_pCombat_Assistant->Get_TargetCollider();

				if (nullptr == target_collider)
				{
					// �� �Է¹��� ���� ó��
					if (CPlayer::SORA_ANIM_END == m_eNextAttackAnim)
						_breakpoint;

					//Reset_EffectCreated_Boolean_For_RageForm();
					m_pPlayer->Change_PlayerAnimation(m_eNextAttackAnim);

					if (CPlayer::SORA_ANIM_RAGE_FINISH != curr_anim)
						m_pCombat_Assistant->Auto_Rotation();

					// �� �Է� ���� ����
					m_bEarlyInput = false;
					m_eNextAttackAnim = CPlayer::SORA_ANIM_END;

					m_pPlayer->Set_Teleport(false);

					return KEEP_STATE;
				}
				
				_vector collider_position = target_collider->Get_Center();
				_vector player_position = m_pTransformCom->Get_Position();

				vecy(collider_position) = 0.f;
				vecy(player_position) = 0.f;

				// Ÿ�� �ݶ��̴��κ��� ������ ������ ã�� ���̱� ������ player - collider
				_vector direction_xz = _vec3norm(player_position - collider_position);
				_float distance = 0.75f;

				// �ڷ���Ʈ�� ��ġ�� 1�������� ����
				collider_position = target_collider->Get_Center();
				_vector teleport_position = collider_position + (direction_xz * distance);

				/*
					:: �ڷ���Ʈ�� �̵��� ��ġ�� ���������� �̵� ������ ��ġ���� �Ǻ��Ѵ� 
					
					�̵������� ��ġ�� �Ǻ��ϱ� ���� ���������δ� 1. �׺���̼� in �˻�� 2. ���� �˻縦 �����ؾ�������
					���� ��ȹ������ �׺���̼� in �˻�� �ʿ䰡 ���� ������ ���� �˻縸 �����Ѵ�
				*/

				_float gap_y_between_position_and_center = m_pPlayer->Calculate_GapY_Between_PositionAndCenter();
				
				// Navigation �˻�
				CCell* curr_cell = m_pPlayer->Get_Navigation()->Get_CurrentCell();

				if (nullptr == curr_cell)
					_breakpoint;

				_float height_on_cell = curr_cell->Compute_Y(teleport_position);
				_float target_point_y = vecy(teleport_position) - gap_y_between_position_and_center;

				// �̵��Ϸ��� �������� ���̰� 0�� ���ų� 0���� ���� ���
				if (height_on_cell >= target_point_y)
					target_point_y = height_on_cell;

				// �̵��Ϸ��� �������� ���̰� 0���� ���� ���
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

				// �ڵ� ȸ��
				m_pCombat_Assistant->Auto_Rotation();

				// �� �Է¹��� ���� ó��
				if (CPlayer::SORA_ANIM_END == m_eNextAttackAnim)
					_breakpoint;

				//Reset_EffectCreated_Boolean_For_RageForm();
				m_pPlayer->Change_PlayerAnimation(m_eNextAttackAnim);

				if (CPlayer::SORA_ANIM_RAGE_FINISH != curr_anim)
					m_pCombat_Assistant->Auto_Rotation();

				// �� �Է� ���� ����
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
		1. �÷��̾� ��ü ����
		2. ���� �ִϸ��̼� ����Ʈ �¾�

		3. ������ �� ���� ��������
			- ����, ���, ȸ���� 3������ ������ �� ����.
			- ���� �� 3������ ���¿��� �Ϲ����� �̵��� �۵����� �ʴ´�.

		4. �ִϸ��̼� ����
			ù��° �ִϸ��̼����� ��ȯ�Ѵ�.
			- ������ ��� ���� ���ݸ��
			- ������ ��� ���� ���ݸ��

			+ ��ȯ ������ �Ͼ ��� �߰����� �˻縦 �Ѵ�.
			- �÷��̾��� ���� aerial ����(���� OR ����)
			- �÷��̾��� ���� ��ġ(Ư�� ������ ��)
			- ����(�ݶ��̴�)�� ���� ��ġ
			=> ���󿡼� ������ �ߵ��Ǿ�� Ÿ�� �ݶ��̴��� ��ġ�� ������ ��� ���߰����� �߻��Ѵ�.
			=> �� �� Ʈ�������� ���� ���¸� �ٲ�����Ѵ�.
	*/

	// 1. �÷��̾� ����
	m_pPlayer = static_cast<CPlayer*>(m_pGameObject);
	m_pCombat_Assistant = m_pPlayer->Get_CombatAssistant();
	m_pAction_Assistant = m_pPlayer->Get_ActionAssistant();
	m_pPlayer_Weapon = m_pPlayer->Get_Weapon();

	// 3. ������ �� ���� ���� ����
	m_pStateMachine->ClearState(CStateMachine::STATETYPE_NORMALMOVE);
	m_pStateMachine->ClearState(CStateMachine::STATETYPE_GUARD);
	m_pStateMachine->ClearState(CStateMachine::STATETYPE_AVOID);

	for (_uint i = 0; i < HIT_END; ++i)
		m_bEffectCreated[i] = false;

	// 4. �ִϸ��̼� ����
	m_eNextAttackAnim = m_pAction_Assistant->Dice_NextAttack(m_iAttackCount);

	// �ڷ���Ʈ�� �ߵ��� ���
	if (true == m_pPlayer->Get_Teleport())
	{
		m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_JUMP_UP);
		m_pTransformCom->Set_GravityEffect(false);

		m_iAttackCount++;
	}

	// �ڷ���Ʈ �ߵ� ���� ���
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
	// �׼� ��ý���Ʈ���� ����� AnimOrder�� ����ش�
	m_pAction_Assistant->Clear_AttackAnimOrder();

	// ���� ���ݿ� ���� ó���� ���� �� �ʿ䰡 ����
	// ���� ���ݿ� ���� ó���� �����Ѵ�
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
	// �׼� ��ý���Ʈ���� ����� AnimOrder�� ����ش�
	m_pAction_Assistant->Clear_AttackAnimOrder();

	// Able ������ ���ش�
	m_pAction_Assistant->Able(A_ASSIST::ASSIST_CANCLE, true);
	m_pAction_Assistant->Able(A_ASSIST::ASSIST_ATTACK, true);

	// �޾Ƶ� ����Ʈ �޽ø� ����
	if (nullptr != m_pTrailMeshEffect)
		m_pTrailMeshEffect->Set_Dead(true);

	// Ʈ���� off
	m_pPlayer_Weapon->Set_Trail_Active(false); 
	 
	// ��� �ݶ��̴��� �浹���¸� ����
	m_pPlayer_Weapon->Clear_All_Colliding_Data();

	return S_OK;
}

void CState_PlayerAttack::Register_Colliders_On_Normal(_double TimeDelta)
{
	if (false == m_pModelCom->Check_TempAndCurr_EachBody(CPlayer::SB_BODY))
	{
		// ���� ��� ���� ����
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

		// ª�� Ÿ�̹� �ȿ� �ݺ�����Ǵ� ���� �������� ���������� ����ð��� ����Ͽ� ���� �ð� ���Ѵ�
		// ���̽� ���� ���
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (2 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// ���尡 ����� �������� ����صд�
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (6 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// ���尡 ����� �������� ����صд�
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

		// ���̽� ���� ���
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (5 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// ���尡 ����� �������� ����صд�
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (9 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// ���尡 ����� �������� ����صд�
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

		// ���̽� ���� ���
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (4 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// ���尡 ����� �������� ����صд�
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (8 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// ���尡 ����� �������� ����صд�
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

		// ���̽� ���� ���
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (6 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(true);

			// ���尡 ����� �������� ����صд�
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (20 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// ���尡 ����� �������� ����صд�
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

		// ���̽� ���� ���
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (0 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// ���尡 ����� �������� ����صд�
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (5 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// ���尡 ����� �������� ����صд�
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

		// ���̽� ���� ���
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (0 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// ���尡 ����� �������� ����صд�
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (4 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// ���尡 ����� �������� ����صд�
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

		// ���̽� ���� ���
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (1 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// ���尡 ����� �������� ����صд�
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (6 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// ���尡 ����� �������� ����صд�
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

		// ���̽� ���� ���
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (1 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(true);

			// ���尡 ����� �������� ����صд�
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (6 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// ���尡 ����� �������� ����صд�
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

		// ���̽� ���� ���
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (0 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// ���尡 ����� �������� ����صд�
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (5 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// ���尡 ����� �������� ����صд�
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

		// ���̽� ���� ���
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (0 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// ���尡 ����� �������� ����صд�
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (2 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// ���尡 ����� �������� ����صд�
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

		// ���̽� ���� ���
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (1 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// ���尡 ����� �������� ����صд�
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (6 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// ���尡 ����� �������� ����صд�
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

		// ���̽� ���� ���
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (0 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(true);

			// ���尡 ����� �������� ����صд�
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (1 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// ���尡 ����� �������� ����صд�
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

		// ���̽� ���� ���
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (5 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// ���尡 ����� �������� ����صд�
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (10 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// ���尡 ����� �������� ����صд�
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

		// ���̽� ���� ���
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (4 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// ���尡 ����� �������� ����صд�
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (9 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// ���尡 ����� �������� ����صд�
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

		// ���̽� ���� ���
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (4 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// ���尡 ����� �������� ����صд�
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (9 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// ���尡 ����� �������� ����صд�
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

		// ���̽� ���� ���
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (1 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(true);

			// ���尡 ����� �������� ����صд�
			m_iVoiceSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (6 == time_acc_in_uint)
				m_pPlayer->Play_NormalFormSwing();

			// ���尡 ����� �������� ����صд�
			m_iSwingSound_PlayedFrame[CPlayer::SF_NORMAL] = time_acc_in_uint;
		}

		break;
	}

	// ����Ʈ ó��
	if (true == m_pPlayer_Weapon->RegisterCollider())
		m_pPlayer_Weapon->Set_Trail_Active(true);
	else
		m_pPlayer_Weapon->Set_Trail_Active(false);
}

void CState_PlayerAttack::Register_Colliders_On_Hammer(_double TimeDelta)
{
	// �ִϸ��̼��� �ٲ������ �� �����ϰ� ����
	if (false == m_pModelCom->Check_TempAndCurr_EachBody(CPlayer::SB_BODY))
	{
		// Ʈ���� �޽� ���� ���� ����
		m_bHammerForm_Attack_Trail_Effect_Created = false;

		// ���� ĿƮ ���� ���� ����
		m_bHammerForm_Effect_AirCut_Created = false;

		// �׶��� ��Ʈ ���� ����
		m_bHammerForm_Effect_GroundHit_Created = false;

		// �ڹ� ��ƼŬ ���� ���� ����
		m_bHammerForm_Effect_Ember_Created = false;
		m_iEmberCreation_FrameCount = 0;
		m_EmberEffects.clear();

		// ���� ��� ���� ����
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

		// �ظ� �� �޽� Ʈ���� ����Ʈ ����
		if (false == m_bHammerForm_Attack_Trail_Effect_Created && time_acc >= 11.0)
		{
			CEffect_Manager::GetInstance()->Create_HammerForm_Attack1(m_pTransformCom, _zeroPoint);
			m_pPlayer_Weapon->Create_Effect_HammerSwing();

			m_bHammerForm_Effect_Ember_Created = true;
			m_iEmberCreation_FrameCount = 6;

			m_bHammerForm_Attack_Trail_Effect_Created = true;
		}

		// ���� ĿƮ ����Ʈ ����
		if (false == m_bHammerForm_Effect_AirCut_Created && time_acc >= 14.0)
		{
			m_pPlayer_Weapon->Create_Effect_AirCut();
			m_bHammerForm_Effect_AirCut_Created = true;
		}

		// �׶��� ��Ʈ ����Ʈ ����
		if (false == m_bHammerForm_Effect_GroundHit_Created && time_acc >= 18.0)
		{
			_vector hit_point = m_pPlayer_Weapon->Calculate_Point_On_Ground(2, 0);

			CEffect_Manager::GetInstance()->Create_HammerForm_Effect_GroundHit_Glow(hit_point, curr_anim);
			CEffect_Manager::GetInstance()->Create_HammerForm_Effect_GroundHit_Rock(hit_point, curr_anim);

			CEffect_Manager::GetInstance()->Create_HammerForm_Effect_GroundHit_Wave(m_pTransformCom, hit_point, curr_anim);
			CEffect_Manager::GetInstance()->Create_HammerForm_Effect_GroundHit_Crown(m_pTransformCom, hit_point, 30.f, curr_anim);

			m_bHammerForm_Effect_GroundHit_Created = true;

			// ī�޶� ����ŷ
			GM->Get_Camera()->Set_VerticalShake(0.5f, 0.f, 1.f);
		}

		// ���̽� ���� ���
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (4 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// ���尡 ����� �������� ����صд�
			m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (9 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormSwing();

			// ���尡 ����� �������� ����صд�
			m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		// �׶��� ��Ʈ ���� ���
		if (m_iGrounHitSound_PlayedFrame != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (21 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormWaveSound(curr_anim);

			// ���尡 ����� �������� ����صд�
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

		// �ظ� �� �޽� Ʈ���� ����Ʈ ����
		if (false == m_bHammerForm_Attack_Trail_Effect_Created && time_acc >= 13.0)
		{
			CEffect_Manager::GetInstance()->Create_HammerForm_Attack2(m_pTransformCom, _zeroPoint);
			m_pPlayer_Weapon->Create_Effect_HammerSwing();

			m_bHammerForm_Effect_Ember_Created = true;
			m_iEmberCreation_FrameCount = 6;

			m_bHammerForm_Attack_Trail_Effect_Created = true;
		}

		// ���� ĿƮ ����Ʈ ����
		if (false == m_bHammerForm_Effect_AirCut_Created && time_acc >= 18.0)
		{
			m_pPlayer_Weapon->Create_Effect_AirCut();
			m_bHammerForm_Effect_AirCut_Created = true;
		}

		// ���̽� ���� ���
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (8 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// ���尡 ����� �������� ����صд�
			m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (10 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormSwing();

			// ���尡 ����� �������� ����صд�
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

		// �ظ� �� �޽� Ʈ���� ����Ʈ ����
		if (false == m_bHammerForm_Attack_Trail_Effect_Created && time_acc >= 2.0)
		{
			CEffect_Manager::GetInstance()->Create_HammerForm_Attack3(m_pTransformCom, _zeroPoint);
			m_pPlayer_Weapon->Create_Effect_HammerSwing();

			m_bHammerForm_Effect_Ember_Created = true;
			m_iEmberCreation_FrameCount = 18;

			m_bHammerForm_Attack_Trail_Effect_Created = true;
		}

		// ���̽� ���� ���
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (0 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// ���尡 ����� �������� ����صд�
			m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (4 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormSwing();

			// ���尡 ����� �������� ����صд�
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

		// �ظ� �� �޽� Ʈ���� ����Ʈ ����
		if (false == m_bHammerForm_Attack_Trail_Effect_Created && time_acc >= 2.0)
		{
			CEffect_Manager::GetInstance()->Create_HammerForm_Attack4(m_pTransformCom, _zeroPoint);
			m_pPlayer_Weapon->Create_Effect_HammerSwing();

			m_bHammerForm_Effect_Ember_Created = true;
			m_iEmberCreation_FrameCount = 18;

			m_bHammerForm_Attack_Trail_Effect_Created = true;
		}

		// ���̽� ���� ���
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (6 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// ���尡 ����� �������� ����صд�
			m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (8 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormSwing();

			// ���尡 ����� �������� ����صд�
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

		// �ظ� �� �޽� Ʈ���� ����Ʈ ����
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

		// ���� ĿƮ ����Ʈ ����
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

				// ī�޶� ����ŷ
				GM->Get_Camera()->Set_VerticalShake(0.35f, 0.f, 0.5f);
			}
		}
		else
			m_bHammerForm_Effect_GroundHit_Created = false;
		
		// ���̽� ���� ���
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (6 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(false);

			// ���尡 ����� �������� ����صд�
			m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (8 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormSwing();

			// ���尡 ����� �������� ����صд�
			m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		// �׶��� ��Ʈ ���� ���
		if (m_iGrounHitSound_PlayedFrame != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (19 == time_acc_in_uint || 38 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormWaveSound(curr_anim);

			// ���尡 ����� �������� ����صд�
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

		// ī�޶� ����ŷ
		GM->Get_Camera()->Set_VerticalShake(0.5f, 0.f, 1.f);

		// ���̽� ���� ���
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (9 == time_acc_in_uint)
				m_pPlayer->Play_AttackVoiceSound(true);

			// ���尡 ����� �������� ����صд�
			m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (11 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormSwing();

			// ���尡 ����� �������� ����صд�
			m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		// �׶��� ��Ʈ ���� ���
		if (m_iGrounHitSound_PlayedFrame != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (21 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormWaveSound(curr_anim);

			// ���尡 ����� �������� ����صд�
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
		
		// �ظ� �� ��Ʈ ���� ��±�� ����Ʈ ����
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

		// �ظ� �� �޽� Ʈ���� ����Ʈ ����
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

		// ���� ĿƮ ����Ʈ ����
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

				// ī�޶� ����ŷ
				GM->Get_Camera()->Set_VerticalShake(0.65f, 0.f, 1.5f);
			}
		}
		else
			m_bHammerForm_Effect_GroundHit_Created = false;

		// ��Ʈ ���� ���� ���
		if (m_iLastAttackSound_PlayedFrame != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (7 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormLastSound();

			// ���尡 ����� �������� ����صд�
			m_iLastAttackSound_PlayedFrame = time_acc_in_uint;
		}

		// ���̽� ���� ���
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (16 == time_acc_in_uint)
				m_pPlayer->Play_LastAttackVoiceSound();

			// ���尡 ����� �������� ����صд�
			m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (32 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormSwing();

			// ���尡 ����� �������� ����صд�
			m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		// �׶��� ��Ʈ ���� ���
		if (m_iGrounHitSound_PlayedFrame != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (46 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormWaveSound(curr_anim);

			// ���尡 ����� �������� ����صд�
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

		// �ظ� �� ��Ʈ ���� ��±�� ����Ʈ ����
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

		// �ظ� �� ��Ʈ ���� ���� ����Ʈ ����
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

		// �ظ� �� �޽� Ʈ���� ����Ʈ ����
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

		// ���� ĿƮ ����Ʈ ����
		if (false == m_bHammerForm_Effect_AirCut_Created && time_acc >= 21.0)
		{
			m_pPlayer_Weapon->Create_Effect_AirCut();
			m_bHammerForm_Effect_AirCut_Created = true;
		}

		// ��Ʈ ���� ���� ���
		if (m_iLastAttackSound_PlayedFrame != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (2 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormLastSound();

			// ���尡 ����� �������� ����صд�
			m_iLastAttackSound_PlayedFrame = time_acc_in_uint;
		}

		// ���̽� ���� ���
		if (m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (13 == time_acc_in_uint)
				m_pPlayer->Play_LastAttackVoiceSound();

			// ���尡 ����� �������� ����صд�
			m_iVoiceSound_PlayedFrame[CPlayer::SF_HAMMER] = time_acc_in_uint;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_HAMMER] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (18 == time_acc_in_uint)
				m_pPlayer->Play_HammerFormSwing();

			// ���尡 ����� �������� ����صд�
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
		// ����Ʈ �������� ����
		Reset_EffectCreated_Boolean_For_RageForm();
		m_iRageFormSequence = 0;

		// ���� ��� ���� ����
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

		// ������ �� - Double Tap ����Ʈ ����
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

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_RAGE] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (4 == time_acc_in_uint || 14 == time_acc_in_uint)
				m_pPlayer->Play_RageFormSwing(0.65f);

			// ���尡 ����� �������� ����صд�
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

		// ������ �� - Triple Tap ����Ʈ ����
		if (false == m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_NAIL] && time_acc >= 8.0)
		{
			CEffect_Manager::GetInstance()->Create_RageForm_TripleTap(m_pPlayer_Weapon->Rage_Collider_Position(CPlayer_Weapon::RAGE_COLLIDER_NAIL), m_pTransformCom->Get_Look());
			m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_NAIL] = true;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_RAGE] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (9 == time_acc_in_uint || 14 == time_acc_in_uint || 19 == time_acc_in_uint)
				m_pPlayer->Play_RageFormSwing(0.65f);

			// ���尡 ����� �������� ����صд�
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

		// ������ �� - SORA_ANIM_RAGE_ATTACK1 ����Ʈ ����
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

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_RAGE] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (8 == time_acc_in_uint || 12 == time_acc_in_uint || 
				16 == time_acc_in_uint || 20 == time_acc_in_uint || 24 == time_acc_in_uint)
				m_pPlayer->Play_RageFormSwing(0.65f);

			// ���尡 ����� �������� ����صд�
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

		// ������ �� - SORA_ANIM_RAGE_ATTACK2 ����Ʈ ����
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

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_RAGE] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (8 == time_acc_in_uint || 14 == time_acc_in_uint || 19 == time_acc_in_uint)
				m_pPlayer->Play_RageFormSwing(0.65f);

			// ���尡 ����� �������� ����صд�
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

		// ������ �� - SORA_ANIM_RAGE_ATTACK3 ����Ʈ ����
		if (false == m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_AROUND] && time_acc >= 9.0)
		{
			CEffect_Manager::GetInstance()->Create_RageForm_RageAttack3(m_pPlayer_Weapon->Rage_Collider_Position(CPlayer_Weapon::RAGE_COLLIDER_AROUND), m_pTransformCom->Get_Look());
			m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_AROUND] = true;
		}

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_RAGE] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (10 == time_acc_in_uint || 14 == time_acc_in_uint || 18 == time_acc_in_uint || 22 == time_acc_in_uint)
				m_pPlayer->Play_RageFormSwing(0.65f);

			// ���尡 ����� �������� ����صд�
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

				// ������ �� - SORA_ANIM_RAGE_ATTACK4 ����Ʈ ����
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

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_RAGE] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (9 == time_acc_in_uint || 12 == time_acc_in_uint || 15 == time_acc_in_uint || 
				18 == time_acc_in_uint || 21 == time_acc_in_uint || 24 == time_acc_in_uint)
				m_pPlayer->Play_RageFormSwing(0.65f);

			// ���尡 ����� �������� ����صд�
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
				// ������ �� - SORA_ANIM_RAGE_ATTACK5 ����Ʈ ����
				CEffect_Manager::GetInstance()->Create_RageForm_RageAttack5(m_pPlayer->Get_BonePosition(), m_pTransformCom->Get_Look(), m_iRageFormSequence++);

				if (m_iRageFormSequence >= 5)
					m_iRageFormSequence = 0;

				// ������ �� - SORA_ANIM_RAGE_ATTACK4 ����Ʈ ���� ( �� �� ȭ���غ��̰� �߰��� �� )
				auto BoneMatrixPtr = m_pModelCom->Get_BoneMatrixPtr("R_buki");
				auto World = XMLoadFloat4x4(BoneMatrixPtr.pCombinedMatrix) * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();

				CEffect_Manager::GetInstance()->Create_RageForm_RageAttack4(World);

				m_bRageForm_Attack_Effect_Created[CPlayer_Weapon::RAGE_COLLIDER_AROUND] = true;
				
				if (48.0 <= time_acc && 56.0 > time_acc)
				{
					// ������ �� - SORA_ANIM_RAGE_ATTACK5_FINISH ����Ʈ ����
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

		// ���� ���� ���
		if (m_iSwingSound_PlayedFrame[CPlayer::SF_RAGE] != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (9 == time_acc_in_uint || 14 == time_acc_in_uint || 19 == time_acc_in_uint ||
				26 == time_acc_in_uint || 48 == time_acc_in_uint)
				m_pPlayer->Play_RageFormSwing(0.65f);

			if (9 == time_acc_in_uint)
				m_pPlayer->Play_RageWind();

			if (48 == time_acc_in_uint)
				m_pPlayer->Play_RageFormLastSound();

			// ���尡 ����� �������� ����صд�
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

		// ������ �� - SORA_ANIM_RAGE_FINISH_ATTACK ����Ʈ ����
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
		
		// Ÿ�̹� ���߱� ���ؼ� �������� Ÿ�̸� ���� ���⼭ ����
		m_dRageForm_Attack_Effect_WaveTimer -= TimeDelta;

		// ������ �� - SORA_ANIM_RAGE_FINISH_ATTACK_WAVE ����Ʈ ����
		if (true == m_bRageForm_Attack_Effect_WaveCreation && 
			m_dRageForm_Attack_Effect_WaveTimer <= 0.0 && m_iRageForm_Attack_Effect_WaveNumber > 0)
		{
			CEffect_Manager::GetInstance()->Create_RageForm_FinishAttack_Wave(m_pTransformCom);
			m_dRageForm_Attack_Effect_WaveTimer = 0.7;

			m_iRageForm_Attack_Effect_WaveNumber--;
		}

		// �ǴϽ� ���� ���� ���
		if (m_iFinishSound_PlayedFrame != time_acc_in_uint)
		{
			// ���� ��� Ÿ�̹�
			if (0 == time_acc_in_uint)
				m_pPlayer->Play_RageWind();

			if (24 == time_acc_in_uint)
				m_pPlayer->Play_RageFormLastSound();

			if (36 == time_acc_in_uint || 48 == time_acc_in_uint || 60 == time_acc_in_uint ||
				72 == time_acc_in_uint || 84 == time_acc_in_uint)
				m_pPlayer->Play_RageFormFinish();

			// ���尡 ����� �������� ����صд�
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
