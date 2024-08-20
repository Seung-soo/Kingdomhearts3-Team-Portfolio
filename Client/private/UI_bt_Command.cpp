#include "stdafx.h"
#include "..\public\UI_bt_Command.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "InvenManager.h"
#include "SoundManager.h"

#include "Donald.h"
#include "Goofy.h"

#include "Player.h"
#include "Player_Weapon.h"
#include "StateMachine.h"
#include "Effect_Manager.h"

// static ================

static SLOT_STATE eSlot_State = SLOT_STATE::SLOT_STATE_FIRST;
static _int st_iSlot = ON_SLOT1; // Slot 선택

//=====================

CUI_bt_Command::CUI_bt_Command(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_bt_Command::CUI_bt_Command(const CUI_bt_Command & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_bt_Command::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_bt_Command::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));

	iCom_Texture = (*(_int*)pArg); // 텍스쳐 정보
	CHECK_FAILED(SetUp_Components());

	// UI 기본 Setting ===

	m_fSizeX = g_iWinCX;
	m_fSizeY = g_iWinCY;
	m_fX = 0.f;
	m_fY = 0.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, 0.f, 1.f)));

	// Setting =========

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::UI_BT_FRAME0:

		SizeX = 400.f*0.8f;
		SizeY = 170.f*0.8f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 108.f;
		SetY = 165.5f;

		m_fDepth = 0.2f; // 깊이
		m_fFrame = 0.f;
		break;
	case Engine::CUI_Parents::UI_BT_FRAME1:

		SizeX = 400.f*0.8f;
		SizeY = 170.f*0.8f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 108.f;
		SetY = 165.5f;

		m_fDepth = 0.21f; // 깊이

		m_fFrame = 1.f;
		break;
	case Engine::CUI_Parents::UI_BT_FRAME2:

		SizeX = 400.f*0.8f;
		SizeY = 170.f*0.8f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 108.f;
		SetY = 165.5f;

		m_fDepth = 0.22f; // 깊이

		m_fFrame = 2.f;
		break;
	case Engine::CUI_Parents::UI_BT_FRAME_FAN1:

		SizeX = 64.f*0.8f;
		SizeY = 64.f*0.8f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 35.f;
		SetY = 188.5f;

		m_fDepth = 0.23f; // 깊이

		m_fFrame = 3.f;
		break;
	case Engine::CUI_Parents::UI_BT_FRAME_FAN2:

		SizeX = 64.f*0.8f;
		SizeY = 64.f*0.8f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 205.f;
		SetY = 188.5f;

		m_fDepth = 0.23f; // 깊이

		m_fFrame = 3.f;
		break;

	case Engine::CUI_Parents::UI_BT_FRAME4:

		SizeX = 400.f*0.8f;
		SizeY = 170.f*0.8f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 108.f;
		SetY = 165.5f;

		m_fDepth = 0.24f; // 깊이

		m_fFrame = 4.f;
		break;
	case Engine::CUI_Parents::UI_BT_FRAME5:

		SizeX = 400.f*0.7f;
		SizeY = 170.f*0.7f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 165.f*0.8f;
		SetY = 84.f*0.8f;

		m_fDepth = 0.23f; // 깊이

		m_fFrame = 5.f;
		break;
	case Engine::CUI_Parents::UI_BT_FRAME6:

		SizeX = 400.f*0.7f;
		SizeY = 170.f*0.7f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 165.f*0.8f;
		SetY = 84.f*0.8f;

		m_fDepth = 0.29f; // 깊이

		m_fFrame = 6.f;
		break;
	case Engine::CUI_Parents::UI_BT_FRAME7:

		SizeX = 400.f*0.8f;
		SizeY = 170.f*0.8f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 151.f;
		SetY = 71.f;

		m_fDepth = 0.24f; // 깊이

		m_fFrame = 0.f;

		break;
	case Engine::CUI_Parents::UI_BT_SLOT1:

		//SizeX = (294.f + 24.f)*0.8f; // X:294 + 24
		//SizeY = 28.f*0.8f;
		SizeX = (294.f + 24.f); // X:294 + 24
		SizeY = 28.f * 1.1f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = (148.f - 24.f)*0.8f;
		SetY = 131.f*0.8f;

		m_fDepth = 0.24f; // 깊이
		m_fFrame = 0.f;

		break;
	case Engine::CUI_Parents::UI_BT_SLOT2:

		SizeX = (294.f + 24.f); // X:294 + 24
		SizeY = 28.f * 1.1f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = (148.f - 24.f)*0.8f;
		SetY = (131.f - 28.f)*0.8f;

		m_fDepth = 0.24f; // 깊이
		m_fFrame = 0.f;

		break;
	case Engine::CUI_Parents::UI_BT_SLOT3:

		SizeX = (294.f + 24.f); // X:294 + 24
		SizeY = 28.f * 1.1f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = (148.f - 24.f)*0.8f;
		SetY = (131.f - 56.f)*0.8f;

		m_fDepth = 0.24f; // 깊이
		m_fFrame = 0.f;

		break;
	case Engine::CUI_Parents::UI_BT_SLOT4:

		SizeX = (294.f + 24.f); // X:294 + 24
		SizeY = 28.f * 1.1f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = (148.f - 24.f)*0.8f;
		SetY = (131.f - 84.f)*0.8f;

		m_fDepth = 0.24f; // 깊이
		m_fFrame = 0.f;

		break;
	case Engine::CUI_Parents::UI_BT_SLOT2_1:

		SizeX = 288.f*0.8f;
		SizeY = 48.f*0.8f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = (168.f + 294.f)*0.8f;
		SetY = 131.f*0.8f;

		m_fDepth = 0.24f; // 깊이
		m_fFrame = 2.f;

		break;
	case Engine::CUI_Parents::UI_BT_SLOT2_2:

		SizeX = 288.f*0.8f;
		SizeY = 48.f*0.8f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = (168.f + 294.f)*0.8f;
		SetY = (131.f - 28.f)*0.8f;

		m_fDepth = 0.24f; // 깊이
		m_fFrame = 2.f;

		break;
	case Engine::CUI_Parents::UI_BT_SLOT2_3:

		SizeX = 288.f*0.8f;
		SizeY = 48.f*0.8f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = (168.f + 294.f)*0.8f;
		SetY = (131.f - 56.f)*0.8f;

		m_fDepth = 0.24f; // 깊이
		m_fFrame = 0.f;

		break;
	case Engine::CUI_Parents::UI_BT_SLOT2_4:

		SizeX = 288.f*0.8f;
		SizeY = 48.f*0.8f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = (168.f + 294.f)*0.8f;
		SetY = (131.f - 84.f)*0.8f;

		m_fDepth = 0.24f; // 깊이
		m_fFrame = 0.f;

		break;
	case Engine::CUI_Parents::UI_BT_SLOT3_1:

		SizeX = 288.f*0.8f;
		SizeY = 48.f*0.8f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = (168.f + 294.f)*0.8f;
		SetY = (131.f - 28.f)*0.8f;

		m_fDepth = 0.24f; // 깊이
		m_fFrame = 2.f;

		break;
	case Engine::CUI_Parents::UI_BT_SLOT3_2:

		SizeX = 288.f*0.8f;
		SizeY = 48.f*0.8f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = (168.f + 294.f)*0.8f;
		SetY = (131.f - 56.f)*0.8f;

		m_fDepth = 0.24f; // 깊이
		m_fFrame = 2.f;

		break;
	case Engine::CUI_Parents::UI_BT_SLOT3_3:

		SizeX = 288.f*0.8f;
		SizeY = 48.f*0.8f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = (168.f + 294.f)*0.8f;
		SetY = (131.f - 84.f)*0.8f;

		m_fDepth = 0.24f; // 깊이
		m_fFrame = 2.f;

		break;
	case Engine::CUI_Parents::UI_BT_SELECT:

		SizeX = 32.f*0.8f;
		SizeY = 32.f*0.8f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 300.f*0.8f;
		SetY = 131.f*0.8f;

		m_fDepth = 0.2f; // 깊이

		break;
	default:
		break;
	}



	//================================

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	//===============

	LEVEL curr_level = GM->Get_CurrentLevel();
	m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_Front(curr_level, TEXT("Layer_Player")));

	if (nullptr == m_pPlayer)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

_int CUI_bt_Command::Tick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	if (false == GM->Get_UIRender())
		return RESULT_NOPROBLEM;

	if (false == GM->Get_bCameraMove() && false == GM->Get_InvenOn())
	{
		/* ================================================================== */
		switch (iCom_Texture) // UI_BT_FRAME ============================
		{
		case Engine::CUI_Parents::UI_BT_FRAME0:


			break;
		case Engine::CUI_Parents::UI_BT_FRAME1:
			/* COMMANDS 빛 */
			break;
		case Engine::CUI_Parents::UI_BT_FRAME_FAN1:
		case Engine::CUI_Parents::UI_BT_FRAME_FAN2:
			/* 팬 돌리기 */

			//if (true == bFan)
			//{

			//}

			m_pTransformCom->NowRotation(XMVectorSet(0.f, 0.f, 1.f, 1.f), XMConvertToRadians(5.f));
			break;
		case Engine::CUI_Parents::UI_BT_FRAME7:
			/* 빛 */
			if (false == bLight)
			{
				dLightTime_1 += TimeDelta;
				if (10.0 < dLightTime_1)
				{
					bLight = true;
					dLightTime_1 = 0.0;
				}
			}

			if (true == bLight)
			{
				m_fFrame += (_float)(20.0 * TimeDelta);
				if (3.f < m_fFrame)
				{
					iLightCnt += 1;
					m_fFrame = 0.f;
				}

				if (5 <= iLightCnt)
				{
					iLightCnt = 0;
					m_fFrame = 3.f;
					bLight = false;
				}

			}


			break;
		case Engine::CUI_Parents::UI_BT_SELECT: // UI_BT_SELECT =================

			dSelect_Time += TimeDelta;

			if (0.01 < dSelect_Time) // Select 기본 애니매이션(화살표)
			{
				if (false == bSelect_Move_Left)
				{
					fSelect_MoveSetX += 0.2f;

					if (4.f <= fSelect_MoveSetX)
					{
						fSelect_MoveSetX = 2.f;
						bSelect_Move_Left = true;
					}

				}
				else if (true == bSelect_Move_Left)
				{
					fSelect_MoveSetX -= 0.2f;

					if (0.f >= fSelect_MoveSetX)
					{
						fSelect_MoveSetX = 0.f;
						bSelect_Move_Left = false;
					}
				}

				dSelect_Time = 0.0;
			}

			/* 버튼동작 */
			if (SLOT_STATE::SLOT_STATE_FIRST == eSlot_State)	/*SLOT_FIRST*/
			{
				// 키 입력
				if (m_pGameInstance->Get_KeyEnter(DIK_UP))
				{
					iSelect_First_Cnt -= 1;
					SM->Sound_Command(L"UI_Command_1.wav", 1.f);
					//m_pGameInstance->LetsPlaySound(L"UI_Commend_1.wav", CSoundMgr::CHANNELID::CommandA, 1.f);

				}


				if (m_pGameInstance->Get_KeyEnter(DIK_DOWN))
				{
					iSelect_First_Cnt += 1;
					SM->Sound_Command(L"UI_Command_1.wav", 1.f);
					//m_pGameInstance->LetsPlaySound(L"UI_Commend_1.wav", CSoundMgr::CHANNELID::CommandA, 1.f);
				}


				//===

				if (0 > iSelect_First_Cnt)
					iSelect_First_Cnt = 3;

				if (3 < iSelect_First_Cnt)
					iSelect_First_Cnt = 0;

				GM->Set_First_Frame_Num(iSelect_First_Cnt);

				// FIRST_CNT 버튼 위치 지정===
				if (0 == iSelect_First_Cnt)
				{
					iSelect_Second_Cnt = 0; /* Magic_Select 초기화 */
					fSelect_SetX = -40.f;
					fSelect_SetY = 0.f;
					st_iSlot = ON_SLOT1;
				}
				if (1 == iSelect_First_Cnt)
				{
					iSelect_Second_Cnt = 0; /* Magic_Select 초기화 */
					fSelect_SetX = -40.f;
					fSelect_SetY = 28.f;
					st_iSlot = ON_SLOT2;
				}
				if (2 == iSelect_First_Cnt)
				{
					iSelect_Second_Cnt = 0; /* Magic_Select 초기화 */
					fSelect_SetX = -40.f;
					fSelect_SetY = 56.f;
					st_iSlot = ON_SLOT3;
				}
				if (3 == iSelect_First_Cnt)
				{
					iSelect_Second_Cnt = 0; /* Magic_Select 초기화 */
					fSelect_SetX = -40.f;
					fSelect_SetY = 84.f;
					st_iSlot = ON_SLOT4;
				}
			}
			else if (SLOT_STATE::SLOT_STATE_SECOND_2 == eSlot_State) /*SLOT_SECOND_SKILL*/
			{
				// 키 입력
				if (m_pGameInstance->Get_KeyEnter(DIK_UP))
				{
					iSelect_Second_Cnt -= 1;
					SM->Sound_Command(L"UI_Command_1.wav", 1.f);
					//m_pGameInstance->LetsPlaySound(L"UI_Commend_1.wav", CSoundMgr::CHANNELID::CommandB, 1.f);
				}


				if (m_pGameInstance->Get_KeyEnter(DIK_DOWN))
				{
					iSelect_Second_Cnt += 1;
					SM->Sound_Command(L"UI_Command_1.wav", 1.f);
					//m_pGameInstance->LetsPlaySound(L"UI_Commend_1.wav", CSoundMgr::CHANNELID::CommandB, 1.f);
				}


				//===
				if (true == GM->Get_Slot_On_Donald() && false == GM->Get_Slot_On_Goofy())
				{
					if (0 > iSelect_Second_Cnt)
						iSelect_Second_Cnt = 1;

					if (1 < iSelect_Second_Cnt)
						iSelect_Second_Cnt = 0;
				}
				else if (true == GM->Get_Slot_On_Donald() && true == GM->Get_Slot_On_Goofy())
				{
					if (0 > iSelect_Second_Cnt)
						iSelect_Second_Cnt = 3;

					if (3 < iSelect_Second_Cnt)
						iSelect_Second_Cnt = 0;
				}


				GM->Set_iFriendIcon_Num(iSelect_Second_Cnt); // UI_FriendIcon 으로 정보 넘김
				GM->Set_Second_Frame_Num(iSelect_Second_Cnt); // UI_TextCommand::소환 으로 정보넘김

															  // SECOND_CNT 버튼 위치 지정===
				if (0 == iSelect_Second_Cnt)
				{
					iSelect_First_Cnt = 0;
					fSelect_SetX = 278.f;
					fSelect_SetY = -1.f;
					st_iSlot = ON_SLOT2_1;
				}
				if (1 == iSelect_Second_Cnt)
				{
					iSelect_First_Cnt = 0;
					fSelect_SetX = 278.f;
					fSelect_SetY = 27.f;
					st_iSlot = ON_SLOT2_2;
				}
				if (2 == iSelect_Second_Cnt)
				{
					iSelect_First_Cnt = 0;
					fSelect_SetX = 278.f;
					fSelect_SetY = 55.f;
					st_iSlot = ON_SLOT2_3;
				}
				if (3 == iSelect_Second_Cnt)
				{
					iSelect_First_Cnt = 0;
					fSelect_SetX = 278.f;
					fSelect_SetY = 83.f;
					st_iSlot = ON_SLOT2_4;
				}

			}
			else if (SLOT_STATE::SLOT_STATE_SECOND_3 == eSlot_State) /*SLOT_SECOND_ITEM*/
			{
				// 키 입력
				if (m_pGameInstance->Get_KeyEnter(DIK_UP))
				{
					iSelect_Second_Cnt -= 1;
					SM->Sound_Command(L"UI_Command_1.wav", 1.f);
					//m_pGameInstance->LetsPlaySound(L"UI_Commend_1.wav", CSoundMgr::CHANNELID::CommandC, 1.f);
				}


				if (m_pGameInstance->Get_KeyEnter(DIK_DOWN))
				{
					iSelect_Second_Cnt += 1;
					SM->Sound_Command(L"UI_Command_1.wav", 1.f);
					//m_pGameInstance->LetsPlaySound(L"UI_Commend_1.wav", CSoundMgr::CHANNELID::CommandC, 1.f);
				}


				//===

				if (0 > iSelect_Second_Cnt)
					iSelect_Second_Cnt = 2;

				if (2 < iSelect_Second_Cnt)
					iSelect_Second_Cnt = 0;

				/* ===== */

				Inven->Set_QuickItem_Cnt(iSelect_Second_Cnt);

				if (0 == iSelect_Second_Cnt)   // SECOND_CNT 버튼 위치 지정===
				{
					iSelect_First_Cnt = 0;
					fSelect_SetX = 278.f;
					fSelect_SetY = 27.f;
					st_iSlot = ON_SLOT3_1;
				}
				if (1 == iSelect_Second_Cnt)
				{
					iSelect_First_Cnt = 0;
					fSelect_SetX = 278.f;
					fSelect_SetY = 55.f;
					st_iSlot = ON_SLOT3_2;
				}
				if (2 == iSelect_Second_Cnt)
				{
					iSelect_First_Cnt = 0;
					fSelect_SetX = 278.f;
					fSelect_SetY = 83.f;
					st_iSlot = ON_SLOT3_3;
				}

			}
			//==========================================================

			SetX = (300.f + fSelect_SetX + fSelect_MoveSetX)*0.8f; // Frame 포지션 세팅
			SetY = (131.f - fSelect_SetY)*0.8f;

			break;
		default:
			break;
		}

		/* ================================================================== */
		switch (eSlot_State) // UI_BT_SLOT =============================
		{
		case Client::SLOT_STATE_FIRST:

			switch (iCom_Texture)
			{
			case UI_BT_SLOT1:
				Fnc_On_Slot1(TimeDelta); // Slot1의 기능
				break;
			case UI_BT_SLOT2:
				Fnc_On_Slot2(TimeDelta); // Slot2의 기능
				break;
			case UI_BT_SLOT3:
				Fnc_On_Slot3(TimeDelta); // Slot3의 기능
				break;
			case UI_BT_SLOT4:
				Fnc_On_Slot4(TimeDelta); // Slot4의 기능
				break;
			default:
				break;
			}

			break;

		case Client::SLOT_STATE_SECOND_2:

			switch (iCom_Texture)
			{
			case UI_BT_SLOT2_1:
				Fnc_On_Slot2_1(TimeDelta);	// Magic_Slot
				break;
			case UI_BT_SLOT2_2:
				Fnc_On_Slot2_2(TimeDelta);	// Magic_Slot
				break;
			case UI_BT_SLOT2_3:
				Fnc_On_Slot2_3(TimeDelta);	// Magic_Slot
				break;
			case UI_BT_SLOT2_4:
				Fnc_On_Slot2_4(TimeDelta);	// Magic_Slot
				break;
			default:
				break;
			}

			break;

		case Client::SLOT_STATE_SECOND_3:

			switch (iCom_Texture)
			{
			case UI_BT_SLOT3_1:
				Fnc_On_Slot3_1(TimeDelta);  // Item_Slot
				break;
			case UI_BT_SLOT3_2:
				Fnc_On_Slot3_2(TimeDelta);  // Item_Slot
				break;
			case UI_BT_SLOT3_3:
				Fnc_On_Slot3_3(TimeDelta);  // Item_Slot
				break;
			default:
				break;
			}

			break;
		}

		/* ============================== */

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	}

	return RESULT_NOPROBLEM;
}

_int CUI_bt_Command::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	if (false == GM->Get_bCameraMove())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return RESULT_NOPROBLEM;
}

HRESULT CUI_bt_Command::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (false == GM->Get_UIRender())
		return S_OK;

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::UI_BT_FRAME0:
	case Engine::CUI_Parents::UI_BT_FRAME1:
	case Engine::CUI_Parents::UI_BT_FRAME2:
	case Engine::CUI_Parents::UI_BT_FRAME_FAN1:
	case Engine::CUI_Parents::UI_BT_FRAME_FAN2:
	case Engine::CUI_Parents::UI_BT_FRAME4:
	case Engine::CUI_Parents::UI_BT_FRAME5:
	case Engine::CUI_Parents::UI_BT_FRAME6:
	case Engine::CUI_Parents::UI_BT_FRAME7:
	case Engine::CUI_Parents::UI_BT_SLOT1:
	case Engine::CUI_Parents::UI_BT_SLOT2:
	case Engine::CUI_Parents::UI_BT_SLOT3:
	case Engine::CUI_Parents::UI_BT_SLOT4:
	case Engine::CUI_Parents::UI_BT_SELECT:
		CHECK_FAILED(SetUp_ConstantTable());
		m_pVIBufferCom->Render(m_pShaderCom, 0);
		break;

	default:
		break;
	}
	/* =============== */
	/* =============== */

	if (UI_NAME::UI_BT_SLOT2_1 == iCom_Texture && SLOT_STATE_SECOND_2 == eSlot_State)
	{
		CHECK_FAILED(SetUp_ConstantTable());
		m_pVIBufferCom->Render(m_pShaderCom, 0);
	}
	if (UI_NAME::UI_BT_SLOT2_2 == iCom_Texture && SLOT_STATE_SECOND_2 == eSlot_State)
	{
		CHECK_FAILED(SetUp_ConstantTable());
		m_pVIBufferCom->Render(m_pShaderCom, 0);
	}
	if (UI_NAME::UI_BT_SLOT2_3 == iCom_Texture && SLOT_STATE_SECOND_2 == eSlot_State)
	{
		CHECK_FAILED(SetUp_ConstantTable());
		m_pVIBufferCom->Render(m_pShaderCom, 0);
	}
	if (UI_NAME::UI_BT_SLOT2_4 == iCom_Texture && SLOT_STATE_SECOND_2 == eSlot_State)
	{
		CHECK_FAILED(SetUp_ConstantTable());
		m_pVIBufferCom->Render(m_pShaderCom, 0);
	}


	if (UI_NAME::UI_BT_SLOT3_1 == iCom_Texture && SLOT_STATE_SECOND_3 == eSlot_State)
	{
		CHECK_FAILED(SetUp_ConstantTable());
		m_pVIBufferCom->Render(m_pShaderCom, 0);
	}
	if (UI_NAME::UI_BT_SLOT3_2 == iCom_Texture && SLOT_STATE_SECOND_3 == eSlot_State)
	{
		CHECK_FAILED(SetUp_ConstantTable());
		m_pVIBufferCom->Render(m_pShaderCom, 0);
	}
	if (UI_NAME::UI_BT_SLOT3_3 == iCom_Texture && SLOT_STATE_SECOND_3 == eSlot_State)
	{
		CHECK_FAILED(SetUp_ConstantTable());
		m_pVIBufferCom->Render(m_pShaderCom, 0);
	}


	return S_OK;
}

HRESULT CUI_bt_Command::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */ //====================
	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::UI_BT_FRAME0:
	case Engine::CUI_Parents::UI_BT_FRAME1:
	case Engine::CUI_Parents::UI_BT_FRAME2:
	case Engine::CUI_Parents::UI_BT_FRAME_FAN1:
	case Engine::CUI_Parents::UI_BT_FRAME_FAN2:
	case Engine::CUI_Parents::UI_BT_FRAME4:
	case Engine::CUI_Parents::UI_BT_FRAME5:
	case Engine::CUI_Parents::UI_BT_FRAME6:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_bt_Frame"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;

	case Engine::CUI_Parents::UI_BT_FRAME7:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_bt_Frame7"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;

	case Engine::CUI_Parents::UI_BT_SLOT1:
	case Engine::CUI_Parents::UI_BT_SLOT2:
	case Engine::CUI_Parents::UI_BT_SLOT3:
	case Engine::CUI_Parents::UI_BT_SLOT4:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_bt_Slot"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;

	case Engine::CUI_Parents::UI_BT_SLOT2_1:
	case Engine::CUI_Parents::UI_BT_SLOT2_2:
	case Engine::CUI_Parents::UI_BT_SLOT2_3:
	case Engine::CUI_Parents::UI_BT_SLOT2_4:
	case Engine::CUI_Parents::UI_BT_SLOT3_1:
	case Engine::CUI_Parents::UI_BT_SLOT3_2:
	case Engine::CUI_Parents::UI_BT_SLOT3_3:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_bt_SlotN"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;

	case Engine::CUI_Parents::UI_BT_SELECT:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_bt_Point_Select"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;

	default:
		break;
	}

	return S_OK;
}

HRESULT CUI_bt_Command::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	// TextureCom Set ======
	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::UI_BT_FRAME0:
	case Engine::CUI_Parents::UI_BT_FRAME1:
	case Engine::CUI_Parents::UI_BT_FRAME2:
	case Engine::CUI_Parents::UI_BT_FRAME_FAN1:
	case Engine::CUI_Parents::UI_BT_FRAME_FAN2:
	case Engine::CUI_Parents::UI_BT_FRAME4:
	case Engine::CUI_Parents::UI_BT_FRAME5:
	case Engine::CUI_Parents::UI_BT_FRAME6:
	case Engine::CUI_Parents::UI_BT_SLOT1:
	case Engine::CUI_Parents::UI_BT_SLOT2:
	case Engine::CUI_Parents::UI_BT_SLOT3:
	case Engine::CUI_Parents::UI_BT_SLOT4:
	case Engine::CUI_Parents::UI_BT_SLOT2_1:
	case Engine::CUI_Parents::UI_BT_SLOT2_2:
	case Engine::CUI_Parents::UI_BT_SLOT2_3:
	case Engine::CUI_Parents::UI_BT_SLOT2_4:
	case Engine::CUI_Parents::UI_BT_SLOT3_1:
	case Engine::CUI_Parents::UI_BT_SLOT3_2:
	case Engine::CUI_Parents::UI_BT_SLOT3_3:
		CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_fFrame));
		break;

	case Engine::CUI_Parents::UI_BT_FRAME7:
		CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_fFrame));
		break;

	case Engine::CUI_Parents::UI_BT_SELECT:
		CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture"));
		break;
	default:
		break;
	}

	return S_OK;
}

_int CUI_bt_Command::Fnc_On_Slot1(_double TimeDelta)
{
	dSlot_Time += TimeDelta;

	if (ON_SLOT1 == st_iSlot) // Select Slot
	{
		fOn_Slot_Position = 103.f*0.8f;

		if (0.01 < dSlot_Time && false == bOn_Slot)
		{
			if (fOn_Slot_Position <= SetX) // Slot 좌표가 위치에 도착했을 경우
			{
				SetX = fOn_Slot_Position;
				bOn_Slot = true;
				bOff_Slot = false;
			}
			else // Slot 애니매이션
			{
				SetX += 10.0f;
			}
			dSlot_Time = 0.0;
		}

		m_fFrame = 0.f;

		//	================
		// 기능 설정 START ===
		CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
		if (CPlayer::SF_RAGE != curr_form)
		{
			if (m_pGameInstance->Get_KeyEnter(DIK_RIGHT)) // KeyBlade
			{
				m_pWeapon = static_cast<CPlayer_Weapon*>(m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_PlayerWeapon")));

				if (CPlayer_Weapon::WEAPON_TYPE::WEAPON_TYPE_KINGDOM_KEY == m_pWeapon->WeaponType())
				{
					m_pWeapon->WeaponType(CPlayer_Weapon::WEAPON_TYPE::WEAPON_TYPE_FAVORITE_DEPUTY);
					iCom_Texture_Set = 1;
					if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Command_Weapon"), &iCom_Texture_Set)))
						return 0;
					iCom_Texture_Set = 2;
					if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Command_Weapon"), &iCom_Texture_Set)))
						return 0;

				}					
				else if(CPlayer_Weapon::WEAPON_TYPE::WEAPON_TYPE_FAVORITE_DEPUTY == m_pWeapon->WeaponType())
				{
					m_pWeapon->WeaponType(CPlayer_Weapon::WEAPON_TYPE::WEAPON_TYPE_KINGDOM_KEY);
					iCom_Texture_Set = 0;
					if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Command_Weapon"), &iCom_Texture_Set)))
						return 0;
					iCom_Texture_Set = 2;
					if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Command_Weapon"), &iCom_Texture_Set)))
						return 0;
				}
				
				SM->Sound_Command(L"UI_Command_2.wav", 1.f);
			}
		}
		// 기능 설정 END ====
		//	================
	}
	else // Not Select Slot
	{
		fOff_Slot_Position = (103.f - 28.f)*0.8f;

		if (0.01 < dSlot_Time && false == bOff_Slot)
		{
			if (fOff_Slot_Position >= SetX) // Slot 좌표가 위치에 도착했을 경우
			{
				SetX = fOff_Slot_Position;
				bOff_Slot = true;
				bOn_Slot = false;
			}
			else // Slot 애니매이션
			{
				SetX -= 10.f;
			}

			dSlot_Time = 0.0;
		}

		m_fFrame = 1.f;
	}

	return RESULT_NOPROBLEM;
}

_int CUI_bt_Command::Fnc_On_Slot2(_double TimeDelta)
{
	dSlot_Time += TimeDelta;

	if (ON_SLOT2 == st_iSlot) // Select Slot
	{
		fOn_Slot_Position = 103.f*0.8f;

		if (0.01 < dSlot_Time && false == bOn_Slot)
		{
			if (fOn_Slot_Position <= SetX) // Slot 좌표가 위치에 도착했을 경우
			{
				SetX = fOn_Slot_Position;
				bOn_Slot = true;
				bOff_Slot = false;
			}
			else // Slot 애니매이션
			{
				SetX += 10.0f;
			}
			dSlot_Time = 0.0;
		}

		m_fFrame = 0.f;

		//	================
		// 기능 설정 START ===
		CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
		if (CPlayer::SF_RAGE != curr_form)
		{
			if (m_pGameInstance->Get_KeyEnter(DIK_RIGHT) && true == GM->Get_Slot_On_Donald()) // MagicSlot ON
			{
				bOn_Slot = false;
				st_iSlot = ON_SLOT2_1;
				eSlot_State = SLOT_STATE_SECOND_2;
				GM->Set_bFriendIcon(true);

				SM->Sound_Command(L"UI_Command_2.wav", 1.f);
			}
		}
		// 기능 설정 END ====
		//	================
	}
	else // Not Select Slot
	{
		fOff_Slot_Position = (103.f - 28.f)*0.8f;

		if (0.01 < dSlot_Time && false == bOff_Slot)
		{
			if (fOff_Slot_Position >= SetX) // Slot 좌표가 위치에 도착했을 경우
			{
				SetX = fOff_Slot_Position;
				bOff_Slot = true;
				bOn_Slot = false;
			}
			else // Slot 애니매이션
			{
				SetX -= 10.f;
			}
			dSlot_Time = 0.0;
		}

		m_fFrame = 1.f;
	}

	return RESULT_NOPROBLEM;
}

_int CUI_bt_Command::Fnc_On_Slot3(_double TimeDelta)
{
	dSlot_Time += TimeDelta;

	if (ON_SLOT3 == st_iSlot) // Select Slot
	{
		fOn_Slot_Position = 103.f*0.8f;

		if (0.01 < dSlot_Time && false == bOn_Slot)
		{
			if (fOn_Slot_Position <= SetX) // Slot 좌표가 위치에 도착했을 경우
			{
				SetX = fOn_Slot_Position;
				bOn_Slot = true;
				bOff_Slot = false;
			}
			else // Slot 애니매이션
			{
				SetX += 10.0f;
			}
			dSlot_Time = 0.0;
		}

		m_fFrame = 0.f;

		//	================
		// 기능 설정 START ===
		CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
		if (CPlayer::SF_RAGE != curr_form)
		{
			if (m_pGameInstance->Get_KeyEnter(DIK_RIGHT)) // Item ON
			{
				Inven->Set_QuickItem_Text(true);

				bOn_Slot = false;
				st_iSlot = ON_SLOT3_1;
				eSlot_State = SLOT_STATE_SECOND_3;

				SM->Sound_Command(L"UI_Command_2.wav", 1.f);
			}
		}
		// 기능 설정 END ====
		//	================
	}
	else // Not Select Slot
	{
		fOff_Slot_Position = (103.f - 28.f)*0.8f;

		if (0.01 < dSlot_Time && false == bOff_Slot)
		{
			if (fOff_Slot_Position >= SetX) // Slot 좌표가 위치에 도착했을 경우
			{
				SetX = fOff_Slot_Position;
				bOff_Slot = true;
				bOn_Slot = false;
			}
			else // Slot 애니매이션
			{
				SetX -= 10.f;
			}

			dSlot_Time = 0.0;
		}

		m_fFrame = 1.f;
	}

	return RESULT_NOPROBLEM;
}

_int CUI_bt_Command::Fnc_On_Slot4(_double TimeDelta)
{
	dSlot_Time += TimeDelta;

	if (ON_SLOT4 == st_iSlot) // Select Slot
	{
		fOn_Slot_Position = 103.f*0.8f;

		if (0.01 < dSlot_Time && false == bOn_Slot)
		{
			if (fOn_Slot_Position <= SetX) // Slot 좌표가 위치에 도착했을 경우
			{
				SetX = fOn_Slot_Position;
				bOn_Slot = true;
				bOff_Slot = false;
			}
			else // Slot 애니매이션
			{
				SetX += 10.0f;
			}
			dSlot_Time = 0.0;
		}

		m_fFrame = 0.f;

		//	================
		// 기능 설정 START ===
		CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
		if (CPlayer::SF_RAGE != curr_form)
		{
			if (m_pGameInstance->Get_KeyEnter(DIK_RIGHT)) // ??? ON
			{

				SM->Sound_Command(L"UI_Command_2.wav", 1.f);
			}
		}
		// 기능 설정 END ====
		//	================
	}
	else // Not Select Slot
	{
		fOff_Slot_Position = (103.f - 28.f)*0.8f;

		if (0.01 < dSlot_Time && false == bOff_Slot)
		{
			if (fOff_Slot_Position >= SetX) // Slot 좌표가 위치에 도착했을 경우
			{
				SetX = fOff_Slot_Position;
				bOff_Slot = true;
				bOn_Slot = false;
			}
			else // Slot 애니매이션
			{
				SetX -= 10.f;
			}

			dSlot_Time = 0.0;
		}

		m_fFrame = 1.f;
	}

	return RESULT_NOPROBLEM;
}

/* =========================================================== */
/* 소환 ====================================================== */
_int CUI_bt_Command::Fnc_On_Slot2_1(_double TimeDelta)
{
	if (true == GM->Get_KeyBladeLock())
		return RESULT_NOPROBLEM;

	if(CPlayer::SF_RAGE == m_pPlayer->Get_Form())
		return RESULT_NOPROBLEM;

	dSlot_Time += TimeDelta;

	if (ON_SLOT2_1 == st_iSlot)
	{
		fOn_Slot_Position = (168.f + 310.f)*0.8f;

		if (0.01 < dSlot_Time && false == bOn_Slot)
		{
			if (fOff_Slot_Position <= SetX)
			{
				SetX = fOn_Slot_Position;
				bOn_Slot = true;
				bOff_Slot = false;
			}
			else
			{
				SetX += 10.f;
			}
			dSlot_Time = 0.0;
		}
		//==

		m_fFrame = 2.f;


		//	================
		// 기능 설정 START ===

		if (m_pGameInstance->Get_KeyEnter(DIK_LEFT)) // MagicSlot 끄기
		{
			GM->Set_bFriendIcon(false);
			eSlot_State = SLOT_STATE_FIRST;
			st_iSlot = ON_SLOT1;

			SM->Sound_Command(L"UI_Command_1.wav", 1.f);
		}

		if (m_pGameInstance->Get_KeyEnter(DIK_RETURN)) //
		{
			if (50.f <= static_cast<CStatus*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_Player"), TEXT("Com_Status")))->Get_MP())
			{
				// 기능 ===
				_uint eState = CGoofy::STATE_RUN;
				if (FAILED(m_pGameInstance->ThrowToLayer(LEVEL_STATIC, TEXT("Layer_Goofy"), &eState)))
				{
					BREAKPOINT;
					return RESULT_ERROR;
				}
				// ========
				GM->Set_bFriendIcon(false);
				eSlot_State = SLOT_STATE_FIRST;
				st_iSlot = ON_SLOT1;

				SM->Sound_Command(L"UI_Command_2.wav", 1.f);

				m_pPlayer->Set_Input(CPlayer::SORA_INPUT::SI_SUMMON, true);
				m_pPlayer->Set_Magic(CPlayer::SORA_MAGIC::SM_GOOFY1);
			}


		}

		// 기능 설정 END =====
		//	================
	}
	else
	{
		fOff_Slot_Position = (168.f + 294.f)*0.8f;

		if (0.01 < dSlot_Time && false == bOff_Slot)
		{
			if (fOff_Slot_Position >= SetX)
			{
				SetX = fOff_Slot_Position;
				bOff_Slot = true;
				bOn_Slot = false;
			}
			else
			{
				SetX -= 10.f;
			}
			dSlot_Time = 0.0;
		}
		//==

		m_fFrame = 3.f;
	}

	return RESULT_NOPROBLEM;
}

_int CUI_bt_Command::Fnc_On_Slot2_2(_double TimeDelta)
{
	if (true == GM->Get_KeyBladeLock())
		return RESULT_NOPROBLEM;

	if (CPlayer::SF_RAGE == m_pPlayer->Get_Form())
		return RESULT_NOPROBLEM;

	dSlot_Time += TimeDelta;

	if (ON_SLOT2_2 == st_iSlot)
	{
		fOn_Slot_Position = (168.f + 310.f)*0.8f;

		if (0.01 < dSlot_Time && false == bOn_Slot)
		{
			if (fOff_Slot_Position <= SetX)
			{
				SetX = fOn_Slot_Position;
				bOn_Slot = true;
				bOff_Slot = false;
			}
			else
			{
				SetX += 10.f;
			}
			dSlot_Time = 0.0;
		}
		//==

		m_fFrame = 2.f;

		//	================
		// 기능 설정 START ===

		if (m_pGameInstance->Get_KeyEnter(DIK_LEFT)) // MagicSlot 끄기
		{
			GM->Set_bFriendIcon(false);
			eSlot_State = SLOT_STATE_FIRST;
			st_iSlot = ON_SLOT1;

			SM->Sound_Command(L"UI_Command_1.wav", 1.f);
		}

		if (m_pGameInstance->Get_KeyEnter(DIK_RETURN)) //
		{
			if (50.f <= static_cast<CStatus*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_Player"), TEXT("Com_Status")))->Get_MP())
			{

				// 기능 ===
				_uint eState = CGoofy::STATE_THROWSTART;
				if (FAILED(m_pGameInstance->ThrowToLayer(LEVEL_STATIC, TEXT("Layer_Goofy"), &eState)))
				{
					BREAKPOINT;
					return RESULT_ERROR;
				}
				// ========
				GM->Set_bFriendIcon(false);
				eSlot_State = SLOT_STATE_FIRST;
				st_iSlot = ON_SLOT1;

				SM->Sound_Command(L"UI_Command_2.wav", 1.f);

				m_pPlayer->Set_Input(CPlayer::SORA_INPUT::SI_SUMMON, true);
				m_pPlayer->Set_Magic(CPlayer::SORA_MAGIC::SM_GOOFY2);
			}
		}
		// 기능 설정 END =====
		//	================
	}
	else
	{
		fOff_Slot_Position = (168.f + 294.f)*0.8f;

		if (0.01 < dSlot_Time && false == bOff_Slot)
		{
			if (fOff_Slot_Position >= SetX)
			{
				SetX = fOff_Slot_Position;
				bOff_Slot = true;
				bOn_Slot = false;
			}
			else
			{
				SetX -= 10.f;
			}
			dSlot_Time = 0.0;
		}
		//==

		m_fFrame = 3.f;
	}

	return RESULT_NOPROBLEM;
}

_int CUI_bt_Command::Fnc_On_Slot2_3(_double TimeDelta)
{
	if (true == GM->Get_KeyBladeLock())
		return RESULT_NOPROBLEM;

	if (CPlayer::SF_RAGE == m_pPlayer->Get_Form())
		return RESULT_NOPROBLEM;

	dSlot_Time += TimeDelta;

	if (ON_SLOT2_3 == st_iSlot)
	{
		fOn_Slot_Position = (168.f + 310.f)*0.8f;

		if (0.01 < dSlot_Time && false == bOn_Slot)
		{
			if (fOff_Slot_Position <= SetX)
			{
				SetX = fOn_Slot_Position;
				bOn_Slot = true;
				bOff_Slot = false;
			}
			else
			{
				SetX += 10.f;
			}
			dSlot_Time = 0.0;
		}
		//==

		m_fFrame = 0.f;

		//	================
		// 기능 설정 START ===

		if (m_pGameInstance->Get_KeyEnter(DIK_LEFT)) // MagicSlot 끄기
		{
			GM->Set_bFriendIcon(false);
			eSlot_State = SLOT_STATE_FIRST;
			st_iSlot = ON_SLOT1;

			SM->Sound_Command(L"UI_Command_1.wav", 1.f);
		}

		if (m_pGameInstance->Get_KeyEnter(DIK_RETURN)) //
		{
			if (50.f <= static_cast<CStatus*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_Player"), TEXT("Com_Status")))->Get_MP())
			{
				// 기능 ===
				_uint eState = CDonald::STATE_ATTACK;
				if (FAILED(m_pGameInstance->ThrowToLayer(LEVEL_STATIC, TEXT("Layer_Donald"), &eState)))
				{
					BREAKPOINT;
					return RESULT_ERROR;
				}
				// ========
				GM->Set_bFriendIcon(false);
				eSlot_State = SLOT_STATE_FIRST;
				st_iSlot = ON_SLOT1;

				SM->Sound_Command(L"UI_Command_2.wav", 1.f);

				m_pPlayer->Set_Input(CPlayer::SORA_INPUT::SI_SUMMON, true);
				m_pPlayer->Set_Magic(CPlayer::SORA_MAGIC::SM_DONALD1);
			}
		}
		// 기능 설정 END =====
		//	================
	}
	else
	{
		fOff_Slot_Position = (168.f + 294.f)*0.8f;

		if (0.01 < dSlot_Time && false == bOff_Slot)
		{
			if (fOff_Slot_Position >= SetX)
			{
				SetX = fOff_Slot_Position;
				bOff_Slot = true;
				bOn_Slot = false;
			}
			else
			{
				SetX -= 10.f;
			}
			dSlot_Time = 0.0;
		}
		//==

		m_fFrame = 1.f;
	}


	return RESULT_NOPROBLEM;
}

_int CUI_bt_Command::Fnc_On_Slot2_4(_double TimeDelta)
{
	if (true == GM->Get_KeyBladeLock())
		return RESULT_NOPROBLEM;

	if (CPlayer::SF_RAGE == m_pPlayer->Get_Form())
		return RESULT_NOPROBLEM;

	dSlot_Time += TimeDelta;

	if (ON_SLOT2_4 == st_iSlot)
	{
		fOn_Slot_Position = (168.f + 310.f)*0.8f;

		if (0.01 < dSlot_Time && false == bOn_Slot)
		{
			if (fOff_Slot_Position <= SetX)
			{
				SetX = fOn_Slot_Position;
				bOn_Slot = true;
				bOff_Slot = false;
			}
			else
			{
				SetX += 10.f;
			}
			dSlot_Time = 0.0;
		}
		//==

		m_fFrame = 0.f;

		//	================
		// 기능 설정 START ===

		if (m_pGameInstance->Get_KeyEnter(DIK_LEFT)) // MagicSlot 끄기
		{
			GM->Set_bFriendIcon(false);
			eSlot_State = SLOT_STATE_FIRST;
			st_iSlot = ON_SLOT1;

			SM->Sound_Command(L"UI_Command_1.wav", 1.f);
		}

		if (m_pGameInstance->Get_KeyEnter(DIK_RETURN)) // 
		{
			if (50.f <= static_cast<CStatus*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_Player"), TEXT("Com_Status")))->Get_MP())
			{
				// 기능 ===
				_uint eState = CDonald::STATE_DANCE;
				if (FAILED(m_pGameInstance->ThrowToLayer(LEVEL_STATIC, TEXT("Layer_Donald"), &eState)))
				{
					BREAKPOINT;
					return RESULT_ERROR;
				}
				// ========
				GM->Set_bFriendIcon(false);
				eSlot_State = SLOT_STATE_FIRST;
				st_iSlot = ON_SLOT1;

				SM->Sound_Command(L"UI_Command_2.wav", 1.f);

				m_pPlayer->Set_Input(CPlayer::SORA_INPUT::SI_SUMMON, true);
				m_pPlayer->Set_Magic(CPlayer::SORA_MAGIC::SM_DONALD2);
			}
		}
		// 기능 설정 END =====
		//	================
	}
	else
	{
		fOff_Slot_Position = (168.f + 294.f)*0.8f;

		if (0.01 < dSlot_Time && false == bOff_Slot)
		{
			if (fOff_Slot_Position >= SetX)
			{
				SetX = fOff_Slot_Position;
				bOff_Slot = true;
				bOn_Slot = false;
			}
			else
			{
				SetX -= 10.f;
			}
			dSlot_Time = 0.0;
		}
		//==

		m_fFrame = 1.f;
	}

	return RESULT_NOPROBLEM;
}

/* 소환END =================================================== */
/* =========================================================== */

/* =========================================================== */
/* 아이템 ====================================================== */

_int CUI_bt_Command::Fnc_On_Slot3_1(_double TimeDelta)
{
	dSlot_Time += TimeDelta;

	if (ON_SLOT3_1 == st_iSlot)
	{
		fOn_Slot_Position = (168.f + 310.f)*0.8f;

		if (0.01 < dSlot_Time && false == bOn_Slot)
		{
			if (fOff_Slot_Position <= SetX)
			{
				SetX = fOn_Slot_Position;
				bOn_Slot = true;
				bOff_Slot = false;
			}
			else
			{
				SetX += 10.f;
			}
			dSlot_Time = 0.0;
		}
		//==

		m_fFrame = 4.f;


		//	================
		// 기능 설정 START ===

		if (m_pGameInstance->Get_KeyEnter(DIK_LEFT)) // ItemSlot 끄기
		{
			Inven->Set_QuickItem_Text(false);
			eSlot_State = SLOT_STATE_FIRST;
			st_iSlot = ON_SLOT1;

			SM->Sound_Command(L"UI_Command_1.wav", 1.f);
		}

		if (m_pGameInstance->Get_KeyEnter(DIK_RETURN)) //
		{
			// 기능 ===
			if (0 < Inven->Get_Slot1())
			{
				Fnc_Slot3_Qucik(Inven->Get_Slot1_Name());
				Inven->Set_Slot1_Name(SLOT_EMPTY);
				Inven->Set_Slot1(0);

				Inven->Inven_sort(0);/* 갯수 업데이트 */

				SM->Sound_Command(L"Command_Item.wav", 1.f);
			}

			// ========
			Inven->Set_QuickItem_Text(false);
			eSlot_State = SLOT_STATE_FIRST;
			st_iSlot = ON_SLOT1;

		}

		// 기능 설정 END =====
		//	================
	}
	else
	{
		fOff_Slot_Position = (168.f + 294.f)*0.8f;

		if (0.01 < dSlot_Time && false == bOff_Slot)
		{
			if (fOff_Slot_Position >= SetX)
			{
				SetX = fOff_Slot_Position;
				bOff_Slot = true;
				bOn_Slot = false;
			}
			else
			{
				SetX -= 10.f;
			}
			dSlot_Time = 0.0;
		}
		//==

		m_fFrame = 5.f;
	}

	return RESULT_NOPROBLEM;
}

_int CUI_bt_Command::Fnc_On_Slot3_2(_double TimeDelta)
{
	dSlot_Time += TimeDelta;

	if (ON_SLOT3_2 == st_iSlot)
	{
		fOn_Slot_Position = (168.f + 310.f)*0.8f;

		if (0.01 < dSlot_Time && false == bOn_Slot)
		{
			if (fOff_Slot_Position <= SetX)
			{
				SetX = fOn_Slot_Position;
				bOn_Slot = true;
				bOff_Slot = false;
			}
			else
			{
				SetX += 10.f;
			}
			dSlot_Time = 0.0;
		}
		//==

		m_fFrame = 4.f;


		//	================
		// 기능 설정 START ===

		if (m_pGameInstance->Get_KeyEnter(DIK_LEFT)) // ItemSlot 끄기
		{
			Inven->Set_QuickItem_Text(false);
			eSlot_State = SLOT_STATE_FIRST;
			st_iSlot = ON_SLOT1;

			SM->Sound_Command(L"UI_Command_1.wav", 1.f);
		}

		if (m_pGameInstance->Get_KeyEnter(DIK_RETURN)) //
		{
			// 기능 ===
			if (0 < Inven->Get_Slot2())
			{
				Fnc_Slot3_Qucik(Inven->Get_Slot2_Name());
				Inven->Set_Slot2_Name(SLOT_EMPTY);
				Inven->Set_Slot2(0);

				Inven->Inven_sort(0);/* 갯수 업데이트 */

				SM->Sound_Command(L"Command_Item.wav", 1.f);
			}

			// ========
			Inven->Set_QuickItem_Text(false);
			eSlot_State = SLOT_STATE_FIRST;
			st_iSlot = ON_SLOT1;

		
		}

		// 기능 설정 END =====
		//	================
	}
	else
	{
		fOff_Slot_Position = (168.f + 294.f)*0.8f;

		if (0.01 < dSlot_Time && false == bOff_Slot)
		{
			if (fOff_Slot_Position >= SetX)
			{
				SetX = fOff_Slot_Position;
				bOff_Slot = true;
				bOn_Slot = false;
			}
			else
			{
				SetX -= 10.f;
			}
			dSlot_Time = 0.0;
		}
		//==

		m_fFrame = 5.f;
	}

	return RESULT_NOPROBLEM;
}

_int CUI_bt_Command::Fnc_On_Slot3_3(_double TimeDelta)
{
	dSlot_Time += TimeDelta;

	if (ON_SLOT3_3 == st_iSlot)
	{
		fOn_Slot_Position = (168.f + 310.f)*0.8f;

		if (0.01 < dSlot_Time && false == bOn_Slot)
		{
			if (fOff_Slot_Position <= SetX)
			{
				SetX = fOn_Slot_Position;
				bOn_Slot = true;
				bOff_Slot = false;
			}
			else
			{
				SetX += 10.f;
			}
			dSlot_Time = 0.0;
		}
		//==

		m_fFrame = 4.f;


		//	================
		// 기능 설정 START ===

		if (m_pGameInstance->Get_KeyEnter(DIK_LEFT)) // ItemSlot 끄기
		{
			Inven->Set_QuickItem_Text(false);
			eSlot_State = SLOT_STATE_FIRST;
			st_iSlot = ON_SLOT1;

			SM->Sound_Command(L"UI_Command_1.wav", 1.f);
		}

		if (m_pGameInstance->Get_KeyEnter(DIK_RETURN)) //
		{
			// 기능 ===
			if (0 < Inven->Get_Slot3())
			{
				Fnc_Slot3_Qucik(Inven->Get_Slot3_Name());
				Inven->Set_Slot3_Name(SLOT_EMPTY);
				Inven->Set_Slot3(0);

				Inven->Inven_sort(0);/* 갯수 업데이트 */

				SM->Sound_Command(L"Command_Item.wav", 1.f);
			}
			// ========
			Inven->Set_QuickItem_Text(false);
			eSlot_State = SLOT_STATE_FIRST;
			st_iSlot = ON_SLOT1;

		}

		// 기능 설정 END =====
		//	================
	}
	else
	{
		fOff_Slot_Position = (168.f + 294.f)*0.8f;

		if (0.01 < dSlot_Time && false == bOff_Slot)
		{
			if (fOff_Slot_Position >= SetX)
			{
				SetX = fOff_Slot_Position;
				bOff_Slot = true;
				bOn_Slot = false;
			}
			else
			{
				SetX -= 10.f;
			}
			dSlot_Time = 0.0;
		}
		//==

		m_fFrame = 5.f;
	}

	return RESULT_NOPROBLEM;
}

void CUI_bt_Command::Fnc_Slot3_Qucik(_int _Name)
{
	if (SLOT_EMPTY == _Name)
	{

	}
	else if (SLOT_POTION == _Name)
	{
		/* 포션 효과 */
		static_cast<CStatus*>(GM->Get_Player()->Get_Component(TEXT("Com_Status")))->Heal(0.25f);
		Inven->Math_Potion(-1);
		if (0 >= Inven->Get_Potion())
			QuickSlot_InvenSort(_Name);

		CEffect_Manager::GetInstance()->Create_PlayerHealEffect(GM->Get_Player()->Get_Transform());
	}
	else if (SLOT_HIGH_POTION == _Name)
	{
		/* 하이포션 효과 */
		static_cast<CStatus*>(GM->Get_Player()->Get_Component(TEXT("Com_Status")))->Heal(0.5f);
		Inven->Math_High_Potion(-1);
		if (0 >= Inven->Get_High_Potion())
			QuickSlot_InvenSort(_Name);

		CEffect_Manager::GetInstance()->Create_PlayerHealEffect_Full(GM->Get_Player()->Get_Transform());
	}
	else if (SLOT_ELIXIR == _Name)
	{
		/* 엘릭서 효과 */
		static_cast<CStatus*>(GM->Get_Player()->Get_Component(TEXT("Com_Status")))->HealMP(0.5f);
		Inven->Math_Elixir(-1);
		if (0 >= Inven->Get_Elixir())
			QuickSlot_InvenSort(_Name);

		CEffect_Manager::GetInstance()->Create_PlayerHealEffect_MP(GM->Get_Player()->Get_Transform());
	}
	else if (SLOT_ATKUP == _Name)
	{
		/* 공격력 UP 효과 */
		if (true == GM->Get_Player()->Set_ItemValues(CPlayer::PLAYER_ITEM::PLAYER_ITEM_DAMAGE))
			GM->Get_Player()->Apply_Item(CPlayer::PLAYER_ITEM::PLAYER_ITEM_DAMAGE);
		
		Inven->Math_AtkUp(-1);
		if (0 >= Inven->Get_AtkUp())
			QuickSlot_InvenSort(_Name);
	}
	else if (SLOT_ATKSPEEDUP == _Name)
	{
		/* 공격속도 UP 효과 */
		if (true == GM->Get_Player()->Set_ItemValues(CPlayer::PLAYER_ITEM::PLAYER_ITEM_ATTACK_SPEED))
			GM->Get_Player()->Apply_Item(CPlayer::PLAYER_ITEM::PLAYER_ITEM_ATTACK_SPEED);

		Inven->Math_AtkSpeedUp(-1);
		if (0 >= Inven->Get_AtkSpeedUp())
			QuickSlot_InvenSort(_Name);
	}
	else if (SLOT_SPEEDUP == _Name)
	{
		/* 스피드 UP 효과 */
		if (true == GM->Get_Player()->Set_ItemValues(CPlayer::PLAYER_ITEM::PLAYER_ITEM_MOVE_SPEED))
			GM->Get_Player()->Apply_Item(CPlayer::PLAYER_ITEM::PLAYER_ITEM_MOVE_SPEED);

		Inven->Math_SpeedUp(-1);
		if (0 >= Inven->Get_SpeedUp())
			QuickSlot_InvenSort(_Name);
	}
	else
	{
	}
}

void CUI_bt_Command::QuickSlot_InvenSort(_int _Name)
{
	if (_Name == Inven->Get_InvenSlot1_Name())
	{
		Inven->Set_InvenSlot1_Name(SLOT_EMPTY);
	}
	else if (_Name == Inven->Get_InvenSlot2_Name())
	{
		Inven->Set_InvenSlot2_Name(SLOT_EMPTY);
	}
	else if (_Name == Inven->Get_InvenSlot3_Name())
	{
		Inven->Set_InvenSlot3_Name(SLOT_EMPTY);
	}
	else if (_Name == Inven->Get_InvenSlot4_Name())
	{
		Inven->Set_InvenSlot4_Name(SLOT_EMPTY);
	}
	else if (_Name == Inven->Get_InvenSlot5_Name())
	{
		Inven->Set_InvenSlot5_Name(SLOT_EMPTY);
	}
	else if (_Name == Inven->Get_InvenSlot6_Name())
	{
		Inven->Set_InvenSlot6_Name(SLOT_EMPTY);
	}
}

//========================================================================
//========================================================================
//========================================================================

CUI_bt_Command * CUI_bt_Command::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_bt_Command*	pInstance = new CUI_bt_Command(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_bt_Command");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_bt_Command::Clone(void * pArg)
{
	CUI_bt_Command*	pInstance = new CUI_bt_Command(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_bt_Command");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_bt_Command::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
