#include "stdafx.h"
#include "State_PlayerMove.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Camera_Perspective.h"
#include "Terrain.h"
#include "Action_Assistant.h"

_uint CState_PlayerMove::g_Angle_for_ExceptionHandling = 165;

HRESULT CState_PlayerMove::Initialize(void * pArg)
{
	return S_OK;
}

_bool CState_PlayerMove::KeyInput(_double TimeDelta)
{
	CGameInstance* pGI = CGameInstance::GetInstance();

	_vector average_dir = XMVectorZero();
	vector<MOVE_DIR> move_directions;

	_bool move = false;

	if (pGI->Get_KeyPressing(DIK_A))
	{
		move_directions.push_back(DIR_LEFT);
		move = true;
	}

	else if (pGI->Get_KeyPressing(DIK_D))
	{
		move_directions.push_back(DIR_RIGHT);
		move = true;
	}

	if (pGI->Get_KeyPressing(DIK_S))
	{
		move_directions.push_back(DIR_BACKWARD);
		move = true;
	}

	else if (pGI->Get_KeyPressing(DIK_W))
	{
		move_directions.push_back(DIR_FORWARD);
		move = true;
	}

	vector<_vector> dir_vector;
	CCamera_Perspective* camera = m_pPlayer->Get_LinkedCamera();

	// temp
	CCell* current_cell = m_pNavigation->Get_CurrentCell();
	CCell::TYPE current_cell_type = current_cell->Get_Type();

	for (auto& dir : move_directions)
	{
		_vector move_dir;
		switch (dir)
		{
		case MOVE_DIR::DIR_FORWARD:
			move_dir = camera->Get_Transform()->Get_State(CTransform::STATE_LOOK);
			break;
		case MOVE_DIR::DIR_BACKWARD:
			move_dir = camera->Get_Transform()->Get_State(CTransform::STATE_LOOK) * -1.f;
			break;
		case MOVE_DIR::DIR_LEFT:
			move_dir = camera->Get_Transform()->Get_State(CTransform::STATE_RIGHT) * -1.f;

			if (CCell::TYPE::TYPE_RUNNING == current_cell_type)
				move_dir = XMLoadFloat3(&current_cell->Get_RightVector()) * -1.f;

			break;
		case MOVE_DIR::DIR_RIGHT:
			move_dir = camera->Get_Transform()->Get_State(CTransform::STATE_RIGHT);

			if (CCell::TYPE::TYPE_RUNNING == current_cell_type)
				move_dir = XMLoadFloat3(&current_cell->Get_RightVector()) * 1.f;

			break;
		}

		vecy(move_dir) = 0.f;
		move_dir = XMVector3Normalize(move_dir);

		dir_vector.push_back(move_dir);
	}

	if (true == move)
	{
		for (auto& dir : dir_vector)
			average_dir += dir;

		average_dir /= (_float)dir_vector.size();
		average_dir = XMVector3Normalize(average_dir);

		m_pPlayer->Set_MoveDir(average_dir);
	}

	return move;
}

_bool CState_PlayerMove::Action(_double TimeDelta)
{
	_bool input = KeyInput(TimeDelta);

	if (false == input)
	{
		ExitState();
		return DELETE_STATE;
	}

	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
	CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);

	_bool anim_done = m_pModelCom->CheckStop_EachBody(CPlayer::SB_BODY);
	_bool hovering = m_pPlayer->Get_Hovering();

	_bool able_to_change_animation = true;
	_bool able_to_move_position = true;

	// �ִϸ��̼� ������ ���ƾ��ϴ� ���¿� ���� ó��
	// 1. ����
	if (true == m_pStateMachine->FindState(CStateMachine::STATETYPE_JUMP))
	{
		// ȣ���� ���°� �ƴ� ������ �ִϸ��̼� ��ȯ false
		if(false == hovering)
			able_to_change_animation = false;

		// ȣ���� ������ ���� ��쿡 ���� �ִϸ��̼� ��ȯ�� ��������Ѵ�
		else
		{
			// ȣ���� ���¿��� �̵� �ִϸ��̼��� ��ȯ�ϴ� ������ Ȯ���Ѵ�
			_bool hovering_move_animation = m_pPlayer->Get_Hovering_MoveAnimation();
			if (false == hovering_move_animation)
				able_to_change_animation = false;
		}
	}

	// 2. ��ȯ
	if (true == m_pStateMachine->FindState(CStateMachine::STATETYPE_SUMMON))
	{
		able_to_change_animation = false;

		// ��ȯ ���� �� ���� �� ���°� ��� ���� �ƴ� ��� �̵� ����
		// (������ ���� ���� �ߵ��� �Ұ����ϰ� �ظ� ���� �̵� �� ������� �ִϸ��̼��� ���� ����)
		if (CPlayer::SF_NORMAL != curr_form)
			able_to_move_position = false;
	}

	// �̵� �ִϸ��̼����� ��ȯ�� ������ ���
	if (true == able_to_change_animation)
	{
		// �� ��ȭ ���¿� ���� �̵� �ִϸ��̼� ������Ʈ
		switch (curr_form)
		{
		case Client::CPlayer::SF_NORMAL:
			ChangeAnimation_OnNormalForm();
			break;
		case Client::CPlayer::SF_HAMMER:
			ChangeAnimation_OnHammerForm();
			break;
		case Client::CPlayer::SF_RAGE:
			ChangeAnimation_OnRageForm();
			break;
		}

		// ȣ���� ������ ��� �ִϸ��̼��� �ٽ� �� �� ���´�
		if (true == m_pPlayer->Get_Hovering())
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_AIR_MOVE);
	}

	// ���� ���
	PlayStepSound_OnRun();

	// ȸ��, �̵�
	if (true == able_to_move_position)
	{
		Rotate_Player(TimeDelta);
		Translate_Player(TimeDelta);
	}
	
	return KEEP_STATE;
}

HRESULT CState_PlayerMove::EnterState()
{
	// �÷��̾� ���� �� ���� ���� ����
	m_pPlayer = static_cast<CPlayer*>(m_pGameObject);
	m_pCamera = m_pPlayer->Get_LinkedCamera();
	m_pAction_Assistant = m_pPlayer->Get_ActionAssistant();

	m_pNavigation = static_cast<CNavigation*>(m_pPlayer->Get_Component(TEXT("Com_Navigation")));

	return S_OK;
}

HRESULT CState_PlayerMove::ExitState()
{
	// ��ų ��� ���°� ������ ������ �ִϸ��̼��� �ٸ� ������ ��ȯ��Ű�� �ȵȴ�
	// �ִϸ��̼� ������ ������ �ٸ� �͵鸸 �����ϰ� return
	if (true == m_pStateMachine->FindState(CStateMachine::STATETYPE_SUMMON))
	{
		// �̵����� ����
		m_pPlayer->Set_MoveDir(XMVectorZero());

		return S_OK;
	}

	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
	_bool air_state = m_pTransformCom->Get_JumpBoolean();
	_bool holding_weapon = m_pPlayer->Get_HoldingWeapon();

	// ����
	if (false == air_state)
	{
		switch (curr_form)
		{
		case Client::CPlayer::SF_NORMAL:

			if (false == holding_weapon)
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
		}
	}

	// ����
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
		}
	}

	_bool hovering = m_pPlayer->Get_Hovering();

	if (true == hovering)
		m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_AIR_IDLE);

	// �̵����� ����
	m_pPlayer->Set_MoveDir(XMVectorZero());
	
	return S_OK;
}

void CState_PlayerMove::Rotate_Player(_double TimeDelta)
{
	_vector average_dir = m_pPlayer->Get_MoveDir();

	_vector player_look = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vecy(player_look) = 0.f;
	player_look = XMVector3Normalize(player_look);

	_float angle = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenNormals(average_dir, player_look)));

	if (g_Angle_for_ExceptionHandling <= (_uint)angle)
	{
		average_dir = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

		vecy(average_dir) = 0.f;
		average_dir = XMVector3Normalize(average_dir);
	}

	// ȸ��
	_vector new_look = (player_look * 0.75f) + (average_dir * 0.25f);
	new_look = XMVector3Normalize(new_look);

	_vector new_right = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), new_look);
	new_right = XMVector3Normalize(new_right);

	_vector new_up = XMVector3Cross(new_look, new_right);
	new_up = XMVector3Normalize(new_up);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, new_right);
	m_pTransformCom->Set_State(CTransform::STATE_UP, new_up);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, new_look);
}

void CState_PlayerMove::Translate_Player(_double TimeDelta)
{
	_bool ignore_navi = static_cast<CNavigation*>(m_pPlayer->Get_Component(TEXT("Com_Navigation")))->Get_Ignore();
	
	// ���� �˻��ؼ� ���� �׺�� ��ġ���迡 ���� �ٸ� �̵����� �����ؾ��Ѵ�.
	if (false == ignore_navi)
	{
		CGameInstance* pGI = CGameInstance::GetInstance();

		_vector average_dir = m_pPlayer->Get_MoveDir();
		_vector player_position = _zeroPoint;

		// ���� �� Ÿ�Կ� ���� �ٸ���.
		CCell::TYPE current_cell_type = m_pNavigation->Get_CurrentType();
		switch (current_cell_type)
		{
		case Engine::CCell::TYPE_TERRAIN:

			if (LEVEL_RAPUNZEL_BOSS == pGI->Get_LevelIndex())
			{
				// �̵�
				m_pTransformCom->Go_Vector(average_dir, TimeDelta, m_pNavigation);

				// �ͷ��ο� �°�
				CTerrain* terrain_in_this_level = static_cast<CTerrain*>(pGI->Get_Front(LEVEL_RAPUNZEL_BOSS, TEXT("Layer_Terrain")));
				if (nullptr != terrain_in_this_level &&
					false == m_pStateMachine->FindState(CStateMachine::STATETYPE_JUMP))
				{
					// �ͷ��� ���� �÷��̾� ��ġ�� ����
					player_position = terrain_in_this_level->Compute_PositionOnTerrain(m_pTransformCom->Get_Position());
					m_pTransformCom->Set_Position(player_position);
				}
			}

			break;
		case Engine::CCell::TYPE_ROAD:
		case Engine::CCell::TYPE_FLOW:
		{
			// �̵�
			m_pTransformCom->Go_Vector(average_dir, TimeDelta, m_pNavigation);

			if (CCell::TYPE_RUNNING != m_pNavigation->Get_CurrentType() &&
				false == m_pStateMachine->FindState(CStateMachine::STATETYPE_JUMP))
			{
				// �̵� ���� �÷��̾� ��ġ �޾ƿ�
				player_position = m_pTransformCom->Get_Position();

				// �׺���̼� ��� ���� ��ġ ���� �� �ش� ������ y���� ����
				_float position_y_on_terrain = m_pNavigation->Compute_Y(player_position);
				vecy(player_position) = position_y_on_terrain;

				// ���� ��ġ ����
				m_pTransformCom->Set_Position(player_position);
			}
		}

			break;
		case Engine::CCell::TYPE_RUNNING:
		{
			/*
				Current Cell Type�� TYPE_RUNNING�ε� NormalMoveState�� ����ٴ� ����
				��Ÿ�� ���߿� ���� ���� ���¿��� NormalMoveState�� ���Դٴ� ���̴�.

				���� �հ� �������� ���ϵ��� ����ó�� �ؾ���.
				Upperside �˻��ؼ� �̵��� ���´�.
			*/

			_vector position = m_pTransformCom->Get_Position();
			_vector temp_position = position + average_dir * (_float)TimeDelta * m_pTransformCom->Get_Speed();

			CCell* current_cell = m_pNavigation->Get_CurrentCell();
			if (nullptr == current_cell)
			{
				BREAKPOINT;
			}

			if (true == current_cell->isUpperSide(temp_position))
 				m_pTransformCom->Go_Vector(average_dir, TimeDelta, m_pNavigation);
		}

			break;
		}
	}
	
	else
	{
		_vector average_dir = m_pPlayer->Get_MoveDir();
		_vector position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		position += average_dir * m_pTransformCom->Get_Speed() * (_float)TimeDelta;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, position);
	}
}

void CState_PlayerMove::ChangeAnimation_OnNormalForm()
{
	if (false == m_pPlayer->Get_HoldingWeapon())
		m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RUN_FORWARD);
	else
		m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RUN_FORWARD_BLADE);
}

void CState_PlayerMove::ChangeAnimation_OnHammerForm()
{
	m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_HAMMER_RUN);
}

void CState_PlayerMove::ChangeAnimation_OnRageForm()
{
	m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_RUN);
}

void CState_PlayerMove::PlayStepSound_OnRun()
{
	// �ִϸ��̼� �ε����� �����´�
	_uint curr_anim = m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);

	// ���带 ����� �ִϸ��̼� �������� Ȯ��
	_bool check_return = true;
	if (CPlayer::SORA_ANIM_RUN_FORWARD == curr_anim ||
		CPlayer::SORA_ANIM_RUN_FORWARD_BLADE == curr_anim ||
		CPlayer::SORA_ANIM_HAMMER_RUN == curr_anim ||
		CPlayer::SORA_ANIM_RAGE_RUN == curr_anim)
		check_return = false;

	if (true == check_return)
		return;

	// ��� �ð��� �����´�
	_double time_acc = m_pModelCom->Get_TargetTimeAcc(curr_anim);

	// ��� �ð��� UINT������ ����ȯ
	_uint time_acc_in_uint = (_uint)time_acc;

	// �÷��̾� ���� ��� �̵� �ִϸ��̼ǿ��� ���� ���鿡 �� �� ��´�
	_uint first_step = 0;
	_uint second_step = 0;

	// �ִϸ��̼ǿ� ���� ��ũ�� �´� Ÿ�̹����� first, second�� �������ش�
	switch (curr_anim)
	{
	case Client::CPlayer::SORA_ANIM_RUN_FORWARD:
		first_step = 6;
		second_step = 14;
		break;
	case Client::CPlayer::SORA_ANIM_RUN_FORWARD_BLADE:
		first_step = 7;
		second_step = 15;
		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_RUN:
		first_step = 4;
		second_step = 11;
		break;
	case Client::CPlayer::SORA_ANIM_RAGE_RUN:
		first_step = 5;
		second_step = 13;
		break;
	}

	// ����ó��
	if (0 == first_step || 0 == second_step)
	{
		_breakpoint;
		return;
	}

	// ���� �����ӿ��� ���带 ������� �ʰԲ� ������ ���尡 ����� ������ ��ϰ� ���� ����ð��� ��
	if (m_iPlayedFrame != time_acc_in_uint)
	{
		// ���� ���
		if (first_step == time_acc_in_uint || second_step == time_acc_in_uint)
		{
			// �̵� ���� ���
			m_pPlayer->Play_MoveSound();

			// ���尡 ����� �������� ����صд�
			m_iPlayedFrame = time_acc_in_uint;
		}
	}
}

CState_PlayerMove * CState_PlayerMove::Create(void * pArg)
{
	CState_PlayerMove* pInstance = new CState_PlayerMove;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

void CState_PlayerMove::Free()
{
	__super::Free();
}
