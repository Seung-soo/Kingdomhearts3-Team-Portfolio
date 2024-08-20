#include "stdafx.h"
#include "..\public\UI_Menu.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "InvenManager.h"
#include "UI_Num_Manager.h"
#include "SoundManager.h"

CUI_Menu::CUI_Menu(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_Menu::CUI_Menu(const CUI_Menu & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_Menu::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_Menu::NativeConstruct(void * pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));

	iCom_Texture = (*(_int*)pArg); // ≈ÿΩ∫√ƒ ¡§∫∏
	CHECK_FAILED(SetUp_Components());

	// UI ±‚∫ª Setting ===

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
	case Engine::CUI_Parents::MENU_BACKGROUND:

		SizeX = 1280.f * 0.01f;
		SizeY = 720.f * 0.01f;
		SetX = 640.f;
		SetY = 360.f;


		fBackGround_Size = 0.01f;

		m_fDepth = 0.09f; // ±Ì¿Ã
		break;
	case Engine::CUI_Parents::MENU_FRAME:

		SizeX = 70.f;
		SizeY = 720.f;
		SetX = -35.f; // 35.f
		SetY = 360.f;

		m_fDepth = 0.07f; // ±Ì¿Ã

		iCom_Texture_Set = CUI_Parents::UI_MENU::MENU_PLAYER_BACK;
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Menu"), &iCom_Texture_Set)))
			return E_FAIL;
		break;
	case Engine::CUI_Parents::MENU_FRAME_MONEY:

		SizeX = 322.f;
		SizeY = 39.f;
		SetX = 250.f;
		SetY = -40.f; // 40.f

		m_fDepth = 0.0f; // ±Ì¿Ã

		iCom_Texture_Set = CUI_Parents::UI_MENU::MENU_ICON_ITEM;
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Menu"), &iCom_Texture_Set)))
			return E_FAIL;

		iCom_Texture_Set = CUI_Parents::UI_MENU::MENU_ICON_SHOP;
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Menu"), &iCom_Texture_Set)))
			return E_FAIL;

		iCom_Texture_Set = CUI_Parents::UI_MENU::MENU_PLAYER;
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Menu"), &iCom_Texture_Set)))
			return E_FAIL;

		break;
	case Engine::CUI_Parents::MENU_ICON_ITEM:

		SizeX = 256.f * 0.01f;
		SizeY = 256.f * 0.01f;
		SetX = 300.f;
		SetY = 500.f;

		m_fDepth = 0.07f; // ±Ì¿Ã
		break;
	case Engine::CUI_Parents::MENU_ICON_SHOP:

		SizeX = 256.f * 0.01f;
		SizeY = 256.f * 0.01f;
		SetX = 500.f;
		SetY = 300.f;

		m_fDepth = 0.07f; // ±Ì¿Ã
		break;
	case Engine::CUI_Parents::MENU_PLAYER:
		SizeX = 400.f*1.2f;
		SizeY = 640.f*1.2f;
		SetX = 1600.f;
		SetY = 360.f;

		m_fDepth = 0.07f; // ±Ì¿Ã
		break;
	case Engine::CUI_Parents::MENU_PLAYER_BACK:
		SizeX = 800.f;
		SizeY = 720.f;
		SetX = 880.f;
		SetY = 360.f;

		m_fDepth = 0.08f; // ±Ì¿Ã
		break;
	default:
		break;
	}
	/*
		m_fDepth = 0.08; << µ⁄ø° simbol ∂≥æÓ¡ˆ¥¬∞≈
	*/
	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	return S_OK;
}

_int CUI_Menu::Tick(_double TimeDelta)
{
	if (true == m_bDead)
	{
		if (iCom_Texture == CUI_Parents::MENU_BACKGROUND)
			GM->PlaySpeedControl(1.f);
		return RESULT_DEAD;
	}

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	dBurbleTime += TimeDelta;
	if (0.25 < dBurbleTime)
	{
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Burble"), TEXT("Prototype_GameObject_UI_Burble"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		dBurbleTime = 0.0;
	}

	if (false == Inven->Get_ItemWindow() && false == Inven->Get_ShopWindow()) /* Window On/off ===== */
	{
		switch (iCom_Texture)
		{
		case Engine::CUI_Parents::MENU_BACKGROUND:
			if (1 == GM->Get_MenuStart())
			{

				if (0 == GM->Get_MenuPatternCnt())
				{
					dBackGrondTime += TimeDelta;

					if (0.01 < dBackGrondTime)
					{
						fBackG_SizeSpeed += 0.07f;

						if (1.f > fBackGround_Size)
							fBackGround_Size += fBackG_SizeSpeed;

						if (1.f <= fBackGround_Size)
						{
							fBackGround_Size = 1.f;
							fBackG_SizeSpeed = 0.f;
							GM->Set_MenuPatternCnt(1);
						}

						SizeX = 1280.f * fBackGround_Size;
						SizeY = 720.f * fBackGround_Size;

						dBackGrondTime = 0.0;
					}
				}
			}
			if (3 == GM->Get_MenuStart())
			{
				if (5 == GM->Get_MenuPatternCnt())
				{
					dBackGrondTime += TimeDelta;

					if (0.01 < dBackGrondTime)
					{
						fBackG_SizeSpeed += 0.1f;

						if (0.f < fBackGround_Size)
							fBackGround_Size -= fBackG_SizeSpeed;

						if (0.f >= fBackGround_Size)
						{
							fBackGround_Size = 0.f;
							fBackG_SizeSpeed = 0.f;
							GM->Set_MenuPatternCnt(0);
							GM->Set_MenuStart(0);
							m_bDead = true;
						}

						SizeX = 1280.f * fBackGround_Size;
						SizeY = 720.f * fBackGround_Size;

						dBackGrondTime = 0.0;
					}

					if (FAILED(pGameInstance->Release_LayerObject(LEVEL_TWILIGHT, TEXT("Layer_Burble"))))
					{
						BREAKPOINT;
						return E_FAIL;
					}
				}
			}
			break;

		case Engine::CUI_Parents::MENU_FRAME:
			if (1 == GM->Get_MenuStart())
			{
				if (1 == GM->Get_MenuPatternCnt())
				{
					dFrameTime += TimeDelta;

					if (0.01 < dFrameTime)
					{
						fFrameX_Speed += 0.3f;

						if (35.f > SetX)
							SetX += fFrameX_Speed;

						if (35.f <= SetX)
						{
							SetX = 35.f;
							fFrameX_Speed = 0.f;
							GM->Set_MenuPatternCnt(2); // ¿Ã»ƒ ø™¿∏∑Œ
						}

						dFrameTime = 0.0;
					}
				}
			}
			if (3 == GM->Get_MenuStart())
			{
				if (4 == GM->Get_MenuPatternCnt())
				{
					dFrameTime += TimeDelta;

					if (0.01 < dFrameTime)
					{
						fFrameX_Speed += 0.3f;

						if (-35.f < SetX)
							SetX -= fFrameX_Speed;

						if (-35.f >= SetX)
						{
							SetX = -35.f;
							fFrameX_Speed = 0.f;
							GM->Set_MenuPatternCnt(5); // ¿Ã»ƒ ø™¿∏∑Œ
							m_bDead = true;
						}

						dFrameTime = 0.0;
					}
				}
			}
			break;

		case Engine::CUI_Parents::MENU_FRAME_MONEY:
			if (1 == GM->Get_MenuStart())
			{
				if (2 == GM->Get_MenuPatternCnt())
				{
					dMoneyBarTime += TimeDelta;

					if (0.01 < dMoneyBarTime)
					{
						fMoneyBarY_Speed += 0.3f;

						if (40.f > SetY)
							SetY += fMoneyBarY_Speed;

						if (40.f <= SetY)
						{
							SetY = 40.f;
							fMoneyBarY_Speed = 0.f;

							NM->Set_MenuNum_Render(true);
							GM->Set_MenuPatternCnt(3);
							GM->Set_MenuStart(2);
						}
						dMoneyBarTime = 0.0;

						bMoneyRender = true;
					}
				}
			}
			if (3 == GM->Get_MenuStart())
			{
				if (3 == GM->Get_MenuPatternCnt())
				{
					dMoneyBarTime += TimeDelta;

					if (0.01 < dMoneyBarTime)
					{
						fMoneyBarY_Speed += 0.3f;

						if (-40.f < SetY)
							SetY -= fMoneyBarY_Speed;

						if (-40.f >= SetY)
						{
							SetY = -40.f;
							fMoneyBarY_Speed = 0.f;
							GM->Set_MenuPatternCnt(4);
							m_bDead = true;
						}
						dMoneyBarTime = 0.0;
					}
				}
			}
			break;
		case Engine::CUI_Parents::MENU_ICON_ITEM:
			if (2 == GM->Get_MenuStart())
			{
				if (1.f > fIconSize)
				{
					dIconSizeTime += TimeDelta;
					if (0.01 < dIconSizeTime)
					{
						fIconSizeSpeed += 0.04f;
						dIconSizeTime = 0.0;
					}

					fIconSize += fIconSizeSpeed;
				}


				if (0 == GM->Get_MenuIconCnt())
				{
					if (1.f <= fIconSize)
					{
						fIconSize = 1.f;
						fIconSizeSpeed = 0.f;
					}
					/* ±‚¥… =====*/
					if (m_pGameInstance->Get_KeyEnter(DIK_RETURN))
					{
						Inven->Set_InvenDelete_List(false);
						Inven->Set_ShopItemText(false);

						iCom_Texture_Set = CUI_Parents::MENU_WINDOW::ITEMWINDOW0;
						if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
							return 0;

						Inven->Set_ItemWindow(true);

						SM->Sound_Command(L"Menu_Select.wav", 1.f);
					}
					/* ========== */
				}
				else
				{
					if (0.5f <= fIconSize)
					{
						fIconSize = 0.5f;
						fIconSizeSpeed = 0.f;
					}
				}

				SizeX = 256.f*fIconSize;
				SizeY = 256.f*fIconSize;

			}
			if (3 == GM->Get_MenuStart())
			{
				m_bDead = true;
			}

			break;
		case Engine::CUI_Parents::MENU_ICON_SHOP:
			if (2 == GM->Get_MenuStart())
			{
				if (1.f > fIconSize)
				{
					dIconSizeTime += TimeDelta;
					if (0.01 < dIconSizeTime)
					{
						fIconSizeSpeed += 0.04f;
						dIconSizeTime = 0.0;
					}

					fIconSize += fIconSizeSpeed;
				}


				if (1 == GM->Get_MenuIconCnt())
				{
					if (1.f <= fIconSize)
					{
						fIconSize = 1.f;
						fIconSizeSpeed = 0.f;
					}
					/* ±‚¥… =====*/
					if (m_pGameInstance->Get_KeyEnter(DIK_RETURN))
					{
						Inven->Set_InvenDelete_List(false);
						Inven->Set_ShopItemText(true);

						iCom_Texture_Set = CUI_Parents::MENU_WINDOW::SHOPWINDOW0;
						if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu_Window"), &iCom_Texture_Set)))
							return 0;

						Inven->Set_ShopWindow(true);
						SM->Sound_Command(L"Menu_Select.wav", 1.f);
					}
					/* ========== */
				}
				else
				{
					if (0.5f <= fIconSize)
					{
						fIconSize = 0.5f;
						fIconSizeSpeed = 0.f;
					}
				}

				SizeX = 256.f*fIconSize;
				SizeY = 256.f*fIconSize;

			}
			if (3 == GM->Get_MenuStart())
			{
				m_bDead = true;
			}

			break;
		case Engine::CUI_Parents::MENU_PLAYER:
			if (2 == GM->Get_MenuStart())
			{
				dPlayerTime += TimeDelta;
				if (0.01 < dPlayerTime)
				{
					fPlayerSetXSpeed *= 1.5f;
					dPlayerTime = 0.0;
				}

				if (520.f <= fPlayerSetXSpeed)
				{
					fPlayerSetXSpeed = 520.f;
				}

				fPlayerSetX = 1600.f - fPlayerSetXSpeed;
				SetX = fPlayerSetX;
			}
			if (3 == GM->Get_MenuStart())
			{
				m_bDead = true;
			}

			break;
		case Engine::CUI_Parents::MENU_PLAYER_BACK:
			if (3 == GM->Get_MenuStart())
			{
				m_bDead = true;
			}
			break;
		default:
			break;
		}

		m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

		if (2 == GM->Get_MenuStart()) /* MenuIcon_Select */
		{
			if (pGameInstance->Get_KeyEnter(DIK_UP))
			{
				GM->Math_MenuIconCnt(-1);
				SM->Sound_Command(L"Menu_Move.wav", 1.f);
			}
				
			if (pGameInstance->Get_KeyEnter(DIK_DOWN))
			{
				GM->Math_MenuIconCnt(1);
				SM->Sound_Command(L"Menu_Move.wav", 1.f);
			}
				
			if (-1 >= GM->Get_MenuIconCnt())
				GM->Set_MenuIconCnt(1);

			if (2 <= GM->Get_MenuIconCnt())
				GM->Set_MenuIconCnt(0);
		}

		/* ================================= */
		/* ================================= */

		if (pGameInstance->Get_KeyEnter(DIK_BACKSPACE) && 2 == GM->Get_MenuStart())
		{
			SM->Sound_Command(L"UI_Menu_Close.wav", 1.f);

			GM->Set_MenuStart(3);
			NM->Set_MenuNum_Render(false); /* Money Render */
			GM->Set_InvenOn(false);
		}

	} /* Window On/off ===== */

	  /* Money Render */
	if (true == bMoneyRender)
	{
		NM->Setting_Menu(Inven->Get_TotalMoney(), 20.f);
	}
	/* ============ */

	return _int();
}

_int CUI_Menu::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::MENU_BACKGROUND:
		break;
	case Engine::CUI_Parents::MENU_FRAME:
		break;
	case Engine::CUI_Parents::MENU_FRAME_MONEY:
		break;
	case Engine::CUI_Parents::MENU_ICON_ITEM:
		break;
	case Engine::CUI_Parents::MENU_ICON_SHOP:
		break;
	case Engine::CUI_Parents::MENU_PLAYER:
		break;
	case Engine::CUI_Parents::MENU_PLAYER_BACK:
		break;
	default:
		break;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CUI_Menu::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::MENU_BACKGROUND:
		break;
	case Engine::CUI_Parents::MENU_FRAME:
		break;
	case Engine::CUI_Parents::MENU_FRAME_MONEY:
		break;
	case Engine::CUI_Parents::MENU_ICON_ITEM:
		break;
	case Engine::CUI_Parents::MENU_ICON_SHOP:
		break;
	case Engine::CUI_Parents::MENU_PLAYER:
		break;
	case Engine::CUI_Parents::MENU_PLAYER_BACK:
		break;
	default:
		break;
	}

	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT CUI_Menu::SetUp_Components()
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
	case Engine::CUI_Parents::MENU_BACKGROUND:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_Background"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::MENU_FRAME:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_Frame"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::MENU_FRAME_MONEY:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_Frame_Money"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::MENU_ICON_ITEM:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_Icon_Item"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::MENU_ICON_SHOP:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_Icon_Shop"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::MENU_PLAYER:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_Player"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::MENU_PLAYER_BACK:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_Player_Back"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CUI_Menu::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture"));

	return S_OK;
}

CUI_Menu * CUI_Menu::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Menu*	pInstance = new CUI_Menu(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_Menu");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Menu::Clone(void * pArg)
{
	CUI_Menu*	pInstance = new CUI_Menu(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_Menu");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Menu::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
