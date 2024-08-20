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

	// 애니메이션 변경을 막아야하는 상태에 대한 처리
	// 1. 점프
	if (true == m_pStateMachine->FindState(CStateMachine::STATETYPE_JUMP))
	{
		// 호버링 상태가 아닐 때에는 애니메이션 전환 false
		if(false == hovering)
			able_to_change_animation = false;

		// 호버링 상태일 때는 경우에 따라서 애니메이션 전환을 시켜줘야한다
		else
		{
			// 호버링 상태에서 이동 애니메이션을 전환하는 조건을 확인한다
			_bool hovering_move_animation = m_pPlayer->Get_Hovering_MoveAnimation();
			if (false == hovering_move_animation)
				able_to_change_animation = false;
		}
	}

	// 2. 소환
	if (true == m_pStateMachine->FindState(CStateMachine::STATETYPE_SUMMON))
	{
		able_to_change_animation = false;

		// 소환 상태 중 현재 폼 상태가 노멀 폼이 아닐 경우 이동 제한
		// (레이지 폼은 마법 발동이 불가능하고 해머 폼은 이동 중 마법사용 애니메이션이 없기 때문)
		if (CPlayer::SF_NORMAL != curr_form)
			able_to_move_position = false;
	}

	// 이동 애니메이션으로 전환이 가능할 경우
	if (true == able_to_change_animation)
	{
		// 폼 변화 상태에 따른 이동 애니메이션 업데이트
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

		// 호버링 상태일 경우 애니메이션을 다시 한 번 덮는다
		if (true == m_pPlayer->Get_Hovering())
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_AIR_MOVE);
	}

	// 사운드 재생
	PlayStepSound_OnRun();

	// 회전, 이동
	if (true == able_to_move_position)
	{
		Rotate_Player(TimeDelta);
		Translate_Player(TimeDelta);
	}
	
	return KEEP_STATE;
}

HRESULT CState_PlayerMove::EnterState()
{
	// 플레이어 변수 및 관련 변수 참조
	m_pPlayer = static_cast<CPlayer*>(m_pGameObject);
	m_pCamera = m_pPlayer->Get_LinkedCamera();
	m_pAction_Assistant = m_pPlayer->Get_ActionAssistant();

	m_pNavigation = static_cast<CNavigation*>(m_pPlayer->Get_Component(TEXT("Com_Navigation")));

	return S_OK;
}

HRESULT CState_PlayerMove::ExitState()
{
	// 스킬 사용 상태가 존재할 때에는 애니메이션을 다른 것으로 전환시키면 안된다
	// 애니메이션 변경을 제외한 다른 것들만 수행하고 return
	if (true == m_pStateMachine->FindState(CStateMachine::STATETYPE_SUMMON))
	{
		// 이동방향 삭제
		m_pPlayer->Set_MoveDir(XMVectorZero());

		return S_OK;
	}

	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
	_bool air_state = m_pTransformCom->Get_JumpBoolean();
	_bool holding_weapon = m_pPlayer->Get_HoldingWeapon();

	// 지상
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

	// 공중
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

	// 이동방향 삭제
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

	// 회전
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
	
	// 조건 검사해서 세로 네비와 위치관계에 따라 다른 이동식을 수행해야한다.
	if (false == ignore_navi)
	{
		CGameInstance* pGI = CGameInstance::GetInstance();

		_vector average_dir = m_pPlayer->Get_MoveDir();
		_vector player_position = _zeroPoint;

		// 현재 셀 타입에 따라서 다르게.
		CCell::TYPE current_cell_type = m_pNavigation->Get_CurrentType();
		switch (current_cell_type)
		{
		case Engine::CCell::TYPE_TERRAIN:

			if (LEVEL_RAPUNZEL_BOSS == pGI->Get_LevelIndex())
			{
				// 이동
				m_pTransformCom->Go_Vector(average_dir, TimeDelta, m_pNavigation);

				// 터레인에 맞게
				CTerrain* terrain_in_this_level = static_cast<CTerrain*>(pGI->Get_Front(LEVEL_RAPUNZEL_BOSS, TEXT("Layer_Terrain")));
				if (nullptr != terrain_in_this_level &&
					false == m_pStateMachine->FindState(CStateMachine::STATETYPE_JUMP))
				{
					// 터레인 상의 플레이어 위치를 구함
					player_position = terrain_in_this_level->Compute_PositionOnTerrain(m_pTransformCom->Get_Position());
					m_pTransformCom->Set_Position(player_position);
				}
			}

			break;
		case Engine::CCell::TYPE_ROAD:
		case Engine::CCell::TYPE_FLOW:
		{
			// 이동
			m_pTransformCom->Go_Vector(average_dir, TimeDelta, m_pNavigation);

			if (CCell::TYPE_RUNNING != m_pNavigation->Get_CurrentType() &&
				false == m_pStateMachine->FindState(CStateMachine::STATETYPE_JUMP))
			{
				// 이동 후의 플레이어 위치 받아옴
				player_position = m_pTransformCom->Get_Position();

				// 네비게이션 평면 상의 위치 연산 후 해당 값으로 y값을 조정
				_float position_y_on_terrain = m_pNavigation->Compute_Y(player_position);
				vecy(player_position) = position_y_on_terrain;

				// 최종 위치 적용
				m_pTransformCom->Set_Position(player_position);
			}
		}

			break;
		case Engine::CCell::TYPE_RUNNING:
		{
			/*
				Current Cell Type이 TYPE_RUNNING인데 NormalMoveState가 생겼다는 것은
				벽타는 도중에 끊고 낙하 상태에서 NormalMoveState가 들어왔다는 뜻이다.

				벽을 뚫고 지나가지 못하도록 예외처리 해야함.
				Upperside 검사해서 이동을 막는다.
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
	// 애니메이션 인덱스를 가져온다
	_uint curr_anim = m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);

	// 사운드를 재생할 애니메이션 상태인지 확인
	_bool check_return = true;
	if (CPlayer::SORA_ANIM_RUN_FORWARD == curr_anim ||
		CPlayer::SORA_ANIM_RUN_FORWARD_BLADE == curr_anim ||
		CPlayer::SORA_ANIM_HAMMER_RUN == curr_anim ||
		CPlayer::SORA_ANIM_RAGE_RUN == curr_anim)
		check_return = false;

	if (true == check_return)
		return;

	// 재생 시간을 가져온다
	_double time_acc = m_pModelCom->Get_TargetTimeAcc(curr_anim);

	// 재생 시간을 UINT값으로 형변환
	_uint time_acc_in_uint = (_uint)time_acc;

	// 플레이어 모델은 모든 이동 애니메이션에서 발이 지면에 두 번 닿는다
	_uint first_step = 0;
	_uint second_step = 0;

	// 애니메이션에 따라 싱크가 맞는 타이밍으로 first, second를 세팅해준다
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

	// 예외처리
	if (0 == first_step || 0 == second_step)
	{
		_breakpoint;
		return;
	}

	// 같은 프레임에는 사운드를 출력하지 않게끔 이전에 사운드가 재생된 프레임 기록과 현재 재생시간을 비교
	if (m_iPlayedFrame != time_acc_in_uint)
	{
		// 사운드 재생
		if (first_step == time_acc_in_uint || second_step == time_acc_in_uint)
		{
			// 이동 사운드 재생
			m_pPlayer->Play_MoveSound();

			// 사운드가 재생된 프레임을 기록해둔다
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
