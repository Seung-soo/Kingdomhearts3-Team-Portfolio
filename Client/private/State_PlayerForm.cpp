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
		// 이펙트 생성 여부 확인하는 Bool 초기화
		m_bRageForm_Effect_Created[i] = false;

		// 이펙트 생성 타이밍 설정
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
	// 레이지 폼 캔슬 LShift + G
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

		//	// 공격 상태 생성
		//	m_pPlayer->Create_PlayerState(CStateMachine::STATETYPE_ATK);

		//	// 애니메이션 변경
		//	m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_HAMMER_FINISH);
		//	m_pPlayer->Set_AbleToFinish(false);

		//	// 카메라 연출 true -> 만들어야함

		//	// 무적시간 설정
		//	m_pPlayer->Invincibility_Input(
		//		m_pModelCom->Get_TargetDuration(CPlayer::SORA_ANIM_HAMMER_FINISH),
		//		m_pModelCom->Get_TargetPlaySpeed(CPlayer::SORA_ANIM_HAMMER_FINISH));

		//	// 게임 스피드 제어
		//	list<CMonster*>* monster_list = GM->Get_MonstersList();
		//	for (auto pMonster : *monster_list)
		//	{
		//		CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), pMonster->Get_LayerTag(), 0.1);
		//		CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_MonsterSkill"), 0.1);
		//	}
		//}

		//	// UI 사운드 재생
		//	m_pPlayer->Play_FormChangeUI();


		break;
	case Client::CPlayer::SF_RAGE:

		if (pGI->Get_KeyEnter(DIK_F))
		{
			if (false == able_to_finish)
				return true;

			// 공격 상태 생성
			m_pPlayer->Create_PlayerState(CStateMachine::STATETYPE_ATK);

			// 애니메이션 변경
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_FINISH);
			m_pPlayer->Set_AbleToFinish(false);

			// 카메라 연출 true
			GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_RAGE_FINISHATTACK, m_pTransformCom);

			// 무적시간 설정
			m_pPlayer->Invincibility_Input(
				m_pModelCom->Get_TargetDuration(CPlayer::SORA_ANIM_RAGE_FINISH),
				m_pModelCom->Get_TargetPlaySpeed(CPlayer::SORA_ANIM_RAGE_FINISH));

			// 게임 스피드 제어
			list<CMonster*>* monster_list = GM->Get_MonstersList();
			for (auto pMonster : *monster_list)
			{
				CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), pMonster->Get_LayerTag(), 0.1);
				CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_MonsterSkill"), 0.1);
			}

			// UI 사운드 재생
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

	// 폼 해제 조건 검사
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

	// 폼 상태 로직
	CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);

	_bool anim_done = m_pModelCom->CheckStop_EachBody(CPlayer::SB_BODY);
	_bool air_state = m_pTransformCom->Get_JumpBoolean();

	_double time_acc = m_pModelCom->Get_TargetTimeAcc(curr_anim);
	_bool hovering = m_pPlayer->Get_Hovering();

	switch (curr_anim)
	{
	case Client::CPlayer::SORA_ANIM_RAGE_FORM:

		// 애니메이션이 종료되었다면
		if (true == anim_done)
		{
			// 지상
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

			// 게임 스피드 원상복구
			list<CMonster*>* monster_list = GM->Get_MonstersList();
			for (auto pMonster : *monster_list)
			{
				CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), pMonster->Get_LayerTag(), 1.0);
				CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_MonsterSkill"), 1.0);
			}
		}
		
		// 레이지 폼 이펙트 생성
		Create_RageForm_Effects();
		
		// 레이지 폼 사운드 재생
		Play_RageForm_Sounds();

		{
			_uint time_acc_in_uint = (_uint)m_pModelCom->Get_TargetTimeAcc(CPlayer::SORA_ANIM_RAGE_FORM);
			if (false == m_pPlayer->Get_FormTexture() && 30 <= time_acc_in_uint)
				m_pPlayer->Set_FormTexture(true);
		}

		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_FORM:

		// 애니메이션이 종료되었다면
		if (true == anim_done)
		{
			// 지상
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
			
			m_pPlayer_Weapon->WeaponType(CPlayer_Weapon::WEAPON_TYPE_HYPER_HAMMER);

			// 게임 스피드 원상복구
			list<CMonster*>* monster_list = GM->Get_MonstersList();
			for (auto pMonster : *monster_list)
			{
				CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), pMonster->Get_LayerTag(), 1.0);
				CGameInstance::GetInstance()->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_MonsterSkill"), 1.0);
			}
		}

		// 해머 폼 사운드 재생
		Play_HammerForm_Sounds();

		// 해머 폼 변신 이펙트 생성
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
			// 피니시 어택 변수 설정
			m_pPlayer->Set_AbleToFinish(true);
		}
	}

	// 타이머 체크 후 폼 상태 해제
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
	// 플레이어 및 관련 변수 참조
	m_pPlayer = static_cast<CPlayer*>(m_pGameObject);
	m_pCombat_Assistant = m_pPlayer->Get_CombatAssistant();
	m_pAction_Assistant = m_pPlayer->Get_ActionAssistant();
	m_pPlayer_Weapon = m_pPlayer->Get_Weapon();

	// 어떤 폼으로 변환했는지 가져온다
	m_eInitialForm = m_pPlayer->Get_Form();
	
	// 변환한 폼에 따른 처리
	CPlayer::SORA_ANIM form_animation = CPlayer::SORA_ANIM_END;
	switch (m_eInitialForm)
	{
	case Client::CPlayer::SF_HAMMER:

		// 폼 변환 애니메이션 세팅
		form_animation = CPlayer::SORA_ANIM_HAMMER_FORM;

		// 카메라 워크
		GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_HAMMER_TRANS);

		// 웨폰 림 라이트 함수 호출
		m_pPlayer_Weapon->Appear_Hammer();

		break;
	case Client::CPlayer::SF_RAGE:

		// 폼 변환 애니메이션 세팅
		form_animation = CPlayer::SORA_ANIM_RAGE_FORM;

		// 카메라 워크
		GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_RAGE_TRANS);

		// 레이지 폼 게이지 켜기
		GM->Set_Rage_Gauge(true);

		break;
	}

	// 애니메이션 변경
	m_pPlayer->Change_PlayerAnimation(form_animation);

	// 하이퍼 해머 폼으로 변신할 경우 무기 전환해준다
	if (CPlayer::SORA_ANIM_HAMMER_FORM == form_animation)
	{
		m_pPlayer_Weapon->WeaponType(CPlayer_Weapon::WEAPON_TYPE_HYPER_HAMMER_CHANGING);
		m_pPlayer_Weapon->Reset_Animation();
	}

	// 지속시간 설정
	m_dFormTimer = g_Form_Duration;

	// 무적시간 설정
	m_pPlayer->Invincibility_Input(
		m_pModelCom->Get_TargetDuration(form_animation),
		m_pModelCom->Get_TargetPlaySpeed(form_animation));

	// 게임 스피드 제어
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
	// 하이퍼해머 폼이 종료될 때 무기를 FAVORITE_DEPUTY로 돌려준다
	if (CPlayer::SF_HAMMER == m_eInitialForm)
		m_pPlayer_Weapon->WeaponType(CPlayer_Weapon::WEAPON_TYPE_FAVORITE_DEPUTY);
	
	// 폼 상태를 노멀폼 상태로 돌려준다
	m_pPlayer->Set_Form(CPlayer::SF_NORMAL);

	// 체공 상태를 가져온다
	_bool air = m_pTransformCom->Get_JumpBoolean();

	// 지상
	if (false == air)
	{
		_bool holding_weapon = m_pPlayer->Get_HoldingWeapon();
		_bool hovering = m_pPlayer->Get_Hovering();

		// 무기
		if (false == holding_weapon)
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_IDLE);

		// 맨손
		else
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_IDLE_BLADE);

		if (true == hovering)
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_AIR_IDLE);
	}

	// 공중
	else
	{
		_float dir = m_pTransformCom->Calc_DirectionOnJump(GM->Get_Gravity());

		// 상승 중
		if (0 < dir)
			m_pTransformCom->BasicJump(CTransform::JUMP_FALL);

		// 하강 중
		else
			m_pTransformCom->BasicJump(CTransform::JUMP_CONTINUE);
	}

	// 점프 상태가 존재할 경우 == 공중에 있을 경우
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
	// 하이퍼해머 폼이 종료될 때 무기를 FAVORITE_DEPUTY로 돌려준다
	if (CPlayer::SF_HAMMER == m_eInitialForm)
		m_pPlayer_Weapon->WeaponType(CPlayer_Weapon::WEAPON_TYPE_FAVORITE_DEPUTY);

	// 폼 상태를 노멀폼 상태로 돌려준다
	m_pPlayer->Set_Form(CPlayer::SF_NORMAL);
	m_pPlayer->Set_FormTexture(false);

	// UI 제거
	for (_uint i = 0; i < CPlayer::FORM_FUNCTION::FORM_FUNC_END; ++i)
	{
		m_pPlayer->Dead_FormUI((CPlayer::FORM_FUNCTION)i);
		m_pPlayer->Null_FormUI((CPlayer::FORM_FUNCTION)i);
	}

	// 모든 Able 변수 true
	for (_uint i = 0; i < CAction_Assistant::ASSIST_TYPE::ASSIST_END; ++i)
		m_pAction_Assistant->Able((CAction_Assistant::ASSIST_TYPE)i, true);

	// 현재 애니메이션과 체공 상태를 가져온다
	CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);
	_bool air_state = m_pTransformCom->Get_JumpBoolean();

	// CState_PlayerForm::ClearState()가 호출되었을 때 현재 애니메이션이 피니시 어택이라면 착지 애니메이션으로 전환해줘야한다
	if (CPlayer::SORA_ANIM_RAGE_FINISH == curr_anim ||
		CPlayer::SORA_ANIM_HAMMER_FINISH == curr_anim)
	{
		// 지상
		if (false == air_state)
		{
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_FALL_END);
		}

		// 공중
		else
		{
			CState_Jump* jump_state = static_cast<CState_Jump*>(m_pStateMachine->Get_State(CStateMachine::STATETYPE::STATETYPE_JUMP));

			m_pTransformCom->BasicJump(CTransform::JUMP_FALL);

			jump_state->SetUp_AnimationChain();
			jump_state->Change_Animation_Falling();
		}

		// 게임 스피드 원상복구
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
	// 모든 이펙트가 생성되었다면 break;
	if (true == m_bRageForm_Effect_Created[RAGE_EFFECT_ROAR])
		return;

	// 타이밍 체크하는데 필요한 변수 가져옴
	_uint curr_anim = m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);
	_double time_acc = m_pModelCom->Get_TargetTimeAcc(curr_anim);

	// 시간 체크하면서 이펙트 순차적으로 생성
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
	// 마지막 사운드까지 전부 재생되었다면 return
	if (true == m_bRageForm_Sound_Played[3])
		return;

	// 타이밍 체크하는데 필요한 변수 가져옴
	_uint curr_anim = m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);
	_uint time_acc_in_uint = (_uint)m_pModelCom->Get_TargetTimeAcc(curr_anim);

	// 시간 체크하면서 사운드 순차적으로 재생
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

	// 타이밍 체크하는데 필요한 변수 가져옴
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
