#include "stdafx.h"
#include "..\public\Level_Endding.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_Dynamic.h"
#include "GameManager.h"
#include "MiniGame_Manager.h"

#include "UI_Parents.h"
#include "UI_Logo.h"


CLevel_Ending::CLevel_Ending(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Ending::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(Ready_Prototype_GameObject()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	//m_pGameInstance->PlayBGM(L"Ending_Bgm.ogg", 0.5f);

	RECT	rc;
	POINT	pt1, pt2;
	GetClientRect(g_hWnd, &rc);

	pt1.x = rc.left;
	pt1.y = rc.top;
	pt2.x = rc.right;
	pt2.y = rc.bottom;

	ClientToScreen(g_hWnd, &pt1);
	ClientToScreen(g_hWnd, &pt2);

	rc.left = pt1.x;
	rc.top = pt1.y;
	rc.right = pt2.x;
	rc.bottom = pt2.y;

	ClipCursor(&rc);

	return S_OK;
}

_int CLevel_Ending::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	dTime += TimeDelta;

	if (3.0 < dTime && false == GameStart)
	{
		MINIGAME->Set_Boss(1);
		GameStart = true;
	}
		
	/* ================================================== */
	/* ================================================== */
	if (false == GameStart)
		return 0;

	switch (MINIGAME->Get_Boss())
	{
	case 1:

		Boss1(TimeDelta);
		break;
	case 2:

		Boss2(TimeDelta);
		break;
	case 3:

		Boss3(TimeDelta);
		break;
	case 4:

		Boss4(TimeDelta);
		break;
	case 5:

		Boss5(TimeDelta);
		break;
	default:
		break;
	}

	/* === */
	if (true == MINIGAME->Get_Create_Ending())
	{
		//iCom_Texture = Engine::CUI_Parents::MINI_BOSS1;
		//if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BOSS"), TEXT("Prototype_GameObject_Minigame_Reward"), &iCom_Texture)))
		//{
		//	BREAKPOINT;
		//	return 0;
		//}

		//iCom_Texture = Engine::CUI_Parents::MINI_BOSS2;
		//if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BOSS"), TEXT("Prototype_GameObject_Minigame_Reward"), &iCom_Texture)))
		//{
		//	BREAKPOINT;
		//	return 0;
		//}

		//iCom_Texture = Engine::CUI_Parents::MINI_BOSS3;
		//if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BOSS"), TEXT("Prototype_GameObject_Minigame_Reward"), &iCom_Texture)))
		//{
		//	BREAKPOINT;
		//	return 0;
		//}

		//iCom_Texture = Engine::CUI_Parents::MINI_BOSS4;
		//if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BOSS"), TEXT("Prototype_GameObject_Minigame_Reward"), &iCom_Texture)))
		//{
		//	BREAKPOINT;
		//	return 0;
		//}


		//iCom_Texture = Engine::CUI_Parents::MINI_BOSS5;
		//if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BOSS"), TEXT("Prototype_GameObject_Minigame_Reward"), &iCom_Texture)))
		//{
		//	BREAKPOINT;
		//	return 0;
		//}

		iCom_Texture = Engine::CUI_Parents::MINI_BOSS1;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BOSS"), TEXT("Prototype_GameObject_Minigame_Reward"), &iCom_Texture)))
		{
			BREAKPOINT;
			return 0;
		}

		MINIGAME->Set_Create_Ending(false);
	}

	if (true == MINIGAME->Get_EndTexture())
	{
		EndCreateTime += TimeDelta;
		if (3.0 < EndCreateTime)
		{
			iCom_Texture = Engine::CUI_Parents::MINI_GAMEEND;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BOSS"), TEXT("Prototype_GameObject_Minigame_Reward"), &iCom_Texture)))
			{
				BREAKPOINT;
				return 0;
			}
			MINIGAME->Set_EndTexture(false);
		}
	}

	/* ================================================== */
	/* ================================================== */

	return RESULT_NOPROBLEM;
}

_int CLevel_Ending::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	return RESULT_NOPROBLEM;
}

HRESULT CLevel_Ending::Render()
{
	if (FAILED(__super::Render()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Ending::Ready_Prototype_GameObject()
{

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI"), TEXT("Prototype_GameObject_Minigame_BackGround"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* === */

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_Player"), TEXT("Prototype_GameObject_Minigame_Player"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

void CLevel_Ending::Boss1(_double TimeDelta)
{
	if (false == Boss1_Creat)
	{
		iCom_Texture = Engine::CUI_Parents::MINI_BOSS1;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BOSS"), TEXT("Prototype_GameObject_Minigame_Boss"),&iCom_Texture)))
		{
			BREAKPOINT;
			return;
		}

		Boss1_Creat = true;
	}

}

void CLevel_Ending::Boss2(_double TimeDelta)
{
	if (false == Boss2_Creat)
	{
		iCom_Texture = Engine::CUI_Parents::MINI_BOSS2;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BOSS"), TEXT("Prototype_GameObject_Minigame_Boss"), &iCom_Texture)))
		{
			BREAKPOINT;
			return;
		}

		Boss2_Creat = true;
	}

}

void CLevel_Ending::Boss3(_double TimeDelta)
{
	if (false == Boss3_Creat)
	{
		iCom_Texture = Engine::CUI_Parents::MINI_BOSS3;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BOSS"), TEXT("Prototype_GameObject_Minigame_Boss"), &iCom_Texture)))
		{
			BREAKPOINT;
			return;
		}

		Boss3_Creat = true;
	}

}

void CLevel_Ending::Boss4(_double TimeDelta)
{
	if (false == Boss4_Creat)
	{
		iCom_Texture = Engine::CUI_Parents::MINI_BOSS4;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BOSS"), TEXT("Prototype_GameObject_Minigame_Boss"), &iCom_Texture)))
		{
			BREAKPOINT;
			return;
		}

		Boss4_Creat = true;
	}

}

void CLevel_Ending::Boss5(_double TimeDelta)
{
	if (false == Boss5_Creat)
	{
		iCom_Texture = Engine::CUI_Parents::MINI_BOSS5;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BOSS"), TEXT("Prototype_GameObject_Minigame_Boss"), &iCom_Texture)))
		{
			BREAKPOINT;
			return;
		}

		Boss5_Creat = true;
	}

}

CLevel_Ending * CLevel_Ending::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLevel_Ending*	pInstance = new CLevel_Ending(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Ending::Free()
{
	__super::Free();
}
