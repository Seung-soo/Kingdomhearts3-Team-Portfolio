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

			// �۵� ���� �ƴ϶�� break
			if (false == m_bWork[ASSIST_CANCLE])
			{
				m_bAble[ASSIST_CANCLE] = true;
				break;
			}

			// �� ���¿� ���� ������Ʈ
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

			// �۵� ���� �ƴ϶�� break
			if (false == m_bWork[ASSIST_ATTACK])
			{
				m_bAble[ASSIST_ATTACK] = true;
				break;
			}

			// �� ���¿� ���� ������Ʈ
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

			// �۵� ���� �ƴ϶�� break
			if (false == m_bWork[ASSIST_MOVE])
			{
				m_bAble[ASSIST_MOVE] = true;
				break;
			}

			// �ִϸ��̼ǿ� ���� ������Ʈ
			// Ÿ�̹��� �ִϸ��̼Ǹ��� �ٸ��� �� Ÿ�Կ� ������ �ִϸ��̼� ��ü�� ����
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

			// �۵� ���� �ƴ϶�� break
			if (false == m_bWork[ASSIST_GRAVITY])
			{
				m_bAble[ASSIST_GRAVITY] = true;
				break;
			}

			// ���� ���°� �ƴ϶�� break
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

			// ���� �ݺ��� Ż��
			_types->push_back((ASSIST_TYPE)i);
			break;
		}
	}
}

CPlayer::SORA_ANIM CAction_Assistant::Dice_NextAttack(_uint _attackCount)
{
	/*
		�� �Լ��� AttackState �ȿ��� ���� ���� �ִϸ��̼��� ������ �� ȣ���Ѵ�
		����ؾ��� �׸�
		1. ����, ����
		2. ����
		3. ����
		4. ���� ����
		5. ���° ��������
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
	// ���� ������ Ÿ�ٰ� �׿� ���� ������ �޾ƿ´�. Ÿ���� ���� ��� target_desc�� ZeroMemory�� �� ���·� ��ȯ�ȴ�
	CCombat_Assistant::ASSIST_TYPE target_type = m_pCombat_Assistant->Get_TargetType();
	CCombat_Assistant::ASSIST_DESC target_desc = m_pCombat_Assistant->Get_Desc(target_type);

	// ü�� ���¸� �޾ƿ´�
	_bool air = m_pPlayerTransformCom->Get_JumpBoolean();

	CPlayer::SORA_ANIM next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_END;
	switch (_attackCount)
	{
	case 0:

		// ���� ���� ��
		if (false == air)
		{
			// ���� ��
			if (1.5f < target_desc.fDistanceY)
			{
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_UPPER_ATTACK1;
			}
			// �Ϲ� ������ ��
			else
			{
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_GROUND_ATTACK1;
			}
		}

		// ���߿� ���� ��
		else
		{
			// ���� ��
			if (1.5f < target_desc.fDistanceY)
			{
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_UPPER_ATTACK1;
			}
			// ���� ��
			else if (-0.75f > target_desc.fDistanceY)
			{
				// ��� ��ǰ��� XZ �Ÿ��� ���� �پ��� ������� ��ǵ� ������ ���� ��.
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_LOWER_ATTACK1;
			}
			// �Ϲ� ������ ��
			else
			{
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_AIR_ATTACK1;
			}
		}

		break;
	case 1:

		// ���� ���� ��
		if (false == air)
		{
			next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_GROUND_ATTACK2;
		}
		// ���߿� ���� ��
		else
		{
			// ���� ��
			if (1.5f < target_desc.fDistanceY)
			{
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_UPPER_ATTACK2;
			}
			// ���� ��
			else if (-0.75f > target_desc.fDistanceY)
			{
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_LOWER_ATTACK2;
			}
			// �Ϲ� ������ ��
			else
			{
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_AIR_ATTACK2;
			}
		}

		break;
	case 2:

		// ���� ���� ��
		if (false == air)
		{
			next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_GROUND_ATTACK3;
		}
		// ���߿� ���� ��
		else
		{
			// ���� ��
			if (1.5f < target_desc.fDistanceY)
			{
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_UPPER_ATTACK3;
			}
			// ���� ��
			else if (-0.75f > target_desc.fDistanceY)
			{
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_LOWER_ATTACK3;
			}
			// �Ϲ� ������ ��
			else
			{
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_AIR_ATTACK3;
			}
		}

		break;
	case 3:

		// ���� ���� ��
		if (false == air)
		{
			next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_GROUND_ATTACK4;
		}
		// ���߿� ���� ��
		else
		{
			// ���� ��
			if (1.5f < target_desc.fDistanceY)
			{
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_UPPER_ATTACK4;
			}
			// ���� ��
			else if (-0.75f > target_desc.fDistanceY)
			{
				next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_LOWER_ATTACK4;
			}
			// �Ϲ� ������ ��
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
	// ���� ������ Ÿ�ٰ� �׿� ���� ������ �޾ƿ´�
	CCombat_Assistant::ASSIST_TYPE target_type = m_pCombat_Assistant->Get_TargetType();
	CCombat_Assistant::ASSIST_DESC target_desc = m_pCombat_Assistant->Get_Desc(target_type);

	// ü�� ���¸� �޾ƿ´�
	_bool air = m_pPlayerTransformCom->Get_JumpBoolean();

	// ��ó ���� ���� �޾ƿ´�
	_uint enemy_number = m_pCombat_Assistant->EnemyNumber_In_Range(g_Range_Search_Enemy);

	// ���� �ִϸ��̼��� �޾Ƶ� ������ �̸� ����
	CPlayer::SORA_ANIM next_attack_animation = CPlayer::SORA_ANIM::SORA_ANIM_END;

	// �ִϸ��̼� Ȯ���ϱ� ���� ���� switch�� �ۿ��� �̸� ������
	_bool hammer_upper_attack_done = false;

	// ���� ���� ���
	if (false == air)
	{
		switch (_attackCount)
		{
		case 0:

			// ��ó�� ���� ���� ���
			if (g_Conditional_Figures_To_Change_Attack_Animation_On_HammerForm <= enemy_number)
				next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_03;

			// ��ó�� ���� ���� ���
			else
			{
				// SORA_ANIM_HAMMER_ATTACK_01 �� XZ �̵����� �޾ƿ´�
				_float movement_xz_attack1 = m_pPlayerModelCom->Get_TargetMovementAmountXZ(CPlayer::SORA_ANIM_HAMMER_ATTACK_01);

				// Ÿ���� ���� ��� �⺻ �������� ����
				if(nullptr == target_desc.pCollider)
					next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_01;

				// Ÿ���� ���� ��� ���ǿ� �´� �������� ����
				else
				{
					// Ÿ�ٰ��� �Ÿ��� SORA_ANIM_HAMMER_ATTACK_01 �� XZ �̵������� ���� ���
					if (target_desc.fDistanceXZ < movement_xz_attack1)
						next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_01;

					// Ÿ�ٰ��� �Ÿ��� SORA_ANIM_HAMMER_ATTACK_01 �� XZ �̵������� ū ���
					else
						next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_02;
				}
			}

			break;
		case 1:

			// ���� ���� �ִϸ��̼��� Ȯ���Ͽ� ���� �ִϸ��̼��� ����
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

			// ���� ���� �ִϸ��̼��� Ȯ���Ͽ� ���� �ִϸ��̼��� ����
			switch (m_ePrevAttackAnim)
			{
			case CPlayer::SORA_ANIM_HAMMER_ATTACK_01:

				// �������ظ��� �÷�ġ�� ���� ( SORA_ANIM_HAMMER_ATTACK_02 ) �� �ߵ��� �� �ִ��� Ȯ��
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

			// ���� ���� �ִϸ��̼��� Ȯ���Ͽ� ���� �ִϸ��̼��� ����
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

	// ���߿� ���� ���
	else
	{
		switch (_attackCount)
		{
		case 0:

		{
			// SORA_ANIM_HAMMER_ATTACK_04 �� XZ �̵����� �޾ƿ´�
			_float movement_xz_attack4 = m_pPlayerModelCom->Get_TargetMovementAmountXZ(CPlayer::SORA_ANIM_HAMMER_ATTACK_04);

			// Ÿ���� ���� ��� �⺻ �������� ����
			if (nullptr == target_desc.pCollider)
				next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_04;

			// Ÿ���� ���� ��� ���ǿ� �´� �������� ����
			else
			{
				// Ÿ�ٰ��� �Ÿ��� SORA_ANIM_HAMMER_ATTACK_04 �� XZ �̵������� ���� ���
				if (target_desc.fDistanceXZ < movement_xz_attack4)
					next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_03;

				// Ÿ�ٰ��� �Ÿ��� SORA_ANIM_HAMMER_ATTACK_04 �� XZ �̵������� ū ���
				else
					next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_04;
			}
		}
			

			//// Ÿ���� ������ Y������ �÷��̾��� ������ Y���� �� ������ fDistanceY �� ������ ��ϵȴ�

			//// Ÿ���� ���̿� �÷��̾��� ���� ���̰� ����ġ���� ū ��� ( �÷��̾ �� ���� �ִٴ� ���� �Ͽ� )
			//if (nullptr != target_desc.pCollider &&
			//	-1.5f >= target_desc.fDistanceY)
			//	next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_06;
			//
			//// Ÿ�ٰ� �÷��̾��� ���� ���̰� ũ�� �ʰų� Ÿ���� ���̰� �� ���� ���
			//else
			//{
			//	
			//}

			break;
		case 1:

			// ���� ���� �ִϸ��̼��� Ȯ���Ͽ� ���� �ִϸ��̼��� ����
			switch (m_ePrevAttackAnim)
			{
				// ���� ���ݿ��� �������� �Ѿ�� ���
			case CPlayer::SORA_ANIM_HAMMER_ATTACK_02:

			{
				// SORA_ANIM_HAMMER_ATTACK_04 �� XZ �̵����� �޾ƿ´�
				_float movement_xz_attack4 = m_pPlayerModelCom->Get_TargetMovementAmountXZ(CPlayer::SORA_ANIM_HAMMER_ATTACK_04);

				// Ÿ���� ���� ��� �⺻ �������� ����
				if (nullptr == target_desc.pCollider)
					next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_04;

				// Ÿ���� ���� ��� ���ǿ� �´� �������� ����
				else
				{
					// Ÿ�ٰ��� �Ÿ��� SORA_ANIM_HAMMER_ATTACK_04 �� XZ �̵������� ���� ���
					if (target_desc.fDistanceXZ < movement_xz_attack4)
						next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_03;

					// Ÿ�ٰ��� �Ÿ��� SORA_ANIM_HAMMER_ATTACK_04 �� XZ �̵������� ū ���
					else
						next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_04;
				}
			}

				//// Ÿ���� ���̿� �÷��̾��� ���� ���̰� ����ġ���� ū ��� ( �÷��̾ �� ���� �ִٴ� ���� �Ͽ� )
				//if (nullptr != target_desc.pCollider &&
				//	-1.5f >= target_desc.fDistanceY)
				//	next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_06;

				//// Ÿ�ٰ� �÷��̾��� ���� ���̰� ũ�� �ʰų� Ÿ���� ���̰� �� ���� ���
				//else
				//{
				//	
				//}

				break;

				// ���� �������� �����ؼ� ���߿� �ӹ����� �ִ� ���
			case CPlayer::SORA_ANIM_HAMMER_ATTACK_03:
				next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_04;
				break;
			case CPlayer::SORA_ANIM_HAMMER_ATTACK_04:
				next_attack_animation = CPlayer::SORA_ANIM_HAMMER_ATTACK_03;
				break;
			}

			break;
		case 2:

			// ���� ���� �ִϸ��̼��� Ȯ���Ͽ� ���� �ִϸ��̼��� ����
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

			// �̹� SORA_ANIM_HAMMER_ATTACK_02 �� �ߵ��� ���� �ִٸ� SORA_ANIM_HAMMER_AIR_LAST_ATTACK �� �ٷ� �Ѿ��

			// �������ظ��� �÷�ġ�� ���� ( SORA_ANIM_HAMMER_ATTACK_02 ) �� �ߵ��� �� �ִ��� Ȯ��
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
	// ���� ������ Ÿ�ٰ� �׿� ���� ������ �޾ƿ´�
	CCombat_Assistant::ASSIST_TYPE target_type = m_pCombat_Assistant->Get_TargetType();
	CCombat_Assistant::ASSIST_DESC target_desc = m_pCombat_Assistant->Get_Desc(target_type);

	// ü�� ���¸� �޾ƿ´�
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
	
	// �����̵� ������ �����ϱ� ���� Ÿ���� ������
	CCollider* target_collider = m_pCombat_Assistant->Get_TargetCollider();

	// Ÿ���� ������ ���� �����̵�
	if (nullptr != target_collider)
	{
		_vector collider_position = target_collider->Get_Center();
		vecy(collider_position) -= m_pPlayer->Calculate_GapY_Between_PositionAndCenter();

		_vector player_position = m_pPlayer->Get_Transform()->Get_Position();

		// Ÿ�ٰ� �÷��̾� ���� Y�� �Ÿ��� ���ص�
		_float distance_y = fabsf(vecy(collider_position) - vecy(player_position));

		// XZ ������ �ϱ����ؼ� Y�� ���� 0���� ����
		vecy(collider_position) = 0.f;
		vecy(player_position) = 0.f;

		// Ÿ�ٰ� �÷��̾� ���� XZ�� �Ÿ��� ���ص�
		_float distance_xz = DISTANCE(collider_position - player_position);

		// �ִϸ��̼��� XZ �̵���
		_float anim_movement_xz = m_pPlayerModelCom->Get_TargetMovementAmountXZ(next_attack_animation);

		// XZ�Ÿ��� XZ �̵������� ũ�ų� Y�Ÿ��� 0.5f���� Ŭ ��� �����̵� ����
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

		// AIR_DASH ĵ�� Ÿ�̹� : 2/3
		if (true == m_pPlayerModelCom->Check_ProgressRate_EachBody(CPlayer::SB_BODY, g_CancleAction_AirDash_ProgressRate))
			m_bAble[ASSIST_CANCLE] = true;
		else
			m_bAble[ASSIST_CANCLE] = false;

		break;
	case Client::CPlayer::SORA_ANIM_GUARD_SUCCEEDED:
	case Client::CPlayer::SORA_ANIM_AIR_GUARD_SUCCEEDED:

		// GUARD_SUCCEEDED ĵ�� Ÿ�̹� : 1/3
		if (true == m_pPlayerModelCom->Check_ProgressRate_EachBody(CPlayer::SB_BODY, g_CancleAction_GuardSucceeded_ProgressRate))
			m_bAble[ASSIST_CANCLE] = true;
		else
			m_bAble[ASSIST_CANCLE] = false;

		break;
	default:

		// �Ϲ����� ĵ�� Ÿ�̹� : 1/2
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

		// DASH ĵ�� Ÿ�̹� : 2/3
		if (true == m_pPlayerModelCom->Check_ProgressRate_EachBody(CPlayer::SB_BODY, g_CancleAction_AirDash_ProgressRate))
			m_bAble[ASSIST_CANCLE] = true;
		else
			m_bAble[ASSIST_CANCLE] = false;

		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_GUARD_SUCCEEDED:
	case Client::CPlayer::SORA_ANIM_HAMMER_AIR_GUARD_SUCCEEDED:

		// GUARD_SUCCEEDED ĵ�� Ÿ�̹� : 1/3
		if (true == m_pPlayerModelCom->Check_ProgressRate_EachBody(CPlayer::SB_BODY, g_CancleAction_GuardSucceeded_ProgressRate))
			m_bAble[ASSIST_CANCLE] = true;
		else
			m_bAble[ASSIST_CANCLE] = false;

		break;

		// ���� �ִϸ��̼� ĵ�� Ÿ�̹�
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

		// AIR_DASH ĵ�� Ÿ�̹� : 2/3
		if (true == m_pPlayerModelCom->Check_ProgressRate_EachBody(CPlayer::SB_BODY, g_CancleAction_AirDash_ProgressRate))
			m_bAble[ASSIST_CANCLE] = true;
		else
			m_bAble[ASSIST_CANCLE] = false;

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_GUARD_SUCCEEDED:

		// GUARD_SUCCEEDED ĵ�� Ÿ�̹� : 1/3
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
		// ���� ���� Ÿ�̹� : 1/2
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
			// else ���� Ȱ���ϱ� ���ؼ� �� �ʿ��� ���ǰ˻�
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
