#include "stdafx.h"
#include "State_BigTreeAtk.h"
#include "GameInstance.h"
#include "Monster.h"
#include "MonsterCooler.h"
#include "MonsterSkill.h"
#include "Crow.h"
#include "Effect_Manager.h"
#include "Terrain.h"
#include "GameManager.h"
#include "Camera_Perspective.h"
#include "Renderer.h"

CState_BigTreeAtk::CState_BigTreeAtk()
{
}

HRESULT CState_BigTreeAtk::Initialize(void* pArg)
{ 
	if (pArg == nullptr)
	{
		BREAKPOINT;
	}

	// Bigtree �� ����� ������ ��������
	memcpy(&m_eSkill, pArg, sizeof(CBigTree::BIGTREE_SKILL));

	return S_OK; 
}

HRESULT CState_BigTreeAtk::EnterState()
{
	// BigTree ����
	m_pBigTree           = dynamic_cast<CBigTree*>(m_pGameObject);
	m_pMonsterNavigation = static_cast<CNavigation*>(m_pGameObject->Get_Component(TEXT("Com_Navigation")));
	
	if (m_pMonsterNavigation == nullptr)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// ���� ������ �ͷ��� ����
	m_pCurTerrain = static_cast<CTerrain*>(pGameInstance->Get_Front(LEVEL_RAPUNZEL_BOSS, TEXT("Layer_Terrain")));

	// ���� ������ �ִ� Player �� Transform ������Ʈ�� ����
	m_pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_LevelIndex(), TEXT("Layer_Player"), TEXT("Com_Transform"), 0));

	// �ִϸ��̼� ����
	Set_Animation();

	return S_OK;
}

_bool CState_BigTreeAtk::Action(_double TimeDelta)
{
	_bool bStateEnd = BigTree_Atk(TimeDelta);

	// CState_BigTreeAtk ���� ���� �� 
	if (bStateEnd == true)
	{
		// ���� ���� ����
		m_pGameObject->Set_IsAttacking(false);
	}

	if (bStateEnd == true)
		return DELETE_STATE;

	return KEEP_STATE;
}

void CState_BigTreeAtk::Set_Animation()
{
	// ����� ��ų�� ���� �ִϸ��̼� ����
	if (m_eSkill == CBigTree::BTSKILL_SUMMONCROW)
	{
		m_iAnimIndex = 2; // ��� ��ȯ
	}
	else if (m_eSkill == CBigTree::BTSKILL_GROUNDHEADBUTT)
	{
		m_iAnimIndex = 8; // ���ڸ� ��ġ��
	}
	else if (m_eSkill == CBigTree::BTSKILL_HUG)
	{
		m_iAnimIndex = 2; // �޷��ͼ� ��ų ���°Ŷ� 2������
	}
	else if (m_eSkill == CBigTree::BTSKILL_POWERHEADBUTT)
	{
		m_iAnimIndex = 2; // �޷��ͼ� ��ų ���°Ŷ� 2������
	}
	else if (m_eSkill == CBigTree::BTSKILL_BUILDINGCLIMB)
	{
		m_pTransformCom->Set_Position(__vector(129.f, 42.f, 170.f, 1.f));
		m_pTransformCom->Rotation(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
		static_cast<CNavigation*>(m_pGameObject->Get_Component(TEXT("Com_Navigation")))->Find_Cell(m_pTransformCom->Get_Position());

		m_pPlayerTransform->Set_Position(__vector(130.f, 0.1f, 100.f, 1.f));
		static_cast<CNavigation*>(CGameInstance::GetInstance()->Get_Component(CGameInstance::GetInstance()->Get_LevelIndex(), TEXT("Layer_Player"), TEXT("Com_Navigation"), 0))->Find_Cell(m_pPlayerTransform->Get_Position());

		m_iAnimIndex = 14; // ȭ����ȯ �� ���� �տ��� �ִϸ��̼� ���

		GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_BIGTREE_CLIMB);

		// ��Ʈ���� ���� �� �ְ� �Ȱ� ������� �Ⱦ��
		CRenderer* pRenderer = static_cast<CRenderer*>(m_pBigTree->Get_Component(TEXT("Com_Renderer")));
		pRenderer->Set_FogStart(60.f);
		pRenderer->Set_FogEnd(160.f);
	}
	else if (m_eSkill == CBigTree::BTSKILL_SUMMONFRUITS)
	{
		m_iAnimIndex = 17; // ���� ������ ������ ��
	}
	else if (m_eSkill == CBigTree::BTSKILL_SCREWDASH)
	{
		m_iAnimIndex = 2; // �޷��ͼ� ��ų ���°Ŷ� 2������
	}

	m_pBigTree->Set_AnimAndReset(m_pModelCom, m_iAnimIndex);
}

_bool CState_BigTreeAtk::BigTree_Atk(_double TimeDelta)
{
	// ����� ��ų�� ���� Atk Action ����
	if (m_eSkill == CBigTree::BTSKILL_SUMMONCROW)
	{
		// 2 ��(RunLoop) �ִϸ��̼��� ��
		if (m_pModelCom->Get_CurrAnimIndex() == 2)
		{
			// �����Ÿ����� �����ϰ� 7 �� �ִϸ��̼����� ����
			if (m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta))
				m_pBigTree->Set_AnimAndReset(m_pModelCom, 7);
		}

		// �ͷ��� �¿��
		m_pTransformCom->Set_Position(m_pCurTerrain->Compute_PositionOnTerrain(m_pTransformCom->Get_Position())); 

		if (m_pModelCom->Get_TargetTimeAcc(7) >= 20.0 && m_bEffectEnd[0] == false)
		{
			CEffect_Manager::GetInstance()->Create_BigTree_LandingWind(m_pTransformCom);
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("BigTree_SummonWings.wav"), 0.7f);
			m_bEffectEnd[0] = true;
		}

		if (m_pModelCom->Get_TargetTimeAcc(7) >= 60.0 && m_bEffectEnd[1] == false)
		{
			CEffect_Manager::GetInstance()->Create_BigTree_SummonWings(m_pTransformCom);
			m_bEffectEnd[1] = true;
		}

		if (m_pModelCom->Get_TargetTimeAcc(7) >= 70.0 && m_bSoundEnd[0] == false)
		{
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("BigTree_Roar.wav"), 0.7f);
			m_bSoundEnd[0] = true;
		}

		if (m_pModelCom->Get_TargetTimeAcc(7) >= 100.0 && m_bSoundEnd[1] == false)
		{
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("BigTree_SummonCrow.wav"), 0.8f);
			m_bSoundEnd[1] = true;
		}

		// ��� ��ȯ
		if (m_pModelCom->Get_TargetTimeAcc(7) >= 60.0 && m_pModelCom->Get_TargetTimeAcc(7) <= 140.0)
			SummonCrow(0.15, TimeDelta);

		if (m_pModelCom->CheckFinished_Input(7) == true)
			return true;
	}
	else if (m_eSkill == CBigTree::BTSKILL_GROUNDHEADBUTT)
	{
		// �ͷ��� �¿��
		m_pTransformCom->Set_Position(m_pCurTerrain->Compute_PositionOnTerrain(m_pTransformCom->Get_Position()));

		// ���� ���� �ѱ�
		if (m_pModelCom->Get_TargetTimeAcc(m_iAnimIndex) >= 100.0)
			m_pGameObject->Set_IsAttacking(true);

		// ����Ʈ
		if (m_pModelCom->Get_TargetTimeAcc(m_iAnimIndex) >= 110.0 && m_bEffectEnd[0] == false)
		{
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("BigTree_AtkGroundHeadButt.wav"), 0.6f, true, m_pTransformCom->Get_Position());
			CEffect_Manager::GetInstance()->Create_BigTreeSmoke(m_pBigTree->Get_EffectBonePos()[0], 80, -5.f, 5.f, -5.f, 5.f, -5.f, 5.f, 2.f);
			m_bEffectEnd[0] = true;
		}

		// ���� ���� ����
		if (m_pModelCom->Get_TargetTimeAcc(m_iAnimIndex) >= 115.0)
		{
			m_pGameObject->Set_IsAttacking(false);
		}

		// 8 �� �ִϸ��̼��� ������ true ����
		return m_pModelCom->CheckFinished_Input(8);
	}
	else if (m_eSkill == CBigTree::BTSKILL_HUG)
	{
		// �ͷ��� �¿��
		m_pTransformCom->Set_Position(m_pCurTerrain->Compute_PositionOnTerrain(m_pTransformCom->Get_Position()));

		// 2 ��(RunLoop) �ִϸ��̼��� ��
		if (m_pModelCom->Get_CurrAnimIndex() == 2)
		{
			// �����Ÿ����� �����ϰ� 9 ��(���ȱ�) �ִϸ��̼����� ����
			_bool bGoToTarget = m_pTransformCom->Go_To_TargetXZ(m_pPlayerTransform, 30.f, TimeDelta, m_pMonsterNavigation);
			_bool bDoTurn     = m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);

			if (bGoToTarget && bDoTurn)
				m_pBigTree->Set_AnimAndReset(m_pModelCom, 9);
		}

		if (m_pModelCom->Get_CurrAnimIndex() == 9 && m_pModelCom->Get_TargetTimeAcc(9) <= 20.0)
		{
			m_pTransformCom->Go_Straight(TimeDelta, m_pMonsterNavigation);
		}

		if (m_pModelCom->Get_TargetTimeAcc(9) >= 32.0)
			m_pGameObject->Set_IsAttacking(true);

		if (m_pModelCom->Get_TargetTimeAcc(9) >= 32.0 && m_bEffectEnd[0] == false)
		{
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("BigTree_AtkHug.wav"), 0.6f, true, m_pTransformCom->Get_Position());
			CEffect_Manager::GetInstance()->Create_BigTree_Slash(m_pTransformCom, __vector(0.f, 5.f, 0.f, 0.f));
			m_bEffectEnd[0] = true;
		}

		if (m_pModelCom->Get_TargetTimeAcc(9) >= 40.0)
			m_pGameObject->Set_IsAttacking(false);

		// 9 �� �ִϸ��̼��� ������ true ����
		return m_pModelCom->CheckFinished_Input(9);
	}
	else if (m_eSkill == CBigTree::BTSKILL_POWERHEADBUTT)
	{
		// �ͷ��� �¿��
		m_pTransformCom->Set_Position(m_pCurTerrain->Compute_PositionOnTerrain(m_pTransformCom->Get_Position()));

		// 2 ��(RunLoop) �ִϸ��̼��� ��
		if (m_pModelCom->Get_CurrAnimIndex() == 2)
		{
			// �����Ÿ����� �����ϰ� 10 ��(�� ¤��) �ִϸ��̼��� �ƴϸ� �ִϸ��̼� ����
			if (Chase(15.f, TimeDelta) && m_pModelCom->Get_CurrAnimIndex() != 10)
			{
				m_pBigTree->Set_AnimAndReset(m_pModelCom, 10);
			}
		}

		// ���� �ִϸ��̼��� 10 ���̰�, �����ٸ� 11 ������ ����
		if (m_pModelCom->Get_CurrAnimIndex() == 10 && m_pModelCom->Get_TargetTimeAcc(10) >= 20.0)
		{
			m_pBigTree->Set_AnimAndReset(m_pModelCom, 11);
			CEffect_Manager::GetInstance()->Create_BigTreeSmoke(m_pTransformCom, 80, -5.f, 5.f, -5.f, 5.f, -5.f, 5.f, 2.f);
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("BigTree_AtkPowerHeadButt.wav"), 0.6f, true, m_pTransformCom->Get_Position());
		}

		if (m_pModelCom->Get_CurrAnimIndex() == 11)
		{
			if (m_pModelCom->Get_TargetTimeAcc(11) >= 35.0 && m_bEffectEnd[0] == false)
			{
				CEffect_Manager::GetInstance()->Create_SoaringWind(&m_pBigTree->Get_EffectBoneWorldMatrix()[0]);
				CEffect_Manager::GetInstance()->Create_SoaringWind(&m_pBigTree->Get_EffectBoneWorldMatrix()[1]);
				CEffect_Manager::GetInstance()->Create_SoaringWind(&m_pBigTree->Get_EffectBoneWorldMatrix()[2]);
				m_bEffectEnd[0] = true;
			}

			if (m_pModelCom->Get_TargetTimeAcc(11) >= 90.0 && m_bEffectEnd[1] == false)
			{
				CEffect_Manager::GetInstance()->Create_BigTreeFall(m_pTransformCom, &(m_pBigTree->Get_EffectBoneWorldMatrix()[0]), 1.25f);
				m_bEffectEnd[1] = true;
			}

			if (m_pModelCom->Get_TargetTimeAcc(11) >= 120.0 && m_bEffectEnd[2] == false)
			{
				CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("BigTree_Fall.wav"), 0.6f, true, m_pTransformCom->Get_Position());
				CEffect_Manager::GetInstance()->Create_BigTreeSmoke((m_pBigTree->Get_EffectBonePos())[0], 150, -10.f, 10.f, -10.f, 10.f, -10.f, 10.f, 1.f);
				m_bEffectEnd[2] = true;
			}
		}

		if (m_pModelCom->Get_TargetTimeAcc(11) >= 110.0)
			m_pGameObject->Set_IsAttacking(true);
		if (m_pModelCom->Get_TargetTimeAcc(11) >= 122.0)
			m_pGameObject->Set_IsAttacking(false);

		// 11 �� �ִϸ��̼��� ������ true ����
		return m_pModelCom->CheckFinished_Input(11);
	}
	else if (m_eSkill == CBigTree::BTSKILL_BUILDINGCLIMB)
	{
		// 14 ~ 16 : ���� ������ / 17 : �������� �ϴ÷� �� �ø��� ����
		if (m_pModelCom->Check_CurAnim_And_Finished(14))
			m_pBigTree->Set_AnimAndReset(m_pModelCom, 15);

		if (m_pModelCom->Check_CurAnim_And_Finished(15))
			m_pBigTree->Set_AnimAndReset(m_pModelCom, 16);

		if (m_pModelCom->Get_CurrAnimIndex() == 16 && m_pModelCom->Get_TargetTimeAcc(16) >= 50.0)
		{
			if (m_bEffectEnd[0] == false)
			{
				CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("BigTree_AtkSummonMoon.wav"), 0.6f, true, m_pTransformCom->Get_Position());
				CEffect_Manager::GetInstance()->Create_BigTreeSummonApple(m_pTransformCom);
				CEffect_Manager::GetInstance()->Create_BigTreeSummonApple_Ring(m_pTransformCom);
				CEffect_Manager::GetInstance()->Create_BigTreeSummonAppleParticle(m_pTransformCom);
				m_bEffectEnd[0] = true;
			}

			if (m_pModelCom->Get_CurrAnimIndex() == 16 && m_pModelCom->Get_TargetTimeAcc(16) >= 50.0)
			{
				m_EffectTimeAcc += TimeDelta;

				if (m_EffectTimeAcc >= 0.5)
				{
					CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("BigTree_AtkSummonMoon.wav"), 0.1f, true, m_pTransformCom->Get_Position());
					CEffect_Manager::GetInstance()->Create_BigTreeSummonApple_Ring(m_pTransformCom);
					CEffect_Manager::GetInstance()->Create_BigTreeSummonAppleParticle(m_pTransformCom);
					m_EffectTimeAcc = 0.0;
				}
			}

			SummonFruits(0.3, TimeDelta);
		}

		if (m_pModelCom->Check_CurAnim_And_Finished(16)) 
			m_pBigTree->Set_AnimAndReset(m_pModelCom, 17);

		if (m_pModelCom->Get_CurrAnimIndex() == 17)
		{
			m_EffectTimeAcc += TimeDelta;

			if (m_EffectTimeAcc >= 0.5)
			{
				CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("BigTree_AtkSummonMoon.wav"), 0.1f, true, m_pTransformCom->Get_Position());
				CEffect_Manager::GetInstance()->Create_BigTreeSummonApple_Ring(m_pTransformCom);
				CEffect_Manager::GetInstance()->Create_BigTreeSummonAppleParticle(m_pTransformCom);
				m_EffectTimeAcc = 0.0;
			}
		}

		if (m_pModelCom->Get_CurrAnimIndex() == 17 && m_pModelCom->Get_TargetTimeAcc(17) >= 90.0)
		{
			SummonFruits(0.3, TimeDelta);
		}

		if (m_pModelCom->Check_CurAnim_And_Finished(17))
		{
			m_pBigTree->ResetAtkCool();
			return true;
		}
	}
	else if (m_eSkill == CBigTree::BTSKILL_SUMMONFRUITS)
	{
		if (m_pModelCom->Check_ProgressRate(0.4f))
		{
			SummonFruits(0.3, TimeDelta);
		} 

		if (m_pModelCom->Get_CurrAnimIndex() == 17 && m_pModelCom->Get_TargetTimeAcc(17) <= 120.0)
		{
			m_EffectTimeAcc += TimeDelta;

			if (m_EffectTimeAcc >= 0.5)
			{
				CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("BigTree_AtkSummonMoon.wav"), 0.1f, true, m_pTransformCom->Get_Position());
				CEffect_Manager::GetInstance()->Create_BigTreeSummonApple_Ring(m_pTransformCom);
				m_EffectTimeAcc = 0.0;
			}
		}

		if (m_pModelCom->Get_CurrAnimIndex() == 17)
		{
			m_ParticleTimeAcc += TimeDelta;

			if (m_ParticleTimeAcc >= 0.5)
			{
				CEffect_Manager::GetInstance()->Create_BigTreeSummonAppleParticle(m_pTransformCom);
				m_ParticleTimeAcc = 0.0;
			}
		}

		if (m_pModelCom->Get_CurrFinished())
			++m_iAnimFinishedAcc;

		// �ִϸ��̼� 1 �� �����ϰ� ���� �ִϸ��̼��� 23 ��(�������� �ִϸ��̼�) �� �ƴ϶�� ����
		if (m_iAnimFinishedAcc == 1 && m_pModelCom->Get_TempIndex() != 23)
			m_pBigTree->Set_AnimAndReset(m_pModelCom, 23); 

		if (m_pModelCom->Get_TargetTimeAcc(23) >= 15.0 && m_pModelCom->Get_TargetTimeAcc(23) < 35.0)
			m_pTransformCom->Go_Left(TimeDelta);

		if (m_pModelCom->Get_CurrAnimIndex() == 23 && m_pTransformCom->SetHeight(TimeDelta * 1.5f, vecy(m_pCurTerrain->Compute_PositionOnTerrain(m_pTransformCom->Get_Position()))))
		{
			static_cast<CNavigation*>(m_pGameObject->Get_Component(TEXT("Com_Navigation")))->Set_Ignore(true);
			
			if (m_pModelCom->Get_TargetTimeAcc(23) >= 30.0 && m_bEffectEnd[0] == false)
			{
				for (_uint i = 1; i < 4; ++i)
					CEffect_Manager::GetInstance()->Create_BigTreeSmoke((m_pBigTree->Get_EffectBonePos())[i], 80, -5.f, 5.f, -5.f, 5.f, -5.f, 5.f, 2.f);

				CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("BigTree_Fall.wav"), 0.6f, true, m_pTransformCom->Get_Position());
				m_bEffectEnd[0] = true;
			}

			// �ִϸ��̼� ������ ü�� 30% �� ���ܵΰ� true ����
			if (m_pModelCom->Get_CurrFinished())
			{
				CStatus* pBigTreeStatus = static_cast<CStatus*>(m_pGameObject->Get_Component(TEXT("Com_Status")));
				pBigTreeStatus->Set_HP(pBigTreeStatus->Get_MaxHP() * 0.3f);
				return true;
			}
		}
	}
	else if (m_eSkill == CBigTree::BTSKILL_SCREWDASH)
	{
		// �ͷ��� �¿��
		m_pTransformCom->Set_Position(m_pCurTerrain->Compute_PositionOnTerrain(m_pTransformCom->Get_Position()));

		// 2 ��(RunLoop) �ִϸ��̼��� ��
		if (m_pModelCom->Get_CurrAnimIndex() == 2)
		{
			// �����Ÿ����� �����ϰ� 18 ��(��ũ�����) �ִϸ��̼����� ����
			if (m_pTransformCom->Go_To_TargetXZ(m_pPlayerTransform, 20.f, TimeDelta, m_pMonsterNavigation) == false)
				m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);
			else
			{
				m_pBigTree->Set_AnimAndReset(m_pModelCom, 18);
				CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("BigTree_AtkScrew.wav"), 0.6f, true, m_pTransformCom->Get_Position());
			}
		}

		// ���� ���� �ѱ�
		if (m_pModelCom->Get_TargetTimeAcc(18) >= 50.0)
		{
			m_pGameObject->Set_IsAttacking(true);
		}

		if (m_pModelCom->Check_CurAnim_And_Finished(18))
			m_pBigTree->Set_AnimAndReset(m_pModelCom, 19);

		// 19 ��(����) �ִϸ��̼��� �� ������ �̵� 
		if (m_pModelCom->Get_CurrAnimIndex() == 19)
		{
			m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta * 2.0);

			m_pTransformCom->SetHeightZero(TimeDelta, m_pMonsterNavigation);
			m_pTransformCom->Go_Straight(TimeDelta, m_pMonsterNavigation);
		}

		if (m_pModelCom->Check_CurAnim_And_Finished(19))
			m_pBigTree->Set_AnimAndReset(m_pModelCom, 21);

		// ���� ���� ����
		if (m_pModelCom->Get_TargetTimeAcc(21) >= 15.0)
			m_pGameObject->Set_IsAttacking(false);

		// �ִϸ��̼��� ������ true ����
		if (m_pModelCom->Get_CurrAnimIndex() == 21 && m_pModelCom->Check_ProgressRate(0.7f))
		{
			return true;
		}
	}

	return false;
}

void CState_BigTreeAtk::SummonCrow(_double TimeInterval, _double TimeDelta)
{
	m_TimeAcc += TimeDelta;

	if (m_TimeAcc >= TimeInterval)
	{
		_vector vPos = XMVectorSetY(m_pTransformCom->Get_Position(), 10.5f);
		vPos        += XMVector4Normalize(m_pTransformCom->Get_Look()) * 2.f;

		_vector vDir = XMVector4Normalize(m_pTransformCom->Get_Look()) * 5.f;
		vDir        += XMVectorSet((_float)(rand() % 11) - 5.f, (_float)(rand() % 11) - 5.f, (_float)(rand() % 11) - 5.f, 0.f);

		CCrow::CROWDESC CrowDesc;
		CrowDesc.vPos       = vPos;
		CrowDesc.vRandomDir = XMVector4Normalize(vDir);

		CGameInstance::GetInstance()->Add_GameObject(CGameInstance::GetInstance()->Get_LevelIndex(), TEXT("Layer_Crow"), TEXT("Prototype_GameObject_Crow"), &CrowDesc);

		m_TimeAcc = 0.0;
	}
}

void CState_BigTreeAtk::SummonFruits(_double TimeInterval, _double TimeDelta)
{
	m_TimeAcc += TimeDelta;

	if (m_TimeAcc >= TimeInterval)
	{
		_vector vPos = m_pPlayerTransform->Get_Position();
		vPos        += XMVectorSet((_float)(rand() % 21 - 10), (_float)(rand() % 5 + 30), (_float)(rand() % 21 - 10), 0.f);

		CMonsterSkill::MONSTERSKILLDESC MonsterSkillDesc;
		MonsterSkillDesc.eSkillType = CMonsterSkill::MONSTERSKILLTYPE_BOUNCE;
		MonsterSkillDesc.vPos       = vPos;
		MonsterSkillDesc.vDir       = XMVector4Normalize(__vector(0.f, -1.f, 0.f, 0.f));
		MonsterSkillDesc.fSpeed     = (_float)(rand() % 5) + 20.f;
		MonsterSkillDesc.fDamage    = 5.f;
		MonsterSkillDesc.Duration   = 3.5f;
		MonsterSkillDesc.fSize      = 1.5f;
		MonsterSkillDesc.bNoModel   = true;

		CGameObject* pSkill = CGameInstance::GetInstance()->Add_GameObject_And_Return(CGameInstance::GetInstance()->Get_LevelIndex(), TEXT("Layer_MonsterSkill"), TEXT("Prototype_GameObject_MonsterSkill"), &MonsterSkillDesc);
		CEffect_Manager::GetInstance()->Create_FruitZone(vPos);

		m_TimeAcc = 0.0;
	}
}

_bool CState_BigTreeAtk::Chase(_float fDistDiff, _double TimeDelta)
{
	// �÷��̾� Transform �� nullptr �̸� ���� �ٷ� ����� ���� true ����
	if (m_pPlayerTransform == nullptr)
		return true;

	// �÷��̾������� ȸ��
	m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);

	// �÷��̾� ���� fDistDiff �Ÿ���ŭ �ٰ����� ��ǥ �Ÿ��� �����ϸ� true ����
	return m_pTransformCom->Go_To_TargetXZ(m_pPlayerTransform, fDistDiff, TimeDelta, m_pMonsterNavigation);
}

CState_BigTreeAtk* CState_BigTreeAtk::Create(void* pArg)
{
	CState_BigTreeAtk* pInstance = new CState_BigTreeAtk;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

void CState_BigTreeAtk::Free()
{
	__super::Free();
}