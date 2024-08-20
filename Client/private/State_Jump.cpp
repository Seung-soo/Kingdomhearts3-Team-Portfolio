#include "stdafx.h"
#include "State_Jump.h"
#include "GameInstance.h"

#include "Player.h"
#include "GameManager.h"

#include "Terrain.h"
#include "Action_Assistant.h"

/* 
	-0.3은 부드럽게 떨어지는 느낌. 
	-0.4 ~ -0.5는 어느정도 속도감을 줄 수 있는 느낌.
*/

_float CState_Jump::g_Falling_Speed_Limit = -0.4f;
_float CState_Jump::g_Falling_Speed_Limit_Hovering = -0.05f;
_double CState_Jump::g_SecondJump_CoolDown = 0.5;

HRESULT CState_Jump::Initialize(void * pArg)
{
	// 멤버변수
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
				// 애니메이션 체인 재설정
				SetUp_AnimationChain(true);

				// 애니메이션 변경
				Reset_State();

				// 점프 상태 재설정
				m_pTransformCom->BasicJump(CTransform::JUMP_Editional1);

				// 이단 점프 여부 설정
				if (true == hovering)
				{
					m_bSecondJump = false;
					m_dSecondJumpTimer = g_SecondJump_CoolDown;
				}
				else
					m_bSecondJump = true;

				// 이단 점프가 발동될 때에 사운드 재생
				m_pPlayer->Play_ActionVoiceSound(0.75f);
			}
		}
	}

	/*
		호버링 상태와 현재 애니메이션 상태에서 space 입력받는 것을 판별하여
		일반 점프 -> 호버링 / 호버링 -> 일반 점프 상태 전환

		점프 상태에서 Space -> 호버링으로 전환
		호버링 상태에서 L Control + Space -> 일반 낙하로 전환
	*/

	if (true == CGameInstance::GetInstance()->Get_KeyEnter(DIK_V))
	{
		_bool hovering_state_changed = false;
		if (false == hovering)
		{
			if (true == on_ground)
				return true;

			// 호버링 상태 변경 및 애니메이션 재생변수 리셋
			m_pPlayer->Set_Hovering(true);
			m_pPlayer->Set_Hovering_MoveAnimation(true);

			m_bSecondJump = false;
			m_dSecondJumpTimer = 0.0;

			hovering_state_changed = true;
		}
		else
		{
			// 호버링 상태 변경 및 애니메이션 재생변수 리셋
			m_pPlayer->Set_Hovering(false);
			m_pPlayer->Set_Hovering_MoveAnimation(false);

			hovering_state_changed = true;
		}
		
		if (true == hovering_state_changed)
		{
			// 자유낙하 상태로 설정
			m_pTransformCom->BasicJump(CTransform::JUMP_TYPE::JUMP_FALL);

			// 애니메이션 체인 세팅
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

	// 중력의 영향을 받지 않을 경우
	if (false == (*m_JumpDesc).bGravityEffect)
		return KEEP_STATE;

	// 점프 타임 float으로 형변환 및 중력변수 가져옴
	_float jump_time = (_float)(*m_JumpDesc).dJumpTime;
	_float gravity = GM->Get_Gravity();
	
	// 점프 이동량 계산
	_float jump_move_amount = (*m_JumpDesc).fJumpPower * jump_time - gravity * jump_time * jump_time * 0.5f;

	// 호버링 상태 가져옴
	_bool hovering = m_pPlayer->Get_Hovering();

	// 점프 이동량 클리핑
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
	

	// temp_position을 만드는 이유는 current_position을 혹시라도 쓸 일이 있을까봐 남겨두기 위함
	_vector current_position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector temp_position = current_position;

	// 높이값 갱신
	vecy(temp_position) += jump_move_amount;

	// 착지여부 판단하기위한 bool 변수
	_bool land = false;

	// 상승 중
	if (0 <= jump_move_amount)
	{
		// 플레이어 기준, 현재는 마땅히 들어갈 코드가 없음.
	}

	// 하강 중
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

		// 애니메이션이 멈췄을 때
		if (true == m_pModelCom->Get_CurrStopped_EachBody(CPlayer::SB_BODY))
		{
			_uint current_jump_anim_index = m_JumpAnimChain[m_Desc.iJumpAnimIndex];

			// 애니메이션 경우의 수 검사
			if (true == m_pPlayer->Anim_Check_PlusJumpChain(current_jump_anim_index))
			{
				// 호버링 이동을 재생할 수 있는 조건인지 확인 후 해당 변수 true
				if (true == m_pPlayer->Anim_Check_HoveringStart(current_jump_anim_index))
					m_pPlayer->Set_Hovering_MoveAnimation(true);

				m_Desc.iJumpAnimIndex++;
				Change_Animation();
			}
		}

		// 착지 판정 시작
		CCell* current_cell = m_pNavigation->Get_CurrentCell();
		if (nullptr == current_cell)
		{
			BREAKPOINT;
		}

		// cell type == TYPE_RUNNING 일 때의 착지
		if (CCell::TYPE::TYPE_RUNNING == current_cell->Get_Type())
		{
			if (current_cell->LowestY() > vecy(temp_position))
			{
				// 위치 조정
				vecy(temp_position) = current_cell->LowestY();

				// 착지 판정
				land = true;
			}
		}

		// 그 외의 착지
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
					// 위치 조정
					vecy(temp_position) = current_cell->Compute_Y(temp_position);

					// 착지 판정
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
						// 위치 조정
						vecy(temp_position) = player_position_on_terrain;

						// 착지 판정
						land = true;
					}
				}

				break;
			}
		}
	}
	
	// 위치, 시간을 갱신한다
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, temp_position);
	(*m_JumpDesc).dJumpTime += TimeDelta * (*m_JumpDesc).fJumpSpeed;

	// 2단 점프 쿨다운 차감
	m_dSecondJumpTimer -= TimeDelta;

	// 이 프레임에 완성된 점프 정보를 트랜스폼에 세팅
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

		// 현재 점프 애니메이션 인덱스 갱신(착지)
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
	// 플레이어 참조
	m_pPlayer = static_cast<CPlayer*>(m_pGameObject);

	// 플레이어 관련 변수 참조
	m_pCombat_Assistant = m_pPlayer->Get_CombatAssistant();
	m_pAction_Assistant = m_pPlayer->Get_ActionAssistant();
	m_pPlayer_Weapon = m_pPlayer->Get_Weapon();

	m_pNavigation = static_cast<CNavigation*>(m_pPlayer->Get_Component(TEXT("Com_Navigation")));

	if (nullptr != m_pNavigation)
		m_pNavigation->Set_Climb(false);

	// 점프 DESC 받아놓기
 	m_JumpDesc = m_pTransformCom->Get_JumpDescPtr();
	m_JumpAnimChain.reserve(m_Desc.iNumberOfJumpAnim);

	// 애니메이션 체인 세팅
	SetUp_AnimationChain();

	// 애니메이션 체인의 첫번째 애니메이션으로 전환
	if (true == m_Desc.bChangeAnimation)
	{
		Change_Animation();
		m_pPlayer->Play_ActionVoiceSound(0.75f);
	}

	// 호버링 상태의 이동 애니메이션 재생여부 변수 리셋
	m_pPlayer->Set_Hovering_MoveAnimation(false);

	// 착지 모션이 진행 중일 때 점프를 다시 발생시키면 Able이 안 바뀌어서 이동이 안되는 문제 때문에 여기서 true
	// 하지만 애초에 착지 모션이 진행 중일 때 점프가 발생하는 것 자체가 문제
	m_pAction_Assistant->Able(A_ASSIST::ASSIST_MOVE, true);

	return S_OK;
}

HRESULT CState_Jump::ExitState()
{
	return S_OK;
}

void CState_Jump::Reset_State()
{
	// 호버링 상태의 이동 애니메이션 재생여부 변수 리셋
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
	// 이미 애니메이션 체인이 있을 경우 클리어
	if (0 != m_JumpAnimChain.size())
		m_JumpAnimChain.clear();

	// 호버링 상태 받아놓기
	_bool hovering = m_pPlayer->Get_Hovering();

	// 현재 폼 상태에 맞는 점프 애니메이션 체인 세팅
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
