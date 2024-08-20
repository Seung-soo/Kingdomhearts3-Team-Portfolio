#include "stdafx.h"
#include "State_GoatAtk.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "Monster.h"
#include "MonsterSkill.h"
#include "PillarOfFire.h"
#include "Effect_Manager.h"
#include "SoundMgr.h"

CState_GoatAtk::CState_GoatAtk()
{
}

HRESULT CState_GoatAtk::Initialize(void* pArg)
{
	if (pArg == nullptr)
	{
		BREAKPOINT;
	}

	// Goat �� ����� ������ ��������
	memcpy(&m_eGoatSkill, pArg, sizeof(CGoat::GOAT_ATK));

	return S_OK;
}

HRESULT CState_GoatAtk::EnterState()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// Goat ����
	m_pGoat              = static_cast<CGoat*>(m_pGameObject);
	m_pMonsterNavigation = static_cast<CNavigation*>(m_pGameObject->Get_Component(TEXT("Com_Navigation")));

	// GoatWeapon ����
	m_pGoatWeapon          = static_cast<CGoatWeapon*>(pGameInstance->Get_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_GoatWeapon"), 0));
	m_pGoatWeaponTransform = m_pGoatWeapon->Get_Transform();

	if (m_pMonsterNavigation == nullptr)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// �÷��̾� Ʈ������ ����
	m_pPlayerTransform = static_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_LevelIndex(), TEXT("Layer_Player"), TEXT("Com_Transform"), 0));
	
	// ī�޶� Ʈ������ ����
	m_pCameraTransform = static_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_LevelIndex(), TEXT("Layer_Camera"), TEXT("Com_Transform")));

	// ī�޶� ���⺤�� ���
	m_vCamLook  =  XMVector3Normalize(XMVectorSetY(m_pCameraTransform->Get_Look(), 0.f));
	m_vCamBack  = -XMVector3Normalize(XMVectorSetY(m_pCameraTransform->Get_Look(), 0.f));
	m_vCamRight =  XMVector3Normalize(XMVectorSetY(m_pCameraTransform->Get_Right(), 0.f));
	m_vCamLeft  = -XMVector3Normalize(XMVectorSetY(m_pCameraTransform->Get_Right(), 0.f));

	// AnimPlayIndex �ʱ�ȭ
	m_pGoat->ResetAnimPlayIndex();

	// �ִϸ��̼� ���� �� �ʱⰪ ����
	switch (m_eGoatSkill)
	{
	case CGoat::GOAT_ATK_SPEAR:
		// ���Ʈ���� �ѱ�
		m_pGoat->Set_MotionTrail(true);
		m_pGoat->Set_AnimAndReset(m_pModelCom, m_Spear[0]);
		break;
	case CGoat::GOAT_ATK_TELEPORTSPEAR:
	{
		// �ڷ���Ʈ �Ϸ��� ��ġ��
		vector<_vector> vDir = { m_vCamLook,
								 m_vCamLook + m_vCamRight,
								 m_vCamLook + m_vCamLeft,
								 m_pTransformCom->Get_Up() };

		// �÷��̾������� �ڷ���Ʈ
		while (true)
		{
			// ���ѷ��� Ż��
			++m_iLoopAcc;

			_uint iRandNum = rand() % (vDir.size() - 1);
			if (m_iLoopAcc == 10)
				iRandNum = (_uint)(vDir.size() - 1);
			if (m_pTransformCom->TeleportAroundTarget(m_pPlayerTransform, vDir[iRandNum], 15.f, m_pMonsterNavigation) == true)
			{
				CEffect_Manager::GetInstance()->Create_GoatTeleport(m_pTransformCom);
				m_pGoat->StartTeleportDissolve();
				break;
			}
		}
		// �ִϸ��̼� ����
		m_pGoat->Set_AnimAndReset(m_pModelCom, m_Spear[0]);
		break;
	}
	case CGoat::GOAT_ATK_CONSECUTIVE_SPEAR:
	{
		while (true)
		{
			// ���ѷ��� Ż��
			++m_iLoopAcc;

			_uint iRandNum = rand() % (m_vConsecutiveDir.size() - 1);
			if (m_iLoopAcc == 10)
				iRandNum = (_uint)(m_vConsecutiveDir.size() - 1);
			if (m_pTransformCom->TeleportAroundTarget(m_pPlayerTransform, m_vConsecutiveDir[iRandNum], 7.5f, m_pMonsterNavigation) == true)
			{
				CEffect_Manager::GetInstance()->Create_GoatTeleport(m_pTransformCom);
				m_pGoat->StartTeleportDissolve();
				break;
			}
		}
		// �ִϸ��̼� ����
		m_pGoat->Set_AnimAndReset(m_pModelCom, m_ConsecutiveSpear[0]);
		break;
	}
		break;
	case CGoat::GOAT_ATK_FLYSWING:
		// ���Ʈ���� �ѱ�
		m_pGoat->Set_MotionTrail(true);
		m_pGoat->Set_AnimAndReset(m_pModelCom, m_FlySwing[0]);
		break;
	case CGoat::GOAT_ATK_TELEPORTFLYSWING:
	{
		// �ڷ���Ʈ �Ϸ��� ��ġ��
		vector<_vector> vDir = { m_vCamLook, 
								 m_vCamLook + m_vCamRight,
								 m_vCamLook + m_vCamLeft,
								 m_vCamRight, m_vCamLeft,
								 m_pTransformCom->Get_Up() };

		while (true)
		{
			// ���ѷ��� Ż��
			++m_iLoopAcc;

			_uint iRandNum = rand() % (vDir.size() - 1);
			if (m_iLoopAcc == 10)
				iRandNum = (_uint)(vDir.size() - 1);
			if (m_pTransformCom->TeleportAroundTarget(m_pPlayerTransform, vDir[iRandNum], 5.5f, m_pMonsterNavigation) == true)
			{
				CEffect_Manager::GetInstance()->Create_GoatTeleport(m_pTransformCom);
				m_pGoat->StartTeleportDissolve();
				break;
			}
		}
		// �ִϸ��̼� ����
		m_pGoat->Set_AnimAndReset(m_pModelCom, 25);
		break;
	}
	case CGoat::GOAT_ATK_DASHSWING:
		// ���Ʈ���� �ѱ�
		m_pGoat->Set_MotionTrail(true);
		m_pGoat->Set_AnimAndReset(m_pModelCom, m_DashSwing[0]);
		break;
	case CGoat::GOAT_ATK_TELEPORTDASHSWING:
	{
		// �ڷ���Ʈ �Ϸ��� ��ġ��
		vector<_vector> vDir = { m_pTransformCom->Get_Right() - m_pTransformCom->Get_Look(),
								-m_pTransformCom->Get_Right() - m_pTransformCom->Get_Look(),
								 m_pTransformCom->Get_Up() - m_pTransformCom->Get_Look(),
								-m_pTransformCom->Get_Up() - m_pTransformCom->Get_Look(),
								 m_pTransformCom->Get_Up() };

		// ���� �ֺ����� �ڷ���Ʈ
		while (true)
		{
			// ���ѷ��� Ż��
			++m_iLoopAcc;

			_uint iRandNum = rand() % (vDir.size() - 1);
			if (m_iLoopAcc == 10)
				iRandNum = (_uint)(vDir.size() - 1);
			if (m_pTransformCom->Teleport(vDir[iRandNum], 5.5f, m_pMonsterNavigation) == true)
			{
				CEffect_Manager::GetInstance()->Create_GoatTeleport(m_pTransformCom);
				m_pGoat->StartTeleportDissolve();
				break;
			}
		}
		// �ִϸ��̼� ����
		m_pGoat->Set_AnimAndReset(m_pModelCom, m_DashSwing[0]);
		break;
	}
	case CGoat::GOAT_ATK_SPINDASHSWING:
		// ���Ʈ���� �ѱ�
		m_pGoat->Set_MotionTrail(true);
		m_pGoat->Set_AnimAndReset(m_pModelCom, m_SpinDashSwing[0]);
		break;
	case CGoat::GOAT_ATK_TELEPORTSPINDASHSWING:
	{
		// �ڷ���Ʈ �Ϸ��� ��ġ��
		vector<_vector> vDir = { m_pTransformCom->Get_Right() - m_pTransformCom->Get_Look(),
								-m_pTransformCom->Get_Right() - m_pTransformCom->Get_Look(),
								 m_pTransformCom->Get_Up() - m_pTransformCom->Get_Look(),
								-m_pTransformCom->Get_Up() - m_pTransformCom->Get_Look(),
								 m_pTransformCom->Get_Up() };

		// ���� �ֺ����� �ڷ���Ʈ
		while (true)
		{
			// ���ѷ��� Ż��
			++m_iLoopAcc;

			_uint iRandNum = rand() % (vDir.size() - 1);
			if (m_iLoopAcc == 10)
				iRandNum = (_uint)(vDir.size() - 1);
			if (m_pTransformCom->Teleport(vDir[iRandNum], 5.5f, m_pMonsterNavigation) == true)
			{
				CEffect_Manager::GetInstance()->Create_GoatTeleport(m_pTransformCom);
				m_pGoat->StartTeleportDissolve();
				break;
			}
		}
		// �ִϸ��̼� ����
		m_pGoat->Set_AnimAndReset(m_pModelCom, m_SpinDashSwing[0]);
		break;
	}
	case CGoat::GOAT_ATK_JUMPDASHSWING:
		// ���Ʈ���� �ѱ�
		m_pGoat->Set_MotionTrail(true);
		m_pGoat->Set_AnimAndReset(m_pModelCom, m_JumpDashSwing[0]);
		break;
	case CGoat::GOAT_ATK_UNDERSWING:
		// ���Ʈ���� �ѱ�
		m_pGoat->Set_MotionTrail(true);
		m_pGoat->Set_AnimAndReset(m_pModelCom, m_UnderSwing[0]);
		break;
	case CGoat::GOAT_ATK_TELEPORTUNDERSWING:
	{
		// �ڷ���Ʈ �Ϸ��� ��ġ��
		vector<_vector> vDir = { m_vCamLook + m_pPlayerTransform->Get_Up(),
								 m_vCamLook + m_vCamRight + m_pPlayerTransform->Get_Up(),
								 m_vCamLook + m_vCamLeft + m_pPlayerTransform->Get_Up(),
								 m_pTransformCom->Get_Up() };

		// �÷��̾������� �ڷ���Ʈ
		while (true)
		{
			// ���ѷ��� Ż��
			++m_iLoopAcc;

			_uint iRandNum = rand() % (vDir.size() - 1);
			if (m_iLoopAcc == 10)
				iRandNum = (_uint)(vDir.size() - 1);
			if (m_pTransformCom->TeleportAroundTarget(m_pPlayerTransform, vDir[iRandNum], 3.f, m_pMonsterNavigation) == true)
			{
				CEffect_Manager::GetInstance()->Create_GoatTeleport(m_pTransformCom);
				m_pGoat->StartTeleportDissolve();
				break;
			}
		}
		// �ִϸ��̼� ����
		m_pGoat->Set_AnimAndReset(m_pModelCom, 35);
		break;
	}
	case CGoat::GOAT_ATK_LONGUNDERATK:
		m_pGoat->Set_AnimAndReset(m_pModelCom, m_LongUnderAtk[0]);
		break;
	case CGoat::GOAT_ATK_TELEPORTLONGUNDERATK:
		m_pGoat->Set_AnimAndReset(m_pModelCom, m_LongUnderAtk[0]);
		break;
	case CGoat::GOAT_ATK_CONSECUTIVE_SWING:
		// �ִϸ��̼� ����
		m_pGoat->Set_AnimAndReset(m_pModelCom, m_ConsecutiveSwing[0]);
		break;
	case CGoat::GOAT_ATK_MAGICSHOT:
		m_pGoat->Set_AnimAndReset(m_pModelCom, m_MagicShot[0]);
		break;
	case CGoat::GOAT_ATK_SHORTPILLAROFFIRE:
		m_pGoat->Set_AnimAndReset(m_pModelCom, m_ShortPillarOfFire[0]);
		break;
	case CGoat::GOAT_ATK_GRAVITYPILLAROFFIRE:
		m_pGoat->Set_AnimAndReset(m_pModelCom, m_GravityPillarOfFire[0]);
		m_pTransformCom->Set_LookY_Zero_Once(0.0);
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Goat_ChangePhase.wav"), 0.5f);
		break;
	case CGoat::GOAT_ATK_PILLAROFFIRE:
		m_pGoat->Set_AnimAndReset(m_pModelCom, m_PillarOfFire[0]);
		m_pTransformCom->Set_LookY_Zero_Once(0.0);
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Goat_ChangePhase.wav"), 0.5f);
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CState_GoatAtk::ClearState()
{
	// ���� ���� ����
	m_pGoat->SetIsWeaponAttacking(false);

	// ��Ǻ�����
	m_pGoat->Set_Pass(0);

	// �������
	CGameInstance::GetInstance()->StopMonsterSound(m_iCurrentSoundChannel);

	// ����Ʈ ����
	if (m_EffectObjects.size() != 0)
	{
		for (auto& pEffect : m_EffectObjects)
		{
			if (pEffect != nullptr)
				pEffect->Set_Dead(true);
		}
		m_EffectObjects.clear();
	}

	return S_OK;
}

_bool CState_GoatAtk::Action(_double TimeDelta)
{
	// �ð� ����
	m_TimeAcc += TimeDelta;
	m_EffectIntervalAcc += TimeDelta;

	// ī�޶� ���⺤�� ���
	m_vCamLook  =  XMVector3Normalize(XMVectorSetY(m_pCameraTransform->Get_Look(), 0.f));
	m_vCamBack  = -XMVector3Normalize(XMVectorSetY(m_pCameraTransform->Get_Look(), 0.f));
	m_vCamRight =  XMVector3Normalize(XMVectorSetY(m_pCameraTransform->Get_Right(), 0.f));
	m_vCamLeft  = -XMVector3Normalize(XMVectorSetY(m_pCameraTransform->Get_Right(), 0.f));

	_bool bStateEnd = false;

	switch (m_eGoatSkill)
	{
	case CGoat::GOAT_ATK_SPEAR:
		bStateEnd = Atk_Spear(TimeDelta);
		break;
	case CGoat::GOAT_ATK_TELEPORTSPEAR:
		bStateEnd = Atk_TeleportSpear(TimeDelta);
		break;
	case CGoat::GOAT_ATK_CONSECUTIVE_SPEAR:
		bStateEnd = Atk_ConsecutiveSpear(TimeDelta);
		break;
	case CGoat::GOAT_ATK_FLYSWING:
		bStateEnd = Atk_FlySwing(TimeDelta);
		break;
	case CGoat::GOAT_ATK_TELEPORTFLYSWING:
		bStateEnd = Atk_TeleportFlySwing(TimeDelta);
		break;
	case CGoat::GOAT_ATK_DASHSWING:
		bStateEnd = Atk_DashSwing(TimeDelta);
		break;
	case CGoat::GOAT_ATK_TELEPORTDASHSWING:
		bStateEnd = Atk_TeleportDashSwing(TimeDelta);
		break;
	case CGoat::GOAT_ATK_SPINDASHSWING:
		bStateEnd = Atk_SpinDashSwing(TimeDelta);
		break;
	case CGoat::GOAT_ATK_TELEPORTSPINDASHSWING:
		bStateEnd = Atk_TeleportSpinDashSwing(TimeDelta);
		break;
	case CGoat::GOAT_ATK_JUMPDASHSWING:
		bStateEnd = Atk_JumpDashSwing(TimeDelta);
		break;
	case CGoat::GOAT_ATK_UNDERSWING:
		bStateEnd = Atk_UnderSwing(TimeDelta);
		break;
	case CGoat::GOAT_ATK_TELEPORTUNDERSWING:
		bStateEnd = Atk_TeleportUnderSwing(TimeDelta);
		break;
	case CGoat::GOAT_ATK_LONGUNDERATK:
		bStateEnd = Atk_LongUnderAtk(TimeDelta);
		break;
	case CGoat::GOAT_ATK_TELEPORTLONGUNDERATK:
		bStateEnd = Atk_TeleportLongUnderAtk(TimeDelta);
		break;
	case CGoat::GOAT_ATK_CONSECUTIVE_SWING:
		bStateEnd = Atk_ConsecutiveSwing(TimeDelta);
		break;
	case CGoat::GOAT_ATK_MAGICSHOT:
		bStateEnd = Atk_MagicShot(TimeDelta);
		break;
	case CGoat::GOAT_ATK_SHORTPILLAROFFIRE:
		bStateEnd = Atk_ShortPillarOfFire(TimeDelta);
		break;
	case CGoat::GOAT_ATK_GRAVITYPILLAROFFIRE:
		bStateEnd = Atk_GravityPillarOfFire(TimeDelta);
		break;
	case CGoat::GOAT_ATK_PILLAROFFIRE:
		bStateEnd = Atk_PillarOfFire(TimeDelta);
		break;
	default:
		break;
	}

	if (bStateEnd == true)
	{
		// ���� ���� ����
		m_pGoat->SetIsWeaponAttacking(false);
		m_pGoat->Set_MotionTrail(false);
		return DELETE_STATE;
	}

	return KEEP_STATE;
}

_bool CState_GoatAtk::Atk_Spear(_double TimeDelta)
{
	// �����ݶ��̴� ó��
	if (m_pModelCom->Get_CurrAnimIndex() == 23)
	{
		m_pGoat->SetWeaponAttackTime(m_pModelCom, 23, 0.0, 14.0);
	}
	if (m_pModelCom->Get_CurrAnimIndex() == 24)
	{
		m_pGoat->SetWeaponAttackTime(m_pModelCom, 24, 0.0, 3.0);
		m_pGoat->SetMotionBlurTime(m_pModelCom, 24, 0.0, 3.0);
	}

	// 24��(��� ������) �ִϸ��̼��� �� ����� �÷��̾������� ȸ��
	Turn_ExceptAnim(24, TimeDelta);

	// ��� �غ����� ������ 23��(���) �ִϸ��̼����� ����
	if (m_pModelCom->Check_CurAnim_And_Finished(22))
	{
		// �̹� ��Ÿ� �����̸� �ٷ� ����
		if (m_pTransformCom->Get_Distance(m_pPlayerTransform) <= 3.f)
		{
			m_pGoat->Set_AnimAndReset(m_pModelCom, 24);
			CGameInstance::GetInstance()->LetsPlaySound_Monster(GetRandomSpearSound(), 0.6f, true, m_pTransformCom->Get_Position());
		}
		else
			m_pModelCom->Set_TempIndex(23);

		m_EffectObjects = CEffect_Manager::GetInstance()->Create_GoatSpear(m_pTransformCom->Get_WorldFloat4x4_Pointer());

		// �ٶ������� ����
		m_iCurrentSoundChannel = CGameInstance::GetInstance()->PlayMonsterRoofSound(TEXT("Goat_45_Wind.wav"), 0.7f, true, m_pTransformCom->Get_Position());
	}

	// 23�� �ִϸ��̼��� �� ��ƼŬ ����Ʈ ����鼭 ������ ���ư���
	if (m_pModelCom->Get_CurrAnimIndex() == 23)
	{
		if (m_EffectIntervalAcc >= 0.05)
		{
			CEffect_Manager::GetInstance()->Create_GoatParticle(m_pTransformCom, __vector(0.f, 2.f, 0.f, 0.f));
			m_EffectIntervalAcc = 0.0;
		}

		if (GoStraight_Limit(3.f, 3.0, TimeDelta))
		{
			m_pGoat->Set_AnimAndReset(m_pModelCom, 24);

			// �ٶ������� ���� �����ϰ� ��� ����
			CGameInstance::GetInstance()->StopMonsterSound(m_iCurrentSoundChannel);
			CGameInstance::GetInstance()->LetsPlaySound_Monster(GetRandomSpearSound(), 0.6f, true, m_pTransformCom->Get_Position());
		}
	}

	// 24�� �ִϸ��̼��� �� ������ ���ư���
	if (m_pModelCom->Get_TargetTimeAcc(24) <= 10.0)
		m_pTransformCom->Go_Straight(TimeDelta, m_pMonsterNavigation);

	// ����Ʈ ����
	if (m_pModelCom->Get_TargetTimeAcc(24) >= 5.0 && m_EffectObjects.size() != 0)
	{
		for (auto& pEffect : m_EffectObjects)
		{
			pEffect->Set_Dead(true);
		}
		m_EffectObjects.clear();
	}

	return m_pModelCom->Check_CurAnim_And_Finished(m_Spear.back());
}

_bool CState_GoatAtk::Atk_FlySwing(_double TimeDelta)
{
	// �����ݶ��̴� ó��
	m_pGoat->SetWeaponAttackTime(m_pModelCom, 25, 15.0, 20.0);
	m_pGoat->SetMotionBlurTime(m_pModelCom, 25, 15.0, 20.0);

	// 25��(����) �ִϸ��̼��� �� ����� �÷��̾������� ȸ��
	Turn_ExceptAnim(25, TimeDelta);

	// 5�� �ִϸ��̼�(��� �غ���)�� ������ 9�� �ִϸ��̼�(���) �� ����
	if (m_pModelCom->Check_CurAnim_And_Finished(5))
	{
		// �̹� ��Ÿ� �����̸� �ٷ� ����
		if (m_pTransformCom->Get_Distance(m_pPlayerTransform) <= 3.5f)
		{
			m_pGoat->Set_AnimAndReset(m_pModelCom, 25);
			m_pGoat->Set_MotionTrail(false);
		}
		else
		{
			m_pModelCom->Set_TempIndex(9);
			// �ٶ������� ����
			m_iCurrentSoundChannel = CGameInstance::GetInstance()->PlayMonsterRoofSound(TEXT("Goat_45_Wind.wav"), 0.7f, true, m_pTransformCom->Get_Position());
		}
	}

	// 9�� �ִϸ��̼��� �� ������ ���ư���
	if (m_pModelCom->Get_CurrAnimIndex() == 9)
	{
		if (GoStraight_Limit(3.5f, 3.0, TimeDelta))
		{
			m_pGoat->Set_AnimAndReset(m_pModelCom, 25);
			m_pGoat->Set_MotionTrail(false);
		}
	}

	if (m_pModelCom->Get_CurrAnimIndex() == 25)
		m_pTransformCom->Set_LookY_Zero(TimeDelta * 0.5);

	if (m_pModelCom->Get_TargetTimeAcc(25) >= 15.0 && m_bEffectEnd[0] == false)
	{
		CEffect_Manager::GetInstance()->Create_GoatFlySwing(m_pTransformCom);
		// �ٶ������� ���� �����ϰ� ���� ����
		CGameInstance::GetInstance()->StopMonsterSound(m_iCurrentSoundChannel);
		CGameInstance::GetInstance()->LetsPlaySound_Monster(GetRandomSwingSound(), 0.7f, true, m_pTransformCom->Get_Position());

		m_bEffectEnd[0] = true;
	}

	return m_pModelCom->Check_CurAnim_And_Finished(m_FlySwing.back());
}

_bool CState_GoatAtk::Atk_DashSwing(_double TimeDelta)
{
	// �����ݶ��̴� ó��
	if (m_pModelCom->Get_CurrAnimIndex() == 27)
		m_pGoat->SetWeaponAttackTime(m_pModelCom, 27, 0.0, 14.0);
	if (m_pModelCom->Get_CurrAnimIndex() == 28)
	{
		m_pGoat->SetWeaponAttackTime(m_pModelCom, 28, 0.0, 10.0);
		m_pGoat->SetMotionBlurTime(m_pModelCom, 28, 0.0, 10.0);
	}

	// 28�� �ִϸ��̼�(����)�� �ƴ� ���� �÷��̾������� ȸ��
	Turn_ExceptAnim(28, TimeDelta);

	// ���� �غ����� ������ 27��(�����Ϸ� ���ư���) �ִϸ��̼����� ����
	if (m_pModelCom->Check_CurAnim_And_Finished(26))
	{
		// �̹� ��Ÿ� �����̸� �ٷ� ����
		if (m_pTransformCom->Get_Distance(m_pPlayerTransform) <= 3.f)
		{
			m_pGoat->Set_AnimAndReset(m_pModelCom, 28);
			m_pGoat->Set_MotionTrail(false);
		}
		else
		{
			m_pModelCom->Set_TempIndex(27);
			// �ٶ������� ����
			m_iCurrentSoundChannel = CGameInstance::GetInstance()->PlayMonsterRoofSound(TEXT("Goat_45_Wind.wav"), 0.7f, true, m_pTransformCom->Get_Position());
		}
	}

	// 27�� �ִϸ��̼��� �� ������ ���ư���
	if (m_pModelCom->Get_CurrAnimIndex() == 27)
	{
		if (GoStraight_Limit(3.f, 3.0, TimeDelta))
		{
			m_pGoat->Set_AnimAndReset(m_pModelCom, 28);
			m_pGoat->Set_MotionTrail(false);
		}
	}

	if (m_pModelCom->Get_CurrAnimIndex() == 28)
		m_pTransformCom->Set_LookY_Zero(TimeDelta * 0.5);

	if (m_pModelCom->Get_TargetTimeAcc(28) >= 4.0 && m_bEffectEnd[0] == false)
	{
		CEffect_Manager::GetInstance()->Create_GoatDashSwing(m_pTransformCom);
		// �ٶ������� ���� �����ϰ� ���� ����
		CGameInstance::GetInstance()->StopMonsterSound(m_iCurrentSoundChannel);
		CGameInstance::GetInstance()->LetsPlaySound_Monster(GetRandomSwingSound(), 0.7f, true, m_pTransformCom->Get_Position());

		m_bEffectEnd[0] = true;
	}

	return m_pModelCom->Check_CurAnim_And_Finished(m_DashSwing.back());
}

_bool CState_GoatAtk::Atk_SpinDashSwing(_double TimeDelta)
{
	// �����ݶ��̴� ó��
	if (m_pModelCom->Get_CurrAnimIndex() == 30)
	{
		m_pGoat->SetWeaponAttackTime(m_pModelCom, 30, 0.0, 9.0);
		m_pGoat->SetMotionBlurTime(m_pModelCom, 31, 0.0, 9.0);
	}
	if (m_pModelCom->Get_CurrAnimIndex() == 31)
	{
		m_pGoat->SetWeaponAttackTime(m_pModelCom, 31, 0.0, 20.0);
		m_pGoat->SetMotionBlurTime(m_pModelCom, 31, 0.0, 20.0);
	}

	m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);
	m_pTransformCom->Turn_To_Direction_AxisX(m_pPlayerTransform, TimeDelta);

	// ���� �غ����� ������ 30��(����) �ִϸ��̼����� ����
	if (m_pModelCom->Check_CurAnim_And_Finished(29))
	{
		m_pGoat->Set_MotionTrail(false);

		// �̹� ��Ÿ� �����̸� �ٷ� ����
		if (m_pTransformCom->Get_Distance(m_pPlayerTransform) <= 0.5f)
			m_pGoat->Set_AnimAndReset(m_pModelCom, 31);
		else
		{
			m_pModelCom->Set_TempIndex(30);
			m_EffectObjects = CEffect_Manager::GetInstance()->Create_GoatSpinDashSwing(m_pTransformCom);
			m_iCurrentSoundChannel = CGameInstance::GetInstance()->PlayMonsterRoofSound(TEXT("Goat_43_Spin.wav"), 0.7f, true, m_pTransformCom->Get_Position());
		}
	}

	// 30�� �ִϸ��̼��� �� ������ ���ư���
	if (m_pModelCom->Get_CurrAnimIndex() == 30)
	{
		if (GoStraight_Limit(0.5f, 4.0, TimeDelta * 0.65))
		{
			m_pGoat->Set_AnimAndReset(m_pModelCom, 31); 
			CGameInstance::GetInstance()->StopMonsterSound(m_iCurrentSoundChannel);
		}
	}

	if (m_pModelCom->Get_CurrAnimIndex() == 31)
		m_pTransformCom->Set_LookY_Zero(TimeDelta * 0.5);

	if (m_pModelCom->Get_TargetTimeAcc(31) >= 10.0 && m_bEffectEnd[0] == false)
	{
		CEffect_Manager::GetInstance()->Create_SpinDashSwingFinish(m_pTransformCom);
		CGameInstance::GetInstance()->LetsPlaySound_Monster(GetRandomSwingSound(), 0.7f, true, m_pTransformCom->Get_Position());

		m_bEffectEnd[0] = true;
	}

	// ����Ʈ ����
	if (m_pModelCom->Get_TargetTimeAcc(31) >= 1.0 && m_EffectObjects.size() != 0)
	{
		for (auto& pEffect : m_EffectObjects)
		{
			pEffect->Set_Dead(true);
		}
		m_EffectObjects.clear();
	}

	return m_pModelCom->Check_CurAnim_And_Finished(m_SpinDashSwing.back());
}

_bool CState_GoatAtk::Atk_JumpDashSwing(_double TimeDelta)
{
	// �����ݶ��̴� ó��
	if (m_pModelCom->Get_CurrAnimIndex() == 33)
		m_pGoat->SetWeaponAttackTime(m_pModelCom, 33, 0.0, 14.0);
	if (m_pModelCom->Get_CurrAnimIndex() == 34)
	{
		m_pGoat->SetWeaponAttackTime(m_pModelCom, 34, 0.0, 20.0);
		m_pGoat->SetMotionBlurTime(m_pModelCom, 34, 0.0, 20.0);
	}

	// 34�� �ִϸ��̼�(����)�� �ƴ� ���� �÷��̾������� ȸ��
	Turn_ExceptAnim(34, TimeDelta);

	// �غ����� ������ 33��(����) �ִϸ��̼����� ����
	if (m_pModelCom->Check_CurAnim_And_Finished(32))
	{
		// �̹� ��Ÿ� �����̸� �ٷ� ����
		if (m_pTransformCom->Get_Distance(m_pPlayerTransform) <= 3.5f)
		{
			m_pGoat->Set_AnimAndReset(m_pModelCom, 34);
			m_pGoat->Set_MotionTrail(false);
		}
		else
		{
			m_pModelCom->Set_TempIndex(33);
			// �ٶ������� ����
			m_iCurrentSoundChannel = CGameInstance::GetInstance()->PlayMonsterRoofSound(TEXT("Goat_45_Wind.wav"), 0.7f, true, m_pTransformCom->Get_Position());
		}
	}

	// 33�� �ִϸ��̼��� �� ������ ���ư���
	if (m_pModelCom->Get_CurrAnimIndex() == 33)
	{
		if (GoStraight_Limit(3.5f, 3.5, TimeDelta))
		{
			m_pGoat->Set_AnimAndReset(m_pModelCom, 34);
			m_pGoat->Set_MotionTrail(false);
		}
	}

	if (m_pModelCom->Get_CurrAnimIndex() == 34)
	{
		if (m_pModelCom->Get_TargetTimeAcc(34) <= 20.0)
		{
			if (m_EffectIntervalAcc >= 0.05)
			{
				CEffect_Manager::GetInstance()->Create_GoatParticle(m_pTransformCom, __vector(0.f, 2.f, 0.f, 0.f));
				m_EffectIntervalAcc = 0.0;
			}
		}

		m_pTransformCom->Set_LookY_Zero(TimeDelta * 0.5);
	}

	if (m_pModelCom->Get_TargetTimeAcc(34) >= 5.0 && m_bEffectEnd[0] == false)
	{
		CEffect_Manager::GetInstance()->Create_GoatJumpDashSwing(m_pTransformCom);
		// �ٶ������� ���� �����ϰ� ���� ����
		CGameInstance::GetInstance()->StopMonsterSound(m_iCurrentSoundChannel);
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Goat_34_UnderAtk.wav"), 0.7f, true, m_pTransformCom->Get_Position());

		m_bEffectEnd[0] = true;
	}

	return m_pModelCom->Check_CurAnim_And_Finished(m_JumpDashSwing.back());
}

_bool CState_GoatAtk::Atk_UnderSwing(_double TimeDelta)
{
	// �����ݶ��̴� ó��
	m_pGoat->SetWeaponAttackTime(m_pModelCom, 35, 1.0, 20.0);
	m_pGoat->SetMotionBlurTime(m_pModelCom, 35, 1.0, 20.0);

	// 35��(����) �ִϸ��̼��� �� ����� �÷��̾������� ȸ��
	Turn_ExceptAnim(35, TimeDelta);

	// 5�� �ִϸ��̼�(��� �غ���)�� ������ 9�� �ִϸ��̼�(���) �� ����
	if (m_pModelCom->Check_CurAnim_And_Finished(5))
	{
		// �̹� ��Ÿ� �����̸� �ٷ� ����
		if (m_pTransformCom->Get_Distance(m_pPlayerTransform) <= 3.5f)
		{
			m_pTransformCom->Set_LookY_Zero_Once(TimeDelta);
			m_pGoat->Set_AnimAndReset(m_pModelCom, 35);
			m_pGoat->Set_MotionTrail(false);
		}
		else
		{
			m_pModelCom->Set_TempIndex(9);
			// �ٶ������� ����
			m_iCurrentSoundChannel = CGameInstance::GetInstance()->PlayMonsterRoofSound(TEXT("Goat_45_Wind.wav"), 0.7f, true, m_pTransformCom->Get_Position());
		}
	}

	// 9�� �ִϸ��̼��� �� ������ ���ư���
	if (m_pModelCom->Get_CurrAnimIndex() == 9)
	{
		if (GoStraight_Limit(3.5f, 3.0, TimeDelta))
		{
			m_pTransformCom->Set_LookY_Zero_Once(TimeDelta);
			m_pGoat->Set_AnimAndReset(m_pModelCom, 35);
			m_pGoat->Set_MotionTrail(false);
		}
	}

	if (m_pModelCom->Get_TargetTimeAcc(35) >= 10.0 && m_bEffectEnd[0] == false)
	{
		CEffect_Manager::GetInstance()->Create_GoatUnderSwing(m_pTransformCom);
		// �ٶ������� ���� �����ϰ� ������� ����
		CGameInstance::GetInstance()->StopMonsterSound(m_iCurrentSoundChannel);
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Goat_34_UnderAtk.wav"), 0.7f, true, m_pTransformCom->Get_Position());
		m_bEffectEnd[0] = true;
	}

	return m_pModelCom->Check_CurAnim_And_Finished(m_UnderSwing.back());
}

_bool CState_GoatAtk::Atk_LongUnderAtk(_double TimeDelta)
{
	// ���Ʈ���� �ٷ� ����
	m_pGoat->Set_MotionTrail(false);
	// ��Ǻ� �ѱ�
	m_pGoat->SetMotionBlurTime(m_pModelCom, 37, 0.0, 127.0);
	return m_pGoat->PlayAnimations(m_pModelCom, m_LongUnderAtk);
}

_bool CState_GoatAtk::Atk_ConsecutiveSwing(_double TimeDelta)
{
	// 31 -> 28 -> 31 -> 24 -> 35
	// �����ݶ��̴� ó��
	if (m_pModelCom->Get_CurrAnimIndex() == 31)
	{
		m_pGoat->SetWeaponAttackTime(m_pModelCom, 31, 0.0, 20.0);
		m_pGoat->SetMotionBlurTime(m_pModelCom, 31, 0.0, 20.0);
	}
	if (m_pModelCom->Get_CurrAnimIndex() == 28)
	{
		m_pGoat->SetWeaponAttackTime(m_pModelCom, 28, 0.0, 10.0);
		m_pGoat->SetMotionBlurTime(m_pModelCom, 28, 0.0, 10.0);
	}
	if (m_pModelCom->Get_CurrAnimIndex() == 24)
	{
		m_pGoat->SetWeaponAttackTime(m_pModelCom, 24, 0.0, 3.0);
		m_pGoat->SetMotionBlurTime(m_pModelCom, 24, 0.0, 3.0);
	}
	m_pGoat->SetWeaponAttackTime(m_pModelCom, 35, 1.0, 20.0);
	m_pGoat->SetMotionBlurTime(m_pModelCom, 35, 1.0, 20.0);

	if (m_pModelCom->Get_CurrAnimIndex() == 31 ||
		m_pModelCom->Get_CurrAnimIndex() == 28 ||
		m_pModelCom->Get_CurrAnimIndex() == 24)
		m_pTransformCom->Set_LookY_Zero(TimeDelta * 0.5);

	_double SpeedPerTime = TimeDelta * 0.35;
	_float	fMinDistance = 3.5f;

	// 35�� �ִϸ��̼� �ƴ� �� + �Ÿ��� �� ���� ȸ��
	if (m_pTransformCom->Get_Distance(m_pPlayerTransform))
		Turn_ExceptAnim(35, TimeDelta);

	// ���ݸ��� ������ ����
	if (m_pModelCom->Get_TargetTimeAcc(31) >= 10.0 && m_pModelCom->Get_TargetTimeAcc(31) <= 15.0)
	{
		if (m_pTransformCom->Get_Distance(m_pPlayerTransform) >= fMinDistance)
			m_pTransformCom->Go_Straight(SpeedPerTime, m_pMonsterNavigation);
	}

	// ���ݸ��� ������ ����
	if (m_pModelCom->Get_CurrAnimIndex() == 28 && m_pModelCom->Get_TargetTimeAcc(28) <= 3.0) 
	{
		if (m_pTransformCom->Get_Distance(m_pPlayerTransform) >= fMinDistance)
			m_pTransformCom->Go_Straight(SpeedPerTime, m_pMonsterNavigation);
	}

	// ���ݸ��� ������ ����
	if (m_pModelCom->Get_TargetTimeAcc(24) <= 5.0)
	{
		m_pTransformCom->Go_Straight(SpeedPerTime, m_pMonsterNavigation);
	}

	// �÷��̾������� �ڷ���Ʈ
	if (m_pModelCom->Check_CurAnim_And_Finished(24))
	{
		vector<_vector> vDir = { m_vCamLook + m_pPlayerTransform->Get_Up(),
								 m_vCamLook + m_vCamRight + m_pPlayerTransform->Get_Up(),
								 m_vCamLook + m_vCamLeft + m_pPlayerTransform->Get_Up(),
								 m_pTransformCom->Get_Up() };

		while (true)
		{
			// ���ѷ��� Ż��
			++m_iLoopAcc;

			_uint iRandNum = rand() % (vDir.size() - 1);
			if (m_iLoopAcc == 10)
				iRandNum = (_uint)(vDir.size() - 1);
			if (m_pTransformCom->TeleportAroundTarget(m_pPlayerTransform, vDir[iRandNum], 3.f, m_pMonsterNavigation) == true)
			{
				CEffect_Manager::GetInstance()->Create_GoatTeleport(m_pTransformCom);
				m_pGoat->StartTeleportDissolve();
				break;
			}
		}
	}

	// ����Ʈ
	if (m_pModelCom->Get_TargetTimeAcc(31) >= 10.0 && m_bEffectEnd[0] == false)
	{
		CEffect_Manager::GetInstance()->Create_SpinDashSwingFinish(m_pTransformCom);
		CGameInstance::GetInstance()->LetsPlaySound_Monster(GetRandomSwingSound(), 0.7f, true, m_pTransformCom->Get_Position());
		m_bEffectEnd[0] = true;
	}

	if (m_pModelCom->Check_CurAnim_And_Finished(31))
		m_bEffectEnd[0] = false;

	if (m_pModelCom->Get_TargetTimeAcc(28) >= 4.0 && m_bEffectEnd[1] == false)
	{
		CEffect_Manager::GetInstance()->Create_GoatDashSwing(m_pTransformCom);
		CGameInstance::GetInstance()->LetsPlaySound_Monster(GetRandomSwingSound(), 0.7f, true, m_pTransformCom->Get_Position());
		m_bEffectEnd[1] = true;
	}

	if (m_pModelCom->Get_CurrAnimIndex() == 30 && m_bEffectEnd[2] == false)
	{
		m_EffectObjects = CEffect_Manager::GetInstance()->Create_GoatSpinDashSwing(m_pTransformCom);
		CGameInstance::GetInstance()->LetsPlaySound_Monster(GetRandomSwingSound(), 0.7f, true, m_pTransformCom->Get_Position());
		m_bEffectEnd[2] = true;
	}

	// ����Ʈ ����
	if (m_pModelCom->Check_CurAnim_And_Finished(30))
	{
		for (auto& pEffect : m_EffectObjects)
		{
			pEffect->Set_Dead(true);
		}
		m_EffectObjects.clear();
	}

	if (m_pModelCom->Get_CurrAnimIndex() == 24 && m_bEffectEnd[3] == false)
	{
		m_EffectObjects = CEffect_Manager::GetInstance()->Create_GoatSpear(m_pTransformCom->Get_WorldFloat4x4_Pointer());
		CGameInstance::GetInstance()->LetsPlaySound_Monster(GetRandomSpearSound(), 0.6f, true, m_pTransformCom->Get_Position());

		m_bEffectEnd[3] = true;
	}

	// ����Ʈ ����
	if (m_pModelCom->Get_TargetTimeAcc(24) >= 5.0 && m_EffectObjects.size() != 0)
	{
		for (auto& pEffect : m_EffectObjects)
		{
			pEffect->Set_Dead(true);
		}
		m_EffectObjects.clear();
	}

	if (m_pModelCom->Get_TargetTimeAcc(35) >= 10.0 && m_bEffectEnd[4] == false)
	{
		CEffect_Manager::GetInstance()->Create_GoatUnderSwing(m_pTransformCom);
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Goat_34_UnderAtk.wav"), 0.7f, true, m_pTransformCom->Get_Position());
		m_bEffectEnd[4] = true;
	}

	return m_pGoat->PlayAnimations(m_pModelCom, m_ConsecutiveSwing);
}

_bool CState_GoatAtk::Atk_MagicShot(_double TimeDelta)
{
	_vector vRight = XMVector3Normalize(m_pTransformCom->Get_Right());
	_vector vUp    = XMVector3Normalize(m_pTransformCom->Get_Up());
	_vector vLook  = XMVector3Normalize(m_pTransformCom->Get_Look());
	_vector vLeft  = -XMVector3Normalize(m_pTransformCom->Get_Right());
	_vector vBack  = -XMVector3Normalize(m_pTransformCom->Get_Look());
	_float fOffset = 10.f;

	_vector vDirs[8]{
		vRight, vLeft, vLook, vBack,
		vLook + vRight, vLook + vLeft, vBack + vRight, vBack + vLeft
	};

	if (m_TimeAcc >= 0.2)
	{
		// ���� ��ų ����
		CMonsterSkill::MONSTERSKILLDESC MonsterSkillDesc;
		MonsterSkillDesc.eSkillType       = CMonsterSkill::MONSTERSKILLTYPE_GOATMAGICSHOT;
		MonsterSkillDesc.bNoModel         = true;
		MonsterSkillDesc.fSpeed           = 50.f;
		MonsterSkillDesc.fDamage          = 5.f;
		MonsterSkillDesc.Duration         = 7.f;
		MonsterSkillDesc.fSize            = 2.5f;
		MonsterSkillDesc.pPlayerTransform = m_pPlayerTransform;

		CGameObject* pSkill = nullptr;

		MonsterSkillDesc.vPos = m_pTransformCom->Get_Position() + vDirs[iIndex] * fOffset + __vector(0.f, 5.f, 0.f, 0.f);
		MonsterSkillDesc.RandTime = (rand() % 30) * 0.1 + 1.0;

		pSkill = CGameInstance::GetInstance()->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_MonsterSkill"), TEXT("Prototype_GameObject_MonsterSkill"), &MonsterSkillDesc);
		CEffect_Manager::GetInstance()->Create_Goat_FireBall(pSkill->Get_Transform(), pSkill->Get_DeadPointer());

		++iIndex;
		m_TimeAcc = 0.0;
	}

	return m_pGoat->PlayAnimations(m_pModelCom, m_MagicShot);
}

_bool CState_GoatAtk::Atk_ShortPillarOfFire(_double TimeDelta)
{
	m_pTransformCom->Set_LookY_Zero(TimeDelta * 0.5);

	if (m_pModelCom->Get_TargetTimeAcc(40) >= 15.0 && m_bSkillEnd[0] == false)
	{
		// ª�� �ұ�� ����
		CPillarOfFire::PILLARDESC PillarDesc;
		PillarDesc.vPosition    = XMVectorSetY(m_pPlayerTransform->Get_Position(), 0.f);
		PillarDesc.bGravitation = false;
		PillarDesc.bShortPillar = true;

		CGameObject* pSkill = CGameInstance::GetInstance()->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_PillarOfFire"), TEXT("Prototype_GameObject_PillarOfFire"), &PillarDesc);

		m_bSkillEnd[0] = true;
	}

	return m_pGoat->PlayAnimations(m_pModelCom, m_ShortPillarOfFire);
}

_bool CState_GoatAtk::Atk_GravityPillarOfFire(_double TimeDelta)
{
	// ������� �ұ�� ����
	if (m_pModelCom->Get_TargetTimeAcc(37) >= 70.0 && m_bSkillEnd[0] == false)
	{
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Goat_SummonPillar.wav"), 0.5f);

		CPillarOfFire::PILLARDESC PillarDesc;
		PillarDesc.vPosition = __vector(0.f, 0.f, 0.f, 1.f);
		PillarDesc.bGravitation = true;

		CGameObject* pSkill = CGameInstance::GetInstance()->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_PillarOfFire"), TEXT("Prototype_GameObject_PillarOfFire"), &PillarDesc);

		CEffect_Manager::GetInstance()->Create_GoatPillarOfFire(pSkill->Get_Transform());

		m_bSkillEnd[0] = true;
	}

	// ª�� �ұ�� ����
	if (m_pModelCom->Get_TargetTimeAcc(40) >= 15.0 && m_bSkillEnd[1] == false)
	{
		CPillarOfFire::PILLARDESC PillarDesc;
		PillarDesc.vPosition = XMVectorSetY(m_pPlayerTransform->Get_Position(), 0.f);
		PillarDesc.bGravitation = false;
		PillarDesc.bShortPillar = true;

		CGameObject* pSkill = CGameInstance::GetInstance()->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_PillarOfFire"), TEXT("Prototype_GameObject_PillarOfFire"), &PillarDesc);

		m_bSkillEnd[1] = true;
	}

	if (m_pModelCom->Check_CurAnim_And_Finished(40))
		m_bSkillEnd[1] = false;

	return m_pGoat->PlayAnimations(m_pModelCom, m_GravityPillarOfFire);
}

_bool CState_GoatAtk::Atk_PillarOfFire(_double TimeDelta)
{
	if (m_pModelCom->Get_TargetTimeAcc(37) >= 70.0 && m_bSkillEnd[0] == false)
	{
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Goat_SummonPillar.wav"), 0.5f);

		// �����ϴ� �ұ�� ����
		_float fDistance = 25.f;

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		CGameObject* pSkill = nullptr;

		_vector vPos = m_pPlayerTransform->Get_Position();
		vPos = XMVectorSetY(vPos, 0.f);

		CPillarOfFire::PILLARDESC PillarDesc;
		PillarDesc.bGravitation = false;

		PillarDesc.vPosition = vPos + __vector(fDistance, 0.f, 0.f, 0.f);
		pSkill = pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_PillarOfFire"), TEXT("Prototype_GameObject_PillarOfFire"), &PillarDesc);
		CEffect_Manager::GetInstance()->Create_GoatPillarOfFire_Rotation(pSkill->Get_Transform());

		PillarDesc.vPosition = vPos + __vector(-fDistance, 0.f, 0.f, 0.f);
		pSkill = pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_PillarOfFire"), TEXT("Prototype_GameObject_PillarOfFire"), &PillarDesc);
		CEffect_Manager::GetInstance()->Create_GoatPillarOfFire_Rotation(pSkill->Get_Transform());

		PillarDesc.vPosition = vPos + __vector(0.f, 0.f, fDistance, 0.f);
		pSkill = pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_PillarOfFire"), TEXT("Prototype_GameObject_PillarOfFire"), &PillarDesc);
		CEffect_Manager::GetInstance()->Create_GoatPillarOfFire_Rotation(pSkill->Get_Transform());

		PillarDesc.vPosition = vPos + __vector(0.f, 0.f, -fDistance, 0.f);
		pSkill = pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_PillarOfFire"), TEXT("Prototype_GameObject_PillarOfFire"), &PillarDesc);
		CEffect_Manager::GetInstance()->Create_GoatPillarOfFire_Rotation(pSkill->Get_Transform());

		_uint SoundChannel = CGameInstance::GetInstance()->PlayMonsterRoofSound(TEXT("Goat_57_PillarOfFire.wav"), 0.7f, true, m_pTransformCom->Get_Position());
		static_cast<CPillarOfFire*>(pSkill)->SetSoundChannel(SoundChannel);

		m_bSkillEnd[0] = true;
	}

	return m_pGoat->PlayAnimations(m_pModelCom, m_PillarOfFire);
}

_bool CState_GoatAtk::Atk_TeleportSpear(_double TimeDelta)
{
	// �����ݶ��̴� ó��
	if (m_pModelCom->Get_CurrAnimIndex() == 23)
		m_pGoat->SetWeaponAttackTime(m_pModelCom, 23, 0.0, 14.0);
	if (m_pModelCom->Get_CurrAnimIndex() == 24)
	{
		m_pGoat->SetWeaponAttackTime(m_pModelCom, 24, 0.0, 3.0);
		m_pGoat->SetMotionBlurTime(m_pModelCom, 24, 0.0, 3.0);
	}

	// 24��(��� ������) �ִϸ��̼��� �� ����� �÷��̾������� ȸ��
	Turn_ExceptAnim(24, TimeDelta);

	// ��� �غ����� ������ 23��(���) �ִϸ��̼����� ����
	if (m_pModelCom->Check_CurAnim_And_Finished(22))
	{
		m_pModelCom->Set_TempIndex(23);

		m_EffectObjects = CEffect_Manager::GetInstance()->Create_GoatSpear(m_pTransformCom->Get_WorldFloat4x4_Pointer());

		// �ٶ������� ����
		m_iCurrentSoundChannel = CGameInstance::GetInstance()->PlayMonsterRoofSound(TEXT("Goat_45_Wind.wav"), 0.7f, true, m_pTransformCom->Get_Position());
	}

	// 23�� �ִϸ��̼��� �� ������ ���ư���
	if (m_pModelCom->Get_CurrAnimIndex() == 23)
	{
		if (m_EffectIntervalAcc >= 0.05)
		{
			CEffect_Manager::GetInstance()->Create_GoatParticle(m_pTransformCom, __vector(0.f, 2.f, 0.f, 0.f));
			m_EffectIntervalAcc = 0.0;
		}

		if (GoStraight_Limit(3.f, 3.0, TimeDelta))
		{
			m_pGoat->Set_AnimAndReset(m_pModelCom, 24);
			// �ٶ������� ���� �����ϰ� ��� ����
			CGameInstance::GetInstance()->StopMonsterSound(m_iCurrentSoundChannel);
			CGameInstance::GetInstance()->LetsPlaySound_Monster(GetRandomSpearSound(), 0.6f, true, m_pTransformCom->Get_Position());
		}
	}

	// 24�� �ִϸ��̼��� �� ������ ���ư���
	if (m_pModelCom->Get_TargetTimeAcc(24) <= 10.0)
		m_pTransformCom->Go_Straight(TimeDelta, m_pMonsterNavigation);

	// ����Ʈ ����
	if (m_pModelCom->Get_TargetTimeAcc(24) >= 5.0 && m_EffectObjects.size() != 0)
	{
		// ����Ʈ �����ֱ�
		for (auto& pEffect : m_EffectObjects)
		{
			pEffect->Set_Dead(true);
		}
		m_EffectObjects.clear();
	}

	return m_pModelCom->Check_CurAnim_And_Finished(m_Spear.back());
}

_bool CState_GoatAtk::Atk_ConsecutiveSpear(_double TimeDelta)
{
	// �����ݶ��̴� ó��
	if (m_pModelCom->Get_CurrAnimIndex() == 23)
		m_pGoat->SetWeaponAttackTime(m_pModelCom, 23, 0.0, 14.0);
	if (m_pModelCom->Get_CurrAnimIndex() == 24)
	{
		m_pGoat->SetWeaponAttackTime(m_pModelCom, 24, 0.0, 3.0);
		m_pGoat->SetMotionBlurTime(m_pModelCom, 24, 0.0, 3.0);
	}

	// ��� �غ����� ������ 23��(���) �ִϸ��̼����� ����
	if (m_pModelCom->Check_CurAnim_And_Finished(22))
	{
		m_pModelCom->Set_TempIndex(23);

		m_EffectObjects = CEffect_Manager::GetInstance()->Create_GoatSpear(m_pTransformCom->Get_WorldFloat4x4_Pointer());

		// �ٶ������� ����
		m_iCurrentSoundChannel = CGameInstance::GetInstance()->PlayMonsterRoofSound(TEXT("Goat_45_Wind.wav"), 0.7f, true, m_pTransformCom->Get_Position());
	}

	// 23�� �ִϸ��̼��� �� ������ ���ư���
	if (m_pModelCom->Get_CurrAnimIndex() == 23)
	{
		if (m_EffectIntervalAcc >= 0.05)
		{
			CEffect_Manager::GetInstance()->Create_GoatParticle(m_pTransformCom, __vector(0.f, 2.f, 0.f, 0.f));
			m_EffectIntervalAcc = 0.0;
		}

		if (GoStraight_Limit(3.f, 1.0, TimeDelta * 2.0))
		{
			m_pGoat->Set_AnimAndReset(m_pModelCom, 24);

			// �ٶ������� ���� �����ϰ� ��� ����
			CGameInstance::GetInstance()->StopMonsterSound(m_iCurrentSoundChannel);
			CGameInstance::GetInstance()->LetsPlaySound_Monster(GetRandomSpearSound(), 0.6f, true, m_pTransformCom->Get_Position());
		}
	}

	// 24�� �ִϸ��̼��� �� ������ ���ư���
	if (m_pModelCom->Get_TargetTimeAcc(24) <= 10.0)
	{
		if (m_EffectIntervalAcc >= 0.05)
		{
			CEffect_Manager::GetInstance()->Create_GoatParticle(m_pTransformCom, __vector(0.f, 2.f, 0.f, 0.f));
			m_EffectIntervalAcc = 0.0;
		}

		m_pTransformCom->Go_Straight(TimeDelta, m_pMonsterNavigation);
	}

	// ����Ʈ ����
	if (m_pModelCom->Get_TargetTimeAcc(24) >= 5.0 && m_EffectObjects.size() != 0)
	{
		for (auto& pEffect : m_EffectObjects)
		{
			pEffect->Set_Dead(true);
		}
		m_EffectObjects.clear();
	}

	if (m_pModelCom->Get_TargetTimeAcc(24) >= 13.0)
	{
		// m_iAtkLoopAcc ����
		m_iAtkLoopAcc += 1;

		if (m_iAtkLoopAcc == 5)
			return true;

		// �÷��̾������� �ڷ���Ʈ
		while (true)
		{
			// ���ѷ��� Ż��
			++m_iLoopAcc;

			_uint iRandNum = rand() % (m_vConsecutiveDir.size() - 1);
			if (m_iLoopAcc == 10)
				iRandNum = (_uint)(m_vConsecutiveDir.size() - 1);
			if (m_pTransformCom->TeleportAroundTarget(m_pPlayerTransform, m_vConsecutiveDir[iRandNum], 7.5f, m_pMonsterNavigation) == true)
			{
				CEffect_Manager::GetInstance()->Create_GoatTeleport(m_pTransformCom);
				m_pGoat->StartTeleportDissolve();
				break;
			}
		}
		// �ִϸ��̼� ����
		m_pModelCom->ResetAnimation(24);
		m_pGoat->Set_AnimAndReset(m_pModelCom, 23);

		m_EffectObjects = CEffect_Manager::GetInstance()->Create_GoatSpear(m_pTransformCom->Get_WorldFloat4x4_Pointer());

		// m_TimeAcc �ʱ�ȭ
		m_TimeAcc = 0.0;
	}

	return false;
}

_bool CState_GoatAtk::Atk_TeleportFlySwing(_double TimeDelta)
{
	// �����ݶ��̴� ó��
	m_pGoat->SetWeaponAttackTime(m_pModelCom, 25, 15.0, 20.0);
	m_pGoat->SetMotionBlurTime(m_pModelCom, 25, 15.0, 20.0);

	if (m_pModelCom->Get_CurrAnimIndex() == 25)
		m_pTransformCom->Set_LookY_Zero(TimeDelta * 0.5);

	if (m_pModelCom->Get_TargetTimeAcc(25) >= 15.0 && m_bEffectEnd[0] == false)
	{
		CEffect_Manager::GetInstance()->Create_GoatFlySwing(m_pTransformCom);
		CGameInstance::GetInstance()->LetsPlaySound_Monster(GetRandomSwingSound(), 0.7f, true, m_pTransformCom->Get_Position());

		m_bEffectEnd[0] = true;
	}

	return m_pModelCom->Check_CurAnim_And_Finished(m_FlySwing.back());
}

_bool CState_GoatAtk::Atk_TeleportDashSwing(_double TimeDelta)
{
	// �����ݶ��̴� ó��
	if (m_pModelCom->Get_CurrAnimIndex() == 27)
		m_pGoat->SetWeaponAttackTime(m_pModelCom, 27, 0.0, 14.0);
	if (m_pModelCom->Get_CurrAnimIndex() == 28)
	{
		m_pGoat->SetWeaponAttackTime(m_pModelCom, 28, 0.0, 10.0);
		m_pGoat->SetMotionBlurTime(m_pModelCom, 28, 0.0, 10.0);
	}

	// 28�� �ִϸ��̼�(����)�� �ƴ� ���� �÷��̾������� ȸ��
	Turn_ExceptAnim(28, TimeDelta);

	// ���� �غ����� ������ 27��(�����Ϸ� ���ư���) �ִϸ��̼����� ����
	if (m_pModelCom->Check_CurAnim_And_Finished(26))
	{
		// �̹� ��Ÿ� �����̸� �ٷ� ����
		if (m_pTransformCom->Get_Distance(m_pPlayerTransform) <= 3.f)
			m_pGoat->Set_AnimAndReset(m_pModelCom, 28);
		else
		{
			m_pModelCom->Set_TempIndex(27);
			// �ٶ������� ����
			m_iCurrentSoundChannel = CGameInstance::GetInstance()->PlayMonsterRoofSound(TEXT("Goat_45_Wind.wav"), 0.7f, true, m_pTransformCom->Get_Position());
		}
	}

	// 27�� �ִϸ��̼��� �� ������ ���ư���
	if (m_pModelCom->Get_CurrAnimIndex() == 27)
	{
		if (GoStraight_Limit(3.f, 3.0, TimeDelta))
			m_pGoat->Set_AnimAndReset(m_pModelCom, 28);
	}

	if (m_pModelCom->Get_CurrAnimIndex() == 28)
		m_pTransformCom->Set_LookY_Zero(TimeDelta * 0.5);

	if (m_pModelCom->Get_TargetTimeAcc(28) >= 4.0 && m_bEffectEnd[0] == false)
	{
		CEffect_Manager::GetInstance()->Create_GoatDashSwing(m_pTransformCom);
		// �ٶ������� ���� �����ϰ� ���� ����
		CGameInstance::GetInstance()->StopMonsterSound(m_iCurrentSoundChannel);
		CGameInstance::GetInstance()->LetsPlaySound_Monster(GetRandomSwingSound(), 0.7f, true, m_pTransformCom->Get_Position());

		m_bEffectEnd[0] = true;
	}

	return m_pModelCom->Check_CurAnim_And_Finished(m_DashSwing.back());
}

_bool CState_GoatAtk::Atk_TeleportSpinDashSwing(_double TimeDelta)
{
	// �����ݶ��̴� ó��
	if (m_pModelCom->Get_CurrAnimIndex() == 30)
		m_pGoat->SetWeaponAttackTime(m_pModelCom, 30, 0.0, 9.0);
	if (m_pModelCom->Get_CurrAnimIndex() == 31)
	{
		m_pGoat->SetWeaponAttackTime(m_pModelCom, 31, 0.0, 20.0);
		m_pGoat->SetMotionBlurTime(m_pModelCom, 31, 0.0, 20.0);
	}

	m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);
	m_pTransformCom->Turn_To_Direction_AxisX(m_pPlayerTransform, TimeDelta);

	// ���� �غ����� ������ 30��(����) �ִϸ��̼����� ����
	if (m_pModelCom->Check_CurAnim_And_Finished(29))
	{
		// �̹� ��Ÿ� �����̸� �ٷ� ����
		if (m_pTransformCom->Get_Distance(m_pPlayerTransform) <= 0.5f)
			m_pGoat->Set_AnimAndReset(m_pModelCom, 31);
		else
		{
			m_pModelCom->Set_TempIndex(30);
			m_EffectObjects = CEffect_Manager::GetInstance()->Create_GoatSpinDashSwing(m_pTransformCom);
			m_iCurrentSoundChannel = CGameInstance::GetInstance()->PlayMonsterRoofSound(TEXT("Goat_43_Spin.wav"), 0.7f, true, m_pTransformCom->Get_Position());
		}
	}

	// 30�� �ִϸ��̼��� �� ������ ���ư���
	if (m_pModelCom->Get_CurrAnimIndex() == 30)
	{
		if (GoStraight_Limit(0.5f, 4.0, TimeDelta * 0.65))
		{
			m_pGoat->Set_AnimAndReset(m_pModelCom, 31);
			CGameInstance::GetInstance()->StopMonsterSound(m_iCurrentSoundChannel);
		}
	}

	if (m_pModelCom->Get_CurrAnimIndex() == 31)
		m_pTransformCom->Set_LookY_Zero(TimeDelta * 0.5);

	if (m_pModelCom->Get_TargetTimeAcc(31) >= 10.0 && m_bEffectEnd[0] == false)
	{
		CEffect_Manager::GetInstance()->Create_SpinDashSwingFinish(m_pTransformCom);
		CGameInstance::GetInstance()->LetsPlaySound_Monster(GetRandomSwingSound(), 0.7f, true, m_pTransformCom->Get_Position());

		m_bEffectEnd[0] = true;
	}

	// ����Ʈ ����
	if (m_pModelCom->Get_TargetTimeAcc(31) >= 1.0 && m_EffectObjects.size() != 0)
	{
		for (auto& pEffect : m_EffectObjects)
		{
			pEffect->Set_Dead(true);
		}
		m_EffectObjects.clear();
	}

	return m_pModelCom->Check_CurAnim_And_Finished(m_SpinDashSwing.back());
}

_bool CState_GoatAtk::Atk_TeleportUnderSwing(_double TimeDelta)
{
	// �����ݶ��̴� ó��
	m_pGoat->SetWeaponAttackTime(m_pModelCom, 35, 1.0, 20.0);
	m_pGoat->SetMotionBlurTime(m_pModelCom, 35, 1.0, 20.0);

	if (m_pModelCom->Get_TargetTimeAcc(35) >= 10.0 && m_bEffectEnd[0] == false)
	{
		CEffect_Manager::GetInstance()->Create_GoatUnderSwing(m_pTransformCom);
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Goat_34_UnderAtk.wav"), 0.7f, true, m_pTransformCom->Get_Position());
		m_bEffectEnd[0] = true;
	}

	return m_pModelCom->Check_CurAnim_And_Finished(m_UnderSwing.back());
}

_bool CState_GoatAtk::Atk_TeleportLongUnderAtk(_double TimeDelta)
{
	// �������� �� �ö� ������ �÷��̾� �Ӹ� ���� �ڷ���Ʈ
	if (m_pModelCom->Get_TargetTimeAcc(37) >= 60.0 && m_bSkillEnd[0] == false)
	{
		// �÷��̾� ���� �ڷ���Ʈ
		_vector vPlayerPos = m_pPlayerTransform->Get_Position() + __vector(0.f, 15.f, 0.f, 0.f);
		m_pTransformCom->Set_Position(vPlayerPos);

		m_bSkillEnd[0] = true;
	}

	return m_pModelCom->Check_CurAnim_And_Finished(m_LongUnderAtk.back());
}

_bool CState_GoatAtk::Turn_ExceptAnim(_uint iExceptAnim, _double TimeDelta)
{
	_bool bTurnAxisY = false;
	_bool bTurnAxisX = false;

	if (m_pModelCom->Get_CurrAnimIndex() != iExceptAnim)
	{
		bTurnAxisY = m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);
		bTurnAxisX = m_pTransformCom->Turn_To_Direction_AxisX(m_pPlayerTransform, TimeDelta);
	}

	return bTurnAxisY && bTurnAxisX;
}

_bool CState_GoatAtk::GoStraight_Limit(_float fDistanceLimit, _double TimeLimit, _double TimeDelta)
{
	m_pTransformCom->Go_Straight(TimeDelta, m_pMonsterNavigation);

	if (m_pTransformCom->Get_Distance(m_pPlayerTransform) <= fDistanceLimit || m_TimeAcc >= TimeLimit)
		return true;

	return false;
}

CState_GoatAtk* CState_GoatAtk::Create(void* pArg)
{
	CState_GoatAtk* pInstance = new CState_GoatAtk;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

void CState_GoatAtk::Free()
{
	__super::Free();
}