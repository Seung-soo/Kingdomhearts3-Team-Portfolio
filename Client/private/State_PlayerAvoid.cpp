#include "stdafx.h"
#include "State_PlayerAvoid.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "StateMachine.h"
#include "Effect_Manager.h"

HRESULT CState_PlayerAvoid::Initialize(void * pArg)
{
	return S_OK;
}

_bool CState_PlayerAvoid::KeyInput(_double TimeDelta)
{
	/*
		input에서 할 것이 없다.
		항상 true를 반환하게끔 만들고

		액션에서 돌아가는 로직에 의해서
		스테이트를 삭제한다.
	*/

	return true;
}

_bool CState_PlayerAvoid::Action(_double TimeDelta)
{
	_bool input = KeyInput(TimeDelta);

	// 키 입력이 없을 경우
	if (false == input)
	{
		ExitState();
		return DELETE_STATE;
	}

	// 애니메이션 인덱스를 받아둔다
	CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);
	if (true == m_pStateMachine->Find_AnimIndex(m_iStateType, curr_anim))
	{
		// 애니메이션이 끝났을 경우 exit
		_bool anim_done = m_pModelCom->CheckStop_EachBody(CPlayer::SB_BODY);
		if (true == anim_done)
		{
			ExitState();
			return DELETE_STATE;
		}

		// 사운드 재생(레이지 폼이 아닌 폼에서의 사운드 재생이다)
		if (false == m_bAvoidSoundPlayed)
			m_bAvoidSoundPlayed = m_pPlayer->Play_AvoidSound(curr_anim);

		// 레이지 폼 사운드 재생
		_uint time_acc_in_uint = (_uint)m_pModelCom->Get_TargetTimeAcc(curr_anim);
		if (m_iSoundPlayedFrame != time_acc_in_uint)
		{
			switch (curr_anim)
			{
			case Client::CPlayer::SORA_ANIM_RAGE_ROLL:
			case Client::CPlayer::SORA_ANIM_RAGE_AIR_DASH:

				if (2 == time_acc_in_uint || 16 == time_acc_in_uint)
				{
					m_pPlayer->Play_RageTeleport();
					m_iSoundPlayedFrame = time_acc_in_uint;
				}

				break;
			}
		}
	}
	
	return KEEP_STATE;
}

HRESULT CState_PlayerAvoid::EnterState()
{
	// 플레이어 자료형으로 형변환해서 주소저장
	m_pPlayer = static_cast<CPlayer*>(m_pGameObject);
	m_pCombat_Assistant = m_pPlayer->Get_CombatAssistant();
	m_pAction_Assistant = m_pPlayer->Get_ActionAssistant();
	m_pPlayer_Weapon = m_pPlayer->Get_Weapon();

	/*
		상태의 애니메이션으로 전환한다.
		 - 지상일 경우 구르기
		 - 공중일 경우 에어대시
			- 중력 해제
	*/

	// 해당 프레임에 입력받아 저장해둔 방향으로 캐릭터를 회전시킨다
	_vector avoiding_direction = m_pPlayer->Get_AvoidingDir();
	m_pTransformCom->MakeSameDirection(avoiding_direction);

	// 현재 폼 상태
	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();

	// 체공 상태
	_bool air = m_pTransformCom->Get_JumpBoolean();

	// 지상에 있을 경우
	if (false == air)
	{
		switch (curr_form)
		{
		case Client::CPlayer::SF_NORMAL:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_ROLL);
			CEffect_Manager::GetInstance()->Create_AirWind(m_pTransformCom->Get_WorldFloat4x4_Pointer());
			break;
		case Client::CPlayer::SF_HAMMER:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_HAMMER_DASH);
			break;
		case Client::CPlayer::SF_RAGE:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_ROLL);
			break;
		case Client::CPlayer::SF_END:
			_breakpoint;
			break;
		}
	}

	// 공중에 있을 경우
	else
	{
		switch (curr_form)
		{
		case Client::CPlayer::SF_NORMAL:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_AIR_DASH);

			CEffect_Manager::GetInstance()->Create_Dash(m_pTransformCom);
			CEffect_Manager::GetInstance()->Create_AirWind(m_pTransformCom->Get_WorldFloat4x4_Pointer());
			break;
		case Client::CPlayer::SF_HAMMER:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_HAMMER_DASH);
			break;
		case Client::CPlayer::SF_RAGE:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_RAGE_AIR_DASH);
			break;
		case Client::CPlayer::SF_END:
			_breakpoint;
			break;
		}
	}

	// 이동 스테이트가 있을 경우 해당 스테이트를 삭제
	m_pStateMachine->ClearState(CStateMachine::STATETYPE_NORMALMOVE);
	m_pStateMachine->Clear_CrashTypes((CStateMachine::STATETYPE)m_iStateType);

	// 사운드 재생
	m_pPlayer->Play_ActionVoiceSound(0.75f);

	return S_OK;
}

HRESULT CState_PlayerAvoid::ExitState()
{
	// 현재 폼 상태
	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();

	// 체공 상태, 무기 상태
	_bool air = m_pTransformCom->Get_JumpBoolean();
	_bool holding_weapon = m_pPlayer->Get_HoldingWeapon();

	// 지상에 있을 경우
	if (false == air)
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
		case Client::CPlayer::SF_END:
			_breakpoint;
			break;
		}
	}

	// 공중에 있을 경우
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

CState_PlayerAvoid * CState_PlayerAvoid::Create(void * pArg)
{
	CState_PlayerAvoid* pInstance = new CState_PlayerAvoid;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

void CState_PlayerAvoid::Free()
{
	__super::Free();
}
