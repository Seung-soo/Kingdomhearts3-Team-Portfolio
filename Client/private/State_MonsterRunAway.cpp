#include "stdafx.h"
#include "State_MonsterRunAway.h"
#include "GameInstance.h"
#include "Monster.h"
#include "Effect_Manager.h"
#include "Icicle.h"
#include "Goat.h"

CState_MonsterRunAway::CState_MonsterRunAway()
{
}

HRESULT CState_MonsterRunAway::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CState_MonsterRunAway::EnterState()
{
	// ���� Ÿ�� ����
	m_eMonsterType = (MONSTERTYPE)dynamic_cast<CMonster*>(m_pGameObject)->Get_MonsterType();

	// ���� ����
	m_pMonster           = dynamic_cast<CMonster*>(m_pGameObject);
	m_pMonsterNavigation = static_cast<CNavigation*>(m_pGameObject->Get_Component(TEXT("Com_Navigation")));

	if (m_pMonsterNavigation == nullptr)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// ���� ������ �ִ� Player �� Transform ������Ʈ�� ����
	m_pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_LevelIndex(), TEXT("Layer_Player"), TEXT("Com_Transform"), 0));

	// �ִϸ��̼� ����
	switch (m_eMonsterType)
	{
	case MONSTERTYPE_SKULLSOLDIER:
		m_pMonster->Set_AnimAndReset(m_pModelCom, 6);
		break;
	case MONSTERTYPE_CROWSOLDIER:
		m_pMonster->Set_AnimAndReset(m_pModelCom, 5);
		break;
	case MONSTERTYPE_BOWMAN:
		m_pMonster->Set_AnimAndReset(m_pModelCom, 2);
		break;
	case MONSTERTYPE_NIGHTMARE:
		m_pMonster->Set_MotionTrail(true);
		m_pMonster->Set_AnimAndReset(m_pModelCom, 2);
		break;
	case MONSTERTYPE_NOBODY:
		m_pMonster->Set_AnimAndReset(m_pModelCom, 1);
		break;
	case MONSTERTYPE_THINSOLDIER:
		m_pMonster->Set_AnimAndReset(m_pModelCom, 3);
		break;
	case MONSTERTYPE_ICEMAN:
		m_pMonster->Set_AnimAndReset(m_pModelCom, 11);
		// �������� ���� �ʱ�ȭ
		CTransform::MONSTERJUMPDESC MonsterJumpDesc;
		MonsterJumpDesc.fOriginY    = vecy(m_pTransformCom->Get_Position());
		MonsterJumpDesc.fJumpSpeed  = 5.f;
		MonsterJumpDesc.fJumpPower  = 20.f;
		MonsterJumpDesc.JumpTimeAcc = 0.0;
		m_pTransformCom->SetMonsterJumpDesc(MonsterJumpDesc);
		break;
	case MONSTERTYPE_GOAT:
		m_pModelCom->Set_TempIndex(1);
		break;
	default:
		break;
	}

	return S_OK;
}

_bool CState_MonsterRunAway::Action(_double TimeDelta)
{
	_bool bStateEnd = false;

	// ���� Ÿ�Կ� ���� Action �б�
	switch (m_eMonsterType)
	{
	case MONSTERTYPE_SKULLSOLDIER:
		bStateEnd = TurnBackAndRunAway(2.0, TimeDelta);
		break;
	case MONSTERTYPE_CROWSOLDIER:
		bStateEnd = FlyAway(2.5f, 0.3, TimeDelta);
		break;
	case MONSTERTYPE_BOWMAN:
		bStateEnd = TurnBackAndRunAway(2.0, TimeDelta);
		break;
	case MONSTERTYPE_NIGHTMARE:
		m_EffectInterval += TimeDelta;
		if (m_EffectInterval >= 0.1)
		{
			CEffect_Manager::GetInstance()->Create_NightmareRunEffect(m_pTransformCom, &m_iEffectCount);
			m_EffectInterval = 0.0;
		}
		bStateEnd = TurnBackAndRunAway(0.5, TimeDelta);
		if (bStateEnd == true)
			m_pMonster->Set_MotionTrail(false);
		break;
	case MONSTERTYPE_NOBODY:
		bStateEnd = FlyAway(2.f, 0.3, TimeDelta); 
		break;
	case MONSTERTYPE_THINSOLDIER:
		bStateEnd = LookPlayerAndRunAway(3.f, TimeDelta);
		break;
	case MONSTERTYPE_ICEMAN:
		bStateEnd = IceManRunAway(TimeDelta);
		break;
	case MONSTERTYPE_GOAT:
		// �ڷ���Ʈ �Ϸ��� ��ġ��
		vector<_vector> vDir = { m_pTransformCom->Get_Up() + m_pTransformCom->Get_Right() + m_pTransformCom->Get_Look(),
								 m_pTransformCom->Get_Up() - m_pTransformCom->Get_Right() + m_pTransformCom->Get_Look(),
								 m_pTransformCom->Get_Up() - m_pTransformCom->Get_Look(),
								 m_pTransformCom->Get_Up() + m_pTransformCom->Get_Look(),
								 m_pTransformCom->Get_Up() };

		// ���� �ֺ����� �ڷ���Ʈ
		_uint iLoopAcc = 0;
		while (true)
		{
			// ���ѷ��� Ż��
			++iLoopAcc;

			// �÷��̾ �Ĵٺ��� �ϱ�
			m_pTransformCom->LookAt(m_pPlayerTransform);

			_uint iRandNum = rand() % (vDir.size() - 1);
			if (iLoopAcc == 10)
				iRandNum = (_uint)(vDir.size() - 1);
			if (m_pTransformCom->Teleport(vDir[iRandNum], 15.f, m_pMonsterNavigation) == true)
			{
				CEffect_Manager::GetInstance()->Create_GoatTeleport(m_pTransformCom);
				static_cast<CGoat*>(m_pMonster)->StartTeleportDissolve();
				bStateEnd = true;
				break;
			}
		}
		break;
	}

	if (bStateEnd == true)
		return DELETE_STATE;

	return KEEP_STATE;
}

_bool CState_MonsterRunAway::TurnBackAndRunAway(_double MaxTime, _double TimeDelta)
{
	// �ڵ��Ƽ� MaxTime ��ŭ ��������
	_vector vDir = m_pPlayerTransform->Get_Dir(m_pTransformCom);
	m_pTransformCom->Turn_To_Direction(vDir, TimeDelta);
	m_pTransformCom->Go_Straight(TimeDelta, m_pMonsterNavigation);
	
	m_TimeAcc += TimeDelta;

	// MaxTime �̻� ���������� true ����
	if (m_TimeAcc >= MaxTime)
		return true;

	return false;
}

_bool CState_MonsterRunAway::TurnBackAndRunAway(_float fDiff, _double TimeDelta)
{
	// �ڵ��Ƽ� fDiff ��ŭ ��������
	_vector vDir = m_pPlayerTransform->Get_Dir(m_pTransformCom);
	m_pTransformCom->Turn_To_Direction(vDir, TimeDelta);
	m_pTransformCom->Go_Straight(TimeDelta, m_pMonsterNavigation);

	// fDiff ��ŭ ���������� true ����
	if (m_pTransformCom->Get_Distance(m_pPlayerTransform) >= fDiff)
		return true;

	return false;
}

_bool CState_MonsterRunAway::LookPlayerAndRunAway(_double MaxTime, _double TimeDelta)
{
	// �÷��̾ ���� MaxTime ��ŭ ��������
	m_pPlayerTransform->Turn_To_Direction(m_pPlayerTransform, TimeDelta);
	m_pTransformCom->Go_Backward(TimeDelta, m_pMonsterNavigation);

	m_TimeAcc += TimeDelta;

	if (m_TimeAcc >= MaxTime)
		return true;

	return false;
}

_bool CState_MonsterRunAway::FlyAway(_float fHeight, _double Speed, _double TimeDelta)
{
	_vector vDir         = m_pTransformCom->Get_Dir(m_pPlayerTransform);
	_double SpeedPerTime = TimeDelta * Speed;
	m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta);
	m_pTransformCom->Go_Backward(SpeedPerTime, m_pMonsterNavigation);

	// ��ǥ ���̺��� ���ٸ� ���� �ö󰡱�
	_float fCurHeight    = vecy(m_pTransformCom->Get_Position());
	_float fTargetHeight = vecy(m_pPlayerTransform->Get_Position()) + fHeight;

	// �÷��̾�� ���� ����
	if (fTargetHeight > fCurHeight)
		m_pTransformCom->Go_Up(SpeedPerTime, m_pMonsterNavigation);

	// Ÿ�� ���̺��� ������ Ÿ�� ���̷� ����
	else if (fCurHeight >= fTargetHeight)
		m_pTransformCom->Set_Position(XMVectorSetY(m_pTransformCom->Get_Position(), fTargetHeight));

	m_TimeAcc += TimeDelta;

	if (m_TimeAcc >= 1.5)
		return true;

	return false;
}

_bool CState_MonsterRunAway::IceManRunAway(_double TimeDelta)
{
	// ����Ʈ
	if (m_pModelCom->Check_CurAnim_And_Finished(11))
	{
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("IceMan_Jump_17.wav"), 0.7f, true, m_pTransformCom->Get_Position());

		CEffect_Manager::GetInstance()->Create_IceManJump(m_pTransformCom); // ����
	}
	if (m_pModelCom->Check_CurAnim_And_Finished(14))
	{
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("IceMan_Landing_14.wav"), 0.7f, true, m_pTransformCom->Get_Position());

		CEffect_Manager::GetInstance()->Create_IceManJumpLanding(m_pTransformCom); // ����
	}

	// 12, 13, 14 �ִϸ��̼� ���� ���� �� ����
	if (m_pModelCom->Get_CurrAnimIndex() == 12 || m_pModelCom->Get_CurrAnimIndex() == 13 || m_pModelCom->Get_CurrAnimIndex() == 14)
	{
		// ����
		m_pTransformCom->MonsterJump(TimeDelta, 0.f);
		
		// �÷��̾� �Ĵٺ��鼭 �־�����
		m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta * 2.0, 0.05f);
		m_pTransformCom->Go_Vector(-m_pTransformCom->Get_Dir(m_pPlayerTransform), TimeDelta * 7.5, m_pMonsterNavigation);
	}

	// ���� �ִϸ��̼��� ������ ��帧 ����
	if (m_pModelCom->Check_CurAnim_And_Finished(14))
	{
		for (_uint i = 0; i < 3; ++i)
			CreateIcicle(TimeDelta);
	}

	// ������ �ִϸ��̼� �� ���̰� �׺񺸴� ������ �׺���̷� �����ֱ�
	if (m_pModelCom->Get_CurrAnimIndex() == 15)
		m_pTransformCom->SetHeightZero(TimeDelta * 5.0, m_pMonsterNavigation);

	// �ִϸ��̼��� ������ true �����ؼ� ���� ����ǰ� ��
	return m_pMonster->PlayAnimations(m_pModelCom, vector<_uint>{ 11, 12, 13, 14, 15 }, vector<_uint>{ 12, 14 });
}

_bool CState_MonsterRunAway::CreateIcicle(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// �÷��̾� ��ġ�� ������
	_vector vPos = m_pPlayerTransform->Get_Position();

	// �÷��̾� ��ġ �ֺ�����(XZ) ������ ��ġ�� �����
	vPos += __vector(_float(rand() % 10) + 5.f, 0.f, _float(rand() % 10) + 5.f, 0.f);
	vPos  = XMVectorSetY(vPos, 0.f);

	// �׺���̼� ��ġ�� ��������� �������� �ʰ� false ����
	if (m_pMonsterNavigation->Move_OnNavigation(vPos, vPos) == UNMOVABLE)
		return false;

	// �ֺ��� �̹� ��帧�� ������ �������� �ʰ� false ����
	list<CGameObject*>* Icicles = pGameInstance->Get_ObjectList(pGameInstance->Get_LevelIndex(), TEXT("Layer_Icicle"));

	if (Icicles != nullptr)
	{
		for (auto& pIcicle : *Icicles)
		{
			if (static_cast<CIcicle*>(pIcicle)->IsIcicleInRange(vPos, 3.5f) == true)
				return false;
		}
	}

	// ��帧 ����
	CGameObject* pIcicle = pGameInstance->Add_GameObject_And_Return(pGameInstance->Get_LevelIndex(), TEXT("Layer_Icicle"), TEXT("Prototype_GameObject_Icicle"), &vPos);

	return false;
}

CState_MonsterRunAway* CState_MonsterRunAway::Create(void* pArg)
{
	CState_MonsterRunAway* pInstance = new CState_MonsterRunAway;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

void CState_MonsterRunAway::Free()
{
	__super::Free();
}