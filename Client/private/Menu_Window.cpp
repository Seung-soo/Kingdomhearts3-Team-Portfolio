#include "stdafx.h"
#include "..\public\Menu_Window.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "InvenManager.h"
#include "UI_Num_Manager.h"
#include "SoundManager.h"

/* ITEMWINDOW 1 */
static _float ItemWindow1_X = 0.f;
static _float ItemWindow1_Y = 0.f;
static _bool ItemWindow1Delete = false;
static _bool ShopWindowDelete = false; // 구매,판매 UI
static _bool ShopWindowDelete_Up = false; // 상단 UI
static _bool ShopWindowDelete_List = false; // 구매 List
static _int WindowLevel = 0;
/* ============ */
static _float MovePositionX = 0.f;
static _float MovePositionY = 0.f;
/* ============ */

CMenu_Window::CMenu_Window(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CMenu_Window::CMenu_Window(const CMenu_Window & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CMenu_Window::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CMenu_Window::NativeConstruct(void * pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

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

	SettingX = m_fX - (g_iWinCX*0.5f);
	SettingY = m_fY - (g_iWinCY*0.5f);

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::ITEMWINDOW0:
		/* X:500*1.4 ,Y:420*1.4 */
		SizeX = 800 * 1.4f;
		SizeY = 5.f * 1.4f;
		SetX = 450.f;
		SetY = 370.f;

		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));
		m_fFrame = 0.f;
		m_fDepth = 0.05f; // 깊이



		/* 커서 생성 */
		iCom_Texture_Set = CUI_Parents::MENU_WINDOW::WINDOW_SELECT;
		if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
			return E_FAIL;

		/* Move Native Position */
		MovePositionX = 5.f;
		MovePositionY = 5.f;

		break;

	case Engine::CUI_Parents::ITEMWINDOW1:
		SizeX = 64.f * 1.4f;
		SizeY = 64.f * 1.4f;
		SetX = 0.f;
		SetY = 0.f;

		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));
		m_fFrame = 1.f;
		m_fDepth = 0.05f; // 깊이
		break;

	case Engine::CUI_Parents::SHOPWINDOW0:
		SizeX = 500.f * 1.4f;
		SizeY = 420.f * 1.4f;
		SetX = 450.f;
		SetY = 400.f;

		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));
		m_fFrame = 0.f;
		m_fDepth = 0.05f; // 깊이

		/* Move Native Position */
		//MovePositionX = 5.f;
		//MovePositionY = 5.f;

		/* 커서 생성 */
		iCom_Texture_Set = CUI_Parents::MENU_WINDOW::WINDOW_SELECT;
		if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
			return E_FAIL;

		break;
	case Engine::CUI_Parents::SHOPWINDOW1:

		SizeX = 500.f * 1.4f;
		SizeY = 420.f * 1.4f;
		SetX = 450.f;
		SetY = 400.f;

		m_fFrame = 1.f;
		m_fDepth = 0.03f; // 깊이

		break;
	case Engine::CUI_Parents::SHOPWINDOW2:

		SizeX = 500.f * 1.4f;
		SizeY = 420.f * 1.4f;
		SetX = 450.f;
		SetY = 400.f;

		m_fFrame = 2.f;
		m_fDepth = 0.03f; // 깊이

		break;
	case Engine::CUI_Parents::SHOPWINDOW3:

		SizeX = 500.f * 1.4f;
		SizeY = 420.f * 1.4f;
		SetX = 450.f;
		SetY = 400.f;

		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));
		m_fFrame = 3.f;
		m_fDepth = 0.05f; // 깊이

		break;
	case Engine::CUI_Parents::SHOPWINDOW4:

		SizeX = 500.f * 1.4f;
		SizeY = 420.f * 1.4f;
		SetX = 450.f;
		SetY = 400.f;

		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));
		m_fFrame = 4.f;
		m_fDepth = 0.05f; // 깊이

		break;
	case Engine::CUI_Parents::SHOPWINDOW5:

		SizeX = 500.f * 1.4f;
		SizeY = 420.f * 1.4f;
		SetX = 450.f;
		SetY = 400.f;

		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));
		m_fFrame = 5.f;
		m_fDepth = 0.04f; // 깊이

		break;

	case Engine::CUI_Parents::SHOPWINDOW6:

		SizeX = 500.f * 1.4f;
		SizeY = 420.f * 1.4f;
		SetX = 450.f;
		SetY = 400.f;

		m_fFrame = 6.f;
		m_fDepth = 0.01f; // 깊이

		break;
	case Engine::CUI_Parents::SHOPWINDOW7:
		SizeX = 500.f * 1.4f;
		SizeY = 420.f * 1.4f;
		SetX = 450.f;
		SetY = 400.f;

		m_fFrame = 7.f;
		m_fDepth = 0.01f; // 깊이
		break;
	case Engine::CUI_Parents::SHOPWINDOW8:
		SizeX = 500.f * 1.4f;
		SizeY = 420.f * 1.4f;
		SetX = 450.f;
		SetY = 400.f;

		m_fFrame = 8.f;
		m_fDepth = 0.01f; // 깊이
		break;
	case Engine::CUI_Parents::WINDOW_SELECT:

		SizeX = 8.f * 1.4f;
		SizeY = 16.f * 1.4f;
		SetX = 0.f;
		SetY = 0.f;

		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));

		m_fDepth = 0.f; // 깊이
		break;

	default:
		break;
	}


	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	return S_OK;
}

_int CMenu_Window::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	_int inttest = 0;

	/* =========== */

	/* Move Random Position */

	dRandomTime += TimeDelta;

	if (0.005 <= dRandomTime)
	{
		MovePositionX += fRandomX;
		MovePositionY += fRandomY;

		dRandomTime = 0.0;
	}

	if (false == bRandomX && 5.f <= MovePositionX || -5.f >= MovePositionX)
	{
		if (10.f <= MovePositionX)
			MovePositionX = 4.999f;
		if (-10 >= MovePositionX)
			MovePositionX = -4.999f;

		bRandomX = true;
	}
	if (false == bRandomY && 5.f <= MovePositionY || -5.f >= MovePositionY)
	{
		if (10.f <= MovePositionY)
			MovePositionY = 4.999f;
		if (-10 >= MovePositionY)
			MovePositionY = -4.999f;

		bRandomY = true;
	}

	if (true == bRandomX)
	{
		if (0 == iXCnt)
			fRandomX = (_float)((rand() % 9 + 1.f) * 0.005f);
		if (1 == iXCnt)
			fRandomX = -1.f * (_float)((rand() % 9 + 1.f) * 0.005f);

		iXCnt += 1;
		if (2 <= iXCnt)
			iXCnt = 0;

		bRandomX = false;
	}
	if (true == bRandomY)
	{
		if (0 == iYCnt)
			fRandomY = (_float)((rand() % 9 + 1.f) * 0.005f);
		if (1 == iYCnt)
			fRandomY = -1.f * (_float)((rand() % 9 + 1.f) * 0.005f);
		
		iYCnt += 1;
		if (2 <= iYCnt)
			iYCnt = 0;

		bRandomY = false;
	}

	/* ==================== */

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::ITEMWINDOW0:

		/* Animation === */
		dItemTime_First += TimeDelta;

		if (true == bCreate)
		{
			switch (iItemPatternCnt)
			{
			case 0:
				/* 5 700 */
				dItemTime_First += TimeDelta;

				if (dSetTime < dItemTime_First)
				{
					fItemWindowSpeedX *= fSpeedSet_1;
					fItemWindowSpeedY *= fSpeedSet_2;

					SizeX -= fItemWindowSpeedX;
					SizeY += fItemWindowSpeedY;

					dItemTime_First = 0.0;
				}

				if (5.f >= SizeX || 700.f <= SizeY)
				{
					//SizeX = 5.f * 1.4f;
					//SizeY = 700.f * 1.4f;
					SizeX = 5.f;
					SizeY = 700.f;

					fItemWindowSpeedX = 1.f;
					fItemWindowSpeedY = 1.f;

					iItemPatternCnt = 1;
				}

				break;

			case 1:
				/* 700 100 */
				dItemTime_First += TimeDelta;

				if (dSetTime < dItemTime_First)
				{
					fItemWindowSpeedX *= fSpeedSet_2;
					fItemWindowSpeedY *= fSpeedSet_1;

					SizeX += fItemWindowSpeedX;
					SizeY -= fItemWindowSpeedY;

					dItemTime_First = 0.0;
				}

				if (700.f <= SizeX || 100.f >= SizeY)
				{
					//SizeX = 700.f * 1.4f;
					//SizeY = 100.f * 1.4f;
					SizeX = 700.f;
					SizeY = 100.f;

					fItemWindowSpeedX = 1.f;
					fItemWindowSpeedY = 1.f;

					iItemPatternCnt = 2;
				}
				break;

			case 2:
				/* 200 250 */
				dItemTime_First += TimeDelta;

				if (dSetTime < dItemTime_First)
				{
					fItemWindowSpeedX *= fSpeedSet_1;
					fItemWindowSpeedY *= fSpeedSet_2;

					SizeX -= fItemWindowSpeedX;
					SizeY += fItemWindowSpeedY;

					dItemTime_First = 0.0;
				}

				if (200.f >= SizeX || 250.f <= SizeY)
				{
					//SizeX = 200.f*1.4f;
					//SizeY = 250.f*1.4f;
					SizeX = 200.f;
					SizeY = 250.f;

					fItemWindowSpeedX = 1.f;
					fItemWindowSpeedY = 1.f;

					iItemPatternCnt = 3;
				}
				break;

			case 3:
				/* 500 420 */
				dItemTime_First += TimeDelta;

				if (dSetTime < dItemTime_First)
				{
					fItemWindowSpeedX *= fSpeedSet_1;
					fItemWindowSpeedY *= fSpeedSet_1;

					SizeX += fItemWindowSpeedX;
					SizeY += fItemWindowSpeedY;

					dItemTime_First = 0.0;
				}

				if (620.f <= SizeX || 380.f <= SizeY)
				{
					//SizeX = 520.f*1.4f;
					//SizeY = 420.f*1.4f;
					SizeX = 620.f;
					SizeY = 380.f;

					fItemWindowSpeedX = 1.f;
					fItemWindowSpeedY = 1.f;

					iItemPatternCnt = 4;
				}
				break;

			case 4:
				SizeX = 500.f * 1.4f;
				SizeY = 420.f * 1.4f;
				SetX = 450.f;
				SetY = 370.f;

				/* Inven List 생성 */
				Inven->Set_InvenNumRender(true);

				iCom_Texture_Set = CUI_Parents::MENU_WINDOW::ITEMWINDOW_SLOT1;
				if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_ItemText"), &iCom_Texture_Set)))
					return E_FAIL;

				iCom_Texture_Set = CUI_Parents::MENU_WINDOW::ITEMWINDOW_SLOT2;
				if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_ItemText"), &iCom_Texture_Set)))
					return E_FAIL;

				iCom_Texture_Set = CUI_Parents::MENU_WINDOW::ITEMWINDOW_SLOT3;
				if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_ItemText"), &iCom_Texture_Set)))
					return E_FAIL;

				iCom_Texture_Set = CUI_Parents::MENU_WINDOW::ITEMWINDOW_SLOT4;
				if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_ItemText"), &iCom_Texture_Set)))
					return E_FAIL;

				iCom_Texture_Set = CUI_Parents::MENU_WINDOW::ITEMWINDOW_SLOT5;
				if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_ItemText"), &iCom_Texture_Set)))
					return E_FAIL;

				iCom_Texture_Set = CUI_Parents::MENU_WINDOW::ITEMWINDOW_SLOT6;
				if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_ItemText"), &iCom_Texture_Set)))
					return E_FAIL;

				iItemPatternCnt = 5;

				break;

			case 5:

			default:
				break;
			}
		}
		else
		{
			switch (iItemPatternCnt)
			{
			case 0:
				/* 500 420 */
				Inven->Set_InvenNumRender(false);

				dItemTime_First += TimeDelta;

				if (dSetTime < dItemTime_First)
				{
					fItemWindowSpeedX *= fSpeedSet_2;
					fItemWindowSpeedY *= fSpeedSet_1;

					SizeX += fItemWindowSpeedX;
					SizeY -= fItemWindowSpeedY;

					dItemTime_First = 0.0;
				}

				if (5.f >= SizeY)
				{
					SizeY = 5.f;
					m_bDead = true;
				}
				break;

		
			default:
				break;
			}
		}
		/* Animation_END=== */
		

		/* ============= */

		if (m_pGameInstance->Get_KeyEnter(DIK_BACKSPACE) && 0 == WindowLevel)
		{
			Inven->Set_InvenDelete_List(true);
			Inven->Set_ItemWindow(false);
			Inven->Set_InvenDelete(true);

			SM->Sound_Command(L"UI_Menu_Close.wav", 1.f);

			iItemPatternCnt = 0;
			bCreate = false;
		}

		break;
	case Engine::CUI_Parents::ITEMWINDOW1:

		if (true == ItemWindow1Delete)
		{
			ItemWindow1Delete = false;
			m_bDead = true;
		}
			
		
		SetX = ItemWindow1_X;
		SetY = ItemWindow1_Y;

		inttest = 10;
		break;
	case Engine::CUI_Parents::SHOPWINDOW0:

		/* Animation === */
		dItemTime_First += TimeDelta;

		if (true == bCreate)
		{
			switch (iItemPatternCnt)
			{
			case 0:
				/* 5 700 */
				dItemTime_First += TimeDelta;

				if (dSetTime < dItemTime_First)
				{
					fItemWindowSpeedX *= fSpeedSet_1;
					fItemWindowSpeedY *= fSpeedSet_2;

					SizeX -= fItemWindowSpeedX;
					SizeY += fItemWindowSpeedY;

					dItemTime_First = 0.0;
				}

				if (5.f >= SizeX || 700.f <= SizeY)
				{
					//SizeX = 5.f * 1.4f;
					//SizeY = 700.f * 1.4f;
					SizeX = 5.f;
					SizeY = 700.f;

					fItemWindowSpeedX = 1.f;
					fItemWindowSpeedY = 1.f;

					iItemPatternCnt = 1;
				}

				break;

			case 1:
				/* 700 100 */
				dItemTime_First += TimeDelta;

				if (dSetTime < dItemTime_First)
				{
					fItemWindowSpeedX *= fSpeedSet_2;
					fItemWindowSpeedY *= fSpeedSet_1;

					SizeX += fItemWindowSpeedX;
					SizeY -= fItemWindowSpeedY;

					dItemTime_First = 0.0;
				}

				if (700.f <= SizeX || 100.f >= SizeY)
				{
					//SizeX = 700.f * 1.4f;
					//SizeY = 100.f * 1.4f;
					SizeX = 700.f;
					SizeY = 100.f;

					fItemWindowSpeedX = 1.f;
					fItemWindowSpeedY = 1.f;

					iItemPatternCnt = 2;
				}
				break;

			case 2:
				/* 200 250 */
				dItemTime_First += TimeDelta;

				if (dSetTime < dItemTime_First)
				{
					fItemWindowSpeedX *= fSpeedSet_1;
					fItemWindowSpeedY *= fSpeedSet_2;

					SizeX -= fItemWindowSpeedX;
					SizeY += fItemWindowSpeedY;

					dItemTime_First = 0.0;
				}

				if (200.f >= SizeX || 250.f <= SizeY)
				{
					//SizeX = 200.f*1.4f;
					//SizeY = 250.f*1.4f;
					SizeX = 200.f;
					SizeY = 250.f;

					fItemWindowSpeedX = 1.f;
					fItemWindowSpeedY = 1.f;

					iItemPatternCnt = 3;
				}
				break;

			case 3:
				/* 500 420 */
				dItemTime_First += TimeDelta;

				if (dSetTime < dItemTime_First)
				{
					fItemWindowSpeedX *= fSpeedSet_1;
					fItemWindowSpeedY *= fSpeedSet_1;

					SizeX += fItemWindowSpeedX;
					SizeY += fItemWindowSpeedY;

					dItemTime_First = 0.0;
				}

				if (620.f <= SizeX || 380.f <= SizeY)
				{
					//SizeX = 520.f*1.4f;
					//SizeY = 420.f*1.4f;
					SizeX = 620.f;
					SizeY = 380.f;

					fItemWindowSpeedX = 1.f;
					fItemWindowSpeedY = 1.f;

					iItemPatternCnt = 4;
				}
				break;

			case 4:
				SizeX = 500.f * 1.4f;
				SizeY = 420.f * 1.4f;
				SetX = 450.f;
				SetY = 400.f;

				iItemPatternCnt = 5;

				break;

			case 5:

			default:
				break;
			}
		}
		else
		{
			switch (iItemPatternCnt)
			{
			case 0:
				/* 500 420 */
				dItemTime_First += TimeDelta;

				if (dSetTime < dItemTime_First)
				{
					fItemWindowSpeedX *= fSpeedSet_2;
					fItemWindowSpeedY *= fSpeedSet_1;

					SizeX += fItemWindowSpeedX;
					SizeY -= fItemWindowSpeedY;

					dItemTime_First = 0.0;
				}

				if (5.f >= SizeY)
				{
					SizeY = 5.f;
					m_bDead = true;
				}
				break;


			default:
				break;
			}
		}
		/* Animation_END=== */


		/* ============= */

		if (m_pGameInstance->Get_KeyEnter(DIK_BACKSPACE) && 0 == WindowLevel)
		{
			Inven->Set_ShopWindow(false);
			iItemPatternCnt = 0;
			bCreate = false;

			SM->Sound_Command(L"UI_Menu_Close.wav", 1.f);
		}

		break;
	case Engine::CUI_Parents::SHOPWINDOW1:

		if (true == ShopWindowDelete)
		{
			ShopWindowDelete = false;
			m_bDead = true;
		}

		break;
	case Engine::CUI_Parents::SHOPWINDOW2:
		
		if (true == ShopWindowDelete)
		{
			ShopWindowDelete = false;
			m_bDead = true;
		}

		break;
	case Engine::CUI_Parents::SHOPWINDOW3:

		if (true == ShopWindowDelete_Up)
		{
			ShopWindowDelete_Up = false;
			m_bDead = true;
		}

		break;
	case Engine::CUI_Parents::SHOPWINDOW4:

		if (true == ShopWindowDelete_Up)
		{
			ShopWindowDelete_Up = false;
			m_bDead = true;
		}

		break;
	case Engine::CUI_Parents::SHOPWINDOW5:

		if (true == ShopWindowDelete_List)
		{
			ShopWindowDelete_List = false;
			m_bDead = true;
		}

		break;
	case Engine::CUI_Parents::SHOPWINDOW6:

		ShopWindow6Time += TimeDelta;

		if (2.0 < ShopWindow6Time)
			m_bDead = true;


		break;
	case Engine::CUI_Parents::SHOPWINDOW7:

		ShopWindow6Time += TimeDelta;

		if (2.0 < ShopWindow6Time)
			m_bDead = true;


		break;
	case Engine::CUI_Parents::SHOPWINDOW8:

		ShopWindow6Time += TimeDelta;

		if (2.0 < ShopWindow6Time)
			m_bDead = true;


		break;
	case Engine::CUI_Parents::WINDOW_SELECT:
	
		///* TEST === */
		//if (m_pGameInstance->Get_KeyPressing(DIK_UP))
		//	SetY += 1.f;

		//if (m_pGameInstance->Get_KeyPressing(DIK_DOWN))
		//	SetY -= 1.f;

		//if (m_pGameInstance->Get_KeyPressing(DIK_LEFT))
		//	SetX -= 1.f;

		//if (m_pGameInstance->Get_KeyPressing(DIK_RIGHT))
		//	SetX += 1.f;
		///* ========= */


		/* 실시간 적용 */
		Inven->Inven_Object_Zero();
		/* =========== */

		if(true == Inven->Get_ItemWindow())
		{
			/* ====== FirstWindow ====== */
			/* ===================== */
			if (m_pGameInstance->Get_KeyEnter(DIK_UP) && true == bInvenFirst)
			{
				iInvenSlotCntFirst -= 1;
				SM->Sound_Command(L"Menu_Move.wav", 1.f);
			}

			if (m_pGameInstance->Get_KeyEnter(DIK_DOWN) && true == bInvenFirst)
			{
				iInvenSlotCntFirst += 1;
				SM->Sound_Command(L"Menu_Move.wav", 1.f);
			}
				

			if (0 >= iInvenSlotCntFirst)
				iInvenSlotCntFirst = 6;
			if (7 <= iInvenSlotCntFirst)
				iInvenSlotCntFirst = 1;
			/* ===================== */

			if (true == bInvenFirst)
			{
				switch (iInvenSlotCntFirst)
				{
				case 1:
					SetX = 140.f + fSelect_PosX;
					SetY = 539.f - fSelect_PosY;

					if (SLOT_EMPTY != Inven->Get_InvenSlot1_Name())
					{
						if (m_pGameInstance->Get_KeyEnter(DIK_RETURN) && true == bInvenFirst)
						{
							if (1 <= iCreateCnt)
							{
								ItemWindow1_X = 523.f;
								ItemWindow1_Y = 515.f;

								/* === */
								WindowLevel = 1;

								bInvenFirst = false;
								bInvenSecond = true;
								iCreateCnt = 0;

								/* 기능 */
								iInvenTemp = Inven->Get_InvenSlot1_Name(); // 인벤토리 슬롯1 정보 담기

																		   /* 선택 Window === */
								iCom_Texture_Set = CUI_Parents::MENU_WINDOW::ITEMWINDOW1;
								if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
									return 0;

								SM->Sound_Command(L"Menu_Slot.wav", 1.f);
							}
							iCreateCnt += 1;
						}
					}

					break;
				case 2:
					SetX = 140.f + (fSelect_PosX*2.f);
					SetY = 484.f - fSelect_PosY;

					if (SLOT_EMPTY != Inven->Get_InvenSlot2_Name())
					{
						if (m_pGameInstance->Get_KeyEnter(DIK_RETURN) && true == bInvenFirst)
						{
							if (1 <= iCreateCnt)
							{
								ItemWindow1_X = 523.f + 9.f;
								ItemWindow1_Y = 459.f;

								/* === */
								WindowLevel = 1;

								bInvenFirst = false;
								bInvenSecond = true;
								iCreateCnt = 0;

								/* 선택 Window === */
								iInvenTemp = Inven->Get_InvenSlot2_Name(); // 인벤토리 슬롯2 정보 담기

								iCom_Texture_Set = CUI_Parents::MENU_WINDOW::ITEMWINDOW1;
								if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
									return 0;

								SM->Sound_Command(L"Menu_Slot.wav", 1.f);
							}
							iCreateCnt += 1;
						}
					}
					break;
				case 3:
					SetX = 140.f + (fSelect_PosX*3.f);
					SetY = 429.f - fSelect_PosY;

					if (SLOT_EMPTY != Inven->Get_InvenSlot3_Name())
					{
						if (m_pGameInstance->Get_KeyEnter(DIK_RETURN) && true == bInvenFirst)
						{
							if (1 <= iCreateCnt)
							{
								ItemWindow1_X = 523.f + 18.f;
								ItemWindow1_Y = 403.f;

								/* === */
								WindowLevel = 1;

								bInvenFirst = false;
								bInvenSecond = true;
								iCreateCnt = 0;

								/* 선택 Window === */
								iInvenTemp = Inven->Get_InvenSlot3_Name(); // 인벤토리 슬롯3 정보 담기

								iCom_Texture_Set = CUI_Parents::MENU_WINDOW::ITEMWINDOW1;
								if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
									return 0;

								SM->Sound_Command(L"Menu_Slot.wav", 1.f);
							}
							iCreateCnt += 1;
						}
					}
					break;
				case 4:
					SetX = 140.f + (fSelect_PosX*4.f);
					SetY = 374.f - fSelect_PosY;

					if (SLOT_EMPTY != Inven->Get_InvenSlot4_Name())
					{
						if (m_pGameInstance->Get_KeyEnter(DIK_RETURN) && true == bInvenFirst)
						{
							if (1 <= iCreateCnt)
							{
								ItemWindow1_X = 523.f + 27.f;
								ItemWindow1_Y = 347.f;

								/* === */
								WindowLevel = 1;

								bInvenFirst = false;
								bInvenSecond = true;
								iCreateCnt = 0;

								/* 선택 Window === */
								iInvenTemp = Inven->Get_InvenSlot4_Name(); // 인벤토리 슬롯4 정보 담기

								iCom_Texture_Set = CUI_Parents::MENU_WINDOW::ITEMWINDOW1;
								if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
									return 0;

								SM->Sound_Command(L"Menu_Slot.wav", 1.f);
							}
							iCreateCnt += 1;
						}
					}
					break;
				case 5:
					SetX = 140.f + (fSelect_PosX*5.f);
					SetY = 318.f - fSelect_PosY;

					if (SLOT_EMPTY != Inven->Get_InvenSlot5_Name())
					{
						if (m_pGameInstance->Get_KeyEnter(DIK_RETURN) && true == bInvenFirst)
						{
							if (1 <= iCreateCnt)
							{
								ItemWindow1_X = 523.f + 36.f;
								ItemWindow1_Y = 291.f;

								/* === */
								WindowLevel = 1;

								bInvenFirst = false;
								bInvenSecond = true;
								iCreateCnt = 0;

								/* 선택 Window === */
								iInvenTemp = Inven->Get_InvenSlot5_Name(); // 인벤토리 슬롯5 정보 담기

								iCom_Texture_Set = CUI_Parents::MENU_WINDOW::ITEMWINDOW1;
								if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
									return 0;

								SM->Sound_Command(L"Menu_Slot.wav", 1.f);
							}
							iCreateCnt += 1;
						}
					}
					break;
				case 6:
					SetX = 140.f + (fSelect_PosX*6.f);
					SetY = 262.f - fSelect_PosY;
					if (SLOT_EMPTY != Inven->Get_InvenSlot6_Name())
					{
						if (m_pGameInstance->Get_KeyEnter(DIK_RETURN) && true == bInvenFirst)
						{
							if (1 <= iCreateCnt)
							{
								ItemWindow1_X = 523.f + 45.f;
								ItemWindow1_Y = 235.f;

								/* === */
								WindowLevel = 1;

								bInvenFirst = false;
								bInvenSecond = true;
								iCreateCnt = 0;

								/* 선택 Window === */
								iInvenTemp = Inven->Get_InvenSlot6_Name(); // 인벤토리 슬롯6 정보 담기

								iCom_Texture_Set = CUI_Parents::MENU_WINDOW::ITEMWINDOW1;
								if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
									return 0;

								SM->Sound_Command(L"Menu_Slot.wav", 1.f);
							}
							iCreateCnt += 1;
						}
					}	
					break;
				default:
					break;
				}
			}
			/* ====== FirstWindow_END ====== */

			/* ====== SecondWindow ====== */
			/* ===================== */
			if (m_pGameInstance->Get_KeyEnter(DIK_UP) && true == bInvenSecond)
			{
				iInvenSlotCntSecond -= 1;
				SM->Sound_Command(L"Menu_Move.wav", 1.f);
			}
				
			if (m_pGameInstance->Get_KeyEnter(DIK_DOWN) && true == bInvenSecond)
			{
				iInvenSlotCntSecond += 1;
				SM->Sound_Command(L"Menu_Move.wav", 1.f);
			}
			
			if (0 >= iInvenSlotCntSecond)
				iInvenSlotCntSecond = 2;
			if (3 <= iInvenSlotCntSecond)
				iInvenSlotCntSecond = 1;
			/* ===================== */

			if (true == bInvenSecond)
			{
				switch (iInvenSlotCntSecond)
				{
				case 1: /* 선택 */
					SetX = ItemWindow1_X - 23.f;
					SetY = ItemWindow1_Y + 12.f;

					if (m_pGameInstance->Get_KeyEnter(DIK_RETURN) && true == bInvenSecond) 
					{
						if (2 <= iCreateCnt)
						{
							/* 기능 ==== */

							Inven->Set_InvenDelete(false);

							iCom_Texture_Set = CUI_Parents::ITEMWINDOWQUICK_SLOT1;
							if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_ItemText"), &iCom_Texture_Set)))
								return 0;

							iCom_Texture_Set = CUI_Parents::ITEMWINDOWQUICK_SLOT2;
							if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_ItemText"), &iCom_Texture_Set)))
								return 0;

							iCom_Texture_Set = CUI_Parents::ITEMWINDOWQUICK_SLOT3;
							if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_ItemText"), &iCom_Texture_Set)))
								return 0;

							iCom_Texture_Set = CUI_Parents::ITEMWINDOWQUICK_SLOT4;
							if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_ItemText"), &iCom_Texture_Set)))
								return 0;

							/* ========= */
							ItemWindow1Delete = true;
							/* === */
							WindowLevel = 0;
							iCreateCnt = 0;

							iInvenSlotCntSecond = 1;
							bInvenFirst = false;
							bInvenSecond = false;
							bInvenThird = true;
						}
						iCreateCnt += 1;

						SM->Sound_Command(L"Menu_Select.wav", 1.f);
					}
					break;

				case 2: /* 취소 */
					SetX = ItemWindow1_X - 23.f;
					SetY = ItemWindow1_Y - 15.f;

					if (m_pGameInstance->Get_KeyEnter(DIK_RETURN) && true == bInvenSecond)
					{
						if (2 <= iCreateCnt)
						{
							ItemWindow1Delete = true;
							/* === */
							WindowLevel = 0;
							iCreateCnt = 0;

							iInvenSlotCntSecond = 1;
							bInvenFirst = true;
							bInvenSecond = false;
							bInvenThird = false;
						}
						iCreateCnt += 1;

						SM->Sound_Command(L"Menu_Select.wav", 1.f);
					}
					break;
				default:
					break;
				}

			}
			/* ====== SecondWindow_END ====== */

			/* ====== ThirdWindow ====== */
			if (true == bInvenThird)
			{
				/* ===================== */
				if (m_pGameInstance->Get_KeyEnter(DIK_UP) && true == bInvenThird)
				{
					iInvenSlotCntThird -= 1;
					SM->Sound_Command(L"Menu_Move.wav", 1.f);
				}
				
				if (m_pGameInstance->Get_KeyEnter(DIK_DOWN) && true == bInvenThird)
				{
					iInvenSlotCntThird += 1;
					SM->Sound_Command(L"Menu_Move.wav", 1.f);
				}
					
				if (0 >= iInvenSlotCntThird)
					iInvenSlotCntThird = 4;
				if (5 <= iInvenSlotCntThird)
					iInvenSlotCntThird = 1;
				/* ===================== */

				switch (iInvenSlotCntThird)
				{
				case 1:
					SetX = 675.f - 57.f;
					SetY = 594.f - 10.f;

					if (m_pGameInstance->Get_KeyEnter(DIK_RETURN) && true == bInvenThird)
					{
						if (2 <= iCreateCnt)
						{
							if (Inven->Get_Slot2_Name() == iInvenTemp)
							{
								Inven->Set_Slot2_Name(SLOT_EMPTY);
								Inven->Set_Slot2(0);
							}
							if (Inven->Get_Slot3_Name() == iInvenTemp)
							{
								Inven->Set_Slot3_Name(SLOT_EMPTY);
								Inven->Set_Slot3(0);
							}
						
							Inven->Item_QuickSlot1_Set(iInvenTemp);

							/* === */
							WindowLevel = 0;
							iCreateCnt = 0;

							iInvenSlotCntSecond = 1;
							bInvenFirst = true;
							bInvenSecond = false;
							bInvenThird = false;

							SM->Sound_Command(L"Menu_Slot.wav", 1.f);

							Inven->Set_InvenDelete(true);
						}

						iCreateCnt += 1;
					}

					break;
				case 2:
					SetX = 680.f - 56.f;
					SetY = 559.f - 10.f;

					if (m_pGameInstance->Get_KeyEnter(DIK_RETURN) && true == bInvenThird)
					{
						if (2 <= iCreateCnt)
						{

							if (Inven->Get_Slot1_Name() == iInvenTemp)
							{
								Inven->Set_Slot1_Name(SLOT_EMPTY);
								Inven->Set_Slot1(0);
							}
							if (Inven->Get_Slot3_Name() == iInvenTemp)
							{
								Inven->Set_Slot3_Name(SLOT_EMPTY);
								Inven->Set_Slot3(0);
							}

							SM->Sound_Command(L"Menu_Slot.wav", 1.f);
								
							Inven->Item_QuickSlot2_Set(iInvenTemp);

							/* === */
							WindowLevel = 0;
							iCreateCnt = 0;

							iInvenSlotCntSecond = 1;
							bInvenFirst = true;
							bInvenSecond = false;
							bInvenThird = false;

							Inven->Set_InvenDelete(true);
						}

						iCreateCnt += 1;
					}

					break;
				case 3:
					SetX = 685.f - 55.f;
					SetY = 524.f - 10.f;

					if (m_pGameInstance->Get_KeyEnter(DIK_RETURN) && true == bInvenThird)
					{
						if (2 <= iCreateCnt)
						{
							if (Inven->Get_Slot1_Name() == iInvenTemp)
							{
								Inven->Set_Slot1_Name(SLOT_EMPTY);
								Inven->Set_Slot1(0);
							}
							if (Inven->Get_Slot2_Name() == iInvenTemp)
							{
								Inven->Set_Slot2_Name(SLOT_EMPTY);
								Inven->Set_Slot2(0);
							}

							SM->Sound_Command(L"Menu_Slot.wav", 1.f);
								
							Inven->Item_QuickSlot3_Set(iInvenTemp);

							/* === */
							WindowLevel = 0;
							iCreateCnt = 0;

							iInvenSlotCntSecond = 1;
							bInvenFirst = true;
							bInvenSecond = false;
							bInvenThird = false;

							Inven->Set_InvenDelete(true);
						}

						iCreateCnt += 1;
					}

					break;
				case 4:
					SetX = 690.f - 54.f;
					SetY = 489.f - 10.f;

					if (m_pGameInstance->Get_KeyEnter(DIK_RETURN) && true == bInvenThird)
					{
						if (2 <= iCreateCnt)
						{
							/* === */
							WindowLevel = 0;
							iCreateCnt = 0;

							iInvenSlotCntSecond = 1;
							bInvenFirst = true;
							bInvenSecond = false;
							bInvenThird = false;

							SM->Sound_Command(L"Menu_Slot.wav", 1.f);

							Inven->Set_InvenDelete(true);
						}

						iCreateCnt += 1;
					}
					break;
				}
			}
			/* ====== ThirdWindow_END ====== */
		}  /* Item_Window_END */ 

		/* ======================================================================== */
		/* ======================================================================== */

		if (true == Inven->Get_ShopWindow()) /* Shop_Window */
		{
			if (true == bShopFirst) /* 구매 판매 선택 창*/
			{
				if (m_pGameInstance->Get_KeyEnter(DIK_UP))
				{
					iShopSlotCntFirst -= 1;
					SM->Sound_Command(L"Menu_Move.wav", 1.f);
				}

				if (m_pGameInstance->Get_KeyEnter(DIK_DOWN))
				{
					iShopSlotCntFirst += 1;
					SM->Sound_Command(L"Menu_Move.wav", 1.f);
				}

				if (0 >= iShopSlotCntFirst)
					iShopSlotCntFirst = 2;
				if (3 <= iShopSlotCntFirst)
					iShopSlotCntFirst = 1;

				/* ================== */

				switch (iShopSlotCntFirst)
				{
				case 1:
					SetX = 632.f - 24.f;
					SetY = 542.f + 27.f;

					m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));

					WindowLevel = 0;
					if (m_pGameInstance->Get_KeyEnter(DIK_RETURN))
					{
						if (1 <= iCreateCnt)
						{
							/* 상단 구매 UI */
							iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW3;
							if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
								return 0;
							/* ============ */

							/* 구매 UI */
							iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW5;
							if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
								return 0;
							/* ============ */

							bSale = true; /* 구매 페이지 */

							WindowLevel = 1;
							bShopFirst = false;
							bShopSecond = true;
							bShopThird = false;

							iShopSlotCntSecond = 1;
							iCreateCnt = 0;

							SM->Sound_Command(L"Menu_Slot.wav", 1.f);
						}
						iCreateCnt += 1;

					}

					break;
				case 2:
					SetX = 640.f - 13.f;
					SetY = 490.f + 30.f;

					m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));

					WindowLevel = 0;
					if (m_pGameInstance->Get_KeyEnter(DIK_RETURN))
					{
						if (1 <= iCreateCnt)
						{
							/* 상단 판매 UI */
							iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW4;
							if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
								return 0;
							/* ============ */

							/* Inven List 생성 */
							Inven->Set_InvenNumRender(true);

							Inven->Set_InvenDelete_List(false);
							Inven->Set_InvenDelete(false);

							iCom_Texture_Set = CUI_Parents::MENU_WINDOW::ITEMWINDOW_SLOT1;
							if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_ItemText"), &iCom_Texture_Set)))
								return E_FAIL;

							iCom_Texture_Set = CUI_Parents::MENU_WINDOW::ITEMWINDOW_SLOT2;
							if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_ItemText"), &iCom_Texture_Set)))
								return E_FAIL;

							iCom_Texture_Set = CUI_Parents::MENU_WINDOW::ITEMWINDOW_SLOT3;
							if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_ItemText"), &iCom_Texture_Set)))
								return E_FAIL;

							iCom_Texture_Set = CUI_Parents::MENU_WINDOW::ITEMWINDOW_SLOT4;
							if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_ItemText"), &iCom_Texture_Set)))
								return E_FAIL;

							iCom_Texture_Set = CUI_Parents::MENU_WINDOW::ITEMWINDOW_SLOT5;
							if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_ItemText"), &iCom_Texture_Set)))
								return E_FAIL;

							iCom_Texture_Set = CUI_Parents::MENU_WINDOW::ITEMWINDOW_SLOT6;
							if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_ItemText"), &iCom_Texture_Set)))
								return E_FAIL;

							/* ============ */

							bSale = false; /* 판매 페이지 */

							WindowLevel = 1;
							bShopFirst = false;
							bShopSecond = true;
							bShopThird = false;

							iShopSlotCntSecond = 1;
							iCreateCnt = 0;

							SM->Sound_Command(L"Menu_Slot.wav", 1.f);
						}
						iCreateCnt += 1;

					}

					break;
				default:
					break;
				}

			}

			/* ============================================================ */
			/* ============================================================ */

			if (true == bShopSecond)
			{
				if (m_pGameInstance->Get_KeyEnter(DIK_UP))
				{
					iShopSlotCntSecond -= 1;
					SM->Sound_Command(L"Menu_Move.wav", 1.f);
				}
				if (m_pGameInstance->Get_KeyEnter(DIK_DOWN)) 
				{
					iShopSlotCntSecond += 1;
					SM->Sound_Command(L"Menu_Move.wav", 1.f);
				}

				if (0 >= iShopSlotCntSecond)
					iShopSlotCntSecond = 6;
				if (7 <= iShopSlotCntSecond)
					iShopSlotCntSecond = 1;

				/* ================== */

				_float PlusX = 8.f;
				_float PlusY = -44.f;

				switch (iShopSlotCntSecond)
				{
				case 1:
					SetX = 188.f + PlusX - 12.f - 10.f;
					SetY = 553.f + PlusY - 6.f - 2.f - 2.f;
					
					m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));

					if (m_pGameInstance->Get_KeyEnter(DIK_RETURN))
					{
						if (true == bSale) /* 포션 구매 */
						{

							if (2 <= iCreateCnt)
							{
								iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW1;
								if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
									return 0;

								iShopObject = SLOT_POTION;

								bShopFirst = false;
								bShopSecond = false;
								bShopThird = true;

								iShopSlotCntThird = 2;
								iCreateCnt = 0;

								SM->Sound_Command(L"Menu_Slot.wav", 1.f);
							}
							iCreateCnt += 1;
						
						}
						else /* 판매 */
						{

							if (2 <= iCreateCnt && SLOT_EMPTY != Inven->Get_InvenSlot1_Name())
							{
								iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW2;
								if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
									return 0;

								iShopObject = Inven->Get_InvenSlot1_Name();

								bShopFirst = false;
								bShopSecond = false;
								bShopThird = true;

								iShopSlotCntThird = 2;
								iCreateCnt = 0;
							}
							iCreateCnt += 1;

						}

						SM->Sound_Command(L"Menu_Slot.wav", 1.f);
					}
					break;
				case 2:
					SetX = 188.f + PlusX - 8.f - 8.f;
					SetY = 495.f + PlusY - 5.f - 2.f - 1.f;

					m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));

					if (m_pGameInstance->Get_KeyEnter(DIK_RETURN))
					{
						if (true == bSale) /* 하이포션 구매 */
						{

							if (2 <= iCreateCnt)
							{
								iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW1;
								if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
									return 0;

								iShopObject = SLOT_HIGH_POTION;

								bShopFirst = false;
								bShopSecond = false;
								bShopThird = true;

								iShopSlotCntThird = 2;
								iCreateCnt = 0;
							}
							iCreateCnt += 1;

						}
						else /* 판매 */
						{

							if (2 <= iCreateCnt && SLOT_EMPTY != Inven->Get_InvenSlot2_Name())
							{
								iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW2;
								if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
									return 0;

								iShopObject = Inven->Get_InvenSlot2_Name();

								bShopFirst = false;
								bShopSecond = false;
								bShopThird = true;

								iShopSlotCntThird = 2;
								iCreateCnt = 0;
							}
							iCreateCnt += 1;

						}

						SM->Sound_Command(L"Menu_Slot.wav", 1.f);
					}
					break;
				case 3:
					SetX = 188.f + PlusX - 4.f - 6.f;
					SetY = 437.f + PlusY - 4.f - 2.f;

					m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));

					if (m_pGameInstance->Get_KeyEnter(DIK_RETURN))
					{
						if (true == bSale) /* 엘릭서 구매 */
						{

							if (2 <= iCreateCnt)
							{
								iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW1;
								if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
									return 0;

								iShopObject = SLOT_ELIXIR;

								bShopFirst = false;
								bShopSecond = false;
								bShopThird = true;

								iShopSlotCntThird = 2;
								iCreateCnt = 0;
							}
							iCreateCnt += 1;

						}
						else /* 판매 */
						{

							if (2 <= iCreateCnt && SLOT_EMPTY != Inven->Get_InvenSlot3_Name())
							{
								iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW2;
								if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
									return 0;

								iShopObject = Inven->Get_InvenSlot3_Name();

								bShopFirst = false;
								bShopSecond = false;
								bShopThird = true;

								iShopSlotCntThird = 2;
								iCreateCnt = 0;
							}
							iCreateCnt += 1;

						}

						SM->Sound_Command(L"Menu_Slot.wav", 1.f);
					}
					break;
				case 4:
					SetX = 188.f + PlusX - 4.f;
					SetY = 379.f + PlusY - 3.f;

					m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));

					if (m_pGameInstance->Get_KeyEnter(DIK_RETURN))
					{
						if (true == bSale) /* 공격력 UP 구매 */
						{

							if (2 <= iCreateCnt)
							{
								iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW1;
								if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
									return 0;

								iShopObject = SLOT_ATKUP;

								bShopFirst = false;
								bShopSecond = false;
								bShopThird = true;

								iShopSlotCntThird = 2;
								iCreateCnt = 0;
							}
							iCreateCnt += 1;

						}
						else /* 판매 */
						{

							if (2 <= iCreateCnt && SLOT_EMPTY != Inven->Get_InvenSlot4_Name())
							{
								iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW2;
								if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
									return 0;

								iShopObject = Inven->Get_InvenSlot4_Name();

								bShopFirst = false;
								bShopSecond = false;
								bShopThird = true;

								iShopSlotCntThird = 2;
								iCreateCnt = 0;
							}
							iCreateCnt += 1;

						}

						SM->Sound_Command(L"Menu_Slot.wav", 1.f);
					}
					break;
				case 5:
					SetX = 188.f + PlusX + 4.f - 2.f;
					SetY = 321.f + PlusY - 2.f ;
					
					m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));

					if (m_pGameInstance->Get_KeyEnter(DIK_RETURN))
					{
						if (true == bSale) /* 공격속도 UP 구매 */
						{

							if (2 <= iCreateCnt)
							{
								iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW1;
								if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
									return 0;

								iShopObject = SLOT_ATKSPEEDUP;

								bShopFirst = false;
								bShopSecond = false;
								bShopThird = true;

								iShopSlotCntThird = 2;
								iCreateCnt = 0;
							}
							iCreateCnt += 1;

						}
						else /* 판매 */
						{

							if (2 <= iCreateCnt && SLOT_EMPTY != Inven->Get_InvenSlot5_Name())
							{
								iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW2;
								if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
									return 0;

								iShopObject = Inven->Get_InvenSlot5_Name();

								bShopFirst = false;
								bShopSecond = false;
								bShopThird = true;

								iShopSlotCntThird = 2;
								iCreateCnt = 0;
							}
							iCreateCnt += 1;

						}

						SM->Sound_Command(L"Menu_Slot.wav", 1.f);
					}
					break;
				case 6:
					SetX = 188.f + PlusX + 8.f;
					SetY = 263.f + PlusY;

					m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));

					if (m_pGameInstance->Get_KeyEnter(DIK_RETURN))
					{
						if (true == bSale) /* 이동속도 UP 구매 */
						{

							if (2 <= iCreateCnt)
							{
								iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW1;
								if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
									return 0;

								iShopObject = SLOT_SPEEDUP;

								bShopFirst = false;
								bShopSecond = false;
								bShopThird = true;

								iShopSlotCntThird = 2;
								iCreateCnt = 0;
							}
							iCreateCnt += 1;

						}
						else /* 판매 */
						{

							if (2 <= iCreateCnt && SLOT_EMPTY != Inven->Get_InvenSlot6_Name())
							{
								iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW2;
								if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
									return 0;

								iShopObject = Inven->Get_InvenSlot6_Name();

								bShopFirst = false;
								bShopSecond = false;
								bShopThird = true;

								iShopSlotCntThird = 2;
								iCreateCnt = 0;
							}
							iCreateCnt += 1;

						}

						SM->Sound_Command(L"Menu_Slot.wav", 1.f);
					}
					break;
				default:
					break;
				}

				if (m_pGameInstance->Get_KeyEnter(DIK_BACKSPACE))
				{
					if (2 <= iCreateCnt)
					{
						Inven->Set_InvenNumRender(false);

						bShopFirst = true;
						bShopSecond = false;
						bShopThird = false;

						iCreateCnt = 0;

						if (true == bSale)
							ShopWindowDelete_List = true;/* 구매 List Delete */
						else
							Inven->Set_InvenDelete_List(true);/*판매 List Delete*/

						ShopWindowDelete_Up = true; /* 상단 UI Delete */
					}
					iCreateCnt += 1;

					SM->Sound_Command(L"UI_Menu_Close.wav", 1.f);
				}

			}

			/* ============================================================ */
			/* ============================================================ */

			if (true == bShopThird) /* 구매 판매 Frame */
			{
				if (m_pGameInstance->Get_KeyEnter(DIK_LEFT))
					iShopSlotCntThird -= 1;
				if (m_pGameInstance->Get_KeyEnter(DIK_RIGHT))
					iShopSlotCntThird += 1;

				if (0 >= iShopSlotCntThird)
					iShopSlotCntThird = 2;
				if (3 <= iShopSlotCntThird)
					iShopSlotCntThird = 1;

				/* ================== */

				NM->Setting_Menu_Shop(iShopObjectCnt, 26.f);

				switch (iShopSlotCntThird)
				{
				case 1:
					SetX = 642.f;
					SetY = 167.f;

					m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(0.f));

					if (true == bSale) /* 구매 */
					{
						if (m_pGameInstance->Get_KeyEnter(DIK_UP))
						{
							iShopObjectCnt += 1;
							SM->Sound_Command(L"Menu_Move.wav", 1.f);
						}
						if (m_pGameInstance->Get_KeyEnter(DIK_DOWN))
						{
							iShopObjectCnt -= 1;
							SM->Sound_Command(L"Menu_Move.wav", 1.f);
						}

						if (0 >= iShopObjectCnt)
							iShopObjectCnt = 1;
						if (99 <= iShopObjectCnt)
							iShopObjectCnt = 99;
						/* ============================================= */

						if (m_pGameInstance->Get_KeyEnter(DIK_RETURN))
						{
							if (2 <= iCreateCnt)
							{
								/* 기능 === */
								Shop_Buy(iShopObjectCnt); // 상점 구매용
								m_pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_Money_Shop"));
								/* ======== */
								ShopWindowDelete = true;

								bShopFirst = false;
								bShopSecond = true;
								bShopThird = false;

								iShopObjectCnt = 1;
								iCreateCnt = 1;

								SM->Sound_Command(L"Menu_Select.wav", 1.f);
							}
							iCreateCnt += 1;
						}

					}
					else /* 판매 */
					{
						if (m_pGameInstance->Get_KeyEnter(DIK_UP))
						{
							iShopObjectCnt += 1;
							SM->Sound_Command(L"Menu_Move.wav", 1.f);
						}
						if (m_pGameInstance->Get_KeyEnter(DIK_DOWN))
						{
							iShopObjectCnt -= 1;
							SM->Sound_Command(L"Menu_Move.wav", 1.f);
						}

						if (0 >= iShopObjectCnt)
							iShopObjectCnt = 1;
		
						if (SLOT_POTION == iShopObject)
						{
							if (Inven->Get_Potion() <= iShopObjectCnt)
								iShopObjectCnt = Inven->Get_Potion();
						}
						else if (SLOT_HIGH_POTION == iShopObject)
						{
							if (Inven->Get_High_Potion() <= iShopObjectCnt)
								iShopObjectCnt = Inven->Get_High_Potion();
						}
						else if (SLOT_ELIXIR == iShopObject)
						{
							if (Inven->Get_Elixir() <= iShopObjectCnt)
								iShopObjectCnt = Inven->Get_Elixir();
						}
						else if (SLOT_ATKUP == iShopObject)
						{
							if (Inven->Get_AtkUp() <= iShopObjectCnt)
								iShopObjectCnt = Inven->Get_AtkUp();
						}
						else if (SLOT_ATKSPEEDUP == iShopObject)
						{
							if (Inven->Get_AtkSpeedUp() <= iShopObjectCnt)
								iShopObjectCnt = Inven->Get_AtkSpeedUp();
						}
						else if (SLOT_SPEEDUP == iShopObject)
						{
							if (Inven->Get_SpeedUp() <= iShopObjectCnt)
								iShopObjectCnt = Inven->Get_SpeedUp();
						}

						/* ============================================= */

						if (m_pGameInstance->Get_KeyEnter(DIK_RETURN))
						{
							if (2 <= iCreateCnt)
							{
								/* 기능 === */

								/* 판매 완료 */
								Shop_Sell(iShopObjectCnt);
								Inven->Inven_sort(0);
								m_pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_Money_Shop"));
								/* ======== */
								ShopWindowDelete = true;

								bShopFirst = false;
								bShopSecond = true;
								bShopThird = false;

								iShopObjectCnt = 1;
								iCreateCnt = 1;

								SM->Sound_Command(L"Menu_Select.wav", 1.f);
							}
							iCreateCnt += 1;
						}
					}
					break;
				case 2: /* 취소 */
					SetX = 707.f;
					SetY = 167.f;

					m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(0.f));

					if (true == bSale) /* 구매(취소) */
					{
						if (m_pGameInstance->Get_KeyEnter(DIK_UP))
						{
							iShopObjectCnt += 1;
							SM->Sound_Command(L"Menu_Move.wav", 1.f);
						}
						if (m_pGameInstance->Get_KeyEnter(DIK_DOWN))
						{
							iShopObjectCnt -= 1;
							SM->Sound_Command(L"Menu_Move.wav", 1.f);
						}

						if (0 >= iShopObjectCnt)
							iShopObjectCnt = 1;
						if (99 <= iShopObjectCnt)
							iShopObjectCnt = 99;
						/* ============================================= */

						if (m_pGameInstance->Get_KeyEnter(DIK_RETURN))
						{
							if (2 <= iCreateCnt)
							{
								m_pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_Money_Shop"));

								ShopWindowDelete = true;

								bShopFirst = false;
								bShopSecond = true;
								bShopThird = false;

								iShopObjectCnt = 1;
								iCreateCnt = 1;

								SM->Sound_Command(L"UI_Menu_Close.wav", 1.f);
							}
							iCreateCnt += 1;
						}
					}
					else /* 판매(취소) */
					{
						if (m_pGameInstance->Get_KeyEnter(DIK_UP))
						{
							iShopObjectCnt += 1;
							SM->Sound_Command(L"Menu_Move.wav", 1.f);
						}
						if (m_pGameInstance->Get_KeyEnter(DIK_DOWN))
						{
							iShopObjectCnt -= 1;
							SM->Sound_Command(L"Menu_Move.wav", 1.f);
						}

						if (0 >= iShopObjectCnt)
							iShopObjectCnt = 1;

						if (SLOT_POTION == iShopObject)
						{
							if (Inven->Get_Potion() <= iShopObjectCnt)
								iShopObjectCnt = Inven->Get_Potion();
						}
						else if (SLOT_HIGH_POTION == iShopObject)
						{
							if (Inven->Get_High_Potion() <= iShopObjectCnt)
								iShopObjectCnt = Inven->Get_High_Potion();
						}
						else if (SLOT_ELIXIR == iShopObject)
						{
							if (Inven->Get_Elixir() <= iShopObjectCnt)
								iShopObjectCnt = Inven->Get_Elixir();
						}
						else if (SLOT_ATKUP == iShopObject)
						{
							if (Inven->Get_AtkUp() <= iShopObjectCnt)
								iShopObjectCnt = Inven->Get_AtkUp();
						}
						else if (SLOT_ATKSPEEDUP == iShopObject)
						{
							if (Inven->Get_AtkSpeedUp() <= iShopObjectCnt)
								iShopObjectCnt = Inven->Get_AtkSpeedUp();
						}
						else if (SLOT_SPEEDUP == iShopObject)
						{
							if (Inven->Get_SpeedUp() <= iShopObjectCnt)
								iShopObjectCnt = Inven->Get_SpeedUp();
						}
						/* ============================================= */

						if (m_pGameInstance->Get_KeyEnter(DIK_RETURN))
						{
							if (2 <= iCreateCnt)
							{
								m_pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_Money_Shop"));

								ShopWindowDelete = true;

								bShopFirst = false;
								bShopSecond = true;
								bShopThird = false;

								iShopObjectCnt = 1;
								iCreateCnt = 1;

								SM->Sound_Command(L"UI_Menu_Close.wav", 1.f);
							}
							iCreateCnt += 1;
						}
					}
					break;
				default:
					break;
				}


			}

		}  /* Shop_Window_END */

		if (m_pGameInstance->Get_KeyEnter(DIK_BACKSPACE) && 0 == WindowLevel)
		{
			m_bDead = true;
			SM->Sound_Command(L"UI_Menu_Close.wav", 1.f);
		}
			
		inttest = 10;
		break;

	default:
		break;
	}

	Inven->Set_MovePositionX(MovePositionX);
	Inven->Set_MovePositionY(MovePositionY);



	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX + MovePositionX, SettingY + SetY + MovePositionY, 0.f, 1.f));

	return _int();
}

_int CMenu_Window::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::ITEMWINDOW0:

		break;
	case Engine::CUI_Parents::ITEMWINDOW1:

		break;
	case Engine::CUI_Parents::SHOPWINDOW0:
		break;
	case Engine::CUI_Parents::SHOPWINDOW1:
		break;
	case Engine::CUI_Parents::SHOPWINDOW2:
		break;
	case Engine::CUI_Parents::SHOPWINDOW3:
		break;
	case Engine::CUI_Parents::SHOPWINDOW4:

		break;
	case Engine::CUI_Parents::SHOPWINDOW5:

		break;
	case Engine::CUI_Parents::SHOPWINDOW6:

		break;
	case Engine::CUI_Parents::SHOPWINDOW7:

		break;
	case Engine::CUI_Parents::SHOPWINDOW8:

		break;
	case Engine::CUI_Parents::WINDOW_SELECT:

		break;
	default:
		break;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CMenu_Window::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::ITEMWINDOW0:

		break;
	case Engine::CUI_Parents::ITEMWINDOW1:

		break;
	case Engine::CUI_Parents::SHOPWINDOW0:
		break;
	case Engine::CUI_Parents::SHOPWINDOW1:
		break;
	case Engine::CUI_Parents::SHOPWINDOW2:
		break;
	case Engine::CUI_Parents::SHOPWINDOW3:
		break;
	case Engine::CUI_Parents::SHOPWINDOW4:

		break;
	case Engine::CUI_Parents::SHOPWINDOW5:

		break;
	case Engine::CUI_Parents::SHOPWINDOW6:

		break;
	case Engine::CUI_Parents::SHOPWINDOW7:

		break;
	case Engine::CUI_Parents::SHOPWINDOW8:

		break;
	case Engine::CUI_Parents::WINDOW_SELECT:
		
		break;
	default:
		break;
	}

	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}


/* SHOP ======================================== */
 
void CMenu_Window::Shop_Buy(_int _Cnt)
{
	if (SLOT_POTION == iShopObject)
	{
		if (50 * _Cnt > Inven->Get_TotalMoney())
		{
			/* 구매 실패 */
			iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW6;
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
				return;
		}

		if (50 * _Cnt <= Inven->Get_TotalMoney())
		{

			Inven->Math_TotalMoney(-50 * _Cnt);
			Inven->Math_Potion(_Cnt);
			Inven->Inven_sort(SLOT_POTION);

			/* 구매 성공 */
			iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW7;
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
				return;
		}
	}
	else if (SLOT_HIGH_POTION == iShopObject)
	{
		if (100 * _Cnt > Inven->Get_TotalMoney())
		{
			/* 구매 실패 */
			iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW6;
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
				return;
		}

		if (100 * _Cnt <= Inven->Get_TotalMoney())
		{
			Inven->Math_TotalMoney(-100 * _Cnt);
			Inven->Math_High_Potion(_Cnt);
			Inven->Inven_sort(SLOT_HIGH_POTION);

			/* 구매 성공 */
			iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW7;
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
				return;
		}
	}
	else if (SLOT_ELIXIR == iShopObject)
	{
		if (200 * _Cnt > Inven->Get_TotalMoney())
		{
			/* 구매 실패 */
			iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW6;
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
				return;
		}

		if (200 * _Cnt <= Inven->Get_TotalMoney())
		{
			Inven->Math_TotalMoney(-200 * _Cnt);
			Inven->Math_Elixir(_Cnt);
			Inven->Inven_sort(SLOT_ELIXIR);

			/* 구매 성공 */
			iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW7;
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
				return;
		}

	}
	else if (SLOT_ATKUP == iShopObject)
	{
		if (300 * _Cnt > Inven->Get_TotalMoney())
		{
			/* 구매 실패 */
			iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW6;
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
				return;
		}

		if (300 * _Cnt <= Inven->Get_TotalMoney())
		{
			Inven->Math_TotalMoney(-300 * _Cnt);
			Inven->Math_AtkUp(_Cnt);
			Inven->Inven_sort(SLOT_ATKUP);

			/* 구매 성공 */
			iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW7;
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
				return;
		}
	}
	else if (SLOT_ATKSPEEDUP == iShopObject)
	{
		if (300 * _Cnt > Inven->Get_TotalMoney())
		{
			/* 구매 실패 */
			iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW6;
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
				return;
		}

		if (300 * _Cnt <= Inven->Get_TotalMoney())
		{
			Inven->Math_TotalMoney(-300 * _Cnt);
			Inven->Math_AtkSpeedUp(_Cnt);
			Inven->Inven_sort(SLOT_ATKSPEEDUP);

			/* 구매 성공 */
			iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW7;
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
				return;
		}
	}
	else if (SLOT_SPEEDUP == iShopObject)
	{
		if (300 * _Cnt > Inven->Get_TotalMoney())
		{
			/* 구매 실패 */
			iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW6;
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
				return;
		}

		if (300 * _Cnt <= Inven->Get_TotalMoney())
		{
			Inven->Math_TotalMoney(-300 * _Cnt);
			Inven->Math_SpeedUp(_Cnt);
			Inven->Inven_sort(SLOT_SPEEDUP);

			/* 구매 성공 */
			iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW7;
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
				return;
		}
	}
}

void CMenu_Window::Shop_Sell(_int _Cnt)
{

	if (SLOT_POTION == iShopObject)
	{
		Inven->Math_Potion(-_Cnt);
		Inven->Math_TotalMoney(25 * _Cnt);
		Inven->Inven_sort(iShopObject);
		iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW8;
		if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
			return;
	}
	else if (SLOT_HIGH_POTION == iShopObject)
	{
		Inven->Math_High_Potion(-_Cnt);
		Inven->Math_TotalMoney(50 * _Cnt);
		Inven->Inven_sort(iShopObject);
		iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW8;
		if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
			return;
	}
	else if (SLOT_ELIXIR == iShopObject)
	{
		Inven->Math_Elixir(-_Cnt);
		Inven->Math_TotalMoney(100 * _Cnt);
		Inven->Inven_sort(iShopObject);
		iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW8;
		if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
			return;
	}
	else if (SLOT_ATKUP == iShopObject)
	{
		Inven->Math_AtkUp(-_Cnt);
		Inven->Math_TotalMoney(150 * _Cnt);
		Inven->Inven_sort(iShopObject);
		iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW8;
		if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
			return;
	}
	else if (SLOT_ATKSPEEDUP == iShopObject)
	{
		Inven->Math_AtkSpeedUp(-_Cnt);
		Inven->Math_TotalMoney(150 * _Cnt);
		Inven->Inven_sort(iShopObject);
		iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW8;
		if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
			return;
	}
	else if (SLOT_SPEEDUP == iShopObject)
	{
		Inven->Math_SpeedUp(-_Cnt);
		Inven->Math_TotalMoney(150 * _Cnt);
		Inven->Inven_sort(iShopObject);
		iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW8;
		if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
			return;
	}

}

/* ============================================= */


HRESULT CMenu_Window::SetUp_Components()
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
	case Engine::CUI_Parents::ITEMWINDOW0:
	case Engine::CUI_Parents::ITEMWINDOW1:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_Inven_Frame"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;

	case Engine::CUI_Parents::SHOPWINDOW0:
	case Engine::CUI_Parents::SHOPWINDOW1:
	case Engine::CUI_Parents::SHOPWINDOW2:
	case Engine::CUI_Parents::SHOPWINDOW3:
	case Engine::CUI_Parents::SHOPWINDOW4:
	case Engine::CUI_Parents::SHOPWINDOW5:
	case Engine::CUI_Parents::SHOPWINDOW6:
	case Engine::CUI_Parents::SHOPWINDOW7:
	case Engine::CUI_Parents::SHOPWINDOW8:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_Shop_Frame"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;

	case Engine::CUI_Parents::WINDOW_SELECT:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_Select"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	
	default:
		break;
	}

	return S_OK;
}

HRESULT CMenu_Window::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));


	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::ITEMWINDOW0:
	case Engine::CUI_Parents::ITEMWINDOW1:
	case Engine::CUI_Parents::SHOPWINDOW0:
	case Engine::CUI_Parents::SHOPWINDOW1:
	case Engine::CUI_Parents::SHOPWINDOW2:
	case Engine::CUI_Parents::SHOPWINDOW3:
	case Engine::CUI_Parents::SHOPWINDOW4:
	case Engine::CUI_Parents::SHOPWINDOW5:
	case Engine::CUI_Parents::SHOPWINDOW6:
	case Engine::CUI_Parents::SHOPWINDOW7:
	case Engine::CUI_Parents::SHOPWINDOW8:
		CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_fFrame));
		break;

	case Engine::CUI_Parents::WINDOW_SELECT:
		CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture"));
		break;

	default:
		break;
	}

	return S_OK;
}

CMenu_Window * CMenu_Window::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMenu_Window*	pInstance = new CMenu_Window(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CMenu_Window");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMenu_Window::Clone(void * pArg)
{
	CMenu_Window*	pInstance = new CMenu_Window(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMenu_Window");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMenu_Window::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
