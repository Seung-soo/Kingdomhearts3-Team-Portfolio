#include "stdafx.h"
#include "Action_Assistant.h"

#include "Transform.h"
#include "Model.h"

#include "Combat_Assistant.h"

#include "Navigation.h"
#include "Cell.h"

_float CAction_Assistant::g_CancleAction_ProgressRate = 50.f;
_float CAction_Assistant::g_AttackNormal_ProgressRate = 50.f;
_float CAction_Assistant::g_MovingNormal_ProgressRate = 50.f;
_float CAction_Assistant::g_GravityDefiance_ProgressRate = 66.f;

_float CAction_Assistant::g_CancleAction_AirDash_ProgressRate = 66.f;
_float CAction_Assistant::g_CancleAction_GuardSucceeded_ProgressRate = 33.f;

_float CAction_Assistant::g_Range_Search_Enemy = 10.f;
_uint CAction_Assistant::g_Conditional_Figures_To_Change_Attack_Animation_On_HammerForm = 3;

HRESULT CAction_Assistant::Initialize(CGameObject* _player)
{
	m_pPlayer = static_cast<CPlayer*>(_player);
	m_pPlayerTransformCom = m_pPlayer->Get_Transform();
	m_pPlayerModelCom = m_pPlayer->Get_Model();

	m_pCombat_Assistant = m_pPlayer->Get_CombatAssistant();

	if (nullptr == m_pPlayerModelCom || nullptr == m_pPlayerTransformCom)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	for (_uint i = 0; i < ASSIST_END; ++i)
	{
		m_bWork[i] = true;
		m_bAble[i] = true;
	}

	return S_OK;
}

void CAction_Assistant::Run(_double Timedelta)
{
	CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pPlayerModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);
	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();

	_bool anim_done = m_pPlayerModelCom->CheckStop_Input(curr_anim);

	vector<ASSIST_TYPE> types;
	FindOut_Type(curr_anim, &types);

	for (auto& type : types)
	{
		switch (type)
		{
		case Client::CAction_Assistant::ASSIST_CANCLE:

			// 작동 중이 아니라면 break
			if (false == m_bWork[ASSIST_CANCLE])
			{
				m_bAble[ASSIST_CANCLE] = true;
				break;
			}

			// 폼 상태에 따른 업데이트
			switch (curr_form)
			{
			case Client::CPlayer::SF_NORMAL:
				Cancle_Form_Normal(curr_anim);
				break;
			case Client::CPlayer::SF_HAMMER:
				Cancle_Form_Hammer(curr_anim);
				break;
			case Client::CPlayer::SF_RAGE:
				Cancle_Form_Rage(curr_anim);
				break;
			case Client::CPlayer::SF_END:
				_breakpoint;
				break;
			}
			
			break;
		case Client::CAction_Assistant::ASSIST_ATTACK:

			// 작동 중이 아니라면 break
			if (false == m_bWork[ASSIST_ATTACK])
			{
				m_bAble[ASSIST_ATTACK] = true;
				break;
			}

			// 폼 상태에 따른 업데이트
			switch (curr_form)
			{
			case Client::CPlayer::SF_NORMAL:
				Attack_Form_Normal(curr_anim);
				break;
			case Client::CPlayer::SF_HAMMER:
				Attack_Form_Hammer(curr_anim);
				break;
			case Client::CPlayer::SF_RAGE:
				Attack_Form_Rage(curr_anim);
				break;
			case Client::CPlayer::SF_END:
				_breakpoint;
				break;
			}

			break;
		case Client::CAction_Assistant::ASSIST_MOVE:

			// 작동 중이 아니라면 break
			if (false == m_bWork[ASSIST_MOVE])
			{
				m_bAble[ASSIST_MOVE] = true;
				break;
			}

			// 애니메이션에 따른 업데이트
			// 타이밍이 애니메이션마다 다르고 이 타입에 들어오는 애니메이션 자체가 적다
			switch (curr_anim)
			{
			case CPlayer::SORA_ANIM_FALL_END:
			case CPlayer::SORA_ANIM_RAGE_FALL_END:
			case CPlayer::SORA_ANIM_HAMMER_FALL_END:

				if (true == m_pPlayerModelCom->Check_ProgressRate_EachBody(CPlayer::SB_BODY, 50.f))
					m_bAble[ASSIST_MOVE] = true;
				else
					m_bAble[ASSIST_MOVE] = false;

				break;

			case CPlayer::SORA_ANIM_ROLL:
			case CPlayer::SORA_ANIM_RAGE_ROLL:
			case CPlayer::SORA_ANIM_AIR_DASH:
			case CPlayer::SORA_ANIM_HAMMER_DASH:

				if (true == m_pPlayerModelCom->Check_ProgressRate_EachBody(CPlayer::SB_BODY, 80.f))
					m_bAble[ASSIST_MOVE] = true;
				else
					m_bAble[ASSIST_MOVE] = false;

				break;
			}

			break;
		case Client::CAction_Assistant::ASSIST_GRAVITY:

			// 작동 중이 아니라면 break
			if (false == m_bWork[ASSIST_GRAVITY])
			{
				m_bAble[ASSIST_GRAVITY] = true;
				break;
			}

			// 점프 상태가 아니라면 break
			if (false == m_pPlayer->Get_Transform()->Get_JumpBoolean())
				break;

			switch (curr_form)
			{
			case Client::CPlayer::SF_NORMAL:
				Gravity_Form_Normal(curr_anim);
				break;
			case Client::CPlayer::SF_HAMMER:
				Gravity_Form_Hammer(curr_anim);
				break;
			case Client::CPlayer::SF_RAGE:
				Gravity_Form_Rage(curr_anim);
				break;
			case Client::CPlayer::SF_END:
				_breakpoint;
				break;
			}

			break;
		}
	}
}

void CAction_Assistant::SetUp_Weapon()
{
	if (nullptr != m_pPlayerWeapon)
		return;

	m_pPlayerWeapon = m_pPlayer->Get_Weapon();
}

_bool CAction_Assistant::Add_AssistAnim(ASSIST_TYPE _actionType, CPlayer::SORA_ANIM _soraAnim)
{
	for (auto& anim_enum : m_AssistAnim[_actionType])
	{
		if (anim_enum == _soraAnim)
			return false;
	}

	m_AssistAnim[_actionType].push_back(_soraAnim);
	return true;
}

_bool CAction_Assistant::Rmv_AssistAnim(ASSIST_TYPE _actionType, CPlayer::SORA_ANIM _soraAnim)
{
	auto iter = m_AssistAnim[_actionType].begin();
	for (auto& anim_enum : m_AssistAnim[_actionType])
	{
		if (anim_enum == _soraAnim)
		{
			m_AssistAnim[_actionType].erase(iter);
			return true;
		}
		else
			iter++;
	}

	return false;
}

void CAction_Assistant::FindOut_Type(CPlayer::SORA_ANIM _soraAnim, vector<ASSIST_TYPE>* _types)
{
	for (_uint i = 0; i < ASSIST_END; ++i)
	{
		for (auto& anim_enum : m_AssistAnim[i])
		{
			if (anim_enum != _soraAnim)
				continue;

			// 안쪽 반복문 탈출
			_types->push_back((ASSIST_TYPE)i);
			break;
		}
	}
}

CPlayer::SORA_ANIM CAction_Assistant::Dice_NextAttack(_uint _attackCount)
{
	/*
		이 함수는 AttackState 안에서 다음 공격 애니메이션을 결정할 때 호출한다
		고려해야할 항목
		1. 지상, 공중
		2. 높이
		3. 방향
		4. 이전 동작
		5. 몇번째 동작인지
	*/

	CPlayer::SORA_ANIM next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_END; 
	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();

	switch (curr_form)
	{
	case Client::CPlayer::SF_NORMAL:
		next_attack_animation = Next_AttackAnimation_Normal(_attackCount);
		break;
	case Client::CPlayer::SF_HAMMER:
		next_attack_animation = Next_AttackAnimation_Hammer(_attackCount);
		break;
	case Client::CPlayer::SF_RAGE:
		next_attack_animation = Next_AttackAnimation_Rage(_attackCount);
		break;
	case Client::CPlayer::SF_END:
		_breakpoint;
		break;
	}
	
	if (CPlayer::SORA_ANIM::SORA_ANIM_END == next_attack_animation)
		_breakpoint;

	return next_attack_animation;
}

CPlayer::SORA_ANIM CAction_Assistant::Next_AttackAnimation_Normal(_uint _attackCount)
{
	// 현재 설정된 타겟과 그에 대한 정보를 받아온다. 타겟이 없을 경우 target_desc는 ZeroMemory가 된 상태로 반환된다
	CCombat_Assistant::ASSIST_TYPE target_type = m_pCombat_Assistant->Get_TargetType();
	CCombat_Assistant::ASSIST_DESC target_desc = m_pCombat_Assistant->Get_Desc(target_type);

	// 체공 상태를 받아온다
	_bool air = m_pPlayerTransformCom->Get_JumpBoolean();

	CPlayer::SORA_ANIM next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_END;
	switch (_attackCount)
	{
	case 0:

		// 지상에 있을 때
		if (false == air)
		{
			// 높은 적
			if (1.5f < target_desc.fDistanceY)
			{
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_UPPER_ATTACK1;
			}
			// 일반 높이의 적
			else
			{
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_GROUND_ATTACK1;
			}
		}

		// 공중에 있을 때
		else
		{
			// 높은 적
			if (1.5f < target_desc.fDistanceY)
			{
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_UPPER_ATTACK1;
			}
			// 낮은 적
			else if (-0.75f > target_desc.fDistanceY)
			{
				// 찌르기 모션같은 XZ 거리에 따른 다양한 지상공격 모션도 넣으면 좋을 것.
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_LOWER_ATTACK1;
			}
			// 일반 높이의 적
			else
			{
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_AIR_ATTACK1;
			}
		}

		break;
	case 1:

		// 지상에 있을 때
		if (false == air)
		{
			next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_GROUND_ATTACK2;
		}
		// 공중에 있을 때
		else
		{
			// 높은 적
			if (1.5f < target_desc.fDistanceY)
			{
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_UPPER_ATTACK2;
			}
			// 낮은 적
			else if (-0.75f > target_desc.fDistanceY)
			{
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_LOWER_ATTACK2;
			}
			// 일반 높이의 적
			else
			{
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_AIR_ATTACK2;
			}
		}

		break;
	case 2:

		// 지상에 있을 때
		if (false == air)
		{
			next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_GROUND_ATTACK3;
		}
		// 공중에 있을 때
		else
		{
			// 높은 적
			if (1.5f < target_desc.fDistanceY)
			{
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_UPPER_ATTACK3;
			}
			// 낮은 적
			else if (-0.75f > target_desc.fDistanceY)
			{
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_LOWER_ATTACK3;
			}
			// 일반 높이의 적
			else
			{
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_AIR_ATTACK3;
			}
		}

		break;
	case 3:

		// 지상에 있을 때
		if (false == air)
		{
			next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_GROUND_ATTACK4;
		}
		// 공중에 있을 때
		else
		{
			// 높은 적
			if (1.5f < target_desc.fDistanceY)
			{
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_UPPER_ATTACK4;
			}
			// 낮은 적
			else if (-0.75f > target_desc.fDistanceY)
			{
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_LOWER_ATTACK4;
			}
			// 일반 높이의 적
			else
			{
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_AIR_ATTACK4;
			}
		}

		break;
	}

	return next_attack_animation;
}

CPlayer::SORA_ANIM CAction_Assistant::Next_AttackAnimation_Hammer(_uint _attackCount)
{
	// 현재 설정된 타겟과 그에 대한 정보를 받아온다
	CCombat_Assistant::ASSIST_TYPE target_type = m_pCombat_Assistant->Get_TargetType();
	CCombat_Assistant::ASSIST_DESC target_desc = m_pCombat_Assistant->Get_Desc(target_type);

	// 체공 상태를 받아온다
	_bool air = m_pPlayerTransformCom->Get_JumpBoolean();

	// 근처 적의 수를 받아온다
	_uint enemy_number = m_pCombat_Assistant->EnemyNumber_In_Range(g_Range_Search_Enemy);

	// 다음 애니메이션을 받아둘 변수를 미리 선언
	CPlayer::SORA_ANIM next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_END;

	// 애니메이션 확인하기 위한 변수 switch문 밖에서 미리 만들어둠
	_bool hammer_upper_attack_done = false;

	// 지상에 있을 경우
	if (false == air)
	{
		switch (_attackCount)
		{
		case 0:

			// 근처의 적이 많을 경우
			if (g_Conditional_Figures_To_Change_Attack_Animation_On_HammerForm <= enemy_number)
				next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_03;

			// 근처의 적이 적을 경우
			else
			{
				// SORA_ANIM_HAMMER_ATTACK_01 의 XZ 이동량을 받아온다
				_float movement_xz_attack1 = m_pPlayerModelCom->Get_TargetMovementAmountXZ(CPlayer::SORA_ANIM_HAMMER_ATTACK_01);

				// 타겟이 없을 경우 기본 동작으로 진행
				if(nullptr == target_desc.pCollider)
					next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_01;

				// 타겟이 있을 경우 조건에 맞는 동작으로 진행
				else
				{
					// 타겟과의 거리가 SORA_ANIM_HAMMER_ATTACK_01 의 XZ 이동량보다 작은 경우
					if (target_desc.fDistanceXZ < movement_xz_attack1)
						next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_01;

					// 타겟과의 거리가 SORA_ANIM_HAMMER_ATTACK_01 의 XZ 이동량보다 큰 경우
					else
						next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_02;
				}
			}

			break;
		case 1:

			// 이전 공격 애니메이션을 확인하여 다음 애니메이션을 선정
			switch (m_ePrevAttackAnim)
			{
			case CPlayer::SORA_ANIM_HAMMER_ATTACK_01:
				next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_03;
				break;
			case CPlayer::SORA_ANIM_HAMMER_ATTACK_02:
				next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_01;
				break;
			case CPlayer::SORA_ANIM_HAMMER_ATTACK_03:
				next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_01;
				break;
			}

			break;
		case 2:

			// 이전 공격 애니메이션을 확인하여 다음 애니메이션을 선정
			switch (m_ePrevAttackAnim)
			{
			case CPlayer::SORA_ANIM_HAMMER_ATTACK_01:

				// 하이퍼해머의 올려치기 공격 ( SORA_ANIM_HAMMER_ATTACK_02 ) 이 발동된 적 있는지 확인
				hammer_upper_attack_done = false;
				for (auto anim_enum : m_AttackAnimOrder)
				{
					if (CPlayer::SORA_ANIM_HAMMER_ATTACK_02 == anim_enum)
						hammer_upper_attack_done = true;
				}

				if(false == hammer_upper_attack_done)
					next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_05;
				else
					next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_03;

				break;
			case CPlayer::SORA_ANIM_HAMMER_ATTACK_03:
				next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_05;
				break;
			}

			break;
		case 3:

			// 이전 공격 애니메이션을 확인하여 다음 애니메이션을 선정
			switch (m_ePrevAttackAnim)
			{
			case CPlayer::SORA_ANIM_HAMMER_ATTACK_03:
				next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_05;
				break;
			case CPlayer::SORA_ANIM_HAMMER_ATTACK_05:
				next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_02;
				break;
			}
			break;
		case 4:
			next_attack_animation = CPlayer::SORA_ANIM_HAMMER_GROUND_LAST_ATTACK;
			break;
		}
	}

	// 공중에 있을 경우
	else
	{
		switch (_attackCount)
		{
		case 0:

		{
			// SORA_ANIM_HAMMER_ATTACK_04 의 XZ 이동량을 받아온다
			_float movement_xz_attack4 = m_pPlayerModelCom->Get_TargetMovementAmountXZ(CPlayer::SORA_ANIM_HAMMER_ATTACK_04);

			// 타겟이 없을 경우 기본 동작으로 진행
			if (nullptr == target_desc.pCollider)
				next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_04;

			// 타겟이 있을 경우 조건에 맞는 동작으로 진행
			else
			{
				// 타겟과의 거리가 SORA_ANIM_HAMMER_ATTACK_04 의 XZ 이동량보다 작은 경우
				if (target_desc.fDistanceXZ < movement_xz_attack4)
					next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_03;

				// 타겟과의 거리가 SORA_ANIM_HAMMER_ATTACK_04 의 XZ 이동량보다 큰 경우
				else
					next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_04;
			}
		}
			

			//// 타겟의 포지션 Y값보다 플레이어의 포지션 Y값이 더 높으면 fDistanceY 는 음수로 기록된다

			//// 타겟의 높이와 플레이어의 높이 차이가 기준치보다 큰 경우 ( 플레이어가 더 높이 있다는 가정 하에 )
			//if (nullptr != target_desc.pCollider &&
			//	-1.5f >= target_desc.fDistanceY)
			//	next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_06;
			//
			//// 타겟과 플레이어의 높이 차이가 크지 않거나 타겟의 높이가 더 높은 경우
			//else
			//{
			//	
			//}

			break;
		case 1:

			// 이전 공격 애니메이션을 확인하여 다음 애니메이션을 선정
			switch (m_ePrevAttackAnim)
			{
				// 지상 공격에서 공중으로 넘어온 경우
			case CPlayer::SORA_ANIM_HAMMER_ATTACK_02:

			{
				// SORA_ANIM_HAMMER_ATTACK_04 의 XZ 이동량을 받아온다
				_float movement_xz_attack4 = m_pPlayerModelCom->Get_TargetMovementAmountXZ(CPlayer::SORA_ANIM_HAMMER_ATTACK_04);

				// 타겟이 없을 경우 기본 동작으로 진행
				if (nullptr == target_desc.pCollider)
					next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_04;

				// 타겟이 있을 경우 조건에 맞는 동작으로 진행
				else
				{
					// 타겟과의 거리가 SORA_ANIM_HAMMER_ATTACK_04 의 XZ 이동량보다 작은 경우
					if (target_desc.fDistanceXZ < movement_xz_attack4)
						next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_03;

					// 타겟과의 거리가 SORA_ANIM_HAMMER_ATTACK_04 의 XZ 이동량보다 큰 경우
					else
						next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_04;
				}
			}

				//// 타겟의 높이와 플레이어의 높이 차이가 기준치보다 큰 경우 ( 플레이어가 더 높이 있다는 가정 하에 )
				//if (nullptr != target_desc.pCollider &&
				//	-1.5f >= target_desc.fDistanceY)
				//	next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_06;

				//// 타겟과 플레이어의 높이 차이가 크지 않거나 타겟의 높이가 더 높은 경우
				//else
				//{
				//	
				//}

				break;

				// 공중 공격으로 시작해서 공중에 머무르고 있는 경우
			case CPlayer::SORA_ANIM_HAMMER_ATTACK_03:
				next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_04;
				break;
			case CPlayer::SORA_ANIM_HAMMER_ATTACK_04:
				next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_03;
				break;
			}

			break;
		case 2:

			// 이전 공격 애니메이션을 확인하여 다음 애니메이션을 선정
			switch (m_ePrevAttackAnim)
			{
			case CPlayer::SORA_ANIM_HAMMER_ATTACK_03:
				next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_04;
				break;
			case CPlayer::SORA_ANIM_HAMMER_ATTACK_04:
				next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_03;
				break;
			}

			break;
		case 3:

			// 이미 SORA_ANIM_HAMMER_ATTACK_02 가 발동된 적이 있다면 SORA_ANIM_HAMMER_AIR_LAST_ATTACK 로 바로 넘어간다

			// 하이퍼해머의 올려치기 공격 ( SORA_ANIM_HAMMER_ATTACK_02 ) 이 발동된 적 있는지 확인
			hammer_upper_attack_done = false;
			for (auto anim_enum : m_AttackAnimOrder)
			{
				if (CPlayer::SORA_ANIM_HAMMER_ATTACK_02 == anim_enum)
					hammer_upper_attack_done = true;
			}

			if(false == hammer_upper_attack_done)
				next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_02;
			else
				next_attack_animation = CPlayer::SORA_ANIM_HAMMER_AIR_LAST_ATTACK;

			break;
		case 4:
			next_attack_animation = CPlayer::SORA_ANIM_HAMMER_AIR_LAST_ATTACK;
			break;
		}
	}

	m_ePrevAttackAnim = next_attack_animation;
	m_AttackAnimOrder.push_back(m_ePrevAttackAnim);

	return next_attack_animation;
}

CPlayer::SORA_ANIM CAction_Assistant::Next_AttackAnimation_Rage(_uint _attackCount)
{
	// 현재 설정된 타겟과 그에 대한 정보를 받아온다
	CCombat_Assistant::ASSIST_TYPE target_type = m_pCombat_Assistant->Get_TargetType();
	CCombat_Assistant::ASSIST_DESC target_desc = m_pCombat_Assistant->Get_Desc(target_type);

	// 체공 상태를 받아온다
	_bool air = m_pPlayerTransformCom->Get_JumpBoolean();

	CPlayer::SORA_ANIM next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_END;
	switch (_attackCount)
	{
	case 0:

		if (false == air)
			next_attack_animation = CPlayer::SORA_ANIM_RAGE_DOUBLE_TAP;
		else
			next_attack_animation = CPlayer::SORA_ANIM_RAGE_ATTACK1;

		break;
	case 1:

		if(CPlayer::SORA_ANIM_RAGE_DOUBLE_TAP == m_ePrevAttackAnim)
			next_attack_animation = CPlayer::SORA_ANIM_RAGE_TRIPLE_TAP;
		else
			next_attack_animation = CPlayer::SORA_ANIM_RAGE_ATTACK2;

		/*if (false == air)
		{
			next_attack_animation = CPlayer::SORA_ANIM_RAGE_TRIPLE_TAP;
		}

		else
		{
			next_attack_animation = CPlayer::SORA_ANIM_RAGE_ATTACK2;
		}*/

		break;
	case 2:
		next_attack_animation = CPlayer::SORA_ANIM_RAGE_ATTACK3;
		break;
	case 3:
		next_attack_animation = CPlayer::SORA_ANIM_RAGE_ATTACK4;
		break;
	}
	
	// 순간이동 로직을 수행하기 위해 타겟을 가져옴
	CCollider* target_collider = m_pCombat_Assistant->Get_TargetCollider();

	// 타겟이 존재할 때만 순간이동
	if (nullptr != target_collider)
	{
		_vector collider_position = target_collider->Get_Center();
		vecy(collider_position) -= m_pPlayer->Calculate_GapY_Between_PositionAndCenter();

		_vector player_position = m_pPlayer->Get_Transform()->Get_Position();

		// 타겟과 플레이어 간의 Y축 거리를 구해둠
		_float distance_y = fabsf(vecy(collider_position) - vecy(player_position));

		// XZ 연산을 하기위해서 Y의 값을 0으로 만듦
		vecy(collider_position) = 0.f;
		vecy(player_position) = 0.f;

		// 타겟과 플레이어 간의 XZ축 거리를 구해둠
		_float distance_xz = DISTANCE(collider_position - player_position);

		// 애니메이션의 XZ 이동량
		_float anim_movement_xz = m_pPlayerModelCom->Get_TargetMovementAmountXZ(next_attack_animation);

		// XZ거리가 XZ 이동량보다 크거나 Y거리가 0.5f보다 클 경우 순간이동 실행
		if (anim_movement_xz < distance_xz ||
			0.5f < distance_y)
			m_pPlayer->Set_Teleport(true);
		else
			m_pPlayer->Set_Teleport(false);
	}

	m_ePrevAttackAnim = next_attack_animation;
	return next_attack_animation;
}

void CAction_Assistant::Clear_AttackAnimOrder()
{
	m_AttackAnimOrder.clear();
}

void CAction_Assistant::Cancle_Form_Normal(CPlayer::SORA_ANIM _current_anim)
{
	switch (_current_anim)
	{
	case Client::CPlayer::SORA_ANIM_AIR_DASH:

		// AIR_DASH 캔슬 타이밍 : 2/3
		if (true == m_pPlayerModelCom->Check_ProgressRate_EachBody(CPlayer::SB_BODY, g_CancleAction_AirDash_ProgressRate))
			m_bAble[ASSIST_CANCLE] = true;
		else
			m_bAble[ASSIST_CANCLE] = false;

		break;
	case Client::CPlayer::SORA_ANIM_GUARD_SUCCEEDED:
	case Client::CPlayer::SORA_ANIM_AIR_GUARD_SUCCEEDED:

		// GUARD_SUCCEEDED 캔슬 타이밍 : 1/3
		if (true == m_pPlayerModelCom->Check_ProgressRate_EachBody(CPlayer::SB_BODY, g_CancleAction_GuardSucceeded_ProgressRate))
			m_bAble[ASSIST_CANCLE] = true;
		else
			m_bAble[ASSIST_CANCLE] = false;

		break;
	default:

		// 일반적인 캔슬 타이밍 : 1/2
		if (true == m_pPlayerModelCom->Check_ProgressRate_EachBody(CPlayer::SB_BODY, g_CancleAction_ProgressRate))
			m_bAble[ASSIST_CANCLE] = true;
		else
			m_bAble[ASSIST_CANCLE] = false;

		break;
	}
}

void CAction_Assistant::Cancle_Form_Hammer(CPlayer::SORA_ANIM _current_anim)
{
	_double time_acc = m_pPlayerModelCom->Get_CurrTimeAcc_EachBody(CPlayer::SB_BODY);

	switch (_current_anim)
	{
	case Client::CPlayer::SORA_ANIM_HAMMER_DASH:

		// DASH 캔슬 타이밍 : 2/3
		if (true == m_pPlayerModelCom->Check_ProgressRate_EachBody(CPlayer::SB_BODY, g_CancleAction_AirDash_ProgressRate))
			m_bAble[ASSIST_CANCLE] = true;
		else
			m_bAble[ASSIST_CANCLE] = false;

		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_GUARD_SUCCEEDED:
	case Client::CPlayer::SORA_ANIM_HAMMER_AIR_GUARD_SUCCEEDED:

		// GUARD_SUCCEEDED 캔슬 타이밍 : 1/3
		if (true == m_pPlayerModelCom->Check_ProgressRate_EachBody(CPlayer::SB_BODY, g_CancleAction_GuardSucceeded_ProgressRate))
			m_bAble[ASSIST_CANCLE] = true;
		else
			m_bAble[ASSIST_CANCLE] = false;

		break;

		// 공격 애니메이션 캔슬 타이밍
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_01:

		if(28.0 <= time_acc)
			m_bAble[ASSIST_CANCLE] = true;
		else
			m_bAble[ASSIST_CANCLE] = false;

		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_02:

		if (25.0 <= time_acc)
			m_bAble[ASSIST_CANCLE] = true;
		else
			m_bAble[ASSIST_CANCLE] = false;

		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_03:

		if (36.0 <= time_acc)
			m_bAble[ASSIST_CANCLE] = true;
		else
			m_bAble[ASSIST_CANCLE] = false;

		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_04:

		if (28.0 <= time_acc)
			m_bAble[ASSIST_CANCLE] = true;
		else
			m_bAble[ASSIST_CANCLE] = false;

		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_05:

		if (43.0 <= time_acc)
			m_bAble[ASSIST_CANCLE] = true;
		else
			m_bAble[ASSIST_CANCLE] = false;

		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_06:

		if (32.0 <= time_acc)
			m_bAble[ASSIST_CANCLE] = true;
		else
			m_bAble[ASSIST_CANCLE] = false;

		break;
	/*case Client::CPlayer::SORA_ANIM_HAMMER_GROUND_LAST_ATTACK:
		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_AIR_LAST_ATTACK:
		break;*/
	}
}

void CAction_Assistant::Cancle_Form_Rage(CPlayer::SORA_ANIM _current_anim)
{
	_double time_acc = m_pPlayerModelCom->Get_CurrTimeAcc_EachBody(CPlayer::SB_BODY);

	switch (_current_anim)
	{
	case Client::CPlayer::SORA_ANIM_RAGE_AIR_DASH:

		// AIR_DASH 캔슬 타이밍 : 2/3
		if (true == m_pPlayerModelCom->Check_ProgressRate_EachBody(CPlayer::SB_BODY, g_CancleAction_AirDash_ProgressRate))
			m_bAble[ASSIST_CANCLE] = true;
		else
			m_bAble[ASSIST_CANCLE] = false;

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_GUARD_SUCCEEDED:

		// GUARD_SUCCEEDED 캔슬 타이밍 : 1/3
		if (true == m_pPlayerModelCom->Check_ProgressRate_EachBody(CPlayer::SB_BODY, g_CancleAction_GuardSucceeded_ProgressRate))
			m_bAble[ASSIST_CANCLE] = true;
		else
			m_bAble[ASSIST_CANCLE] = false;

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_DOUBLE_TAP:

		if (21.0 <= time_acc)
			m_bAble[ASSIST_CANCLE] = true;
		else
			m_bAble[ASSIST_CANCLE] = false;

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_TRIPLE_TAP:

		if (28.0 <= time_acc)
			m_bAble[ASSIST_CANCLE] = true;
		else
			m_bAble[ASSIST_CANCLE] = false;

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_ATTACK1:

		if (33.0 <= time_acc)
			m_bAble[ASSIST_CANCLE] = true;
		else
			m_bAble[ASSIST_CANCLE] = false;

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_ATTACK2:

		if (28.0 <= time_acc)
			m_bAble[ASSIST_CANCLE] = true;
		else
			m_bAble[ASSIST_CANCLE] = false;

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_ATTACK3:

		if (35.0 <= time_acc)
			m_bAble[ASSIST_CANCLE] = true;
		else
			m_bAble[ASSIST_CANCLE] = false;

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_ATTACK4:

		if (true == m_pPlayerModelCom->CheckStop_EachBody(CPlayer::SB_BODY))
			m_bAble[ASSIST_CANCLE] = true;
		else
			m_bAble[ASSIST_CANCLE] = false;

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_ATTACK5:

		if (72.0 <= time_acc)
			m_bAble[ASSIST_CANCLE] = true;
		else
			m_bAble[ASSIST_CANCLE] = false;

		break;
	}
}

void CAction_Assistant::Attack_Form_Normal(CPlayer::SORA_ANIM _current_anim)
{
	if (true == m_bWork[ASSIST_ATTACK])
	{
		// 공격 가능 타이밍 : 1/2
		if (true == m_pPlayerModelCom->Check_ProgressRate_EachBody(CPlayer::SB_BODY, g_AttackNormal_ProgressRate))
			m_bAble[ASSIST_ATTACK] = true;
		else
			m_bAble[ASSIST_ATTACK] = false;
	}
}

void CAction_Assistant::Attack_Form_Hammer(CPlayer::SORA_ANIM _current_anim)
{
	_double time_acc = m_pPlayerModelCom->Get_CurrTimeAcc_EachBody(CPlayer::SB_BODY);

	switch (_current_anim)
	{
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_01:

		if (28.0 <= time_acc)
			m_bAble[ASSIST_ATTACK] = true;
		else
			m_bAble[ASSIST_ATTACK] = false;

		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_02:

		if (25.0 <= time_acc)
			m_bAble[ASSIST_ATTACK] = true;
		else
			m_bAble[ASSIST_ATTACK] = false;

		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_03:

		if (36.0 <= time_acc)
			m_bAble[ASSIST_ATTACK] = true;
		else
			m_bAble[ASSIST_ATTACK] = false;

		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_04:

		if (28.0 <= time_acc)
			m_bAble[ASSIST_ATTACK] = true;
		else
			m_bAble[ASSIST_ATTACK] = false;

		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_05:

		if (43.0 <= time_acc)
			m_bAble[ASSIST_ATTACK] = true;
		else
			m_bAble[ASSIST_ATTACK] = false;

		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_06:

		if (32.0 <= time_acc)
			m_bAble[ASSIST_ATTACK] = true;
		else
			m_bAble[ASSIST_ATTACK] = false;

		break;
	/*case Client::CPlayer::SORA_ANIM_HAMMER_GROUND_LAST_ATTACK:
		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_AIR_LAST_ATTACK:
		break;*/
	}
}

void CAction_Assistant::Attack_Form_Rage(CPlayer::SORA_ANIM _current_anim)
{
	_double time_acc = m_pPlayerModelCom->Get_CurrTimeAcc_EachBody(CPlayer::SB_BODY);

	switch (_current_anim)
	{
	case Client::CPlayer::SORA_ANIM_RAGE_DOUBLE_TAP:

		if (21.0 <= time_acc)
			m_bAble[ASSIST_ATTACK] = true;
		else
			m_bAble[ASSIST_ATTACK] = false;

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_TRIPLE_TAP:

		if (28.0 <= time_acc)
			m_bAble[ASSIST_ATTACK] = true;
		else
			m_bAble[ASSIST_ATTACK] = false;

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_ATTACK1:

		if (33.0 <= time_acc)
			m_bAble[ASSIST_ATTACK] = true;
		else
			m_bAble[ASSIST_ATTACK] = false;

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_ATTACK2:

		if (28.0 <= time_acc)
			m_bAble[ASSIST_ATTACK] = true;
		else
			m_bAble[ASSIST_ATTACK] = false;

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_ATTACK3:

		if (35.0 <= time_acc)
			m_bAble[ASSIST_ATTACK] = true;
		else
			m_bAble[ASSIST_ATTACK] = false;

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_ATTACK4:

		if (true == m_pPlayerModelCom->CheckStop_EachBody(CPlayer::SB_BODY))
			m_bAble[ASSIST_ATTACK] = true;
		else
			m_bAble[ASSIST_ATTACK] = false;

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_ATTACK5:

		if (72.0 <= time_acc)
			m_bAble[ASSIST_ATTACK] = true;
		else
			m_bAble[ASSIST_ATTACK] = false;

		break;
	}
}

void CAction_Assistant::Gravity_Form_Normal(CPlayer::SORA_ANIM _current_anim)
{
	if (true == m_pPlayerModelCom->Check_ProgressRate_EachBody(CPlayer::SB_BODY, g_GravityDefiance_ProgressRate))
	{
		if (false == m_pPlayer->Get_Transform()->Get_GravityEffect())
		{
			m_bAble[ASSIST_GRAVITY] = true;
			m_pPlayerTransformCom->BasicJump(CTransform::JUMP_FALL);
		}
	}
	else
	{
		m_bAble[ASSIST_GRAVITY] = false;
		m_pPlayerTransformCom->Set_GravityEffect(false);
	}
}

void CAction_Assistant::Gravity_Form_Hammer(CPlayer::SORA_ANIM _current_anim)
{
	_double time_acc = m_pPlayerModelCom->Get_CurrTimeAcc_EachBody(CPlayer::SB_BODY);

	switch (_current_anim)
	{
	/*case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_01:
		break;*/
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_02:

		if (21.0 <= time_acc)
		{
			if (false == m_pPlayer->Get_Transform()->Get_GravityEffect())
			{
				m_bAble[ASSIST_GRAVITY] = true;
				m_pPlayerTransformCom->BasicJump(CTransform::JUMP_FALL);
			}
		}
		else
		{
			if (false == m_pPlayerModelCom->CheckStop_EachBody(CPlayer::SB_BODY))
			{
				m_bAble[ASSIST_GRAVITY] = false;
				m_pPlayerTransformCom->Set_GravityEffect(false);
			}
		}

		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_03:

		if (40.0 <= time_acc)
		{
			if (false == m_pPlayer->Get_Transform()->Get_GravityEffect())
			{
				m_bAble[ASSIST_GRAVITY] = true;
				m_pPlayerTransformCom->BasicJump(CTransform::JUMP_FALL);
			}
		}
		else
		{
			if (false == m_pPlayerModelCom->CheckStop_EachBody(CPlayer::SB_BODY))
			{
				m_bAble[ASSIST_GRAVITY] = false;
				m_pPlayerTransformCom->Set_GravityEffect(false);
			}
		}

		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_04:

		if (29.0 <= time_acc)
		{
			if (false == m_pPlayer->Get_Transform()->Get_GravityEffect())
			{
				m_bAble[ASSIST_GRAVITY] = true;
				m_pPlayerTransformCom->BasicJump(CTransform::JUMP_FALL);
			}
		}
		else
		{
			if (false == m_pPlayerModelCom->CheckStop_EachBody(CPlayer::SB_BODY))
			{
				m_bAble[ASSIST_GRAVITY] = false;
				m_pPlayerTransformCom->Set_GravityEffect(false);
			}
		}

		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_AIR_LAST_ATTACK:

		if (46.0 <= time_acc)
		{
			if (false == m_pPlayer->Get_Transform()->Get_GravityEffect())
			{
				m_bAble[ASSIST_GRAVITY] = true;
				m_pPlayerTransformCom->BasicJump(CTransform::JUMP_FALL);
			}
		}
		else
		{
			if (false == m_pPlayerModelCom->CheckStop_EachBody(CPlayer::SB_BODY))
			{
				m_bAble[ASSIST_GRAVITY] = false;
				m_pPlayerTransformCom->Set_GravityEffect(false);
			}
		}

		break;
	default:

		if (true == m_pPlayerModelCom->Check_ProgressRate_EachBody(CPlayer::SB_BODY, g_GravityDefiance_ProgressRate))
		{
			if (false == m_pPlayer->Get_Transform()->Get_GravityEffect())
			{
				m_bAble[ASSIST_GRAVITY] = true;
				m_pPlayerTransformCom->BasicJump(CTransform::JUMP_FALL);
			}
		}
		else
		{
			m_bAble[ASSIST_GRAVITY] = false;
			m_pPlayerTransformCom->Set_GravityEffect(false);
		}

		break;
	}
}

void CAction_Assistant::Gravity_Form_Rage(CPlayer::SORA_ANIM _current_anim)
{
	_double time_acc = m_pPlayerModelCom->Get_CurrTimeAcc_EachBody(CPlayer::SB_BODY);

	switch (_current_anim)
	{
	case Client::CPlayer::SORA_ANIM_RAGE_DOUBLE_TAP:

		if (21.0 <= time_acc)
		{
			// else 문을 활용하기 위해서 안 쪽에서 조건검사
			if (false == m_pPlayer->Get_Transform()->Get_GravityEffect())
			{
				m_bAble[ASSIST_GRAVITY] = true;
				m_pPlayerTransformCom->BasicJump(CTransform::JUMP_FALL);
			}
		}
		else
		{
			if (false == m_pPlayerModelCom->CheckStop_EachBody(CPlayer::SB_BODY))
			{
				m_bAble[ASSIST_GRAVITY] = false;
				m_pPlayerTransformCom->Set_GravityEffect(false);
			}
		}

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_TRIPLE_TAP:

		if (28.0 <= time_acc)
		{
			if (false == m_pPlayer->Get_Transform()->Get_GravityEffect())
			{
				m_bAble[ASSIST_GRAVITY] = true;
				m_pPlayerTransformCom->BasicJump(CTransform::JUMP_FALL);
			}
		}
		else
		{
			if (false == m_pPlayerModelCom->CheckStop_EachBody(CPlayer::SB_BODY))
			{
				m_bAble[ASSIST_GRAVITY] = false;
				m_pPlayerTransformCom->Set_GravityEffect(false);
			}
		}

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_ATTACK1:

		if (33.0 <= time_acc)
		{
			if (false == m_pPlayer->Get_Transform()->Get_GravityEffect())
			{
				m_bAble[ASSIST_GRAVITY] = true;
				m_pPlayerTransformCom->BasicJump(CTransform::JUMP_FALL);
			}
		}
		else
		{
			if (false == m_pPlayerModelCom->CheckStop_EachBody(CPlayer::SB_BODY))
			{
				m_bAble[ASSIST_GRAVITY] = false;
				m_pPlayerTransformCom->Set_GravityEffect(false);
			}
		}

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_ATTACK2:

		if (28.0 <= time_acc)
		{
			if (false == m_pPlayer->Get_Transform()->Get_GravityEffect())
			{
				m_bAble[ASSIST_GRAVITY] = true;
				m_pPlayerTransformCom->BasicJump(CTransform::JUMP_FALL);
			}
		}
		else
		{
			if (false == m_pPlayerModelCom->CheckStop_EachBody(CPlayer::SB_BODY))
			{
				m_bAble[ASSIST_GRAVITY] = false;
				m_pPlayerTransformCom->Set_GravityEffect(false);
			}
		}

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_ATTACK3:

		if (35.0 <= time_acc)
		{
			if (false == m_pPlayer->Get_Transform()->Get_GravityEffect())
			{
				m_bAble[ASSIST_GRAVITY] = true;
				m_pPlayerTransformCom->BasicJump(CTransform::JUMP_FALL);
			}
		}
		else
		{
			if (false == m_pPlayerModelCom->CheckStop_EachBody(CPlayer::SB_BODY))
			{
				m_bAble[ASSIST_GRAVITY] = false;
				m_pPlayerTransformCom->Set_GravityEffect(false);
			}
		}

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_ATTACK4:

		if (true == m_pPlayerModelCom->CheckStop_EachBody(CPlayer::SB_BODY))
		{
			if (false == m_pPlayer->Get_Transform()->Get_GravityEffect())
			{
				m_bAble[ASSIST_GRAVITY] = true;
				m_pPlayerTransformCom->BasicJump(CTransform::JUMP_FALL);
			}
		}
		else
		{
			if (false == m_pPlayerModelCom->CheckStop_EachBody(CPlayer::SB_BODY))
			{
				m_bAble[ASSIST_GRAVITY] = false;
				m_pPlayerTransformCom->Set_GravityEffect(false);
			}
		}

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_ATTACK5:

		if (72.0 <= time_acc)
		{
			if (false == m_pPlayer->Get_Transform()->Get_GravityEffect())
			{
				m_bAble[ASSIST_GRAVITY] = true;
				m_pPlayerTransformCom->BasicJump(CTransform::JUMP_FALL);
			}
		}
		else
		{
			if (false == m_pPlayerModelCom->CheckStop_EachBody(CPlayer::SB_BODY))
			{
				m_bAble[ASSIST_GRAVITY] = false;
				m_pPlayerTransformCom->Set_GravityEffect(false);
			}
		}

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_FINISH:

		if (true == m_pPlayerModelCom->CheckStop_EachBody(CPlayer::SB_BODY))
		{
			if (false == m_pPlayer->Get_Transform()->Get_GravityEffect())
			{
				m_bAble[ASSIST_GRAVITY] = true;
				m_pPlayerTransformCom->BasicJump(CTransform::JUMP_FALL);
			}
		}
		else
		{
			if (false == m_pPlayerModelCom->CheckStop_EachBody(CPlayer::SB_BODY))
			{
				m_bAble[ASSIST_GRAVITY] = false;
				m_pPlayerTransformCom->Set_GravityEffect(false);
			}
		}

		break;
	default:

		if (true == m_pPlayerModelCom->Check_ProgressRate_EachBody(CPlayer::SB_BODY, g_GravityDefiance_ProgressRate))
		{
			if (false == m_pPlayer->Get_Transform()->Get_GravityEffect())
			{
				m_bAble[ASSIST_GRAVITY] = true;
				m_pPlayerTransformCom->BasicJump(CTransform::JUMP_FALL);
			}
		}
		else
		{
			m_bAble[ASSIST_GRAVITY] = false;
			m_pPlayerTransformCom->Set_GravityEffect(false);
		}

		break;
	}
}

CAction_Assistant * CAction_Assistant::Create(CGameObject* _player)
{
	CAction_Assistant*	pInstance = new CAction_Assistant();

	if (FAILED(pInstance->Initialize(_player)))
	{
		MSGBOX("Failed to Created CAction_Assistant");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAction_Assistant::Free()
{
}
