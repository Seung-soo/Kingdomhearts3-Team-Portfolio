#include "stdafx.h"
#include "..\public\State_PlayerSummon.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "Effect_Manager.h"

HRESULT CState_PlayerSummon::Initialize(void * pArg)
{
	return S_OK;
}

_bool CState_PlayerSummon::KeyInput(_double TimeDelta)
{
	return true;
}

_bool CState_PlayerSummon::Action(_double TimeDelta)
{
	_bool input = KeyInput(TimeDelta);
	_bool result = KEEP_STATE;

	// 입력이 없음
	if (false == input)
	{
		ExitState();
		return DELETE_STATE;
	}

	// 입력이 있는 상태
	_bool anim_done = m_pModelCom->CheckStop_Input(m_eInitialAnim);
	if (true == anim_done)
	{
		ExitState();
		return DELETE_STATE;
	}

	_double time_acc = m_pModelCom->Get_TargetTimeAcc(m_eInitialAnim);

	// 애니메이션이 끝나지 않았을 때
	switch (m_eInitialAnim)
	{
		// 1단계 스킬 발동
	case Client::CPlayer::SORA_ANIM_SKILL_LEVEL1:
	case Client::CPlayer::SORA_ANIM_AIR_SKILL_LEVEL1:

		// 이동 중 스킬 발동
	case Client::CPlayer::SORA_ANIM_SKILL_MOVE:
	case Client::CPlayer::SORA_ANIM_AIR_SKILL_MOVE:

		// 해머 폼 1단계 스킬 발동
	case Client::CPlayer::SORA_ANIM_HAMMER_SKILL_LEVEL01:
	case Client::CPlayer::SORA_ANIM_HAMMER_AIR_SKILL_LEVEL01:

		if (false == m_bSummonEffect_Created)
		{
			CEffect_Manager::GetInstance()->Create_Summon(m_pTransformCom);
			CEffect_Manager::GetInstance()->Create_SummonParticle(m_pTransformCom);
			m_pPlayer->Play_SummonSound();

			m_bSummonEffect_Created = true;
		}

		break;

		// 2단계 스킬 발동
	case Client::CPlayer::SORA_ANIM_SKILL_LEVEL2:
	case Client::CPlayer::SORA_ANIM_AIR_SKILL_LEVEL2:

		// 해머 폼 2단계 스킬 발동
	case Client::CPlayer::SORA_ANIM_HAMMER_SKILL_LEVEL02:
	case Client::CPlayer::SORA_ANIM_HAMMER_AIR_SKILL_LEVEL02:

		if (time_acc >= 7.0 && false == m_bSummonEffect_Created)
		{
			CEffect_Manager::GetInstance()->Create_Summon(m_pTransformCom);
			CEffect_Manager::GetInstance()->Create_SummonParticle(m_pTransformCom);
			m_pPlayer->Play_SummonSound();

			m_bSummonEffect_Created = true;
		}

		break;
	}

	return result;
}

HRESULT CState_PlayerSummon::EnterState()
{
	m_pPlayer = static_cast<CPlayer*>(m_pGameObject);
	m_pCombat_Assistant = m_pPlayer->Get_CombatAssistant();
	m_pAction_Assistant = m_pPlayer->Get_ActionAssistant();
	m_pPlayer_Weapon = m_pPlayer->Get_Weapon();

	CPlayer::SORA_MAGIC magic = m_pPlayer->Get_Magic();
	_bool air = m_pPlayer->Get_Transform()->Get_JumpBoolean();
	_bool move = m_pStateMachine->FindState(CStateMachine::STATETYPE_NORMALMOVE);
	CPlayer::SORA_FORM form = m_pPlayer->Get_Form();

	switch (magic)
	{
	case Client::CPlayer::SM_DONALD1:
	case Client::CPlayer::SM_GOOFY1:

		// 지상
		if (false == air)
		{
			switch (form)
			{
			case Client::CPlayer::SF_NORMAL:

				if (false == move)
					m_eInitialAnim = CPlayer::SORA_ANIM_SKILL_LEVEL1;
				else
					m_eInitialAnim = CPlayer::SORA_ANIM_SKILL_MOVE;

				break;
			case Client::CPlayer::SF_HAMMER:

				m_eInitialAnim = CPlayer::SORA_ANIM_HAMMER_SKILL_LEVEL01;

				break;
			case Client::CPlayer::SF_RAGE:

				// 레이지 폼에서는 마법 사용 불가능

				break;
			}
		}

		// 공중
		else
		{
			switch (form)
			{
			case Client::CPlayer::SF_NORMAL:

				if (false == move)
					m_eInitialAnim = CPlayer::SORA_ANIM_AIR_SKILL_LEVEL1;
				else
					m_eInitialAnim = CPlayer::SORA_ANIM_AIR_SKILL_MOVE;

				break;
			case Client::CPlayer::SF_HAMMER:

				m_eInitialAnim = CPlayer::SORA_ANIM_HAMMER_AIR_SKILL_LEVEL01;

				break;
			case Client::CPlayer::SF_RAGE:

				// 레이지 폼에서는 마법 사용 불가능

				break;
			}
		}

		break;
	case Client::CPlayer::SM_DONALD2:
	case Client::CPlayer::SM_GOOFY2:

		// 지상
		if (false == air)
		{
			switch (form)
			{
			case Client::CPlayer::SF_NORMAL:

				if (false == move)
					m_eInitialAnim = CPlayer::SORA_ANIM_SKILL_LEVEL2;
				else
					m_eInitialAnim = CPlayer::SORA_ANIM_SKILL_MOVE;

				break;
			case Client::CPlayer::SF_HAMMER:

				m_eInitialAnim = CPlayer::SORA_ANIM_HAMMER_SKILL_LEVEL02;

				break;
			case Client::CPlayer::SF_RAGE:

				// 레이지 폼에서는 마법 사용 불가능

				break;
			}
		}

		// 공중
		else
		{
			switch (form)
			{
			case Client::CPlayer::SF_NORMAL:

				if (false == move)
					m_eInitialAnim = CPlayer::SORA_ANIM_AIR_SKILL_LEVEL2;
				else
					m_eInitialAnim = CPlayer::SORA_ANIM_AIR_SKILL_MOVE;

				break;
			case Client::CPlayer::SF_HAMMER:

				m_eInitialAnim = CPlayer::SORA_ANIM_HAMMER_AIR_SKILL_LEVEL02;

				break;
			case Client::CPlayer::SF_RAGE:

				// 레이지 폼에서는 마법 사용 불가능

				break;
			}
		}

		break;
	}

	// 애니메이션 세팅
	m_pModelCom->ResetAnimation((_uint)m_eInitialAnim);
	m_pPlayer->Change_PlayerAnimation(m_eInitialAnim);

	// 무적 시간 세팅
	m_pPlayer->Invincibility_Input(
		m_pModelCom->Get_TargetDuration((_uint)m_eInitialAnim),
		m_pModelCom->Get_TargetPlaySpeed((_uint)m_eInitialAnim));

	return S_OK;
}

HRESULT CState_PlayerSummon::ExitState()
{
	// 현재 애니메이션 상태가 무엇이었는지 확인
	CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);
	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();

	_bool air_state = m_pPlayer->Get_Transform()->Get_JumpBoolean();

	switch (curr_anim)
	{
		// 지상 연출 애니메이션
	case Client::CPlayer::SORA_ANIM_SKILL_LEVEL1:
	case Client::CPlayer::SORA_ANIM_SKILL_LEVEL2:
	case Client::CPlayer::SORA_ANIM_HAMMER_SKILL_LEVEL01:
	case Client::CPlayer::SORA_ANIM_HAMMER_SKILL_LEVEL02:

		switch (curr_form)
		{
		case Client::CPlayer::SF_NORMAL:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_IDLE);
			break;
		case Client::CPlayer::SF_HAMMER:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_HAMMER_IDLE_SHORT);
			break;
		case Client::CPlayer::SF_RAGE:
			// 레이지 폼 상태에서는 마법 사용 불가능
			break;
		}
		
		break;

		// 공중 연출 애니메이션
	case Client::CPlayer::SORA_ANIM_AIR_SKILL_LEVEL1:
	case Client::CPlayer::SORA_ANIM_AIR_SKILL_LEVEL2:
	case Client::CPlayer::SORA_ANIM_SKILL_MOVE:
	case Client::CPlayer::SORA_ANIM_AIR_SKILL_MOVE:
	case Client::CPlayer::SORA_ANIM_HAMMER_AIR_SKILL_LEVEL01:
	case Client::CPlayer::SORA_ANIM_HAMMER_AIR_SKILL_LEVEL02:

		if (false == air_state)
		{
			switch (curr_form)
			{
			case Client::CPlayer::SF_NORMAL:
				m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_FALL_END);
				break;
			case Client::CPlayer::SF_HAMMER:
				// 해머 폼 상태에서는 지상에서 낙하연출할 것이 없음
				break;
			case Client::CPlayer::SF_RAGE:
				// 레이지 폼 상태에서는 마법 사용 불가능
				break;
			}
		}
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
				// 레이지 폼 상태에서는 마법 사용 불가능
				break;
			}

			m_pTransformCom->BasicJump(CTransform::JUMP_FALL);
		}
		

		break;
	}

	m_pPlayer->Set_Input(CPlayer::SORA_INPUT::SI_SUMMON, false);
	m_pPlayer->Set_Magic(CPlayer::SORA_MAGIC::SM_END);

	return S_OK;
}

HRESULT CState_PlayerSummon::ClearState()
{
	m_pPlayer->Set_Input(CPlayer::SORA_INPUT::SI_SUMMON, false);
	m_pPlayer->Set_Magic(CPlayer::SORA_MAGIC::SM_END);

	_bool air_state = m_pTransformCom->Get_JumpBoolean();

	// 현재 애니메이션 상태가 무엇이었는지 확인
	CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);
	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
	
	switch (curr_anim)
	{
		// 지상 연출 애니메이션
	case Client::CPlayer::SORA_ANIM_SKILL_LEVEL1:
	case Client::CPlayer::SORA_ANIM_SKILL_LEVEL2:
	case Client::CPlayer::SORA_ANIM_HAMMER_SKILL_LEVEL01:
	case Client::CPlayer::SORA_ANIM_HAMMER_SKILL_LEVEL02:

		switch (curr_form)
		{
		case Client::CPlayer::SF_NORMAL:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_IDLE);
			break;
		case Client::CPlayer::SF_HAMMER:
			m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_HAMMER_IDLE_SHORT);
			break;
		case Client::CPlayer::SF_RAGE:
			// 레이지 폼 상태에서는 마법 사용 불가능
			break;
		}

		break;

		// 공중 연출 애니메이션
	case Client::CPlayer::SORA_ANIM_AIR_SKILL_LEVEL1:
	case Client::CPlayer::SORA_ANIM_AIR_SKILL_LEVEL2:
	case Client::CPlayer::SORA_ANIM_SKILL_MOVE:
	case Client::CPlayer::SORA_ANIM_AIR_SKILL_MOVE:
	case Client::CPlayer::SORA_ANIM_HAMMER_AIR_SKILL_LEVEL01:
	case Client::CPlayer::SORA_ANIM_HAMMER_AIR_SKILL_LEVEL02:

		if (false == air_state)
		{
			switch (curr_form)
			{
			case Client::CPlayer::SF_NORMAL:
				m_pPlayer->Change_PlayerAnimation(CPlayer::SORA_ANIM_FALL_END);
				m_pStateMachine->ClearState(CStateMachine::STATETYPE::STATETYPE_NORMALMOVE);
				break;
			case Client::CPlayer::SF_HAMMER:
				// 해머 폼 상태에서는 지상에서 낙하연출할 것이 없음
				break;
			case Client::CPlayer::SF_RAGE:
				// 레이지 폼 상태에서는 마법 사용 불가능
				break;
			}
		}
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
				// 레이지 폼 상태에서는 마법 사용 불가능
				break;
			}

			m_pTransformCom->BasicJump(CTransform::JUMP_FALL);
		}


		break;
	}

	return S_OK;
}

CState_PlayerSummon * CState_PlayerSummon::Create(void * pArg)
{
	CState_PlayerSummon* pInstance = new CState_PlayerSummon;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

void CState_PlayerSummon::Free()
{
	__super::Free();
}
