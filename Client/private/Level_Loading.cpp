#include "stdafx.h"
#include "..\Public\Level_Loading.h"
#include "Loader.h"
#include "GameInstance.h"
#include "LEVEL_TWILIGHT.h"
#include "Level_MapTool.h"
#include "Level_EffectTool.h"
#include "Level_ColliderTool.h"
#include "Level_Test.h"
#include "Level_Rapunzel.h"
#include "Level_RapunzelBoss.h"
#include "Level_Frozen.h"
#include "Level_KeyGrave.h"
#include "Level_FinalBoss.h"
#include "Level_Endding.h"
#include "UI_Parents.h"
#include "GameManager.h"
#include "Progress_Manager.h"
#include "Camera_Perspective.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Loading::NativeConstruct(LEVEL eNextLevel)
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	//	몬스터 리스트 클리어
	GM->Get_MonstersList()->clear();

	m_eNextLevel = eNextLevel;

	m_pLoader = CLoader::Create(m_pDevice, m_pDeviceContext, m_eNextLevel);

	if (FAILED(Ready_Layer_LoadingUI(TEXT("Layer_LoadingUI"), LEVEL_LOADING)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

_int CLevel_Loading::Tick(_double TimeDelta)
{
	if(0 > __super::Tick(TimeDelta))
		return -1;


	return 0;
}

_int CLevel_Loading::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return -1;

	if (true == m_pLoader->isFinished())
	{
		CGameInstance*	pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		// 씬 옮길 때마다 라이트 다 지워줌
		pGameInstance->Release_Light();

		CLevel*		pLevel = nullptr;

		CRenderer*	pRenderer = static_cast<CRenderer*>(pGameInstance->Find_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer")));

		GM->Set_CurrentLevel(m_eNextLevel);

		switch (m_eNextLevel)
		{
		case LEVEL_TWILIGHT:
			pLevel = CLevel_Twilight::Create(m_pDevice, m_pDeviceContext);
			pRenderer->Set_StartLum(0);
			pRenderer->Reset_Values();
			pGameInstance->Set_StaticLightViewMatrix(__vector(-150.f, 150.f, -150.f, 1.f));
			pRenderer->Set_LUTDesc(false, 0);
			break;
		case LEVEL_RAPUNZEL:
			pLevel = CLevel_Rapunzel::Create(m_pDevice, m_pDeviceContext);
			pRenderer->Set_StartLum(0);
			pRenderer->Reset_Values();
			pGameInstance->Set_StaticLightViewMatrix(__vector(-200.f, 150.f, -150.f, 1.f));
			pRenderer->Set_LUTDesc(true, 95);
			break;
		case LEVEL_RAPUNZEL_BOSS:
			pLevel = CLevel_RapunzelBoss::Create(m_pDevice, m_pDeviceContext);
			pRenderer->Set_StartLum(0);
			pRenderer->Reset_Values();
			break;
		case LEVEL_FROZEN:
			pLevel = CLevel_Frozen::Create(m_pDevice, m_pDeviceContext);
			pRenderer->Set_StartLum(0);
			pRenderer->Reset_Values(false);
			break;
		case LEVEL_KEYGRAVE:
			pLevel = CLevel_KeyGrave::Create(m_pDevice, m_pDeviceContext);
			pRenderer->Set_StartLum(0);
			pRenderer->Reset_Values(false);
			break;
		case LEVEL_FINALBOSS:
			pLevel = CLevel_FinalBoss::Create(m_pDevice, m_pDeviceContext);
			pRenderer->Set_StartLum(0);
			pRenderer->Reset_Values(false);
			break;
			// =====================================================================================================================================================================
		case LEVEL_MAPTOOL:
			pLevel = CLevel_MapTool::Create(m_pDevice, m_pDeviceContext);
			pRenderer->Set_StartLum(0);
			pRenderer->Reset_Values();
			break;
		case LEVEL_EFFECTTOOL:
			pLevel = CLevel_EffectTool::Create(m_pDevice, m_pDeviceContext);
			pRenderer->Set_StartLum(0);
			pRenderer->Reset_Values();
			break;
		case LEVEL_COLLIDERTOOL:
			pLevel = CLevel_ColliderTool::Create(m_pDevice, m_pDeviceContext);
			pRenderer->Set_StartLum(0);
			pRenderer->Reset_Values();
			break;
		case LEVEL_TEST:
			pLevel = CLevel_Test::Create(m_pDevice, m_pDeviceContext);
			pRenderer->Set_StartLum(0);
			pRenderer->Reset_Values();
			break;

		case LEVEL_ENDING:
			pLevel = CLevel_Ending::Create(m_pDevice, m_pDeviceContext);
			pRenderer->Set_StartLum(0);
			pRenderer->Reset_Values();
			break;
		}

		if (FAILED(pGameInstance->OpenLevel(m_eNextLevel, pLevel)))
			return E_FAIL;

		Safe_Release(pGameInstance);
	}

	return 0;
}

HRESULT CLevel_Loading::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_LoadingUI(const _tchar * pLayerTag, LEVEL eNextLevel)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_LOADING;
	CHECK_FAILED(pGameInstance->Add_GameObject(eNextLevel, pLayerTag, TEXT("Prototype_GameObject_UI_Normal"), &iCom_Texture_Set));

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BLACK;
	CHECK_FAILED(pGameInstance->Add_GameObject(eNextLevel, pLayerTag, TEXT("Prototype_GameObject_UI_Normal"), &iCom_Texture_Set));

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_NOW_LOADING;
	CHECK_FAILED(pGameInstance->Add_GameObject(eNextLevel, pLayerTag, TEXT("Prototype_GameObject_UI_Normal"), &iCom_Texture_Set));

	return S_OK;
}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVEL eNextLevel)
{
	CLevel_Loading*	pInstance = new CLevel_Loading(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(eNextLevel)))
	{
		MSGBOX("Failed to Creating CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
