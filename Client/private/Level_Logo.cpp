#include "stdafx.h"
#include "..\Public\Level_Logo.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_Dynamic.h"
#include "GameManager.h"

#include "UI_Parents.h"
#include "UI_Logo.h"


CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{

}

HRESULT CLevel_Logo::NativeConstruct()
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

	//m_pGameInstance->PlayBGM(L"Logo.wav", 0.5f);

	return S_OK;
}

_int CLevel_Logo::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	if (true == GM->Get_MoveOnToNextLevel())
	{
		if (FAILED(m_pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, GM->Get_NextLevel()))))
		{
			BREAKPOINT;
			return RESULT_ERROR;
		}
	}

	/* UI_BackSimbol */
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (15 > GM->Get_SimbolCnt())
	{
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Layer_Logo"), TEXT("Prototype_GameObject_BackSimbol"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		GM->Math_SimbolCnt(1);

		if (false == bBackSimbol)
		{
			GM->Math_GCnt(1);
			iBackSimbolCnt += 1;

			if (15 == iBackSimbolCnt)
			{
				GM->Set_GCnt(4);
				bBackSimbol = true;
			}
				
		}	
	}

	return RESULT_NOPROBLEM;
}

_int CLevel_Logo::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	/* */
	if (pGameInstance->Get_KeyEnter(DIK_UP))
		iSelect_Point += 1;

	if (pGameInstance->Get_KeyEnter(DIK_DOWN))
		iSelect_Point -= 1;

	if (2 <= iSelect_Point)
		iSelect_Point = 0;

	if (-1 >= iSelect_Point)
		iSelect_Point = 1;

	/* Go to LEVEL_TWILIGHT ===== */
	if (0 == iSelect_Point && pGameInstance->Get_KeyEnter(DIK_Q)) // LEVEL_TWILIGHT
	{
		//if (FAILED(pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_TWILIGHT))))
		//{
		//	BREAKPOINT;
		//	return RESULT_ERROR;
		//}
		GM->Set_bLoadingStart(true);
		bLoading = true;
		m_pGameInstance->StopSound(CSoundMgr::CHANNELID::BGM);
		m_pGameInstance->LetsPlaySound(L"Logo_Select.wav", CSoundMgr::CHANNELID::EFFECT, 0.5f);
		m_pGameInstance->LetsPlaySound(L"Logo_Start.wav", CSoundMgr::CHANNELID::EFFECT1, 0.5f);
	}
		
	
	if (1 == iSelect_Point && pGameInstance->Get_KeyEnter(DIK_RETURN)) // EXIT
	{
		PostQuitMessage(0);
	}


	if (true == bLoading) // LEVEL_TWILIGHT
	{
		dLoadingTime += TimeDelta;

		if (5.0 < dLoadingTime)
		{
			if (false == bBlackCreate)
			{
				iCom_Texture_Set = 0;
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Layer_Logo"), TEXT("Prototype_GameObject_LoadingBlack"), &iCom_Texture_Set)))
				{
					BREAKPOINT;
					return E_FAIL;
				}
				bBlackCreate = true;
			}
		}

		if (7.0 < dLoadingTime)
		{
			if (FAILED(pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_TWILIGHT))))
			{
				BREAKPOINT;
				return RESULT_ERROR;
			}
		}

	}
	/* ========================= */

	

#ifdef _DEBUG
	// 맵툴 레벨로
	if (pGameInstance->Get_KeyEnter(DIK_M))
	{
		m_pGameInstance->StopSound(CSoundMgr::CHANNELID::BGM);

		if (FAILED(pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_MAPTOOL))))
		{
			BREAKPOINT;
			return RESULT_ERROR;
		}
	}

	// 이펙트 레벨로
	if (pGameInstance->Get_KeyEnter(DIK_E))
	{
		m_pGameInstance->StopSound(CSoundMgr::CHANNELID::BGM);

		if (FAILED(pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_EFFECTTOOL))))
		{
			BREAKPOINT;
			return RESULT_ERROR;
		}
	}

	// 콜라이더 레벨로
	if (pGameInstance->Get_KeyEnter(DIK_C))
	{
		m_pGameInstance->StopSound(CSoundMgr::CHANNELID::BGM);

		if (FAILED(pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_COLLIDERTOOL))))
		{
			BREAKPOINT;
			return RESULT_ERROR;
		}
	}

	// 테스트 레벨로
	if (pGameInstance->Get_KeyEnter(DIK_T))
	{
		m_pGameInstance->StopSound(CSoundMgr::CHANNELID::BGM);

		if (FAILED(pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_TEST))))
		{
			BREAKPOINT;
			return RESULT_ERROR;
		}
	}

	/* 엔딩 크레딧 */
	if (pGameInstance->Get_KeyEnter(DIK_L))
	{
		m_pGameInstance->StopSound(CSoundMgr::CHANNELID::BGM);

		if (FAILED(pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_ENDING))))
		{
			BREAKPOINT;
			return RESULT_ERROR;
		}
	}
#endif

	/* 엔딩 크레딧 */
	if (pGameInstance->Get_KeyEnter(DIK_L))
	{
		m_pGameInstance->StopSound(CSoundMgr::CHANNELID::BGM);

		if (FAILED(pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_ENDING))))
		{
			BREAKPOINT;
			return RESULT_ERROR;
		}
	}

	return 0;
}

HRESULT CLevel_Logo::Render()
{
	if (FAILED(__super::Render()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Prototype_GameObject()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	//Logo
	iCom_Texture_Set = CUI_Parents::UI_LOGO::LOGO_MAIN;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Layer_Logo"), TEXT("Prototype_GameObject_Logo"),&iCom_Texture_Set)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	iCom_Texture_Set = CUI_Parents::UI_LOGO::LOGO_BUTTON0;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Layer_Logo"), TEXT("Prototype_GameObject_Logo"), &iCom_Texture_Set)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	iCom_Texture_Set = CUI_Parents::UI_LOGO::LOGO_BUTTON1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Layer_Logo"), TEXT("Prototype_GameObject_Logo"), &iCom_Texture_Set)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	iCom_Texture_Set = CUI_Parents::UI_LOGO::LOGO_TITLE;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Layer_Logo"), TEXT("Prototype_GameObject_Logo"), &iCom_Texture_Set)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	iCom_Texture_Set = CUI_Parents::UI_LOGO::LOGO_HEART;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Layer_Logo"), TEXT("Prototype_GameObject_Logo"), &iCom_Texture_Set)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	iCom_Texture_Set = CUI_Parents::UI_LOGO::LOGO_TRD;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Layer_Logo"), TEXT("Prototype_GameObject_Logo"), &iCom_Texture_Set)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	iCom_Texture_Set = CUI_Parents::UI_LOGO::LOGO_CROWN;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Layer_Logo"), TEXT("Prototype_GameObject_Logo"), &iCom_Texture_Set)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	iCom_Texture_Set = CUI_Parents::UI_LOGO::LOGO_SORA;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Layer_Logo"), TEXT("Prototype_GameObject_Logo"), &iCom_Texture_Set)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	iCom_Texture_Set = CUI_Parents::UI_LOGO::LOGO_POINT;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Layer_Logo"), TEXT("Prototype_GameObject_Logo"), &iCom_Texture_Set)))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	
	return S_OK;
}

CLevel_Logo * CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Logo*	pInstance = new CLevel_Logo(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();

}
