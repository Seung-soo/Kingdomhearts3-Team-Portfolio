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
	// 몬스터 타입 설정
	m_eMonsterType = (MONSTERTYPE)dynamic_cast<CMonster*>(m_pGameObject)->Get_MonsterType();

	// 몬스터 설정
	m_pMonster           = dynamic_cast<CMonster*>(m_pGameObject);
	m_pMonsterNavigation = static_cast<CNavigation*>(m_pGameObject->Get_Component(TEXT("Com_Navigation")));

	if (m_pMonsterNavigation == nullptr)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// 현재 레벨에 있는 Player 의 Transform 컴포넌트를 얻어옴
	m_pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_LevelIndex(), TEXT("Layer_Player"), TEXT("Com_Transform"), 0));

	// 애니메이션 설정
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
		// 점프관련 변수 초기화
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

	// 몬스터 타입에 따라 Action 분기
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
		// 텔레포트 하려는 위치들
		vector<_vector> vDir = { m_pTransformCom->Get_Up() + m_pTransformCom->Get_Right() + m_pTransformCom->Get_Look(),
								 m_pTransformCom->Get_Up() - m_pTransformCom->Get_Right() + m_pTransformCom->Get_Look(),
								 m_pTransformCom->Get_Up() - m_pTransformCom->Get_Look(),
								 m_pTransformCom->Get_Up() + m_pTransformCom->Get_Look(),
								 m_pTransformCom->Get_Up() };

		// 본인 주변으로 텔레포트
		_uint iLoopAcc = 0;
		while (true)
		{
			// 무한루프 탈출
			++iLoopAcc;

			// 플레이어를 쳐다보게 하기
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
	// 뒤돌아서 MaxTime 만큼 도망가기
	_vector vDir = m_pPlayerTransform->Get_Dir(m_pTransformCom);
	m_pTransformCom->Turn_To_Direction(vDir, TimeDelta);
	m_pTransformCom->Go_Straight(TimeDelta, m_pMonsterNavigation);
	
	m_TimeAcc += TimeDelta;

	// MaxTime 이상 도망갔으면 true 리턴
	if (m_TimeAcc >= MaxTime)
		return true;

	return false;
}

_bool CState_MonsterRunAway::TurnBackAndRunAway(_float fDiff, _double TimeDelta)
{
	// 뒤돌아서 fDiff 만큼 도망가기
	_vector vDir = m_pPlayerTransform->Get_Dir(m_pTransformCom);
	m_pTransformCom->Turn_To_Direction(vDir, TimeDelta);
	m_pTransformCom->Go_Straight(TimeDelta, m_pMonsterNavigation);

	// fDiff 만큼 도망갔으면 true 리턴
	if (m_pTransformCom->Get_Distance(m_pPlayerTransform) >= fDiff)
		return true;

	return false;
}

_bool CState_MonsterRunAway::LookPlayerAndRunAway(_double MaxTime, _double TimeDelta)
{
	// 플레이어를 보고 MaxTime 만큼 도망가기
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

	// 목표 높이보다 낮다면 높게 올라가기
	_float fCurHeight    = vecy(m_pTransformCom->Get_Position());
	_float fTargetHeight = vecy(m_pPlayerTransform->Get_Position()) + fHeight;

	// 플레이어보다 높게 날기
	if (fTargetHeight > fCurHeight)
		m_pTransformCom->Go_Up(SpeedPerTime, m_pMonsterNavigation);

	// 타겟 높이보다 높으면 타겟 높이로 고정
	else if (fCurHeight >= fTargetHeight)
		m_pTransformCom->Set_Position(XMVectorSetY(m_pTransformCom->Get_Position(), fTargetHeight));

	m_TimeAcc += TimeDelta;

	if (m_TimeAcc >= 1.5)
		return true;

	return false;
}

_bool CState_MonsterRunAway::IceManRunAway(_double TimeDelta)
{
	// 이펙트
	if (m_pModelCom->Check_CurAnim_And_Finished(11))
	{
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("IceMan_Jump_17.wav"), 0.7f, true, m_pTransformCom->Get_Position());

		CEffect_Manager::GetInstance()->Create_IceManJump(m_pTransformCom); // 점프
	}
	if (m_pModelCom->Check_CurAnim_And_Finished(14))
	{
		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("IceMan_Landing_14.wav"), 0.7f, true, m_pTransformCom->Get_Position());

		CEffect_Manager::GetInstance()->Create_IceManJumpLanding(m_pTransformCom); // 착지
	}

	// 12, 13, 14 애니메이션 실행 중일 때 점프
	if (m_pModelCom->Get_CurrAnimIndex() == 12 || m_pModelCom->Get_CurrAnimIndex() == 13 || m_pModelCom->Get_CurrAnimIndex() == 14)
	{
		// 점프
		m_pTransformCom->MonsterJump(TimeDelta, 0.f);
		
		// 플레이어 쳐다보면서 멀어지기
		m_pTransformCom->Turn_To_Direction(m_pPlayerTransform, TimeDelta * 2.0, 0.05f);
		m_pTransformCom->Go_Vector(-m_pTransformCom->Get_Dir(m_pPlayerTransform), TimeDelta * 7.5, m_pMonsterNavigation);
	}

	// 착지 애니메이션이 끝나면 고드름 생성
	if (m_pModelCom->Check_CurAnim_And_Finished(14))
	{
		for (_uint i = 0; i < 3; ++i)
			CreateIcicle(TimeDelta);
	}

	// 마지막 애니메이션 때 높이가 네비보다 높으면 네비높이로 맞춰주기
	if (m_pModelCom->Get_CurrAnimIndex() == 15)
		m_pTransformCom->SetHeightZero(TimeDelta * 5.0, m_pMonsterNavigation);

	// 애니메이션이 끝나면 true 리턴해서 상태 종료되게 함
	return m_pMonster->PlayAnimations(m_pModelCom, vector<_uint>{ 11, 12, 13, 14, 15 }, vector<_uint>{ 12, 14 });
}

_bool CState_MonsterRunAway::CreateIcicle(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// 플레이어 위치를 가져옴
	_vector vPos = m_pPlayerTransform->Get_Position();

	// 플레이어 위치 주변으로(XZ) 랜덤한 위치를 잡아줌
	vPos += __vector(_float(rand() % 10) + 5.f, 0.f, _float(rand() % 10) + 5.f, 0.f);
	vPos  = XMVectorSetY(vPos, 0.f);

	// 네비게이션 위치를 벗어나있으면 생성하지 않고 false 리턴
	if (m_pMonsterNavigation->Move_OnNavigation(vPos, vPos) == UNMOVABLE)
		return false;

	// 주변에 이미 고드름이 있으면 생성하지 않고 false 리턴
	list<CGameObject*>* Icicles = pGameInstance->Get_ObjectList(pGameInstance->Get_LevelIndex(), TEXT("Layer_Icicle"));

	if (Icicles != nullptr)
	{
		for (auto& pIcicle : *Icicles)
		{
			if (static_cast<CIcicle*>(pIcicle)->IsIcicleInRange(vPos, 3.5f) == true)
				return false;
		}
	}

	// 고드름 생성
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