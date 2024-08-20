#include "stdafx.h"
#include "State_FrozenWolfAtk.h"
#include "GameInstance.h"
#include "Monster.h"
#include "MonsterCooler.h"
#include "MonsterSkill.h"
#include "Effect_Manager.h"
#include "GameManager.h"
#include "Camera_Perspective.h"

#include "LittleWolf.h"

CState_FrozenWolfAtk::CState_FrozenWolfAtk()
{
}

HRESULT CState_FrozenWolfAtk::Initialize(void* pArg)
{
    if (pArg == nullptr)
    {
        BREAKPOINT;
    }

  
    memcpy(&m_eFrozenWolfSkill, pArg, sizeof(CFrozenWolf::WOLF_SKILL));

	m_vDashPoint[0] = __vector(0.f, 0.f, 30.f, 1.f);
	m_vDashPoint[1] = __vector(21.f, 0.f, 21.f, 1.f);
	m_vDashPoint[2] = __vector(21.f, 0.f, -21.f, 1.f);
	m_vDashPoint[3] = __vector(0.f, 0.f, -30.f, 1.f);
	m_vDashPoint[4] = __vector(-21.f, 0.f, -21.f, 1.f);
	m_vDashPoint[5] = __vector(-30.f, 0.f, 0.f, 1.f);
	m_vDashPoint[6] = __vector(-21.f, 0.f, 21.f, 1.f);
	m_vDashPoint[7] = __vector(0.f, 0.f, 30.f, 1.f);

    return S_OK;
}

HRESULT CState_FrozenWolfAtk::EnterState()
{
    // FrozenWolf 설정
    m_pFrozenWolf = static_cast<CFrozenWolf*>(m_pGameObject);
    m_pMonsterNavigation = static_cast<CNavigation*>(m_pGameObject->Get_Component(TEXT("Com_Navigation")));

    if (m_pMonsterNavigation == nullptr)
    {
        BREAKPOINT;
        return E_FAIL;
    }

    // 플레이어 트랜스폼 설정
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    m_pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_LevelIndex(), TEXT("Layer_Player"), TEXT("Com_Transform"), 0));

	m_vPlayerFocusPos = m_pPlayerTransform->Get_Position();

	// 애니메이션 설정 및 초기값 설정
	switch (m_eFrozenWolfSkill)
	{
	case CFrozenWolf::WOLF_DASH:
		m_vTargetingPos = m_vPlayerFocusPos;
		m_pModelCom->Set_TargetPlaySpeed_Rate(m_Dash[1], 1.f);
		m_pModelCom->Set_TempIndex(8);
		m_pModelCom->Set_TargetPlaySpeed(8, 50.0);
		break;
	case CFrozenWolf::WOLF_SCRATCH:
		m_vPlayerFocusPos = XMVectorSetY(m_vPlayerFocusPos, 0.f);
        m_pFrozenWolf->Set_AnimAndReset(m_pModelCom, 8);
		m_pModelCom->Set_TargetPlaySpeed(8, 80.0);
		m_pModelCom->Set_TargetPlaySpeed(9, 50.0);
		break;
	case CFrozenWolf::WOLF_SUMMON:
		m_pModelCom->Set_TempIndex(8);
		m_pModelCom->Set_TargetPlaySpeed(8, 50.0);
		break;
	case CFrozenWolf::WOLF_SPINATTACK:
		m_vTargetingPos = m_pTransformCom->Get_Position() + __vector(0.f, 40.f, 0.f, 0.f);
		m_pModelCom->Set_TempIndex(8);
		m_pModelCom->Set_TargetPlaySpeed(8, 50.0);
		m_pModelCom->Set_TargetPlaySpeed_Rate(32, 1.0);
		m_pModelCom->Set_TargetPlaySpeed_Rate(35, 1.0);
		m_pModelCom->Set_TargetPlaySpeed_Rate(36, 1.0);
		break;
	case CFrozenWolf::WOLF_THROWFIRE:
		m_pModelCom->Set_TargetPlaySpeed(37, 50.0);
		m_pModelCom->Set_TempIndex(8);
		m_pModelCom->Set_TargetPlaySpeed(8, 50.0);
		break;
	case CFrozenWolf::WOLF_PURPLERAIN:
		m_pModelCom->Set_TempIndex(8);
		m_pModelCom->Set_TargetPlaySpeed(8, 50.0);
		m_vTargetingPos = m_pTransformCom->Get_Position() + __vector(0.f, 80.f, 0.f, 0.f);
		GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_FROZENWOLF_RAIN, m_pTransformCom);
		break;
	default:
		break;
	}

	return S_OK;

}

_bool CState_FrozenWolfAtk::Action(_double TimeDelta)
{
	_bool bStateEnd = false;

	m_TimeAcc += TimeDelta;
	m_EffectInterval += TimeDelta;

	// 애니메이션 설정 및 초기값 설정
	switch (m_eFrozenWolfSkill)
	{
	case CFrozenWolf::WOLF_DASH:
		bStateEnd = Atk_Dash(TimeDelta);
		break;
	case CFrozenWolf::WOLF_SCRATCH:
		bStateEnd = Atk_Scratch(TimeDelta);
		break;
	case CFrozenWolf::WOLF_SUMMON:
		bStateEnd = Atk_SummonWolf(TimeDelta);
		break;
	case CFrozenWolf::WOLF_SPINATTACK:
		bStateEnd = Atk_SpinAtk(TimeDelta);
		break;
	case CFrozenWolf::WOLF_THROWFIRE:
		bStateEnd = Atk_ThrowFire(TimeDelta);
		break;
	case CFrozenWolf::WOLF_PURPLERAIN:
		bStateEnd = Atk_PurpleRain(TimeDelta);
		break;
	default:
		break;
	}

	if (bStateEnd == true)
	{
		// 공격 상태 끄기
		m_pFrozenWolf->MoveToNextSkill();
		m_pGameObject->Set_IsAttacking(false);
		m_pFrozenWolf->Set_MotionTrail(false);

		return DELETE_STATE;
	}

	return KEEP_STATE;
}

_bool CState_FrozenWolfAtk::Atk_Dash(_double TimeDelta)
{
	_bool	 IsSkillFinished = false;
	_float	 fDistanceOffset = 0.5f;

	m_pFrozenWolf->SetAttackTime(m_pModelCom, m_Dash[2], 1.0, 21.0);
	m_pFrozenWolf->SetAttackTime(m_pModelCom, m_Dash[1], 1.0, 12.0);

	// 달리기 전 플레이어를 바라보게 설정
	if (m_pModelCom->Get_CurrAnimIndex() == 8)
	{
		auto isTurnFinished = m_pTransformCom->Turn_To_Direction(m_vPlayerFocusPos, TimeDelta * 5.0);

		if (true == isTurnFinished)
		{
			m_pFrozenWolf->Set_AnimAndReset(m_pModelCom, m_Dash[0]);
			m_vTargetingPos = m_pPlayerTransform->Get_Position();
			m_vTargetingPos = XMVectorSetY(m_vTargetingPos, 0.f);

			auto vDir = m_pTransformCom->Get_Dir(m_vTargetingPos);
			vDir = XMVectorSetY(vDir, 0.f);

			m_pTransformCom->MakeSameDirection(vDir);
		}

		else
			m_pTransformCom->Go_Straight(TimeDelta, m_pMonsterNavigation);
	}

	// 달리기 전 준비 동작이 끝나면 달리는 동작으로 이어지게 설정
	if (m_pModelCom->Check_CurAnim_And_Finished(m_Dash[0]) == true)
	{
		// Once
		m_pModelCom->Set_TempIndex(m_Dash[1]);
		m_pModelCom->Set_TargetPlaySpeed_Rate(m_Dash[1], 1.2f);
	}


	// 달리기
	if (m_pModelCom->Get_CurrAnimIndex() == m_Dash[1])
	{
		if (m_isReadyToDash == false)
		{
			// 플레이어 향해 돌진
			if(m_isHeadingPoint == false)
			{
				auto fDistance = m_pTransformCom->Get_Distance(m_vTargetingPos);

				// 플레이어한테 일단 돌진 후 포인트로 돌진
				if (fDistance > 2.f)
				{
					auto vDir = m_pTransformCom->Get_Dir(m_vTargetingPos);
					vDir = XMVectorSetY(vDir, 0.f);
					
					if (false == m_isTurning)
						m_isTurning = m_pTransformCom->Turn_To_Direction(vDir, TimeDelta * 5.0);
					
					m_pTransformCom->Go_Straight(TimeDelta * 5.0, m_pMonsterNavigation);
				}

				else
				{
					_float fMinDistance = 0;

					m_isTurning = false;

					// 제일 가까운 핀포인트 찾기
					for (_uint i = 0; i < 7; ++i)
					{
						auto fDistance = DISTANCE(m_vDashPoint[i] - m_pTransformCom->Get_Position());

						if (i == 0 || fDistance < fMinDistance)
						{
							fMinDistance = fDistance;
							m_iDashIndex = i;
						}
					}

					m_isHeadingPoint = true;
				}
			}

			else
			{
				auto vDir = m_vDashPoint[m_iDashIndex] - m_pTransformCom->Get_Position();
				auto fDistance = DISTANCE(vDir);
				vDir = XMVector3Normalize(vDir);

				auto isTurnFinished = m_pTransformCom->Turn_To_Direction(vDir, TimeDelta * 3.0);

				auto isClosed = m_pTransformCom->Go_To_Target(m_vDashPoint[m_iDashIndex], 1.f, TimeDelta * 10.0, m_pMonsterNavigation);

				if (isClosed == true)
				{
					m_isReadyToDash = true;
					m_TimeAcc = 0.0;
				}
			}
		}

		else
		{
			if (m_isReadyToBite == false)
			{
				// 물기 공격 전 플레이어 바라보게 턴
				if (true == m_bTurningBeforeAttack)
				{
					auto vDir = m_pTransformCom->Get_Dir(m_vTargetingPos);
					vDir = XMVectorSetY(vDir, 0.f);

					_bool isFinish = m_pTransformCom->Turn_To_Direction(vDir, TimeDelta * 5.0);

					if (isFinish)
					{
						m_isReadyToBite = true;
						m_pTransformCom->MakeSameDirection(vDir);
						CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("DarkWolf_Bite.wav"), 0.5f, true, m_pTransformCom->Get_Position());
					}

					else
						m_pTransformCom->Go_Straight(TimeDelta, m_pMonsterNavigation);

					m_TimeAcc = 0.0;
				}

				else
				{
					// 공전이 끝난 후 방향 설정
					if (m_TimeAcc >= 4.f)
					{
						m_bTurningBeforeAttack = true;
						m_vTargetingPos = m_pPlayerTransform->Get_Position();
						m_vTargetingPos = XMVectorSetY(m_vTargetingPos, 0.f);
						m_bCalRevolutionDir = false;

						// Trail 끄기
						m_pFrozenWolf->Set_TrailOn(false);
					}

					else
					{
						if (m_pModelCom->Get_TargetTimeAcc(m_Dash[1]) >= 2.0 && m_pModelCom->Get_TargetTimeAcc(m_Dash[1]) < 3.0)
						{
							CEffect_Manager::GetInstance()->Create_Wolf_BlowingParticle(m_pFrozenWolf->Get_BonePos_Pointer(0), __vector(0.f, 0.f, 0.f, 0.f));
							CEffect_Manager::GetInstance()->Create_Wolf_BlowingParticle(m_pFrozenWolf->Get_BonePos_Pointer(1), __vector(0.f, 0.f, 0.f, 0.f));
						}

						// 공전
						if (false == m_bCalRevolutionDir)
						{
							int iLeftPoint = m_iCurDashPoint - 1;
							int iRightPoint = m_iCurDashPoint + 1;

							if (iLeftPoint < 0)
								iLeftPoint = 7;

							if (iRightPoint > 7)
								iRightPoint = 0;

							// 현재 내 룩 방향 반대 방향
							_vector vMyDir = -XMVector3Normalize(m_pTransformCom->Get_Look());

							// 내 왼쪽 포인트
							_vector vLeftDir = XMVector3Normalize(m_vDashPoint[iLeftPoint] - m_vDashPoint[m_iCurDashPoint]);
							_vector vRightDir = XMVector3Normalize(m_vDashPoint[iRightPoint] - m_vDashPoint[m_iCurDashPoint]);

							// 왼쪽 각도 구하기
							float fLeftAngle = XMConvertToDegrees(acos(XMVectorGetX(XMVector3Dot(vMyDir, vLeftDir))));

							// 오른쪽 각도 구하기
							float fRightAngle = XMConvertToDegrees(acos(XMVectorGetX(XMVector3Dot(vMyDir, vRightDir))));

							if (fRightAngle > fLeftAngle)
								m_iRevolutionDir = 1;

							else
								m_iRevolutionDir = -1;

							m_pFrozenWolf->Set_TrailOn(true);

							m_bCalRevolutionDir = true;
						}

						auto vLook = m_pTransformCom->Revolution_And_ReturnLook(__vector(0.f, 1.f, 0.f, 0.f), __vector(0.f, 0.f, 0.f, 1.f), TimeDelta * m_iRevolutionDir);

						vLook = XMVector3Normalize(vLook);

						m_pTransformCom->Turn_To_Direction(vLook, TimeDelta * 5.0);
					}
				}
				
			}

			else
			{
				// 물기 공격
				auto vDir = m_pTransformCom->Get_Dir(m_vTargetingPos);
				vDir = XMVectorSetY(vDir, 0.f);

				auto isTurnFinished = m_pTransformCom->Turn_To_Direction(vDir, TimeDelta * 10.0, 0.35f);
				auto fDistance = m_pTransformCom->Get_Distance(m_vTargetingPos);

				// 플레이어와의 거리가 25이상이면 플레이어 향해 돌진
				if(fDistance > 25.f)
					m_pTransformCom->Go_Straight(TimeDelta * 10.0, m_pMonsterNavigation);

				else
				{
					// 해당 범위 안에 들어오면 물기 애니메이션 재생
					//m_pTransformCom->MakeSameDirection(m_pTransformCom->Get_Dir(m_vTargetingPos));
					m_pFrozenWolf->Set_AnimAndReset(m_pModelCom, m_Dash[2]);
					m_isReadyToBite = false;
					m_isReadyToDash = false;
					m_bTurningBeforeAttack = false;
					
					_float fMinDistance = 0;

					for (_uint i = 0; i < 7; ++i)
					{
						auto fDistance = DISTANCE(m_vDashPoint[i] - m_pTransformCom->Get_Position());

						if (i == 0 || fDistance < fMinDistance)
						{
							fMinDistance = fDistance;
							m_iDashIndex = i;
						}
					}
				}
			}
		}
	}
	
	if (m_pModelCom->Check_CurAnim_And_Finished(m_Dash[2]) == true)
	{
		++m_iDashCnt;

		m_TimeAcc = 0.0;

		m_pModelCom->Set_TempIndex(m_Dash[1]);
	}

	if (m_iDashCnt > 2)
	{
		IsSkillFinished = true;

		m_iDashCnt = 0;

		m_isReadyToDash = false;

		m_isHeadingPoint = false;
	}



	return IsSkillFinished;
}

_bool CState_FrozenWolfAtk::Atk_Scratch(_double TimeDelta)
{
	_float fDist = m_pTransformCom->Get_Distance(m_vPlayerFocusPos);

	if (m_pModelCom->Get_CurrAnimIndex() == 8)
	{
		m_fExitTime += TimeDelta;

		auto vDir = m_pTransformCom->Get_Dir(m_vPlayerFocusPos);

		if (false == m_isTurnFinish)
		{
			if (m_pTransformCom->Turn_To_Direction(vDir, TimeDelta * 3.0, 0.3f) == false)
			{
				auto vPosY = vecy(m_pTransformCom->Get_Position());

				if (vPosY < 0.f)
					m_pTransformCom->Set_PositionY(0.f);

				else
					m_pTransformCom->Go_Straight(TimeDelta, m_pMonsterNavigation);
			}
			// 바라보는 것이 완료됐으면 애니메이션 변경
			else
			{
				m_isTurnFinish = true;
			}
		}

		//12 아니면 13
		if (fDist >= 11.f && fDist <= 14.f && m_isTurnFinish == true)
		{
			m_pFrozenWolf->Set_AnimAndReset(m_pModelCom, m_Scratch[0]);

			//m_pTransformCom->MakeSameDirection(vDir);

			m_pModelCom->Set_TargetPlaySpeed(28, 50.0);
		}

		// 14보다 클 경우
		if (fDist > 14.f)
		{
			auto vPos = XMVectorLerp(m_pTransformCom->Get_Position(), m_vPlayerFocusPos, (_float)TimeDelta * 5.f);

			m_pTransformCom->Sliding_Vector(vPos, m_pTransformCom->Get_Position(), m_pMonsterNavigation, (_float)TimeDelta * 5.f);
		}
	
		// 11보다 멀 가까울 경우
		if (fDist < 11.f)
		{
			m_pFrozenWolf->Set_AnimAndReset(m_pModelCom, 10);

			auto vPos = XMVectorLerp(m_pTransformCom->Get_Position(), m_vPlayerFocusPos, (_float)TimeDelta * 5.0f);

			m_pTransformCom->Sliding_Vector(vPos, m_pTransformCom->Get_Position(), m_pMonsterNavigation, TimeDelta * 5.0);
		}
	}


	if (m_pModelCom->Check_CurAnim_And_Finished(10) == true)
		m_pModelCom->Set_TempIndex(8);

	if (m_pModelCom->Get_TargetTimeAcc(27) >= 17 && m_bEffect[0] == false)
	{

		CEffect_Manager::GetInstance()->Create_Wolf_Scratch(m_pTransformCom, 0);
		m_bEffect[0] = true;

		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("DarkWolf_Scratch1.wav"), 0.5f, true, m_pTransformCom->Get_Position());

		m_pFrozenWolf->Set_MotionTrail(true);

	}

	if (m_pModelCom->Get_TargetTimeAcc(27) >= 16.0 && m_pModelCom->Get_TargetTimeAcc(27) < 18.0)
	{
		CEffect_Manager::GetInstance()->Create_Wolf_BlowingParticle(m_pFrozenWolf->Get_BonePos_Pointer(0), __vector(0.f, 0.f, 0.f, 0.f));
	}

	if (m_pModelCom->Get_TargetTimeAcc(27) >= 34.0 && m_pModelCom->Get_TargetTimeAcc(27) < 36.0)
	{
		CEffect_Manager::GetInstance()->Create_Wolf_BlowingParticle(m_pFrozenWolf->Get_BonePos_Pointer(1), __vector(0.f, 0.f, 0.f, 0.f));
	}

	if (m_pModelCom->Get_TargetTimeAcc(27) >= 34 && m_bEffect[1] == false)
	{
		CEffect_Manager::GetInstance()->Create_Wolf_Scratch(m_pTransformCom, 1);

		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("DarkWolf_Scratch2.wav"), 0.5f, true, m_pTransformCom->Get_Position());

		//CEffect_Manager::GetInstance()->Create_Wolf_BlowingParticle(m_pFrozenWolf->Get_BonePos_Pointer(1), __vector(0.f, 1.f, 0.f, 0.f));

		m_bEffect[1] = true;
	}

	if (m_pModelCom->Get_TargetTimeAcc(28) >= 24 && m_bEffect[2] == false)
	{
		CEffect_Manager::GetInstance()->Create_Wolf_Scratch(m_pTransformCom, 3);
		CEffect_Manager::GetInstance()->Create_Wolf_BlowingParticle(m_pFrozenWolf->Get_BonePos_Pointer(0), __vector(0.f, 0.f, 0.f, 0.f));
		CEffect_Manager::GetInstance()->Create_Wolf_BlowingParticle(m_pFrozenWolf->Get_BonePos_Pointer(1), __vector(0.f, 0.f, 0.f, 0.f));


		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("DarkWolf_Scratch3.wav"), 0.5f, true, m_pTransformCom->Get_Position());
		m_bEffect[2] = true;
	}

	// 할퀴기 공격 콜라이더 키기
	m_pFrozenWolf->SetAttackTime(m_pModelCom, 27, 17.0, 20.0);
	m_pFrozenWolf->SetAttackTime(m_pModelCom, 27, 34.0, 37.0);
	m_pFrozenWolf->SetAttackTime(m_pModelCom, 28, 24.0, 27.0);

	return m_pFrozenWolf->PlayAnimations(m_pModelCom, m_Scratch);
}

_bool CState_FrozenWolfAtk::Atk_SummonWolf(_double TimeDelta)
{
	ChasePlayer_ChangeAnim(TimeDelta, m_SummonWolf[0]);

	if (m_pModelCom->Get_TargetTimeAcc(m_SummonWolf[0]) >= 75.0 && m_bEffect[1] == false)
	{
		CEffect_Manager::GetInstance()->Create_Wolf_SmokeWave(m_pTransformCom);

		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("DarkWolf_Summon.wav"), 0.5f, true, m_pTransformCom->Get_Position());
		m_bEffect[1] = true;
	}

	if (m_pModelCom->Get_TargetTimeAcc(m_SummonWolf[0]) >= 10.0 && m_bEffect[0] == false)
	{

		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("DarkWolf_Howl.wav"), 0.5f, true, m_pTransformCom->Get_Position());
		m_bEffect[0] = true;
	}

	if (m_pModelCom->Get_TargetTimeAcc(m_SummonWolf[0]) >= 81.0 && m_pModelCom->Get_TargetTimeAcc(m_SummonWolf[0]) <= 88.0)
	{
		auto vPos = m_pFrozenWolf->Get_TailPos();
		CEffect_Manager::GetInstance()->Create_Wolf_Rock(vPos);
	}

	if (m_pModelCom->Get_TargetTimeAcc(m_SummonWolf[0]) >= 81.0 && m_pModelCom->Get_TargetTimeAcc(m_SummonWolf[0]) <= 82.0 ||
		m_pModelCom->Get_TargetTimeAcc(m_SummonWolf[0]) >= 83.0 && m_pModelCom->Get_TargetTimeAcc(m_SummonWolf[0]) <= 84.0 ||
		m_pModelCom->Get_TargetTimeAcc(m_SummonWolf[0]) >= 85.0 && m_pModelCom->Get_TargetTimeAcc(m_SummonWolf[0]) <= 86.0 ||
		m_pModelCom->Get_TargetTimeAcc(m_SummonWolf[0]) >= 87.0 && m_pModelCom->Get_TargetTimeAcc(m_SummonWolf[0]) <= 88.0 )
	{
		// 10마리 정도 생성 (꼬리뼈 위치 찾아서 생성시 룩뱡항 넘겨주고, 디렉션 생성해서 넘겨줌)
		Summon_LittleWolves(TimeDelta, 0.f);
	}

	return m_pFrozenWolf->PlayAnimations(m_pModelCom, m_SummonWolf);
}
          
_bool CState_FrozenWolfAtk::Atk_SpinAtk(_double TimeDelta)
{
	_bool		isFinished = false;

	ChasePlayer_ChangeAnim(TimeDelta, m_SpinAttack[0]);

	if (m_pModelCom->Check_CurAnim_And_Finished(m_SpinAttack[0]) == true)
	{
		m_pFrozenWolf->Set_AnimAndReset(m_pModelCom, m_SpinAttack[2]);

		m_pModelCom->Set_TargetPlaySpeed(m_SpinAttack[0], 50.f);
	}

	// 하늘로 올라가는 애니메이션
	if (m_pModelCom->Get_CurrAnimIndex() == m_SpinAttack[0])
	{
		if (m_pModelCom->Get_TargetTimeAcc(m_SpinAttack[0]) >= 32.0 && m_pModelCom->Get_TargetTimeAcc(m_SpinAttack[0]) <= 44.0)
		{
			if (m_bEffect[3] == false)
			{
				CEffect_Manager::GetInstance()->Create_Wolf_TailWind(m_pTransformCom);
				CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("DarkWolf_RoarToSpin.wav"), 0.5f, true, m_pTransformCom->Get_Position());
				m_bEffect[3] = true;
			}
			auto vPos = m_pFrozenWolf->Get_TailPos();
			CEffect_Manager::GetInstance()->Create_Wolf_Rock(vPos);
		}

		if (m_pModelCom->Get_TargetTimeAcc(m_SpinAttack[0]) > 66.f)
		{
			if (m_bEffect[0] == false)
			{
				CEffect_Manager::GetInstance()->Create_Wolf_SoaringRipple(m_pTransformCom);

				m_bEffect[0] = true;
			}

			if (false == m_bEffect[1])
			{
				CEffect_Manager::GetInstance()->Create_Wolf_SoaringRibbon(m_pFrozenWolf->Get_CenterBoneMatrix());
				m_bEffect[1] = true;
			}

			auto vPos = XMVectorLerp(m_pTransformCom->Get_Position(), m_vTargetingPos, (_float)TimeDelta * 2.f);

			m_pTransformCom->Sliding_Vector(vPos, m_pTransformCom->Get_Position(), m_pMonsterNavigation, TimeDelta * 2.0);

			// m_pTransformCom->Go_Up(TimeDelta *6 10.0, m_pMonsterNavigation);
			
			m_pModelCom->Set_TargetPlaySpeed_Rate(32, 3.0);
		}
	}

	// 하늘에서 마무리 하는 동작
	if (m_pModelCom->Get_CurrAnimIndex() == m_SpinAttack[2])
	{
		auto vDir = m_pTransformCom->Get_Dir(m_vTargetingPos);
		auto vPos = XMVectorLerp(m_pTransformCom->Get_Position(), m_vTargetingPos, (_float)TimeDelta * 5.f);

		if(m_pTransformCom->Get_Distance(m_vTargetingPos) > 1.f)
			m_pTransformCom->Sliding_Vector(vPos, m_pTransformCom->Get_Position(), m_pMonsterNavigation, TimeDelta * 5.0);
	}

	if (m_pModelCom->Check_CurAnim_And_Finished(m_SpinAttack[2]) == true)
	{
		m_vTargetingPos = m_pPlayerTransform->Get_Position();

		m_vTargetingPos = XMVectorSetY(m_vTargetingPos, 0.f);
		m_GoStraightPower = m_pTransformCom->Get_Distance(m_vTargetingPos) * 0.15f;


		//m_pFrozenWolf->Set_AnimAndReset(m_pModelCom, m_SpinAttack[3]);

		m_pModelCom->Set_TempIndex(m_SpinAttack[3]);

		//m_pModelCom->Set_TargetPlaySpeed(m_SpinAttack[3], 50.0);
	}

	// 준비 후 내리찍기 (중력)
	if (m_pModelCom->Get_CurrAnimIndex() == 35)
	{
		CGameInstance::GetInstance()->PlayRoofSound(TEXT("DarkWolf_SpinAttack.wav"), CSoundMgr::BOSS, 0.5f, false, m_pTransformCom->Get_Position());

		if (m_bEffect[2] == false)
		{
			CEffect_Manager::GetInstance()->Create_Wolf_SpinAttackRing(m_pFrozenWolf->Get_CenterBoneMatrix(), &m_bEffectDead);
			m_bEffect[2] = true;
		}
		auto fDist = m_pTransformCom->Get_Distance(m_vTargetingPos);
		auto vDir = XMVector3Normalize(m_pTransformCom->Get_Dir(m_vTargetingPos));

	// 바닥에 닿은 후
	if (fDist <= 0.2f)
	{
		//p + 2n(-p * n)
		auto vYAxis = __vector(0.f, 1.f, 0.f, 0.f);
		auto vReflect = vDir + 2 * (vYAxis * XMVector3Dot(-vDir, vYAxis));
		vDir = XMVectorSetY(vDir, 0.f);


		m_vTargetingPos = m_pTransformCom->Get_Position() + vDir * 2.f;
		m_vTargetingPos = XMVectorSetY(m_vTargetingPos, 0.f);

		m_pFrozenWolf->Set_AnimAndReset(m_pModelCom, 36);

		CEffect_Manager::GetInstance()->Create_Wolf_LandingEffect(m_pTransformCom);

		CGameInstance::GetInstance()->StopSound(CSoundMgr::BOSS);

		m_bEffectDead = true;
	}

	else
	{
		m_pTransformCom->Go_Vector(vDir, TimeDelta * 10.0, m_pMonsterNavigation);
	}

	}

	if (m_pModelCom->Get_CurrAnimIndex() == 36)
	{
		m_pModelCom->Set_TargetPlaySpeed_Rate(36, m_fSpinSpeed);
	
		m_fSpinSpeed -= (_float)TimeDelta * 5.f;

		if(m_fSpinSpeed < 1.f)
			m_fSpinSpeed = 1.f;

		auto vDir = m_pTransformCom->Get_Dir(m_vTargetingPos);
		//m_pTransformCom->Go_Vector(vDir, TimeDelta * 5.0);
		//m_pFrozenWolf->Set_GravitySwitch(true);

		auto vPos = XMVectorLerp(m_pTransformCom->Get_Position(), m_vTargetingPos, (_float)TimeDelta * 5.f);

		m_pTransformCom->Sliding_Vector(vPos, m_pTransformCom->Get_Position(), m_pMonsterNavigation, TimeDelta * 5.0);

		//m_pFrozenWolf->Set_GravitySwitch(true);


	}

	if (m_pModelCom->Check_CurAnim_And_Finished(36) == true)
	{
		isFinished = true;
		m_pFrozenWolf->Set_GravitySwitch(false);


	}

	m_pFrozenWolf->SetAttackTime(m_pModelCom, 35, 1.0, 10.0);
	m_pFrozenWolf->SetAttackTime(m_pModelCom, 36, 1.0, 35.0);

	return isFinished;
}

_bool CState_FrozenWolfAtk::Atk_ThrowFire(_double TimeDelta)
{

	ChasePlayer_ChangeAnim(TimeDelta, m_TrhowFire[0]);

	if (m_pModelCom->Get_CurrAnimIndex() == m_TrhowFire[0])
	{
		if (m_pModelCom->Get_TargetTimeAcc(m_TrhowFire[0]) >= 50.f && m_bThrowFire[0] == false ||
			m_pModelCom->Get_TargetTimeAcc(m_TrhowFire[0]) >= 68.f && m_bThrowFire[1] == false || 
			m_pModelCom->Get_TargetTimeAcc(m_TrhowFire[0]) >= 87.f && m_bThrowFire[2] == false)
		{
			_matrix HeadMatrix = m_pFrozenWolf->Get_AtkBoneMatrix();
			_vector vDir = XMVector3Normalize(HeadMatrix.r[2]);
			CMonsterSkill::MONSTERSKILLDESC MonsterSkillDesc;
			MonsterSkillDesc.bNoModel = true;
			MonsterSkillDesc.vPos = HeadMatrix.r[3];
			MonsterSkillDesc.vDestPos = HeadMatrix.r[3] + vDir * 5.f;
			MonsterSkillDesc.vDir = vDir;
			MonsterSkillDesc.fSpeed = 30.f;
			MonsterSkillDesc.fDamage = 5.f;
			MonsterSkillDesc.Duration = 1.5f;
			MonsterSkillDesc.fSize = 3.f;
			MonsterSkillDesc.pPlayerTransform = m_pPlayerTransform;
			MonsterSkillDesc.eSkillType = CMonsterSkill::MONSTERSKILLTYPE_TRACING;

			CGameObject* pSkill = CGameInstance::GetInstance()->Add_GameObject_And_Return(CGameInstance::GetInstance()->Get_LevelIndex(), TEXT("Layer_MonsterSkill"), TEXT("Prototype_GameObject_MonsterSkill"), &MonsterSkillDesc);
			CEffect_Manager::GetInstance()->Create_Wolf_Fire(pSkill->Get_Transform(), pSkill->Get_DeadPointer());
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("DarkWolf_ThrowFire.wav"), 0.5f, true, m_pTransformCom->Get_Position());
			m_bThrowFire[m_iFireIndex++] = true;
		}
	
	}

	return m_pFrozenWolf->PlayAnimations(m_pModelCom, m_TrhowFire);
}

_bool CState_FrozenWolfAtk::Atk_PurpleRain(_double TimeDelta)
{
	_bool isFinished = false;

	ChasePlayer_ChangeAnim(TimeDelta, m_PurpleRain[0]);

	if (m_pModelCom->Get_CurrAnimIndex() == m_PurpleRain[0])
	{
		if (m_pModelCom->Get_TargetTimeAcc(m_PurpleRain[0]) > 9.0)
		{
			if (false == m_bCameraMoving)
			{
				auto pCam = CGameManager::GetInstance()->Get_Camera();

				pCam->Set_VerticalShake(2.f, 1.0f, 1.f);
				pCam->Set_HorizonShake(2.f, 1.0f, 1.f);

				m_bCameraMoving = true;
			}
			
		}

		if (m_pModelCom->Get_TargetTimeAcc(m_PurpleRain[0]) > 35.0)
		{
			if (false == m_bEffect[0])
			{
				CEffect_Manager::GetInstance()->Create_Wolf_Marble();
				m_bEffect[0] = true;
				CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("DarkWolf_MakeRain.wav"), 0.5f, true, m_pTransformCom->Get_Position());

			}
		}
		if (m_pModelCom->Get_TargetTimeAcc(m_PurpleRain[0]) > 82.0)
		{
			if (false == m_bEffect[1])
			{
				CEffect_Manager::GetInstance()->Create_Wolf_SoaringRipple(m_pTransformCom);
				m_bEffect[1] = true;
			}

			if (false == m_bEffect[2])
			{
				CEffect_Manager::GetInstance()->Create_Wolf_SoaringRibbon(m_pFrozenWolf->Get_CenterBoneMatrix());
				m_bEffect[2] = true;
			}

			auto vPos = XMVectorLerp(m_pTransformCom->Get_Position(), m_vTargetingPos, (_float)TimeDelta * 2.f);

			m_pTransformCom->Sliding_Vector(vPos, m_pTransformCom->Get_Position(), m_pMonsterNavigation, TimeDelta * 2.0);

			// m_pTransformCom->Go_Up(TimeDelta * 10.0, m_pMonsterNavigation);

			m_pModelCom->Set_TargetPlaySpeed_Rate(m_PurpleRain[0], 3.0);

			m_TimeAcc = 0.0;
			m_TimeAccum_Sum = 0.0;
		}
	}

	if (m_pModelCom->Check_CurAnim_And_Finished(m_PurpleRain[0]) == true)
	{
		auto fDist = m_pTransformCom->Get_Distance(m_vTargetingPos);

		if (fDist > 1.f)
		{
			auto vPos = XMVectorLerp(m_pTransformCom->Get_Position(), m_vTargetingPos, (_float)TimeDelta * 2.f);

			m_pTransformCom->Sliding_Vector(vPos, m_pTransformCom->Get_Position(), m_pMonsterNavigation, TimeDelta * 2.0);
		}

		m_fRainSkillTime += TimeDelta;
		m_TimeAccum_Sum += TimeDelta;

		if (m_fRainSkillTime < 15.0)
		{
			if (m_TimeAcc > 0.2)
			{
				CMonsterSkill::MONSTERSKILLDESC MonsterSkillDesc;
				MonsterSkillDesc.bNoModel = true;
				MonsterSkillDesc.vPos = __vector(_float(rand() % 50 - 25), 50.f, _float(rand() % 50 - 25), 1.f);
				MonsterSkillDesc.fSpeed = 30.f;
				MonsterSkillDesc.fDamage = 5.f;
				MonsterSkillDesc.Duration = 1.5f;
				MonsterSkillDesc.fSize = 3.f;
				MonsterSkillDesc.pPlayerTransform = m_pPlayerTransform;
				MonsterSkillDesc.eSkillType = CMonsterSkill::MONSTERSKILLTYPE_FALLSTRAIGHT;

				CGameObject* pSkill = CGameInstance::GetInstance()->Add_GameObject_And_Return(CGameInstance::GetInstance()->Get_LevelIndex(), TEXT("Layer_MonsterSkill"), TEXT("Prototype_GameObject_MonsterSkill"), &MonsterSkillDesc);
				
				CEffect_Manager::GetInstance()->Create_Wolf_Rain(pSkill->Get_Transform(), pSkill->Get_DeadPointer());
				m_TimeAcc = 0.0;
			}

			// 1.0
			if (m_TimeAccum_Sum > 2.0)
			{
				CLittleWolf::WOLFDESC Desc;
				Desc.vStartPos = __vector(_float(rand() % 40 - 20), 50.f, _float(rand() % 40 - 20), 1.f);
				Desc.vStartDir = __vector(0.f, 0.f, 1.f, 0.f);
				Desc.bRain = true;

				CGameInstance::GetInstance()->Add_GameObject(CGameInstance::GetInstance()->Get_LevelIndex(), TEXT("Layer_LittleWolf"), TEXT("Prototype_GameObject_LittleWolf"), &Desc);

				m_TimeAccum_Sum = 0.0;
			}
			
		}

		else
		{
			m_pModelCom->Set_TempIndex(40);

			m_vTargetingPos = m_pTransformCom->Get_Position();
			m_vTargetingPos = XMVectorSetY(m_vTargetingPos, 0.f);
		}
		
	}

	if (m_pModelCom->Get_CurrAnimIndex() == 40)
	{
		auto vPosY = XMVectorGetY(m_pTransformCom->Get_Position());

		if (vPosY > 1.f)
		{
			auto vPos = XMVectorLerp(m_pTransformCom->Get_Position(), m_vTargetingPos, (_float)TimeDelta * 10.f);
			m_pTransformCom->Sliding_Vector(vPos, m_pTransformCom->Get_Position(), m_pMonsterNavigation, TimeDelta * 7.0);
		}

		else
		{
			m_pTransformCom->Set_PositionY(0.f);
			m_pFrozenWolf->Set_AnimAndReset(m_pModelCom, 41);
			CEffect_Manager::GetInstance()->Create_Wolf_LandingEffect(m_pTransformCom);
			CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("DarkWolf_Landing.wav"), 0.5f, true, m_pTransformCom->Get_Position());
		}

	}

	if (m_pModelCom->Check_CurAnim_And_Finished(41) == true)
	{
		isFinished = true;
	}


	return isFinished;
}

void CState_FrozenWolfAtk::ChasePlayer_ChangeAnim(_double TimeDelta, _uint iAnimIndex)
{
	if(true == m_isFinishChase)
		return;
	// 걷는 애니메이션(9) 실행 중일 때 플레이어를 바라보면서 플레이어에게 접근

	_float fDist = m_pTransformCom->Get_Distance(m_vPlayerFocusPos);

	if (m_pModelCom->Get_CurrAnimIndex() == 8 && fDist > 12.f)
	{
		m_fExitTime += TimeDelta;

		auto vDir = m_pTransformCom->Get_Dir(m_vPlayerFocusPos);

		if (m_pTransformCom->Turn_To_Direction(vDir, TimeDelta * 3.0, 0.3f) == false)
		{
			auto vPosY = vecy(m_pTransformCom->Get_Position());

			if(vPosY < 1.f)
				m_pTransformCom->Set_PositionY(1.f);

			else
				m_pTransformCom->Go_Straight(TimeDelta, m_pMonsterNavigation);
		}
		// 바라보는 것이 완료됐으면 애니메이션 변경
		else
		{
			m_pFrozenWolf->Set_AnimAndReset(m_pModelCom, iAnimIndex);

			m_pTransformCom->MakeSameDirection(vDir);

			m_fExitTime = 0.0;

			m_isFinishChase = true;
		}
	}

	else if (fDist <= 12.f)
	{
		m_pFrozenWolf->Set_AnimAndReset(m_pModelCom, 10);
		m_pTransformCom->Go_Backward(TimeDelta * 5.0, m_pMonsterNavigation);
	
	}

	if (m_pModelCom->Check_CurAnim_And_Finished(10) == true)
		m_pModelCom->Set_TempIndex(8);

}

_bool CState_FrozenWolfAtk::Summon_LittleWolves(_double TimeDelta, _double Interval)
{
	_bool	isFinish = false;


	if (m_TimeAccum_Sum == 0.0)
	{

		_vector vDir = XMVector4Normalize(m_pTransformCom->Get_Look());

		// 방향 설정
		auto Matrix = XMMatrixRotationY(XMConvertToRadians(45.f * m_iLittleWolfCnt++));
		vDir = XMVector3TransformNormal(vDir, Matrix);

		_vector vPos = m_pTransformCom->Get_Position()+ __vector(0.f, 5.5f, 0.f, 0.f);
		vPos += XMVector4Normalize(vDir) * 10.f;

		if (m_pMonsterNavigation->Move_OnNavigation(vPos, m_pTransformCom->Get_Position()) == true)
		{
			vDir = XMVectorSetW(vDir, 0.f);

			CLittleWolf::WOLFDESC Desc;
			Desc.vStartPos = vPos;
			Desc.vStartDir = XMVector4Normalize(vDir);

			CGameInstance::GetInstance()->Add_GameObject(CGameInstance::GetInstance()->Get_LevelIndex(), TEXT("Layer_LittleWolf"), TEXT("Prototype_GameObject_LittleWolf"), &Desc);
		}

		m_TimeAcc = 0.0;

		isFinish = true;
	}

	m_TimeAccum_Sum += TimeDelta; 

	if(m_TimeAccum_Sum > Interval)
		m_TimeAccum_Sum = 0.0;

	return isFinish;
}

CState_FrozenWolfAtk* CState_FrozenWolfAtk::Create(void* pArg)
{
	CState_FrozenWolfAtk* pInstance = new CState_FrozenWolfAtk;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		BREAKPOINT;
		return nullptr;
	}

	return pInstance;
}

void CState_FrozenWolfAtk::Free()
{
	__super::Free();
}
