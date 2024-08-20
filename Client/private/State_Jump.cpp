#include "stdafx.h"
#include "State_Jump.h"
#include "GameInstance.h"

#include "Player.h"
#include "GameManager.h"

#include "Terrain.h"
#include "Action_Assistant.h"

/* 
	-0.3�� �ε巴�� �������� ����. 
	-0.4 ~ -0.5�� ������� �ӵ����� �� �� �ִ� ����.
*/

_float CState_Jump::g_Falling_Speed_Limit = -0.4f;
_float CState_Jump::g_Falling_Speed_Limit_Hovering = -0.05f;
_double CState_Jump::g_SecondJump_CoolDown = 0.5;

HRESULT CState_Jump::Initialize(void * pArg)
{
	// �������
	if (nullptr != pArg)
		memcpy(&m_Desc, pArg, sizeof(JUMP_STATE_DESC));

	return S_OK;
}

_bool CState_Jump::KeyInput(_double TimeDelta)
{
	_uint current_jump_anim_index = m_JumpAnimChain[m_Desc.iJumpAnimIndex];

	_bool hovering = m_pPlayer->Get_Hovering();
	_bool on_ground = m_pPlayer->Check_OnGround();

	CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);

	if (true == CGameInstance::GetInstance()->Get_KeyEnter(DIK_SPACE))
	{
		if (true == m_pPlayer->Anim_Check_AbleToSecondJump(curr_anim))
		{
			if (false == m_bSecondJump && 0.0 >= m_dSecondJumpTimer)
			{
				// �ִϸ��̼� ü�� �缳��
				SetUp_AnimationChain(true);

				// �ִϸ��̼� ����
				Reset_State();

				// ���� ���� �缳��
				m_pTransformCom->BasicJump(CTransform::JUMP_Editional1);

				// �̴� ���� ���� ����
				if (true == hovering)
				{
					m_bSecondJump = false;
					m_dSecondJumpTimer = g_SecondJump_CoolDown;
				}
				else
					m_bSecondJump = true;

				// �̴� ������ �ߵ��� ���� ���� ���
				m_pPlayer->Play_ActionVoiceSound(0.75f);
			}
		}
	}

	/*
		ȣ���� ���¿� ���� �ִϸ��̼� ���¿��� space �Է¹޴� ���� �Ǻ��Ͽ�
		�Ϲ� ���� -> ȣ���� / ȣ���� -> �Ϲ� ���� ���� ��ȯ

		���� ���¿��� Space -> ȣ�������� ��ȯ
		ȣ���� ���¿��� L Control + Space -> �Ϲ� ���Ϸ� ��ȯ
	*/

	if (true == CGameInstance::GetInstance()->Get_KeyEnter(DIK_V))
	{
		_bool hovering_state_changed = false;
		if (false == hovering)
		{
			if (true == on_ground)
				return true;

			// ȣ���� ���� ���� �� �ִϸ��̼� ������� ����
			m_pPlayer->Set_Hovering(true);
			m_pPlayer->Set_Hovering_MoveAnimation(true);

			m_bSecondJump = false;
			m_dSecondJumpTimer = 0.0;

			hovering_state_changed = true;
		}
		else
		{
			// ȣ���� ���� ���� �� �ִϸ��̼� ������� ����
			m_pPlayer->Set_Hovering(false);
			m_pPlayer->Set_Hovering_MoveAnimation(false);

			hovering_state_changed = true;
		}
		
		if (true == hovering_state_changed)
		{
			// �������� ���·� ����
			m_pTransformCom->BasicJump(CTransform::JUMP_TYPE::JUMP_FALL);

			// �ִϸ��̼� ü�� ����
			SetUp_AnimationChain();
			m_Desc.iJumpAnimIndex = 0;

			m_Desc.iJumpAnimIndex++;
			m_Desc.iJumpAnimIndex++;
			Change_Animation();
		}
	}

	return true;
}

_bool CState_Jump::Action(_double TimeDelta)
{
	_bool input = KeyInput(TimeDelta);

	if (false == input)
	{
		ExitState();
		return DELETE_STATE;
	}

	// �߷��� ������ ���� ���� ���
	if (false == (*m_JumpDesc).bGravityEffect)
		return KEEP_STATE;

	// ���� Ÿ�� float���� ����ȯ �� �߷º��� ������
	_float jump_time = (_float)(*m_JumpDesc).dJumpTime;
	_float gravity = GM->Get_Gravity();
	
	// ���� �̵��� ���
	_float jump_move_amount = (*m_JumpDesc).fJumpPower * jump_time - gravity * jump_time * jump_time * 0.5f;

	// ȣ���� ���� ������
	_bool hovering = m_pPlayer->Get_Hovering();

	// ���� �̵��� Ŭ����
	if (false == hovering)
	{
		if (g_Falling_Speed_Limit >= jump_move_amount)
			jump_move_amount = g_Falling_Speed_Limit;
	}
	else
	{
		if (g_Falling_Speed_Limit_Hovering >= jump_move_amount)
			jump_move_amount = g_Falling_Speed_Limit_Hovering;
	}
	

	// temp_position�� ����� ������ current_position�� Ȥ�ö� �� ���� ������� ���ܵα� ����
	_vector current_position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector temp_position = current_position;

	// ���̰� ����
	vecy(temp_position) += jump_move_amount;

	// �������� �Ǵ��ϱ����� bool ����
	_bool land = false;

	// ��� ��
	if (0 <= jump_move_amount)
	{
		// �÷��̾� ����, ����� ������ �� �ڵ尡 ����.
	}

	// �ϰ� ��
	else
	{
		/*CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);
		_bool continue_logic = false;
		for (auto anim_enum : m_JumpAnimChain)
		{
			if (anim_enum == curr_anim)
				continue_logic = true;
		}

		if (false == continue_logic)
			return KEEP_STATE;*/

		// �ִϸ��̼��� ������ ��
		if (true == m_pModelCom->Get_CurrStopped_EachBody(CPlayer::SB_BODY))
		{
			_uint current_jump_anim_index = m_JumpAnimChain[m_Desc.iJumpAnimIndex];

			// �ִϸ��̼� ����� �� �˻�
			if (true == m_pPlayer->Anim_Check_PlusJumpChain(current_jump_anim_index))
			{
				// ȣ���� �̵��� ����� �� �ִ� �������� Ȯ�� �� �ش� ���� true
				if (true == m_pPlayer->Anim_Check_HoveringStart(current_jump_anim_index))
					m_pPlayer->Set_Hovering_MoveAnimation(true);

				m_Desc.iJumpAnimIndex++;
				Change_Animation();
			}
		}

		// ���� ���� ����
		CCell* current_cell = m_pNavigation->Get_CurrentCell();
		if (nullptr == current_cell)
		{
			BREAKPOINT;
		}

		// cell type == TYPE_RUNNING �� ���� ����
		if (CCell::TYPE::TYPE_RUNNING == current_cell->Get_Type())
		{
			if (current_cell->LowestY() > vecy(temp_position))
			{
				// ��ġ ����
				vecy(temp_position) = current_cell->LowestY();

				// ���� ����
				land = true;
			}
		}

		// �� ���� ����
		else
		{
			CGameInstance* pGI = CGameInstance::GetInstance();

			LEVEL current_level = (LEVEL)pGI->Get_LevelIndex();
			CTerrain* terrain_in_this_level = nullptr;
			switch (current_level)
			{
			case LEVEL_TWILIGHT:
			case LEVEL_TEST:
			case LEVEL_RAPUNZEL:
			case LEVEL_FROZEN:
			case LEVEL_KEYGRAVE:
			case LEVEL_FINALBOSS:

				if (current_cell->Compute_Y(temp_position) > vecy(temp_position))
				{
					// ��ġ ����
					vecy(temp_position) = current_cell->Compute_Y(temp_position);

					// ���� ����
					land = true;
				}

				break;
			case LEVEL_RAPUNZEL_BOSS:

				terrain_in_this_level = static_cast<CTerrain*>(pGI->Get_Front(current_level, TEXT("Layer_Terrain")));
				if (nullptr != terrain_in_this_level)
				{
					_float player_position_on_terrain = vecy(terrain_in_this_level->Compute_PositionOnTerrain(temp_position));
					if (player_position_on_terrain > vecy(temp_position))
					{
						// ��ġ ����
						vecy(temp_position) = player_position_on_terrain;

						// ���� ����
						land = true;
					}
				}

				break;
			}
		}
	}
	
	// ��ġ, �ð��� �����Ѵ�
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, temp_position);
	(*m_JumpDesc).dJumpTime += TimeDelta * (*m_JumpDesc).fJumpSpeed;

	// 2�� ���� ��ٿ� ����
	m_dSecondJumpTimer -= TimeDelta;

	// �� �����ӿ� �ϼ��� ���� ������ Ʈ�������� ����
	m_pTransformCom->Set_JumpDesc((*m_JumpDesc));

	if (true == land)
	{
		m_pTransformCom->BasicJump(CTransform::JUMP_LAND);

		m_pPlayer->Set_Hovering(false);

		m_pStateMachine->ClearState(CStateMachine::STATETYPE_NORMALMOVE);
		m_pStateMachine->ClearState(CStateMachine::STATETYPE_ATK);
		m_pStateMachine->ClearState(CStateMachine::STATETYPE_GUARD);
		m_pStateMachine->ClearState(CStateMachine::STATETYPE_AVOID);
		m_pStateMachine->ClearState(CStateMachine::STATETYPE_DAMAGED);
		m_pStateMachine->ClearState(CStateMachine::STATETYPE_SUMMON);

		// ���� ���� �ִϸ��̼� �ε��� ����(����)
		m_Desc.iJumpAnimIndex = (_uint)m_JumpAnimChain.size() - 1;
		Change_Animation();

		m_pNavigation->Set_Climb(false);
		
		if (false == GM->Get_IgnoreNavigation())
			m_pNavigation->Find_Cell(m_pTransformCom->Get_Position());

		m_pPlayer->Set_MoveAmountY(0.f);

		ExitState();
		return DELETE_STATE;
	}

	return KEEP_STATE;
}

HRESULT CState_Jump::EnterState()
{
	// �÷��̾� ����
	m_pPlayer = static_cast<CPlayer*>(m_pGameObject);

	// �÷��̾� ���� ���� ����
	m_pCombat_Assistant = m_pPlayer->Get_CombatAssistant();
	m_pAction_Assistant = m_pPlayer->Get_ActionAssistant();
	m_pPlayer_Weapon = m_pPlayer->Get_Weapon();

	m_pNavigation = static_cast<CNavigation*>(m_pPlayer->Get_Component(TEXT("Com_Navigation")));

	if (nullptr != m_pNavigation)
		m_pNavigation->Set_Climb(false);

	// ���� DESC �޾Ƴ���
 	m_JumpDesc = m_pTransformCom->Get_JumpDescPtr();
	m_JumpAnimChain.reserve(m_Desc.iNumberOfJumpAnim);

	// �ִϸ��̼� ü�� ����
	SetUp_AnimationChain();

	// �ִϸ��̼� ü���� ù��° �ִϸ��̼����� ��ȯ
	if (true == m_Desc.bChangeAnimation)
	{
		Change_Animation();
		m_pPlayer->Play_ActionVoiceSound(0.75f);
	}

	// ȣ���� ������ �̵� �ִϸ��̼� ������� ���� ����
	m_pPlayer->Set_Hovering_MoveAnimation(false);

	// ���� ����� ���� ���� �� ������ �ٽ� �߻���Ű�� Able�� �� �ٲ� �̵��� �ȵǴ� ���� ������ ���⼭ true
	// ������ ���ʿ� ���� ����� ���� ���� �� ������ �߻��ϴ� �� ��ü�� ����
	m_pAction_Assistant->Able(A_ASSIST::ASSIST_MOVE, true);

	return S_OK;
}

HRESULT CState_Jump::ExitState()
{
	return S_OK;
}

void CState_Jump::Reset_State()
{
	// ȣ���� ������ �̵� �ִϸ��̼� ������� ���� ����
	m_pPlayer->Set_Hovering_MoveAnimation(false);

	m_Desc.iJumpAnimIndex = 0;
	Change_Animation();
}

void CState_Jump::Change_Animation()
{
	m_pModelCom->ResetAnimation(m_JumpAnimChain[m_Desc.iJumpAnimIndex]);
	static_cast<CPlayer*>(m_pGameObject)->Change_PlayerAnimation((CPlayer::SORA_ANIM)m_JumpAnimChain[m_Desc.iJumpAnimIndex]);
}

void CState_Jump::Change_Animation_Falling()
{
	m_Desc.iJumpAnimIndex = 2;
	Change_Animation();
}

void CState_Jump::SetUp_AnimationChain(_bool bSecond)
{
	// �̹� �ִϸ��̼� ü���� ���� ��� Ŭ����
	if (0 != m_JumpAnimChain.size())
		m_JumpAnimChain.clear();

	// ȣ���� ���� �޾Ƴ���
	_bool hovering = m_pPlayer->Get_Hovering();

	// ���� �� ���¿� �´� ���� �ִϸ��̼� ü�� ����
	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
	switch (curr_form)
	{
	case Client::CPlayer::SF_NORMAL:

 		if (false == hovering)
		{
			if (false == bSecond)
			{
				m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_JUMP_UP);
				m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_FALL_START);
				m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_FALL_LOOP);
				m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_FALL_END);
			}
			else
			{
				m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_SECOND_JUMP_UP);
				m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_SECOND_JUMP_FALL_START);
				m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_FALL_LOOP);
				m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_FALL_END);
			}
		}
		else
		{
			m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_SECOND_JUMP_UP);
			m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_SECOND_JUMP_FALL_START);
			m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_FALL_LOOP);
			m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_FALL_END);
		}

		break;
	case Client::CPlayer::SF_HAMMER:

		if (false == hovering)
		{
			if (false == bSecond)
			{
				m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_HAMMER_JUMP_UP);
				m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_HAMMER_FALL_START);
				m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_HAMMER_FALL_LOOP);
				m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_HAMMER_FALL_END);
			}
			else
			{
				m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_HAMMER_SECOND_JUMP_UP);
				m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_HAMMER_FALL_START);
				m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_HAMMER_FALL_LOOP);
				m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_HAMMER_FALL_END);
			}
		}
		else
		{
			m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_HAMMER_SECOND_JUMP_UP);
			m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_HAMMER_FALL_START);
			m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_HAMMER_FALL_LOOP);
			m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_HAMMER_FALL_END);
		}

		break;
	case Client::CPlayer::SF_RAGE:

		if (false == hovering)
		{
			if (false == bSecond)
			{
				m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_RAGE_JUMP_UP);
				m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_RAGE_FALL_START);
				m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_RAGE_FALL_LOOP);
				m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_RAGE_FALL_END);
			}
			else
			{
				m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_SECOND_JUMP_UP);
				m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_SECOND_JUMP_FALL_START);
				m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_RAGE_FALL_LOOP);
				m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_RAGE_FALL_END);
			}
		}
		else
		{
			m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_SECOND_JUMP_UP);
			m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_SECOND_JUMP_FALL_START);
			m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_RAGE_FALL_LOOP);
			m_JumpAnimChain.push_back(CPlayer::SORA_ANIM_RAGE_FALL_END);
		}

		break;
	}
}

_bool CState_Jump::Check_JumpAnimation(CPlayer::SORA_ANIM eAnimation)
{
	if (0 >= m_JumpAnimChain.size())
	{
		_breakpoint;
		return false;
	}

	_bool result = false;

	for (auto anim_enum : m_JumpAnimChain)
	{
		if (anim_enum == eAnimation)
		{
			result = true;
			break;
		}
	}

	return result;
}

CState_Jump * CState_Jump::Create(void * pArg)
{
	CState_Jump* pInstance = new CState_Jump;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

void CState_Jump::Free()
{
	__super::Free();
}
