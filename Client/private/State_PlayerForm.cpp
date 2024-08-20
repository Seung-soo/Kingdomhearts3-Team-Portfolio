#include "stdafx.h"
#include "..\public\State_PlayerForm.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Effect_Manager.h"

#include "StateMachine.h"
#include "State_Jump.h"

#include "Camera_Perspective.h"

#include "Player_Weapon.h"
#include "Monster.h"
#include "Action_Assistant.h"

_double CState_PlayerForm::g_Form_Duration = 30.0;

HRESULT CState_PlayerForm::Initialize(void * pArg)
{
	for (_uint i = 0; i < RAGE_EFFECT_END; ++i)
	{
		// ����Ʈ ���� ���� Ȯ���ϴ� Bool �ʱ�ȭ
		m_bRageForm_Effect_Created[i] = false;

		// ����Ʈ ���� Ÿ�̹� ����
		switch (RAGE_FORM_EFFECT(i))
		{
		case Client::CState_PlayerForm::RAGE_EFFECT_PEARL:
			m_dRageForm_Effect_CreateTime[i] = 10.0;
			break;
		case Client::CState_PlayerForm::RAGE_EFFECT_RIPPLE:
			m_dRageForm_Effect_CreateTime[i] = 12.0;
			break;
		case Client::CState_PlayerForm::RAGE_EFFECT_MARBLE:
			m_dRageForm_Effect_CreateTime[i] = 14.0;
			break;
		case Client::CState_PlayerForm::RAGE_EFFECT_PARTICLE:
			m_dRageForm_Effect_CreateTime[i] = 16.0;
			break;
		case Client::CState_PlayerForm::RAGE_EFFECT_ROAR:
			m_dRageForm_Effect_CreateTime[i] = 56.0;
			break;
		}
	}

	for (_uint i = 0; i < 4; ++i)
		m_bRageForm_Sound_Played[i] = false;

	return S_OK;
}

_bool CState_PlayerForm::KeyInput(_double TimeDelta)
{
	// ������ �� ĵ�� LShift + G
	CGameInstance* pGI = CGameInstance::GetInstance();
	if (pGI->Get_KeyPressing(DIK_LSHIFT))
	{
		if (pGI->Get_KeyEnter(DIK_H))
			return false;
	}

	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
	_bool able_to_finish = m_pPlayer->Get_AbleToFinish();
	switch (curr_form)
	{
	case Client::CPlayer::SF_HAMMER:

		//if (pGI->Get_KeyEnter(DIK_F))
		//{
		//	if (false == able_to_finish)
		//		return true;

		//	// ���� ���� ����
		//	m_pPlayer->Create_PlayerState(CStateMachine::STATETYPE_ATK);

		//	// �ִϸ��̼� ����
		//	m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_HAMMER_FINISH);
		//	m_pPlayer->Set_AbleToFinish(false);

		//	// ī�޶� ���� true -> ��������

		//	// �����ð� ����
		//	m_pPlayer->Invincibility_Input(
		//		m_pModelCom->Get_TargetDuration(CPlayer::SORA_ANIM_HAMMER_FINISH),
		//		m_pModelCom->Get_TargetPlaySpeed(CPlayer::SORA_ANIM_HAMMER_FINISH));

		//	// ���� ���ǵ� ����
		//	list<CMonster*>* monster_list = GM->Get_MonstersList();
		//	for (auto pMonster : *monster_list)
		//	{
		//		CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), pMonster->Get_LayerTag(), 0.1);
		//		CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_MonsterSkill"), 0.1);
		//	}
		//}

		//	// UI ���� ���
		//	m_pPlayer->Play_FormChangeUI();


		break;
	case Client::CPlayer::SF_RAGE:

		if (pGI->Get_KeyEnter(DIK_F))
		{
			if (false == able_to_finish)
				return true;

			// ���� ���� ����
			m_pPlayer->Create_PlayerState(CStateMachine::STATETYPE_ATK);

			// �ִϸ��̼� ����
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_FINISH);
			m_pPlayer->Set_AbleToFinish(false);

			// ī�޶� ���� true
			GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_RAGE_FINISHATTACK, m_pTransformCom);

			// �����ð� ����
			m_pPlayer->Invincibility_Input(
				m_pModelCom->Get_TargetDuration(CPlayer::SORA_ANIM_RAGE_FINISH),
				m_pModelCom->Get_TargetPlaySpeed(CPlayer::SORA_ANIM_RAGE_FINISH));

			// ���� ���ǵ� ����
			list<CMonster*>* monster_list = GM->Get_MonstersList();
			for (auto pMonster : *monster_list)
			{
				CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), pMonster->Get_LayerTag(), 0.1);
				CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_MonsterSkill"), 0.1);
			}

			// UI ���� ���
			m_pPlayer->Play_FormChangeUI();
		}

		break;
	}

	return true;
}

_bool CState_PlayerForm::Action(_double TimeDelta)
{
	_bool input = KeyInput(TimeDelta);

	if (false == input)
	{
		ExitState();
		return DELETE_STATE;
	}

	// �� ���� ���� �˻�
	if (false == m_pPlayer->Check_PlayerState(CStateMachine::STATETYPE_GUARD) &&
		false == m_pPlayer->Check_PlayerState(CStateMachine::STATETYPE_AVOID) &&
		false == m_pPlayer->Check_PlayerState(CStateMachine::STATETYPE_ATK))
	{
		if (true == m_bFormEnd)
		{
			ExitState();
			return DELETE_STATE;
		}
	}

	// �� ���� ����
	CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);

	_bool anim_done = m_pModelCom->CheckStop_EachBody(CPlayer::SB_BODY);
	_bool air_state = m_pTransformCom->Get_JumpBoolean();

	_double time_acc = m_pModelCom->Get_TargetTimeAcc(curr_anim);
	_bool hovering = m_pPlayer->Get_Hovering();

	switch (curr_anim)
	{
	case Client::CPlayer::SORA_ANIM_RAGE_FORM:

		// �ִϸ��̼��� ����Ǿ��ٸ�
		if (true == anim_done)
		{
			// ����
			if (false == air_state)
			{
				if (false == hovering)
				{
					m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_FALL_END);
					m_pTransformCom->Set_GravityEffect(true);
				}
				else
				{
					m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_AIR_IDLE);
					m_pTransformCom->Set_GravityEffect(true);
				}
			}

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

			// ���� ���ǵ� ���󺹱�
			list<CMonster*>* monster_list = GM->Get_MonstersList();
			for (auto pMonster : *monster_list)
			{
				CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), pMonster->Get_LayerTag(), 1.0);
				CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_MonsterSkill"), 1.0);
			}
		}
		
		// ������ �� ����Ʈ ����
		Create_RageForm_Effects();
		
		// ������ �� ���� ���
		Play_RageForm_Sounds();

		{
			_uint time_acc_in_uint = (_uint)m_pModelCom->Get_TargetTimeAcc(CPlayer::SORA_ANIM_RAGE_FORM);
			if (false == m_pPlayer->Get_FormTexture() && 30 <= time_acc_in_uint)
				m_pPlayer->Set_FormTexture(true);
		}

		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_FORM:

		// �ִϸ��̼��� ����Ǿ��ٸ�
		if (true == anim_done)
		{
			// ����
			if (false == air_state)
			{
				if (false == hovering)
				{
					m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_HAMMER_FALL_END);
					m_pTransformCom->Set_GravityEffect(true);
				}
				else
				{
					m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_AIR_IDLE);
					m_pTransformCom->Set_GravityEffect(true);
				}
			}

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
			
			m_pPlayer_Weapon->WeaponType(CPlayer_Weapon::WEAPON_TYPE_HYPER_HAMMER);

			// ���� ���ǵ� ���󺹱�
			list<CMonster*>* monster_list = GM->Get_MonstersList();
			for (auto pMonster : *monster_list)
			{
				CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), pMonster->Get_LayerTag(), 1.0);
				CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_MonsterSkill"), 1.0);
			}
		}

		// �ظ� �� ���� ���
		Play_HammerForm_Sounds();

		// �ظ� �� ���� ����Ʈ ����
		Create_HammerForm_ChangingEffect();

		{
			_uint time_acc_in_uint = (_uint)m_pModelCom->Get_TargetTimeAcc(CPlayer::SORA_ANIM_HAMMER_FORM);
			if (false == m_pPlayer->Get_FormTexture() && 90 <= time_acc_in_uint)
				m_pPlayer->Set_FormTexture(true);
		}

		break;
	}

	if (CPlayer::SORA_ANIM_HAMMER_FORM == curr_anim ||
		CPlayer::SORA_ANIM_RAGE_FORM == curr_anim)
	{
		_bool able_to_finish = m_pPlayer->Get_AbleToFinish();
		if (true == anim_done && false == able_to_finish)
		{
			// �ǴϽ� ���� ���� ����
			m_pPlayer->Set_AbleToFinish(true);
		}
	}

	// Ÿ�̸� üũ �� �� ���� ����
	m_dFormTimer -= TimeDelta;
	if (0.0 >= m_dFormTimer)
	{
		m_bFormEnd = true;
		m_dFormTimer = 0.0;
	}

	return KEEP_STATE;
}

HRESULT CState_PlayerForm::EnterState()
{
	// �÷��̾� �� ���� ���� ����
	m_pPlayer = static_cast<CPlayer*>(m_pGameObject);
	m_pCombat_Assistant = m_pPlayer->Get_CombatAssistant();
	m_pAction_Assistant = m_pPlayer->Get_ActionAssistant();
	m_pPlayer_Weapon = m_pPlayer->Get_Weapon();

	// � ������ ��ȯ�ߴ��� �����´�
	m_eInitialForm = m_pPlayer->Get_Form();
	
	// ��ȯ�� ���� ���� ó��
	CPlayer::SORA_ANIM form_animation = CPlayer::SORA_ANIM_END;
	switch (m_eInitialForm)
	{
	case Client::CPlayer::SF_HAMMER:

		// �� ��ȯ �ִϸ��̼� ����
		form_animation = CPlayer::SORA_ANIM_HAMMER_FORM;

		// ī�޶� ��ũ
		GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_HAMMER_TRANS);

		// ���� �� ����Ʈ �Լ� ȣ��
		m_pPlayer_Weapon->Appear_Hammer();

		break;
	case Client::CPlayer::SF_RAGE:

		// �� ��ȯ �ִϸ��̼� ����
		form_animation = CPlayer::SORA_ANIM_RAGE_FORM;

		// ī�޶� ��ũ
		GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_RAGE_TRANS);

		// ������ �� ������ �ѱ�
		GM->Set_Rage_Gauge(true);

		break;
	}

	// �ִϸ��̼� ����
	m_pPlayer->Change_PlayerAnimation(form_animation);

	// ������ �ظ� ������ ������ ��� ���� ��ȯ���ش�
	if (CPlayer::SORA_ANIM_HAMMER_FORM == form_animation)
	{
		m_pPlayer_Weapon->WeaponType(CPlayer_Weapon::WEAPON_TYPE_HYPER_HAMMER_CHANGING);
		m_pPlayer_Weapon->Reset_Animation();
	}

	// ���ӽð� ����
	m_dFormTimer = g_Form_Duration;

	// �����ð� ����
	m_pPlayer->Invincibility_Input(
		m_pModelCom->Get_TargetDuration(form_animation),
		m_pModelCom->Get_TargetPlaySpeed(form_animation));

	// ���� ���ǵ� ����
	list<CMonster*>* monster_list = GM->Get_MonstersList();
	for (auto pMonster : *monster_list)
	{
		CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), pMonster->Get_LayerTag(), 0.1);
		CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_MonsterSkill"), 0.1);
	}
	
	return S_OK;
}

HRESULT CState_PlayerForm::ExitState()
{
	// �������ظ� ���� ����� �� ���⸦ FAVORITE_DEPUTY�� �����ش�
	if (CPlayer::SF_HAMMER == m_eInitialForm)
		m_pPlayer_Weapon->WeaponType(CPlayer_Weapon::WEAPON_TYPE_FAVORITE_DEPUTY);
	
	// �� ���¸� ����� ���·� �����ش�
	m_pPlayer->Set_Form(CPlayer::SF_NORMAL);

	// ü�� ���¸� �����´�
	_bool air = m_pTransformCom->Get_JumpBoolean();

	// ����
	if (false == air)
	{
		_bool holding_weapon = m_pPlayer->Get_HoldingWeapon();
		_bool hovering = m_pPlayer->Get_Hovering();

		// ����
		if (false == holding_weapon)
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_IDLE);

		// �Ǽ�
		else
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_IDLE_BLADE);

		if (true == hovering)
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_AIR_IDLE);
	}

	// ����
	else
	{
		_float dir = m_pTransformCom->Calc_DirectionOnJump(GM->Get_Gravity());

		// ��� ��
		if (0 < dir)
			m_pTransformCom->BasicJump(CTransform::JUMP_FALL);

		// �ϰ� ��
		else
			m_pTransformCom->BasicJump(CTransform::JUMP_CONTINUE);
	}

	// ���� ���°� ������ ��� == ���߿� ���� ���
	if (true == m_pStateMachine->FindState(CStateMachine::STATETYPE::STATETYPE_JUMP))
	{
		CState_Jump* jump_state = static_cast<CState_Jump*>(m_pStateMachine->Get_State(CStateMachine::STATETYPE::STATETYPE_JUMP));

		jump_state->SetUp_AnimationChain();
		jump_state->Change_Animation_Falling();
	}

	return S_OK;
}

HRESULT CState_PlayerForm::ClearState()
{
	// �������ظ� ���� ����� �� ���⸦ FAVORITE_DEPUTY�� �����ش�
	if (CPlayer::SF_HAMMER == m_eInitialForm)
		m_pPlayer_Weapon->WeaponType(CPlayer_Weapon::WEAPON_TYPE_FAVORITE_DEPUTY);

	// �� ���¸� ����� ���·� �����ش�
	m_pPlayer->Set_Form(CPlayer::SF_NORMAL);
	m_pPlayer->Set_FormTexture(false);

	// UI ����
	for (_uint i = 0; i < CPlayer::FORM_FUNCTION::FORM_FUNC_END; ++i)
	{
		m_pPlayer->Dead_FormUI((CPlayer::FORM_FUNCTION)i);
		m_pPlayer->Null_FormUI((CPlayer::FORM_FUNCTION)i);
	}

	// ��� Able ���� true
	for (_uint i = 0; i < CAction_Assistant::ASSIST_TYPE::ASSIST_END; ++i)
		m_pAction_Assistant->Able((CAction_Assistant::ASSIST_TYPE)i, true);

	// ���� �ִϸ��̼ǰ� ü�� ���¸� �����´�
	CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);
	_bool air_state = m_pTransformCom->Get_JumpBoolean();

	// CState_PlayerForm::ClearState()�� ȣ��Ǿ��� �� ���� �ִϸ��̼��� �ǴϽ� �����̶�� ���� �ִϸ��̼����� ��ȯ������Ѵ�
	if (CPlayer::SORA_ANIM_RAGE_FINISH == curr_anim ||
		CPlayer::SORA_ANIM_HAMMER_FINISH == curr_anim)
	{
		// ����
		if (false == air_state)
		{
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_FALL_END);
		}

		// ����
		else
		{
			CState_Jump* jump_state = static_cast<CState_Jump*>(m_pStateMachine->Get_State(CStateMachine::STATETYPE::STATETYPE_JUMP));

			m_pTransformCom->BasicJump(CTransform::JUMP_FALL);

			jump_state->SetUp_AnimationChain();
			jump_state->Change_Animation_Falling();
		}

		// ���� ���ǵ� ���󺹱�
		list<CMonster*>* monster_list = GM->Get_MonstersList();
		for (auto pMonster : *monster_list)
		{
			CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), pMonster->Get_LayerTag(), 1.0);
			CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_MonsterSkill"), 1.0);
		}
	}

	return S_OK;
}

void CState_PlayerForm::Create_RageForm_Effects()
{
	// ��� ����Ʈ�� �����Ǿ��ٸ� break;
	if (true == m_bRageForm_Effect_Created[RAGE_EFFECT_ROAR])
		return;

	// Ÿ�̹� üũ�ϴµ� �ʿ��� ���� ������
	_uint curr_anim = m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);
	_double time_acc = m_pModelCom->Get_TargetTimeAcc(curr_anim);

	// �ð� üũ�ϸ鼭 ����Ʈ ���������� ����
	for (_uint i = 0; i < RAGE_EFFECT_END; ++i)
	{
		if (true == m_bRageForm_Effect_Created[i])
			continue;

		if (time_acc >= m_dRageForm_Effect_CreateTime[i])
		{
			CEffect_Manager* em = CEffect_Manager::GetInstance();
			_vector curr_player_position = m_pTransformCom->Get_Position();

			switch (RAGE_FORM_EFFECT(i))
			{
			case Client::CState_PlayerForm::RAGE_EFFECT_PEARL:
				em->Create_RageForm_RedPearl(curr_player_position);
				break;
			case Client::CState_PlayerForm::RAGE_EFFECT_RIPPLE:
				em->Create_RageForm_Ripple(curr_player_position);
				break;
			case Client::CState_PlayerForm::RAGE_EFFECT_MARBLE:

				if (true == m_pPlayer->Check_OnGround())
					vecy(curr_player_position) += 0.1f;

				em->Create_RageForm_Marble(curr_player_position);
				break;
			case Client::CState_PlayerForm::RAGE_EFFECT_PARTICLE:
				em->Create_RageForm_Particle(m_pTransformCom);
				break;
			case Client::CState_PlayerForm::RAGE_EFFECT_ROAR:
				em->Create_RageForm_Roaring(m_pTransformCom);
				break;
			}

			m_bRageForm_Effect_Created[i] = true;
		}
	}
}

void CState_PlayerForm::Play_RageForm_Sounds()
{
	// ������ ������� ���� ����Ǿ��ٸ� return
	if (true == m_bRageForm_Sound_Played[3])
		return;

	// Ÿ�̹� üũ�ϴµ� �ʿ��� ���� ������
	_uint curr_anim = m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);
	_uint time_acc_in_uint = (_uint)m_pModelCom->Get_TargetTimeAcc(curr_anim);

	// �ð� üũ�ϸ鼭 ���� ���������� ���
	for (_uint i = 0; i < 4; ++i)
	{
		if (true == m_bRageForm_Sound_Played[i])
			continue;

		// 0
		if (false == m_bRageForm_Sound_Played[0] && 0 <= time_acc_in_uint)
		{
			m_pPlayer->Play_RageFormSound(0);
			m_bRageForm_Sound_Played[0] = true;
		}

		// 17
		if (false == m_bRageForm_Sound_Played[1] && 17 <= time_acc_in_uint)
		{
			m_pPlayer->Play_RageFormSound(1);
			m_bRageForm_Sound_Played[1] = true;
		}

		// 30
		if (false == m_bRageForm_Sound_Played[2] && 30 <= time_acc_in_uint)
		{
			m_pPlayer->Play_RageFormSound(2);
			m_bRageForm_Sound_Played[2] = true;
		}

		// 56
		if (false == m_bRageForm_Sound_Played[3] && 56 <= time_acc_in_uint)
		{
			m_pPlayer->Play_RageFormSound(3);
			m_bRageForm_Sound_Played[3] = true;
		}
	}
}

void CState_PlayerForm::Play_HammerForm_Sounds()
{
	if (true == m_bHammerForm_Sound_Played)
		return;

	// Ÿ�̹� üũ�ϴµ� �ʿ��� ���� ������
	_uint curr_anim = m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);
	_uint time_acc_in_uint = (_uint)m_pModelCom->Get_TargetTimeAcc(curr_anim);

	m_pPlayer->Play_HammerFormSound();

	m_bHammerForm_Sound_Played = true;
}

void CState_PlayerForm::Create_HammerForm_ChangingEffect()
{
	_uint curr_anim = m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);
	_uint time_acc_in_uint = (_uint)m_pModelCom->Get_TargetTimeAcc(curr_anim);

	if (m_iCreatedFrame_HammerForm_Changing_Effect != time_acc_in_uint)
	{
		// 38, 58, 78, 98, 118
		if (38 == time_acc_in_uint ||
			58 == time_acc_in_uint ||
			78 == time_acc_in_uint)
		{
			CEffect_Manager::GetInstance()->Create_HammerForm_Gust(m_pTransformCom, m_pPlayer->Get_BodyCollider_Center());
			m_iCreatedFrame_HammerForm_Changing_Effect = time_acc_in_uint;
		}
	}
}

CState_PlayerForm * CState_PlayerForm::Create(void * pArg)
{
	CState_PlayerForm* pInstance = new CState_PlayerForm;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

void CState_PlayerForm::Free()
{
	__super::Free();
}
