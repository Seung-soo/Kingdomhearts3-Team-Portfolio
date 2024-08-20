#include "..\public\Level_Manager.h"
#include "GameInstance.h"

READY_SINGLETON(CLevel_Manager)

CLevel_Manager::CLevel_Manager()
{
}

HRESULT CLevel_Manager::OpenLevel(_uint iNextLevelIndex, CLevel * pNextLevel)
{
	if (nullptr == pNextLevel)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pCurrentLevel)
	{
		// 기존 레벨에 있는 오브젝트, 컴포넌트 프로토타입 삭제
		if (FAILED(pGameInstance->Clear_LevelResource(m_iCurrentLevelIndex)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	Safe_Release(m_pCurrentLevel);

	m_iCurrentLevelIndex = iNextLevelIndex;
	m_pCurrentLevel = pNextLevel;

	return S_OK;
}

_int CLevel_Manager::Tick(_double dTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
	{
		BREAKPOINT;
		return -1;
	}

	return m_pCurrentLevel->Tick(dTimeDelta);
}

_int CLevel_Manager::LateTick(_double dTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
	{
		BREAKPOINT;
		return -1;
	}
	return m_pCurrentLevel->LateTick(dTimeDelta);

}

HRESULT CLevel_Manager::Render()
{
	if (nullptr == m_pCurrentLevel)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return m_pCurrentLevel->Render();
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pCurrentLevel);
}
