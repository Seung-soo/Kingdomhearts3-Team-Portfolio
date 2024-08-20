#include "stdafx.h"
#include "FileLoad_Manager.h"
#include "Map.h"

READY_SINGLETON(CFileLoad_Manager)

CFileLoad_Manager::CFileLoad_Manager()
{
}

HRESULT CFileLoad_Manager::FileLoadManager_SetUp()
{
	m_pGameInstance  = CGameInstance::GetInstance();
	m_pDevice        = m_pGameInstance->Get_Device();
	m_pDeviceContext = m_pGameInstance->Get_Device_Context(); 
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	return S_OK;
}

HRESULT CFileLoad_Manager::Load_SaveFile(SAVEFILE eFileType, OUT vector<class CGameObject*>* pObjects)
{
	LEVEL eLevel = LEVEL_END;

	switch (eFileType)
	{
	case SAVEFILE_TT_FIRST:
	{
		eLevel = LEVEL_TWILIGHT;
		CreateObjectFromFile(eLevel, TEXT("../SaveFiles/Objects/TT/TT_First_SkullSoldiers.dat"), TEXT("Layer_SkullSoldier"), pObjects);
		CreateObjectFromFile(eLevel, TEXT("../SaveFiles/Objects/TT/TT_First_CrowSoldiers.dat"), TEXT("Layer_CrowSoldier"), pObjects);
		break;
	}
	case SAVEFILE_TT_SECOND:
	{
		eLevel = LEVEL_TWILIGHT;
		CreateObjectFromFile(eLevel, TEXT("../SaveFiles/Objects/TT/TT_Second_ThinSoldiers.dat"), TEXT("Layer_ThinSoldier"), pObjects);
		break;
	}
	case SAVEFILE_TT_BOSS:
	{
		eLevel = LEVEL_TWILIGHT;
		CreateObjectFromFile(eLevel, TEXT("../SaveFiles/Objects/TT/TT_Boss.dat"), TEXT("Layer_Nightmare"), pObjects);
		break;
	}
	// ==================================================================================================================================================================================================
	case SAVEFILE_RA_FIRST:
		eLevel = LEVEL_RAPUNZEL;
		// 아무것도 로드하지 않음
		break;
	case SAVEFILE_RA_SECOND:
		eLevel = LEVEL_RAPUNZEL;
		CreateObjectFromFile(eLevel, TEXT("../SaveFiles/Objects/RA/RA_First_ShiningEgg.dat"), TEXT("Layer_ShiningEgg"), pObjects);		
		break;
	case SAVEFILE_RA_THIRD:
		eLevel = LEVEL_RAPUNZEL;
		CreateObjectFromFile(eLevel, TEXT("../SaveFiles/Objects/RA/RA_Second_Mower.dat"), TEXT("Layer_Mower"), pObjects);
		CreateObjectFromFile(eLevel, TEXT("../SaveFiles/Objects/RA/RA_Second_Monkey.dat"), TEXT("Layer_Monkey"), pObjects);
		CreateObjectFromFile(eLevel, TEXT("../SaveFiles/Objects/RA/RA_Second_ShiningEgg.dat"), TEXT("Layer_ShiningEgg"), pObjects);
		break;
	case SAVEFILE_RA_FOURTH:
		eLevel = LEVEL_RAPUNZEL;
		CreateObjectFromFile(eLevel, TEXT("../SaveFiles/Objects/RA/RA_Third_ShiningEgg.dat"), TEXT("Layer_ShiningEgg"), pObjects);
		break;
	case SAVEFILE_RA_FIFTH:
		eLevel = LEVEL_RAPUNZEL;
		CreateObjectFromFile(eLevel, TEXT("../SaveFiles/Objects/RA/RA_Fourth_Flower.dat"), TEXT("Layer_Flower"), pObjects);
		break;
	// ==================================================================================================================================================================================================
	case SAVEFILE_RA_BOSS_BIGTREE:
		eLevel = LEVEL_RAPUNZEL_BOSS;
		CreateObjectFromFile(eLevel, TEXT("../SaveFiles/Objects/RA_Boss/RA_Boss_BigTree.dat"), TEXT("Layer_BigTree"), pObjects);
		break;
	// ==================================================================================================================================================================================================
	case SAVEFILE_FZ_ICICLES:
		eLevel = LEVEL_FROZEN;
		CreateObjectFromFile(eLevel, TEXT("../SaveFiles/Objects/FZ/FZ_Icicles.dat"), TEXT("Layer_Icicle"), pObjects);
		break;
	}

	return S_OK;
}

HRESULT CFileLoad_Manager::Load_MapSaveFile(LEVEL eLevel, const _tchar* pFilePath)
{
	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	DWORD dwBytes = 0;
	CMap::MAPDESC MapDesc;
	ZeroMemory(&MapDesc, sizeof(CMap::MAPDESC));
	_float fFrustumRange = 0.f;

	while (true)
	{
		ReadFile(hFile, &MapDesc, sizeof(CMap::MAPDESC), &dwBytes, nullptr);
		ReadFile(hFile, &fFrustumRange, sizeof(_float), &dwBytes, nullptr);

		if (dwBytes == 0)
			break;

		CGameObject* pNewMap = CGameInstance::GetInstance()->Add_GameObject_And_Return(eLevel, TEXT("Layer_Map"), TEXT("Prototype_GameObject_Map"), &MapDesc);

		// 맵은 생성할 때도 pArg 받고, 세이브데이터 로드할 때도 pArg 받기 때문에 Load_ObjectData 를 호출하는 대신에 객체를 생성하고서 월드행렬과 frustumRange 를 따로 세팅해줌
		pNewMap->Get_Transform()->Set_WorldMatrix(MapDesc.WorldMatrix);
		pNewMap->SetFrustumRange(fFrustumRange);
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CFileLoad_Manager::CreateObjectFromFile(LEVEL eLevel, const _tchar* pFilePath, const _tchar* pLayerTag, OUT vector<class CGameObject*>* pObjects)
{
	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	while (true)
	{
		ReadFile(hFile, &m_WorldMatrix, sizeof(_float4x4), &m_dwBytes, nullptr);

		if (m_dwBytes == 0)
			break;

		// 각 레벨에 맞는 오브젝트풀에서 가져오기
		CGameObject* pGameObject = m_pGameInstance->ThrowToLayer_And_Return(eLevel, pLayerTag, nullptr);
		pGameObject->Get_Transform()->Set_WorldMatrix(m_WorldMatrix);

		if (pObjects != nullptr)
			pObjects->push_back(pGameObject);
	}

	CloseHandle(hFile);

	return S_OK;
}

void CFileLoad_Manager::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}