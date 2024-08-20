#include "..\public\Object_Manager.h"

READY_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{
}

CComponent * CObject_Manager::Get_Component(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		BREAKPOINT;
		return nullptr;
	}

	CLayer*		pLayer = Find_Layer(iLevelIndex, pLayerTag);
	if (nullptr == pLayer)
	{
		return nullptr;
	}
	return pLayer->Get_Component(pComponentTag, iIndex);
}

CGameObject * CObject_Manager::Get_Front(_uint iLevelIndex, const _tchar * pLayerTag)
{
	auto iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	auto ObjectList = iter->second->Get_Object();

	if (0 == ObjectList->size())
		return nullptr;

	return ObjectList->front();
}

CGameObject * CObject_Manager::Get_Back(_uint iLevelIndex, const _tchar * pLayerTag)
{
	auto iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	auto ObjectList = iter->second->Get_Object();

	if (0 == ObjectList->size())
		return nullptr;

	return ObjectList->back();
}

CGameObject * CObject_Manager::Get_ObjectPoolBack(_uint iLevelIndex, const _tchar * pLayerTag)
{
	auto iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	auto ObjectList = iter->second->Get_ObjectPool();

	if (0 == ObjectList->size())
		return nullptr;

	return ObjectList->back();
}

map<const _tchar*, CLayer*> CObject_Manager::Get_ObjectMap(_uint iLevelIndex)
{
	return m_pLayers[iLevelIndex];
}

list<CGameObject*>* CObject_Manager::Get_ObjectList(_uint iLevelIndex, const _tchar * pLayerTag)
{
	CLayer* pList = Find_Layer(iLevelIndex, pLayerTag);
	if (nullptr == pList)
		return nullptr;
	else
		return pList->Get_Object();
}

CGameObject* CObject_Manager::Get_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, _uint iIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_GameObject(iIndex);
}

void CObject_Manager::Set_TimeSpeed(_uint iLevelIndex, const _tchar * pLayerTag, _double TimeSpeed)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		BREAKPOINT;
		return;
	}

	CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);
	if (nullptr == pLayer)
	{
		return;
	}

	pLayer->Set_TimeSpeed(TimeSpeed);
}

HRESULT CObject_Manager::Save_Layer(_uint iLevelIndex, const _tchar* pLayerTag, HANDLE hFile)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
		return E_FAIL;

	pLayer->Save_Layer(hFile);

	return S_OK;
}

HRESULT CObject_Manager::Reserve_Container(_uint iNumLevels)
{
	m_iNumLevels = iNumLevels;

	m_pLayers = new LAYERS[m_iNumLevels];

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == pPrototype)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (nullptr != Find_Prototype(pPrototypeTag))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_Prototypes.emplace(pPrototypeTag, pPrototype);

	return S_OK;
}

HRESULT CObject_Manager::Rmv_Prototype(const _tchar * pPrototypeTag)
{
	if (nullptr == Find_Prototype(pPrototypeTag))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	Safe_Release(m_Prototypes.find(pPrototypeTag)->second);
	m_Prototypes.erase(pPrototypeTag);

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CGameObject*	pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CGameObject*	pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CLayer*			pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
		{
			BREAKPOINT;
			return E_FAIL;
		}

		pGameObject->Set_LayerTag(pLayerTag);
		if (FAILED(pLayer->Add_GameObject(pGameObject)))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		m_pLayers[iLevelIndex].emplace(pLayerTag, pLayer);
	}
	else
	{
		pGameObject->Set_LayerTag(pLayerTag);
		if (FAILED(pLayer->Add_GameObject(pGameObject)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CObject_Manager::Rmv_GameObject(_uint iLevelIndex, const _tchar * pLayerTag, CGameObject * pObject)
{
	if (nullptr == pLayerTag ||
		nullptr == pObject)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CLayer* target_layer = Find_Layer(iLevelIndex, pLayerTag);
	target_layer->Delete_Object(pObject);

	return S_OK;
}

CGameObject* CObject_Manager::Add_GameObject_And_Return(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		return nullptr;
	}

	CGameObject* pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
	{
		return nullptr;
	}

	CGameObject* pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
	{
		return nullptr;
	}

	CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
		{
			return nullptr;
		}

		pGameObject->Set_LayerTag(pLayerTag);
		if (FAILED(pLayer->Add_GameObject(pGameObject)))
		{
			return nullptr;
		}

		m_pLayers[iLevelIndex].emplace(pLayerTag, pLayer);
	}
	else
	{
		pGameObject->Set_LayerTag(pLayerTag);
		if (FAILED(pLayer->Add_GameObject(pGameObject)))
		{
			return nullptr;
		}
	}

	return pGameObject;
}

HRESULT CObject_Manager::ThrowToLayer(_uint iLevelIndex, const _tchar * pLayerTag, void* pArg)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		BREAKPOINT;
		return E_FAIL;
	}
	else
	{
		if (FAILED(pLayer->ThrowToLayer(pArg)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	return S_OK;
}

CGameObject* CObject_Manager::ThrowToLayer_And_Return(_uint iLevelIndex, const _tchar* pLayerTag, void* pArg)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		BREAKPOINT;
		return nullptr;
	}

	CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		BREAKPOINT;
		return nullptr;
	}
	else
	{
		CGameObject* pGameObject = pLayer->ThrowToLayer_And_Return(pArg);
		return pGameObject;
	}

	return nullptr;
}

HRESULT CObject_Manager::Add_ObjectPool(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CGameObject* pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CGameObject* pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
		{
			BREAKPOINT;
			return E_FAIL;
		}

		pGameObject->Set_LayerTag(pLayerTag);
		if (FAILED(pLayer->Add_ObjectPool(pGameObject)))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		m_pLayers[iLevelIndex].emplace(pLayerTag, pLayer);
	}
	else
	{
		pGameObject->Set_LayerTag(pLayerTag);
		if (FAILED(pLayer->Add_ObjectPool(pGameObject)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}


	return S_OK;

}

HRESULT CObject_Manager::Clear_LevelObject(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	for (auto& Pair : m_pLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();

	return S_OK;
}

HRESULT CObject_Manager::Release_LayerObject(_uint iLevelIndex, const _tchar* pLayerTag)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	auto iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));

	if (iter == m_pLayers[iLevelIndex].end())
		return S_OK;

	CLayer* pLayer = iter->second;
	pLayer->Free();
	Safe_Release(pLayer);

	m_pLayers[iLevelIndex].erase(iter);

	return S_OK;
}

HRESULT CObject_Manager::Clear_Layer(_uint iLevelIndex, const _tchar* pLayerTag)
{
	CLayer* pList = Find_Layer(iLevelIndex, pLayerTag);
	
	return pList->Clear_Layer();
}

HRESULT CObject_Manager::FrontChange(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		BREAKPOINT;
		return E_FAIL;
	}
	auto iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));

	if (iter == m_pLayers[iLevelIndex].end())
	{
		BREAKPOINT;
		return E_FAIL;
	}

	((*iter).second)->FrontChange();

	return S_OK;
}

_int CObject_Manager::Tick(_double dTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr == Pair.second)
				continue;
			RETURN_NEGATIVE(if (0 > Pair.second->Tick(dTimeDelta)))
		}
	}
	return RESULT_NOPROBLEM;
}

_int CObject_Manager::LateTick(_double dTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr == Pair.second)
				continue;
			RETURN_NEGATIVE(if (0 > Pair.second->LateTick(dTimeDelta)))
		}
	}
	return RESULT_NOPROBLEM;
}

void CObject_Manager::GameObject_PostRender(_double TimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->PostRender(TimeDelta);
		}
	}
}

CGameObject * CObject_Manager::Find_Prototype(const _tchar * pPrototypeTag)
{
	auto iter = find_if(m_Prototypes.begin(), m_Prototypes.end()
		, CTagFinder(pPrototypeTag));

	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

CLayer * CObject_Manager::Find_Layer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	auto iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CObject_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);

		m_pLayers[i].clear();
	}

	Safe_Delete_Array(m_pLayers);

	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);

	m_Prototypes.clear();
}
