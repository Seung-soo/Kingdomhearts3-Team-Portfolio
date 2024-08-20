#include "stdafx.h"
#include "..\public\Progress_Manager.h"
#include "GameInstance.h"
#include "Player.h"
#include "Camera_Perspective.h"
#include "GameManager.h"
#include "Status.h"
#include "Monster.h"
#include "UI_FadeOut.h"
#include "UI_QusetNavi.h"
#include "TreasureBox.h"

READY_SINGLETON(CProgress_Manager)

CProgress_Manager::CProgress_Manager()
{
}

void CProgress_Manager::Set_ReadyWhiteFadeOut(_bool bReadyWhiteFadeOut)
{
	m_bReadyWhiteFadeOut = bReadyWhiteFadeOut;
}

void CProgress_Manager::Update(_double TimeDelta)
{
	// 시간 간격이 필요할 때
	if (true == m_bUseInterval)
	{
		m_fInterval -= (_float)TimeDelta;
		if (0.f >= m_fInterval)
		{
			/*해야할 일*/
			switch (m_eAct)
			{
			case ACT_SUMMONTUTORIAL:
				Summon_Tutorial();
				break;
			case ACT_MINIGAME:
				m_bMinigameFade = true;
				break;
			}

			m_bUseInterval = false;
		}
	}

	if (true == m_bReadyWhiteFadeOut)
		WhiteFadeOut(TimeDelta);

	/*Minigame*/
	if (GM->Get_CurrentLevel() == LEVEL_RAPUNZEL_BOSS)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (true == m_bMinigameFade && false == m_bMinigameStart)
		{ 
			//pGameInstance->PlayBGM(TEXT("RapunzelBossBGM.mp3"), 1.f);
			GM->Set_bCameraMove(true);
			if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Minigame_Fade"))))
			{
				BREAKPOINT;
				return;
			}
			m_bMinigameFade = false;
		}
		else if (true == m_bMinigameEnd && true == m_bMinigameStart)
		{
			m_bMinigameStart = false;
			m_bMinigameEnd = false;
			GM->Set_bCameraMove(false);


			if (FAILED(pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_MiniGame"))))
			{
				BREAKPOINT;
				return;
			}
		}
		else
		{
		}
	}
}

void CProgress_Manager::CameraScene_End()
{
	GM->Get_Player()->Set_ControlMode(true);
	GM->Get_Camera()->Set_StartScene(false);
	GM->Set_bCameraMove(false);
}

void CProgress_Manager::ToUseInterval(_bool bUseInterval, _float fInterval, ACT eAct)
{
	m_bUseInterval = bUseInterval;
	m_fInterval    = fInterval;
	m_eAct         = eAct;
}

void CProgress_Manager::Rapunzel_StartLogoScene()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_int iLogoNumber = 1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_StageLogo"), &iLogoNumber)))
	{
		BREAKPOINT;
		return;
	}

	GM->Get_Player()->Set_ControlMode(false);
	GM->Get_Camera()->Set_StartScene(true);
	GM->Set_bCameraMove(true);
}

void CProgress_Manager::Summon_Tutorial()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_int iTutorialUI = 0;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_Tutorial"), TEXT("Prototype_GameObject_UI_Tip"), &iTutorialUI)))
	{
		BREAKPOINT;
		return;
	}

	iTutorialUI	= 5;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_Tutorial"), TEXT("Prototype_GameObject_UI_Tip"), &iTutorialUI)))
	{
		BREAKPOINT;
		return;
	}
}

void CProgress_Manager::WhiteFadeOut(_double TimeDelta)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	if (1 == GM->Get_MonstersList()->size())
	{
		if (0.f >= static_cast<CStatus*>(GM->Get_MonstersList()->back()->Get_Component(TEXT("Com_Status")))->Get_HP() && false == m_bMonsterDead)
		{
			if (false == m_bWhiteFirst)
			{
				GM->Set_UIRender(false);

				m_bWhiteFirst = true;
			}
			GM->PlaySpeedControl(0.03f);
			_int iCom_Texture = 2;
			m_pFade = pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Fade"), TEXT("Prototype_GameObject_LoadingBlack"), &iCom_Texture);

			m_bMonsterDead = true;

			pGameInstance->StopSound(CSoundMgr::BGM);
			pGameInstance->StopSound(CSoundMgr::SYSTEM_EFFECT);
			pGameInstance->LetsPlaySound(TEXT("WhiteFadeOut.wav"), CSoundMgr::SYSTEM_EFFECT, 0.7f);
		}
	}

	if (true == m_bMinigameWhiteFadeOut)
	{
		pGameInstance->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_Minigame"), 0.03f);
		pGameInstance->Set_TimeSpeed(GM->Get_CurrentLevel(), TEXT("Layer_MiniGame"), 0.03f);
		_int iCom_Texture = 2;
		m_pFade = pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Fade"), TEXT("Prototype_GameObject_LoadingBlack"), &iCom_Texture);
		m_bMonsterDead = true;

		m_bMinigameWhiteFadeOut = false;
	}

	if (true == m_bMonsterDead)
	{
		m_fWhiteTime -= (_float)TimeDelta;
	}
	if (nullptr != m_pFade && true == static_cast<UI_FadeOut*>(m_pFade)->Get_FadeOutMax())
	{
		GM->PlaySpeedControl(1.f);
		GM->Get_Player()->GetOnNavigation();
		GM->Set_UIRender(true);
	}
	if (0.f > m_fWhiteTime)
	{
		if (nullptr != m_pFade)
		{
			m_pFade->Set_Dead(true);
			m_pFade = nullptr;

			if (LEVEL_RAPUNZEL == GM->Get_CurrentLevel())
			{
				GM->Set_UIRender(false);
				GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_RAPUNZEL_PORTAL);
			}
			else if (LEVEL_RAPUNZEL_BOSS == GM->Get_CurrentLevel())
			{
				if (false == m_bMinigameStart)
				{
					GM->Get_Camera()->Get_Transform()->Set_Position(__vector(129.f, 3.16f, 158.26f, 1.f));
					GM->Get_Camera()->Get_Transform()->MakeSameDirection(__vector(0.f, 0.f, -1.f, 0.f));
					GM->Set_UIRender(false);
					ToUseInterval(true, 3.f, ACT_MINIGAME);
					m_bMinigameWaiting = true;
				}
				else
				{
					GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_RAPUNZELBOSS_PORTAL);

					PM->Set_MinigameEnd(true);
					GM->Set_UIRender(false);
					m_bMinigameWaiting = false;
				}
			}
			else if (LEVEL_FROZEN == GM->Get_CurrentLevel())
			{
				CTreasureBox::BOXDESC tBoxDesc;
				tBoxDesc.eBoxTheme = CTreasureBox::THEME_FZ;
				tBoxDesc.eEvent = CTreasureBox::EVENT_DONALD;
				tBoxDesc.vLook = XMVector3Normalize(__vector(-0.059f, 0.f, -0.998f, 0.f));
				tBoxDesc.vPosition = __vector(-4.871f, 0.f, 31.276f, 1.f);
				// TreasureBox
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_FROZEN, TEXT("Layer_TreasureBox"), TEXT("Prototype_GameObject_TreasureBox"), &tBoxDesc)))
				{
					BREAKPOINT;
					return;
				}
				GM->Set_UIRender(false);
				GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_FROZEN_PORTAL);
			}
			else if (LEVEL_KEYGRAVE == GM->Get_CurrentLevel())
			{
				GM->Set_UIRender(false);
				GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_KEYGRAVE_PORTAL);
			}
			else if (LEVEL_FINALBOSS == GM->Get_CurrentLevel())
			{
				GM->Set_MoveOnToNextLevel(true);
				GM->Set_NextLevel(LEVEL_ENDING);
				m_fWhiteTime = 5.f;
				m_bMonsterDead = false;
				m_bReadyWhiteFadeOut = false;

				m_bWhiteFirst = false;
				return;
			}

			_int iCom_Texture = 3;
			if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Fade"), TEXT("Prototype_GameObject_LoadingBlack"), &iCom_Texture)))
			{
				BREAKPOINT;
				return;
			}

			m_fWhiteTime = 5.f;
			m_bMonsterDead = false;
			m_bReadyWhiteFadeOut = false;

			m_bWhiteFirst = false;
		}
	}
}

void CProgress_Manager::QuestNavi_TargetSetting(_fvector vPos)
{
	m_pQuestNavi->Set_TargetPosition(vPos);
}

void CProgress_Manager::Create_QuestNavi()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pQuestNavi = static_cast<CUI_QusetNavi*>(pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_QuestNavi")));
}

void CProgress_Manager::Free()
{
}
