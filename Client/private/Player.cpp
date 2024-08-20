#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"
#include "MeshContainer.h"
#include "GameManager.h"
#include "ImGui_Manager.h"
#include "Progress_Manager.h"

#include "Camera_Perspective.h"
#include "State_PlayerIdle.h"
#include "State_PlayerMove.h"
#include "State_PlayerDynamicMove.h"
#include "State_Jump.h"
#include "State_PlayerAttack.h"
#include "State_PlayerSummon.h"
#include "State_PlayerGuard.h"
#include "State_PlayerAvoid.h"
#include "State_PlayerDamaged.h"
#include "State_PlayerInteraction.h"
#include "State_PlayerForm.h"

#include "Player_Weapon.h"
#include "Combat_Assistant.h"
#include "Action_Assistant.h"
#include "Effect_VISwordTrail.h"
#include "Effect_Mesh.h"
#include "Effect_Texture.h"
#include "Effect_Particle.h"

#include "Effect_Manager.h"
#include "Camera_Manager.h"
#include "Effect_Trail.h"

#include "UI_Num_Manager.h"
#include "Terrain.h"
#include "MotionTrail.h"

#include "Monster.h"
#include "IceMan.h"
#include "UI_FormChange.h"

_bool CPlayer::g_RatioControl = false;
_float CPlayer::g_RatioXZ = 0.f;
_float CPlayer::g_RatioY = 0.f;

_bool CPlayer::g_SoldierType = false;

_float CPlayer::g_PlayerDefaultMoveSpeed = 11.5f;

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
{
}

_bool CPlayer::Set_ItemValues(PLAYER_ITEM eItemType, _double dDuration)
{
	if (true == m_bItem[eItemType])
		return false;

	// bool ���� ����, ���ӽð� ����
	m_bItem[eItemType] = true;
	m_dItemTimer[eItemType] = dDuration;

	// ����Ʈ ����
	switch (eItemType)
	{
	case Client::CPlayer::PLAYER_ITEM_DAMAGE:
		CEffect_Manager::GetInstance()->Create_PlayerBuffEffect(m_pTransformCom);
		break;
	case Client::CPlayer::PLAYER_ITEM_ATTACK_SPEED:
		CEffect_Manager::GetInstance()->Create_PlayerBuffEffect_AttackSpeed(m_pTransformCom);
		break;
	case Client::CPlayer::PLAYER_ITEM_MOVE_SPEED:
		CEffect_Manager::GetInstance()->Create_PlayerBuffEffect_MovingSpeed(m_pTransformCom);
		break;
	}

	// ������Ʈ bool ���� ����
	m_isUsingRim = true;

	// ������Ʈ ���� ����
	_vector rim_color = XMLoadFloat4(&m_vRimColor);
	rim_color += XMLoadFloat4(&m_vRimColorEachItem[eItemType]);
	XMStoreFloat4(&m_vRimColor, rim_color);
	
	return true;
}

void CPlayer::Apply_Item(PLAYER_ITEM eItemType)
{
	_float value = 0.f;
	vector<CAnimation*>* animations = m_pModelCom->Get_Animations();

	switch (eItemType)
	{
	case Client::CPlayer::PLAYER_ITEM_DAMAGE:

		value = m_pStatusCom->Get_Damage();
		m_pWeapon->Get_StatusCom()->Set_Damage(value + 1.f);

		break;
	case Client::CPlayer::PLAYER_ITEM_ATTACK_SPEED:

		for (auto anim_enum : m_Animation_NormalAttacks)
			(*animations)[anim_enum]->Set_PlaySpeed_Rate(1.5);

		break;
	case Client::CPlayer::PLAYER_ITEM_MOVE_SPEED:

		m_pTransformCom->Set_Speed(14.f);

		break;
	}
}

void CPlayer::Play_MoveSound(_float fVolum)
{
	// ���� ������ ä��
	CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_ACTION];

	LEVEL current_level = GM->Get_CurrentLevel();
	if (LEVEL::LEVEL_RAPUNZEL_BOSS == current_level)
	{
		_int move_on_grass_sound_index = rand() % 4;
		_vector current_player_position = m_pTransformCom->Get_Position();

		m_pGameInstance->StopSound(target_channel);

		switch (move_on_grass_sound_index)
		{
		case 0:
			m_pGameInstance->LetsPlaySound(TEXT("Player_StepGrass0.ogg"), target_channel, fVolum, true, current_player_position);
			break;
		case 1:
			m_pGameInstance->LetsPlaySound(TEXT("Player_StepGrass1.ogg"), target_channel, fVolum, true, current_player_position);
			break;
		case 2:
			m_pGameInstance->LetsPlaySound(TEXT("Player_StepGrass2.ogg"), target_channel, fVolum, true, current_player_position);
			break;
		case 3:
			m_pGameInstance->LetsPlaySound(TEXT("Player_StepGrass3.ogg"), target_channel, fVolum, true, current_player_position);
			break;
		}

		Update_SoundChannel(PLAYER_SOUND_ACTION);
	}
	else
	{
		_int move_sound_index = rand() % 4;
		_vector current_player_position = m_pTransformCom->Get_Position();

		m_pGameInstance->StopSound(target_channel);

		switch (move_sound_index)
		{
		case 0:
			m_pGameInstance->LetsPlaySound(TEXT("Player_Step0.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 1:
			m_pGameInstance->LetsPlaySound(TEXT("Player_Step1.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 2:
			m_pGameInstance->LetsPlaySound(TEXT("Player_Step2.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 3:
			m_pGameInstance->LetsPlaySound(TEXT("Player_Step3.wav"), target_channel, fVolum, true, current_player_position);
			break;
		}

		Update_SoundChannel(PLAYER_SOUND_ACTION);
	}
}

void CPlayer::Play_LandSound(_float fVolum)
{
	CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_ACTION];
	_vector current_player_position = m_pTransformCom->Get_Position();

	m_pGameInstance->StopSound(target_channel);
	m_pGameInstance->LetsPlaySound(TEXT("Player_Land.wav"), target_channel, fVolum, true, current_player_position);

	Update_SoundChannel(PLAYER_SOUND_ACTION);
}

void CPlayer::Play_RiseSound(_float fVolum)
{
	CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_ACTION];
	_vector current_player_position = m_pTransformCom->Get_Position();

	m_pGameInstance->StopSound(target_channel);
	m_pGameInstance->LetsPlaySound(TEXT("Player_Rise.wav"), target_channel, fVolum, true, current_player_position);

	Update_SoundChannel(PLAYER_SOUND_ACTION);

	m_bLandSoundPlayed = true;
}

void CPlayer::Play_AttackVoiceSound(_bool bLastCombo, _float fVolum)
{
	// �Ϲ� ����
	if (false == bLastCombo)
	{
		// ���� ������ ä��
		CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_ATTACK];

		// ���� ����� ���� �̰� 3D ���带 ���� ���� ��ġ�� �޾Ƶд�
		_int normal_attack_sound_index = rand() % 6;
		_vector current_player_position = m_pTransformCom->Get_Position();

		m_pGameInstance->StopSound(target_channel);

		switch (normal_attack_sound_index)
		{
		case 0:
			m_pGameInstance->LetsPlaySound(TEXT("Player_Attack0.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 1:
			m_pGameInstance->LetsPlaySound(TEXT("Player_Attack1.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 2:
			m_pGameInstance->LetsPlaySound(TEXT("Player_Attack2.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 3:
			m_pGameInstance->LetsPlaySound(TEXT("Player_Attack3.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 4:
			m_pGameInstance->LetsPlaySound(TEXT("Player_Attack4.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 5:
			m_pGameInstance->LetsPlaySound(TEXT("Player_Attack5.wav"), target_channel, fVolum, true, current_player_position);
			break;
		}

		Update_SoundChannel(PLAYER_SOUND_ATTACK);
	}
	// �޺��� ������ ����
	else
	{
		// ���� ������ ä��
		CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_LAST_AND_FINISH];

		// ���� ����� ���� �̰� 3D ���带 ���� ���� ��ġ�� �޾Ƶд�
		_int last_attack_sound_index = rand() % 2;
		_vector current_player_position = m_pTransformCom->Get_Position();

		m_pGameInstance->StopSound(target_channel);

		switch (last_attack_sound_index)
		{
		case 0:
			m_pGameInstance->LetsPlaySound(TEXT("Player_AttackLast0.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 1:
			m_pGameInstance->LetsPlaySound(TEXT("Player_AttackLast1.wav"), target_channel, fVolum, true, current_player_position);
			break;
		}

		Update_SoundChannel(PLAYER_SOUND_LAST_AND_FINISH);
	}
}

void CPlayer::Play_LastAttackVoiceSound(_float fVolum)
{
	// �ʿ��� ���� �غ�
	_vector current_player_position = m_pTransformCom->Get_Position();

	// ���� ������ ä��
	CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_LAST_AND_FINISH];
	m_pGameInstance->StopSound(target_channel);

	_int last_attack_sound_index = rand() % 8;
	switch (last_attack_sound_index)
	{
	case 0:
		m_pGameInstance->LetsPlaySound(TEXT("Player_LastAttack0.wav"), target_channel, fVolum, true, current_player_position);
		break;
	case 1:
		m_pGameInstance->LetsPlaySound(TEXT("Player_LastAttack1.wav"), target_channel, fVolum, true, current_player_position);
		break;
	case 2:
		m_pGameInstance->LetsPlaySound(TEXT("Player_LastAttack2.wav"), target_channel, fVolum, true, current_player_position);
		break;
	case 3:
		m_pGameInstance->LetsPlaySound(TEXT("Player_LastAttack3.wav"), target_channel, fVolum, true, current_player_position);
		break;
	case 4:
		m_pGameInstance->LetsPlaySound(TEXT("Player_LastAttack4.wav"), target_channel, fVolum, true, current_player_position);
		break;
	case 5:
		m_pGameInstance->LetsPlaySound(TEXT("Player_LastAttack5.wav"), target_channel, fVolum, true, current_player_position);
		break;
	case 6:
		m_pGameInstance->LetsPlaySound(TEXT("Player_LastAttack6.wav"), target_channel, fVolum, true, current_player_position);
		break;
	case 7:
		m_pGameInstance->LetsPlaySound(TEXT("Player_LastAttack7.wav"), target_channel, fVolum, true, current_player_position);
		break;
	}

	Update_SoundChannel(PLAYER_SOUND_LAST_AND_FINISH);
}

void CPlayer::Play_AttackHitSound(_bool bLastCombo, _float fVolum)
{
	// �ʿ��� ���� �غ�
	_vector current_player_position = m_pTransformCom->Get_Position();
	CPlayer_Weapon::WEAPON_TYPE weapon_type = m_pWeapon->WeaponType();
	SORA_ANIM curr_anim = (SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(SB_BODY);

	// ���� ������ ä��
	CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_ATTACK];
	m_pGameInstance->StopSound(target_channel);

	_uint hit_sound_index = UINT_MAX;
	switch (weapon_type)
	{
	case Client::CPlayer_Weapon::WEAPON_TYPE_KINGDOM_KEY:

		switch (m_eForm)
		{
		case Client::CPlayer::SF_NORMAL:

			if (false == bLastCombo)
			{
				hit_sound_index = rand() % 7;
				switch (hit_sound_index)
				{
				case 0:
					m_pGameInstance->LetsPlaySound(TEXT("Player_KeyHit0.wav"), target_channel, fVolum, true, current_player_position);
					break;
				case 1:
					m_pGameInstance->LetsPlaySound(TEXT("Player_KeyHit1.wav"), target_channel, fVolum, true, current_player_position);
					break;
				case 2:
					m_pGameInstance->LetsPlaySound(TEXT("Player_KeyHit2.wav"), target_channel, fVolum, true, current_player_position);
					break;
				case 3:
					m_pGameInstance->LetsPlaySound(TEXT("Player_KeyHit3.wav"), target_channel, fVolum, true, current_player_position);
					break;
				case 4:
					m_pGameInstance->LetsPlaySound(TEXT("Player_KeyHit4.wav"), target_channel, fVolum, true, current_player_position);
					break;
				case 5:
					m_pGameInstance->LetsPlaySound(TEXT("Player_KeyHit5.wav"), target_channel, fVolum, true, current_player_position);
					break;
				case 6:
					m_pGameInstance->LetsPlaySound(TEXT("Player_KeyHit6.wav"), target_channel, fVolum, true, current_player_position);
					break;
				}
			}
			else
			{
				hit_sound_index = rand() % 3;
				switch (hit_sound_index)
				{
				case 0:
					m_pGameInstance->LetsPlaySound(TEXT("Player_KeyHitLast0.wav"), target_channel, fVolum, true, current_player_position);
					break;
				case 1:
					m_pGameInstance->LetsPlaySound(TEXT("Player_KeyHitLast1.wav"), target_channel, fVolum, true, current_player_position);
					break;
				case 2:
					m_pGameInstance->LetsPlaySound(TEXT("Player_KeyHitLast2.wav"), target_channel, fVolum, true, current_player_position);
					break;
				}
			}

			break;
		case Client::CPlayer::SF_RAGE:

			switch (curr_anim)
			{
			case Client::CPlayer::SORA_ANIM_RAGE_DOUBLE_TAP:
			case Client::CPlayer::SORA_ANIM_RAGE_TRIPLE_TAP:
			case Client::CPlayer::SORA_ANIM_RAGE_ATTACK1:
			case Client::CPlayer::SORA_ANIM_RAGE_ATTACK2:
				hit_sound_index = rand() % 4;
				switch (hit_sound_index)
				{
				case 0:
					m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormHit0.wav"), target_channel, fVolum, true, current_player_position);
					break;
				case 1:
					m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormHit1.wav"), target_channel, fVolum, true, current_player_position);
					break;
				case 2:
					m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormHit2.wav"), target_channel, fVolum, true, current_player_position);
					break;
				case 3:
					m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormHit3.wav"), target_channel, fVolum, true, current_player_position);
					break;
				}
				break;
			case Client::CPlayer::SORA_ANIM_RAGE_ATTACK3:
			case Client::CPlayer::SORA_ANIM_RAGE_ATTACK4:
				hit_sound_index = rand() % 5;
				switch (hit_sound_index)
				{
				case 0:
					m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormHit4.wav"), target_channel, fVolum, true, current_player_position);
					break;
				case 1:
					m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormHit5.wav"), target_channel, fVolum, true, current_player_position);
					break;
				case 2:
					m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormHit6.wav"), target_channel, fVolum, true, current_player_position);
					break;
				case 3:
					m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormHit7.wav"), target_channel, fVolum, true, current_player_position);
					break;
				case 4:
					m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormHit8.wav"), target_channel, fVolum, true, current_player_position);
					break;
				}
				break;
			case Client::CPlayer::SORA_ANIM_RAGE_ATTACK5:
				hit_sound_index = rand() % 9;
				switch (hit_sound_index)
				{
				case 0:
					m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormHit0.wav"), target_channel, fVolum, true, current_player_position);
					break;
				case 1:
					m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormHit1.wav"), target_channel, fVolum, true, current_player_position);
					break;
				case 2:
					m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormHit2.wav"), target_channel, fVolum, true, current_player_position);
					break;
				case 3:
					m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormHit3.wav"), target_channel, fVolum, true, current_player_position);
					break;
				case 4:
					m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormHit4.wav"), target_channel, fVolum, true, current_player_position);
					break;
				case 5:
					m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormHit5.wav"), target_channel, fVolum, true, current_player_position);
					break;
				case 6:
					m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormHit6.wav"), target_channel, fVolum, true, current_player_position);
					break;
				case 7:
					m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormHit7.wav"), target_channel, fVolum, true, current_player_position);
					break;
				case 8:
					m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormHit8.wav"), target_channel, fVolum, true, current_player_position);
					break;
				}
				break;
			}

			break;
		}
		
		break;
	case Client::CPlayer_Weapon::WEAPON_TYPE_FAVORITE_DEPUTY:
	case Client::CPlayer_Weapon::WEAPON_TYPE_HYPER_HAMMER:

		if (false == bLastCombo)
		{
			hit_sound_index = rand() % 6;
			switch (hit_sound_index)
			{
			case 0:
				m_pGameInstance->LetsPlaySound(TEXT("Player_FvdHit0.wav"), target_channel, fVolum, true, current_player_position);
				break;
			case 1:
				m_pGameInstance->LetsPlaySound(TEXT("Player_FvdHit1.wav"), target_channel, fVolum, true, current_player_position);
				break;
			case 2:
				m_pGameInstance->LetsPlaySound(TEXT("Player_FvdHit2.wav"), target_channel, fVolum, true, current_player_position);
				break;
			case 3:
				m_pGameInstance->LetsPlaySound(TEXT("Player_FvdHit3.wav"), target_channel, fVolum, true, current_player_position);
				break;
			case 4:
				m_pGameInstance->LetsPlaySound(TEXT("Player_FvdHit4.wav"), target_channel, fVolum, true, current_player_position);
				break;
			case 5:
				m_pGameInstance->LetsPlaySound(TEXT("Player_FvdHit5.wav"), target_channel, fVolum, true, current_player_position);
				break;
			}
		}
		else
		{
			hit_sound_index = rand() % 3;
			switch (hit_sound_index)
			{
			case 0:
				m_pGameInstance->LetsPlaySound(TEXT("Player_FvdHitLast0.wav"), target_channel, fVolum, true, current_player_position);
				break;
			case 1:
				m_pGameInstance->LetsPlaySound(TEXT("Player_FvdHitLast1.wav"), target_channel, fVolum, true, current_player_position);
				break;
			case 2:
				m_pGameInstance->LetsPlaySound(TEXT("Player_FvdHitLast2.wav"), target_channel, fVolum, true, current_player_position);
				break;
			}
		}

		break;
	}

	Update_SoundChannel(PLAYER_SOUND_ATTACK);
}

void CPlayer::Play_NormalFormSwing(_float fVolum)
{
	// �ʿ��� ���� �غ�
	_vector current_player_position = m_pTransformCom->Get_Position();
	CPlayer_Weapon::WEAPON_TYPE weapon_type = m_pWeapon->WeaponType();
	_int normal_swing_sound_index = rand() % 5;

	// ���� ������ ä��
	CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_ATTACK];
	m_pGameInstance->StopSound(target_channel);

	if (CPlayer_Weapon::WEAPON_TYPE::WEAPON_TYPE_KINGDOM_KEY)
	{
		switch (normal_swing_sound_index)
		{
		case 0:
			m_pGameInstance->LetsPlaySound(TEXT("Player_KeySwing0.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 1:
			m_pGameInstance->LetsPlaySound(TEXT("Player_KeySwing1.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 2:
			m_pGameInstance->LetsPlaySound(TEXT("Player_KeySwing2.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 3:
			m_pGameInstance->LetsPlaySound(TEXT("Player_KeySwing3.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 4:
			m_pGameInstance->LetsPlaySound(TEXT("Player_KeySwing4.wav"), target_channel, fVolum, true, current_player_position);
			break;
		}
	}
	else
	{
		switch (normal_swing_sound_index)
		{
		case 0:
			m_pGameInstance->LetsPlaySound(TEXT("Player_FvdSwing0.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 1:
			m_pGameInstance->LetsPlaySound(TEXT("Player_FvdSwing1.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 2:
			m_pGameInstance->LetsPlaySound(TEXT("Player_FvdSwing2.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 3:
			m_pGameInstance->LetsPlaySound(TEXT("Player_FvdSwing3.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 4:
			m_pGameInstance->LetsPlaySound(TEXT("Player_FvdSwing4.wav"), target_channel, fVolum, true, current_player_position);
			break;
		}
	}

	Update_SoundChannel(PLAYER_SOUND_ATTACK);
}

void CPlayer::Play_HammerFormSound(_float fVolum)
{
	// �ʿ��� ���� �غ�
	_vector current_player_position = m_pTransformCom->Get_Position();

	// ���� ������ ä��
	CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_EFFECT];
	m_pGameInstance->StopSound(target_channel);

	m_pGameInstance->LetsPlaySound(TEXT("Player_HammerForm.wav"), target_channel, fVolum, true, current_player_position);
	Update_SoundChannel(PLAYER_SOUND_EFFECT);
}

void CPlayer::Play_HammerFormSwing(_float fVolum)
{
	// �ʿ��� ���� �غ�
	_vector current_player_position = m_pTransformCom->Get_Position();
	_int hammer_swing_sound_index = rand() % 3;

	// ���� ������ ä��
	CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_ATTACK];
	m_pGameInstance->StopSound(target_channel);

	switch (hammer_swing_sound_index)
	{
	case 0:
		m_pGameInstance->LetsPlaySound(TEXT("Player_HamSwing0.wav"), target_channel, fVolum, true, current_player_position);
		break;
	case 1:
		m_pGameInstance->LetsPlaySound(TEXT("Player_HamSwing1.wav"), target_channel, fVolum, true, current_player_position);
		break;
	case 2:
		m_pGameInstance->LetsPlaySound(TEXT("Player_HamSwing2.wav"), target_channel, fVolum, true, current_player_position);
		break;
	}

	Update_SoundChannel(PLAYER_SOUND_ATTACK);
}

void CPlayer::Play_HammerFormWaveSound(SORA_ANIM eCurrentnAnim, _float fVolum)
{
	// �ʿ��� ���� �غ�
	_vector current_player_position = m_pTransformCom->Get_Position();
	_int hammer_swing_sound_index = rand() % 2;
	

	// ���� ������ ä��
	CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_ATTACK];
	m_pGameInstance->StopSound(target_channel);

	switch (eCurrentnAnim)
	{
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_01:
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_06:

		if (0 == hammer_swing_sound_index)
			m_pGameInstance->LetsPlaySound(TEXT("Player_HammerGroundHit2.wav"), target_channel, fVolum, true, current_player_position);
		else
			m_pGameInstance->LetsPlaySound(TEXT("Player_HammerGroundHit3.wav"), target_channel, fVolum, true, current_player_position);

		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_05:
		
		if (0 == hammer_swing_sound_index)
			m_pGameInstance->LetsPlaySound(TEXT("Player_HammerGroundHit0.wav"), target_channel, fVolum, true, current_player_position);
		else
			m_pGameInstance->LetsPlaySound(TEXT("Player_HammerGroundHit1.wav"), target_channel, fVolum, true, current_player_position);

		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_GROUND_LAST_ATTACK:
		
		if (0 == hammer_swing_sound_index)
			m_pGameInstance->LetsPlaySound(TEXT("Player_HammerGroundHit4.wav"), target_channel, fVolum, true, current_player_position);
		else
			m_pGameInstance->LetsPlaySound(TEXT("Player_HammerGroundHit5.wav"), target_channel, fVolum, true, current_player_position);

		break;
	}

	Update_SoundChannel(PLAYER_SOUND_ATTACK);
}

void CPlayer::Play_HammerFormLastSound(_float fVolum)
{
	// �ʿ��� ���� �غ�
	_vector current_player_position = m_pTransformCom->Get_Position();

	// ���� ������ ä��
	CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_EFFECT];
	m_pGameInstance->StopSound(target_channel);

	m_pGameInstance->LetsPlaySound(TEXT("Player_HammerLast.wav"), target_channel, fVolum, true, current_player_position);
	Update_SoundChannel(PLAYER_SOUND_EFFECT);
}

void CPlayer::Play_ActionVoiceSound(_float fVolum)
{
	if (SF_RAGE == m_eForm)
		return;

	// ���� ������ ä��
	CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_ACTION];

	_int action_sound_index = rand() % 5;
	_vector current_player_position = m_pTransformCom->Get_Position();

	m_pGameInstance->StopSound(target_channel);

	switch (action_sound_index)
	{
	case 0:
		m_pGameInstance->LetsPlaySound(TEXT("Player_Action0.wav"), target_channel, fVolum, true, current_player_position);
		break;
	case 1:
		m_pGameInstance->LetsPlaySound(TEXT("Player_Action1.wav"), target_channel, fVolum, true, current_player_position);
		break;
	case 2:
		m_pGameInstance->LetsPlaySound(TEXT("Player_Action2.wav"), target_channel, fVolum, true, current_player_position);
		break;
	case 3:
		m_pGameInstance->LetsPlaySound(TEXT("Player_Action3.wav"), target_channel, fVolum, true, current_player_position);
		break;
	case 4:
		m_pGameInstance->LetsPlaySound(TEXT("Player_Action4.wav"), target_channel, fVolum, true, current_player_position);
		break;
	}

	Update_SoundChannel(PLAYER_SOUND_ACTION);
}

void CPlayer::Play_DamagedVoiceSound(_float fVolum)
{
	// ���� ������ ä��
	CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_DAMAGED];

	_int damaged_sound_index = rand() % 5;
	_vector current_player_position = m_pTransformCom->Get_Position();

	m_pGameInstance->StopSound(target_channel);

	switch (damaged_sound_index)
	{
	case 0:
		m_pGameInstance->LetsPlaySound(TEXT("Player_Damaged0.wav"), target_channel, fVolum, true, current_player_position);
		break;
	case 1:
		m_pGameInstance->LetsPlaySound(TEXT("Player_Damaged1.wav"), target_channel, fVolum, true, current_player_position);
		break;
	case 2:
		m_pGameInstance->LetsPlaySound(TEXT("Player_Damaged2.wav"), target_channel, fVolum, true, current_player_position);
		break;
	case 3:
		m_pGameInstance->LetsPlaySound(TEXT("Player_Damaged3.wav"), target_channel, fVolum, true, current_player_position);
		break;
	case 4:
		m_pGameInstance->LetsPlaySound(TEXT("Player_Damaged4.wav"), target_channel, fVolum, true, current_player_position);
		break;
	}

	Update_SoundChannel(PLAYER_SOUND_DAMAGED);
}

void CPlayer::Play_GuardSucceededSound(_float fVolum)
{
	// �ʿ��� ���� �غ�
	_vector current_player_position = m_pTransformCom->Get_Position();
	_bool air_state = m_pTransformCom->Get_JumpBoolean();

	// ���� ������ ä��
	CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_DAMAGED];
	m_pGameInstance->StopSound(target_channel);
	
	m_pGameInstance->LetsPlaySound(TEXT("Player_GuardSucceeded.wav"), target_channel, fVolum, true, current_player_position);
	Update_SoundChannel(PLAYER_SOUND_DAMAGED);

	// ���� ���� �� �ٴڿ� ������ ����
	if (false == air_state)
	{
		target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_DAMAGED];
		m_pGameInstance->StopSound(target_channel);

		m_pGameInstance->LetsPlaySound(TEXT("Player_GuardSucceeded_KnockBackGround.wav"), target_channel, fVolum, true, current_player_position);
		Update_SoundChannel(PLAYER_SOUND_DAMAGED);
	}
}

_bool CPlayer::Play_AvoidSound(SORA_ANIM eCurrentnAnim, _float fVolum)
{
	_bool result = false;

	switch (eCurrentnAnim)
	{
		// ROLL
	case Client::CPlayer::SORA_ANIM_ROLL:

		if (true == m_pModelCom->Check_ProgressRate_EachBody(CPlayer::SB_BODY, 25.f))
		{
			// �ʿ��� ���� �غ�
			_vector current_player_position = m_pTransformCom->Get_Position();
			_bool air_state = m_pTransformCom->Get_JumpBoolean();

			// ���� ������ ä��
			CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_ACTION];
			m_pGameInstance->StopSound(target_channel);

			m_pGameInstance->LetsPlaySound(TEXT("Player_AvoidGround.wav"), target_channel, fVolum, true, current_player_position);
			Update_SoundChannel(PLAYER_SOUND_ACTION);

			result = true;
		}

		break;

		// AIR DASH
	case Client::CPlayer::SORA_ANIM_AIR_DASH:
	case Client::CPlayer::SORA_ANIM_HAMMER_DASH:

		// �ʿ��� ���� �غ�
		_vector current_player_position = m_pTransformCom->Get_Position();
		_bool air_state = m_pTransformCom->Get_JumpBoolean();

		// ���� ������ ä��
		CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_ACTION];
		m_pGameInstance->StopSound(target_channel);

		m_pGameInstance->LetsPlaySound(TEXT("Player_AvoidAir.wav"), target_channel, fVolum, true, current_player_position);
		Update_SoundChannel(PLAYER_SOUND_ACTION);

		result = true;

		break;
	}

	return result;
}

void CPlayer::Play_SummonSound(_float fVolum)
{
	if (SORA_MAGIC::SM_END == m_eMagic)
		_breakpoint;

	// �ʿ��� ���� �غ�
	_vector current_player_position = m_pTransformCom->Get_Position();

	// ���� ������ ä��
	CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_EFFECT];
	m_pGameInstance->StopSound(target_channel);

	switch (m_eMagic)
	{
	case Client::CPlayer::SM_DONALD1:
	case Client::CPlayer::SM_DONALD2:
		m_pGameInstance->LetsPlaySound(TEXT("Player_CallDonald.wav"), target_channel, fVolum, true, current_player_position);
		break;
	case Client::CPlayer::SM_GOOFY1:
	case Client::CPlayer::SM_GOOFY2:
		m_pGameInstance->LetsPlaySound(TEXT("Player_CallGoofy.wav"), target_channel, fVolum, true, current_player_position);
		break;
	}

	Update_SoundChannel(PLAYER_SOUND_EFFECT);
}

void CPlayer::Play_FormChangeUI(_float fVolum)
{
	// �ʿ��� ���� �غ�
	_vector current_player_position = m_pTransformCom->Get_Position();

	// ���� ������ ä��
	CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_EFFECT];
	m_pGameInstance->StopSound(target_channel);

	m_pGameInstance->LetsPlaySound(TEXT("Player_FormChangeUI.wav"), target_channel, fVolum, true, current_player_position);
	Update_SoundChannel(PLAYER_SOUND_EFFECT);
}

void CPlayer::Play_RageFormSound(_uint iSoundIndex, _float fVolum)
{
	// �ʿ��� ���� �غ�
	_vector current_player_position = m_pTransformCom->Get_Position();

	// ���� ������ ä��
	CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_EFFECT];
	m_pGameInstance->StopSound(target_channel);

	switch (iSoundIndex)
	{
	case 0:
		m_pGameInstance->LetsPlaySound(TEXT("Player_RageForm0.wav"), target_channel, fVolum, true, current_player_position);
		break;
	case 1:
		m_pGameInstance->LetsPlaySound(TEXT("Player_RageForm1.wav"), target_channel, fVolum, true, current_player_position);
		break;
	case 2:
		m_pGameInstance->LetsPlaySound(TEXT("Player_RageForm2.wav"), target_channel, fVolum, true, current_player_position);
		break;
	case 3:
		m_pGameInstance->LetsPlaySound(TEXT("Player_RageForm3.wav"), target_channel, fVolum, true, current_player_position);
		break;
	}

	Update_SoundChannel(PLAYER_SOUND_EFFECT);
}

void CPlayer::Play_RageFormSwing(_float fVolum)
{
	// �ʿ��� ���� �غ�
	_vector current_player_position = m_pTransformCom->Get_Position();
	SORA_ANIM curr_anim = (SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(SB_BODY);
	_uint rage_swing_sound_index = UINT_MAX;

	// ���� ������ ä��
	CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_ACTION];
	m_pGameInstance->StopSound(target_channel);

	// 0, 1, 2, 3 => ������, Ʈ���� ��, 1, 2, 4, 5
	// 4, 5, 6, 7, 8 => 3, 5
	switch (curr_anim)
	{
	case Client::CPlayer::SORA_ANIM_RAGE_DOUBLE_TAP:
	case Client::CPlayer::SORA_ANIM_RAGE_TRIPLE_TAP:
	case Client::CPlayer::SORA_ANIM_RAGE_ATTACK1:
	case Client::CPlayer::SORA_ANIM_RAGE_ATTACK2:
	case Client::CPlayer::SORA_ANIM_RAGE_ATTACK4:

		rage_swing_sound_index = rand() % 4;
		switch (rage_swing_sound_index)
		{
		case 0:
			m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormSwing0.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 1:
			m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormSwing1.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 2:
			m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormSwing2.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 3:
			m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormSwing3.wav"), target_channel, fVolum, true, current_player_position);
			break;
		}

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_ATTACK3:

		rage_swing_sound_index = rand() % 5;
		switch (rage_swing_sound_index)
		{
		case 0:
			m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormSwing4.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 1:
			m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormSwing5.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 2:
			m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormSwing6.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 3:
			m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormSwing7.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 4:
			m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormSwing8.wav"), target_channel, fVolum, true, current_player_position);
			break;
		}

		break;
	case Client::CPlayer::SORA_ANIM_RAGE_ATTACK5:

		rage_swing_sound_index = rand() % 9;
		switch (rage_swing_sound_index)
		{
		case 0:
			m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormSwing0.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 1:
			m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormSwing1.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 2:
			m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormSwing2.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 3:
			m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormSwing3.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 4:
			m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormSwing4.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 5:
			m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormSwing5.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 6:
			m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormSwing6.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 7:
			m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormSwing7.wav"), target_channel, fVolum, true, current_player_position);
			break;
		case 8:
			m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormSwing8.wav"), target_channel, fVolum, true, current_player_position);
			break;
		}

		break;
	}

	Update_SoundChannel(PLAYER_SOUND_ACTION);
}

void CPlayer::Play_RageWind(_float fVolum)
{
	// �ʿ��� ���� �غ�
	_vector current_player_position = m_pTransformCom->Get_Position();

	// ���� ������ ä��
	CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_EFFECT];
	m_pGameInstance->StopSound(target_channel);

	m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormWindSound.wav"), target_channel, fVolum, true, current_player_position);
	Update_SoundChannel(PLAYER_SOUND_EFFECT);
}

void CPlayer::Play_RageTeleport(_float fVolum)
{
	// �ʿ��� ���� �غ�
	_vector current_player_position = m_pTransformCom->Get_Position();

	// ���� ������ ä��
	CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_EFFECT];
	m_pGameInstance->StopSound(target_channel);

	m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormTeleport.wav"), target_channel, fVolum, true, current_player_position);
	Update_SoundChannel(PLAYER_SOUND_EFFECT);
}

void CPlayer::Play_RageGuard(_float fVolum)
{
	// �ʿ��� ���� �غ�
	_vector current_player_position = m_pTransformCom->Get_Position();

	// ���� ������ ä��
	CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_EFFECT];
	m_pGameInstance->StopSound(target_channel);

	m_pGameInstance->LetsPlaySound(TEXT("Player_RageFormGuard.wav"), target_channel, fVolum, true, current_player_position);
	Update_SoundChannel(PLAYER_SOUND_EFFECT);
}

void CPlayer::Play_RageFormLastSound(_float fVolum)
{
	// �ʿ��� ���� �غ�
	_vector current_player_position = m_pTransformCom->Get_Position();

	// ���� ������ ä��
	CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_EFFECT];
	m_pGameInstance->StopSound(target_channel);

	m_pGameInstance->LetsPlaySound(TEXT("Player_RageForm2.wav"), target_channel, fVolum, true, current_player_position);
	Update_SoundChannel(PLAYER_SOUND_EFFECT);
}

void CPlayer::Play_RageFormFinish(_float fVolum)
{
	// �ʿ��� ���� �غ�
	_vector current_player_position = m_pTransformCom->Get_Position();

	// ���� ������ ä��
	CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_EFFECT];
	m_pGameInstance->StopSound(target_channel);

	m_pGameInstance->LetsPlaySound(TEXT("Player_RageFinish.wav"), target_channel, fVolum, true, current_player_position);
	Update_SoundChannel(PLAYER_SOUND_EFFECT);
}

void CPlayer::Play_OpenChest(_uint iSoundIndex, _float fVolum)
{
	// �ʿ��� ���� �غ�
	_vector current_player_position = m_pTransformCom->Get_Position();

	// ���� ������ ä��
	CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_ACTION];
	m_pGameInstance->StopSound(target_channel);

	switch (iSoundIndex)
	{
	case 0:
		m_pGameInstance->LetsPlaySound(TEXT("Player_OpenChest0.wav"), target_channel, fVolum, true, current_player_position);
		break;
	case 1:
		m_pGameInstance->LetsPlaySound(TEXT("Player_OpenChest1.wav"), target_channel, fVolum, true, current_player_position);
		break;
	case 2:
		m_pGameInstance->LetsPlaySound(TEXT("Player_OpenChest2.wav"), target_channel, fVolum, true, current_player_position);
		break;
	case 3:
		m_pGameInstance->LetsPlaySound(TEXT("Player_OpenChest3.wav"), target_channel, fVolum, true, current_player_position);
		break;
	}
	
	Update_SoundChannel(PLAYER_SOUND_ACTION);
}

void CPlayer::Play_SideUI_PopUp_Sound(_float fVolum)
{
	// �ʿ��� ���� �غ�
	_vector current_player_position = m_pTransformCom->Get_Position();

	// ���� ������ ä��
	CSoundMgr::CHANNELID target_channel = (CSoundMgr::CHANNELID)m_iPlayerSound[PLAYER_SOUND_EFFECT];
	m_pGameInstance->StopSound(target_channel);

	m_pGameInstance->LetsPlaySound(TEXT("Player_FormChangeUI_PopUp.wav"), target_channel, fVolum, true, current_player_position);
	Update_SoundChannel(PLAYER_SOUND_EFFECT);
}

void CPlayer::Update_SoundChannel(PLAYER_SOUND_TYPE eSoundType)
{
	switch (eSoundType)
	{
	case Client::CPlayer::PLAYER_SOUND_ACTION:
		m_iPlayerSound[PLAYER_SOUND_ACTION]++;
		if (CSoundMgr::PLAYER_ACTION6 < m_iPlayerSound[PLAYER_SOUND_ACTION])
			m_iPlayerSound[PLAYER_SOUND_ACTION] = CSoundMgr::PLAYER_ACTION1;
		break;
	case Client::CPlayer::PLAYER_SOUND_ATTACK:
		m_iPlayerSound[PLAYER_SOUND_ATTACK]++;
		if (CSoundMgr::PLAYER_ATTACK6 < m_iPlayerSound[PLAYER_SOUND_ATTACK])
			m_iPlayerSound[PLAYER_SOUND_ATTACK] = CSoundMgr::PLAYER_ATTACK1;
		break;
	case Client::CPlayer::PLAYER_SOUND_LAST_AND_FINISH:
		m_iPlayerSound[PLAYER_SOUND_LAST_AND_FINISH]++;
		if (CSoundMgr::PLAYER_ATTACK_LF4 < m_iPlayerSound[PLAYER_SOUND_LAST_AND_FINISH])
			m_iPlayerSound[PLAYER_SOUND_LAST_AND_FINISH] = CSoundMgr::PLAYER_ATTACK_LF1;
		break;
	case Client::CPlayer::PLAYER_SOUND_DAMAGED:
		m_iPlayerSound[PLAYER_SOUND_DAMAGED]++;
		if (CSoundMgr::PLAYER_DAMAGED6 < m_iPlayerSound[PLAYER_SOUND_DAMAGED])
			m_iPlayerSound[PLAYER_SOUND_DAMAGED] = CSoundMgr::PLAYER_DAMAGED1;
		break;
	case Client::CPlayer::PLAYER_SOUND_EFFECT:
		m_iPlayerSound[PLAYER_SOUND_EFFECT]++;
		if (CSoundMgr::PLAYER_EFFECT6 < m_iPlayerSound[PLAYER_SOUND_EFFECT])
			m_iPlayerSound[PLAYER_SOUND_EFFECT] = CSoundMgr::PLAYER_EFFECT1;
		break;
	}
}

HRESULT CPlayer::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::NativeConstruct(void * pArg)
{
	// GameObject ���� ����
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = g_PlayerDefaultMoveSpeed;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;
	
	m_eObjectType = OBJECT_TYPE::OBJECT_PLAYER;
	m_iPass = 15;

	m_pTransformCom->Set_JumpValue(1.75f);
	m_pTransformCom->Set_Position(__vector(-20.f, 0.f, -7.25f, 1.f));
	m_pTransformCom->LookAtXZ(__vector(-12.f, 0.f, -15.f, 1.f));

	// Ʈ���϶��� Ÿ�� ���� ��Ÿ��
	//m_pTransformCom->Set_Position(__vector(20.f, 0.f, -20.f, 1.f));
	//m_pTransformCom->MakeSameDirection(_axisX);

	// Player ������Ʈ ����
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	// ========================================================================================
	// Model Component
	if (FAILED(Separate_Channels()))
		return E_FAIL;

	Adjust_Animation_Details();
	// ========================================================================================
	// StateMachine
	Create_PlayerState(CStateMachine::STATETYPE_IDLE);

	// �̱� ������Ʈ ����
	m_pStateMachineCom->Add_SingleType(CStateMachine::STATETYPE::STATETYPE_NORMALMOVE);
	m_pStateMachineCom->Add_SingleType(CStateMachine::STATETYPE::STATETYPE_ATK);
	m_pStateMachineCom->Add_SingleType(CStateMachine::STATETYPE::STATETYPE_GUARD);
	m_pStateMachineCom->Add_SingleType(CStateMachine::STATETYPE::STATETYPE_AVOID);
	m_pStateMachineCom->Add_SingleType(CStateMachine::STATETYPE::STATETYPE_DAMAGED);
	m_pStateMachineCom->Add_SingleType(CStateMachine::STATETYPE::STATETYPE_INTERACTION);
	// ========================================================================================
	// Player ���� ����

	// �ִϸ��̼� �ε���
	m_eSoraAnim[SORA_BODY::SB_BODY] = SORA_ANIM::SORA_ANIM_IDLE;
	m_eSoraAnim[SORA_BODY::SB_FACE] = SORA_ANIM::SORA_ANIM_IDLE;

	// �� ���� �ʱ�ȭ
	m_eForm = SORA_FORM::SF_NORMAL;

	// ���� ���� �ý���
	C_ASSIST::ASSIST_INIT_DESC c_assi_init_desc;
	c_assi_init_desc.range = 20.f;
	c_assi_init_desc.max_search = 10;
	c_assi_init_desc.skip_limit = 3;

	m_pCombat_Assistant = m_pCombat_Assistant->Create(this, c_assi_init_desc);
	m_pAction_Assistant = m_pAction_Assistant->Create(this);
	SetUp_Assistant();

	// �� ������ �浹 ����Ʈ
	ZeroMemory(&m_vWallFinder, sizeof(_float3));
	m_pWallFinderCom->Set_Color(_hotpink4);
	m_pWallFinderCom->Set_Scale(_float3(0.15f, 0.15f, 0.15f));
	
	// Collider ���̱� ���� Bone ������ �����´�.
	m_BoneMatrixPtr = m_pModelCom->Get_BoneMatrixPtr("center");
	m_SocketTransformMatrix = m_pModelCom->Get_TransformMatrix();

	m_bImGuiController = true;

	// Effect_Trail
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CEffect_Trail::TRAILDESC		tDesc;
	tDesc.pTargetTransform = m_pTransformCom;
	tDesc.pBoneMatrixPtr = m_pModelCom->Get_BoneMatrixPtr("L_te_belt_spo");
	tDesc.SocketPivotmMatrix = m_pModelCom->Get_TransformMatrix();
	tDesc.iPassIndex = 19;
	tDesc.tTrailBuffDesc.iNumVerticies = 300;
	tDesc.tTrailBuffDesc.fLifeTime = 0.1f;
	tDesc.tTrailBuffDesc.fLerpCnt = 10.f;
	tDesc.tTrailBuffDesc.fDuration = 0.01f;
	tDesc.vAdjustValue = XMVectorSet(0.f, 0.05f, 0.f, 0.f);
	tDesc.iTextureIndex = 160;
	tDesc.isWind = false;

	tDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);

	tDesc.bBlur = true;
	
	m_pTrail[0] = static_cast<CEffect_Trail*>(pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Trail"), &tDesc));

	tDesc.pBoneMatrixPtr = m_pModelCom->Get_BoneMatrixPtr("R_te_belt_spo");
	m_pTrail[1] = static_cast<CEffect_Trail*>(pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Trail"), &tDesc));

	tDesc.pBoneMatrixPtr = m_pModelCom->Get_BoneMatrixPtr("L_ashi1");
	m_pTrail[2] = static_cast<CEffect_Trail*>(pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Trail"), &tDesc));

	tDesc.pBoneMatrixPtr = m_pModelCom->Get_BoneMatrixPtr("R_ashi1");
	m_pTrail[3] = static_cast<CEffect_Trail*>(pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Trail"), &tDesc));	

	// �׽�Ʈ ���� ������ ��ġ��
	creation_point[0] = -15.f;
	creation_point[1] = 2.f;
	creation_point[2] = -15.f;

	// ���� Ÿ�̸�
	m_dGuardTimer_Max = 3.0;

	// ���� ���� �ʱ�ȭ
	for (_uint i = 0; i < P_SOUND::PLAYER_SOUND_END; ++i)
	{
		switch (i)
		{
		case Client::CPlayer::PLAYER_SOUND_ACTION:
			m_iPlayerSound[i] = CSoundMgr::PLAYER_ACTION1;
			break;
		case Client::CPlayer::PLAYER_SOUND_ATTACK:
			m_iPlayerSound[i] = CSoundMgr::PLAYER_ATTACK1;
			break;
		case Client::CPlayer::PLAYER_SOUND_LAST_AND_FINISH:
			m_iPlayerSound[i] = CSoundMgr::PLAYER_ATTACK_LF1;
			break;
		case Client::CPlayer::PLAYER_SOUND_DAMAGED:
			m_iPlayerSound[i] = CSoundMgr::PLAYER_DAMAGED1;
			break;
		case Client::CPlayer::PLAYER_SOUND_EFFECT:
			m_iPlayerSound[i] = CSoundMgr::PLAYER_EFFECT1;
			break;
		}
	}

	// ������ ���� ���� �ʱ�ȭ
	for (_uint i = 0; i < PLAYER_ITEM_END; ++i)
	{
		PLAYER_ITEM item_enum = (PLAYER_ITEM)i;
		m_bItem[item_enum] = false;
		m_dItemTimer[item_enum] = 0.0;
		
		switch (item_enum)
		{
		case Client::CPlayer::PLAYER_ITEM_DAMAGE:
			m_vRimColorEachItem[item_enum] = _float4(0.6f, 0.4f, 0.f, 0.f);
			break;
		case Client::CPlayer::PLAYER_ITEM_ATTACK_SPEED:
			m_vRimColorEachItem[item_enum] = _float4(1.f, 0.1f, 0.f, 0.f);
			break;
		case Client::CPlayer::PLAYER_ITEM_MOVE_SPEED:
			m_vRimColorEachItem[item_enum] = _float4(0.f, 0.3f, 1.f, 0.f);
			break;
		}
	}

	return S_OK;
}

_int CPlayer::Tick(_double TimeDelta)
{
	if (true == PM->Get_MinigameWaiting())
		return RESULT_NOPROBLEM;

	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	if (true == m_bDead)
		return RESULT_DEAD;

	if (true == m_pGameInstance->Get_KeyEnter(DIK_F1))
		CEffect_Manager::GetInstance()->Create_HammerForm_Gust(m_pTransformCom, m_pTransformCom->Get_Position());

	// ���� ��ü ���� �� ī�޶� ��ũ �۾� ����
	if (true == m_bFirstTick)
		Work_On_FirstTick();

	// �� ü���� ���� UI�� �����ش�
	Show_UI_FormChange();

	// ī�޶�� �÷��̾� ���� ���� ���
	if (nullptr != m_pCamera && nullptr != m_pCameraTransform)
		Calculate_AngleValues();

	// ���� ����� ���ΰ� �ش� ���α����� �Ÿ� ����
	if (nullptr != m_pNavigationCom)
		Calculate_CellValues();

	// ������ �Է°��� ����
	for (_uint i = 0; i < SI_END; ++i)
	{
		// ��ȯ ������ ��ȯ ���� ��ü ���� �Ŀ� false�� �ٲ۴�
		if (SI_SUMMON == (SORA_INPUT)i)
			continue;

		m_bInput[i] = false;
	}

	// ���� �������� �Է°��� �޾Ƶ�
	Update_Input(TimeDelta);
	Update_Terms(TimeDelta);

	// ���� �������� �Է°��� ���� ���� ����
	ChangeForm();
	ChangeState();

	// ���ݻ��°� �ƴ϶�� ������ '����Ÿ��' ���� -> Action�� ���� ���� �����صд�.
	if (false == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_ATK))
		m_pCombat_Assistant->Clear_Target(C_ASSIST::ASSIST_TYPE::ASSIST_ATTACK);

	// ����Ʈ ���� ON/OFF
	Effect_Control();

	// ImGuI�� ���� ��Ʈ�Ѱ� �����ϱ� ����.
	if (true == m_bControlMode)
	{
		// �÷��̾� ������Ʈ �ӽ� �۵�
		if (FAILED(m_pStateMachineCom->Action(TimeDelta)))
		{
			BREAKPOINT;
			return RESULT_ERROR;
		}
	}

	// ���� Ÿ���� �̹� �����ӿ� �ٲ���� ��, �� ���Կ� ���� ó��
	CellEntry_Processing();
	
	// ���������� ���õ� �ε���(TempIndex)�� �ִϸ��̼��� �ʱ�ȭ �� ������Ʈ ����
	if (false == m_pModelCom->Check_TempAndCurr_EachBody(SB_BODY))
		m_pModelCom->ResetAnimation(m_eSoraAnim[SORA_BODY::SB_BODY]);
	
	// ���� ��� ���� �ִϸ��̼��� ���� �ִϸ��̼��̰�
	// �ش� �ִϸ��̼� �ε����� �̹� �����ӿ� ���� ���̶��
	_uint temp_index = m_pModelCom->Get_TempIndex_EachBody(SB_BODY);
	if (true == m_pStateMachineCom->Find_AnimIndex(CStateMachine::STATETYPE::STATETYPE_ATK, temp_index) &&
		false == m_pModelCom->Check_TempAndCurr_EachBody(SB_BODY))
	{
		// ���� �ִϸ��̼��� XZ, Y�̵����� �����´�
		_float movement_XZ = m_pModelCom->Get_TargetMovementAmountXZ(temp_index);
		_float movement_Y = m_pModelCom->Get_TargetMovementAmountY(temp_index);
		m_pCombat_Assistant->Auto_Position(movement_XZ, movement_Y);

		// �ִϸ��̼� ���� �̵��� �����ϰ� ���� �� ����ó���ϴ� ��ġ
		if (SORA_ANIM::SORA_ANIM_LOWER_ATTACK3 == temp_index)
		{
			m_pModelCom->Set_TargetMovementRatioXZ(temp_index, 0.15f);
			m_pModelCom->Set_TargetMovementRatioY(temp_index, 0.15f);
		}

		_bool set_location_assistant = Anim_Check_MoveOnAxisY(temp_index);
		if (true == set_location_assistant)
		{
			// ���� ������ Ÿ�ٰ� �׿� ���� ������ �޾ƿ´�. Ÿ���� ���� ��� target_desc�� ZeroMemory�� �� ���·� ��ȯ�ȴ�
			CCombat_Assistant::ASSIST_TYPE target_type = m_pCombat_Assistant->Get_TargetType();
			CCombat_Assistant::ASSIST_DESC target_desc = m_pCombat_Assistant->Get_Desc(target_type);

			if (nullptr != target_desc.pCollider)
			{
				_vector target_collider_center = target_desc.pCollider->Get_Center();

				// ���̰� ����, �����̼� ��ý��Ͻ� ����ü ����
				Set_LocationAssistance_MoveAmountY(target_collider_center);
				Set_LocationAssistance_InputValues((SORA_ANIM)temp_index);
			}
		}
	}

	// ���� �ִϸ��̼��� �پ��� ���°�ü���� ��ȯ��Ű�� ������ �̰����� �ѹ��� ó���Ѵ�
	_uint curr_anim = m_pModelCom->Get_CurrAnimIndex_EachBody(SB_BODY);
	if (true == Anim_Check_LandOnGround(curr_anim))
	{
		// uint�� ����ȯ�ؼ� �޴´�
		_uint time_acc_in_uint = (_uint)m_pModelCom->Get_TargetTimeAcc(curr_anim);

		// ������ ���� ���� ��� -> 0�� ������ ������ �ߺ������ ����� ����
		if (0 == time_acc_in_uint && false == m_bLandSoundPlayed)
		{
			Play_LandSound();
			m_bLandSoundPlayed = true;
		}
	}
	else
		m_bLandSoundPlayed = false;

	// ���� ������ ���⼭ ����
	if (true == Anim_Check_GuardSucceeded(curr_anim))
	{
		// uint�� ����ȯ�ؼ� �޴´�
		_uint time_acc_in_uint = (_uint)m_pModelCom->Get_TargetTimeAcc(curr_anim);
		_bool anim_done = m_pModelCom->Get_TargetStopped(curr_anim);

		// ������ ���� ���� ��� -> 0�� ������ ������ �ߺ������ ����� ����
		if (0 == time_acc_in_uint && false == anim_done)
		{
			if (false == m_bGuardSucceededPlayed)
			{
				Play_GuardSucceededSound();
				m_bGuardSucceededPlayed = true;
			}
		}
		else
			m_bGuardSucceededPlayed = false;
	}
	else
		m_bGuardSucceededPlayed = false;

	// ImGuI �����
	if (true == g_RatioControl)
	{
		m_pModelCom->Set_TargetMovementRatioXZ(temp_index, g_RatioXZ);
		m_pModelCom->Set_TargetMovementRatioY(temp_index, g_RatioY);
	}

	// �˹� ������Ʈ
	if (0.0 < m_pTransformCom->Get_KnockBackTime())
		m_pTransformCom->KnockBackTick(TimeDelta, m_pNavigationCom); 

	// Y�̵� ����(Location Assistance) ������Ʈ
	if (0.0 < m_pTransformCom->Get_LocationInputTimer())
		LocationAssistance_Tick(TimeDelta);

	// ������ ���ӽð� ������Ʈ
	_bool check_all_item_false = true;
	for (_uint i = 0; i < PLAYER_ITEM::PLAYER_ITEM_END; ++i)
	{
		PLAYER_ITEM curr_item = (PLAYER_ITEM)i;

		if (false == m_bItem[curr_item])
			continue;

		m_dItemTimer[curr_item] -= TimeDelta;

		if (0.0 >= m_dItemTimer[curr_item])
		{
			vector<CAnimation*>* animations = m_pModelCom->Get_Animations();
			switch (curr_item)
			{
			case Client::CPlayer::PLAYER_ITEM_DAMAGE:
				// ���ݷ� �������
				m_pStatusCom->Set_Damage(5.f);
				break;
			case Client::CPlayer::PLAYER_ITEM_ATTACK_SPEED:
				// ���� �ӵ� �������
				for (auto anim_enum : m_Animation_NormalAttacks)
					(*animations)[anim_enum]->Set_PlaySpeed_Rate(1.25);
				break;
			case Client::CPlayer::PLAYER_ITEM_MOVE_SPEED:
				// �̵� �ӵ� �������
				m_pTransformCom->Set_Speed(g_PlayerDefaultMoveSpeed);
				break;
			}

			m_dItemTimer[i] = 0.0;
			m_bItem[i] = false;

			_vector current_rim_colir = XMLoadFloat4(&m_vRimColor);
			current_rim_colir -= XMLoadFloat4(&m_vRimColorEachItem[curr_item]);
			XMStoreFloat4(&m_vRimColor, current_rim_colir);
		}
		else
			check_all_item_false = false;
	}

	if (true == check_all_item_false)
	{
		m_isUsingRim = false;
		m_vRimColor = _float4(0.f);
	}

	Teleport_Dissolve(TimeDelta);


	// �� Ŭ���� ������Ʈ
	if (nullptr != m_pNavigationCom)
	{
		if (false == m_pNavigationCom->Get_Ignore())
			m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta, m_pTransformCom, m_pNavigationCom);
		else
			m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta, m_pTransformCom);
	}
	else
		m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta, m_pTransformCom);

	// �ൿ���� �ý��� ���� �ϰ�ó��
	m_pAction_Assistant->Run(TimeDelta);

	// �ݶ��̴� ������Ʈ
	_matrix transformation_matrix = XMLoadFloat4x4(m_BoneMatrixPtr.pCombinedMatrix) * 
		XMLoadFloat4x4(&m_SocketTransformMatrix) * m_pTransformCom->Get_WorldMatrix();

	m_pColliderCom->Update_Transform(transformation_matrix);
	m_pGameInstance->Add_Collider(string("Player_Hit"), m_pColliderCom);

	// ���� �� ����� ����Ʈ ������Ʈ
	m_pWallFinderCom->Update_Transform(transformation_matrix);
	_vector look_norm = _vec3norm(m_pTransformCom->Get_Look());
	XMStoreFloat3(&m_vWallFinder, m_pColliderCom->Get_Center() + (look_norm * m_fFinderDistance));
	m_pWallFinderCom->Set_Center(m_vWallFinder);

	// For Shadow
	auto LightDesc = CGameInstance::GetInstance()->Get_DirectionalLightDesc();
	XMStoreFloat4(&LightDesc->vPosition, m_pTransformCom->Get_Position());

	SORA_ANIM current_anim = (SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(SB_BODY);
	if (SORA_ANIM_RAGE_ATTACK5 == current_anim || SORA_ANIM_RAGE_ATTACK4 == current_anim)
		m_bUseMotionTrail = true;
	else
		m_bUseMotionTrail = false;



	return RESULT_NOPROBLEM;
}

_int CPlayer::LateTick(_double TimeDelta)
{
	if (true == PM->Get_MinigameWaiting())
		return RESULT_NOPROBLEM;

	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;

	// �浹 �˻�
	OnCollision();

	// ���� Ÿ�̸�(�׳� ���⼭ ���. ���� �ǹ� ����)
	if (0.0 < m_dGuardTimer)
	{
		m_dGuardTimer -= TimeDelta;

		if (0.0 > m_dGuardTimer)
			m_dGuardTimer = 0.0;
	}

	// �޺� Ÿ�̸�(�׳� ���⼭ ���. ���� �ǹ� ����)
	if (0.0 < m_dComboTimer)
	{
		m_dComboTimer -= TimeDelta;

		if (0.0 > m_dComboTimer)
		{
			m_dComboTimer = 0.0;
			m_iCombo = 0;

			m_iHammerForm_Combo = 0;

			// �޺� �ٸ� �����Ѵ�
			NM->Delete_ComboBar();
		}
	}

	// ���� Ÿ�̸�
	if (0.0 < m_dInvincibilityTimer)
	{
		m_dInvincibilityTimer -= TimeDelta * m_dInvincibilityTimer_Speed;

		if (0.0 > m_dInvincibilityTimer)
		{
			m_dInvincibilityTimer = 0.0;
			m_dInvincibilityTimer_Max = 0.0;
		}
	}

	// �ڽ� ���� ����
	if (0.0 < m_dSoundTimer_BoxOpen)
	{
		_uint target_frame = (_uint)m_dSoundTimer_BoxOpen_Duration - (_uint)m_dSoundTimer_BoxOpen;
		if (target_frame != m_iRecorded_SoundPlayedFrame)
		{
			// 0, 54, 59, 87
			LEVEL curr_level = GM->Get_CurrentLevel();

			if (0 == target_frame)
				Play_OpenChest(0);

			if (54 == target_frame)
				Play_OpenChest(1);

			if (59 == target_frame)
				Play_OpenChest(2);

			if (LEVEL_TWILIGHT != curr_level)
			{
				if (87 == target_frame)
					Play_OpenChest(3);
			}
			
			m_iRecorded_SoundPlayedFrame = target_frame;
		}

		_double anim_speed_open_box_big = m_pModelCom->Get_TargetPlaySpeed(SORA_ANIM::SORA_ANIM_OPEN_BIG);
		m_dSoundTimer_BoxOpen -= TimeDelta * anim_speed_open_box_big;

		if (0.0 >= m_dSoundTimer_BoxOpen)
		{
			m_dSoundTimer_BoxOpen = 0.0;
			m_iRecorded_SoundPlayedFrame = UINT_MAX;
		}
	}

	// Ÿ�� ��� ������Ʈ �� Ÿ�� ���� ��� ������ ����
	m_pCombat_Assistant->Run(TimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
	m_pRendererCom->Add_Debug(m_pColliderCom);
	m_pRendererCom->Add_Debug(m_pWallFinderCom);
	m_pRendererCom->Add_Debug(m_pNavigationCom);
#endif // _DEBUG

	return RESULT_NOPROBLEM;
}

HRESULT CPlayer::Render()
{
	if (nullptr == m_pModelCom)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumberOfMaterials();
	_uint iPass = m_iPass;

	vector<CModel::MESHDESC*> MeshDescVector;

	for (_uint i = 0; i < iNumMaterials; ++i) 
	{
		if (i == 6)
			iPass = 9;

		// ���� �����϶� ��Ǻ�
		else if (true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_ATK))
		{
			_bool air = m_pTransformCom->Get_JumpBoolean();
			_bool gravity_effect = m_pTransformCom->Get_GravityEffect();

			if (false == air || (true == air && false == gravity_effect))
				iPass = 15;
			else
				iPass = 0;
		}
		else
			iPass = 0;

		if (true == m_isUsingBlur)
			iPass = 6;

		if(true == m_isUsingRim)
			iPass = 12;

		if (SORA_ANIM_RAGE_JUMP_UP == (SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(SB_BODY) || true == m_bTeleportDissolve)
			iPass = 7;

		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_NORMALS, "g_NormalTexture");

		// ��� ���� �ƴ� �� �� ���¿� �´� �ؽ��ķ� ������
		Overwrite_OutfitTexture(i);
		
		if (true == m_bUseMotionTrail)
			m_pModelCom->Render(m_pShaderCom, iPass, i, "g_BoneMatrices", &MeshDescVector);
		else
			m_pModelCom->Render(m_pShaderCom, iPass, i, "g_BoneMatrices");
	}

	if (true == m_bUseMotionTrail)
	{
		CMotionTrail::MOTIONTRAILDESC tMotionTrailDesc;
		tMotionTrailDesc.iSize = 280;
		tMotionTrailDesc.pMeshDescVector = &MeshDescVector;
		tMotionTrailDesc.vColor = _float4(0.f, 0.f, 0.f, 1.f);
		tMotionTrailDesc.fRimPower = 0.3f;
		if (FAILED(m_pGameInstance->Add_GameObject(
			GM->Get_CurrentLevel(), TEXT("Layer_MotionTrail"), 
			TEXT("Prototype_GameObject_MotionTrail"), &tMotionTrailDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), 
			TEXT("Layer_MotionTrail"))->Get_Transform()->
			Set_WorldMatrix(m_pTransformCom->Get_WorldFloat4x4());

		MeshDescVector.clear();
	}

	m_pTransformCom->Set_OldWorldMatrixFloat4x4(m_pTransformCom->Get_WorldFloat4x4());

	_float fLUTMask = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fLUTMask", &fLUTMask, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

void CPlayer::PostRender(_double TimeDelta)
{
	ImGui::Begin("Player Control Panel");

	static _float anim_speed = 1.5f;
	static _bool all_animation = false;
	static _bool only_hammer_air_last = false;
	ImGui::DragFloat("Animation Speed", &anim_speed, 0.05f);
	ImGui::Checkbox("Change All Anim", &all_animation); ImGui::SameLine();
	ImGui::Checkbox("Only Hammer Last", &only_hammer_air_last);

	if (ImGui::Button("Change Speed"))
	{
		vector<CAnimation*>* animations = m_pModelCom->Get_Animations();
		if (true == only_hammer_air_last)
		{
			(*animations)[SORA_ANIM_HAMMER_AIR_LAST_ATTACK]->Set_PlaySpeed_Rate((_double)anim_speed);
		}
		else
		{
			if (true == all_animation)
			{
				for (auto animation : (*animations))
					animation->Set_PlaySpeed_Rate((_double)anim_speed);
			}
			else
			{
				for (_uint i = SORA_ANIM_HAMMER_ATTACK_01; i < SORA_ANIM_HAMMER_FORM; ++i)
					(*animations)[i]->Set_PlaySpeed_Rate((_double)anim_speed);
			}
		}
		
		// ���� �ִϸ��̼��� ��ũ�� ������� ������ ����.
		(*animations)[SORA_ANIM_HAMMER_FORM]->Set_PlaySpeed_Rate(1.25);
	}

	if (true == ImGui::CollapsingHeader("UI Rendering Control"))
	{
		_bool ui_render = GM->Get_UIRender();
		_bool rage_gauge = GM->Get_Rage_Gauge();

		ImGui::Checkbox("UI Render", &ui_render);
		ImGui::Checkbox("Render Rage Gauge", &rage_gauge);

		GM->Set_UIRender(ui_render);
		GM->Set_Rage_Gauge(rage_gauge);

		ImGui::Separator();

		ImGui::SliderInt("RageForm Gauge", &m_iRageForm_Gauge, 0, m_iRageForm_GaugeMax);
		ImGui::SliderInt("HammerForm Combo", &m_iHammerForm_Combo, 0, m_iHammerForm_ComboMax);

		_int combo = (_int)m_iCombo;
		ImGui::SliderInt("Attack Combo", &combo, 0, 1000);
		m_iCombo = (_uint)combo;

		for (_uint i = 0; i < FORM_FUNC_END; ++i)
		{
			if (m_pFormFunctionUI[i] == nullptr)
			{
				ImGui::Text("Form UI%d is nullptr", i);
				continue;
			}

			switch (FORM_FUNCTION(i))
			{
			case Client::CPlayer::FORM_FUNC_RAGEFORM:
				ImGui::Text("FUNC_RAGEFORM is not nullptr");
				break;
			case Client::CPlayer::FORM_FUNC_RAGE_FINISH:
				ImGui::Text("FORM_FUNC_RAGE_FINISH is not nullptr");
				break;
			case Client::CPlayer::FORM_FUNC_HAMMERFORM:
				ImGui::Text("FORM_FUNC_HAMMERFORM is not nullptr");
				break;
			case Client::CPlayer::FORM_FUNC_HAMMER_FINISH:
				ImGui::Text("FORM_FUNC_HAMMER_FINISH is not nullptr");
				break;
			}
		}
	}

	if (true == ImGui::CollapsingHeader("Blue and Rim Light Control"))
	{
		ImGui::Checkbox("Player Blur", &m_isUsingBlur);
		ImGui::Checkbox("Player Rim Light", &m_isUsingRim);

		ImGui::Separator();
		ImGui::Separator();

		ImGui::DragFloat("Power", &m_fRimPower, 0.05f);
		ImGui::DragFloat("Range", &m_fRimRange, 0.05f);

		_float rim_color[4] = { m_vRimColor.x, m_vRimColor.y, m_vRimColor.z, m_vRimColor.w };
		ImGui::DragFloat4("Color", rim_color, 0.05f);
		memcpy(&m_vRimColor, rim_color, sizeof(_float) * 4);
	}

	ImGui::Separator();
	ImGui::Separator();

	if (true == m_bControlMode)
		ImGui::Text("[ MODE : Control Mode ]");
	else
		ImGui::Text("[ MODE : ImGuI Mode ]");

	_bool blade_lock = GM->Get_KeyBladeLock();
	ImGui::Checkbox("Key Blade Lock", &blade_lock);
	GM->Set_KeyBladeLock(blade_lock);

	ImGui::Separator();

	ImGui::Checkbox("Form Change Lock", &m_bFormChangeLock);

	if (false == m_bFormChangeLock)
	{
		GM->Set_Slot_On_Donald(true);
		GM->Set_Slot_On_Goofy(true);
		GM->Set_Rage_Gauge(true);

		m_pStatusCom->Set_MP(m_pStatusCom->Get_MaxMP());
	}
	else
	{
		GM->Set_Slot_On_Donald(false);
		GM->Set_Slot_On_Goofy(false);
		GM->Set_Rage_Gauge(false);
	}

	ImGui::Separator();
	ImGui::Separator();

	_vector position = m_pTransformCom->Get_Position();
	ImGui::Text("[ Player Position : %.2f, %.2f, %.2f", vecx(position), vecy(position), vecz(position));
	ImGui::Text("[ Player MoveDir : %.2f, %.2f, %.2f", vecx(m_vMoveDir), vecy(m_vMoveDir), vecz(m_vMoveDir));

	ImGui::Separator();
	ImGui::Separator();
	
	for (_uint i = 0; i < PLAYER_SOUND_TYPE::PLAYER_SOUND_END; ++i)
	{
		switch (i)
		{
		case Client::CPlayer::PLAYER_SOUND_ACTION:
			ImGui::Text("[ PLAYER_SOUND_ACTION : %d ]", m_iPlayerSound[PLAYER_SOUND_ACTION]);
			break;
		case Client::CPlayer::PLAYER_SOUND_ATTACK:
			ImGui::Text("[ PLAYER_SOUND_ATTACK : %d ]", m_iPlayerSound[PLAYER_SOUND_ATTACK]);
			break;
		case Client::CPlayer::PLAYER_SOUND_LAST_AND_FINISH:
			ImGui::Text("[ PLAYER_SOUND_LAST_AND_FINISH : %d ]", m_iPlayerSound[PLAYER_SOUND_LAST_AND_FINISH]);
			break;
		case Client::CPlayer::PLAYER_SOUND_DAMAGED:
			ImGui::Text("[ PLAYER_SOUND_DAMAGED : %d ]", m_iPlayerSound[PLAYER_SOUND_DAMAGED]);
			break;
		case Client::CPlayer::PLAYER_SOUND_EFFECT:
			ImGui::Text("[ PLAYER_SOUND_EFFECT : %d ]", m_iPlayerSound[PLAYER_SOUND_EFFECT]);
			break;
		}
	}

	ImGui::Separator();
	ImGui::Separator();
	
	_float second_jump_cooldown = (_float)CState_Jump::g_SecondJump_CoolDown;
	ImGui::DragFloat("Second Jump CoolDown", &second_jump_cooldown, 0.05f, 0.f, 1.f);
	CState_Jump::g_SecondJump_CoolDown = (_double)second_jump_cooldown;

	ImGui::Separator();
	ImGui::Separator();

	_int monster_type = (_int)m_eMonsterType;
	ImGui::SliderInt("Monster Type", &monster_type, 0, MONSTERTYPE::MONSTERTYPE_END - 1);
	m_eMonsterType = (MONSTERTYPE)monster_type;

	switch (m_eMonsterType)
	{
	case Client::MONSTERTYPE_SKULLSOLDIER:
		ImGui::Text("Selected Type : SKULLSOLDIER");
		break;
	case Client::MONSTERTYPE_CROWSOLDIER:
		ImGui::Text("Selected Type : CROWSOLDIER");
		break;
	case Client::MONSTERTYPE_THINSOLDIER:
		ImGui::Text("Selected Type : THINSOLDIER");
		break;
	case Client::MONSTERTYPE_NIGHTMARE:
		ImGui::Text("Selected Type : NIGHTMARE");
		break;
	case Client::MONSTERTYPE_SHININGEGG:
		ImGui::Text("Selected Type : SHININGEGG");
		break;
	case Client::MONSTERTYPE_MONKEY:
		ImGui::Text("Selected Type : MONKEY");
		break;
	case Client::MONSTERTYPE_MOWER:
		ImGui::Text("Selected Type : MOWER");
		break;
	case Client::MONSTERTYPE_FLOWER:
		ImGui::Text("Selected Type : FLOWER");
		break;
	case Client::MONSTERTYPE_NOBODY:
		ImGui::Text("Selected Type : NOBODY");
		break;
	case Client::MONSTERTYPE_BIGTREE:
		ImGui::Text("Selected Type : BIGTREE");
		break;
	case Client::MONSTERTYPE_BOWMAN:
		ImGui::Text("Selected Type : BOWMAN");
		break;
	case Client::MONSTERTYPE_CANNON:
		ImGui::Text("Selected Type : CANNON");
		break;
	case Client::MONSTERTYPE_ICEMAN:
		ImGui::Text("Selected Type : ICEMAN");
		break;
	case Client::MONSTERTYPE_FROZENWOLF:
		ImGui::Text("Selected Type : FROZENWOLF");
		break;
	case Client::MONSTERTYPE_SUMMONWOLF:
		ImGui::Text("Selected Type : SUMMONWOLF");
		break;
	case Client::MONSTERTYPE_GOAT:
		ImGui::Text("Selected Type : GOAT");
		break;
	}

	ImGui::Separator();
	ImGui::Separator();

	if (true == ImGui::Button("Change_Weapon"))
		m_pWeapon->Plus_WeaponType();

	ImGui::SameLine();

	if (true == ImGui::Button("Sheathe_Weapon"))
		Sheathe_Weapon();

	ImGui::Separator();
	ImGui::Separator();

	ImGui::Checkbox("Animation Movement", &g_RatioControl);
	ImGui::DragFloat("Ratio XZ", &g_RatioXZ, 0.025f, 0.f, 2.f);
	ImGui::DragFloat("Ratio Y", &g_RatioY, 0.025f, 0.f, 2.f);

	ImGui::Separator();
	ImGui::Separator();

	ImGui::Text("Invincibility Timer : %.2lf / %.2lf", m_dInvincibilityTimer, m_dInvincibilityTimer_Max);

	ImGui::Separator();
	ImGui::Separator();

	ImGui::Checkbox("Solider Type", &g_SoldierType);

	ImGui::SameLine();

	if (true == ImGui::Button("Create Monster"))
	{
		_float4 vPosition = _float4(creation_point[0], creation_point[1], creation_point[2], 1.f);
		if (true == g_SoldierType)
		{
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_SkullSoldier"), TEXT("Prototype_GameObject_SkullSoldier"), &vPosition)))
				_breakpoint;
		}
		else
		{
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_SkullSoldier"), TEXT("Prototype_GameObject_CrowSoldier"), &vPosition)))
				_breakpoint;
		}
	}

	ImGui::Text("[ True = Skull Soldier  /  False = Crow Soldier");

	ImGui::Separator();

	ImGui::DragFloat3("Test Monster Creation Point", creation_point, 0.025f);

	ImGui::Separator();
	ImGui::Separator();

	CState* attack_state = m_pStateMachineCom->Get_State(CStateMachine::STATETYPE_ATK);
	if (nullptr == attack_state)
		ImGui::Text("There is no Attack State");
	else
	{
		ImGui::Text("There is a Attack State");

		_int attack_count = (_int)static_cast<CState_PlayerAttack*>(attack_state)->AttackCount();
		ImGui::Text("Attack Count : %d", attack_count);
	}

	ImGui::Separator();
	ImGui::Separator();

	_float form_duration = (_float)CState_PlayerForm::g_Form_Duration;
	ImGui::DragFloat("Form Duration", &form_duration, 0.05f, 0.f);
	CState_PlayerForm::g_Form_Duration = (_double)form_duration;

	CState* form_state = m_pStateMachineCom->Get_State(CStateMachine::STATETYPE_FORM);
	if (nullptr == form_state)
	{
		ImGui::Text("[ Sora is on Normal State ]");
		ImGui::Text("[ Form Change Timer || %.2lf / %.2lf ]", 0.0, 0.0);
	}
	else
	{
		switch (m_eForm)
		{
		case Client::CPlayer::SF_NORMAL:
			ImGui::Text("[ Sora is on Normal Form State ]");
			break;
		case Client::CPlayer::SF_HAMMER:
			ImGui::Text("[ Sora is on Hammer Form State ]");
			break;
		case Client::CPlayer::SF_RAGE:
			ImGui::Text("[ Sora is on Rage Form State ]");
			break;
		}

		_double form_time_acc = static_cast<CState_PlayerForm*>(form_state)->Get_FormTimer();
		_double form_duration = static_cast<CState_PlayerForm*>(form_state)->g_Form_Duration;
		ImGui::Text("[ Form Change Timer || %.2lf / %.2lf ]", form_time_acc, form_duration);
	}

	ImGui::Separator();
	ImGui::Separator();

	if (true == ImGui::CollapsingHeader("KnockBack Activation"))
	{
		static _float knockback_dir[3] = { 0.f ,0.f, 0.f };
		static _float knockback_power = 0.f;
		static _float knockback_timer = 0.f;

		ImGui::DragFloat3("KnockBack Direction", knockback_dir, 0.025f);
		ImGui::DragFloat("KnockBack Power", &knockback_power, 0.025f);
		ImGui::DragFloat("KnockBack Timer", &knockback_timer, 0.025f);

		static _float3 dir_f3 = _float3(0.f);
		dir_f3.x = knockback_dir[0];
		dir_f3.y = knockback_dir[1];
		dir_f3.z = knockback_dir[2];

		if (true == ImGui::Button("Front Dir"))
		{
			_vector look = m_pTransformCom->Get_Look();

			XMStoreFloat3(&dir_f3, look);
			memcpy(knockback_dir, &look, sizeof(_float) * 3);
		}

		ImGui::SameLine();

		if (true == ImGui::Button("Back Dir"))
		{
			_vector look = m_pTransformCom->Get_Look() * -1.f;

			XMStoreFloat3(&dir_f3, look);
			memcpy(knockback_dir, &look, sizeof(_float) * 3);
		}

		if (true == ImGui::Button("Left Dir"))
		{
			_vector look = m_pTransformCom->Get_Right() * -1.f;

			XMStoreFloat3(&dir_f3, look);
			memcpy(knockback_dir, &look, sizeof(_float) * 3);
		}

		ImGui::SameLine();

		if (true == ImGui::Button("Right Dir"))
		{
			_vector look = m_pTransformCom->Get_Right();

			XMStoreFloat3(&dir_f3, look);
			memcpy(knockback_dir, &look, sizeof(_float) * 3);
		}

		ImGui::Text("Ready Direction : %.2f, %.2f, %.2f", dir_f3.x, dir_f3.y, dir_f3.z);

		static _bool look_at_negative_dir = false;
		ImGui::Checkbox("Look Negative Direction", &look_at_negative_dir);

		if (ImGui::Button("KnockBack"))
		{
			_vector knockback_direction = _vec3norm(XMLoadFloat3(&dir_f3));

			if (true == look_at_negative_dir)
				knockback_direction *= -1.f;

			if (false == XMVector3Equal(XMVectorZero(), knockback_direction))
				m_pTransformCom->MakeSameDirection(knockback_direction);
			else
				_bool bDebug = true;

			m_pTransformCom->KnockBackInput(dir_f3, knockback_power, (_double)knockback_timer);
		}
	}
	
	if (true == ImGui::CollapsingHeader("KnockBack Info"))
	{
		CTransform::KNOCKBACK_DESC knockback_desc = m_pTransformCom->Get_KnockBackDesc();

		_float3 direction = knockback_desc.vDirection;
		ImGui::Text("[ Direction : %.2f, %.2f, %.2f ]", direction.x, direction.y, direction.z);

		_float input_power = knockback_desc.fInputPower;
		_float power = knockback_desc.fPower;

		_double input_time = knockback_desc.dInputTimer;
		_double time = knockback_desc.dTimer;

		ImGui::Text("[ Input Power : %.2f ]", input_power);
		ImGui::Text("[ Power : %.2f ]", power);

		ImGui::Text("[ Input Time : %.2lf ]", input_time);
		ImGui::Text("[ Time : %.2lf ]", time);
	}

	if (true == ImGui::CollapsingHeader("Navigation Control"))
	{
		ImGui::Text("Climb Timer : %.2lf", m_dTimer_ToStartFreeRunning);
		ImGui::Text("Angle Between Wall : %.2f", m_fAngle_Between_Wall);

		if (true == m_pNavigationCom->Get_Climb())
			ImGui::Text("Climb Boolean is [ True ]");
		else
			ImGui::Text("Climb Boolean is [ False ]");
		
		// ===================================================================================
		_bool ignore_navigation = m_pNavigationCom->Get_Ignore();

		ImGui::Checkbox("Ignore Navigation", &ignore_navigation);
		m_pNavigationCom->Set_Ignore(ignore_navigation);
		// ===================================================================================

		// �÷��̾ ���� �ӹ��� ���� �ε��� ���
		_uint cell_index_player_stay = m_pNavigationCom->Get_CurrentIndex();
		ImGui::Text("Cell Index Player Stay Currently: %d", (_int)cell_index_player_stay);

		// �÷��̾� ���� ���� �ε��� ���
		_uint cell_index_previous = m_pNavigationCom->Get_PreviousIndex();
		ImGui::Text("Cell Index Player Located before : %d", (_int)cell_index_previous);

		ImGui::Separator(); ImGui::Separator();

		// ���� �� ��ü�� ����
		CCell* current_cell = m_pNavigationCom->Get_CurrentCell();
		if (nullptr == current_cell)
		{
			BREAKPOINT;
			return;
		}

		// ���� �� ������ ��� CollapsingHeader
		if (true == ImGui::CollapsingHeader("Current Cell Info"))
		{
			// ���� �� Ÿ��
			CCell::TYPE line = current_cell->Get_Type();
			switch (line)
			{
			case Engine::CCell::TYPE_TERRAIN:
				ImGui::Text("[ Current Cell Type : TYPE_TERRAIN ]");
				break;
			case Engine::CCell::TYPE_ROAD:
				ImGui::Text("[ Current Cell Type : TYPE_ROAD ]");
				break;
			case Engine::CCell::TYPE_NOTROAD:
				ImGui::Text("[ Current Cell Type : TYPE_NOTROAD ]");
				break;
			case Engine::CCell::TYPE_RUNNING:
				ImGui::Text("[ Current Cell Type : TYPE_RUNNING ]");
				break;
			case Engine::CCell::TYPE_FLOW:
				ImGui::Text("[ Current Cell Type : TYPE_FLOW ]");
				break;
			}

			// ���� �� �ε���
			ImGui::Text("[ Current Cell Index Number : %d", current_cell->Get_Index());
			ImGui::Separator(); ImGui::Separator();

			// ���� ���� �����ϴ� 3���� ����Ʈ ����
			_vector point_a = current_cell->Get_Point(CCell::POINT_A);
			_vector point_b = current_cell->Get_Point(CCell::POINT_B);
			_vector point_c = current_cell->Get_Point(CCell::POINT_C);

			ImGui::Text("[ Point A of Cell : %.2f, %.2f, %.2f ]", vecx(point_a), vecy(point_a), vecz(point_a));
			ImGui::Text("[ Point B of Cell : %.2f, %.2f, %.2f ]", vecx(point_b), vecy(point_b), vecz(point_b));
			ImGui::Text("[ Point C of Cell : %.2f, %.2f, %.2f ]", vecx(point_c), vecy(point_c), vecz(point_c));

			// �� ������ ���� �ǳ��� �̿��� �ε��� ����
			ImGui::Text("[ Neighbor Index opposite LineAB : %d ]", current_cell->Get_NeighborIndex(CCell::LINE_AB));
			ImGui::Text("[ Neighbor Index opposite LineBC : %d ]", current_cell->Get_NeighborIndex(CCell::LINE_BC));
			ImGui::Text("[ Neighbor Index opposite LineCA : %d ]", current_cell->Get_NeighborIndex(CCell::LINE_CA));

			_float3 normal_vector = current_cell->Get_NormalVector();
			ImGui::Text("[ Normal Vector of This Cell : %.2f, %.2f, %.2f", normal_vector.x, normal_vector.y, normal_vector.z);

			// ���� ����� ������ Ÿ�԰� �ش� ���α����� �Ÿ��� �����´�
			CCell::LINES closest_line = CCell::LINES::LINE_END;
			_float distance_to_line = current_cell->Get_CloseLine(position, &closest_line);

			if (CCell::LINES::LINE_END == closest_line)
			{
				BREAKPOINT;
				return;
			}

			// ���� ����� ������ ���� ���
			_vector line_pointA, line_pointB;
			line_pointA = line_pointB = XMVectorZero();

			switch (closest_line)
			{
			case Engine::CCell::LINE_AB:
				ImGui::Text("[ Closest Line :: LINE_AB ]");
				line_pointA = current_cell->Get_Point(CCell::POINT_A);
				line_pointB = current_cell->Get_Point(CCell::POINT_B);

				break;
			case Engine::CCell::LINE_BC:
				ImGui::Text("[ Closest Line :: LINE_BC ]");
				line_pointA = current_cell->Get_Point(CCell::POINT_B);
				line_pointB = current_cell->Get_Point(CCell::POINT_C);

				break;
			case Engine::CCell::LINE_CA:
				ImGui::Text("[ Closest Line :: LINE_CA ]");
				line_pointA = current_cell->Get_Point(CCell::POINT_C);
				line_pointB = current_cell->Get_Point(CCell::POINT_A);

				break;
			}

			ImGui::Text("[ Distance to Line : %.2f ]", distance_to_line);
			ImGui::Text("Closest Line Point A : %.2f, %.2f, %.2f", vecx(line_pointA), vecy(line_pointA), vecz(line_pointA));
			ImGui::Text("Closest Line Point B : %.2f, %.2f, %.2f", vecx(line_pointB), vecy(line_pointB), vecz(line_pointB));

			ImGui::Separator(); ImGui::Separator();

			ImGui::Text(":: Check Upper Side ::");

			if (true == current_cell->isUpperSide(position))
				ImGui::Text("Player is <UpperSide> of the Cell");
			else
				ImGui::Text("Player is <UnderSide> of the Cell");
		}

		ImGui::Separator(); ImGui::Separator();

		if (true == ImGui::CollapsingHeader("Input Index Cell Info"))
		{
			// ã�����ϴ� �ε����� �Է¹���
			static _int input_index = 0;
			_uint nummber_of_cells = m_pNavigationCom->Get_NumberOfCell();

			ImGui::InputInt("Input Index", &input_index);

			// Ŭ����, ����ó��
			if (0 > input_index)
				input_index = 0;
			else if (input_index > (_int)(nummber_of_cells - 1))
				input_index = nummber_of_cells - 1;

			CCell* input_cell = m_pNavigationCom->Get_InputCell((_uint)input_index);
			if (nullptr == input_cell)
			{
				BREAKPOINT;
				return;
			}

			CCell::TYPE cell_type = input_cell->Get_Type();
			switch (cell_type)
			{
			case Engine::CCell::TYPE_TERRAIN:
				ImGui::Text("[ Input Cell Type : TYPE_TERRAIN ]");
				break;
			case Engine::CCell::TYPE_ROAD:
				ImGui::Text("[ Input Cell Type : TYPE_ROAD ]");
				break;
			case Engine::CCell::TYPE_NOTROAD:
				ImGui::Text("[ Input Cell Type : TYPE_NOTROAD ]");
				break;
			case Engine::CCell::TYPE_RUNNING:
				ImGui::Text("[ Input Cell Type : TYPE_RUNNING ]");
				break;
			case Engine::CCell::TYPE_FLOW:
				ImGui::Text("[ Input Cell Type : TYPE_FLOW ]");
				break;
			}

			// ���� ���� �����ϴ� 3���� ����Ʈ ����
			_vector point_a = input_cell->Get_Point(CCell::POINT_A);
			_vector point_b = input_cell->Get_Point(CCell::POINT_B);
			_vector point_c = input_cell->Get_Point(CCell::POINT_C);

			ImGui::Text("[ Point A of Cell : %.2f, %.2f, %.2f ]", vecx(point_a), vecy(point_a), vecz(point_a));
			ImGui::Text("[ Point B of Cell : %.2f, %.2f, %.2f ]", vecx(point_b), vecy(point_b), vecz(point_b));
			ImGui::Text("[ Point C of Cell : %.2f, %.2f, %.2f ]", vecx(point_c), vecy(point_c), vecz(point_c));

			// �� ������ ���� �ǳ��� �̿��� �ε��� ����
			ImGui::Text("[ Neighbor Index opposite LineAB : %d ]", input_cell->Get_NeighborIndex(CCell::LINE_AB));
			ImGui::Text("[ Neighbor Index opposite LineBC : %d ]", input_cell->Get_NeighborIndex(CCell::LINE_BC));
			ImGui::Text("[ Neighbor Index opposite LineCA : %d ]", input_cell->Get_NeighborIndex(CCell::LINE_CA));

			_float3 normal_vector = input_cell->Get_NormalVector();
			ImGui::Text("[ Normal Vector of This Cell : %.2f, %.2f, %.2f", normal_vector.x, normal_vector.y, normal_vector.z);

			// ���� ����� ������ Ÿ�԰� �ش� ���α����� �Ÿ��� �����´�
			CCell::LINES line = CCell::LINES::LINE_END;
			_float distance_to_line = input_cell->Get_CloseLine(position, &line);

			if (CCell::LINES::LINE_END == line)
			{
				BREAKPOINT;
				return;
			}

			// ���� ����� ������ ���� ���
			_vector line_pointA, line_pointB;
			line_pointA = line_pointB = XMVectorZero();

			switch (line)
			{
			case Engine::CCell::LINE_AB:
				ImGui::Text("[ Closest Line :: LINE_AB ]");
				line_pointA = input_cell->Get_Point(CCell::POINT_A);
				line_pointB = input_cell->Get_Point(CCell::POINT_B);

				break;
			case Engine::CCell::LINE_BC:
				ImGui::Text("[ Closest Line :: LINE_BC ]");
				line_pointA = input_cell->Get_Point(CCell::POINT_B);
				line_pointB = input_cell->Get_Point(CCell::POINT_C);

				break;
			case Engine::CCell::LINE_CA:
				ImGui::Text("[ Closest Line :: LINE_CA ]");
				line_pointA = input_cell->Get_Point(CCell::POINT_C);
				line_pointB = input_cell->Get_Point(CCell::POINT_A);

				break;
			}

			ImGui::Text("[ Distance to Line : %.2f ]", distance_to_line);
			ImGui::Text("Closest Line Point A : %.2f, %.2f, %.2f", vecx(line_pointA), vecy(line_pointA), vecz(line_pointA));
			ImGui::Text("Closest Line Point B : %.2f, %.2f, %.2f", vecx(line_pointB), vecy(line_pointB), vecz(line_pointB));

			ImGui::Separator(); ImGui::Separator();

			ImGui::Text(":: Check Upper Side ::");

			if (true == input_cell->isUpperSide(position))
				ImGui::Text("Player is <UpperSide> of Input Cell");
			else
				ImGui::Text("Player is <UnderSide> of Input Cell");
		}
	}


	static _bool show_animation_control = false;
	static _bool show_collider_control = false;
	static _bool show_statemachine_control = false;
	static _bool show_damagestate_control = false;
	static _bool show_combat_assistant_control = false;
	static _bool show_action_assistant_control = false;

	ImGui::Checkbox("Animation Control", &show_animation_control); ImGui::SameLine();
	ImGui::Checkbox("Collider Control", &show_collider_control);

	ImGui::Checkbox("StateMachine Control", &show_statemachine_control); ImGui::SameLine();
	ImGui::Checkbox("DamageState Control", &show_damagestate_control);

	ImGui::Checkbox("Combat Assistant Control", &show_combat_assistant_control); ImGui::SameLine();
	ImGui::Checkbox("Action Assistant Control", &show_action_assistant_control);

	if (show_animation_control)
		Control_Animation();

	if (show_collider_control)
		Control_Collider();

	//Control_Jump();
	
	if (show_statemachine_control)
		Control_StateMachine();

	if (show_damagestate_control)
		Control_DamageState();

	if (show_combat_assistant_control)
		Control_CombatAssistant();

	if (show_action_assistant_control)
		Control_ActionAssistant();
	

	ImGui::End();
}

HRESULT CPlayer::Render_Shadow()
{
	if (nullptr == m_pModelCom)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	_float4x4 LightViewMatrix = pGameInstance->Get_LightMatrix(CLight_Manager::SHADOW_DYNAMIC).LightViewMatrix;
	if (FAILED(m_pShaderCom->Set_RawValue("g_LightViewMatrix", &LightViewMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float4x4 LightProjMatrix = pGameInstance->Get_LightMatrix(CLight_Manager::SHADOW_DYNAMIC).LightProjMatrix;
	if (FAILED(m_pShaderCom->Set_RawValue("g_LightProjMatrix", &LightProjMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumberOfMaterials();
	_uint iPass = 14;

	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		m_pModelCom->Render(m_pShaderCom, iPass, i, "g_BoneMatrices");
	}

	return S_OK;
}

HRESULT CPlayer::Save_ObjectData(HANDLE hFile)
{
	if (hFile == INVALID_HANDLE_VALUE)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// ������� ����
	_float4x4 worldMatrix;
	XMStoreFloat4x4(&worldMatrix, m_pTransformCom->Get_WorldMatrix());

	DWORD dwBytes = 0;
	WriteFile(hFile, &worldMatrix, sizeof(_float4x4), &dwBytes, nullptr);

	return S_OK;
}

HRESULT CPlayer::Load_ObjectData(HANDLE hFile)
{
	if (hFile == INVALID_HANDLE_VALUE)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// ���̺굥���Ϳ��� ��������� �ε�
	DWORD dwBytes = 0;
	_float4x4 worldMatrix;

	ReadFile(hFile, &worldMatrix, sizeof(_float4x4), &dwBytes, nullptr);

	// ������� ����
	m_pTransformCom->Set_WorldMatrix(worldMatrix);

	return S_OK;
}

void CPlayer::Change_PlayerAnimation(SORA_ANIM eSoraAnim)
{
	m_eSoraAnim[SORA_BODY::SB_BODY] = eSoraAnim;
	m_pModelCom->Set_TempIndex_EachBody(SORA_BODY::SB_BODY, eSoraAnim);

	if (true == Check_DrawAnim(eSoraAnim))
		Draw_Weapon();
}

_bool CPlayer::Check_OnFinish()
{
	if (SORA_ANIM_RAGE_FINISH == m_eSoraAnim[SB_BODY] ||
		SORA_ANIM_HAMMER_FINISH == m_eSoraAnim[SB_BODY])
		return true;

	return false;
}

void CPlayer::GetOnNavigation()
{
	if (LEVEL_RAPUNZEL_BOSS != GM->Get_CurrentLevel())
		m_pTransformCom->Set_PositionY(m_pNavigationCom->Compute_Y(m_pTransformCom->Get_Position()));
	else
	{
		_vector vPosition = static_cast<CTerrain*>(m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Terrain")))->Compute_PositionOnTerrain(m_pTransformCom->Get_Position());
		m_pTransformCom->Set_Position(vPosition);
	}
}

_bool CPlayer::Check_OnGround()
{
	LEVEL curr_level = GM->Get_CurrentLevel();

	_vector curr_player_position = m_pTransformCom->Get_Position();
	_float compare_height = FLT_MAX;

	_bool result_value = false;
	switch (curr_level)
	{
	case LEVEL_TWILIGHT:
	case LEVEL_RAPUNZEL:
	case LEVEL_FROZEN:
	case LEVEL_KEYGRAVE:
	case LEVEL_FINALBOSS:
	case LEVEL_TEST:

		compare_height = m_pNavigationCom->Compute_Y(curr_player_position);
		if (compare_height == vecy(curr_player_position))
			result_value = true;
		else
			result_value = false;

		break;
	case LEVEL_RAPUNZEL_BOSS:

		CTerrain* terrain = static_cast<CTerrain*>(m_pGameInstance->Get_Front(LEVEL_RAPUNZEL_BOSS, TEXT("Layer_Terrain")));

		if (nullptr == terrain)
			_breakpoint;

		compare_height = vecy(terrain->Compute_PositionOnTerrain(curr_player_position));
		if (compare_height == vecy(curr_player_position))
			result_value = true;
		else
			result_value = false;

		break;
	}

	return result_value;
}

void CPlayer::Create_PlayerState(_uint iStateType, void* pArg)
{
	switch (iStateType)
	{
	case Engine::CStateMachine::STATETYPE_IDLE:

		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_IDLE, string("State_Idle"), CState_PlayerIdle::Create(pArg));

		break;
	case Engine::CStateMachine::STATETYPE_NORMALMOVE:

		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_NORMALMOVE, string("State_Move"), CState_PlayerMove::Create(pArg), true);

		break;
	case Engine::CStateMachine::STATETYPE_DYNAMICMOVE:

		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_DYNAMICMOVE, string("State_DynamicMove"), CState_PlayerDynamicMove::Create(CState_PlayerDynamicMove::MOVE_TYPE_RUN_WALL));

		break;
	case Engine::CStateMachine::STATETYPE_JUMP:

		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_JUMP, string("State_Jump"), CState_Jump::Create(pArg));

		break;
	case Engine::CStateMachine::STATETYPE_ATK:

		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_ATK, string("State_Attack"), CState_PlayerAttack::Create(pArg));

		break;
	case Engine::CStateMachine::STATETYPE_SUMMON:

		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_SUMMON, string("State_Summon"), CState_PlayerSummon::Create(pArg));

		break;
	case Engine::CStateMachine::STATETYPE_GUARD:

		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_GUARD, string("State_Guard"), CState_PlayerGuard::Create(pArg));

		break;
	case Engine::CStateMachine::STATETYPE_AVOID:

		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_AVOID, string("State_Avoid"), CState_PlayerAvoid::Create(pArg));

		break;
	case Engine::CStateMachine::STATETYPE_DAMAGED:

		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_DAMAGED, string("State_Damaged"), CState_PlayerDamaged::Create(pArg), true);

		break;
	case Engine::CStateMachine::STATETYPE_INTERACTION:

		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_INTERACTION, string("State_Interaction"), CState_PlayerInteraction::Create(pArg));

		break;
	case Engine::CStateMachine::STATETYPE_FORM:

		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_FORM, string("State_Form"), CState_PlayerForm::Create(pArg));

		break;
	}
}

_bool CPlayer::Check_PlayerState(CStateMachine::STATETYPE _stateType)
{
	if (nullptr == m_pStateMachineCom)
		return false;

	if (true == m_pStateMachineCom->FindState(_stateType))
		return true;
	else
		return false;
}

HRESULT CPlayer::MakeState_Idle()
{
	switch (m_eForm)
	{
	case Client::CPlayer::SF_NORMAL:

		// ��� ���� ���� IDLE�� ������ ��� ���¸� �����Ѵ�
		for (_uint i = 0; i < CStateMachine::STATETYPE_END; ++i)
		{
			CStateMachine::STATETYPE state_type = (CStateMachine::STATETYPE)i;
			if (CStateMachine::STATETYPE_IDLE == state_type)
				continue;

			m_pStateMachineCom->ClearState(state_type);
		}

		break;
	case Client::CPlayer::SF_HAMMER:
	case Client::CPlayer::SF_RAGE:

		// ��� ���� �ƴ� Ư�� ���� ���� FORM�� IDLE�� ������ ��� ���¸� �����Ѵ�
		for (_uint i = 0; i < CStateMachine::STATETYPE_END; ++i)
		{
			CStateMachine::STATETYPE state_type = (CStateMachine::STATETYPE)i;
			if (CStateMachine::STATETYPE_IDLE == state_type || CStateMachine::STATETYPE_FORM == state_type)
				continue;

			m_pStateMachineCom->ClearState(state_type);
		}

		break;
	}
	
	// ��� Able���� true�� ����
	for (_uint i = 0; i < CAction_Assistant::ASSIST_TYPE::ASSIST_END; ++i)
		m_pAction_Assistant->Able((CAction_Assistant::ASSIST_TYPE)i, true);

	return S_OK;
}

_uint CPlayer::Get_AttackCount()
{
	// nullptr üũ
	if (false == m_pStateMachineCom)
	{
		BREAKPOINT;
		return 0;
	}

	// ���� ���� ���� ��� üũ
	if (false == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_ATK))
	{
		BREAKPOINT;
		return 0;
	}

	return static_cast<CState_PlayerAttack*>(m_pStateMachineCom->Get_State(CStateMachine::STATETYPE_ATK))->AttackCount();
}

void CPlayer::Add_Combo()
{
	m_iCombo++;
	m_dComboTimer = m_dComboTimer_Limit;

	if (SORA_FORM::SF_NORMAL == m_eForm)
	{
		CPlayer_Weapon::WEAPON_TYPE weapon_type = m_pWeapon->WeaponType();
		switch (weapon_type)
		{
		case Client::CPlayer_Weapon::WEAPON_TYPE_KINGDOM_KEY:

			Raise_RageGauge();

			break;
		case Client::CPlayer_Weapon::WEAPON_TYPE_FAVORITE_DEPUTY:

			Raise_HammerCombo();

			break;
		}
	}

	// �޺� �� ����
	NM->Update_ComboUI_Effect();
}

_bool CPlayer::Anim_Check_PlusJumpChain(_uint iAnimIndex)
{
	if (0 >= m_Animation_PlusJumpChain.size())
	{
		_breakpoint;
		return false;
	}

	_bool result = false;

	for (auto anim_enum : m_Animation_PlusJumpChain)
	{
		if (anim_enum == (SORA_ANIM)iAnimIndex)
		{
			result = true;
			break;
		}
	}

	return result;
}

_bool CPlayer::Anim_Check_HoveringStart(_uint iAnimIndex)
{
	if (0 >= m_Animation_HoveringStart.size())
	{
		_breakpoint;
		return false;
	}

	_bool result = false;

	for (auto anim_enum : m_Animation_HoveringStart)
	{
		if (anim_enum == (SORA_ANIM)iAnimIndex)
		{
			result = true;
			break;
		}
	}

	return result;
}

_bool CPlayer::Anim_Check_AbleToSecondJump(_uint iAnimIndex)
{
	if (0 >= m_Animation_AbleToSecondJump.size())
	{
		_breakpoint;
		return false;
	}

	_bool result = false;

	for (auto anim_enum : m_Animation_AbleToSecondJump)
	{
		if (anim_enum == (SORA_ANIM)iAnimIndex)
		{
			result = true;
			break;
		}
	}

	return result;
}

_bool CPlayer::Anim_Check_LandOnGround(_uint iAnimIndex)
{
	if (0 >= m_Animation_LandOnGround.size())
	{
		_breakpoint;
		return false;
	}

	_bool result = false;

	for (auto anim_enum : m_Animation_LandOnGround)
	{
		if (anim_enum == (SORA_ANIM)iAnimIndex)
		{
			result = true;
			break;
		}
	}

	return result;
}

_bool CPlayer::Anim_Check_GuardSucceeded(_uint iAnimIndex)
{
	if (0 >= m_Animation_GuardSucceeded.size())
	{
		_breakpoint;
		return false;
	}

	_bool result = false;

	for (auto anim_enum : m_Animation_GuardSucceeded)
	{
		if (anim_enum == (SORA_ANIM)iAnimIndex)
		{
			result = true;
			break;
		}
	}

	return result;
}

_bool CPlayer::Anim_Check_MoveOnAxisY(_uint iAnimIndex)
{
	if (0 >= m_Animation_MoveOnAxisY.size())
	{
		_breakpoint;
		return false;
	}

	_bool result = false;

	for (auto anim_enum : m_Animation_MoveOnAxisY)
	{
		if (anim_enum == (SORA_ANIM)iAnimIndex)
		{
			result = true;
			break;
		}
	}

	return result;
}

_bool CPlayer::Anim_Check_LastCombo(_uint iAnimIndex)
{
	if (0 >= m_Animation_LastCombo.size())
	{
		_breakpoint;
		return false;
	}

	_bool result = false;

	for (auto anim_enum : m_Animation_LastCombo)
	{
		if (anim_enum == (SORA_ANIM)iAnimIndex)
		{
			result = true;
			break;
		}
	}

	return result;
}

_bool CPlayer::Anim_Check_NormalAttacks(_uint iAnimIndex)
{
	if (0 >= m_Animation_NormalAttacks.size())
	{
		_breakpoint;
		return false;
	}

	_bool result = false;

	for (auto anim_enum : m_Animation_NormalAttacks)
	{
		if (anim_enum == (SORA_ANIM)iAnimIndex)
		{
			result = true;
			break;
		}
	}

	return result;
}

_float CPlayer::Calculate_GapY_Between_PositionAndCenter()
{
	if (nullptr == m_pColliderCom)
	{
		_breakpoint;
		return 0.f;
	}

	_vector gap = m_pColliderCom->Get_Center() - m_pTransformCom->Get_Position();
	return vecy(gap);
}

void CPlayer::Invincibility_Input(_double dDuration, _double dSpeed)
{
	m_dInvincibilityTimer = dDuration;
	m_dInvincibilityTimer_Max = dDuration;

	m_dInvincibilityTimer_Speed = dSpeed;
}

void CPlayer::Attacked_By_PillarOfFire(_fvector vPillaorPosition)
{
	// ���� �ð� �������� ���� return
	if (0.0 < m_dInvincibilityTimer)
		return;

	// �ǰ� ���� ����
	CTransform::DAMAGEDDIR damaged_dir = m_pTransformCom->Get_Damaged_4Dir(vPillaorPosition);
	Select_DamagedAnimation(damaged_dir);

	// �ִϸ��̼� ����
	SORA_ANIM temp_index = (SORA_ANIM)m_pModelCom->Get_TempIndex_EachBody(SORA_BODY::SB_BODY);
	m_pModelCom->ResetAnimation((_uint)temp_index);

	// �ִϸ��̼� ����
	CState_PlayerDamaged::PLAYER_DAMAGED_DESC damaged_desc;
	damaged_desc.sora_anim = temp_index;

	// �ǰ� ���� ��ü ����
	Create_PlayerState(CStateMachine::STATETYPE::STATETYPE_DAMAGED, &damaged_desc);

	// ������ ����
	_float current_hp = m_pStatusCom->Get_HP();
	m_pStatusCom->Set_HP(current_hp - 5.f);

	// �ǰݽ� ȭ�� ������ �Ǵ� UI
	GM->HitBlood();

	// ���� �ð� ����
	_double damaged_anim_duration = m_pModelCom->Get_TargetDuration(temp_index);
	_double damaged_anim_play_speed = m_pModelCom->Get_TargetPlaySpeed(temp_index);

	if (0.0 == m_dInvincibilityTimer)
		Invincibility_Input(damaged_anim_duration * 1.5, damaged_anim_play_speed);
}

void CPlayer::Attacked_By_Teleport()
{
	// ���� ����
	m_pTransformCom->BasicJump(CTransform::JUMP_FALL);

	// ���� ���� ����
	CState_Jump::JUMP_STATE_DESC jump_state_desc;
	jump_state_desc.eObjectType = m_eObjectType;
	jump_state_desc.iJumpAnimIndex = 2;
	jump_state_desc.iNumberOfJumpAnim = 4;

	Create_PlayerState(CStateMachine::STATETYPE_JUMP, &jump_state_desc);

	// ȣ���� ���� true
	m_bHovering = true;
	m_bHovering_MoveAnimation = true;

	CState_Jump* jump_state = static_cast<CState_Jump*>(m_pStateMachineCom->Get_State(CStateMachine::STATETYPE_JUMP));

	if (nullptr == jump_state)
		_breakpoint;

	// ���� �ִϸ��̼� ü�� ����
	jump_state->SetUp_AnimationChain();
}

void CPlayer::Draw_Weapon()
{
	if (true == m_bHoldingWeapon)
		return;

	m_bHoldingWeapon = true;
	m_pWeapon->WeaponState(CPlayer_Weapon::WEAPON_STATE_DRAW);

	if (SORA_ANIM_DRAW_BLADE == m_eSoraAnim[SB_BODY])
	{
		CEffect_Manager::GetInstance()->Create_SwordEffect(m_pWeapon->Get_Transform()->Get_WorldFloat4x4_Pointer());
		CEffect_Manager::GetInstance()->Create_SwordEffectParticle(m_pWeapon->Get_Transform());

		CEffect_Manager::GetInstance()->Create_ReadyToAttackEffect(m_pTransformCom);
	}
}

void CPlayer::Set_AttackTarget(CCollider * _attackTarget)
{
	if (nullptr == m_pCombat_Assistant)
	{
		BREAKPOINT;
		return;
	}

	m_pCombat_Assistant->Setup_Target(C_ASSIST::ASSIST_TYPE::ASSIST_ATTACK, _attackTarget);
}

void CPlayer::Dead_FormUI(FORM_FUNCTION _uiType)
{
	if (nullptr == m_pFormFunctionUI[_uiType])
		return;

	if (FORM_FUNCTION::FORM_FUNC_COUNTER_ATTACK == _uiType)
		m_pFormFunctionUI[_uiType]->Set_Dead(true);
	else
		static_cast<CUI_FormChange*>(m_pFormFunctionUI[_uiType])->Set_Case0(true);
}

void CPlayer::Null_FormUI(FORM_FUNCTION _uiType)
{
	m_pFormFunctionUI[_uiType] = nullptr;
}

CGameObject * CPlayer::Get_FormUI(FORM_FUNCTION _uiType)
{
	return m_pFormFunctionUI[_uiType];
}

void CPlayer::Raise_RageGauge()
{
	m_iRageForm_Gauge += m_iRageForm_GaugeStep;

	if (m_iRageForm_Gauge > m_iRageForm_GaugeMax)
		m_iRageForm_Gauge = m_iRageForm_GaugeMax;
}

void CPlayer::Empty_RageGauge()
{
	m_iRageForm_Gauge = 0;
}

_float CPlayer::Get_RageForm_GaugeRate()
{
	_float result = 0.f;

	CState_PlayerForm* form_state = static_cast<CState_PlayerForm*>(m_pStateMachineCom->Get_State(CStateMachine::STATETYPE_FORM));

	if (nullptr == form_state)
	{
		result = ((_float)m_iRageForm_Gauge / (_float)m_iRageForm_GaugeMax) * 100.f;
	}
	else
	{
		if (CPlayer::SF_RAGE != m_eForm)
			return 0.f;

		result = ((_float)form_state->Get_FormTimer() / (_float)form_state->Get_FormDuration()) * 100.f;
	}

	return result;
}

void CPlayer::Raise_HammerCombo()
{
	m_iHammerForm_Combo += m_iHammerForm_ComboStep;

	if (m_iHammerForm_Combo > m_iHammerForm_ComboMax)
		m_iHammerForm_Combo = m_iHammerForm_ComboMax;
}

void CPlayer::Empty_HammerCombo()
{
	m_iHammerForm_Combo = 0;
}

void CPlayer::SetUp_BoxOpenSoundValues()
{
	m_dSoundTimer_BoxOpen = m_dSoundTimer_BoxOpen_Duration;
	m_iRecorded_SoundPlayedFrame = UINT_MAX;
}

_float4* CPlayer::Get_BonePosition()
{
	auto BoneMatrix = XMLoadFloat4x4(m_BoneMatrixPtr.pCombinedMatrix) * XMLoadFloat4x4(&m_SocketTransformMatrix) * m_pTransformCom->Get_WorldMatrix();

	XMStoreFloat4(&m_vCenterPosition, XMVectorSetW(BoneMatrix.r[3], 1.f));

	return &m_vCenterPosition;
}

void CPlayer::Sheathe_Weapon()
{
	if (false == m_bHoldingWeapon)
		return;

	m_bHoldingWeapon = false;
	m_pWeapon->WeaponState(CPlayer_Weapon::WEAPON_STATE_SHEATHE);
}

_bool CPlayer::Register_WeaponCollider()
{
	return m_pWeapon->RegisterCollider();
}

void CPlayer::Register_WeaponCollider(_bool bRegister)
{
	m_pWeapon->RegisterCollider(bRegister);
}

_bool CPlayer::Eliminate_DeadTarget(CGameObject * _deadTarget)
{
	return m_pCombat_Assistant->Eliminate_DeadTarget(_deadTarget);
}

CGameObject * CPlayer::Get_Target()
{
	CCombat_Assistant::ASSIST_TYPE target_type = m_pCombat_Assistant->Get_TargetType();
	if (CCombat_Assistant::ASSIST_TYPE::ASSIST_END == target_type)
		return nullptr;

	return m_pCombat_Assistant->Get_Object(target_type);
}

HRESULT CPlayer::SetUp_Components()
{
	/* For.Com_StateMachine */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), TEXT("Com_StateMachine"), (CComponent**)&m_pStateMachineCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sora"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 0.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider_Sphere"), (CComponent**)&m_pColliderCom, &tColliderDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	tColliderDesc.vScale = _float3(0.15f, 0.15f, 0.15f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 0.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_WallFinder"), (CComponent**)&m_pWallFinderCom, &tColliderDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Status */
	CStatus::STATUSDESC StatusDesc;





	// �ӽ÷� �׺�Ž�������Ʈ
	if (LEVEL_TWILIGHT == GM->Get_CurrentLevel())
	{
		StatusDesc.fMaxHP = 200.f;


		CNavigation::NAVIDESC tNaviDesc;
		tNaviDesc.iCurrentIndex = 0;
		tNaviDesc.iPreviousIndex = 0;
		tNaviDesc.iPreviousFrame = 0;
		tNaviDesc.pParent = m_pTransformCom;

		/* For.Com_Navigation */
		if (FAILED(__super::Add_Component(LEVEL_TWILIGHT, TEXT("Prototype_Component_Navigation_Twilight"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &tNaviDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());
	}
	else if (LEVEL_RAPUNZEL == GM->Get_CurrentLevel())
	{
		StatusDesc.fMaxHP = 300.f;

		CNavigation::NAVIDESC tNaviDesc;
		tNaviDesc.iCurrentIndex = 0;
		tNaviDesc.iPreviousIndex = 0;
		tNaviDesc.iPreviousFrame = 0;
		tNaviDesc.pParent = m_pTransformCom;

		/* For.Com_Navigation */
		if (FAILED(__super::Add_Component(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Navigation_Rapunzel"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &tNaviDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());
	}
	else if (LEVEL_RAPUNZEL_BOSS == GM->Get_CurrentLevel())
	{
		StatusDesc.fMaxHP = 300.f;

		CNavigation::NAVIDESC tNaviDesc;
		tNaviDesc.iCurrentIndex = 0;
		tNaviDesc.iPreviousIndex = 0;
		tNaviDesc.iPreviousFrame = 0;
		tNaviDesc.pParent = m_pTransformCom;

		/* For.Com_Navigation */
		if (FAILED(__super::Add_Component(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Navigation_Rapunzel_Boss"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &tNaviDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());
	}
	else if (LEVEL_FROZEN == GM->Get_CurrentLevel())
	{
		StatusDesc.fMaxHP = 400.f;

		CNavigation::NAVIDESC tNaviDesc;
		tNaviDesc.iCurrentIndex = 0;
		tNaviDesc.iPreviousIndex = 0;
		tNaviDesc.iPreviousFrame = 0;
		tNaviDesc.pParent = m_pTransformCom;

		/* For.Com_Navigation */
		if (FAILED(__super::Add_Component(LEVEL_FROZEN, TEXT("Prototype_Component_Navigation_Frozen"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &tNaviDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());
	}
	else if (LEVEL_KEYGRAVE == GM->Get_CurrentLevel())
	{
		StatusDesc.fMaxHP = 400.f;

		CNavigation::NAVIDESC tNaviDesc;
		tNaviDesc.iCurrentIndex = 0;
		tNaviDesc.iPreviousIndex = 0;
		tNaviDesc.iPreviousFrame = 0;
		tNaviDesc.pParent = m_pTransformCom;

		/* For.Com_Navigation */
		if (FAILED(__super::Add_Component(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Navigation_KeyGrave"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &tNaviDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());
	}
	else if (LEVEL_FINALBOSS == GM->Get_CurrentLevel() || LEVEL_TEST == GM->Get_CurrentLevel())
	{
		StatusDesc.fMaxHP = 400.f;

		CNavigation::NAVIDESC tNaviDesc;
		tNaviDesc.iCurrentIndex = 0;
		tNaviDesc.iPreviousIndex = 0;
		tNaviDesc.iPreviousFrame = 0;
		tNaviDesc.pParent = m_pTransformCom;

		/* For.Com_Navigation */
		if (FAILED(__super::Add_Component(LEVEL_FINALBOSS, TEXT("Prototype_Component_Navigation_FinalBoss"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &tNaviDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		///* For.Com_Navigation */
		//if (FAILED(__super::Add_Component(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Navigation_FinalBoss"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &tNaviDesc)))
		//{
		//	BREAKPOINT;
		//	return E_FAIL;
		//}

		m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());
	}
	StatusDesc.fDamaged = 5.f;
	StatusDesc.iLevel = 1;
	StatusDesc.fEXP = 0.f;
	StatusDesc.fHP = StatusDesc.fMaxHP;
	StatusDesc.fMaxMP = 100.f;
	StatusDesc.fMP = StatusDesc.fMaxMP;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Status"), TEXT("Com_Status"), (CComponent**)&m_pStatusCom, &StatusDesc)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		BREAKPOINT;
		MSGBOX("Failed to Create CEffect_Mesh's Texture!");
		return E_FAIL;
	}

	/* For. DiffuseTexture_Rage */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_RageForm"), TEXT("Com_RageForm_DiffuseTexture"), (CComponent**)&m_pDiffuseTexture[SO_RAGE])))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For. DiffuseTexture_Rage */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_HammerForm"), TEXT("Com_HammerForm_DiffuseTexture"), (CComponent**)&m_pDiffuseTexture[SO_HAMMER])))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer::SetUp_ConstantTable()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnOldShader(m_pShaderCom, "g_OldWorldMatrix")))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_fRimPower", &m_fRimPower, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_fRimRange", &m_fRimRange, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_vRimColor", &m_vRimColor, sizeof(_float4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float4 vCam;
	XMStoreFloat4(&vCam, pGameInstance->Get_CamPosition());

	if (FAILED(m_pShaderCom->Set_RawValue("g_vWorldCamPos", &vCam, sizeof(_float4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (true == m_bControlMode)
		m_pRendererCom->Set_FocusPosition(XMStoreFloat4RT(m_pTransformCom->Get_Position()));
	
	if (FAILED(m_pShaderCom->Set_RawValue("g_isRim", &m_isUsingRim, sizeof(_bool))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_isBlur", &m_isUsingBlur, sizeof(_bool))))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	// ������ �ؽ�ó ���ε�
	if (FAILED(m_pRendererCom->Get_NoiseTexture()->SetUp_OnShader(m_pShaderCom, "g_DissolveTexture", 4)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float4 vColor = _float4(0.f, 0.f, 0.f, 1.f);
	if (FAILED(m_pShaderCom->Set_RawValue("g_vDissolveColor1", &vColor, sizeof(_float4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_vDissolveColor2", &vColor, sizeof(_float4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_vDissolveColor3", &vColor, sizeof(_float4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveAlpha", &m_fTime, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float fLUTMask = 1.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fLUTMask", &fLUTMask, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_ThirdTexture", 37);

	return S_OK;
}

HRESULT CPlayer::Separate_Channels()
{
	if (FAILED(m_pModelCom->Make_BodyPartInfo(2)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	const char* part_name1 = "Face";
	const char* bone_name_want_to_search1[] = { "facial" };

	if (FAILED(m_pModelCom->Fill_BodyPartInfo(part_name1, bone_name_want_to_search1, 1)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	const char* part_name2 = "Body";
	const char* bone_name_want_to_search2[] = { m_pModelCom->Get_RootName() };

	if (FAILED(m_pModelCom->Fill_BodyPartInfo(part_name2, bone_name_want_to_search2, 1)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

void CPlayer::Adjust_Animation_Details()
{
	/* Setup RootNode Index */
	SetUp_RootNodeIndex();

	/* Setup Once Animation List */
	MakeList_Once();

	/* Setup Lerp Animation List */
	MakeList_Lerp();

	/* Setup Draw Animation List */
	MakeList_Draw();

	/* SetUp Animation List on StateMachine about Each States and Related Animations */
	SetUp_StateAnimations();

	MakeList_PlusJumpChain();
	MakeList_HoveringStart();
	MakeList_AbleToSecondJump();
	MakeList_LandOnGround();
	MakeList_GuardSucceeded();
	MakeList_LastCombo();
	MakeList_MoveOnAxisY();
	MakeList_NormalAttacks();

	/* Adjust Speed of Animations */
	vector<CAnimation*>* animations = m_pModelCom->Get_Animations();

	/*
		����ȯ�濡�� ��� �׽�Ʈ�ϴ� �ӵ�
		 - ��� �ִϸ��̼� 1.5
		 - �ǰ� �ִϸ��̼� 1.65
		 - ������ �� �ǰ� �ִϸ��̼� 1.75
	*/

	for (auto animation : (*animations))
		animation->Set_PlaySpeed_Rate(1.25);

	// �ǰ� �ִϸ��̼� �ӵ� ����
	for (_uint i = 30; i < SORA_ANIM_HANG_ENTER; ++i)
		(*animations)[i]->Set_PlaySpeed_Rate(1.5);

	// ������ �� �ǰ� �ִϸ��̼� �ӵ� ����
	for (_uint i = 90; i < SORA_ANIM_RAGE_BLOCKED; ++i)
		(*animations)[i]->Set_PlaySpeed_Rate(1.5);
	
#pragma region Deleted Root Y
	// Y Root�� ������� ����, �ִϸ��̼� �̵����� ����ϱ� ���� ����������Ѵ�
	//(*animations)[SORA_ANIM_GROUND_ATTACK4]->Set_UseRootY(true);

	//(*animations)[SORA_ANIM_UPPER_ATTACK1]->Set_UseRootY(true);
	//(*animations)[SORA_ANIM_UPPER_ATTACK2]->Set_UseRootY(true);
	// SORA_ANIM_UPPER_ATTACK2�� �������� ��Ʈ���� Y�̵����� ����ִ�. ����

	//(*animations)[SORA_ANIM_LOWER_ATTACK3]->Set_UseRootY(true);
	//(*animations)[SORA_ANIM_LOWER_ATTACK4]->Set_UseRootY(true);
	//
	//(*animations)[SORA_ANIM_RAGE_DOUBLE_TAP]->Set_UseRootY(true);
	//(*animations)[SORA_ANIM_RAGE_TRIPLE_TAP]->Set_UseRootY(true);
	//(*animations)[SORA_ANIM_RAGE_ATTACK2]->Set_UseRootY(true);
#pragma endregion

	// �ִϸ��̼� �̵��� ���
	for(_uint i = 0; i < SORA_ANIM::SORA_ANIM_END; ++i)
		m_pModelCom->Calculate_MovementAmount(i);

	// ���������� �ӵ��� ������ �ִϸ��̼�
	for (_uint i = SORA_ANIM_HAMMER_ATTACK_01; i < SORA_ANIM_HAMMER_FORM; ++i)
		(*animations)[i]->Set_PlaySpeed_Rate(1.5);

	(*animations)[SORA_ANIM_HAMMER_FINISH]->Set_PlaySpeed_Rate(1.5);
	(*animations)[SORA_ANIM_HAMMER_AIR_LAST_ATTACK]->Set_PlaySpeed_Rate(0.75);
	(*animations)[SORA_ANIM_OPEN_SMALL]->Set_PlaySpeed_Rate(1.5);
	(*animations)[SORA_ANIM_OPEN_BIG]->Set_PlaySpeed_Rate(1.5);

	// ���������� �̵����� ������ �ִϸ��̼�
	(*animations)[SORA_ANIM_RUN_WALL]->Set_MovementRatioY(0.f);
	(*animations)[SORA_ANIM_RAGE_ROLL]->Set_MovementRatioXZ(2.f);
}

HRESULT CPlayer::SetUp_RootNodeIndex()
{
	_int iRootNodeIndex = m_pModelCom->Get_NodeIndexByName("kh_sk");
	if (UINT_MAX == iRootNodeIndex)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_pModelCom->Set_RootNodeIndex(iRootNodeIndex);

	_int iRootNodeY = m_pModelCom->Get_NodeIndexByName("center");
	if (UINT_MAX == iRootNodeY)
		_breakpoint

	m_pModelCom->Set_RootNodeY(iRootNodeY);

	return S_OK;
}

void CPlayer::MakeList_Once()
{
	// �켱 ���� �� �ְ�
	// ���ܽ�ų �͵鸸 ���ܽ�Ų��.
	m_pModelCom->Add_OnceListAll();

	// ====================== ���� ��ų ��� ====================== //

	// ���
	m_pModelCom->Rmv_OnceList(SORA_ANIM_IDLE);

	// �̵�
	m_pModelCom->Rmv_OnceList(SORA_ANIM_RUN_FORWARD);
	m_pModelCom->Rmv_OnceList(SORA_ANIM_RUN_FORWARD_BLADE);

	// ����
	m_pModelCom->Rmv_OnceList(SORA_ANIM_FALL_LOOP);

	// �ǰ� �� ����
	m_pModelCom->Rmv_OnceList(SORA_ANIM_AIR_DAMAGED_AERIAL_F_LOOP);
	m_pModelCom->Rmv_OnceList(SORA_ANIM_AIR_DAMAGED_AERIAL_B_LOOP);
	m_pModelCom->Rmv_OnceList(SORA_ANIM_AIR_DAMAGED_AERIAL_R_LOOP);
	m_pModelCom->Rmv_OnceList(SORA_ANIM_AIR_DAMAGED_AERIAL_L_LOOP);

	// ����
	m_pModelCom->Rmv_OnceList(SORA_ANIM_STUN_LOOP);

	// �Ŵ޸���
	m_pModelCom->Rmv_OnceList(SORA_ANIM_HANG_LOOP);

	// �� �޸���
	m_pModelCom->Rmv_OnceList(SORA_ANIM_RUN_WALL);

	// Rage Form Animations
	m_pModelCom->Rmv_OnceList(SORA_ANIM_RAGE_IDLE);
	m_pModelCom->Rmv_OnceList(SORA_ANIM_RAGE_RUN);

	m_pModelCom->Rmv_OnceList(SORA_ANIM_RAGE_FALL_LOOP);

	// Hammer Form Animations
	m_pModelCom->Rmv_OnceList(SORA_ANIM_HAMMER_RUN);
	m_pModelCom->Rmv_OnceList(SORA_ANIM_HAMMER_FALL_LOOP);

	// ���� ���, ���� �̵�
	m_pModelCom->Rmv_OnceList(SORA_ANIM_AIR_IDLE);
	m_pModelCom->Rmv_OnceList(SORA_ANIM_AIR_MOVE);

	// ============================================================
}

void CPlayer::MakeList_Lerp()
{
	m_pModelCom->Add_LerpList(SORA_ANIM_IDLE);
	m_pModelCom->Add_LerpList(SORA_ANIM_BORING);

	m_pModelCom->Add_LerpList(SORA_ANIM_DRAW_BLADE);
	m_pModelCom->Add_LerpList(SORA_ANIM_IDLE_BLADE);
	m_pModelCom->Add_LerpList(SORA_ANIM_IDLE_BLADE_END);

	m_pModelCom->Add_LerpList(SORA_ANIM_BORING_BLADE);
	m_pModelCom->Add_LerpList(SORA_ANIM_RUN_FORWARD);
	m_pModelCom->Add_LerpList(SORA_ANIM_RUN_FORWARD_BLADE);

	m_pModelCom->Add_LerpList(SORA_ANIM_FALL_START);
	m_pModelCom->Add_LerpList(SORA_ANIM_FALL_LOOP);

	m_pModelCom->Add_LerpList(SORA_ANIM_SECOND_JUMP_FALL_START);

	// Rage Form Animations
	m_pModelCom->Add_LerpList(SORA_ANIM_RAGE_IDLE);
	m_pModelCom->Add_LerpList(SORA_ANIM_RAGE_RUN);

	m_pModelCom->Add_LerpList(SORA_ANIM_RAGE_FALL_START);
	m_pModelCom->Add_LerpList(SORA_ANIM_RAGE_FALL_LOOP);

	// Hammer Form Animations
	m_pModelCom->Add_LerpList(SORA_ANIM_HAMMER_TAKE_OUT);
	m_pModelCom->Add_LerpList(SORA_ANIM_HAMMER_PUT_DOWN);

	m_pModelCom->Add_LerpList(SORA_ANIM_HAMMER_IDLE_LONG);
	m_pModelCom->Add_LerpList(SORA_ANIM_HAMMER_IDLE_SHORT);

	m_pModelCom->Add_LerpList(SORA_ANIM_HAMMER_FALL_START);
	m_pModelCom->Add_LerpList(SORA_ANIM_HAMMER_FALL_LOOP);

	m_pModelCom->Add_LerpList(SORA_ANIM_HAMMER_RUN);

	// ���� ���, ���� �̵�
	m_pModelCom->Add_LerpList(SORA_ANIM_AIR_IDLE);
	m_pModelCom->Add_LerpList(SORA_ANIM_AIR_MOVE);
}

void CPlayer::MakeList_Draw()
{
	// �߰�
	m_WeaponDrawAnim.push_back(SORA_ANIM_DRAW_BLADE);

	// ���� �ִϸ��̼�
	m_WeaponDrawAnim.push_back(SORA_ANIM_GROUND_ATTACK1);
	m_WeaponDrawAnim.push_back(SORA_ANIM_GROUND_ATTACK2);
	m_WeaponDrawAnim.push_back(SORA_ANIM_GROUND_ATTACK3);
	m_WeaponDrawAnim.push_back(SORA_ANIM_GROUND_ATTACK4);

	m_WeaponDrawAnim.push_back(SORA_ANIM_AIR_ATTACK1);
	m_WeaponDrawAnim.push_back(SORA_ANIM_AIR_ATTACK2);
	m_WeaponDrawAnim.push_back(SORA_ANIM_AIR_ATTACK3);
	m_WeaponDrawAnim.push_back(SORA_ANIM_AIR_ATTACK4);

	// ���� �ִϸ��̼�
	m_WeaponDrawAnim.push_back(SORA_ANIM_GUARD);
	m_WeaponDrawAnim.push_back(SORA_ANIM_AIR_GUARD);

	// �ǰ� �ִϸ��̼�
	for (_uint i = 30; i < SORA_ANIM_HANG_ENTER; ++i)
		m_WeaponDrawAnim.push_back((SORA_ANIM)i);

	// ���� �ִϸ��̼� - �߰�
	m_WeaponDrawAnim.push_back(SORA_ANIM_UPPER_ATTACK1);
	m_WeaponDrawAnim.push_back(SORA_ANIM_UPPER_ATTACK2);
	m_WeaponDrawAnim.push_back(SORA_ANIM_UPPER_ATTACK3);
	m_WeaponDrawAnim.push_back(SORA_ANIM_UPPER_ATTACK4);

	m_WeaponDrawAnim.push_back(SORA_ANIM_LOWER_ATTACK1);
	m_WeaponDrawAnim.push_back(SORA_ANIM_LOWER_ATTACK2);
	m_WeaponDrawAnim.push_back(SORA_ANIM_LOWER_ATTACK3);
	m_WeaponDrawAnim.push_back(SORA_ANIM_LOWER_ATTACK4);

	// ��ų �ߵ� �ִϸ��̼�
	m_WeaponDrawAnim.push_back(SORA_ANIM_SKILL_LEVEL1);
	m_WeaponDrawAnim.push_back(SORA_ANIM_SKILL_LEVEL2);
	m_WeaponDrawAnim.push_back(SORA_ANIM_SKILL_MOVE);
	
	m_WeaponDrawAnim.push_back(SORA_ANIM_AIR_SKILL_LEVEL1);
	m_WeaponDrawAnim.push_back(SORA_ANIM_AIR_SKILL_LEVEL2);
	m_WeaponDrawAnim.push_back(SORA_ANIM_AIR_SKILL_MOVE);

	// ���� ���� �ִϸ��̼�
	m_WeaponDrawAnim.push_back(SORA_ANIM_OPEN_SMALL);
	m_WeaponDrawAnim.push_back(SORA_ANIM_OPEN_BIG);

	// Rage Form Animations
	for (_uint i = 81; i < 118; ++i)
		m_WeaponDrawAnim.push_back((SORA_ANIM)i);

	// Hammer Form Animations
	for(_uint i = 118; i < 164; ++i)
		m_WeaponDrawAnim.push_back((SORA_ANIM)i);
}

void CPlayer::SetUp_StateAnimations()
{
	for (_uint i = 0; i < CStateMachine::STATETYPE_END; ++i)
	{
		switch (i)
		{
		case CStateMachine::STATETYPE::STATETYPE_IDLE:

			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_IDLE);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_BORING);
			/*m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_DRAW_BLADE);*/
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_IDLE_BLADE);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_IDLE_BLADE_END);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_BORING_BLADE);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_AIR_IDLE);

			// Rage Form Animations
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_RAGE_IDLE);

			// Hammer Form Animations
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_HAMMER_IDLE_LONG);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_HAMMER_IDLE_SHORT);

			break;
		case CStateMachine::STATETYPE::STATETYPE_NORMALMOVE:

			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_RUN_FORWARD);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_RUN_FORWARD_BLADE);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_AIR_MOVE);

			// Rage Form Animations
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_RAGE_RUN);

			// Hammer Form Animations
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_HAMMER_RUN);

			break;
		case CStateMachine::STATETYPE::STATETYPE_DYNAMICMOVE:

			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_STEPPING_LTOR);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_STEPPING_RTOL);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_HANG_ENTER);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_HANG_LOOP);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_RISE);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_RUN_WALL);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_JUMP_AT_WALL);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_SOMERSAULT);

			break;
		case CStateMachine::STATETYPE::STATETYPE_JUMP:

			// ������ ��� ��ü������ ó��.
			// ���� ��ġ�� �ʾ���.

			break;
		case CStateMachine::STATETYPE::STATETYPE_ATK:

			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_GROUND_ATTACK1);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_GROUND_ATTACK2);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_GROUND_ATTACK3);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_GROUND_ATTACK4);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_AIR_ATTACK1);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_AIR_ATTACK2);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_AIR_ATTACK3);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_AIR_ATTACK4);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_UPPER_ATTACK1);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_UPPER_ATTACK2);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_UPPER_ATTACK3);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_UPPER_ATTACK4);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_LOWER_ATTACK1);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_LOWER_ATTACK2);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_LOWER_ATTACK3);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_LOWER_ATTACK4);

			// Rage Form Animations
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_RAGE_DOUBLE_TAP);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_RAGE_TRIPLE_TAP);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_RAGE_ATTACK1);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_RAGE_ATTACK2);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_RAGE_ATTACK3);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_RAGE_ATTACK4);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_RAGE_ATTACK5);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_RAGE_FINISH);

			// Hammer Form Animations
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_HAMMER_ATTACK_01);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_HAMMER_ATTACK_02);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_HAMMER_ATTACK_03);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_HAMMER_ATTACK_04);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_HAMMER_ATTACK_05);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_HAMMER_ATTACK_06);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_HAMMER_GROUND_LAST_ATTACK);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_HAMMER_AIR_LAST_ATTACK);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_HAMMER_FINISH);

			break;
		case CStateMachine::STATETYPE::STATETYPE_SUMMON:

			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_SKILL_LEVEL1);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_SKILL_LEVEL2);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_SKILL_MOVE);

			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_AIR_SKILL_LEVEL1);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_AIR_SKILL_LEVEL2);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_AIR_SKILL_MOVE);

			// Hammer Form Animations
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_HAMMER_SKILL_LEVEL01);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_HAMMER_SKILL_LEVEL02);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_HAMMER_AIR_SKILL_LEVEL01);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_HAMMER_AIR_SKILL_LEVEL02);

			break;
		case CStateMachine::STATETYPE::STATETYPE_GUARD:

			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_GUARD);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_GUARD_SUCCEEDED);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_AIR_GUARD);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_AIR_GUARD_SUCCEEDED);

			// Rage Form Animations
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_RAGE_GUARD_SUCCEEDED);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_RAGE_GUARD);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_RAGE_AIR_GUARD);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_RAGE_COUNTER_ATTACK);

			// Hammer Form Animations
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_HAMMER_GUARD);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_HAMMER_GUARD_SUCCEEDED);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_HAMMER_AIR_GUARD);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_HAMMER_AIR_GUARD_SUCCEEDED);

			break;
		case CStateMachine::STATETYPE::STATETYPE_AVOID:

			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_ROLL);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_AIR_DASH);

			// Rage Form Animations
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_RAGE_ROLL);
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_RAGE_AIR_DASH);

			// Hammer Form Animations
			m_pStateMachineCom->Add_AnimIndex(i, CPlayer::SORA_ANIM_HAMMER_DASH);

			break;
		case CStateMachine::STATETYPE::STATETYPE_DAMAGED:

			for (_uint j = 30; j < SORA_ANIM_HANG_ENTER; ++j)
				m_pStateMachineCom->Add_AnimIndex(i, (SORA_ANIM)j);

			// Rage Form Animations
			for(_uint j = SORA_ANIM_RAGE_DAMAGED_F; j < SORA_ANIM_RAGE_BLOCKED; ++j)
				m_pStateMachineCom->Add_AnimIndex(i, (SORA_ANIM)j);

			// Hammer Form Animations
			for(_uint j = SORA_ANIM_HAMMER_DAMAGED_F; j < SORA_ANIM_HAMMER_GUARD; ++j)
				m_pStateMachineCom->Add_AnimIndex(i, (SORA_ANIM)j);

			break;
		case CStateMachine::STATETYPE::STATETYPE_FORM:

			// Rage Form Animations
			for(_uint j = SORA_ANIM_RAGE_IDLE; j < 118; j++)
				m_pStateMachineCom->Add_AnimIndex(i, (SORA_ANIM)j);

			// Hammer Form Animations
			for(_uint j = SORA_ANIM_HAMMER_TAKE_OUT; j < 163; ++j)
				m_pStateMachineCom->Add_AnimIndex(i, (SORA_ANIM)j);

			break;
		}
	}
}

void CPlayer::MakeList_PlusJumpChain()
{
	m_Animation_PlusJumpChain.reserve(10);

	m_Animation_PlusJumpChain.push_back(SORA_ANIM_JUMP_UP);
	m_Animation_PlusJumpChain.push_back(SORA_ANIM_FALL_START);
	m_Animation_PlusJumpChain.push_back(SORA_ANIM_SECOND_JUMP_UP);
	m_Animation_PlusJumpChain.push_back(SORA_ANIM_SECOND_JUMP_FALL_START);

	m_Animation_PlusJumpChain.push_back(SORA_ANIM_RAGE_JUMP_UP);
	m_Animation_PlusJumpChain.push_back(SORA_ANIM_RAGE_FALL_START);

	m_Animation_PlusJumpChain.push_back(SORA_ANIM_HAMMER_JUMP_UP);
	m_Animation_PlusJumpChain.push_back(SORA_ANIM_HAMMER_FALL_START);
	m_Animation_PlusJumpChain.push_back(SORA_ANIM_HAMMER_SECOND_JUMP_UP);
	m_Animation_PlusJumpChain.push_back(SORA_ANIM_HAMMER_SECOND_JUMP);
}

void CPlayer::MakeList_HoveringStart()
{
	m_Animation_HoveringStart.reserve(5);

	m_Animation_HoveringStart.push_back(SORA_ANIM_JUMP_UP);
	m_Animation_HoveringStart.push_back(SORA_ANIM_SECOND_JUMP_UP);
	m_Animation_HoveringStart.push_back(SORA_ANIM_RAGE_JUMP_UP);
	m_Animation_HoveringStart.push_back(SORA_ANIM_HAMMER_JUMP_UP);
	m_Animation_HoveringStart.push_back(SORA_ANIM_HAMMER_SECOND_JUMP_UP);
}

void CPlayer::MakeList_AbleToSecondJump()
{
	m_Animation_AbleToSecondJump.reserve(14);

	m_Animation_AbleToSecondJump.push_back(SORA_ANIM_JUMP_UP);
	m_Animation_AbleToSecondJump.push_back(SORA_ANIM_FALL_START);
	m_Animation_AbleToSecondJump.push_back(SORA_ANIM_FALL_LOOP);

	m_Animation_AbleToSecondJump.push_back(SORA_ANIM_SECOND_JUMP_UP);
	m_Animation_AbleToSecondJump.push_back(SORA_ANIM_SECOND_JUMP_FALL_START);

	m_Animation_AbleToSecondJump.push_back(SORA_ANIM_RAGE_JUMP_UP);
	m_Animation_AbleToSecondJump.push_back(SORA_ANIM_RAGE_FALL_START);
	m_Animation_AbleToSecondJump.push_back(SORA_ANIM_RAGE_FALL_LOOP);

	m_Animation_AbleToSecondJump.push_back(SORA_ANIM_HAMMER_JUMP_UP);
	m_Animation_AbleToSecondJump.push_back(SORA_ANIM_HAMMER_FALL_START);
	m_Animation_AbleToSecondJump.push_back(SORA_ANIM_HAMMER_FALL_LOOP);

	m_Animation_AbleToSecondJump.push_back(SORA_ANIM_HAMMER_SECOND_JUMP_UP);

	m_Animation_AbleToSecondJump.push_back(SORA_ANIM_AIR_IDLE);
	m_Animation_AbleToSecondJump.push_back(SORA_ANIM_AIR_MOVE);
}

void CPlayer::MakeList_LandOnGround()
{
	m_Animation_LandOnGround.reserve(3);

	m_Animation_LandOnGround.push_back(SORA_ANIM_FALL_END);
	m_Animation_LandOnGround.push_back(SORA_ANIM_HAMMER_FALL_END);
	m_Animation_LandOnGround.push_back(SORA_ANIM_RAGE_FALL_END);
}

void CPlayer::MakeList_GuardSucceeded()
{
	m_Animation_GuardSucceeded.reserve(5);

	m_Animation_GuardSucceeded.push_back(SORA_ANIM_GUARD_SUCCEEDED);
	m_Animation_GuardSucceeded.push_back(SORA_ANIM_AIR_GUARD_SUCCEEDED);

	m_Animation_GuardSucceeded.push_back(SORA_ANIM_HAMMER_GUARD_SUCCEEDED);
	m_Animation_GuardSucceeded.push_back(SORA_ANIM_HAMMER_AIR_GUARD_SUCCEEDED);

	m_Animation_GuardSucceeded.push_back(SORA_ANIM_RAGE_GUARD_SUCCEEDED);
}

void CPlayer::MakeList_LastCombo()
{
	m_Animation_LastCombo.reserve(8);

	m_Animation_LastCombo.push_back(SORA_ANIM_GROUND_ATTACK4);
	m_Animation_LastCombo.push_back(SORA_ANIM_AIR_ATTACK4);
	m_Animation_LastCombo.push_back(SORA_ANIM_UPPER_ATTACK4);
	m_Animation_LastCombo.push_back(SORA_ANIM_LOWER_ATTACK4);

	m_Animation_LastCombo.push_back(SORA_ANIM_HAMMER_ATTACK_06);
	m_Animation_LastCombo.push_back(SORA_ANIM_HAMMER_GROUND_LAST_ATTACK);
	m_Animation_LastCombo.push_back(SORA_ANIM_HAMMER_AIR_LAST_ATTACK);

	m_Animation_LastCombo.push_back(SORA_ANIM_RAGE_ATTACK5);
}

void CPlayer::MakeList_MoveOnAxisY()
{
	m_Animation_MoveOnAxisY.reserve(14);

	m_Animation_MoveOnAxisY.push_back(SORA_ANIM_AIR_ATTACK1);
	m_Animation_MoveOnAxisY.push_back(SORA_ANIM_AIR_ATTACK2);
	m_Animation_MoveOnAxisY.push_back(SORA_ANIM_AIR_ATTACK4);

	m_Animation_MoveOnAxisY.push_back(SORA_ANIM_UPPER_ATTACK1);
	m_Animation_MoveOnAxisY.push_back(SORA_ANIM_UPPER_ATTACK2);
	m_Animation_MoveOnAxisY.push_back(SORA_ANIM_UPPER_ATTACK4);

	m_Animation_MoveOnAxisY.push_back(SORA_ANIM_LOWER_ATTACK1);
	m_Animation_MoveOnAxisY.push_back(SORA_ANIM_LOWER_ATTACK2);
	m_Animation_MoveOnAxisY.push_back(SORA_ANIM_LOWER_ATTACK3);
	m_Animation_MoveOnAxisY.push_back(SORA_ANIM_LOWER_ATTACK4);

	m_Animation_MoveOnAxisY.push_back(SORA_ANIM_HAMMER_ATTACK_02);
	m_Animation_MoveOnAxisY.push_back(SORA_ANIM_HAMMER_ATTACK_03);
	m_Animation_MoveOnAxisY.push_back(SORA_ANIM_HAMMER_ATTACK_04);
	m_Animation_MoveOnAxisY.push_back(SORA_ANIM_HAMMER_ATTACK_06);
}

void CPlayer::MakeList_NormalAttacks()
{
	vector<SORA_ANIM> temp_vector;
	temp_vector.push_back(SORA_ANIM_GROUND_ATTACK1);
	temp_vector.push_back(SORA_ANIM_GROUND_ATTACK2);
	temp_vector.push_back(SORA_ANIM_GROUND_ATTACK3);
	temp_vector.push_back(SORA_ANIM_GROUND_ATTACK4);

	temp_vector.push_back(SORA_ANIM_AIR_ATTACK1);
	temp_vector.push_back(SORA_ANIM_AIR_ATTACK2);
	temp_vector.push_back(SORA_ANIM_AIR_ATTACK3);
	temp_vector.push_back(SORA_ANIM_AIR_ATTACK4);

	temp_vector.push_back(SORA_ANIM_UPPER_ATTACK1);
	temp_vector.push_back(SORA_ANIM_UPPER_ATTACK2);
	temp_vector.push_back(SORA_ANIM_UPPER_ATTACK3);
	temp_vector.push_back(SORA_ANIM_UPPER_ATTACK4);

	temp_vector.push_back(SORA_ANIM_LOWER_ATTACK1);
	temp_vector.push_back(SORA_ANIM_LOWER_ATTACK2);
	temp_vector.push_back(SORA_ANIM_LOWER_ATTACK3);
	temp_vector.push_back(SORA_ANIM_LOWER_ATTACK4);

	//// �ظ� ��
	//temp_vector.push_back(SORA_ANIM_HAMMER_ATTACK_01);
	//temp_vector.push_back(SORA_ANIM_HAMMER_ATTACK_02);
	//temp_vector.push_back(SORA_ANIM_HAMMER_ATTACK_03);
	//temp_vector.push_back(SORA_ANIM_HAMMER_ATTACK_04);
	//temp_vector.push_back(SORA_ANIM_HAMMER_ATTACK_05);
	//temp_vector.push_back(SORA_ANIM_HAMMER_ATTACK_06);

	//temp_vector.push_back(SORA_ANIM_HAMMER_GROUND_LAST_ATTACK);
	//temp_vector.push_back(SORA_ANIM_HAMMER_AIR_LAST_ATTACK);

	//// ������ ��
	//temp_vector.push_back(SORA_ANIM_RAGE_DOUBLE_TAP);
	//temp_vector.push_back(SORA_ANIM_RAGE_TRIPLE_TAP);

	//temp_vector.push_back(SORA_ANIM_RAGE_ATTACK1);
	//temp_vector.push_back(SORA_ANIM_RAGE_ATTACK2);
	//temp_vector.push_back(SORA_ANIM_RAGE_ATTACK3);
	//temp_vector.push_back(SORA_ANIM_RAGE_ATTACK4);
	//temp_vector.push_back(SORA_ANIM_RAGE_ATTACK5);
	//temp_vector.push_back(SORA_ANIM_RAGE_COUNTER_ATTACK);

	m_Animation_NormalAttacks.reserve(temp_vector.size());

	for (auto anim_enum : temp_vector)
		m_Animation_NormalAttacks.push_back(anim_enum);
}

HRESULT CPlayer::Create_Weapon()
{
	if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_PlayerWeapon"), TEXT("Prototype_GameObject_PlayerWeapon"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_pWeapon = static_cast<CPlayer_Weapon*>(m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_PlayerWeapon")));

	if (nullptr == m_pWeapon)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

void CPlayer::SetUp_Assistant()
{
	// =====================================================================================
	// ASSIST_CANCLE
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_GROUND_ATTACK1);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_GROUND_ATTACK2);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_GROUND_ATTACK3);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_GROUND_ATTACK4);

	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_AIR_ATTACK1);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_AIR_ATTACK2);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_AIR_ATTACK3);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_AIR_ATTACK4);

	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_GUARD_SUCCEEDED);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_AIR_GUARD_SUCCEEDED);

	//m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_AIR_DASH);

	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_UPPER_ATTACK1);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_UPPER_ATTACK2);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_UPPER_ATTACK3);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_UPPER_ATTACK4);

	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_LOWER_ATTACK1);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_LOWER_ATTACK2);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_LOWER_ATTACK3);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_LOWER_ATTACK4);

	// Rage Form Animations
	//m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_RAGE_AIR_DASH);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_RAGE_GUARD_SUCCEEDED);

	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_RAGE_DOUBLE_TAP);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_RAGE_TRIPLE_TAP);

	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_RAGE_ATTACK1);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_RAGE_ATTACK2);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_RAGE_ATTACK3);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_RAGE_ATTACK4);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_RAGE_ATTACK5);

	// Hammer Form Animations
	//m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_HAMMER_DASH);

	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_HAMMER_GUARD_SUCCEEDED);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_HAMMER_AIR_GUARD_SUCCEEDED);

	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_HAMMER_ATTACK_01);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_HAMMER_ATTACK_02);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_HAMMER_ATTACK_03);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_HAMMER_ATTACK_04);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_HAMMER_ATTACK_05);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_CANCLE, SORA_ANIM_HAMMER_ATTACK_06);

	// =====================================================================================
	// ASSIST_ATTACK
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_GROUND_ATTACK1);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_GROUND_ATTACK2);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_GROUND_ATTACK3);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_GROUND_ATTACK4);

	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_AIR_ATTACK1);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_AIR_ATTACK2);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_AIR_ATTACK3);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_AIR_ATTACK4);

	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_UPPER_ATTACK1);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_UPPER_ATTACK2);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_UPPER_ATTACK3);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_UPPER_ATTACK4);

	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_LOWER_ATTACK1);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_LOWER_ATTACK2);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_LOWER_ATTACK3);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_LOWER_ATTACK4);

	// Rage Form Animations
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_RAGE_DOUBLE_TAP);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_RAGE_TRIPLE_TAP);

	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_RAGE_ATTACK1);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_RAGE_ATTACK2);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_RAGE_ATTACK3);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_RAGE_ATTACK4);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_RAGE_ATTACK5);

	// Hammer Form Animations
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_HAMMER_ATTACK_01);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_HAMMER_ATTACK_02);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_HAMMER_ATTACK_03);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_HAMMER_ATTACK_04);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_HAMMER_ATTACK_05);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_HAMMER_ATTACK_06);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_HAMMER_GROUND_LAST_ATTACK);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_ATTACK, SORA_ANIM_HAMMER_AIR_LAST_ATTACK);

	// =====================================================================================
	// ASSIST_MOVE
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_MOVE, SORA_ANIM_FALL_END);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_MOVE, SORA_ANIM_ROLL);

	// Rage Form Animations
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_MOVE, SORA_ANIM_RAGE_FALL_END);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_MOVE, SORA_ANIM_RAGE_ROLL);

	// Hammer Form Animations
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_MOVE, SORA_ANIM_HAMMER_FALL_END);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_MOVE, SORA_ANIM_HAMMER_DASH);

	// =====================================================================================
	// ASSIST_GRAVITY
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_AIR_GUARD);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_AIR_GUARD_SUCCEEDED);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_AIR_DASH);

	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_AIR_ATTACK1);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_AIR_ATTACK2);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_AIR_ATTACK3);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_AIR_ATTACK4);

	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_UPPER_ATTACK1);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_UPPER_ATTACK2);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_UPPER_ATTACK3);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_UPPER_ATTACK4);

	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_LOWER_ATTACK1);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_LOWER_ATTACK2);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_LOWER_ATTACK3);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_LOWER_ATTACK4);

	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_AIR_SKILL_LEVEL1);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_AIR_SKILL_LEVEL2);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_AIR_SKILL_MOVE);

	// �� ü���� �ִϸ��̼��� Gravity ��Ͽ� ���� �ʰ� Form State���� ��ü������ �߷°����Ѵ�.

	// Rage Form Animations
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_RAGE_AIR_GUARD);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_RAGE_GUARD_SUCCEEDED);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_RAGE_AIR_DASH);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_RAGE_COUNTER_ATTACK);

	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_RAGE_DOUBLE_TAP);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_RAGE_TRIPLE_TAP);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_RAGE_ATTACK1);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_RAGE_ATTACK2);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_RAGE_ATTACK3);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_RAGE_ATTACK4);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_RAGE_ATTACK5);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_RAGE_FINISH);

	// Hammer Form Animations
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_HAMMER_DASH);

	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_HAMMER_AIR_GUARD);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_HAMMER_AIR_GUARD_SUCCEEDED);

	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_HAMMER_AIR_SKILL_LEVEL01);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_HAMMER_AIR_SKILL_LEVEL02);

	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_HAMMER_AIR_BUFF);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_HAMMER_AIR_FINISH_READY);

	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_HAMMER_ATTACK_02);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_HAMMER_ATTACK_03);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_HAMMER_ATTACK_04);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_HAMMER_AIR_LAST_ATTACK);
	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_HAMMER_FINISH);

	m_pAction_Assistant->Add_AssistAnim(A_ASSIST::ASSIST_GRAVITY, SORA_ANIM_STRAIGHTEN);
}

void CPlayer::Work_On_FirstTick()
{
	LEVEL current_level = GM->Get_CurrentLevel();
	if ((LEVEL_TWILIGHT <= current_level && LEVEL_MAPTOOL > current_level) || LEVEL_TEST == current_level)
	{
		if (nullptr == m_pCamera)
		{
			m_pCamera = static_cast<CCamera_Perspective*>(m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Camera")));
			m_pCameraTransform = m_pCamera->Get_Transform();
		}

		if (nullptr == m_pCamera)
		{
			BREAKPOINT;
			return;
		}
	}

	Create_Weapon();
	m_pAction_Assistant->SetUp_Weapon();
}

void CPlayer::Calculate_AngleValues()
{
	_vector sora_look = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vecy(sora_look) = 0.f;
	sora_look = _vec3norm(sora_look);

	_vector camera_look = m_pCameraTransform->Get_State(CTransform::STATE_LOOK);
	vecy(camera_look) = 0.f;
	sora_look = _vec3norm(camera_look);

	m_fAngle_Between_CameraR = XMVectorGetX(XMVector3AngleBetweenNormals(sora_look, camera_look));
	m_fAngle_Between_CameraD = XMConvertToDegrees(m_fAngle_Between_CameraR);

	_vector sora_right = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	vecy(sora_right) = 0.f;
	sora_right = _vec3norm(sora_right);

	_vector dot_result = XMVector3Dot(camera_look, sora_right);

	if (0 > XMVectorGetX(dot_result))
	{
		m_fAngle_Between_CameraD *= -1.f;
		m_fAngle_Between_CameraR *= -1.f;
	}
}

void CPlayer::Calculate_CellValues()
{
	// �׺���̼� ��� �� �ƴ϶�� return
	if (true == m_pNavigationCom->Get_Ignore())
		return;

	CCell* current_cell = m_pNavigationCom->Get_CurrentCell();
	if (nullptr == current_cell)
	{
		BREAKPOINT;
		return;
	}

	_vector position = m_pTransformCom->Get_Position();

	// ���� ����� ���ΰ��� �Ÿ��� ������ Ÿ���� �޾Ƶд�
	CCell::LINES temp_type = CCell::LINES::LINE_END;
	m_fDistance_To_Closest_Line = current_cell->Get_CloseLine(position, &temp_type);

	m_iClosest_Line_Type = (_uint)temp_type;
}

void CPlayer::Update_Input(_double Timedelta)
{
	// Imgui �Է�
	if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL))
	{
		// ImGuI On / Off
		if (m_pGameInstance->Get_KeyEnter(DIK_P))
			m_bImGuiController = !m_bImGuiController;

		// ImGui Control On / Off
		if (m_pGameInstance->Get_KeyEnter(DIK_TAB))
			m_bControlMode = !m_bControlMode;
	}

	if (false == m_bControlMode)
		return;

	/*
		���⿡�� �� ���ǰ˻��
		[ ���� Ŭ������ ���� ] �� ���� ���ǰ˻��� ���� �������.
		�ϴ� �����ϰ� �Ѱܼ� ���� Ŭ���� ���ο��� ó���ϴ� ���� �ƴ϶�.
		�ش� ���� Ŭ������ �����ص� ������ �����ؾ��Ѵ�.

		�ʿ��� ����(��ü�� �Է�ó��), ���ѵǾ���� ����(�ַ� ������� ���� ����)
		�� ������ ũ�� ����� �˻�����.
	*/

	if (m_pGameInstance->Get_KeyEnter(DIK_W) || m_pGameInstance->Get_KeyEnter(DIK_S) ||
		m_pGameInstance->Get_KeyEnter(DIK_A) || m_pGameInstance->Get_KeyEnter(DIK_D))
		m_bInput[SI_WASD] = true;

	if (m_pGameInstance->Get_KeyPressing(DIK_W) || m_pGameInstance->Get_KeyPressing(DIK_S) ||
		m_pGameInstance->Get_KeyPressing(DIK_A) || m_pGameInstance->Get_KeyPressing(DIK_D))
		m_bInput[SI_WASD] = true;

	if (true == m_bInput[SI_WASD])
		Calculate_AvoidingDirection();

	if (m_pGameInstance->Get_KeyEnter(DIK_SPACE))
		m_bInput[SI_SPACE] = true;

	if (m_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON))
		m_bInput[SI_LBUTTON] = true;

	if (m_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_RBUTTON))
		m_bInput[SI_RBUTTON] = true;

	if (m_pGameInstance->Get_KeyEnter(DIK_RIGHT))
		m_bInput[SI_RIGHT] = true;

	if (m_pGameInstance->Get_KeyEnter(DIK_R))
		m_bInput[SI_R] = true;

	if (m_pGameInstance->Get_KeyEnter(DIK_E))
		m_bInput[SI_E] = true;

	if (m_pGameInstance->Get_KeyEnter(DIK_H))
		m_bInput[SI_F] = true;

	//if (m_pGameInstance->Get_KeyEnter(DIK_V))
	//	m_bInput[SI_EVENT] = true;
}

void CPlayer::Update_Terms(_double Timedelta)
{
	if (nullptr == m_pNavigationCom || true == m_pNavigationCom->Get_Ignore())
		return;

	// ����, ȸ�� ���� ���� ��Ÿ�� ���� �˻����� ����
	if (true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_JUMP) ||
		true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_ATK) ||
		true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_SUMMON) ||
		true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_GUARD) ||
		true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_AVOID) ||
		true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_FORM))
		return;

	// �Է��� ���� ��(m_bInput���� �˻��� �� ����)
	if (m_pGameInstance->Get_KeyPressing(DIK_W) || m_pGameInstance->Get_KeyPressing(DIK_S) ||
		m_pGameInstance->Get_KeyPressing(DIK_A) || m_pGameInstance->Get_KeyPressing(DIK_D))
	{
		// ���� ����� ������ �ǳ��� �ִ� �̿��� �ε����� �޾ƿ´�
		_int neighbor_index = m_pNavigationCom->Get_CurrentNeighborIndex((CCell::LINES) m_iClosest_Line_Type);

		// ����ó��
		if (-1 == neighbor_index)
		{
			m_dTimer_ToStartFreeRunning = 0.0;
			return;
		}

		// �޾ƿ� �ε����� �ش� ��(�̿� ��)�� ����
		CCell* input_cell = m_pNavigationCom->Get_InputCell(neighbor_index);
		if (nullptr == input_cell)
		{
			BREAKPOINT;
			return;
		}

		// �̿� ���� RUNNING�� �ƴ� ��� return
		CCell::TYPE input_cell_type = input_cell->Get_Type();
		if (CCell::TYPE::TYPE_RUNNING != input_cell_type)
		{
			m_dTimer_ToStartFreeRunning = 0.0;
			return;
		}

		// ���� üũ
		_vector normal = XMLoadFloat3(&input_cell->Get_NormalVector()) * -1.f;
		_vector look_norm = _vec3norm(m_pTransformCom->Get_Look());

		vecy(normal) = 0.f;
		vecy(m_vMoveDir) = 0.f;

		m_fAngle_Between_Wall = XMConvertToDegrees(vecx(XMVector3AngleBetweenVectors(normal, m_vMoveDir)));

		if (45.f < m_fAngle_Between_Wall)
		{
			m_dTimer_ToStartFreeRunning = 0.0;
			return;
		}

		// ���� ������ ������ ���¿��� Wall Finder�� ���� ���� �������� üũ, �� �����ٸ� return
		if (true == m_pNavigationCom->isIn(XMVectorSetW(XMLoadFloat3(&m_vWallFinder), 1.f), nullptr, nullptr, false))
			return;

		m_dTimer_ToStartFreeRunning += Timedelta;

		if (0.25 <= m_dTimer_ToStartFreeRunning)
			m_pNavigationCom->Set_Climb(true);
	}

	else
		m_dTimer_ToStartFreeRunning = 0.0;
}

void CPlayer::Calculate_AvoidingDirection()
{
	vector<MOVE_DIR> move_dir;
	if (m_pGameInstance->Get_KeyEnter(DIK_W) ||
		m_pGameInstance->Get_KeyPressing(DIK_W))
	{
		move_dir.push_back(MOVE_DIR::DIR_FORWARD);
	}

	else if (m_pGameInstance->Get_KeyEnter(DIK_S) ||
		m_pGameInstance->Get_KeyPressing(DIK_S))
	{
		move_dir.push_back(MOVE_DIR::DIR_BACKWARD);
	}

	if (m_pGameInstance->Get_KeyEnter(DIK_A) ||
		m_pGameInstance->Get_KeyPressing(DIK_A))
	{
		move_dir.push_back(MOVE_DIR::DIR_LEFT);
	}

	else if (m_pGameInstance->Get_KeyEnter(DIK_D) ||
		m_pGameInstance->Get_KeyPressing(DIK_D))
	{
		move_dir.push_back(MOVE_DIR::DIR_RIGHT);
	}

	vector<_vector> dir_vector;
	for (auto dir : move_dir)
	{
		_vector move_dir_norm;
		switch (dir)
		{
		case DIR_FORWARD:
			move_dir_norm = m_pCameraTransform->Get_State(CTransform::STATE_LOOK);
			break;
		case DIR_BACKWARD:
			move_dir_norm = m_pCameraTransform->Get_State(CTransform::STATE_LOOK) * -1.f;
			break;
		case DIR_LEFT:
			move_dir_norm = m_pCameraTransform->Get_State(CTransform::STATE_RIGHT) * -1.f;
			break;
		case DIR_RIGHT:
			move_dir_norm = m_pCameraTransform->Get_State(CTransform::STATE_RIGHT);
			break;
		}

		vecy(move_dir_norm) = 0.f;
		move_dir_norm = XMVector3Normalize(move_dir_norm);

		dir_vector.push_back(move_dir_norm);
	}

	_vector average_dir_for_avoiding = XMVectorZero();
	for (auto dir : dir_vector)
		average_dir_for_avoiding += dir;

	average_dir_for_avoiding /= (_float)dir_vector.size();
	average_dir_for_avoiding = XMVector3Normalize(average_dir_for_avoiding);

	m_vAvoidingDirection = average_dir_for_avoiding;
}

void CPlayer::ChangeState()
{
	SORA_ANIM temp_anim = (SORA_ANIM)m_pModelCom->Get_TempIndex_EachBody(SB_BODY);
	SORA_ANIM curr_anim = (SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(SB_BODY);

	// Form ���� ��ü�� �����ϴµ� �ִϸ��̼��� RAGE_FORM(��ȭ �ִϸ��̼�)�� ��
	_bool changing_form = false;
	if (true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_FORM))
	{
		if (SORA_ANIM::SORA_ANIM_RAGE_FORM == curr_anim ||
			SORA_ANIM::SORA_ANIM_RAGE_FORM == temp_anim)
			changing_form = true;

		if (SORA_ANIM::SORA_ANIM_HAMMER_FORM == curr_anim ||
			SORA_ANIM::SORA_ANIM_HAMMER_FORM == temp_anim)
			changing_form = true;
	}

	// 0. ��� : �ƹ��� ó���� ���� �ʴ´�.

	// 1. �̵�
	if (true == m_bInput[SI_WASD])
	{
		_bool move_state = true;

		// �Է� �˻�
		if (true == m_bInput[SI_LBUTTON] ||
			true == m_bInput[SI_RBUTTON])
			move_state = false;

		// �� ��ȯ ������ �˻�
		if (true == changing_form)
			move_state = false;

		// �׼� ��ý���Ʈ MoveAble �˻�
		if (false == m_pAction_Assistant->Able(A_ASSIST::ASSIST_MOVE))
			move_state = false;

		// ���� ���ǰ˻�
		if (true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_DYNAMICMOVE) ||		// ���̳��� ���� ���� ��� ���� false
			true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_ATK)			||		// ���� ���� ��� ���� false
			true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_SUMMON)		||		// ��ȯ ���� ��� ���� false
			true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_GUARD)		||		// ���� ���� ��� ���� false
			true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_DAMAGED)		||		// �ǰ� ���� ��� ���� false
			true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_INTERACTION))
			move_state = false;

		// �ǴϽ� ���� �� ����ó��
		if (true == Check_OnFinish())
			move_state = false;

		if (true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_AVOID))
		{
			// ȸ�� ���� ��� ���� false
			_bool air = m_pTransformCom->Get_JumpBoolean();

			if (true == air ||
				false == m_pAction_Assistant->Able(A_ASSIST::ASSIST_MOVE))
				move_state = false;

			// Able �˻縦 ���ؼ� move ���¸� ������ �� ������ Ȯ�εǸ� ������ avoid ���¸� �����Ѵ�
			if (true == move_state)
				m_pStateMachineCom->ClearState(CStateMachine::STATETYPE_AVOID);
		}

		// ���� ���� ���� �˻�
		if (true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_NORMALMOVE))
			move_state = false;

		// �̵� ���� ����
		if (true == move_state)
		{
			Create_PlayerState(CStateMachine::STATETYPE_NORMALMOVE);
		}
	}

	// 2. ����
	if (true == m_bInput[SI_SPACE])
	{
		_bool jump_state = true;

		// �÷��̾ ������ ������ ���¸� Ȯ���ϰ� �����ؾ��Ѵ�.

		// 1. �Է� �˻�
		if (true == m_bInput[SI_LBUTTON] ||
			true == m_bInput[SI_RBUTTON])
			jump_state = false;

		// �� ��ȯ ������ �˻�
		if (true == changing_form)
			jump_state = false;

		// 2. ���� ���ǰ˻�
		if (true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_DYNAMICMOVE) ||		// ���̳��� ���� ���� ��� ���� false
			true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_ATK)			||		// ���� ���� ��� ���� false
			true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_SUMMON)		||		// ��ȯ ���� ��� ���� false
			true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_GUARD)		||		// ���� ���� ��� ���� false
			true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_AVOID)		||		// ȸ�� ���� ��� ���� false
			true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_DAMAGED)		||		// �ǰ� ���� ��� ���� false
			true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_INTERACTION))
			jump_state = false;

		// �ǴϽ� ���� �� ����ó��
		if (true == Check_OnFinish())
			jump_state = false;

		// ���� ���� ���� �˻�
		if (true == jump_state && true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_JUMP))
			jump_state = false;

		// ���� ���� ����
		if (true == jump_state)
		{
			m_pTransformCom->BasicJump(CTransform::JUMP_NORMAL);

			CState_Jump::JUMP_STATE_DESC jump_state_desc;
			jump_state_desc.eObjectType       = m_eObjectType;
			jump_state_desc.iJumpAnimIndex    = 0;
			jump_state_desc.iNumberOfJumpAnim = 4;

			Create_PlayerState(CStateMachine::STATETYPE_JUMP, &jump_state_desc);
		}
	}

	// 3. ����
	if (true == m_bInput[SI_LBUTTON])
	{
		_bool attack_state = true;

		// 0. keyblade lock
		if(true == GM->Get_KeyBladeLock())
			attack_state = false;

		// 1. �Է� �˻� ========================================================================================
		if (true == m_bInput[SI_RBUTTON])
			attack_state = false;

		// �� ��ȯ ������ �˻�
		if (true == changing_form)
			attack_state = false;

		// =====================================================================================================
		// 2. ���� ���� �˻� ===================================================================================

		// 2-1. ���� ����
		if (true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_GUARD))
		{
			// SUCCEEDED�� �ƴ� ��� ���ݻ��¸� ������ �� ����
			if (SORA_ANIM_GUARD_SUCCEEDED != curr_anim && SORA_ANIM_AIR_GUARD_SUCCEEDED != curr_anim)
				attack_state = false;

			// SUCCEEDED�� ��, Able(ASSIST_CANCLE)�� true�� �ƴ϶�� ���ݻ��¸� ������ �� ����
			else
			{
				if(false == m_pAction_Assistant->Able(A_ASSIST::ASSIST_CANCLE))
					attack_state = false;
			}
		}

		// =====================================================================================================
		// 2-2. ȸ�� ����
		if (true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_AVOID))		
		{
			attack_state = false;

			//// ������ �� ����. (����� ���� ��ø� ������ �ɾ�� ����)
			//_bool air = m_pTransformCom->Get_JumpBoolean();
			//
			//// false == air -> ������ ����. ������ ���Ѵ�ø� ���� ���� �ӽ÷� ���Ƶ���(���Ŀ��� ���� ȸ�� ������ �� Able(ASSIST_CANCLE) �˻��ؾ��Ѵ�)
			//if (false == air)
			//	attack_state = false;
		}

		// =====================================================================================================
		// 2-3. ���� ����
		if (true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_ATK))
			attack_state = false;

		// 2-4. �ǰ� ����
		if(true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_DAMAGED))
			attack_state = false;

		// 2-5. ���̳��� ����
		if(true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_DYNAMICMOVE))
			attack_state = false;

		// 2-6. ��ȣ�ۿ�
		if(true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_INTERACTION))
			attack_state = false;

		if (true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_SUMMON))
		{
			if (false == m_pAction_Assistant->Able(A_ASSIST::ASSIST_CANCLE) ||
				false == m_pAction_Assistant->Able(A_ASSIST::ASSIST_ATTACK))
				attack_state = false;
		}

		// 3. Able �˻�
		if(false == m_pAction_Assistant->Able(A_ASSIST::ASSIST_MOVE))
			attack_state = false;

		// 4. ���� ���� ����
		if (true == attack_state)
		{
			Create_PlayerState(CStateMachine::STATETYPE_ATK);
		}
	}

	if (true == m_bInput[SI_RBUTTON])
	{
		// 4. ���
		if (false == m_bInput[SI_WASD])
		{
			_bool guard_state = true;

			// 0. keyblade lock
			if (true == GM->Get_KeyBladeLock())
				guard_state = false;

			// 1. �Է� �˻� : ����

			// �� ��ȯ ������ �˻�
			if (true == changing_form)
				guard_state = false;

			// 2. ���� ���� �˻�
			// 2-1. �̹� ��� ���̰ų� �̹� ȸ�� ���� ��� ��� ���¸� ������ �� ����.
			if (true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_GUARD) ||
				true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_SUMMON) ||
				true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_DYNAMICMOVE) ||
				true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_DAMAGED) ||
				true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_AVOID) ||
				true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_INTERACTION))
			{
				guard_state = false;
			}

			// �ǴϽ� ���� �� ����ó��
			if (true == Check_OnFinish())
				guard_state = false;

			// 3. ��� ���� ����
			if (true == guard_state)
			{
				Create_PlayerState(CStateMachine::STATETYPE_GUARD);
			}
		}

		// 5. ȸ��
		else
		{
			_bool avoid_state = true;

			// 1. �Է� �˻� : ����

			// �� ��ȯ ������ �˻�
			if (true == changing_form)
				avoid_state = false;

			// 2. ���� ���� �˻�
			if (true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_GUARD))
			{
				if (SORA_ANIM_GUARD_SUCCEEDED != curr_anim &&
					SORA_ANIM_AIR_GUARD_SUCCEEDED != curr_anim)
				{
					avoid_state = false;
				}
			}

			// ĵ�� ������ ���°� �ƴ� ��� ��������
			if(false == m_pAction_Assistant->Able(A_ASSIST::ASSIST_CANCLE))
				avoid_state = false;

			// ���� ���¿� ���� �˻�

			if (true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_DYNAMICMOVE) ||
				true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_SUMMON) ||
				true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_DAMAGED) ||
				true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_AVOID) ||
				true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_INTERACTION))
			{
				avoid_state = false;
			}

			// �ǴϽ� ���� �� ����ó��
			if (true == Check_OnFinish())
				avoid_state = false;

			if (true == avoid_state)
			{
				Create_PlayerState(CStateMachine::STATETYPE_AVOID);
			}
		}
	}

	// 7. �ǰ�
	// onCollision()���� ����

	// 8. �����̵�
	if (true == m_bInput[SI_WASD])
	{
		_bool dynamic_move_state = true;

		// 1. �Է� �˻�
		if (true == m_bInput[SI_LBUTTON] ||
			true == m_bInput[SI_RBUTTON])
			dynamic_move_state = false;

		// �� ��ȯ ������ �˻�
		if (true == changing_form)
			dynamic_move_state = false;

		if(true == m_bHovering)
			dynamic_move_state = false;

		// 2. ���� ���� �˻�
		if (true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_DAMAGED) ||
			true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_ATK) ||
			true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_GUARD) ||
			true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_AVOID) ||
			true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_INTERACTION))
			dynamic_move_state = false;

		if (nullptr != m_pNavigationCom)
		{
			// 3. Ư�� ���� �˻�
			if (false == m_pNavigationCom->Get_Climb())
				dynamic_move_state = false;

			if (CCell::TYPE_RUNNING != m_pNavigationCom->Get_CurrentType())
				dynamic_move_state = false;
		}
		
		if(true == dynamic_move_state)
			Create_PlayerState(CStateMachine::STATETYPE_DYNAMICMOVE);
	}

	// 9. ��ȯ
	if (true == m_bInput[SI_SUMMON])
	{
		/*
			���� ���� �˻��ؾ��ϴ� �������� ���
			- idle : ����
			- normal move : ����
			- dynamic move : �Ұ�
			- jump : ����
			- attack : ĵ��, ���� ������ ���� ����
			- summon : ���� ����, �ߺ������ ���´�
			- guard : ĵ��, ���� ������ ���� ����(���� ���� ��)
			- avoid : �Ұ�
			- damaged : �Ұ�
		*/

		_bool summon_state = true;

		// 0. keyblade lock
		if (true == GM->Get_KeyBladeLock())
			summon_state = false;

		// 1. �Է� �˻�
		if (SORA_MAGIC::SM_END == m_eMagic)
			summon_state = false;

		// �� ��ȯ ������ �˻�
		if (true == changing_form)
			summon_state = false;

		// 2. ���� �˻�
		if (true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE::STATETYPE_DYNAMICMOVE)	||
			true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE::STATETYPE_AVOID)		||
			true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE::STATETYPE_DAMAGED)		||
			true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE::STATETYPE_SUMMON)		||
			true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_INTERACTION))
			summon_state = false;
		
		// ����, ���� ������ �� cancle, attack�� able���¸� Ȯ���Ͽ� �׿� ���� ó��
		if (true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE::STATETYPE_ATK) ||
			true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE::STATETYPE_GUARD))
		{
			if (false == m_pAction_Assistant->Able(A_ASSIST::ASSIST_CANCLE) ||
				false == m_pAction_Assistant->Able(A_ASSIST::ASSIST_ATTACK))
				summon_state = false;
		}

		// �ǴϽ� ���� �� ����ó��
		if (true == Check_OnFinish())
			summon_state = false;

		if(true == summon_state)
			Create_PlayerState(CStateMachine::STATETYPE_SUMMON);
	}

	// 10. ��ȣ�ۿ�
	if (true == m_bInput[SI_EVENT])
	{
		_bool interaction_state = true;

		// 0. Keyblade Lock -> ���ڿ��� �ִϸ��̼ǵ� Ű ���̵� ���� �Ŀ� �� �� �ִ�.
		if (true == GM->Get_KeyBladeLock())
			interaction_state = false;
		
		// 1. �ٸ� Ű �Է��� �ִ��� �˻�
		// �� �� ����.

		// �� ��ȯ ������ �˻�
		if (true == changing_form)
			interaction_state = false;

		// 2. ���� ���� �˻�
		// ����μ��� ���ڿ��⸸ ��ȣ�ۿ� ���·� ���´�. ���ڿ���� ��� ������ ���� �����ϴ�.
		// ���� ������ ������ Ȯ������.
		if(1 != m_pStateMachineCom->Get_NumberOfStates())
			interaction_state = false;

		if (true == interaction_state)
			Create_PlayerState(CStateMachine::STATETYPE_INTERACTION);
	}
}

void CPlayer::ChangeForm()
{
	// ü�� ����, UI �˾� ���� �� ����
	// FŰ �Է� ���� ���°� �ƴ϶�� return
	if (false == m_bInput[SI_F])
		return;

	// �� ���� Ŀ�ǵ�(LShift + F�Է�)���� �������� ������ �� ����
	CGameInstance* pGI = CGameInstance::GetInstance();
	if (pGI->Get_KeyEnter(DIK_LSHIFT))
		return;

	// ��������, ���ͷ��� �߿��� ���� �Ұ����ϰ� ����
	if (true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_DYNAMICMOVE) ||
		true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_INTERACTION))
		return;

	// ���� ���� Ÿ���� �޾Ƶд�
	CPlayer_Weapon::WEAPON_TYPE weapon_type = m_pWeapon->WeaponType();

	// ���� �������� ������ �޾Ƶд�
	LEVEL current_level = GM->Get_CurrentLevel();

	switch (m_eForm)
	{
	case Client::CPlayer::SF_NORMAL:

		// ����� ���� Ÿ���� �˻��ؼ� �� ü������ ������
		// FAVORITE_DEPUTY ���¿����� ������ ���� ����� �� �ִ�
		// ������ ���� �� ü���� ������ �˻��ؾ��Ѵ�

		switch (weapon_type)
		{
		case Client::CPlayer_Weapon::WEAPON_TYPE_KINGDOM_KEY:

			if (true == m_bFormChangeLock)
			{
				// �������� ���������� �ƴ� ��� return
				if (LEVEL_FINALBOSS != current_level)
					return;
			}

			// ������ �� ���� ������ �������� ���� ��� return
			if (m_iRageForm_GaugeMax != m_iRageForm_Gauge)
				return;

			m_eForm = SORA_FORM::SF_RAGE;
			m_iRageForm_Gauge = 0;

			break;
		case Client::CPlayer_Weapon::WEAPON_TYPE_FAVORITE_DEPUTY:

			if (true == m_bFormChangeLock)
			{
				// Ʈ���϶��� Ÿ���� ��� return
				if (LEVEL_TWILIGHT == current_level)
					return;
			}

			// �ظ� �� ���� ������ �������� ���� ��� return
			if (m_iHammerForm_ComboMax > m_iHammerForm_Combo || m_iHammerForm_ComboMax > (_int)m_iCombo)
				return;

			m_eForm = SORA_FORM::SF_HAMMER;
			m_iHammerForm_Combo = 0;

			break;
		}

		break;
	case Client::CPlayer::SF_HAMMER:

		// ������ �� ��ȭ ���� Ȯ��
		// ����� �׳� return ��Ų��
		return;
	case Client::CPlayer::SF_RAGE:

		// ������ ���� ������ �ٸ� ������ ��ȯ�� �� ����
		return;
	}

	// ��� ���� ����
	m_pStateMachineCom->ClearState(CStateMachine::STATETYPE_NORMALMOVE);
	m_pStateMachineCom->ClearState(CStateMachine::STATETYPE_JUMP);
	m_pStateMachineCom->ClearState(CStateMachine::STATETYPE_ATK);
	m_pStateMachineCom->ClearState(CStateMachine::STATETYPE_SUMMON);
	m_pStateMachineCom->ClearState(CStateMachine::STATETYPE_GUARD);
	m_pStateMachineCom->ClearState(CStateMachine::STATETYPE_AVOID);
	m_pStateMachineCom->ClearState(CStateMachine::STATETYPE_DAMAGED);

	// ���� �߰�
	Create_PlayerState(CStateMachine::STATETYPE_FORM);
}

void CPlayer::Effect_Control()
{
	// ���� Ʈ����
	if (true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_ATK))
	{
		// ����Ʈ ��ġ ��ũ ���߰�, �߰����� ����Ʈ�� ����ٸ� �Ϲ� Ʈ���� ���۴� inActive
		// ���� : �Ϲ� Ʈ���� ���۰� ����� ȭ���� ���̱� ������ �޽� Ʈ������ ��������� �������� ���� �� ����

		//CState_PlayerAttack* attack_state = static_cast<CState_PlayerAttack*>(m_pStateMachineCom->Get_State(CStateMachine::STATETYPE_ATK));
		//if (false == attack_state->EffectCreated())
		//m_pWeapon->Set_Trail_Active(true);
		//else
		//m_pWeapon->Set_Trail_Active(false);

		//m_pWeapon->Set_Trail_Active(true);
	}

	// �ٵ� Ʈ����(��, �߿��� �ٶ� �����µ��� ȿ��)
	_bool on_rage_form = m_eForm == SF_RAGE ? true : false;
	_bool body_trail = on_rage_form;

	SORA_ANIM curr_anim = (SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(SB_BODY);

	for (_uint i = 0; i < CStateMachine::STATETYPE_END; ++i)
	{
		if (false == m_pStateMachineCom->FindState((CStateMachine::STATETYPE)i))
			continue;

		switch (CStateMachine::STATETYPE(i))
		{
		case Engine::CStateMachine::STATETYPE_JUMP:

			if (SORA_ANIM_SECOND_JUMP_UP == curr_anim ||
				SORA_ANIM_HAMMER_SECOND_JUMP_UP == curr_anim)
			{
				body_trail = true;
			}

			break;
		case Engine::CStateMachine::STATETYPE_GUARD:

			if (SORA_ANIM_GUARD == curr_anim ||
				SORA_ANIM_GUARD_SUCCEEDED == curr_anim ||
				SORA_ANIM_AIR_GUARD == curr_anim ||
				SORA_ANIM_AIR_GUARD_SUCCEEDED == curr_anim ||
				SORA_ANIM_HAMMER_GUARD == curr_anim ||
				SORA_ANIM_HAMMER_GUARD_SUCCEEDED == curr_anim ||
				SORA_ANIM_HAMMER_AIR_GUARD == curr_anim ||
				SORA_ANIM_HAMMER_AIR_GUARD_SUCCEEDED == curr_anim)
			{
				body_trail = true;
			}

			break;
		case Engine::CStateMachine::STATETYPE_AVOID:

			if (SORA_ANIM_ROLL == curr_anim ||
				SORA_ANIM_AIR_DASH == curr_anim ||
				SORA_ANIM_HAMMER_DASH == curr_anim)
			{
				body_trail = true;
			}

			break;
		}
	}
	
	if (true == body_trail)
	{
		for (_uint j = 0; j < 4; ++j)
		{
			m_pTrail[j]->ResetColor();
			m_pTrail[j]->Set_Active(true);
		}

		if (true == on_rage_form)
		{
			for (_uint j = 0; j < 4; ++j)
				m_pTrail[j]->ChangeColor(_float4(0.f));
		}
	}
	else
	{
		for (_uint j = 0; j < 4; ++j)
			m_pTrail[j]->Set_Active(false);
	}
}

void CPlayer::CellEntry_Processing()
{
	if (nullptr != m_pNavigationCom)
	{
		// ���� Ÿ���� �̹� �����ӿ� �ٲ���� ��
		CCell* current_cell = m_pNavigationCom->Get_CurrentCell();
		if (nullptr == current_cell)
		{
			BREAKPOINT;
		}

		// ���� �������� �� Ÿ�԰� ���� �������� �� Ÿ�� ��. ������ true �ٸ��� false
		_bool cell_type_same = m_pNavigationCom->Check_PrevAndCurrFrame();
		if (true == cell_type_same)	return;

		// �̹� �����ӿ� �ٲ� ���� ���� ó�� ���� ���� ���� ó���� Ÿ�Ժ��� ����
		CCell::TYPE current_cell_type = current_cell->Get_Type();
		switch (current_cell_type)
		{
		case Engine::CCell::TYPE_TERRAIN:
			break;
		case Engine::CCell::TYPE_ROAD:
			break;
		case Engine::CCell::TYPE_NOTROAD:
			break;
		case Engine::CCell::TYPE_RUNNING:
		{
			_float current_position_y = vecy(m_pTransformCom->Get_Position());
			_float wall_height = current_cell->HighestY();

			// RUNNING Ÿ���� �����µ� �ö󰡴� ��Ȳ�� ��
			if (current_position_y < wall_height &&
				true == m_pNavigationCom->Get_Climb())
			{
				// ���� ������ �̿��� �÷��̾��� ���������� �����ϱ� ���� ���� ��ġ�� ���� ����
				CCell* current_cell = m_pNavigationCom->Get_CurrentCell();

				// ���� ���缭 Right, Up, Look ���� ����
				// ���� ���� : ���� ���� -��� �������� �÷��̾��� Look�� �����Ѵ�
				_vector new_look_norm = _vec3norm(XMLoadFloat3(&current_cell->Get_NormalVector()) * -1.f);
				m_pTransformCom->MakeSameDirection(new_look_norm);

				// ��ġ ���� : ���� ����� ���ΰ��� �Ÿ��� �����ϰ� �ش� �Ÿ���ŭ �����ִ� ������ �ݴ�������� �о��
				_float dist = current_cell->Get_CloseLine(m_pTransformCom->Get_Position(), nullptr, true);
				_vector new_position = m_pTransformCom->Get_Position() + (_vec3norm(XMLoadFloat3(&current_cell->Get_NormalVector())) * dist * 1.1f);
				m_pTransformCom->Set_Position(new_position);
			}

			// RUNNING Ÿ���� �����µ� �������� ��Ȳ�� ��
			else
			{
				_bool jump_state = true;

				if (true == m_bInput[SI_LBUTTON] ||
					true == m_bInput[SI_RBUTTON])
					jump_state = false;

				// 2. ���� ���ǰ˻�
				if (true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_JUMP)  ||	// �̹� ���� ���� ��� ���� false
					true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_ATK)	  ||	// ���� ���� ��� ���� false
					true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_GUARD) ||	// ���� ���� ��� ���� false
					true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_AVOID))		// ȸ�� ���� ��� ���� false
					jump_state = false;

				// ���� ���� ����
				if (true == jump_state)
				{
					m_pStateMachineCom->ClearState(CStateMachine::STATETYPE_DYNAMICMOVE);

					m_pTransformCom->BasicJump(CTransform::JUMP_FALL);

					CState_Jump::JUMP_STATE_DESC jump_state_desc;
					jump_state_desc.eObjectType = m_eObjectType;
					jump_state_desc.iJumpAnimIndex = 0;
					jump_state_desc.iNumberOfJumpAnim = 4;

					Create_PlayerState(CStateMachine::STATETYPE_JUMP, &jump_state_desc);
				}
			}
		}
		break;
		case Engine::CCell::TYPE_FLOW:
			break;
		}
	}
}

void CPlayer::Set_LocationAssistance_MoveAmountY(_fvector vPoint)
{
	LEVEL curr_level = GM->Get_CurrentLevel();
	_vector target_collider_center = vPoint;
	vecy(target_collider_center) -= Calculate_GapY_Between_PositionAndCenter();

	_vector player_position = m_pTransformCom->Get_Position();
	_float gap_y = vecy(target_collider_center) - vecy(player_position);

	if (1.f > fabsf(gap_y))
	{
		m_fMoveAmountY_OnAttack = 0.f;
		return;
	}

	// �̵� �������� ���� �÷��̾��� ��ġ���� ���� �� - ���� ���ϴ� �̵�
	if (vecy(target_collider_center) >= vecy(player_position))
	{
		m_fMoveAmountY_OnAttack = vecy(target_collider_center) - vecy(player_position);

		// �ִ� ���� ����
		if (3.f < m_fMoveAmountY_OnAttack)
			m_fMoveAmountY_OnAttack = 3.f;
	}

	// �̵� �������� ���� �÷��̾��� ��ġ���� ���� �� - �Ʒ��� ���ϴ� �̵�
	else
	{
		// �ٴ� ������ �������� ����� ���� ó���ؾ��Ѵ�
		// ��Ǭ�� ������ ������ ���������� ��
		if (LEVEL_RAPUNZEL_BOSS != curr_level)
		{
			// Navigation �˻�
			CCell* curr_cell = m_pNavigationCom->Get_CurrentCell();

			if (nullptr == curr_cell)
				_breakpoint;

			_float height_on_cell = curr_cell->Compute_Y(target_collider_center);

			if (height_on_cell >= vecy(target_collider_center))
				vecy(target_collider_center) = height_on_cell;

			m_fMoveAmountY_OnAttack = vecy(target_collider_center) - vecy(player_position);
		}

		// ��Ǭ�� ������ ��
		else
		{
			// Terrain �˻�
			CTerrain* curr_terrain = static_cast<CTerrain*>(m_pGameInstance->Get_Back(curr_level, TEXT("Layer_Terrain")));

			if (nullptr == curr_terrain)
				_breakpoint;

			_vector position_on_terrain = curr_terrain->Compute_PositionOnTerrain(target_collider_center);

			if (vecy(position_on_terrain) >= vecy(target_collider_center))
				vecy(target_collider_center) = vecy(position_on_terrain);

			m_fMoveAmountY_OnAttack = vecy(target_collider_center) - vecy(player_position);
		}
	}
}

void CPlayer::Set_LocationAssistance_InputValues(SORA_ANIM eTempIndex)
{
	for (auto anim_enum : m_Animation_MoveOnAxisY)
	{
		if ((SORA_ANIM)eTempIndex != anim_enum)
			continue;

		_double anim_play_speed = m_pModelCom->Get_TargetPlaySpeed(eTempIndex);
		_double location_assist_time = 0.0;

		switch (eTempIndex)
		{
		case Client::CPlayer::SORA_ANIM_AIR_ATTACK1:
			location_assist_time = 8.0;
			break;
		case Client::CPlayer::SORA_ANIM_AIR_ATTACK2:
			location_assist_time = 8.0;
			break;
		case Client::CPlayer::SORA_ANIM_AIR_ATTACK4:
			location_assist_time = 17.0;
			break;
		case Client::CPlayer::SORA_ANIM_UPPER_ATTACK1:
			location_assist_time = 8.0;
			break;
		case Client::CPlayer::SORA_ANIM_UPPER_ATTACK2:
			location_assist_time = 7.0;
			break;
		case Client::CPlayer::SORA_ANIM_UPPER_ATTACK4:
			location_assist_time = 18.0;
			break;
		case Client::CPlayer::SORA_ANIM_LOWER_ATTACK1:
			location_assist_time = 6.0;
			break;
		case Client::CPlayer::SORA_ANIM_LOWER_ATTACK2:
			location_assist_time = 7.0;
			break;
		case Client::CPlayer::SORA_ANIM_LOWER_ATTACK3:
			location_assist_time = 7.0;
			break;
		case Client::CPlayer::SORA_ANIM_LOWER_ATTACK4:
			location_assist_time = 12.0;
			break;
		case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_02:
			location_assist_time = 6.0;
			break;
		case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_03:
			location_assist_time = 27.0;
			break;
		case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_04:
			location_assist_time = 18.0;
			break;
		case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_06:
			// ���� �ʿ�
			break;
		}

		location_assist_time *= m_dAxisY_ChasingSpeed;
		m_pTransformCom->LocationAssistanceInput(m_fMoveAmountY_OnAttack, location_assist_time, anim_play_speed);
		break;
	}
}

void CPlayer::LocationAssistance_Tick(_double TimeDelta)
{
	SORA_ANIM curr_anim = (SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(SB_BODY);
	_double time_acc = m_pModelCom->Get_TargetTimeAcc(curr_anim);

	for (auto anim_enum : m_Animation_MoveOnAxisY)
	{
		if (anim_enum != curr_anim)
			continue;

		switch (curr_anim)
		{
		case Client::CPlayer::SORA_ANIM_AIR_ATTACK1:

			if (time_acc >= 5.0)
				m_pTransformCom->LocationAssistanceTick(TimeDelta, m_pNavigationCom);

			break;
		case Client::CPlayer::SORA_ANIM_AIR_ATTACK2:

			if (time_acc >= 4.0)
				m_pTransformCom->LocationAssistanceTick(TimeDelta, m_pNavigationCom);

			break;
		case Client::CPlayer::SORA_ANIM_AIR_ATTACK4:

			if (time_acc >= 6.0)
				m_pTransformCom->LocationAssistanceTick(TimeDelta, m_pNavigationCom);

			break;
		case Client::CPlayer::SORA_ANIM_UPPER_ATTACK1:

			if (time_acc >= 5.0)
				m_pTransformCom->LocationAssistanceTick(TimeDelta, m_pNavigationCom);

			break;
		case Client::CPlayer::SORA_ANIM_UPPER_ATTACK2:

			if (time_acc >= 2.0)
				m_pTransformCom->LocationAssistanceTick(TimeDelta, m_pNavigationCom);

			break;
		case Client::CPlayer::SORA_ANIM_UPPER_ATTACK4:

			if (time_acc >= 6.0)
				m_pTransformCom->LocationAssistanceTick(TimeDelta, m_pNavigationCom);

			break;
		case Client::CPlayer::SORA_ANIM_LOWER_ATTACK1:

			if (time_acc >= 10.0)
				m_pTransformCom->LocationAssistanceTick(TimeDelta, m_pNavigationCom);

			break;
		case Client::CPlayer::SORA_ANIM_LOWER_ATTACK2:

			if (time_acc >= 9.0)
				m_pTransformCom->LocationAssistanceTick(TimeDelta, m_pNavigationCom);

			break;
		case Client::CPlayer::SORA_ANIM_LOWER_ATTACK3:

			if (time_acc >= 9.0)
				m_pTransformCom->LocationAssistanceTick(TimeDelta, m_pNavigationCom);

			break;
		case Client::CPlayer::SORA_ANIM_LOWER_ATTACK4:

			if (time_acc >= 6.0)
				m_pTransformCom->LocationAssistanceTick(TimeDelta, m_pNavigationCom);

			break;
		case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_02:

			if (time_acc >= 17.0)
				m_pTransformCom->LocationAssistanceTick(TimeDelta, m_pNavigationCom);

			break;
		case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_03:

			if (time_acc >= 4.0)
				m_pTransformCom->LocationAssistanceTick(TimeDelta, m_pNavigationCom);

			break;
		case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_04:

			if (time_acc >= 10.0)
				m_pTransformCom->LocationAssistanceTick(TimeDelta, m_pNavigationCom);

			break;
		case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_06:

			// ���� �ʿ�

			break;
		}

		break;
	}

	// �����̼� ��ý��Ͻ��� ���� -> �������� �̵����� �� ���� ���� �߻�
	_bool air = m_pTransformCom->Get_JumpBoolean();
	if (false == air)
	{
		if (0 < m_fMoveAmountY_OnAttack)
		{
			m_pTransformCom->BasicJump(CTransform::JUMP_FALL);

			CState_Jump::JUMP_STATE_DESC jump_state_desc;
			jump_state_desc.eObjectType = m_eObjectType;
			jump_state_desc.iJumpAnimIndex = 2;
			jump_state_desc.iNumberOfJumpAnim = 4;
			jump_state_desc.bChangeAnimation = false;

			Create_PlayerState(CStateMachine::STATETYPE_JUMP, &jump_state_desc);
		}
	}
}

void CPlayer::Show_UI_FormChange()
{
	CPlayer_Weapon::WEAPON_TYPE weapon_type = m_pWeapon->WeaponType();
	LEVEL current_level = GM->Get_CurrentLevel();

	// �ظ� �� ���� UI 
	if (m_iHammerForm_ComboMax <= m_iHammerForm_Combo && m_iHammerForm_ComboMax <= (_int)m_iCombo && nullptr == m_pFormFunctionUI[FORM_FUNCTION::FORM_FUNC_HAMMERFORM])
	{
		if (SORA_FORM::SF_NORMAL == m_eForm && CPlayer_Weapon::WEAPON_TYPE::WEAPON_TYPE_FAVORITE_DEPUTY == weapon_type)
		{
			if (true == m_bFormChangeLock)
			{
				if (LEVEL_TWILIGHT != current_level)
				{
					_uint ui_index_hammer_form = FORM_FUNCTION::FORM_FUNC_HAMMERFORM + 1;
					m_pFormFunctionUI[FORM_FUNCTION::FORM_FUNC_HAMMERFORM] = m_pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_FormChange"), &ui_index_hammer_form);

					// ���� �÷���
					Play_SideUI_PopUp_Sound();
				}
			}
			else
			{
				_uint ui_index_hammer_form = FORM_FUNCTION::FORM_FUNC_HAMMERFORM + 1;
				m_pFormFunctionUI[FORM_FUNCTION::FORM_FUNC_HAMMERFORM] = m_pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_FormChange"), &ui_index_hammer_form);

				// ���� �÷���
				Play_SideUI_PopUp_Sound();
			}
		}
	}

	// �ظ� �� ���� UI ����
	if (nullptr != m_pFormFunctionUI[FORM_FUNCTION::FORM_FUNC_HAMMERFORM])
	{
		if (m_iHammerForm_ComboMax > m_iHammerForm_Combo || m_iHammerForm_ComboMax > (_int)m_iCombo)
		{
			static_cast<CUI_FormChange*>(m_pFormFunctionUI[FORM_FUNCTION::FORM_FUNC_HAMMERFORM])->Set_Dead(true);
			m_pFormFunctionUI[FORM_FUNCTION::FORM_FUNC_HAMMERFORM] = nullptr;
		}
	}

	// ������ �� ���� UI
	if (m_iRageForm_GaugeMax <= m_iRageForm_Gauge && nullptr == m_pFormFunctionUI[FORM_FUNCTION::FORM_FUNC_RAGEFORM])
	{
		if (SORA_FORM::SF_NORMAL == m_eForm && CPlayer_Weapon::WEAPON_TYPE::WEAPON_TYPE_KINGDOM_KEY == weapon_type)
		{
			if (true == m_bFormChangeLock)
			{
				if (LEVEL_FINALBOSS == current_level)
				{
					_uint ui_index_rage_form = FORM_FUNCTION::FORM_FUNC_RAGEFORM + 1;
					m_pFormFunctionUI[FORM_FUNCTION::FORM_FUNC_RAGEFORM] = m_pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_FormChange"), &ui_index_rage_form);

					// ���� �÷���
					Play_SideUI_PopUp_Sound();
				}
			}
			else
			{
				_uint ui_index_rage_form = FORM_FUNCTION::FORM_FUNC_RAGEFORM + 1;
				m_pFormFunctionUI[FORM_FUNCTION::FORM_FUNC_RAGEFORM] = m_pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_FormChange"), &ui_index_rage_form);

				// ���� �÷���
				Play_SideUI_PopUp_Sound();
			}
		}
	}

	//// �ظ� �� �ǴϽ� ����
	//if (SORA_FORM::SF_HAMMER == m_eForm)
	//{
	//	if (true == m_bForm_AbleToFinish && nullptr == m_pFormFunctionUI[FORM_FUNCTION::FORM_FUNC_HAMMER_FINISH])
	//	{
	//		_uint ui_index_hammer_finish = FORM_FUNCTION::FORM_FUNC_HAMMER_FINISH + 1;
	//		m_pFormFunctionUI[FORM_FUNCTION::FORM_FUNC_HAMMER_FINISH] = m_pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_FormChange"), &ui_index_hammer_finish);
	//	}
	//}

	// ������ �� �ǴϽ� ����
	if (SORA_FORM::SF_RAGE == m_eForm)
	{
		// �ǴϽ� ����
		if (true == m_bForm_AbleToFinish && nullptr == m_pFormFunctionUI[FORM_FUNCTION::FORM_FUNC_RAGE_FINISH])
		{
			_uint ui_index_rage_finish = FORM_FUNCTION::FORM_FUNC_RAGE_FINISH + 1;
			m_pFormFunctionUI[FORM_FUNCTION::FORM_FUNC_RAGE_FINISH] = m_pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_FormChange"), &ui_index_rage_finish);

			// ���� �÷���
			Play_SideUI_PopUp_Sound();
		}

		SORA_ANIM curr_anim = (SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(SB_BODY);

		// ī���� ����
		if (SORA_ANIM_RAGE_GUARD_SUCCEEDED == curr_anim)
		{
			_uint ui_index_rage_counter_attack = FORM_FUNCTION::FORM_FUNC_COUNTER_ATTACK + 1;
			m_pFormFunctionUI[FORM_FUNCTION::FORM_FUNC_COUNTER_ATTACK] = m_pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_FormChange"), &ui_index_rage_counter_attack);
		
			_double guard_succeeded_duration = m_pModelCom->Get_TargetDuration(SORA_ANIM_RAGE_GUARD_SUCCEEDED);
			_double guard_succeeded_play_speed = m_pModelCom->Get_TargetPlaySpeed(SORA_ANIM_RAGE_GUARD_SUCCEEDED);
		
			static_cast<CUI_FormChange*>(m_pFormFunctionUI[FORM_FUNCTION::FORM_FUNC_COUNTER_ATTACK])->Set_CounterAttackUI(guard_succeeded_duration * 0.1, guard_succeeded_play_speed);
		}
	}
}

void CPlayer::Teleport_Dissolve(_double TimeDelta)
{
	SORA_ANIM curr_anim = (SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(SB_BODY);
	if (SORA_ANIM_RAGE_JUMP_UP == curr_anim && false == m_bTeleportDissolve)
	{
		m_bTP = true;
		for (_uint i = 0; i < 4; i++)
		{
			m_pTrail[i]->Set_Render(false);
		}
		m_fTime = (_float)m_pModelCom->Get_CurrTimeAcc_EachBody(SB_BODY) / 3.f;
		if (0.9f < m_fTime)
		{
			m_fTime = 1.f;
			m_bTeleportDissolve = true;
		}
	}
	else if (true == m_bTeleportDissolve)
	{
		m_fTime -= (_float)TimeDelta * 5.f;
		if (0.f > m_fTime)
		{
			m_bTeleportDissolve = false;
			m_fTime = 0.f;
			m_bTP = false;
			for (_uint i = 0; i < 4; i++)
			{
				m_pTrail[i]->Set_Render(true);
			}
		}
	}
}

void CPlayer::OnCollision()
{
	_float hp_before_collision = m_pStatusCom->Get_HP();

	auto pColliderList = m_pGameInstance->Get_ColliderList(string("Monster_Atk"));
	for (auto& pAttackerCollider : *pColliderList)
	{
		// �Ϲ��浹, Enter, Stay, Exit �浹���� ���� �ٸ� �Լ� �����ų �� ����
		if (m_pGameInstance->Collision_Enter_Sphere(m_pColliderCom, pAttackerCollider))
		{
			// ���� ���¿� ������� �÷��̾� �ǰ� ����Ʈ ���
			_vector direction_from_player_center = _vec3norm(pAttackerCollider->Get_Center() - m_pColliderCom->Get_Center());
			_float radius = m_pColliderCom->Get_Sphere()->Radius * 0.6f;

			_vector effect_point = m_pColliderCom->Get_Center() + (direction_from_player_center * radius);
			_float4 effect_point4;
			XMStoreFloat4(&effect_point4, effect_point);

			MONSTERTYPE monster_type = (MONSTERTYPE)static_cast<CMonster*>(pAttackerCollider->Get_Parent())->Get_MonsterType();
			CEffect_Manager::GetInstance()->Create_AttackedEffect(&effect_point4, monster_type);

			// ���� ���� ��� �浹�˻� ����
			if (0.0 < m_dInvincibilityTimer)
				break;

			// ���â �޽��� ���
			OutputDebugStringW(L"Monster Attack to Player �浹 �߻�\n");

			// ����ϱ� ���ϰ� ����ȭ ���ѵд�
			CGameObject* pAttacker = pAttackerCollider->Get_Parent();
			CTransform* pAttacker_tf = pAttacker->Get_Transform();

			// �������� �Ծ����� �Ǵ��ϱ� ���� Boolean ����
			_bool damaged = true;

			// ������ ����� �̸� �غ���
			_float result_attack = m_pStatusCom->Get_HP();
			result_attack -= dynamic_cast<CStatus*>(pAttacker->Get_Component(TEXT("Com_Status")))->Get_Damage();

			// �׾��ٸ�
			if (0.f >= result_attack)
			{
				// �ǰ� ���¸� ���� ���� �ƴϱ� ������ damaged�� �ٽ� false�� �������´�
				damaged = false;
				
				// ü�� ����
				m_pStatusCom->Set_HP(0.f);

				// �׾��� �� ����.
				// ���ο���, ���̵� �ƿ�, ���̺� ����Ʈ���� ��Ȱ

				return;
			}

			// ���� �ʾҴٸ�
			
			SORA_ANIM current_anim = (SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(SB_BODY);

			_vector attack_collider_center = pAttackerCollider->Get_Center();
			CTransform::DAMAGEDDIR damaged_direction = m_pTransformCom->Get_Damaged_4Dir(attack_collider_center);

			_vector knockback_direction = XMVectorZero();

			//MONSTERTYPE monster_type = (MONSTERTYPE)static_cast<CMonster*>(pAttacker)->Get_MonsterType();
			if (MONSTERTYPE::MONSTERTYPE_ICEMAN == monster_type)
			{
				CIceMan* ice_man = static_cast<CIceMan*>(pAttacker);

				if (nullptr == ice_man)
					_breakpoint;

				CIceMan::ICEMAN_SKILL current_skill = ice_man->Get_IceManSkill();
				_bool bDebug = true;
				switch (current_skill)
				{
				case Client::CIceMan::ICEMAN_SKILL_WALKSLASH:
					bDebug = true;
					break;
				case Client::CIceMan::ICEMAN_SKILL_CLAP:
					bDebug = true;
					break;
				case Client::CIceMan::ICEMAN_SKILL_LONGSLASH:
					bDebug = true;
					break;
				case Client::CIceMan::ICEMAN_SKILL_JUMPDOWNWARDBLOW:
					bDebug = true;
					break;
				}
			}

			// �ǰ� ���� �ƴ� ���
			if (false == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_DAMAGED))
			{
				// ���� ���� ���
				if (true == m_pStateMachineCom->FindState(CStateMachine::STATETYPE_GUARD))
				{
					// �˹� ���� ���� switch�� �ۿ��� �̸� �����д�
					_vector direction_v = XMVectorZero();

					switch (current_anim)
					{
					case Client::CPlayer::SORA_ANIM_GUARD:

						// ���� ����, damaged = false
						damaged = false;

						// ���� �������� ��ȯ
						Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_GUARD_SUCCEEDED);
						m_dGuardTimer = m_dGuardTimer_Max;

						// �浹ü �������� ȸ��
						m_pTransformCom->LookAtXZ(attack_collider_center);

						// �˹� �Է�
						knockback_direction = _vec3norm(m_pTransformCom->Get_Look()) * -1.f;
						m_pTransformCom->KnockBackInput_v(knockback_direction, 1.f, 0.5);

						// ����Ʈ ���
						CEffect_Manager::GetInstance()->Create_Guard(m_pTransformCom);

						break;
					case Client::CPlayer::SORA_ANIM_GUARD_SUCCEEDED:

						// ���� ���忡 ���� ó��
						if (0.0 < m_dGuardTimer)
						{
							// ���� ����, damaged = false
							damaged = false;

							// ���� �������� ��ȯ
							m_pModelCom->ResetAnimation(SORA_ANIM::SORA_ANIM_GUARD_SUCCEEDED);
							Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_GUARD_SUCCEEDED);

							// �浹ü �������� ȸ��
							m_pTransformCom->LookAtXZ(attack_collider_center);

							// �˹� �Է�
							knockback_direction = _vec3norm(m_pTransformCom->Get_Look()) * -1.f;
							m_pTransformCom->KnockBackInput_v(knockback_direction, 1.f, 0.5);

							// ����Ʈ ���
							CEffect_Manager::GetInstance()->Create_Guard(m_pTransformCom);
						}

						// ���� ���尡 �Ұ����� ��Ȳ�� ��, �ǰ�
						else
							Select_DamagedAnimation(damaged_direction);

						break;
					case Client::CPlayer::SORA_ANIM_AIR_GUARD:

						// ���� ����, damaged = false
						damaged = false;

						// ���� �������� ��ȯ
						Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_AIR_GUARD_SUCCEEDED);
						m_dGuardTimer = m_dGuardTimer_Max;

						// �浹ü �������� ȸ��
						m_pTransformCom->LookAtXZ(attack_collider_center);

						// �˹� �Է�
						direction_v = _vec3norm(m_pTransformCom->Get_Look()) * -1.f;
						m_pTransformCom->KnockBackInput_v(direction_v, 1.f, 0.5);

						// ����Ʈ ���
						CEffect_Manager::GetInstance()->Create_Guard(m_pTransformCom);

						break;
					case Client::CPlayer::SORA_ANIM_AIR_GUARD_SUCCEEDED:

						// ���� ���忡 ���� ó��
						if (0.0 < m_dGuardTimer)
						{
							// ���� ����, damaged = false
							damaged = false;

							// ���� �������� ��ȯ
							m_pModelCom->ResetAnimation(SORA_ANIM::SORA_ANIM_AIR_GUARD_SUCCEEDED);
							Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_AIR_GUARD_SUCCEEDED);

							// �浹ü �������� ȸ��
							m_pTransformCom->LookAtXZ(attack_collider_center);

							// �˹� �Է�
							direction_v = _vec3norm(m_pTransformCom->Get_Look()) * -1.f;
							m_pTransformCom->KnockBackInput_v(direction_v, 1.f, 0.5);

							// ����Ʈ ���
							CEffect_Manager::GetInstance()->Create_Guard(m_pTransformCom);
						}

						// ���� ���尡 �Ұ����� ��Ȳ�� ��, �ǰ�
						else
							Select_DamagedAnimation(damaged_direction);

						break;
					case Client::CPlayer::SORA_ANIM_RAGE_GUARD:
					case Client::CPlayer::SORA_ANIM_RAGE_AIR_GUARD:

					{
						// ������ �������� ����� ������ ���߻��·� �̾�����
						// �ִϸ��̼��� ������ ���ؼ� ���� ���¸� ���� �����Ѵ�
						m_pTransformCom->BasicJump(CTransform::JUMP_NORMAL);

						CState_Jump::JUMP_STATE_DESC jump_state_desc;
						jump_state_desc.eObjectType = m_eObjectType;
						jump_state_desc.iJumpAnimIndex = 0;
						jump_state_desc.iNumberOfJumpAnim = 4;

						Create_PlayerState(CStateMachine::STATETYPE_JUMP, &jump_state_desc);
					}

						// ���� ����, damaged = false
						damaged = false;

						// ���� �������� ��ȯ
						Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_RAGE_GUARD_SUCCEEDED);

						// �浹ü �������� ȸ��
						m_pTransformCom->LookAtXZ(attack_collider_center);

						// ����Ʈ ���
						CEffect_Manager::GetInstance()->Create_RageForm_Guard_Success(m_pTransformCom);

						break;
					case Client::CPlayer::SORA_ANIM_RAGE_GUARD_SUCCEEDED:
					case Client::CPlayer::SORA_ANIM_RAGE_COUNTER_ATTACK:

						Select_DamagedAnimation(damaged_direction);

						break;
					case Client::CPlayer::SORA_ANIM_HAMMER_GUARD:

						// ���� ����, damaged = false
						damaged = false;

						// ���� �������� ��ȯ
						Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_HAMMER_GUARD_SUCCEEDED);
						m_dGuardTimer = m_dGuardTimer_Max;

						// �浹ü �������� ȸ��
						m_pTransformCom->LookAtXZ(attack_collider_center);

						// �˹� �Է�
						knockback_direction = _vec3norm(m_pTransformCom->Get_Look()) * -1.f;
						m_pTransformCom->KnockBackInput_v(knockback_direction, 1.f, 0.5);

						// ����Ʈ ���
						CEffect_Manager::GetInstance()->Create_Guard(m_pTransformCom);

						break;
					case Client::CPlayer::SORA_ANIM_HAMMER_GUARD_SUCCEEDED:

						// ���� ���忡 ���� ó��
						if (0.0 < m_dGuardTimer)
						{
							// ���� ����, damaged = false
							damaged = false;

							// ���� �������� ��ȯ
							m_pModelCom->ResetAnimation(SORA_ANIM::SORA_ANIM_HAMMER_GUARD_SUCCEEDED);
							Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_HAMMER_GUARD_SUCCEEDED);

							// �浹ü �������� ȸ��
							m_pTransformCom->LookAtXZ(attack_collider_center);

							// �˹� �Է�
							knockback_direction = _vec3norm(m_pTransformCom->Get_Look()) * -1.f;
							m_pTransformCom->KnockBackInput_v(knockback_direction, 1.f, 0.5);

							// ����Ʈ ���
							CEffect_Manager::GetInstance()->Create_Guard(m_pTransformCom);
						}

						// ���� ���尡 �Ұ����� ��Ȳ�� ��, �ǰ�
						else
							Select_DamagedAnimation(damaged_direction);

						break;
					case Client::CPlayer::SORA_ANIM_HAMMER_AIR_GUARD:

						// ���� ����, damaged = false
						damaged = false;

						// ���� �������� ��ȯ
						Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_HAMMER_AIR_GUARD_SUCCEEDED);
						m_dGuardTimer = m_dGuardTimer_Max;

						// �浹ü �������� ȸ��
						m_pTransformCom->LookAtXZ(attack_collider_center);

						// �˹� �Է�
						direction_v = _vec3norm(m_pTransformCom->Get_Look()) * -1.f;
						m_pTransformCom->KnockBackInput_v(direction_v, 1.f, 0.5);

						// ����Ʈ ���
						CEffect_Manager::GetInstance()->Create_Guard(m_pTransformCom);

						break;
					case Client::CPlayer::SORA_ANIM_HAMMER_AIR_GUARD_SUCCEEDED:

						// ���� ���忡 ���� ó��
						if (0.0 < m_dGuardTimer)
						{
							// ���� ����, damaged = false
							damaged = false;

							// ���� �������� ��ȯ
							m_pModelCom->ResetAnimation(SORA_ANIM::SORA_ANIM_HAMMER_AIR_GUARD_SUCCEEDED);
							Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_HAMMER_AIR_GUARD_SUCCEEDED);

							// �浹ü �������� ȸ��
							m_pTransformCom->LookAtXZ(attack_collider_center);

							// �˹� �Է�
							direction_v = _vec3norm(m_pTransformCom->Get_Look()) * -1.f;
							m_pTransformCom->KnockBackInput_v(direction_v, 1.f, 0.5);

							// ����Ʈ ���
							CEffect_Manager::GetInstance()->Create_Guard(m_pTransformCom);
						}

						// ���� ���尡 �Ұ����� ��Ȳ�� ��, �ǰ�
						else
							Select_DamagedAnimation(damaged_direction);

						break;
					}
				} // ���� ���� ��� end

				// �ǰ� ���� �ƴϰ� ���� ���� �ƴ� ���
				else
					Select_DamagedAnimation(damaged_direction);
			}

			// �ǰ� ���� ��
			else
			{
				Select_DamagedAnimation(damaged_direction);
			}

			if (false == damaged)
				continue;

			// HP ����
			m_pStatusCom->Set_HP(result_attack);

			// ���� �ð� ����
			if (0.0 == m_dInvincibilityTimer)
				Invincibility_Input(0.5, 1.0);
		}
	}

	// �ǰ� ���·��� ��ȯ�� �Ͼ�� ������ �߻��� ��쿡 �ִϸ��̼� ó�� �� ���� ����
	_float hp_after_collision = m_pStatusCom->Get_HP();
	if (hp_after_collision != hp_before_collision)
	{
		// �ִϸ��̼� ����
		SORA_ANIM temp_index = (SORA_ANIM)m_pModelCom->Get_TempIndex_EachBody(SORA_BODY::SB_BODY);
		m_pModelCom->ResetAnimation((_uint)temp_index);

		// �ִϸ��̼� ����
		CState_PlayerDamaged::PLAYER_DAMAGED_DESC damaged_desc;
		damaged_desc.sora_anim = temp_index;

		Create_PlayerState(CStateMachine::STATETYPE::STATETYPE_DAMAGED, &damaged_desc);
		Play_DamagedVoiceSound();

		// �ǰݽ� ȭ�� ������ �Ǵ� UI
		GM->HitBlood();
	}
}

void CPlayer::Select_DamagedAnimation(CTransform::DAMAGEDDIR eDirection)
{
	_bool air = m_pTransformCom->Get_JumpBoolean();

	// ���� ���� ��� - ���� �ǰ� �ִϸ��̼�
	if (false == air)
	{
		switch (m_eForm)
		{
		case Client::CPlayer::SF_NORMAL:

			switch (eDirection)
			{
			case Engine::CTransform::DAMAGED_FRONT:
				Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_DAMAGED_NORMAL_FU);
				break;
			case Engine::CTransform::DAMAGED_BACK:
				Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_DAMAGED_NORMAL_BU);
				break;
			case Engine::CTransform::DAMAGED_RIGHT:
				Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_DAMAGED_NORMAL_R);
				break;
			case Engine::CTransform::DAMAGED_LEFT:
				Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_DAMAGED_NORMAL_L);
				break;
			}

			break;
		case Client::CPlayer::SF_HAMMER:
			switch (eDirection)
			{
			case Engine::CTransform::DAMAGED_FRONT:
				Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_HAMMER_DAMAGED_F);
				break;
			case Engine::CTransform::DAMAGED_BACK:
				Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_HAMMER_DAMAGED_B);
				break;
			case Engine::CTransform::DAMAGED_RIGHT:
				Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_HAMMER_DAMAGED_R);
				break;
			case Engine::CTransform::DAMAGED_LEFT:
				Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_HAMMER_DAMAGED_L);
				break;
			}
			break;
		case Client::CPlayer::SF_RAGE:

			switch (eDirection)
			{
			case Engine::CTransform::DAMAGED_FRONT:
				Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_RAGE_DAMAGED_F);
				break;
			case Engine::CTransform::DAMAGED_BACK:
				Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_RAGE_DAMAGED_B);
				break;
			case Engine::CTransform::DAMAGED_RIGHT:
				Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_RAGE_DAMAGED_R);
				break;
			case Engine::CTransform::DAMAGED_LEFT:
				Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_RAGE_DAMAGED_L);
				break;
			}

			break;
		case Client::CPlayer::SF_END:
			_breakpoint;
			break;
		}
	}

	// ���߿� ���� ��� - ���� �ǰ� �ִϸ��̼�
	else
	{
		switch (m_eForm)
		{
		case Client::CPlayer::SF_NORMAL:

			switch (eDirection)
			{
			case Engine::CTransform::DAMAGED_FRONT:
				Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_AIR_DAMAGED_NORMAL_F);
				break;
			case Engine::CTransform::DAMAGED_BACK:
				Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_AIR_DAMAGED_NORMAL_B);
				break;
			case Engine::CTransform::DAMAGED_RIGHT:
				Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_AIR_DAMAGED_NORMAL_R);
				break;
			case Engine::CTransform::DAMAGED_LEFT:
				Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_AIR_DAMAGED_NORMAL_L);
				break;
			}

			break;
		case Client::CPlayer::SF_HAMMER:

			switch (eDirection)
			{
			case Engine::CTransform::DAMAGED_FRONT:
				Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_HAMMER_AIR_DAMAGED_F);
				break;
			case Engine::CTransform::DAMAGED_BACK:
				Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_HAMMER_AIR_DAMAGED_B);
				break;
			case Engine::CTransform::DAMAGED_RIGHT:
				Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_HAMMER_AIR_DAMAGED_R);
				break;
			case Engine::CTransform::DAMAGED_LEFT:
				Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_HAMMER_AIR_DAMAGED_L);
				break;
			}

			break;
		case Client::CPlayer::SF_RAGE:

			switch (eDirection)
			{
			case Engine::CTransform::DAMAGED_FRONT:
				Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_RAGE_AIR_DAMAGED_F);
				break;
			case Engine::CTransform::DAMAGED_BACK:
				Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_RAGE_AIR_DAMAGED_B);
				break;
			case Engine::CTransform::DAMAGED_RIGHT:
				Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_RAGE_AIR_DAMAGED_R);
				break;
			case Engine::CTransform::DAMAGED_LEFT:
				Change_PlayerAnimation(SORA_ANIM::SORA_ANIM_RAGE_AIR_DAMAGED_L);
				break;
			}

			break;
		case Client::CPlayer::SF_END:
			_breakpoint;
			break;
		}
	}
}

void CPlayer::Overwrite_OutfitTexture(_uint iMaterialIndex)
{
	if (false == m_bFormTexture)
		return;

	switch (m_eForm)
	{
	case Client::CPlayer::SF_HAMMER:

		switch (iMaterialIndex)
		{
		case 0: // �Ӹ� ����
			break;
		case 1: // �Ӹ� �ٱ���
			break;
		case 2: // ?
			break;
		case 3: // �� �Ǻ�
			break;
		case 4: // ?
			break;
		case 5: // ����
			break;
		case 6: // ��
			break;
		case 7: // �Ӵ���
			break;
		case 8: // �ݼ� ����(�����, ���� ��)
			m_pDiffuseTexture[SO_HAMMER]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 2);
			break;
		case 9: // ����
			m_pDiffuseTexture[SO_HAMMER]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0);
			break;
		case 10: // ����
			m_pDiffuseTexture[SO_HAMMER]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 1);
			break;
		case 11: // ���� �Ǻ�
			m_pDiffuseTexture[SO_HAMMER]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0);
			break;
		}

		break;
	case Client::CPlayer::SF_RAGE:

		switch (iMaterialIndex)
		{
		case 0: // �Ӹ� ����
			m_pDiffuseTexture[SO_RAGE]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 5);
			break;
		case 1: // �Ӹ� �ٱ���
			m_pDiffuseTexture[SO_RAGE]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 5);
			break;
		case 2: // ?
			break;
		case 3: // �� �Ǻ�
			m_pDiffuseTexture[SO_RAGE]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 2);
			break;
		case 4: // ?
			break;
		case 5: // ����
			break;
		case 6: // ��
			m_pDiffuseTexture[SO_RAGE]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 4);
			break;
		case 7: // �Ӵ���
			break;
		case 8: // �ݼ� ����(�����, ���� ��)
			m_pDiffuseTexture[SO_RAGE]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 3);
			break;
		case 9: // ����
			m_pDiffuseTexture[SO_RAGE]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0);
			break;
		case 10: // ����
			m_pDiffuseTexture[SO_RAGE]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 1);
			break;
		case 11: // ���� �Ǻ�
			m_pDiffuseTexture[SO_RAGE]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0);
			break;
		}

		break;
	}
}

void CPlayer::Control_Animation()
{
	if (true == ImGui::CollapsingHeader("Animation Control"))
	{
		_bool separated_anim_model = m_pModelCom->Get_ModelSeparated();

		// �и��� ��
		if (true == separated_anim_model)
		{
			_uint number_of_animation = m_pModelCom->Get_NumberOfAnimations();

			_uint curr_index_body = m_pModelCom->Get_CurrAnimIndex_EachBody(SB_BODY);
			_uint prev_index_body = m_pModelCom->Get_PrevAnimIndex_EachBody(SB_BODY);

			_uint curr_index_face = m_pModelCom->Get_CurrAnimIndex_EachBody(SB_FACE);
			_uint prev_index_face = m_pModelCom->Get_PrevAnimIndex_EachBody(SB_FACE);

			const char* anim[] = {
				"SORA_ANIM_IDLE", "SORA_ANIM_BORING", "SORA_ANIM_DRAW_BLADE",
				"SORA_ANIM_IDLE_BLADE", "SORA_ANIM_IDLE_BLADE_END",
				"SORA_ANIM_BORING_BLADE", "SORA_ANIM_RUN_FORWARD", "SORA_ANIM_RUN_FORWARD_BLADE", "SORA_ANIM_STEPPING_LTOR",
				"SORA_ANIM_STEPPING_RTOL", "SORA_ANIM_JUMP_UP", "SORA_ANIM_FALL_START", "SORA_ANIM_FALL_LOOP", "SORA_ANIM_FALL_END",
				"SORA_ANIM_GROUND_ATTACK1", "SORA_ANIM_GROUND_ATTACK2", "SORA_ANIM_GROUND_ATTACK3", "SORA_ANIM_GROUND_ATTACK4",
				"SORA_ANIM_AIR_ATTACK1", "SORA_ANIM_AIR_ATTACK2", "SORA_ANIM_AIR_ATTACK3", "SORA_ANIM_AIR_ATTACK4",
				"SORA_ANIM_BLOCKED_BY_ENV", "SORA_ANIM_AIR_BLOCKED_BY_ENV",
				"SORA_ANIM_GUARD", "SORA_ANIM_GUARD_SUCCEEDED",
				"SORA_ANIM_AIR_GUARD", "SORA_ANIM_AIR_GUARD_SUCCEEDED",
				"SORA_ANIM_ROLL", "SORA_ANIM_AIR_DASH"
			};

			static _bool body_control_popup = false;
			static _bool face_control_popup = false;
			static _bool target_control_popup = false;

			ImGui::Checkbox("Body Control", &body_control_popup);
			ImGui::SameLine();
			ImGui::Checkbox("Face Control", &face_control_popup);
			ImGui::SameLine();
			ImGui::Checkbox("Target Control", &target_control_popup);

			if (true == body_control_popup)
			{
				static int selected_body_anim = 0;
				bool body_changed = false;
				const char* preview_body_anim = anim[selected_body_anim];
				if (ImGui::BeginCombo("BODY ANIM", preview_body_anim))
				{
					for (int i = 0; i < IM_ARRAYSIZE(anim); ++i)
					{
						const bool is_selected_anim = (selected_body_anim == i);
						if (ImGui::Selectable(anim[i], is_selected_anim))
						{
							selected_body_anim = i;
							body_changed = true;
						}
					}

					ImGui::EndCombo();
				}

				if (true == body_changed)
				{
					m_eSoraAnim[SORA_BODY::SB_BODY] = (SORA_ANIM)selected_body_anim;
					m_pModelCom->Set_TempIndex_EachBody(SORA_BODY::SB_BODY, m_eSoraAnim[SORA_BODY::SB_BODY]);
				}
			}

			if (true == face_control_popup)
			{
				static int selected_face_anim = 3;
				bool face_changed = false;
				const char* preview_face_anim = anim[selected_face_anim];
				if (ImGui::BeginCombo("FACE ANIM", preview_face_anim))
				{
					for (int i = 0; i < IM_ARRAYSIZE(anim); ++i)
					{
						const bool is_selected_anim = (selected_face_anim == i);
						if (ImGui::Selectable(anim[i], is_selected_anim))
						{
							selected_face_anim = i;
							face_changed = true;
						}
					}

					ImGui::EndCombo();
				}

				if (true == face_changed)
				{
					m_eSoraAnim[SORA_BODY::SB_FACE] = (SORA_ANIM)selected_face_anim;
					m_pModelCom->Set_TempIndex_EachBody(SORA_BODY::SB_FACE, m_eSoraAnim[SORA_BODY::SB_FACE]);
				}
			}

			static int selected_target_anim = 3;
			if (true == target_control_popup)
			{
				bool target_changed = false;
				const char* preview_target_anim = anim[selected_target_anim];
				if (ImGui::BeginCombo("TARGET ANIM", preview_target_anim))
				{
					for (int i = 0; i < IM_ARRAYSIZE(anim); ++i)
					{
						const bool is_selected_anim = (selected_target_anim == i);
						if (ImGui::Selectable(anim[i], is_selected_anim))
						{
							selected_target_anim = i;
							target_changed = true;
						}
					}

					ImGui::EndCombo();
				}
			}

			if (true == body_control_popup)
			{
				if (true == ImGui::CollapsingHeader("Body Part Animation Index Info"))
				{

					ImGui::Text("Current Animation Index || %d / %d", (_int)m_pModelCom->Get_CurrAnimIndex_EachBody(SORA_BODY::SB_BODY), number_of_animation - 1);
					char* fiona_curr_name = m_pModelCom->Get_CurrName_EachBody(SORA_BODY::SB_BODY);
					ImGui::Text(fiona_curr_name);

					_double curr_time_acc = m_pModelCom->Get_CurrTimeAcc_EachBody(SB_BODY);
					_double curr_duration = m_pModelCom->Get_CurrDuration_EachBody(SB_BODY);
					ImGui::Text("Current Animation Time || %.2lf / %.2lf", curr_time_acc, curr_duration);

					ImGui::Separator();

					ImGui::Text("Previous Animation Index || %d / %d", (_int)m_pModelCom->Get_PrevAnimIndex_EachBody(SORA_BODY::SB_BODY), number_of_animation - 1);
					char* fiona_prev_name = m_pModelCom->Get_PrevName_EachBody(SORA_BODY::SB_BODY);
					ImGui::Text(fiona_prev_name);

					_double prev_time_acc = m_pModelCom->Get_PrevTimeAcc_EachBody(SB_BODY);
					_double prev_duration = m_pModelCom->Get_PrevDuration_EachBody(SB_BODY);
					ImGui::Text("Previous Animation Time || %.2lf / %.2lf", prev_time_acc, prev_duration);
				}
			}

			if (true == face_control_popup)
			{
				if (true == ImGui::CollapsingHeader("Face Part Animation Index Info"))
				{
					ImGui::Text("Current Animation Index || %d / %d", (_int)m_pModelCom->Get_CurrAnimIndex_EachBody(SORA_BODY::SB_FACE), number_of_animation - 1);
					char* cat_curr_name = m_pModelCom->Get_CurrName_EachBody(SORA_BODY::SB_FACE);
					ImGui::Text(cat_curr_name);

					ImGui::Separator();

					ImGui::Text("Previous Animation Index || %d / %d", (_int)m_pModelCom->Get_PrevAnimIndex_EachBody(SORA_BODY::SB_FACE), number_of_animation - 1);
					char* cat_prev_name = m_pModelCom->Get_PrevName_EachBody(SORA_BODY::SB_FACE);
					ImGui::Text(cat_prev_name);
				}
			}

			if (true == target_control_popup)
			{
				if (true == ImGui::CollapsingHeader("Target Part Animation Index Info"))
				{
					ImGui::Text("Target Animation Index || %d / %d", (_int)selected_target_anim, number_of_animation - 1);
					char* target_name = m_pModelCom->Get_TargetName((_uint)selected_target_anim);
					ImGui::Text(target_name);

					_double target_time_acc = m_pModelCom->Get_TargetTimeAcc((_uint)selected_target_anim);
					_double target_duration = m_pModelCom->Get_TargetDuration((_uint)selected_target_anim);
					ImGui::Text("Target Animation Time || %.2lf / %.2lf", target_time_acc, target_duration);

					CAnimation::ANIM_TYPE target_type = m_pModelCom->Get_TargetAnimType((_uint)selected_target_anim);
					switch (target_type)
					{
					case Engine::CAnimation::ANIM_ONCE:
						ImGui::Text("Target Animation Type - [ ANIM_ONCE ]");
						break;
					case Engine::CAnimation::ANIM_LOOP:
						ImGui::Text("Target Animation Type - [ ANIM_LOOP ]");
						break;
					}

					_bool stopped = m_pModelCom->Get_TargetStopped((_uint)selected_target_anim);
					if (true == stopped)
						ImGui::Text("Target Animation is Stopped");
					else
						ImGui::Text("Target Animation is Not Stopped");
				}
			}
		}
	}
}

void CPlayer::Control_Collider()
{
	if (true == ImGui::CollapsingHeader("Collider Control"))
	{
		_float3 scale = m_pColliderCom->Get_Scale();
		_float4 rotation = m_pColliderCom->Get_Rotation();
		_float4 position = m_pColliderCom->Get_Translation();

		_float scale_f[3]{ scale.x, scale.y, scale.z };
		_float rotation_f[4]{ rotation.x, rotation.y, rotation.z, rotation.w };
		_float position_f[4]{ position.x, position.y, position.z, position.w };

		ImGui::DragFloat3("Scale", scale_f, 0.05f, 0.1f);
		ImGui::DragFloat4("Rotation", rotation_f, 0.05f);
		ImGui::DragFloat4("Position", position_f, 0.05f);

		scale = _float3(scale_f[0], scale_f[1], scale_f[2]);
		rotation = _float4(rotation_f[0], rotation_f[1], rotation_f[2], rotation_f[3]);
		position = _float4(position_f[0], position_f[1], position_f[2], position_f[3]);

		m_pColliderCom->Set_Scale(scale);
		m_pColliderCom->Set_Rotation(rotation);
		m_pColliderCom->Set_Translation(position);

		XMFLOAT3 center = m_pColliderCom->Get_Sphere()->Center;
		_float radius = m_pColliderCom->Get_Sphere()->Radius;

		ImGui::Text("Center : %.2f, %.2f, %.2f", center.x, center.y, center.z);
		ImGui::Text("Radius : %.2f", radius);
	}
}

void CPlayer::Control_Jump()
{
	if (true == ImGui::CollapsingHeader("Jump Control"))
	{
		// ���� jump desc ���� ���
		ImGui::Text("[ Current Jump Desc Info ]");
		ImGui::Separator();

		CTransform::JUMP_DESC jump_desc = m_pTransformCom->Get_JumpDesc();

		char* jump_type_text = nullptr;
		switch (jump_desc.eJumpType)
		{
		case CTransform::JUMP_NORMAL:
			jump_type_text = "[ JUMP_TYPE ] - JUMP_NORMAL";
			break;
		case CTransform::JUMP_FALL:
			jump_type_text = "[ JUMP_TYPE ] - JUMP_FALL";
			break;
		case CTransform::JUMP_CONTINUE:
			jump_type_text = "[ JUMP_TYPE ] - JUMP_CONTINUE";
			break;
		case CTransform::JUMP_LAND:
			jump_type_text = "[ JUMP_TYPE ] - JUMP_LAND";
			break;
		case CTransform::JUMP_FORCE:
			jump_type_text = "[ JUMP_TYPE ] - JUMP_FORCE";
			break;
		case CTransform::JUMP_PERSONAL:
			jump_type_text = "[ JUMP_TYPE ] - JUMP_PERSONAL";
			break;
		case CTransform::JUMP_Editional1:
			jump_type_text = "[ JUMP_TYPE ] - JUMP_Editional1";
			break;
		case CTransform::JUMP_Editional2:
			jump_type_text = "[ JUMP_TYPE ] - JUMP_Editional2";
			break;
		case CTransform::JUMP_Editional3:
			jump_type_text = "[ JUMP_TYPE ] - JUMP_Editional3";
			break;
		case CTransform::JUMP_END:
			jump_type_text = "[ JUMP_TYPE ] - END. the value is corrupted";
			break;
		}

		ImGui::Text(jump_type_text);

		char* jump_boolean_text = nullptr;
		if (true == jump_desc.bJump)
			jump_boolean_text = "[ BOOLEAN ] - True";
		else
			jump_boolean_text = "[ BOOLEAN ] - False";

		ImGui::Text(jump_boolean_text);

		char* gravity_effect_text = nullptr;
		if (true == jump_desc.bGravityEffect)
			gravity_effect_text = "[ Gravity ] - True";
		else
			gravity_effect_text = "[ Gravity ] - False";

		ImGui::Text("[ Height ] - %.2f", jump_desc.fJumpHeight);
		ImGui::Text("[ Power  ] - %.2f", jump_desc.fJumpPower);
		ImGui::Text("[ Time   ] - %.2lf", jump_desc.dJumpTime);

		ImGui::Separator();
		ImGui::Separator();

		////////////////////////////////////////////////////////////

		static CTransform::JUMP_DESC jump_desc_input;
		if (CTransform::JUMP_END == jump_desc_input.eJumpType)
		{
			jump_desc_input.bGravityEffect = true;
			jump_desc_input.bJump = true;

			jump_desc_input.fJumpPower = 1.5f;
		}

		const char* jump[] = {
			"JUMP_NORMAL", "JUMP_FALL", "JUMP_LAND",
			"JUMP_FORCE", "JUMP_PERSONAL",
			"JUMP_Editional1", "JUMP_Editional2", "JUMP_Editional3"
		};

		static int selected_jump = 0;
		bool jumped = false;
		const char* preview_anim = jump[selected_jump];
		if (ImGui::BeginCombo("JUMP", preview_anim))
		{
			for (int i = 0; i < IM_ARRAYSIZE(jump); ++i)
			{
				const bool is_selected_jump = (selected_jump == i);
				if (ImGui::Selectable(jump[i], is_selected_jump))
				{
					selected_jump = i;
					jumped = true;
				}
			}

			ImGui::EndCombo();
		}

		ImGui::Checkbox("Gravity Effect", &jump_desc_input.bGravityEffect);
		ImGui::SameLine();
		ImGui::Checkbox("Jump Boolean", &jump_desc_input.bJump);

		ImGui::DragFloat("Jump Power", &jump_desc_input.fJumpPower);

		if (true == ImGui::Button("Jump"))
		{
			jump_desc_input.eJumpType = (CTransform::JUMP_TYPE)selected_jump;
			jump_desc_input.fJumpHeight = vecy(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			jump_desc_input.dJumpTime = 0.0;

			m_pTransformCom->Set_JumpDesc(jump_desc_input);

			m_pStateMachineCom->BindState(CStateMachine::STATETYPE_JUMP, string("State_Jump"), CState_Jump::Create(this), true);
			/*if (m_pStateMachineCom->FindStateTag(CStateMachine::STATETYPE_JUMP, string("State_Jump")) == false)
			m_pStateMachineCom->BindState(CStateMachine::STATETYPE_JUMP, string("State_Jump"), CState_Jump::Create(this));*/
		}
	}
}

void CPlayer::Control_StateMachine()
{
	if (true == ImGui::CollapsingHeader("StateMachine Info"))
	{
		_uint number_of_states = m_pStateMachineCom->Get_NumberOfStates();
		ImGui::Text("Number Of States : %d", number_of_states);

		for (_uint i = 0; i < CStateMachine::STATETYPE_END; ++i)
		{
			CStateMachine::STATETYPE current_state_type = (CStateMachine::STATETYPE)i;
			if (true == m_pStateMachineCom->FindState(current_state_type))
			{
				string name = "[ State Name : ";
				name += m_pStateMachineCom->Get_StateName(current_state_type);
				name += " ]";

				ImGui::Text(name.c_str());
			}
			else
			{
				string name = "[ State Name : NULL ]";
				ImGui::Text(name.c_str());
			}
		}
	}
}

void CPlayer::Control_DamageState()
{
	if (true == ImGui::CollapsingHeader("Damaged State Control"))
	{
		ImGui::Text("Damaged State Start Animation Index : %d", m_iDamagedAnim);
		ImGui::InputInt("Damaged State Animation Index", &m_iDamagedAnim);

		if (0 > m_iDamagedAnim)
			m_iDamagedAnim = 0;

		if (m_pModelCom->Get_NumberOfAnimations() <= (_uint)m_iDamagedAnim)
			m_iDamagedAnim = m_pModelCom->Get_NumberOfAnimations() - 1;

		if (true == ImGui::Button("Create Player Damaged State"))
		{
			CState_PlayerDamaged::PLAYER_DAMAGED_DESC damaged_desc;
			damaged_desc.sora_anim = (SORA_ANIM)m_iDamagedAnim;

			Create_PlayerState(CStateMachine::STATETYPE::STATETYPE_DAMAGED, &damaged_desc);
		}
	}
}

void CPlayer::Control_CombatAssistant()
{
	if (true == ImGui::CollapsingHeader("Combat Assistant Control"))
	{

#pragma region Target Searching

		ImGui::Separator();
		ImGui::Separator();

		_uint number_of_target = m_pCombat_Assistant->NumberOfColliders();
		ImGui::Text("[ Number Of Target : %d ]", number_of_target);

		ImGui::Separator();
		ImGui::Separator();

		_float range = m_pCombat_Assistant->Range();
		_uint max_search = m_pCombat_Assistant->MaxNumber();

		ImGui::InputInt("Max Number", (_int*)&max_search);
		ImGui::InputFloat("Search Range", &range);

		m_pCombat_Assistant->Range(range);
		m_pCombat_Assistant->MaxNumber(max_search);

		ImGui::Separator();
		ImGui::Separator();

#pragma endregion

#pragma region Target Skipping

		_uint skip_num = m_pCombat_Assistant->SkipNumber();
		_uint skip_lim = m_pCombat_Assistant->SkipLimit();

		ImGui::Text("[ Number of Skipping target : %d / %d ]", skip_num, skip_lim);

		if (ImGui::Button("Next Target"))
		{
			m_pCombat_Assistant->Change_FastenTarget();
			skip_num = m_pCombat_Assistant->SkipNumber();
		}

		ImGui::SameLine();
		if (ImGui::Button("Turn to Target"))
		{
			C_ASSIST::ASSIST_DESC desc = m_pCombat_Assistant->Get_Desc(C_ASSIST::ASSIST_LOCKON);
			m_pTransformCom->MakeSameDirection(XMLoadFloat3(&desc.vDirectionXZ));
		}

		ImGui::InputInt("Skip Number", (_int*)&skip_num);
		ImGui::InputInt("Skip Limit", (_int*)&skip_lim);

		list<C_ASSIST::ASSIST_DESC>* desc_list = m_pCombat_Assistant->Get_AllSearched();
		_uint collided_number = (_uint)desc_list->size();

		if (skip_lim > max_search)
			skip_lim = max_search;

		if (skip_num > skip_lim - 1 ||
			skip_num > collided_number - 1)
			skip_num = collided_number - 1;

		m_pCombat_Assistant->SkipNumber(skip_num);
		m_pCombat_Assistant->SkipLimit(skip_lim);

		ImGui::Separator();
		ImGui::Separator();

#pragma endregion

#pragma region Clipping

		_bool auto_rot = m_pCombat_Assistant->AutoDir();
		_bool auto_pos = m_pCombat_Assistant->AutoPos();

		ImGui::Checkbox("Auto Rotation", &auto_rot);
		ImGui::Checkbox("Auto Position", &auto_pos);

		m_pCombat_Assistant->AutoDir(auto_rot);
		m_pCombat_Assistant->AutoPos(auto_pos);

		ImGui::Separator();

		_float clipping_max = m_pCombat_Assistant->ClippingMax();
		_float clipping_min = m_pCombat_Assistant->ClippingMin();

		ImGui::Text("[ Clipping Min & Max : %.2f / %.2f ]", clipping_min, clipping_max);

		ImGui::DragFloat("Clipping Max", &clipping_max, 0.025f, 1.f, 3.f);
		ImGui::DragFloat("Clipping Min", &clipping_min, 0.025f, 0.f, 1.f);

		m_pCombat_Assistant->ClippingMax(clipping_max);
		m_pCombat_Assistant->ClippingMin(clipping_min);

		ImGui::Separator();

		_float clipping_max_Y = m_pCombat_Assistant->ClippingMax_Y();
		_float clipping_min_Y = m_pCombat_Assistant->ClippingMin_Y();

		ImGui::Text("[ Clipping Min & Max (Y) : %.2f / %.2f ]", clipping_min_Y, clipping_max_Y);

		ImGui::DragFloat("Clipping Max Y", &clipping_max_Y, 0.025f, 1.f, 3.f);
		ImGui::DragFloat("Clipping Min Y", &clipping_min_Y, 0.025f, 0.f, 1.f);

		m_pCombat_Assistant->ClippingMax_Y(clipping_max_Y);
		m_pCombat_Assistant->ClippingMin_Y(clipping_min_Y);

		ImGui::Separator();
		ImGui::Separator();

#pragma endregion

#pragma region LockOn

		if (true == ImGui::CollapsingHeader("LockOn Infomation"))
		{
			C_ASSIST::ASSIST_DESC desc = m_pCombat_Assistant->Get_Desc(C_ASSIST::ASSIST_LOCKON);

			if (nullptr != desc.pCollider)
			{
				_vector pos = desc.pCollider->Get_Center();

				ImGui::Text("Position : %.2f, %.2f, %.2f", vecx(pos), vecy(pos), vecz(pos));

				ImGui::Separator();

				ImGui::Text("Distance : %.2f", desc.fDistance);
				ImGui::Text("Distance_XZ : %.2f", desc.fDistanceXZ);
				ImGui::Text("Distance_Y : %.2f", desc.fDistanceY);

				ImGui::Separator();

				ImGui::Text("Direction : %.2f, %.2f, %.2f", desc.vDirection.x, desc.vDirection.y, desc.vDirection.z);
				ImGui::Text("Direction_XZ : %.2f, %.2f, %.2f", desc.vDirectionXZ.x, desc.vDirectionXZ.y, desc.vDirectionXZ.z);

				ImGui::Separator();

				ImGui::Text("Angle : %.2f", desc.fAngleXZ);

				ImGui::Separator();

				if (true == desc.bUpDown)
					ImGui::Text("The target is [ Above ] the player");
				else
					ImGui::Text("The target is [ Below ] the player");

				if (true == desc.bLeftRight)
					ImGui::Text("The target is [ Left Side ] the player");
				else
					ImGui::Text("The target is [ Right Side ] the player");

				ImGui::Separator();
				ImGui::Separator();

				list<C_ASSIST::ASSIST_DESC>* desc_list = m_pCombat_Assistant->Get_AllSearched();
				auto iter = desc_list->begin();
				_int index = INT_MAX;
				for (_uint i = 0; i < desc_list->size(); ++i)
				{
					if ((*iter).pCollider == desc.pCollider)
					{
						index = i;
						break;
					}
					else
						iter++;
				}

				ImGui::Text("The Target is same with No. %d on Searched List", index);

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
			}
		}

#pragma endregion

#pragma region Attack

		if (true == ImGui::CollapsingHeader("Attack Infomation"))
		{
			C_ASSIST::ASSIST_DESC desc = m_pCombat_Assistant->Get_Desc(C_ASSIST::ASSIST_ATTACK);

			if (nullptr != desc.pCollider)
			{
				_vector pos = desc.pCollider->Get_Center();

				ImGui::Text("Position : %.2f, %.2f, %.2f", vecx(pos), vecy(pos), vecz(pos));

				ImGui::Separator();

				ImGui::Text("Distance : %.2f", desc.fDistance);
				ImGui::Text("Distance_XZ : %.2f", desc.fDistanceXZ);
				ImGui::Text("Distance_Y : %.2f", desc.fDistanceY);

				ImGui::Separator();

				ImGui::Text("Direction : %.2f, %.2f, %.2f", desc.vDirection.x, desc.vDirection.y, desc.vDirection.z);
				ImGui::Text("Direction_XZ : %.2f, %.2f, %.2f", desc.vDirectionXZ.x, desc.vDirectionXZ.y, desc.vDirectionXZ.z);

				ImGui::Separator();

				ImGui::Text("Angle : %.2f", desc.fAngleXZ);

				ImGui::Separator();

				if (true == desc.bUpDown)
					ImGui::Text("The target is [ Above ] the player");
				else
					ImGui::Text("The target is [ Below ] the player");

				if (true == desc.bLeftRight)
					ImGui::Text("The target is [ Left Side ] the player");
				else
					ImGui::Text("The target is [ Right Side ] the player");

				ImGui::Separator();
				ImGui::Separator();

				list<C_ASSIST::ASSIST_DESC>* desc_list = m_pCombat_Assistant->Get_AllSearched();
				auto iter = desc_list->begin();
				_int index = INT_MAX;
				for (_uint i = 0; i < desc_list->size(); ++i)
				{
					if ((*iter).pCollider == desc.pCollider)
					{
						index = i;
						break;
					}
					else
						iter++;
				}

				ImGui::Text("The Target is same with No. %d on Searched List", index);

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
			}
		}

#pragma endregion

#pragma region Fasten

		if (true == ImGui::CollapsingHeader("Fasten Infomation"))
		{
			ImGui::Separator();
			ImGui::Separator();

			C_ASSIST::ASSIST_DESC desc = m_pCombat_Assistant->Get_Desc(C_ASSIST::ASSIST_FASTEN);

			if (nullptr != desc.pCollider)
			{
				_vector pos = desc.pCollider->Get_Center();

				ImGui::Text("Position : %.2f, %.2f, %.2f", vecx(pos), vecy(pos), vecz(pos));

				ImGui::Separator();

				ImGui::Text("Distance : %.2f", desc.fDistance);
				ImGui::Text("Distance_XZ : %.2f", desc.fDistanceXZ);
				ImGui::Text("Distance_Y : %.2f", desc.fDistanceY);

				ImGui::Separator();

				ImGui::Text("Direction : %.2f, %.2f, %.2f", desc.vDirection.x, desc.vDirection.y, desc.vDirection.z);
				ImGui::Text("Direction_XZ : %.2f, %.2f, %.2f", desc.vDirectionXZ.x, desc.vDirectionXZ.y, desc.vDirectionXZ.z);

				ImGui::Separator();

				ImGui::Text("Angle : %.2f", desc.fAngleXZ);

				ImGui::Separator();

				if (true == desc.bUpDown)
					ImGui::Text("The target is [ Above ] the player");
				else
					ImGui::Text("The target is [ Below ] the player");

				if (true == desc.bLeftRight)
					ImGui::Text("The target is [ Left Side ] the player");
				else
					ImGui::Text("The target is [ Right Side ] the player");

				ImGui::Separator();
				ImGui::Separator();

				list<C_ASSIST::ASSIST_DESC>* desc_list = m_pCombat_Assistant->Get_AllSearched();
				auto iter = desc_list->begin();
				_int index = INT_MAX;
				for (_uint i = 0; i < desc_list->size(); ++i)
				{
					if ((*iter).pCollider == desc.pCollider)
					{
						index = i;
						break;
					}
					else
						iter++;
				}

				ImGui::Text("The Target is same with No. %d on Searched List", index);

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
			}
		}

#pragma endregion

#pragma region Searched Target All

		if (true == ImGui::CollapsingHeader("Searched Targets Information"))
		{
			ImGui::Text("Collider Size : %d,    Object Size : %d", m_pCombat_Assistant->NumberOfColliders(), m_pCombat_Assistant->NumberOfObjects());

			ImGui::Separator();
			ImGui::Separator();

			static _int number_of_show = 0;
			ImGui::DragInt("Number of Information to Show", &number_of_show, 1.f, 0);

			list<C_ASSIST::ASSIST_DESC>* desc_list = m_pCombat_Assistant->Get_AllSearched();
			auto iter = desc_list->begin();

			if (number_of_show <= (_int)m_pCombat_Assistant->NumberOfColliders())
			{
				for (_int i = 0; i < number_of_show; ++i)
				{
					_vector pos = iter->pCollider->Get_Center();

					_float dist = iter->fDistance;

					_float3 dir = iter->vDirection;
					_float3 dirXZ = iter->vDirectionXZ;

					_float angle = iter->fAngleXZ;

					_bool up_down = iter->bUpDown;
					_bool left_right = iter->bLeftRight;

					ImGui::Separator();

					ImGui::Text("Target No. %d", i);
					ImGui::Text("Position : %.2f, %.2f, %.2f", vecx(pos), vecy(pos), vecz(pos));

					ImGui::Separator();

					ImGui::Text("Distance : %.2f", dist);
					ImGui::Text("Distance_XZ : %.2f", iter->fDistanceXZ);
					ImGui::Text("Distance_Y : %.2f", iter->fDistanceY);

					ImGui::Separator();

					ImGui::Text("Direction : %.2f, %.2f, %.2f", dir.x, dir.y, dir.z);
					ImGui::Text("Direction_XZ : %.2f, %.2f, %.2f", dirXZ.x, dirXZ.y, dirXZ.z);

					ImGui::Separator();

					ImGui::Text("Angle : %.2f", angle);

					ImGui::Separator();

					if (true == up_down)
						ImGui::Text("The target is [ Above ] the player");
					else
						ImGui::Text("The target is [ Below ] the player");

					if (true == left_right)
						ImGui::Text("The target is [ Left Side ] the player");
					else
						ImGui::Text("The target is [ Right Side ] the player");

					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();

					iter++;
				}
			}
		}

#pragma endregion

	}
}

void CPlayer::Control_ActionAssistant()
{
	if (ImGui::CollapsingHeader("Action Assistant Control"))
	{
		_bool Work_cancle = m_pAction_Assistant->Work(A_ASSIST::ASSIST_CANCLE);
		_bool Work_attack = m_pAction_Assistant->Work(A_ASSIST::ASSIST_ATTACK);

		ImGui::Checkbox("Work : Cancle Action", &Work_cancle);
		ImGui::Checkbox("Work : Cancle Attack", &Work_attack);

		m_pAction_Assistant->Work(A_ASSIST::ASSIST_CANCLE, Work_cancle);
		m_pAction_Assistant->Work(A_ASSIST::ASSIST_ATTACK, Work_attack);

		ImGui::Separator();

		_float cancle_progress = m_pAction_Assistant->CancleAction_Progress();
		_float attack_progress = m_pAction_Assistant->AttackNormal_Progress();

		ImGui::DragFloat("Cancle Progress", &cancle_progress, 0.025f, 0.f, 100.f);
		ImGui::DragFloat("Attack Progress", &attack_progress, 0.025f, 0.f, 100.f);

		m_pAction_Assistant->CancleAction_Progress(cancle_progress);
		m_pAction_Assistant->AttackNormal_Progress(attack_progress);

		ImGui::Separator();
		ImGui::Separator();

		if (m_pAction_Assistant->Able(A_ASSIST::ASSIST_CANCLE))
			ImGui::Text("ASSIST_CANCLE = true");
		else
			ImGui::Text("ASSIST_CANCLE = false");

		if (m_pAction_Assistant->Able(A_ASSIST::ASSIST_ATTACK))
			ImGui::Text("ASSIST_ATTACK = true");
		else
			ImGui::Text("ASSIST_ATTACK = false");

		if (m_pAction_Assistant->Able(A_ASSIST::ASSIST_MOVE))
			ImGui::Text("ASSIST_MOVE = true");
		else
			ImGui::Text("ASSIST_MOVE = false");

		if (m_pAction_Assistant->Able(A_ASSIST::ASSIST_GRAVITY))
			ImGui::Text("ASSIST_GRAVITY = true");
		else
			ImGui::Text("ASSIST_GRAVITY = false");
	}
}

_bool CPlayer::Check_DrawAnim(SORA_ANIM eSoraAnim)
{
	if (true == GM->Get_KeyBladeLock())
		return false;

	for (auto draw_anim : m_WeaponDrawAnim)
	{
		if (draw_anim == eSoraAnim)
			return true;
	}

	return false;
}

_bool CPlayer::Check_DrawAnim()
{
	SORA_ANIM current_anim = (SORA_ANIM)m_pModelCom->Get_CurrAnimIndex_EachBody(SB_BODY);
	for (auto draw_anim : m_WeaponDrawAnim)
	{
		if (current_anim == draw_anim)
			return true;
	}

	return false;
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlayer*	pInstance = new CPlayer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*	pInstance = new CPlayer(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pCombat_Assistant);
	Safe_Release(m_pAction_Assistant);

	Safe_Release(m_pWallFinderCom);

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pStateMachineCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pStatusCom);

	Safe_Release(m_pDiffuseTexture[SO_RAGE]);
	Safe_Release(m_pDiffuseTexture[SO_HAMMER]);

	//Test
	Safe_Release(m_pTextureCom);
}
