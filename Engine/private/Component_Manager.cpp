#include "..\public\Component_Manager.h"

READY_SINGLETON(CComponent_Manager)

CComponent_Manager::CComponent_Manager()
{
}

HRESULT CComponent_Manager::Reserve_Container(_uint iNumLevels)
{
	m_iNumLevels = iNumLevels;

	m_pPrototypes = new PROTOTYPES[iNumLevels];

	return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (nullptr != Find_Component(iLevelIndex, pPrototypeTag))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_pPrototypes[iLevelIndex].emplace(pPrototypeTag, pPrototype);

	return S_OK;
}

HRESULT CComponent_Manager::Rmv_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (nullptr == Find_Component(iLevelIndex, pPrototypeTag))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	Safe_Release(m_pPrototypes[iLevelIndex].find(pPrototypeTag)->second);
	m_pPrototypes[iLevelIndex].erase(pPrototypeTag);

	return S_OK;
}

CComponent * CComponent_Manager::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		BREAKPOINT;
		return nullptr;
	}

	if (nullptr == m_pPrototypes)
	{
		BREAKPOINT;
		return nullptr;
	}

	CComponent*		pPrototype = Find_Component(iLevelIndex, pPrototypeTag);
	if (nullptr == pPrototype)
	{
		BREAKPOINT;
		return nullptr;
	}

	CComponent*		pComponent = pPrototype->Clone(pArg);
	if (nullptr == pComponent)
	{
		BREAKPOINT;
		return nullptr;
	}

	return pComponent;
}

HRESULT CComponent_Manager::Clear_LevelComponent(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	for (auto& Pair : m_pPrototypes[iLevelIndex])
		Safe_Release(Pair.second);

	m_pPrototypes[iLevelIndex].clear();

	return S_OK;
}

void CComponent_Manager::Clear_All_Components()
{
	for (_uint i = 1; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pPrototypes[i])
			Safe_Release(Pair.second);
		m_pPrototypes[i].clear();
	}
}

CComponent * CComponent_Manager::Find_Component(_uint iLevelIndex, const _tchar * pPrototypeTag)
{
	auto iter = find_if(m_pPrototypes[iLevelIndex].begin(), m_pPrototypes[iLevelIndex].end(), CTagFinder(pPrototypeTag));

	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CComponent_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; i++)
	{
		_uint	iTest = 0;
		for (auto& Pair : m_pPrototypes[i])
		{
			//ForTest
			Safe_Release(Pair.second);
			iTest++;
		}
		m_pPrototypes[i].clear();
	}
	Safe_Delete_Array(m_pPrototypes);
}
