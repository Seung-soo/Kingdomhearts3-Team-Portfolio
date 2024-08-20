#include "stdafx.h"
#include "..\public\Camera_Perspective.h"
#include "GameInstance.h"
#include "Easing_Function.h"
#include "ImGui_Manager.h"
#include "Camera_Manager.h"
#include "GameManager.h"
#include "Player.h"
#include "Progress_Manager.h"
#include "HierarchyNode.h"
#include "Terrain.h"
#include "Combat_Assistant.h"

CCamera_Perspective::CCamera_Perspective(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CCamera(pDevice, pDeviceContext)
{
}

CCamera_Perspective::CCamera_Perspective(const CCamera_Perspective & rhs)
	: CCamera(rhs)
{
}

void CCamera_Perspective::Set_LookNPC(_bool bLookNPC, CTransform* pNpcTransform)
{
	m_bLookNPC = bLookNPC;
	m_pNpcTransform = pNpcTransform;
	m_vBeforeLookAtNPC_Look = m_pTransformCom->Get_Look();
	m_vBeforeLookAtNPC_Position = m_pTransformCom->Get_Position();
}

HRESULT CCamera_Perspective::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Perspective::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Speed(10.f);

	m_fDistance = 4.5f;
	m_fDefaultDistance = 4.5f;
	m_fSensitivity = 0.1f;
	m_fAdjustorX = 0.f;
	m_fAdjustorY = 1.25f;

	if (LEVEL_TWILIGHT <= GM->Get_CurrentLevel() && LEVEL_FINALBOSS >= GM->Get_CurrentLevel())
	{
		CNavigation::NAVIDESC tNaviDesc;
		tNaviDesc.iCurrentIndex = 0;
		tNaviDesc.iPreviousIndex = 0;
		tNaviDesc.iPreviousFrame = 0;
		tNaviDesc.pParent = m_pTransformCom;

		_tchar wszNaviPrototypeTag[128] = TEXT("");

		switch (GM->Get_CurrentLevel())
		{
		case LEVEL_TWILIGHT:
			lstrcpy(wszNaviPrototypeTag, TEXT("Prototype_Component_Navigation_Twilight"));
			break;
		case LEVEL_RAPUNZEL:
			lstrcpy(wszNaviPrototypeTag, TEXT("Prototype_Component_Navigation_Rapunzel"));
			break;
		case LEVEL_RAPUNZEL_BOSS:
			lstrcpy(wszNaviPrototypeTag, TEXT("Prototype_Component_Navigation_Rapunzel_Boss"));
			break;
		case LEVEL_FROZEN:
			lstrcpy(wszNaviPrototypeTag, TEXT("Prototype_Component_Navigation_Frozen"));
			break;
		case LEVEL_KEYGRAVE:
			lstrcpy(wszNaviPrototypeTag, TEXT("Prototype_Component_Navigation_KeyGrave"));
			break;
		case LEVEL_FINALBOSS:
			lstrcpy(wszNaviPrototypeTag, TEXT("Prototype_Component_Navigation_FinalBoss"));
			break;
		}

		/* For.Com_Navigation */
		if (FAILED(__super::Add_Component(GM->Get_CurrentLevel(), wszNaviPrototypeTag, TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &tNaviDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());
	}
	
	m_fLerpInterval = 0.1f;

	return S_OK;
}

_int CCamera_Perspective::Tick(_double TimeDelta)
{
	CGameInstance* pGI = CGameInstance::GetInstance();

	// ImGuI On, Off 전환
	if (pGI->Get_KeyPressing(DIK_LCONTROL))
	{
		if (pGI->Get_KeyEnter(DIK_C))
		{
			m_bImGuiController = !m_bImGuiController;
		}
	}
	
	if (nullptr == m_pFocusingTarget)
	{
		LEVEL current_level = GM->Get_CurrentLevel();
		m_pFocusingTarget = pGI->Get_Back(current_level, TEXT("Layer_Player"));

		if (nullptr == m_pFocusingTarget)
		{

			assert(SUCCEEDED(E_FAIL));
			return RESULT_ERROR;
		}

		else
		{
			m_pTargetTransform = static_cast<CTransform*>(m_pFocusingTarget->Get_Component(CGameObject::m_pTransformTag));

			if (false == m_bStartCameraWork)
			{
				m_bFocusing = true;

				_vector	vLook = m_pTargetTransform->Get_Look();
				vecy(vLook) -= 0.3f;
				m_pTransformCom->MakeSameDirection(vLook);
			}
		}
	}


	if (false == m_bFocusing)		// 카메라 자유모드
	{
		DynamicMove(TimeDelta);
	}
	else							// 평소 상태(플레이어 혹은 타겟과 플레이어 사이를 추적)
	{	
		_long mouse_move = 0;

		if (false == GM->Get_bCameraMove() || true == PM->Get_MinigameWaiting())
		{
			if (mouse_move = pGI->Get_DIMouseMoveState(CInput_Device::MMS_X))
			{
				m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * mouse_move * m_fSensitivity);
			}

			if (mouse_move = pGI->Get_DIMouseMoveState(CInput_Device::MMS_Y))
			{
				m_pTransformCom->CameraTurnY(XMVector4Normalize(m_pTransformCom->Get_Right()), TimeDelta * mouse_move * m_fSensitivity);
			}
		}

		if (false == m_bTalkNPC && false == PM->Get_MinigameWaiting())
		{
			m_fCameraTime += (_float)TimeDelta;
			if (m_fLerpInterval < m_fCameraTime)
				m_fCameraTime = m_fLerpInterval;

			CCombat_Assistant* pCombat_Assistant = GM->Get_Player()->Get_CombatAssistant();
			CCombat_Assistant::ASSIST_TYPE eType = pCombat_Assistant->Get_TargetType();
			CCollider* pLockOnCollider = pCombat_Assistant->Get_TargetCollider();
			_vector vTargetPosition;
			if (nullptr != pLockOnCollider && CCombat_Assistant::ASSIST_FASTEN == eType)
			{
				vTargetPosition = (pLockOnCollider->Get_Center() * 0.35f + m_pTargetTransform->Get_State(CTransform::STATE_POSITION) * 0.65f);
				_float fDistanceGap = m_fDefaultDistance - m_fDistance;
				m_fDefaultDistance = DISTANCE(pLockOnCollider->Get_Center() - m_pTargetTransform->Get_State(CTransform::STATE_POSITION)) * 0.5f + 4.5f;
				m_fDistance = m_fDefaultDistance - fDistanceGap;
				m_fAdjustorY = 0.6f;
			}
			else
			{
				vTargetPosition = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);
				_float fDistanceGap = m_fDefaultDistance - m_fDistance;
				m_fDefaultDistance = 4.5f;
				m_fDistance = m_fDefaultDistance - fDistanceGap;
				m_fAdjustorY = 1.25f;
			}
			_float fDist = DISTANCE(vTargetPosition - m_vLerpPosition);
			if (false == m_bFirstTick)
			{
				m_vLerpPosition = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);
				m_bFirstTick = true;
			}
			else
				m_vLerpPosition = XMVectorLerp(m_vLerpPosition, vTargetPosition, m_fCameraTime / m_fLerpInterval);
			if (0.4f < fDist)
				m_fCameraTime = 0.f;

			// 위치 갱신
			_vector target_position = vTargetPosition;
			if (LEVEL_FINALBOSS == GM->Get_CurrentLevel())
				target_position = m_vLerpPosition;

			_vector this_look = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			this_look = XMVector3Normalize(this_look);

			_vector new_position_of_this = target_position - (this_look * m_fDistance);
			new_position_of_this += XMVectorSet(0.f, 1.f, 0.f, 0.f) * m_fAdjustorY;

			_vector this_right_norm = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			this_right_norm.m128_f32[1] = 0.f;
			this_right_norm = XMVector3Normalize(this_right_norm);

			_vector this_up_norm = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));

			new_position_of_this += this_right_norm * m_fAdjustorX;

			// 카메라의 위치를 지형과 비교하는 작업을 수행

			if (LEVEL_TWILIGHT <= GM->Get_CurrentLevel() && LEVEL_FINALBOSS >= GM->Get_CurrentLevel())
			{
				_float fGroundY = 0.f;
				if (LEVEL_RAPUNZEL_BOSS == GM->Get_CurrentLevel())
				{
					_vector vPos = static_cast<CTerrain*>(m_pGameInstance->Get_Front(LEVEL_RAPUNZEL_BOSS, TEXT("Layer_Terrain")))
						->Compute_PositionOnTerrain(new_position_of_this);
					fGroundY = vecy(vPos);
					if (vecy(new_position_of_this) < fGroundY + 0.25f)
						vecy(new_position_of_this) = fGroundY + 0.25f;
				}
				else
				{
					fGroundY = m_pNavigationCom->Compute_Y(new_position_of_this);
					if (vecy(new_position_of_this) < fGroundY + 0.25f)
						vecy(new_position_of_this) = fGroundY + 0.25f;
				}

				if (LEVEL_TWILIGHT != GM->Get_CurrentLevel())
				{
					_float fDist = m_fDistance;
					_float fPosY = vecy(new_position_of_this);
					while (false == m_pTransformCom->Set_PositionCameraNavi(new_position_of_this, m_pNavigationCom))
					{
						fDist -= 0.05f;
						if (0.1f > fDist)
						{
							fDist = 0.1f;
							break;
						}
						new_position_of_this = target_position - (this_look * fDist);
						new_position_of_this += this_right_norm * m_fAdjustorX;
					}
					vecy(new_position_of_this) = fPosY;
					m_pNavigationCom->Move_OnNavigation(new_position_of_this, m_pTransformCom->Get_Position(), nullptr);
				}
			}

			m_pTransformCom->Set_Position(new_position_of_this);
			if (LEVEL_TWILIGHT < GM->Get_CurrentLevel() && LEVEL_FINALBOSS >= GM->Get_CurrentLevel())
			{
				m_fNaviUpdate += (_float)TimeDelta;
				if (3.f < m_fNaviUpdate)
				{
					m_fNaviUpdate = 0.f;
					m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());
				}
			}																					  
		}
	}


	if (pGI->Get_KeyPressing(DIK_LCONTROL))
	{
		if (pGI->Get_KeyEnter(DIK_F))
			m_bFocusing = !m_bFocusing;
	}

	// 카메라 워크할 때 업데이트
	if (true == m_bStartCameraWork)
	{
		Camera_Work(TimeDelta);
	}

	// NPC를 바라볼 수 있는 상태면 바라보게
	if (true == m_bLookNPC)
	{
		Look_NPC(TimeDelta);
	}

	// 직접 코드쳐서 만든 카메라 씬
	Camera_Scene(TimeDelta);

	// 카메라 씬
	CCamera_Manager::GetInstance()->Play(TimeDelta);
	CCamera_Manager::GetInstance()->F_Play(TimeDelta);

	if (true == m_bStartTimer)
		m_fTimer += (_float)TimeDelta;

	if (0 > __super::Tick(TimeDelta))
		return -1;

	return _int();
}

_int CCamera_Perspective::LateTick(_double TimeDelta)
{

	m_TimeDelta = TimeDelta;

	if (FAILED(Camera_Shake()))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	return __super::LateTick(TimeDelta);
}

HRESULT CCamera_Perspective::Render()
{
	return S_OK;
}

void CCamera_Perspective::PostRender(_double TimeDelta)
{
	ImGui::Begin("Perspective Camera Control Panel");
	
	ImGui::DragFloat("Distance", &m_fDistance, 0.25f, 1.f, 30.f);
	ImGui::DragFloat("Sensitivity", &m_fSensitivity, 0.05f);

	ImGui::Separator();
	ImGui::Separator();

	ImGui::DragFloat("Adjustor_Horizontal", &m_fAdjustorX, 0.25f);
	ImGui::DragFloat("Adjustor_Vertical", &m_fAdjustorY, 0.25f);

	ImGui::Separator();
	ImGui::Separator();

	if (ImGui::Button("PAUSE"))
	{
		GM->PlaySpeedControl(0.f);
		m_bStartTimer = false;
	}

	if (ImGui::Button("CONTINUE"))
	{
		GM->PlaySpeedControl(1.f);
		m_bStartTimer = true;
	}

	if (ImGui::Button("TIME_RESET"))
	{
		m_fTimer = 0.f;
		m_bStartTimer = false;
	}

	ImGui::Separator();
	ImGui::Separator();

	char szTimerText[32] = "Timer : ";
	char szTime[32] = "";
	sprintf_s(szTime, "%.3f", m_fTimer);
	strcat_s(szTimerText, szTime);

	ImGui::Text(szTimerText);

	ImGui::End();
}

void CCamera_Perspective::DynamicMove(_double TimeDelta)
{
	_bool mouse_input = false;

	CGameInstance* pGI = CGameInstance::GetInstance();
	if (pGI->Get_DIMouseButtonState(CInput_Device::MBS_RBUTTON))
	{
		mouse_input = true;
	}

	if (false == mouse_input)
		return;

	if (pGI->Get_KeyPressing(DIK_W))
	{
		m_pTransformCom->Go_Straight(TimeDelta);
	}

	if (pGI->Get_KeyPressing(DIK_S))
	{
		m_pTransformCom->Go_Backward(TimeDelta);
	}

	if (pGI->Get_KeyPressing(DIK_A))
	{
		m_pTransformCom->Go_Left(TimeDelta);
	}

	if (pGI->Get_KeyPressing(DIK_D))
	{
		m_pTransformCom->Go_Right(TimeDelta);
	}

	if (pGI->Get_KeyPressing(DIK_E))
	{
		m_pTransformCom->Go_Up(TimeDelta);
	}

	if (pGI->Get_KeyPressing(DIK_Q))
	{
		m_pTransformCom->Go_Down(TimeDelta);
	}

	_long		MouseMove = 0;

	if (MouseMove = pGI->Get_DIMouseMoveState(CInput_Device::MMS_X))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * MouseMove * 0.1f);
	}

	if (MouseMove = pGI->Get_DIMouseMoveState(CInput_Device::MMS_Y))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), TimeDelta * MouseMove * 0.1f);
	}
}

HRESULT CCamera_Perspective::Camera_Shake()
{
	if (true == m_bHorizenShake)
	{
		if (m_iHShakeInterval % 2 == 0)
		{
			if (true == m_bLeftShake)
			{
				m_pTransformCom->NowRotation(m_pTransformCom->Get_State(CTransform::STATE_UP)
					, XMConvertToRadians(m_fShakePower));
				m_bLeftShake = false;
				m_bReturnRotationH = true;
			}
			else
			{
				m_pTransformCom->NowRotation(m_pTransformCom->Get_State(CTransform::STATE_UP)
					, XMConvertToRadians(-m_fShakePower));
				m_bLeftShake = true;
				m_bReturnRotationH = true;
			}
		}
		m_fHSTime -= (_float)m_TimeDelta;
		if (0.f > m_fHSTime)
			m_bHorizenShake = false;

		if (m_fShakeDownTime > m_fHSTime)
			m_fShakePower = m_fStartShakePower - EaseLine(m_fStartShakePower
				, m_fShakeDownTime - m_fHSTime, m_fShakeDownTime);

		m_iHShakeInterval++;

	}

	if (true == m_bVerticalShake)
	{
		if (m_iVShakeInterval % 2 == 0)
		{
			if (true == m_bTopShake)
			{
				m_pTransformCom->NowRotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(m_fShakePower));
				m_bTopShake = false;
				m_bReturnRotationV = true;
			}
			else
			{
				m_pTransformCom->NowRotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(-m_fShakePower));
				m_bTopShake = true;
				m_bReturnRotationV = true;
			}
		}
		m_fVSTime -= (_float)m_TimeDelta;
		if (0.f > m_fVSTime)
			m_bVerticalShake = false;

		if (m_fShakeDownTime > m_fVSTime)
			m_fShakePower = m_fStartShakePower - EaseLine(m_fStartShakePower, m_fShakeDownTime - m_fVSTime, m_fShakeDownTime);

		m_iVShakeInterval++;
	}

	return S_OK;
}

void CCamera_Perspective::Camera_Scene(_double TimeDelta)
{
	if (true == m_bStartScene)
	{
		switch (GM->Get_CurrentLevel())
		{
		case LEVEL_TWILIGHT:
			TwilightTown_StartScene(TimeDelta);
			break;
		case LEVEL_RAPUNZEL:
			Rapunzel_StartScene(TimeDelta);
			break;
		}
	}

	if (true == m_bBossScene)
	{
		switch (GM->Get_CurrentLevel())
		{
		case LEVEL_TWILIGHT:
			TwilightTown_BossScene(TimeDelta);
			break;
		}
	}
}

void CCamera_Perspective::TwilightTown_StartScene(_double TimeDelta)
{
	_matrix	WorldMatrix;
	WorldMatrix.r[2] = XMVector3Normalize(__vector(0.64f, -0.51f, 0.56f, 0.f));
	WorldMatrix.r[0] = XMVector3Cross(__vector(0.f, 1.f, 0.f, 0.f), WorldMatrix.r[2]);
	WorldMatrix.r[1] = XMVector3Cross(WorldMatrix.r[2], WorldMatrix.r[0]);

	WorldMatrix.r[3] = __vector(-8.9f, 31.88f, -68.68f, 1.f);

	static _float	fTime = 0.f;

	fTime += (_float)TimeDelta;

	WorldMatrix.r[3] += WorldMatrix.r[0] * fTime;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	if (7.f < fTime)
	{
		m_bStartScene = false;
		GM->Set_CamaeraSceneEnd(true);
		m_pTransformCom->MakeSameDirection(GM->Get_Player()->Get_Transform()->Get_NLook());
	}
}

void CCamera_Perspective::TwilightTown_BossScene(_double TimeDelta)
{
	static _float	fTime = 0.f;

	fTime += (_float)TimeDelta;

	_vector vDir = m_pGameInstance->Get_Back(LEVEL_TWILIGHT, TEXT("Layer_Portal"))->Get_Transform()->Get_Position() - m_pTransformCom->Get_Position();
	vecy(vDir) = 3.f;
	vDir = XMVector3Normalize(vDir);

	if (3.f > fTime)
	{
		GM->Set_UIRender(false);
		m_pTransformCom->Go_Vector(vDir, TimeDelta * 0.4f);
	}
		
	m_bFocusing = false;
	if (4.5f < fTime)
	{
		m_bBossScene = false;
		GM->Set_CamaeraSceneEnd(true);
		GM->Set_bCameraMove(false);
		GM->Get_Player()->Set_ControlMode(true);
		GM->Get_Camera()->Set_BossScene(false);
		GM->PlaySpeedControl(1.f);
		m_bFocusing = true;




		Start_CameraWork(WORK_NIGHTMARE_FOCUS);
	}
}

void CCamera_Perspective::Rapunzel_StartScene(_double TimeDelta)
{
	static _float	fTime = 0.f;
	fTime += (_float)TimeDelta;

	_matrix	WorldMatrix;
	if (3.5f > fTime)
	{
		WorldMatrix.r[2] = XMVector3Normalize(__vector(0.67f, -0.71f, 0.2f, 0.f));
		WorldMatrix.r[0] = XMVector3Cross(__vector(0.f, 1.f, 0.f, 0.f), WorldMatrix.r[2]);
		WorldMatrix.r[1] = XMVector3Cross(WorldMatrix.r[2], WorldMatrix.r[0]);

		WorldMatrix.r[3] = __vector(-28.36f, 34.62f, 28.92f, 1.f);

		WorldMatrix.r[3] += WorldMatrix.r[0] * fTime;
	}
	else
	{
		WorldMatrix.r[2] = XMVector3Normalize(__vector(0.55f, -0.65f, 0.51f, 0.f));
		WorldMatrix.r[0] = XMVector3Cross(__vector(0.f, 1.f, 0.f, 0.f), WorldMatrix.r[2]);
		WorldMatrix.r[1] = XMVector3Cross(WorldMatrix.r[2], WorldMatrix.r[0]);

		WorldMatrix.r[3] = __vector(-110.39f, 23.12f, 23.69f, 1.f);

		WorldMatrix.r[3] -= WorldMatrix.r[0] * fTime;
	}


	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	GM->Get_Renderer()->Set_FocusPosition(*m_pTransformCom->Get_PositionFloat4_Pointer());

	if (7.f < fTime)
	{
		PM->CameraScene_End();
		m_pTransformCom->MakeSameDirection(GM->Get_Player()->Get_Transform()->Get_NLook());
	}
}

void CCamera_Perspective::Camera_Work(_double TimeDelta)
{
	switch (m_eCameraWork)
	{
	case WORK_OPENBOX:
		OpenBox(TimeDelta);
		break;
	case WORK_RAGE_TRANS:
		Rage_Trans(TimeDelta);
		break;
	case WORK_RAGE_ATTACKEND:
		Rage_AttackEnd(TimeDelta);
		break;
	case WORK_RAGE_FINISHATTACK:
		Rage_FinishAttack(TimeDelta);
		break;
	case WORK_HAMMER_TRANS:
		Hammer_Trans(TimeDelta);
		break;
	case WORK_HAMMER_ATTACKEND:
		Hammer_AttackEnd(TimeDelta);
		break;
	case WORK_HAMMER_AIRATTACKEND:
		Hammer_AirAttackEnd(TimeDelta);
		break;
	case WORK_HAMMER_FINISHATTACK:
		Hammer_FinishAttack(TimeDelta);
		break;
	case WORK_RAPUNZEL_PORTAL:
		CreatePortal(TimeDelta);
		break;
	case WORK_RAPUNZEL_NOBODY_APPEAR:
		Nobody_Appear(TimeDelta);
		break;
	case WORK_BIGTREE_APPEAR:
		BigTree_Appear(TimeDelta);
		break;
	case WORK_BIGTREE_CLIMB:
		BigTree_Climb(TimeDelta);
		break;
	case WORK_RAPUNZELBOSS_PORTAL:
	case WORK_FROZEN_PORTAL:
	case WORK_KEYGRAVE_PORTAL:
		CreatePortal(TimeDelta);
		break;
	case WORK_RETURNTOPLAYER:
		ReturnToPlayer(TimeDelta);
		break;
	case WORK_RETURNTOPLAYERBACK:
		ReturnToPlayerBack(TimeDelta);
		break;
	case WORK_NIGHTMARE_FOCUS:
		Nightmare_Focus(TimeDelta);
		break;
	case WORK_ICEMAN_APPEAR:
		IceMan_Appear(TimeDelta);
		break;
	case WORK_FROZENWOLF_APPEAR:
		FrozenWolf_Appear(TimeDelta);
		break;
	case WORK_FROZENWOLF_RAIN:
		FrozenWolf_Rain(TimeDelta);
		break;
	case WORK_GOAT_APPEAR:
		Goat_Appear(TimeDelta);
		break;
	case WORK_GOAT_PILLARFIRE:
		Goat_PillarFire(TimeDelta);
		break;
	case WORK_GOAT_PILLARROT:
		Goat_PillarRot(TimeDelta);
		break;
	}
}

void CCamera_Perspective::OpenBox(_double TimeDelta)
{
	m_fCameraWorkTime += (_float)TimeDelta;
	CTransform* pPlayerTransform = GM->Get_Player()->Get_Transform();
	_vector		vPlayerPosition = pPlayerTransform->Get_Position();
	_vector		vPlayerUp = pPlayerTransform->Get_NUp();
	_vector		vPlayerRight = pPlayerTransform->Get_NRight();

	if (0.35f > m_fCameraWorkTime)
	{
		m_pTransformCom->Revolution(vPlayerUp, vPlayerPosition, -TimeDelta * 0.5f);
		m_pTransformCom->Go_Up(TimeDelta * 0.1f);

		m_vLookAtPoint += (vPlayerRight * 0.5f - vPlayerUp * 0.2f) * (_float)TimeDelta;
		m_pTransformCom->LookAt(m_vLookAtPoint);
		m_fLookAtPointY = vecy(m_vLookAtPoint);
	}
	else if (0.5f > m_fCameraWorkTime)
	{
		m_pTransformCom->Revolution(vPlayerUp, vPlayerPosition, -TimeDelta* 2.f);
		vecy(m_vLookAtPoint) = m_fLookAtPointY + EaseOutQuart(2.3f, m_fCameraWorkTime - 0.35f, 0.15f);
		m_pTransformCom->LookAt(m_vLookAtPoint);
	}
	else if (0.7f > m_fCameraWorkTime)
	{
		m_pTransformCom->Revolution(vPlayerUp, vPlayerPosition, -TimeDelta * 1.f);
		vecy(m_vLookAtPoint) = m_fLookAtPointY + (2.3f - EaseOutQuart(0.4f, m_fCameraWorkTime - 0.5f, 0.2f));
		m_pTransformCom->LookAt(m_vLookAtPoint);
	}
	else if (1.f > m_fCameraWorkTime)
	{
		if (false == m_bCameraWorkPoint)
		{
			m_pTransformCom->Set_Position(pPlayerTransform->Get_Position() - pPlayerTransform->Get_NLook() * 1.6f - pPlayerTransform->Get_NRight() * 1.3f + pPlayerTransform->Get_NUp() * 1.3f);
			m_vLookAtPoint = pPlayerTransform->Get_Position() + pPlayerTransform->Get_NLook() * 0.5f + pPlayerTransform->Get_NUp() * 1.5f;
			m_pTransformCom->LookAt(m_vLookAtPoint);
			m_bCameraWorkPoint = true;
		}
		m_pTransformCom->Revolution(vPlayerUp, vPlayerPosition, -TimeDelta * 0.05f);
		m_pTransformCom->LookAt(m_vLookAtPoint);
	}
	else if (2.8f > m_fCameraWorkTime)
	{		
	}
	else
	{
		m_bStartCameraWork = false;
		m_bFocusing = true;
		m_fCameraWorkTime = 0.f;
		m_bCameraWorkPoint = false;

		_vector	vLook = pPlayerTransform->Get_Look();
		vecy(vLook) -= 0.3f;
		m_pTransformCom->MakeSameDirection(vLook);
	}
}

void CCamera_Perspective::Rage_Trans(_double TimeDelta)
{
	m_fCameraWorkTime += (_float)TimeDelta;
	CTransform* pPlayerTransform = GM->Get_Player()->Get_Transform();
	_vector		vPlayerPosition = pPlayerTransform->Get_Position();
	_vector		vPlayerUp = pPlayerTransform->Get_NUp();
	_vector		vPlayerRight = pPlayerTransform->Get_NRight();

	if (0.4f > m_fCameraWorkTime)
	{
		m_pTransformCom->Revolution(vPlayerUp, vPlayerPosition, -TimeDelta);
		m_pTransformCom->Go_Up(TimeDelta * 0.1f);	
		vecy(m_vLookAtPoint) = m_fLookAtPointY 
			+ EaseOutQuart(1.5f, m_fCameraWorkTime, 0.4f);
		m_pTransformCom->LookAt(m_vLookAtPoint);
	}
	else if (0.6f > m_fCameraWorkTime)
	{
		m_pTransformCom->Revolution(vPlayerUp, vPlayerPosition, -TimeDelta);
		m_pTransformCom->Go_Up(TimeDelta * 0.15f);
		m_pTransformCom->Go_Backward(TimeDelta * 0.08f);
		vecy(m_vLookAtPoint) = m_fLookAtPointY 
			+ (1.5f - EaseOutQuart(2.3f, m_fCameraWorkTime - 0.4f, 0.2f));
		m_pTransformCom->LookAt(m_vLookAtPoint);
	}
	else if (0.8f > m_fCameraWorkTime)
	{
		m_pTransformCom->Revolution(vPlayerUp, vPlayerPosition, -TimeDelta * 0.8f);
		m_pTransformCom->Go_Up(TimeDelta * 0.2f);
		m_pTransformCom->Go_Backward(TimeDelta * 0.4f);
		m_pTransformCom->LookAt(m_vLookAtPoint);
	}
	else if (1.4f > m_fCameraWorkTime)
	{
		m_pTransformCom->Revolution(vPlayerUp, vPlayerPosition, -TimeDelta * 0.5f);
		m_pTransformCom->Go_Up(TimeDelta * 0.1f);
		m_pTransformCom->Go_Backward(TimeDelta * 0.05f);
		vecy(m_vLookAtPoint) = m_fLookAtPointY + (-0.8f + EaseOutQuart(2.2f, m_fCameraWorkTime - 0.8f, 0.6f));
		m_pTransformCom->LookAt(m_vLookAtPoint);
	}
	else if (1.5f > m_fCameraWorkTime)
	{
		m_pTransformCom->Revolution(vPlayerUp, vPlayerPosition, -TimeDelta * 2.f);
		m_pTransformCom->Go_Up(-TimeDelta * 0.4f);
		m_pTransformCom->Go_Backward(TimeDelta * 0.3f);
		vecy(m_vLookAtPoint) = m_fLookAtPointY + (1.4f + EaseOutQuart(0.4f, m_fCameraWorkTime - 1.4f, 0.1f));
		m_pTransformCom->LookAt(m_vLookAtPoint);
		m_vSavePos = m_pTransformCom->Get_Position();
	}
	else if (1.6f > m_fCameraWorkTime)
	{
		m_pTransformCom->Go_Up(-TimeDelta * 0.01f);
		m_pTransformCom->Set_Position(m_vSavePos + XMVector3Normalize(m_pTransformCom->Get_Look()) * EaseOutQuart(1.5f, m_fCameraWorkTime - 1.5f, 0.1f));
		m_pTransformCom->LookAt(m_vLookAtPoint);
		if (false == m_bCameraWorkPoint)
		{
			GM->Use_RadialBlur(true, 1.5f, 0.7f, 0.5f, 0.25f);
			m_bCameraWorkPoint = true;
		}
	}
	else if (1.9f > m_fCameraWorkTime)
	{
		m_pTransformCom->LookAt(m_vLookAtPoint);
	}
	else if (2.1f > m_fCameraWorkTime)
	{
		vecy(m_vLookAtPoint) = m_fLookAtPointY + (1.8f - EaseOutQuart(0.5f, m_fCameraWorkTime - 1.9f, 0.2f));
		m_pTransformCom->LookAt(m_vLookAtPoint);
	}
	else
	{
		m_bStartCameraWork = false;
		m_bFocusing = true;
		m_fCameraWorkTime = 0.f;
		m_bCameraWorkPoint = false;

		_vector	vLook = m_pTargetTransform->Get_Look();
		vecy(vLook) -= 0.3f;
		m_pTransformCom->MakeSameDirection(vLook);
	}
}

void CCamera_Perspective::Rage_AttackEnd(_double TimeDelta)
{
	m_fCameraWorkTime += (_float)TimeDelta;
	CTransform* pPlayerTransform = GM->Get_Player()->Get_Transform();
	_vector		vPlayerPosition = pPlayerTransform->Get_Position();
	_vector		vPlayerUp = pPlayerTransform->Get_NUp();
	_vector		vPlayerRight = pPlayerTransform->Get_NRight();
	_vector		vPlayerLook = pPlayerTransform->Get_NLook();

	if (0.8f > m_fCameraWorkTime)
	{
		m_pTransformCom->Revolution(vPlayerUp, vPlayerPosition, TimeDelta * 0.3f);
		m_pTransformCom->Go_Backward(TimeDelta * 0.3f);
		m_pTransformCom->Go_Up(TimeDelta * 0.3f);
		m_vLookAtPoint = pPlayerTransform->Get_Position() + pPlayerTransform->Get_NUp() * 1.f;

		vecy(m_vLookAtPoint) = m_fLookAtPointY + EaseOutQuart(0.2f, m_fCameraWorkTime, 0.8f);
		m_pTransformCom->LookAt(m_vLookAtPoint);
	}
	else if (0.9f > m_fCameraWorkTime)
	{
		m_pTransformCom->Revolution(vPlayerUp, vPlayerPosition, TimeDelta * 0.3f);
		m_vLookAtPoint = pPlayerTransform->Get_Position() + pPlayerTransform->Get_NUp() * 1.f;

		vecy(m_vLookAtPoint) = m_fLookAtPointY + 0.2f + EaseOutQuart(1.5f, m_fCameraWorkTime - 0.8f, 0.2f);
		m_pTransformCom->LookAt(m_vLookAtPoint);
		m_vSavePos = m_pTransformCom->Get_Position();
	}
	else if (1.2f > m_fCameraWorkTime)
	{
		_vector	vPos = vPlayerPosition - vPlayerLook * 1.f + vPlayerRight * 0.3f + __vector(0.f, 1.f, 0.f, 0.f) * 1.f;
		_vector vLookPoint = vPlayerPosition + vPlayerRight * 0.3f + __vector(0.f, 1.f, 0.f, 0.f) * 1.f;

		m_pTransformCom->Set_Position(XMVectorLerp(m_vSavePos, vPos, (m_fCameraWorkTime - 0.9f) / 0.3f));
		m_pTransformCom->LookAt(XMVectorLerp(m_vLookAtPoint, vLookPoint, (m_fCameraWorkTime - 0.9f) / 0.3f));
	}
	else if (1.3f > m_fCameraWorkTime)
	{
	}
	else if (1.5f > m_fCameraWorkTime)
	{
		_float fSpeed = 3.f - EaseInQuart(3.f, m_fCameraWorkTime - 1.3f, 0.2f);

		m_pTransformCom->Go_Backward(TimeDelta * fSpeed);
		m_pTransformCom->Go_Right(TimeDelta * fSpeed * 0.3f);
		m_pTransformCom->Go_Up(TimeDelta * fSpeed * 0.3f);
	}
	else if (1.6f > m_fCameraWorkTime)
	{
	}
	else
	{
		if (true == m_bFinishAttack)
		{
			Start_CameraWork(CCamera_Perspective::WORK_RAGE_FINISHATTACK);
			m_bFinishAttack = false;
		}
		else
		{
			m_bStartCameraWork = false;
			m_bFocusing = true;
		}
	}
}

void CCamera_Perspective::Rage_FinishAttack(_double TimeDelta)
{
	m_fCameraWorkTime += (_float)TimeDelta;
	CTransform* pPlayerTransform = GM->Get_Player()->Get_Transform();
	_vector		vPlayerPosition = pPlayerTransform->Get_Position();
	_vector		vPlayerUp = pPlayerTransform->Get_NUp();
	_matrix WorldMatrix = m_pTargetBone->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&GM->Get_Player()->Get_Model()->Get_TransformMatrix()) * GM->Get_Player()->Get_Transform()->Get_WorldMatrix();

	if (1.f > m_fCameraWorkTime)
	{
		m_pTransformCom->Go_Backward(TimeDelta * 0.2f);
		m_pTransformCom->Go_Up(TimeDelta * 0.1f);
		m_vLookAtPoint = WorldMatrix.r[3] + __vector(0.f, -0.8f, 0.f, 0.f);
		m_fLookAtPointY = vecy(m_vLookAtPoint);
		m_pTransformCom->LookAt(m_vLookAtPoint);
	}
	else if (1.2f > m_fCameraWorkTime)
	{
		_float fSpeed = 5.f - EaseInQuart(5.f, m_fCameraWorkTime - 1.f, 0.2f);

		m_pTransformCom->Go_Backward(TimeDelta * fSpeed);
		m_pTransformCom->Go_Vector(__vector(0.f, 1.f, 0.f, 0.f), TimeDelta * fSpeed * 0.3f);
		m_vLookAtPoint = WorldMatrix.r[3] + __vector(0.f, -0.8f, 0.f, 0.f);
		m_fLookAtPointY = vecy(m_vLookAtPoint);
		m_pTransformCom->LookAt(m_vLookAtPoint);
	}
	else if (2.7f > m_fCameraWorkTime)
	{
		if (false == m_bCameraWorkPoint)
		{
			Set_HorizonShake(1.5f, 1.f, 0.5f);
			m_bCameraWorkPoint = true;
		}
		m_pTransformCom->Revolution(__vector(0.f, 1.f, 0.f, 0.f), vPlayerPosition, TimeDelta * 2.f);
		m_vLookAtPoint = WorldMatrix.r[3] + __vector(0.f, -0.8f, 0.f, 0.f);
		m_fLookAtPointY = vecy(m_vLookAtPoint);
		m_pTransformCom->LookAt(m_vLookAtPoint);
	}
	else
	{
		m_pTargetBone = nullptr;
		m_bStartCameraWork = false;
		m_bFocusing = true;
		m_fCameraWorkTime = 0.f;
		m_bCameraWorkPoint = false;

		_vector	vLook = pPlayerTransform->Get_Look();
		vecy(vLook) -= 0.3f;
		m_pTransformCom->MakeSameDirection(vLook);
	}
}

void CCamera_Perspective::Hammer_Trans(_double TimeDelta)
{
	m_fCameraWorkTime += (_float)TimeDelta;

	CTransform* pPlayerTransform = GM->Get_Player()->Get_Transform();


	if (0.3f > m_fCameraWorkTime)
	{
		_float fSpeed = 0.75f - EaseInQuart(0.75f, m_fCameraWorkTime, 0.3f);

		m_pTransformCom->Turn(__vector(0.f, 1.f, 0.f, 0.f), TimeDelta * fSpeed * 2.f);
		m_pTransformCom->Turn(m_pTransformCom->Get_NRight(), TimeDelta * fSpeed * 2.f);
		m_pTransformCom->Go_Backward(TimeDelta * (0.75f - fSpeed) * 5.f);
		m_pTransformCom->Go_Left(TimeDelta * (fSpeed) / 4.f);
		m_pTransformCom->Go_Vector(__vector(0.f, 1.f, 0.f, 0.f), TimeDelta * fSpeed / 2.5f);
	}
	else if (0.8f > m_fCameraWorkTime)
	{
	}
	else if (1.1f > m_fCameraWorkTime)
	{
		if (false == m_bCameraWorkPoint)
		{
			m_pTargetBone = GM->Get_Player()->Get_Model()->Find_HierarchyNode("R_buki");
			m_bCameraWorkPoint = true;
			m_vLookAtPoint = m_pTransformCom->Get_Position() + m_pTransformCom->Get_NLook() * 5.f;
		}

		m_pTransformCom->Go_Right(TimeDelta * 0.25f);
		//m_pTransformCom->Go_Straight(TimeDelta * 0.3f);
		m_pTransformCom->Go_Vector(__vector(0.f, 1.f, 0.f, 0.f), TimeDelta * 0.6f);

		_matrix WorldMatrix = m_pTargetBone->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&GM->Get_Player()->Get_Model()->Get_TransformMatrix()) * GM->Get_Player()->Get_Transform()->Get_WorldMatrix();
		m_pTransformCom->LookAt(XMVectorLerp(m_vLookAtPoint, WorldMatrix.r[3], (m_fCameraWorkTime - 0.8f) / 0.3f));
	}
	else if (1.3f > m_fCameraWorkTime)
	{
		_float fSpeed = 2.5f - EaseInQuart(2.5f, m_fCameraWorkTime - 1.1f, 0.2f);
		_vector vDir = -m_pTransformCom->Get_NLook();
		vecy(vDir) = 0.f;
		m_pTransformCom->Go_Vector(vDir, TimeDelta * fSpeed);

		_matrix WorldMatrix = m_pTargetBone->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&GM->Get_Player()->Get_Model()->Get_TransformMatrix()) * GM->Get_Player()->Get_Transform()->Get_WorldMatrix();
		m_pTransformCom->LookAt(WorldMatrix.r[3]);
	}
	else if (2.1f > m_fCameraWorkTime)
	{
		_vector		vPlayerPosition = pPlayerTransform->Get_Position();
		m_pTransformCom->Revolution(__vector(0.f, 1.f, 0.f, 0.f), vPlayerPosition, TimeDelta * 0.3f);

		_matrix WorldMatrix = m_pTargetBone->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&GM->Get_Player()->Get_Model()->Get_TransformMatrix()) * GM->Get_Player()->Get_Transform()->Get_WorldMatrix();
		m_pTransformCom->LookAt(WorldMatrix.r[3]);

		m_vSavePos = m_pTransformCom->Get_Position();
	}
	else if (2.4f > m_fCameraWorkTime)
	{
		_vector		vPosition = pPlayerTransform->Get_Position() + __vector(0.f, 1.f, 0.f, 0.f) + pPlayerTransform->Get_NLook() * 1.7f;
		m_pTransformCom->Set_Position(XMVectorLerp(m_vSavePos, vPosition, (m_fCameraWorkTime - 2.1f) / 0.3f));

		_matrix WorldMatrix = m_pTargetBone->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&GM->Get_Player()->Get_Model()->Get_TransformMatrix()) * GM->Get_Player()->Get_Transform()->Get_WorldMatrix();
		m_pTransformCom->LookAt(WorldMatrix.r[3]);
	}

	else if (3.f > m_fCameraWorkTime)
	{
	}
	else
	{
		m_pTargetBone = nullptr;
		m_bStartCameraWork = false;
		m_bFocusing = true;
		m_fCameraWorkTime = 0.f;
		m_bCameraWorkPoint = false;

		_vector	vLook = pPlayerTransform->Get_Look();
		vecy(vLook) -= 0.3f;
		m_pTransformCom->MakeSameDirection(vLook);
	}
}

void CCamera_Perspective::Hammer_AttackEnd(_double TimeDelta)
{
	m_fCameraWorkTime += (_float)TimeDelta;
	CTransform* pPlayerTransform = GM->Get_Player()->Get_Transform();

	if (0.15f > m_fCameraWorkTime)
	{
		m_pTransformCom->Go_Backward((_float)TimeDelta * 0.26f);
		vecy(m_vLookAtPoint) -= (_float)TimeDelta * 0.26f;
		m_fLookAtPointY = vecy(m_vLookAtPoint);
		m_pTransformCom->LookAt(m_vLookAtPoint);
	}
	else if (0.35f > m_fCameraWorkTime)
	{
		m_pTransformCom->Go_Backward(TimeDelta * 0.5f);
		m_pTransformCom->Go_Up(TimeDelta * 1.3f);
		vecy(m_vLookAtPoint) += (_float)TimeDelta * 4.f;
		m_fLookAtPointY = vecy(m_vLookAtPoint);
		m_pTransformCom->LookAt(m_vLookAtPoint);
	}
	else if (0.55f > m_fCameraWorkTime)
	{

	}
	else if (0.75f > m_fCameraWorkTime)
	{
		if (false == m_bCameraWorkPoint)
		{
			m_vSavePos = pPlayerTransform->Get_Position() + pPlayerTransform->Get_NLook() * 1.f + pPlayerTransform->Get_NUp() * 1.5f;
			m_bCameraWorkPoint = true;
		}
		m_pTransformCom->Revolution(__vector(0.f, 1.f, 0.f, 0.f), pPlayerTransform->Get_Position(), TimeDelta * 3.f);
		m_pTransformCom->Go_Vector(__vector(0.f, 1.f, 0.f, 0.f) ,TimeDelta * -2.f);

		m_pTransformCom->LookAt(XMVectorLerp(m_vLookAtPoint, m_vSavePos, (m_fCameraWorkTime - 0.55f) / 0.2f));
	}
	else if (1.35f > m_fCameraWorkTime)
	{
		if (true == m_bCameraWorkPoint)
		{
			m_vLookAtPoint = m_vSavePos;
			m_vSavePos = m_pTransformCom->Get_Position();
			m_vSaveSecondPos = pPlayerTransform->Get_Position() - pPlayerTransform->Get_NLook() * 0.5f + pPlayerTransform->Get_NUp() * 3.f + pPlayerTransform->Get_NRight() * 1.f;
			m_bCameraWorkPoint = false;
		}

		m_pTransformCom->LookAt(XMVectorLerp(m_vLookAtPoint, static_cast<CCollider*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_PlayerWeapon"), TEXT("Com_HammerCollider_Last")))->Get_Center(), (m_fCameraWorkTime - 0.75f) / 0.6f));
		m_pTransformCom->Set_Position(XMVectorLerp(m_vSavePos, m_vSaveSecondPos, (m_fCameraWorkTime - 0.75f) / 0.6f));
	}
	else if (2.f > m_fCameraWorkTime)
	{
	}
	else
	{
		m_pTargetBone = nullptr;
		m_bStartCameraWork = false;
		m_bFocusing = true;
		m_fCameraWorkTime = 0.f;
		m_bCameraWorkPoint = false;

		_vector	vLook = pPlayerTransform->Get_Look();
		vecy(vLook) -= 0.3f;
		m_pTransformCom->MakeSameDirection(vLook);
	}

}

void CCamera_Perspective::Hammer_AirAttackEnd(_double TimeDelta)
{
	m_fCameraWorkTime += (_float)TimeDelta;
	CTransform* pPlayerTransform = GM->Get_Player()->Get_Transform();

	if (0.15f > m_fCameraWorkTime)
	{
		m_pTransformCom->Go_Backward((_float)TimeDelta * 0.26f);
		vecy(m_vLookAtPoint) -= (_float)TimeDelta * 0.26f;
		m_fLookAtPointY = vecy(m_vLookAtPoint);
		m_pTransformCom->LookAt(m_vLookAtPoint);
	}
	else if (0.35f > m_fCameraWorkTime)
	{
		m_pTransformCom->Go_Backward(TimeDelta * 0.5f);
		m_pTransformCom->Go_Up(TimeDelta * 1.3f);
		vecy(m_vLookAtPoint) += (_float)TimeDelta * 4.f;
		m_fLookAtPointY = vecy(m_vLookAtPoint);
		m_pTransformCom->LookAt(m_vLookAtPoint);
	}
	else if (0.55f > m_fCameraWorkTime)
	{

	}
	else if (0.75f > m_fCameraWorkTime)
	{
		if (false == m_bCameraWorkPoint)
		{
			m_vSavePos = pPlayerTransform->Get_Position() + pPlayerTransform->Get_NLook() * 1.f + pPlayerTransform->Get_NUp() * 1.5f;
			m_bCameraWorkPoint = true;
		}
		m_pTransformCom->Revolution(__vector(0.f, 1.f, 0.f, 0.f), pPlayerTransform->Get_Position(), TimeDelta * 3.f);
		m_pTransformCom->Go_Vector(__vector(0.f, 1.f, 0.f, 0.f), TimeDelta * -2.f);

		m_pTransformCom->LookAt(XMVectorLerp(m_vLookAtPoint, m_vSavePos, (m_fCameraWorkTime - 0.55f) / 0.2f));
	}
	else if (1.35f > m_fCameraWorkTime)
	{
		if (true == m_bCameraWorkPoint)
		{
			m_vLookAtPoint = m_vSavePos;
			m_vSavePos = m_pTransformCom->Get_Position();
			m_vSaveSecondPos = pPlayerTransform->Get_Position() - pPlayerTransform->Get_NLook() * 0.5f + pPlayerTransform->Get_NUp() * 3.f + pPlayerTransform->Get_NRight() * 1.f;
			m_bCameraWorkPoint = false;
		}

		m_pTransformCom->LookAt(XMVectorLerp(m_vLookAtPoint, static_cast<CCollider*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_PlayerWeapon"), TEXT("Com_HammerCollider_Last")))->Get_Center(), (m_fCameraWorkTime - 0.75f) / 0.6f));
		m_pTransformCom->Set_Position(XMVectorLerp(m_vSavePos, m_vSaveSecondPos, (m_fCameraWorkTime - 0.75f) / 0.6f));
	}
	else if (2.f > m_fCameraWorkTime)
	{
	}
	else
	{
		m_pTargetBone = nullptr;
		m_bStartCameraWork = false;
		m_bFocusing = true;
		m_fCameraWorkTime = 0.f;
		m_bCameraWorkPoint = false;

		_vector	vLook = pPlayerTransform->Get_Look();
		vecy(vLook) -= 0.3f;
		m_pTransformCom->MakeSameDirection(vLook);
	}

	/*
	m_fCameraWorkTime += (_float)TimeDelta;
	CTransform* pPlayerTransform = GM->Get_Player()->Get_Transform();

	if (0.18f > m_fCameraWorkTime)
	{
	m_pTransformCom->Go_Backward((_float)TimeDelta * 0.26f);
	vecy(m_vLookAtPoint) -= (_float)TimeDelta * 0.26f;
	m_fLookAtPointY = vecy(m_vLookAtPoint);
	m_pTransformCom->LookAt(m_vLookAtPoint);
	}
	else if (0.42f > m_fCameraWorkTime)
	{
	m_pTransformCom->Go_Backward(TimeDelta * 0.5f);
	m_pTransformCom->Go_Up(TimeDelta * 1.3f);
	vecy(m_vLookAtPoint) += (_float)TimeDelta * 4.f;
	m_fLookAtPointY = vecy(m_vLookAtPoint);
	m_pTransformCom->LookAt(m_vLookAtPoint);
	}
	else if (0.66f > m_fCameraWorkTime)
	{

	}
	else if (0.90f > m_fCameraWorkTime)
	{
	if (false == m_bCameraWorkPoint)
	{
	m_vSavePos = pPlayerTransform->Get_Position() + pPlayerTransform->Get_NLook() * 1.f + pPlayerTransform->Get_NUp() * 1.5f;
	m_bCameraWorkPoint = true;
	}
	m_pTransformCom->Revolution(__vector(0.f, 1.f, 0.f, 0.f), pPlayerTransform->Get_Position(), TimeDelta * 3.f);
	m_pTransformCom->Go_Vector(__vector(0.f, 1.f, 0.f, 0.f), TimeDelta * -2.f);

	m_pTransformCom->LookAt(XMVectorLerp(m_vLookAtPoint, m_vSavePos, (m_fCameraWorkTime - 0.66f) / 0.24f));
	}
	else if (1.62f > m_fCameraWorkTime)
	{
	if (true == m_bCameraWorkPoint)
	{
	m_vLookAtPoint = m_vSavePos;
	m_vSavePos = m_pTransformCom->Get_Position();
	m_vSaveSecondPos = pPlayerTransform->Get_Position() - pPlayerTransform->Get_NLook() * 0.5f + pPlayerTransform->Get_NUp() * 3.f + pPlayerTransform->Get_NRight() * 1.f;
	m_bCameraWorkPoint = false;
	}

	m_pTransformCom->LookAt(XMVectorLerp(m_vLookAtPoint, static_cast<CCollider*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_PlayerWeapon"), TEXT("Com_HammerCollider_Last")))->Get_Center(), (m_fCameraWorkTime - 0.90f) / 0.72f));
	m_pTransformCom->Set_Position(XMVectorLerp(m_vSavePos, m_vSaveSecondPos, (m_fCameraWorkTime - 0.90f) / 0.72f));
	}
	else if (2.4f > m_fCameraWorkTime)
	{
	}
	else
	{
	m_pTargetBone = nullptr;
	m_bStartCameraWork = false;
	m_bFocusing = true;
	m_fCameraWorkTime = 0.f;
	m_bCameraWorkPoint = false;

	_vector	vLook = pPlayerTransform->Get_Look();
	vecy(vLook) -= 0.3f;
	m_pTransformCom->MakeSameDirection(vLook);
	}
	*/
}

void CCamera_Perspective::Hammer_FinishAttack(_double TimeDelta)
{

}


void CCamera_Perspective::CreatePortal(_double TimeDelta)
{
	m_fCameraWorkTime += (_float)TimeDelta;

	if (false == m_bCameraWorkPoint && 2.f < m_fCameraWorkTime)
	{
		CGameObject* pPortal = m_pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Portal"), TEXT("Prototype_GameObject_Portal"));
		CTransform*	pPortalTransform = pPortal->Get_Transform();

		if (LEVEL_RAPUNZEL == GM->Get_CurrentLevel())
		{
			pPortalTransform->Set_Position(__vector(-116.61f, -2.55f, 130.84f, 1.f));
			pPortalTransform->MakeSameDirection(__vector(0.417f, 0.f, -0.908f, 0.f));
		}
		else if (LEVEL_RAPUNZEL_BOSS == GM->Get_CurrentLevel())
		{
			pPortalTransform->Set_Position(__vector(128.77f, 3.1f, 160.9f, 1.f));
			pPortalTransform->MakeSameDirection(__vector(0.f, 0.f, 1.f, 0.f));
		}

		m_bCameraWorkPoint = true;
	}

	if (4.f < m_fCameraWorkTime)
	{
		m_bStartCameraWork = false;
		m_bFocusing = true;
		m_fCameraWorkTime = 0.f;
		m_bCameraWorkPoint = false;

		if (LEVEL_RAPUNZEL <= GM->Get_CurrentLevel() && LEVEL_KEYGRAVE >= GM->Get_CurrentLevel())
		{
			_int iComTex = 0;
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_UI_Tip_Down"), &iComTex)))
			{
				BREAKPOINT;
				return;
			}

			iComTex = 5;
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_UI_Tip_Down"), &iComTex)))
			{
				BREAKPOINT;
				return;
			}
		}

		GM->Set_UIRender(true);
	}
}

void CCamera_Perspective::Nobody_Appear(_double TimeDelta)
{
	m_fCameraWorkTime += (_float)TimeDelta;

	if (10.f > m_fCameraWorkTime)
	{
		m_pTransformCom->Set_Position(XMVectorLerp(m_vSavePos, __vector(-101.909f, -1.910f, 75.610f, 1.f), m_fCameraWorkTime / 10.f));
		m_pTransformCom->MakeSameDirection(XMVectorLerp(m_vLookAtPoint, XMVector3Normalize(__vector(-0.235f, 0.209f, 0.949f, 0.f)), m_fCameraWorkTime / 10.f));
	}
	else
	{
		m_bStartCameraWork = false;
		m_bFocusing = true;
		m_fCameraWorkTime = 0.f;
		m_bCameraWorkPoint = false;
		GM->Get_Player()->Set_ControlMode(true);
		GM->Set_UIRender(true);
		// UI 생성
		_int iComTex = 0;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_UI_Tip_Down"), &iComTex)))
		{
			BREAKPOINT;
			return;
		}

		iComTex = 7;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_UI_Tip_Down"), &iComTex)))
		{
			BREAKPOINT;
			return;
		}
		Start_CameraWork(WORK_RETURNTOPLAYER);
	}
}

void CCamera_Perspective::BigTree_Appear(_double TimeDelta)
{
	m_fCameraWorkTime += (_float)TimeDelta;

	GM->Get_Player()->Get_Transform()->LookAtXZ(m_pNpcTransform);

	if (1.25f > m_fCameraWorkTime)
	{
		GM->Set_UIRender(false);
		if (false == m_bCameraWorkPoint)
		{
			m_pTransformCom->Set_Position(__vector(119.236f, 25.506f, 105.375f, 1.f));
			m_pTransformCom->MakeSameDirection(__vector(0.511f, -0.363f, 0.778f, 0.f));
			m_bCameraWorkPoint = true;
		}
		m_pTransformCom->Go_Down(TimeDelta * 0.6f);
	}
	else if (2.5f > m_fCameraWorkTime)
	{
		if (true == m_bCameraWorkPoint)
		{
			m_pTransformCom->Set_Position(__vector(129.028f, 8.274f, 126.966f, 1.f));
			m_pTransformCom->MakeSameDirection(__vector(0.054f, 0.073f, -0.996f, 0.f));
			m_bCameraWorkPoint = false;
		}
		m_pTransformCom->Go_Down(TimeDelta * 0.24f);
		m_pTransformCom->Go_Backward(TimeDelta * 0.6f);
	}
	else if (3.75f > m_fCameraWorkTime)
	{
		static _bool bShake = false;
		if (false == bShake)
		{
			Set_HorizonShake(1.f, 0.5f, 1.f);
			GM->Use_RadialBlur(true, 0.5f, 1.f, 0.75f, 0.3f);
			bShake = true;
			m_vSavePos = m_pTransformCom->Get_Position();
		}
		m_pTransformCom->Set_Position(m_vSavePos + -XMVector3Normalize(m_pTransformCom->Get_Look()) * EaseOutQuart(15.f, m_fCameraWorkTime - 2.5f, 1.25f));
	}
	else if (4.05f > m_fCameraWorkTime)
	{
		if (false == m_bCameraWorkPoint)
		{
			GM->PlaySpeedControl(0.f);
			CTransform* pBigTreeTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_RAPUNZEL_BOSS, TEXT("Layer_BigTree"), TEXT("Com_Transform")));
			m_vSaveSecondPos = pBigTreeTransform->Get_Position();
			m_vSaveSecondPos += (__vector(0.f, 7.f, 0.f, 0.f) + pBigTreeTransform->Get_NLook() * 10.f);
			m_vLookAtSecondPoint = XMVector3Normalize((__vector(0.f, 7.f, 0.f, 0.f) + pBigTreeTransform->Get_Position()) - m_vSaveSecondPos);

			m_vSavePos = m_pTransformCom->Get_Position();
			m_vLookAtPoint = m_pTransformCom->Get_NLook();
			m_bCameraWorkPoint = true;
		}
		_float fLerp = EaseOutQuart(1.f, m_fCameraWorkTime - 3.75f, 0.3f);
		m_pTransformCom->Set_Position(XMVectorLerp(m_vSavePos, m_vSaveSecondPos, fLerp));
		m_pTransformCom->MakeSameDirection(XMVectorLerp(m_vLookAtPoint, m_vLookAtSecondPoint, fLerp));


	}
	else if (6.05f > m_fCameraWorkTime)
	{
		if (true == m_bCameraWorkPoint)
		{
			/* BOSS NAME ANIMATION */
			GM->Set_BossName(true);
			/* =================== */
			m_bCameraWorkPoint = false;
		}

	}
	else
	{
		m_bStartCameraWork = false;
		m_bFocusing = true;
		m_fCameraWorkTime = 0.f;
		m_bCameraWorkPoint = false;
		GM->PlaySpeedControl(1.f);
		GM->Set_UIRender(true);
		Start_CameraWork(WORK_RETURNTOPLAYER);
	}
}

void CCamera_Perspective::BigTree_Climb(_double TimeDelta)
{
	m_fCameraWorkTime += (_float)TimeDelta;

	if (0.416f > m_fCameraWorkTime)
	{
		if (false == m_bCameraWorkPoint)
		{
			m_pTransformCom->Set_Position(__vector(128.323f, 50.94f, 159.254f, 1.f));
			m_pTransformCom->MakeSameDirection(__vector(0.058f, 0.f, 0.986f, 0.f));
			m_pTransformCom->Set_MoveBasedOnCurrentLook(-2.f);
			m_vLookAtPoint = __vector(128.323f, 50.94f, 159.254f, 1.f) + XMVector3Normalize(__vector(0.058f, 0.f, 0.986f, 0.f)) * 10.f;
			m_bCameraWorkPoint = true;
		}
	}
	else if (2.125f > m_fCameraWorkTime)
	{

		m_pTransformCom->Go_Up(TimeDelta * 0.12f);
		m_pTransformCom->Turn(m_pTransformCom->Get_NRight(), -TimeDelta * EaseOutQuart(0.15f, m_fCameraWorkTime - 0.416f, 1.709f));
	}
	else if (2.416f > m_fCameraWorkTime)
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_NRight(), TimeDelta * 0.12f);
		m_pTransformCom->Turn(__vector(0.f, 1.f, 0.f, 0.f), TimeDelta * 0.036f);
	}
	else if (3.f > m_fCameraWorkTime)
	{
	}
	else
	{
		m_bStartCameraWork = false;
		m_bFocusing = true;
		m_fCameraWorkTime = 0.f;
		m_bCameraWorkPoint = false;
		GM->Set_UIRender(true);
	}
}

void CCamera_Perspective::Nightmare_Focus(_double TimeDelta) /* 나이트메어 줌인 2초 정지 */
{
	m_fCameraWorkTime += (_float)TimeDelta;

	if (0.3f > m_fCameraWorkTime)
	{
		_float fLerp = EaseOutQuart(1.f, m_fCameraWorkTime, 0.3f);
		m_pTransformCom->Set_Position(XMVectorLerp(m_vSavePos, m_vSaveSecondPos, fLerp));
		m_pTransformCom->MakeSameDirection(XMVectorLerp(m_vLookAtPoint, m_vLookAtSecondPoint, fLerp));
	}
	else if (2.3f > m_fCameraWorkTime)
	{
		if (false == m_bCameraWorkPoint)
		{
			/* BOSS NAME ANIMATION */
			GM->Set_BossName(true);
			/* =================== */

			GM->PlaySpeedControl(0.f);
			m_bCameraWorkPoint = true;
		}
	}
	else
	{
		m_bStartCameraWork = false;
		m_bFocusing = true;
		m_fCameraWorkTime = 0.f;
		m_bCameraWorkPoint = false;
		GM->PlaySpeedControl(1.f);
		Start_CameraWork(WORK_RETURNTOPLAYER);
	}
}

void CCamera_Perspective::ReturnToPlayer(_double TimeDelta)
{
	m_fCameraWorkTime += (_float)TimeDelta;
	m_fLookAtPointY = EaseOutQuad(1.f, m_fCameraWorkTime, 0.5f);

	if (0.5f >= m_fCameraWorkTime)
	{
		m_pTransformCom->Set_Position(XMVectorLerp(m_vSavePos, m_vSaveSecondPos, m_fLookAtPointY));
	}
	else
	{
		m_bStartCameraWork = false;
		m_bFocusing = true;
		m_fCameraWorkTime = 0.f;
		m_bCameraWorkPoint = false;
		m_fLookAtPointY = 0.f;
	}
}

void CCamera_Perspective::ReturnToPlayerBack(_double TimeDelta)
{
	m_fCameraWorkTime += (_float)TimeDelta;
	m_fLookAtPointY = EaseOutQuad(1.f, m_fCameraWorkTime, 0.5f);

	if (0.5f >= m_fCameraWorkTime)
	{
		m_pTransformCom->Set_Position(XMVectorLerp(m_vSavePos, m_vSaveSecondPos, m_fLookAtPointY));
		m_pTransformCom->MakeSameDirection(XMVectorLerp(m_vLookAtPoint, m_pTargetTransform->Get_NLook(), m_fLookAtPointY));
	}
	else
	{
		m_bStartCameraWork = false;
		m_bFocusing = true;
		m_fCameraWorkTime = 0.f;
		m_bCameraWorkPoint = false;
		m_fLookAtPointY = 0.f;
	}
}


void CCamera_Perspective::IceMan_Appear(_double TimeDelta)
{
	m_fCameraWorkTime += (_float)TimeDelta;
	m_bFocusing = false;
	if (2.5f > m_fCameraWorkTime)
	{
		GM->Set_UIRender(false);
		m_pTransformCom->Go_Backward(TimeDelta * 0.1f);
	}
	else if (2.7f > m_fCameraWorkTime)
	{
		if (false == m_bCameraWorkPoint)
		{
			CTransform* pIceManTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel() , TEXT("Layer_IceMan"), TEXT("Com_Transform")));

			m_vSavePos = m_pTransformCom->Get_Position();
			m_vSaveSecondPos = pIceManTransform->Get_Position() + __vector(0.f, 2.3f, 0.f, 0.f) + pIceManTransform->Get_NLook() * 18.f;

			m_vLookAtPoint = m_pTransformCom->Get_NLook();
			m_vLookAtSecondPoint = XMVector3Normalize(pIceManTransform->Get_Position() + __vector(0.f, 2.3f, 0.f, 0.f) - m_vSaveSecondPos);

			m_bCameraWorkPoint = true;
		}

		m_pTransformCom->Set_Position(XMVectorLerp(m_vSavePos, m_vSaveSecondPos, (m_fCameraWorkTime - 2.5f) / 0.2f));
		m_pTransformCom->MakeSameDirection(XMVectorLerp(m_vLookAtPoint, m_vLookAtSecondPoint, (m_fCameraWorkTime - 2.5f) / 0.2f));
	}
	else if (4.9f > m_fCameraWorkTime)
	{
		m_pTransformCom->Go_Straight(TimeDelta * 0.1f);
	}
	else if (5.2f > m_fCameraWorkTime)
	{
		if (true == m_bCameraWorkPoint)
		{
			CTransform* pIceManTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_IceMan"), TEXT("Com_Transform")));

			m_vSavePos = m_pTransformCom->Get_Position();
			m_vSaveSecondPos = pIceManTransform->Get_Position() + __vector(0.f, 2.3f, 0.f, 0.f) + pIceManTransform->Get_NLook() * 8.f;
			//m_vLookAtPoint = m_pTransformCom->Get_NLook();
			//m_vLookAtSecondPoint = XMVector3Normalize(pIceManTransform->Get_Position() + __vector(0.f, 1.5f, 0.f, 0.f) - m_vSaveSecondPos);

			m_bCameraWorkPoint = false;
		}

		m_pTransformCom->Set_Position(XMVectorLerp(m_vSavePos, m_vSaveSecondPos, (m_fCameraWorkTime - 4.9f) / 0.3f));
		//m_pTransformCom->MakeSameDirection(XMVectorLerp(m_vLookAtPoint, m_vLookAtSecondPoint, (m_fCameraWorkTime - 4.9f) / 0.3f));
	}
	else if (7.2f > m_fCameraWorkTime)
	{
		if (false == m_bCameraWorkPoint)
		{
			GM->PlaySpeedControl(0.f);
			m_pGameInstance->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_Icicle"), 0.f);
			/* BOSS NAME ANIMATION */
			GM->Set_BossName(true);
			/* =================== */
			m_bCameraWorkPoint = true;
		}
	}
	else
	{
		m_bStartCameraWork = false;
		m_bFocusing = true;
		m_fCameraWorkTime = 0.f;
		m_bCameraWorkPoint = false;
		GM->PlaySpeedControl(1.f);
		GM->Get_Player()->Set_ControlMode(true);
		GM->Set_UIRender(true);
		m_pGameInstance->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_Icicle"), 1.f);
		Start_CameraWork(WORK_RETURNTOPLAYER);
	}
}

void CCamera_Perspective::FrozenWolf_Appear(_double TimeDelta)
{
	m_fCameraWorkTime += (_float)TimeDelta;

	if (2.f > m_fCameraWorkTime)
	{
		if (false == m_bCameraWorkPoint)
		{
			m_vLookAtSecondPoint = XMVector3Normalize(__vector(0.221f, 0.390f, -0.893f, 0.f));
			m_vSaveSecondPos = __vector(1.717f, 3.261f, 4.547f, 1.f);

			m_bCameraWorkPoint = true;
		}

		m_pTransformCom->MakeSameDirection(XMVectorLerp(m_vLookAtPoint, m_vLookAtSecondPoint, m_fCameraWorkTime / 2.f));
		m_pTransformCom->Set_Position(XMVectorLerp(m_vSavePos, m_vSaveSecondPos, m_fCameraWorkTime / 2.f));
	}
	else if (3.5f > m_fCameraWorkTime)
	{
		if (true == m_bCameraWorkPoint)
		{
			m_vLookAtPoint = __vector(0.775f, -0.410f, 0.479f, 0.f);
			m_vSavePos = __vector(-4.798f, 9.871f, -3.511f, 1.f);
			m_pTransformCom->MakeSameDirection(m_vLookAtPoint);
			m_pTransformCom->Set_Position(m_vSavePos);

			m_vLookAtSecondPoint = __vector(0.801f, -0.534f, -0.257f, 0.f);
			m_vSaveSecondPos = __vector(-5.799f, 9.871f, 3.265f, 1.f);

			m_bCameraWorkPoint = false;
		}

		m_pTransformCom->MakeSameDirection(XMVectorLerp(m_vLookAtPoint, m_vLookAtSecondPoint, (m_fCameraWorkTime - 2.f) / 1.5f));
		m_pTransformCom->Set_Position(XMVectorLerp(m_vSavePos, m_vSaveSecondPos, (m_fCameraWorkTime - 2.f) / 1.5f));
	}
	else if (5.f > m_fCameraWorkTime)
	{
		if (false == m_bCameraWorkPoint)
		{
			m_vSavePos = __vector(0.524f, 6.780f, 7.488f, 1.f);
			m_vLookAtPoint = m_vSavePos + __vector(0.021f, 0.423f, -0.905f, 0.f) * 4.f;
			m_pTransformCom->LookAt(m_vLookAtPoint);
			m_pTransformCom->Set_Position(m_vSavePos);

			CModel* pModel = static_cast<CModel*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_FrozenWolf"), TEXT("Com_Model")));
			m_pTargetBone = pModel->Find_HierarchyNode("ago1_spo");
			m_TargetModelTransformMatrix = XMLoadFloat4x4(&pModel->Get_TransformMatrix());
			m_pTargetTransformMatrix = static_cast<CTransform*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_FrozenWolf"), TEXT("Com_Transform")))->Get_WorldFloat4x4_Pointer();


			m_vSaveSecondPos = __vector(-4.062f, 4.619f, 11.521f, 1.f);

			m_bCameraWorkPoint = true;
		}

		_matrix WorldMatrix = m_pTargetBone->Get_CombinedTransformationMatrix() * m_TargetModelTransformMatrix * XMLoadFloat4x4(m_pTargetTransformMatrix);
		m_pTransformCom->LookAt(XMVectorLerp(m_vLookAtPoint, WorldMatrix.r[3], (m_fCameraWorkTime - 3.5f) / 1.5f));
		m_pTransformCom->Set_Position(XMVectorLerp(m_vSavePos, m_vSaveSecondPos, (m_fCameraWorkTime - 3.5f) / 1.5f));
	}
	else if (7.6f > m_fCameraWorkTime)
	{
		_matrix WorldMatrix = m_pTargetBone->Get_CombinedTransformationMatrix() * m_TargetModelTransformMatrix * XMLoadFloat4x4(m_pTargetTransformMatrix);
		m_pTransformCom->LookAt(WorldMatrix.r[3]);
	}
	else if (9.6f > m_fCameraWorkTime)
	{
		if (true == m_bCameraWorkPoint)
		{
			GM->PlaySpeedControl(0.f);
			m_pGameInstance->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_BackGround"), 0.f);
			m_pGameInstance->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_Wall"), 0.f);
			
			/* BOSS NAME ANIMATION */
			GM->Set_BossName(true);
			/* =================== */
			m_bCameraWorkPoint = false;
		}
	}
	else
	{
		m_bStartCameraWork = false;
		m_bFocusing = true;
		m_fCameraWorkTime = 0.f;
		m_bCameraWorkPoint = false;
		m_pTargetBone = nullptr;
		GM->PlaySpeedControl(1.f);
		m_pGameInstance->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_BackGround"), 1.f);
		m_pGameInstance->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_Wall"), 1.f);
		GM->Get_Player()->Set_ControlMode(true);
		GM->Set_UIRender(true);
		Start_CameraWork(WORK_RETURNTOPLAYER);
	}
}

void CCamera_Perspective::FrozenWolf_Rain(_double TimeDelta)
{
	m_fCameraWorkTime += (_float)TimeDelta;

	if (1.9f > m_fCameraWorkTime)
	{
		_matrix WorldMatrix = m_pTargetBone->Get_CombinedTransformationMatrix() * m_TargetModelTransformMatrix * XMLoadFloat4x4(m_pTargetTransformMatrix);

		m_pTransformCom->LookAt(WorldMatrix.r[3]);
	}
	else if (2.1f > m_fCameraWorkTime)
	{

	}
	else
	{
		m_bStartCameraWork = false;
		m_bFocusing = true;
		m_fCameraWorkTime = 0.f;
		m_bCameraWorkPoint = false;
		m_pTargetBone = nullptr;
		GM->Set_UIRender(true);
		GM->Get_Player()->Set_ControlMode(true);
		Start_CameraWork(WORK_RETURNTOPLAYER);
		m_pGameInstance->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_Player"), 1.f);
	}
}

void CCamera_Perspective::Goat_Appear(_double TimeDelta)
{
	m_fCameraWorkTime += (_float)TimeDelta;

	if (3.f > m_fCameraWorkTime)
	{
		m_pTransformCom->Revolution(__vector(0.f, 1.f, 0.f, 0.f), __vector(0.f, 0.f, 0.f, 1.f), TimeDelta * 0.5f);
		m_pTransformCom->LookAt(__vector(0.f, 0.f, 0.f, 1.f));
	}
	else if (6.f> m_fCameraWorkTime)
	{
		_matrix WorldMatrix = m_pTargetBone->Get_CombinedTransformationMatrix() * m_TargetModelTransformMatrix * XMLoadFloat4x4(m_pTargetTransformMatrix);
		if (false == m_bCameraWorkPoint)
		{
			_vector vLook;
			memcpy(&vLook, &m_pTargetTransformMatrix->m[2], sizeof(_float4));
			m_pTransformCom->Set_Position(WorldMatrix.r[3] + vLook * 4.f);

			m_bCameraWorkPoint = true;
		}
		//_vector vLook;
		//memcpy(&vLook, &m_pTargetTransformMatrix->m[2], sizeof(_float4));
		//m_pTransformCom->Set_Position(WorldMatrix.r[3] + vLook * 4.f);
		m_pTransformCom->LookAt(WorldMatrix.r[3]);
		m_pTransformCom->Go_Vector(__vector(0.f, 1.f, 0.f, 0.f), TimeDelta * 0.15f);
	}
	else if (6.2f > m_fCameraWorkTime)
	{
		if (true == m_bCameraWorkPoint)
		{
			m_vSavePos = m_pTransformCom->Get_Position();
			 
			m_bCameraWorkPoint = false;
		}
		_matrix WorldMatrix = m_pTargetBone->Get_CombinedTransformationMatrix() * m_TargetModelTransformMatrix * XMLoadFloat4x4(m_pTargetTransformMatrix);
		_vector vLook;
		memcpy(&vLook, &m_pTargetTransformMatrix->m[2], sizeof(_float4));

		m_pTransformCom->Set_Position(XMVectorLerp(m_vSavePos, WorldMatrix.r[3] + vLook * 3.f, (m_fCameraWorkTime - 6.f) / 0.2f));
		m_pTransformCom->LookAt(WorldMatrix.r[3]);
	}
	else if (8.2f > m_fCameraWorkTime)
	{
		if (false == m_bCameraWorkPoint)
		{
			GM->PlaySpeedControl(0.f);
			/* BOSS NAME ANIMATION */
			GM->Set_BossName(true);
			/* =================== */
			m_bCameraWorkPoint = true;
		}
	}
	else
	{
		m_bStartCameraWork = false;
		m_bFocusing = true;
		m_fCameraWorkTime = 0.f;
		m_bCameraWorkPoint = false;
		m_pTargetBone = nullptr;
		GM->PlaySpeedControl(1.f);
		GM->Get_Player()->Set_ControlMode(true);
		GM->Set_UIRender(true);
		Start_CameraWork(WORK_RETURNTOPLAYER);
	}
}

void CCamera_Perspective::Goat_PillarFire(_double TimeDelta)
{
	m_fCameraWorkTime += (_float)TimeDelta;

	if (2.2f > m_fCameraWorkTime)
	{
		_matrix WorldMatrix = m_pTargetBone->Get_CombinedTransformationMatrix() * m_TargetModelTransformMatrix * XMLoadFloat4x4(m_pTargetTransformMatrix);
		m_pTransformCom->LookAt(WorldMatrix.r[3]);
		m_pTransformCom->Go_Vector(__vector(0.f, 1.f, 0.f, 0.f), TimeDelta * 0.2f);
	}
	else if (4.2f > m_fCameraWorkTime)
	{
		_matrix WorldMatrix = m_pTargetBone->Get_CombinedTransformationMatrix() * m_TargetModelTransformMatrix * XMLoadFloat4x4(m_pTargetTransformMatrix);
		CTransform* pGoatTransform = m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Goat"))->Get_Transform();
		if (false == m_bCameraWorkPoint)
		{
			_vector vGoatPos = pGoatTransform->Get_Position();
			vecy(vGoatPos) = 1.f;
			m_pTransformCom->Set_Position(vGoatPos + pGoatTransform->Get_NLook() * 20.f);
			m_bCameraWorkPoint = true;
		}
		m_pTransformCom->Go_Vector(pGoatTransform->Get_NLook(), TimeDelta * 0.1f);
		m_pTransformCom->LookAt(WorldMatrix.r[3]);
	}
	else
	{
		m_bStartCameraWork = false;
		m_bFocusing = true;
		m_fCameraWorkTime = 0.f;
		m_bCameraWorkPoint = false;
		m_pTargetBone = nullptr;
		GM->Set_UIRender(true);
		GM->Get_Player()->Set_ControlMode(true);
		_vector	vLook = GM->Get_Player()->Get_Transform()->Get_Look();
		vecy(vLook) -= 0.3f;
		m_pTransformCom->MakeSameDirection(vLook);
		m_pGameInstance->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_Player"), 1.f);
		Start_CameraWork(WORK_RETURNTOPLAYERBACK);
	}
}

void CCamera_Perspective::Goat_PillarRot(_double TimeDelta)
{
	m_fCameraWorkTime += (_float)TimeDelta;

	if (2.2f > m_fCameraWorkTime)
	{
		_matrix WorldMatrix = m_pTargetBone->Get_CombinedTransformationMatrix() * m_TargetModelTransformMatrix * XMLoadFloat4x4(m_pTargetTransformMatrix);
		m_pTransformCom->LookAt(WorldMatrix.r[3]);
		m_pTransformCom->Go_Vector(__vector(0.f, 1.f, 0.f, 0.f), TimeDelta * 0.2f);
	}
	else
	{
		m_bStartCameraWork = false;
		m_bFocusing = true;
		m_fCameraWorkTime = 0.f;
		m_bCameraWorkPoint = false;
		m_pNpcTransform = nullptr;
		m_pTargetBone = nullptr;
		GM->Set_UIRender(true);
		GM->Get_Player()->Set_ControlMode(true);

		_vector	vLook = GM->Get_Player()->Get_Transform()->Get_Look();
		vecy(vLook) -= 0.3f;
		m_pTransformCom->MakeSameDirection(vLook);
		m_pGameInstance->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_Player"), 1.f);
		Start_CameraWork(WORK_RETURNTOPLAYERBACK);
	}
}


void CCamera_Perspective::Look_NPC(_double TimeDelta)
{
	// 0.5초동안 NPC를 바라보는 카메라연출
	m_fLookNPC_Time += (_float)TimeDelta * 2.f;
	if (1.f < m_fLookNPC_Time)
	{
		// 0.5초가 지나면 NPC와 대화를 함
		m_fLookNPC_Time = 1.f;
		m_bTalkNPC = true;
		m_bLookNPC = false;
		// 보간을 통해 0.5초동안 NPC를 바라보게 회전, 이동
		_vector vFinalLook = XMVector3Normalize(XMVectorSetY(m_pNpcTransform->Get_Position() - m_vBeforeLookAtNPC_Position, 0.f));
		_vector vFinalPosition = (m_vBeforeLookAtNPC_Position + m_pNpcTransform->Get_Position()) * 0.5f;
		vecy(vFinalPosition) = vecy(m_pNpcTransform->Get_Position()) + 0.2f;

		m_pTransformCom->MakeSameDirection(XMVector3Normalize(XMVectorLerp(m_vBeforeLookAtNPC_Look, vFinalLook, m_fLookNPC_Time)));

		m_pTransformCom->Set_Position(XMVectorLerp(m_vBeforeLookAtNPC_Position, vFinalPosition, m_fLookNPC_Time));
	}

	if (false == m_bTalkNPC)
	{
		// 보간을 통해 0.5초동안 NPC를 바라보게 회전, 이동
		_vector vFinalLook = XMVector3Normalize(XMVectorSetY(m_pNpcTransform->Get_Position() - m_vBeforeLookAtNPC_Position, 0.f));
		_vector vFinalPosition = (m_vBeforeLookAtNPC_Position + m_pNpcTransform->Get_Position()) * 0.5f;
		vecy(vFinalPosition) = vecy(m_pNpcTransform->Get_Position()) + 0.2f;

		m_pTransformCom->MakeSameDirection(XMVector3Normalize(XMVectorLerp(m_vBeforeLookAtNPC_Look, vFinalLook, m_fLookNPC_Time)));

		m_pTransformCom->Set_Position(XMVectorLerp(m_vBeforeLookAtNPC_Position, vFinalPosition, m_fLookNPC_Time));
	}
}

void CCamera_Perspective::Start_CameraWork(CAMERAWORK eCameraWork, CTransform* pTargetTransform)
{
	//	카메라 워크 시작
	m_bStartCameraWork = true;

	m_bFocusing = false;
	m_eCameraWork = eCameraWork;

	CTransform* pPlayerTransform = GM->Get_Player()->Get_Transform();
	m_fCameraWorkTime = 0.f;
	m_bCameraWorkPoint = false;

	//	카메라 워크의 종류에 따라 세팅해줘야하는 것들을 세팅.
	switch (m_eCameraWork)
	{
	case WORK_OPENBOX:
	{
		_vector vTargetPos = pTargetTransform->Get_Position();
		_vector vTargetLook = pTargetTransform->Get_NLook();
		
		pPlayerTransform->Set_Position(vTargetPos + vTargetLook);
		pPlayerTransform->MakeSameDirection(-vTargetLook);

		m_pTransformCom->Set_Position(pPlayerTransform->Get_Position() + pPlayerTransform->Get_NLook() 
			* 2.5f + pPlayerTransform->Get_NRight() * 1.f + pPlayerTransform->Get_NUp() * 1.f);
		m_vLookAtPoint = pPlayerTransform->Get_Position() + pPlayerTransform->Get_NRight() * -0.5f 
			+ pPlayerTransform->Get_NUp() * 1.f;
		m_fLookAtPointY = vecy(m_vLookAtPoint);
		m_pTransformCom->LookAt(m_vLookAtPoint);

		GM->Get_Player()->Change_PlayerAnimation(CPlayer::SORA_ANIM_OPEN_BIG);
		GM->Get_Player()->Create_PlayerState(CStateMachine::STATETYPE_INTERACTION);
	}
		break;
	case WORK_RAGE_TRANS:
		m_pTransformCom->Set_Position(pPlayerTransform->Get_Position() + pPlayerTransform->Get_NLook() * 1.f + pPlayerTransform->Get_NRight() * 1.5f + pPlayerTransform->Get_NUp() * 0.5f);
		m_vLookAtPoint = pPlayerTransform->Get_Position() + pPlayerTransform->Get_NUp() * 0.8f;
		m_fLookAtPointY = vecy(m_vLookAtPoint);
		m_pTransformCom->LookAt(m_vLookAtPoint);
		break;
	case WORK_RAGE_ATTACKEND:
		m_pTransformCom->Set_Position(pPlayerTransform->Get_Position() + pPlayerTransform->Get_NLook() * 1.3f + pPlayerTransform->Get_NRight() * 1.8f + pPlayerTransform->Get_NUp() * 1.f);
		m_vLookAtPoint = pPlayerTransform->Get_Position() + pPlayerTransform->Get_NUp() * 1.f;
		m_fLookAtPointY = vecy(m_vLookAtPoint);
		m_pTransformCom->LookAt(m_vLookAtPoint);
		break;
	case WORK_RAGE_FINISHATTACK:
	{
		m_pTargetBone = GM->Get_Player()->Get_Model()->Find_HierarchyNode("center");
		_matrix WorldMatrix = m_pTargetBone->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&GM->Get_Player()->Get_Model()->Get_TransformMatrix()) * GM->Get_Player()->Get_Transform()->Get_WorldMatrix();
		m_pTransformCom->Set_Position(pPlayerTransform->Get_Position() + pPlayerTransform->Get_NLook() * 2.5f + pPlayerTransform->Get_NUp() * 1.f);
		m_vLookAtPoint = WorldMatrix.r[3] + __vector(0.f, -0.5f, 0.f, 0.f);
		m_fLookAtPointY = vecy(m_vLookAtPoint);
		m_pTransformCom->LookAt(m_vLookAtPoint);
	}
		break;
	case WORK_HAMMER_TRANS:
	{
		m_pTargetBone = GM->Get_Player()->Get_Model()->Find_HierarchyNode("center");
		_matrix WorldMatrix = m_pTargetBone->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&GM->Get_Player()->Get_Model()->Get_TransformMatrix()) * GM->Get_Player()->Get_Transform()->Get_WorldMatrix();
		m_pTransformCom->Set_Position(WorldMatrix.r[3] + pPlayerTransform->Get_NLook() * 1.5f + __vector(0.f, 0.4f, 0.f,0.f)/* + pPlayerTransform->Get_NRight() * 1.f*/);
		m_vLookAtPoint = WorldMatrix.r[3] + __vector(0.f, 0.4f, 0.f, 0.f);
		m_fLookAtPointY = vecy(m_vLookAtPoint);
		m_pTransformCom->LookAt(m_vLookAtPoint);
	}
		break;
	case WORK_HAMMER_ATTACKEND:
	{
		m_pTargetBone = GM->Get_Player()->Get_Model()->Find_HierarchyNode("center");
		_matrix WorldMatrix = m_pTargetBone->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&GM->Get_Player()->Get_Model()->Get_TransformMatrix()) * GM->Get_Player()->Get_Transform()->Get_WorldMatrix();
		m_pTransformCom->Set_Position(WorldMatrix.r[3] + pPlayerTransform->Get_NLook() * 1.5f + pPlayerTransform->Get_NRight() * 0.5f/* + __vector(0.f, 0.4f, 0.f, 0.f))*/);
		m_vLookAtPoint = WorldMatrix.r[3] /*+ __vector(0.f, 0.4f, 0.f, 0.f)*/ + pPlayerTransform->Get_NRight() * 0.5f;
		m_fLookAtPointY = vecy(m_vLookAtPoint);
		m_pTransformCom->LookAt(m_vLookAtPoint);
	}
		break;
	case WORK_HAMMER_AIRATTACKEND:
	{
		m_pTargetBone = GM->Get_Player()->Get_Model()->Find_HierarchyNode("center");
		_matrix WorldMatrix = m_pTargetBone->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&GM->Get_Player()->Get_Model()->Get_TransformMatrix()) * GM->Get_Player()->Get_Transform()->Get_WorldMatrix();
		m_pTransformCom->Set_Position(WorldMatrix.r[3] + pPlayerTransform->Get_NLook() * 2.f/* + __vector(0.f, 0.4f, 0.f, 0.f)*/);
		m_vLookAtPoint = WorldMatrix.r[3] + __vector(0.f, 0.4f, 0.f, 0.f);
		m_fLookAtPointY = vecy(m_vLookAtPoint);
		m_pTransformCom->LookAt(m_vLookAtPoint);
	}
		break;
	case WORK_HAMMER_FINISHATTACK:
		break;
	case WORK_RAPUNZEL_PORTAL:
		m_pTransformCom->Set_Position(__vector(-111.35f, -4.5f, 120.22f, 1.f));
		m_pTransformCom->MakeSameDirection(__vector(-0.463f, 0.091f, 0.881f, 0.f));
		break;
	case WORK_RAPUNZEL_NOBODY_APPEAR:
		m_vLookAtPoint = m_pTransformCom->Get_NLook();
		m_vSavePos = m_pTransformCom->Get_Position();
		GM->Get_Player()->MakeState_Idle();
		GM->Get_Player()->Set_ControlMode(false);
		GM->Get_Player()->Change_PlayerAnimation(CPlayer::SORA_ANIM_IDLE);
		GM->Set_UIRender(false);
		break;
	case WORK_BIGTREE_APPEAR:
		// 빅트리 트랜스폼
		m_pNpcTransform = pTargetTransform;
		m_vLookAtPoint = m_pNpcTransform->Get_Position() + m_pNpcTransform->Get_NUp() * 8.f;
		m_pTransformCom->LookAt(m_vLookAtPoint);
		GM->Set_UIRender(false);
		break;
	case WORK_BIGTREE_CLIMB:
		GM->Set_UIRender(false);
		break;
	case WORK_RAPUNZELBOSS_PORTAL:
		m_pTransformCom->Set_Position(__vector(128.77f, 3.1f, 154.9f, 1.f));
		m_pTransformCom->MakeSameDirection(__vector(0.f, 0.f, 1.f, 0.f));
		break;
	case WORK_FROZEN_PORTAL:
		m_pTransformCom->Set_Position(__vector(0.f, 2.8f, 24.f, 1.f));
		m_pTransformCom->MakeSameDirection(XMVector3Normalize(__vector(0.059f, 0.f, 0.998f, 0.f)));
		GM->Set_UIRender(false);
		break;
	case WORK_KEYGRAVE_PORTAL:
		m_pTransformCom->Set_Position(__vector(0.f, 2.8f, 9.f, 1.f));
		m_pTransformCom->MakeSameDirection(__vector(0.f, 0.f, -1.f, 0.f));
		break;
	case WORK_RETURNTOPLAYER:
		m_vSavePos = m_pTransformCom->Get_Position();
		m_vSaveSecondPos = m_pTargetTransform->Get_Position() - (m_pTransformCom->Get_NLook() * m_fDistance) + XMVectorSet(0.f, 1.f, 0.f, 0.f) * m_fAdjustorY;
		m_fLookAtPointY = 0.f;
		break;
	case WORK_RETURNTOPLAYERBACK:
		m_vSavePos = m_pTransformCom->Get_Position();
		m_vSaveSecondPos = m_pTargetTransform->Get_Position() - (m_pTargetTransform->Get_NLook() * m_fDistance) + XMVectorSet(0.f, 1.f, 0.f, 0.f) * m_fAdjustorY;
		m_vLookAtPoint = m_pTransformCom->Get_NLook();
		m_vLookAtSecondPoint = m_pTargetTransform->Get_NLook();
		m_fLookAtPointY = 0.f;
		break;
	case WORK_NIGHTMARE_FOCUS:
	{
		CTransform* pNightmareTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_TWILIGHT, TEXT("Layer_Nightmare"), TEXT("Com_Transform")));
		m_vSaveSecondPos = pNightmareTransform->Get_Position();
		m_vSaveSecondPos += (__vector(0.f, 2.f, 0.f, 0.f) + pNightmareTransform->Get_NLook() * 3.f);
		m_vLookAtSecondPoint = XMVector3Normalize((__vector(0.f, 1.5f, 0.f, 0.f) + pNightmareTransform->Get_Position()) - m_vSaveSecondPos);

		m_vSavePos = m_pTransformCom->Get_Position();
		m_vLookAtPoint = m_pTransformCom->Get_NLook();
	}
		break;
	case WORK_ICEMAN_APPEAR:
	{
		m_pTransformCom->Set_Position(pTargetTransform->Get_Position() + __vector(0.f, 6.f, 0.f, 0.f));
		m_pTransformCom->LookAt(pTargetTransform->Get_Position() + __vector(0.f, 1.f, 0.f, 0.f) + pTargetTransform->Get_NLook() * 0.1f);
		GM->Set_UIRender(false);
	}
		break;
	case WORK_FROZENWOLF_APPEAR:
		m_vLookAtPoint = __vector(0.111f, -0.874f, -0.472f, 0.f);
		m_vSavePos = __vector(1.404f, 3.552f, 6.558f, 1.f);
		m_pTransformCom->MakeSameDirection(XMVector3Normalize(m_vLookAtPoint));
		m_pTransformCom->Set_Position(m_vSavePos);
		GM->Set_UIRender(false);
		break;
	case WORK_GOAT_APPEAR:
	{
		m_pTransformCom->LookAt(__vector(0.f, 0.f, 0.f, 1.f));
		m_pTransformCom->Set_Position(__vector(14.344f, 5.614f, -4.798f, 1.f));
		CModel* pModel = static_cast<CModel*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_Goat"), TEXT("Com_Model")));
		m_pTargetBone = pModel->Find_HierarchyNode("center");
		m_TargetModelTransformMatrix = XMLoadFloat4x4(&pModel->Get_TransformMatrix());
		m_pTargetTransformMatrix = static_cast<CTransform*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_Goat"), TEXT("Com_Transform")))->Get_WorldFloat4x4_Pointer();
		GM->Set_UIRender(false);
	}
		break;
	case WORK_GOAT_PILLARFIRE:
	{
		CModel* pModel = static_cast<CModel*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_Goat"), TEXT("Com_Model")));
		m_pTargetBone = pModel->Find_HierarchyNode("center");
		m_TargetModelTransformMatrix = XMLoadFloat4x4(&pModel->Get_TransformMatrix());
		m_pTargetTransformMatrix = static_cast<CTransform*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_Goat"), TEXT("Com_Transform")))->Get_WorldFloat4x4_Pointer();
		_matrix WorldMatrix = m_pTargetBone->Get_CombinedTransformationMatrix() * m_TargetModelTransformMatrix * XMLoadFloat4x4(m_pTargetTransformMatrix);
		m_pTransformCom->Set_Position(WorldMatrix.r[3] - pTargetTransform->Get_NRight() * 4.f);
		m_pTransformCom->LookAt(WorldMatrix.r[3]);
		GM->Get_Player()->GetOnNavigation();
		GM->Get_Player()->MakeState_Idle();
		GM->Get_Player()->Change_PlayerAnimation(CPlayer::SORA_ANIM_IDLE);
		GM->Get_Player()->Set_ControlMode(false);
		GM->Set_UIRender(false);
		m_pGameInstance->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_Player"), 0.f);
	}
		break;
	case WORK_GOAT_PILLARROT:
	{
		CModel* pModel = static_cast<CModel*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_Goat"), TEXT("Com_Model")));
		m_pTargetBone = pModel->Find_HierarchyNode("center");
		m_TargetModelTransformMatrix = XMLoadFloat4x4(&pModel->Get_TransformMatrix());
		m_pTargetTransformMatrix = static_cast<CTransform*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_Goat"), TEXT("Com_Transform")))->Get_WorldFloat4x4_Pointer();
		_matrix WorldMatrix = m_pTargetBone->Get_CombinedTransformationMatrix() * m_TargetModelTransformMatrix * XMLoadFloat4x4(m_pTargetTransformMatrix);
		m_pTransformCom->Set_Position(WorldMatrix.r[3] - pTargetTransform->Get_NRight() * 4.f);
		m_pTransformCom->LookAt(WorldMatrix.r[3]);
		GM->Set_UIRender(false);
		GM->Get_Player()->Set_ControlMode(false);
		m_pGameInstance->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_Player"), 0.f);
	}
		break;
	case WORK_FROZENWOLF_RAIN:
	{
		CModel* pModel = static_cast<CModel*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_FrozenWolf"), TEXT("Com_Model")));
		m_pTargetBone = pModel->Find_HierarchyNode("ago1_spo");
		m_TargetModelTransformMatrix = XMLoadFloat4x4(&pModel->Get_TransformMatrix());
		m_pNpcTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_FrozenWolf"), TEXT("Com_Transform")));
		m_pTargetTransformMatrix = m_pNpcTransform->Get_WorldFloat4x4_Pointer();
		m_pTransformCom->Set_Position(m_pNpcTransform->Get_Position() + m_pNpcTransform->Get_NLook() * 18.f + __vector(0.f, 5.f, 0.f, 0.f));
		m_pTransformCom->LookAt(m_pNpcTransform->Get_Position() + __vector(0.f, 5.f, 0.f, 0.f));
		GM->Set_UIRender(false);
		GM->Get_Player()->GetOnNavigation();
		GM->Get_Player()->MakeState_Idle();
		GM->Get_Player()->Change_PlayerAnimation(CPlayer::SORA_ANIM_IDLE);
		GM->Get_Player()->Set_ControlMode(false);
		m_pGameInstance->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_Player"), 0.f);
	}
		break;
	}

}

CCamera_Perspective * CCamera_Perspective::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCamera_Perspective*	pInstance = new CCamera_Perspective(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating CCamera_Perspective");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Perspective::Clone(void * pArg)
{
	CCamera_Perspective*	pInstance = new CCamera_Perspective(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CCamera_Perspective");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Perspective::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
