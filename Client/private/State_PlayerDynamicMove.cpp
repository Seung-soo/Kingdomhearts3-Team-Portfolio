#include "stdafx.h"
#include "..\public\State_PlayerDynamicMove.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "StateMachine.h"
#include "State_Jump.h"

#include "Camera_Perspective.h"

HRESULT CState_PlayerDynamicMove::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CState_PlayerDynamicMove::Initialize(MOVE_TYPE _moveType)
{
	m_eMoveType = _moveType;

	return S_OK;
}

_bool CState_PlayerDynamicMove::KeyInput(_double TimeDelta)
{
	// �б� �¿��� Ÿ�Կ� ���� ó��

	CGameInstance* pGI = CGameInstance::GetInstance();

	_bool input = false;
	switch (m_eMoveType)
	{
	case Client::CState_PlayerDynamicMove::MOVE_TYPE_RUN_WALL:
	{
		if (nullptr == m_pNavigation)
			break;

		_vector average_dir = XMVectorZero();
		vector<MOVE_DIR> move_directions;

		if (pGI->Get_KeyPressing(DIK_A))
		{
			move_directions.push_back(DIR_LEFT);
			input = true;
		}

		else if (pGI->Get_KeyPressing(DIK_D))
		{
			move_directions.push_back(DIR_RIGHT);
			input = true;
		}

		if (pGI->Get_KeyPressing(DIK_S))
			input = false;

		else if (pGI->Get_KeyPressing(DIK_W))
		{
			move_directions.push_back(DIR_FORWARD);
			input = true;
		}

		vector<_vector> dir_vector;
		CCamera_Perspective* camera = m_pPlayer->Get_LinkedCamera();

		CCell* current_cell = m_pNavigation->Get_CurrentCell();
		CCell::TYPE current_cell_type = current_cell->Get_Type();

		for (auto& dir : move_directions)
		{
			_vector move_dir;
			switch (dir)
			{
			case MOVE_DIR::DIR_FORWARD:

				// ���� Y�� ����
				move_dir = _axisY;

				break;
			case MOVE_DIR::DIR_LEFT:
				
				// ���� Left
				move_dir = XMLoadFloat3(&current_cell->Get_RightVector()) * -1.f;

				break;
			case MOVE_DIR::DIR_RIGHT:
				
				// ���� Right
				move_dir = XMLoadFloat3(&current_cell->Get_RightVector()) * 1.f;

				break;
			}

			move_dir = XMVector3Normalize(move_dir);

			dir_vector.push_back(move_dir);
		}

		if (true == input)
		{
			for (auto& dir : dir_vector)
				average_dir += dir;

			average_dir /= (_float)dir_vector.size();
			average_dir = XMVector3Normalize(average_dir);

			m_pPlayer->Set_MoveDir_OnWall(average_dir);
		}
	}

		break;
	case Client::CState_PlayerDynamicMove::MOVE_TYPE_SOMERSAULT:

		// �Է� ����

		break;
	case Client::CState_PlayerDynamicMove::MOVE_TYPE_STEPPING:
		break;
	}

	return input;
}

_bool CState_PlayerDynamicMove::Action(_double TimeDelta)
{
	_bool input = KeyInput(TimeDelta);

	_bool result = KEEP_STATE;
	if (false == input)
	{
		ExitState();
		result = DELETE_STATE;
	}
	else
	{
		// �Է��� ������ �����´�
		_vector average_dir = m_pPlayer->Get_MoveDir_OnWall();

		// ���� �� ������ ����
		CCell* current_cell = m_pNavigation->Get_CurrentCell();
		if (nullptr == current_cell)
		{
			BREAKPOINT;
		}

		CPlayer::SORA_ANIM current_anim = (CPlayer::SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);

		// �б� �¿��� Ÿ�Կ� ���� ó��
		switch (m_eMoveType)
		{
		case Client::CState_PlayerDynamicMove::MOVE_TYPE_RUN_WALL:

			switch (current_anim)
			{
			case Client::CPlayer::SORA_ANIM::SORA_ANIM_RUN_WALL:

				// ���⼭ ���� ã�Ƽ� ���� �׼����� ��ȯ���Ѽ� ���� ���� �Ѱ�����Ѵ�.
				// ���� ���� �ѱ�� �������� ���� ��(���� ��)�� ����µ� ���� ��(�̾��� ���� ��)�� ���� �ȿ� ��� ���ѷ��� �ɸ� �� ����

				// 1. ���� ���� ���̿� ���� �������� ����
				// 2. ���� ����� ���� Ÿ���� ã�´�. ���� ����� ���� -1�� ��쿡�� continue

				// �켱�� ���� 2������ ������ �����ؾ��Ѵ�.

				m_pTransformCom->Go_Vector(average_dir, TimeDelta, m_pNavigation);

				// ���� ������ �ö󰡴� �ڵ�
				for (_uint i = 0; i < CCell::LINES::LINE_END; ++i)
				{
					// ��ü ������ ���鼭 �̿��� �˻��Ѵ�. �̿��� ���� ��� continue
					_uint neighbor_index = current_cell->Get_NeighborIndex((CCell::LINES)i);
					if (-1 == neighbor_index)
						continue;

					// �̿��� ���� ��� �� ����. nullptr ����
					CCell* neighbor_cell = m_pNavigation->Get_InputCell(neighbor_index);
					if (nullptr == neighbor_cell)
					{
						BREAKPOINT;
					}

					// �̿��� Ÿ�� �˻�
					CCell::TYPE type = neighbor_cell->Get_Type();
					if (CCell::TYPE::TYPE_RUNNING == type)
						continue;

					// ���� ���� ���� ���� ������ �̿� ���� � ���ΰ� ���ٸ� ���� ����� ����
					// ������ ������ �ٸ�. ���밪 ���ұ�?
					_bool upper_ground = false;
					CCell::LINES line_type = CCell::LINES::LINE_END;
					for (_uint i = 0; i < CCell::LINES::LINE_END; ++i)
					{
						_vector current_highest_line = XMLoadFloat3(&current_cell->Get_HighestLine());
						_vector neighbor_line = XMLoadFloat3(&neighbor_cell->Get_Line((CCell::LINES)i));

						if (XMVector3Equal(XMVectorAbs(current_highest_line), XMVectorAbs(neighbor_line)))
						{
							upper_ground = true;
							line_type = (CCell::LINES)i;
						}
					}

					if (false == upper_ground)
						continue;

					// �ش� ���ΰ��� �Ÿ��� �缭 Ư�� �� ������ ��� ���� �׼����� ��ȯ
					_vector position = m_pTransformCom->Get_Position();
					_float distance_to_highest_line = neighbor_cell->Compute_Distance(position, line_type);

					if (1.5f > distance_to_highest_line)
					{
						_bool bDebug = true;
						m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RISE);
					}
				}

				// ���� ��� �غ�
				{
					_uint curr_anim = m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);
					_uint time_acc_in_uint = (_uint)m_pModelCom->Get_TargetTimeAcc(curr_anim);

					// ���� �����ӿ��� ���带 ������� �ʰԲ� ������ ���尡 ����� ������ ��ϰ� ���� ����ð��� ��
					if (m_iPlayedFrame != time_acc_in_uint)
					{
						if (3 == time_acc_in_uint || 10 == time_acc_in_uint)
						{
							// �̵� ���� ���
							m_pPlayer->Play_MoveSound();

							// ���尡 ����� �������� ����صд�
							m_iPlayedFrame = time_acc_in_uint;
						}
					}
				}

				break;
			case Client::CPlayer::SORA_ANIM::SORA_ANIM_RISE:

				if (CPlayer::SORA_ANIM::SORA_ANIM_RISE == m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY))
				{
					_double time_acc = m_pModelCom->Get_TargetTimeAcc(CPlayer::SORA_ANIM_RISE);

					if (11.0 <= time_acc)
						result = DELETE_STATE;

					// ���� ��� �غ�
					{
						_uint time_acc_in_uint = (_uint)m_pModelCom->Get_TargetTimeAcc(CPlayer::SORA_ANIM_RISE);

						if (m_iPlayedFrame != time_acc_in_uint)
						{
							if (4 == time_acc_in_uint)
							{
								// �̵� ���� ���
								m_pPlayer->Play_RiseSound();

								// ���尡 ����� �������� ����صд�
								m_iPlayedFrame = time_acc_in_uint;
							}
						}
					}
				}

				break;
			}
		}
	}

	if (DELETE_STATE == result)
		ExitState();

	return result;
}

HRESULT CState_PlayerDynamicMove::EnterState()
{
	m_pPlayer = static_cast<CPlayer*>(m_pGameObject);
	m_pCombat_Assistant = m_pPlayer->Get_CombatAssistant();
	m_pAction_Assistant = m_pPlayer->Get_ActionAssistant();
	m_pPlayer_Weapon = m_pPlayer->Get_Weapon();

	m_pCamera = m_pPlayer->Get_LinkedCamera();
	m_pNavigation = static_cast<CNavigation*>(m_pPlayer->Get_Component(TEXT("Com_Navigation")));

	if (CGameInstance::GetInstance()->Get_LevelIndex() != LEVEL_TEST)
	{
		if (nullptr == m_pPlayer || nullptr == m_pNavigation)
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	// �б� �¿��� Ÿ�Կ� ���� ó��
	switch (m_eMoveType)
	{
	case Client::CState_PlayerDynamicMove::MOVE_TYPE_RUN_WALL:

		m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM::SORA_ANIM_RUN_WALL);

		break;
	case Client::CState_PlayerDynamicMove::MOVE_TYPE_SOMERSAULT:
		break;
	case Client::CState_PlayerDynamicMove::MOVE_TYPE_STEPPING:
		break;
	}

	m_pStateMachine->ClearState(CStateMachine::STATETYPE_NORMALMOVE);

	return S_OK;
}

HRESULT CState_PlayerDynamicMove::ExitState()
{
	CState_Jump::JUMP_STATE_DESC jump_state_desc;

	// �б� �¿��� Ÿ�Կ� ���� ó��
	switch (m_eMoveType)
	{
	case Client::CState_PlayerDynamicMove::MOVE_TYPE_RUN_WALL:

		m_pTransformCom->BasicJump(CTransform::JUMP_FALL);

		jump_state_desc.eObjectType = m_pPlayer->Get_ObjectType();
		jump_state_desc.iJumpAnimIndex = 2;
		jump_state_desc.iNumberOfJumpAnim = 4;

		m_pPlayer->Create_PlayerState(CStateMachine::STATETYPE_JUMP, &jump_state_desc);

		break;
	case Client::CState_PlayerDynamicMove::MOVE_TYPE_SOMERSAULT:
		break;
	case Client::CState_PlayerDynamicMove::MOVE_TYPE_STEPPING:
		break;
	}

	return S_OK;
}

CState_PlayerDynamicMove * CState_PlayerDynamicMove::Create(void * pArg)
{
	CState_PlayerDynamicMove* pInstance = new CState_PlayerDynamicMove;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

CState_PlayerDynamicMove * CState_PlayerDynamicMove::Create(MOVE_TYPE _moveType)
{
	CState_PlayerDynamicMove* pInstance = new CState_PlayerDynamicMove;

	if (FAILED(pInstance->Initialize(_moveType)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

void CState_PlayerDynamicMove::Free()
{
	__super::Free();
}
