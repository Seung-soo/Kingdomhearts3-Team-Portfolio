#include "..\public\Layer.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Component.h"

CLayer::CLayer()
{
}

CComponent * CLayer::Get_Component(const _tchar * pComponentTag, _uint iIndex)
{
	if (iIndex >= m_Objects.size())
	{
		BREAKPOINT;
		return nullptr;
	}

	auto iter = m_Objects.begin();

	for (_uint i = 0; i < iIndex; i++)
		++iter;
	
	return (*iter)->Get_Component(pComponentTag);
}

CGameObject* CLayer::Get_GameObject(_uint iIndex)
{
	if (iIndex >= m_Objects.size())
		return nullptr;

	auto iter = m_Objects.begin();
	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter);
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	m_Objects.push_back(pGameObject);

	return S_OK;
}

HRESULT CLayer::ThrowToLayer(void* pArg)
{
	if (false == m_ObjectPool.empty())
	{
		m_Objects.push_back(m_ObjectPool.front());
		m_ObjectPool.pop_front();
		m_Objects.back()->NativeConstruct_Pool(pArg);
	}

	return S_OK;
}

CGameObject* CLayer::ThrowToLayer_And_Return(void* pArg)
{
	if (false == m_ObjectPool.empty())
	{
		m_Objects.push_back(m_ObjectPool.front());
		m_ObjectPool.pop_front();
		m_Objects.back()->NativeConstruct_Pool(pArg);

		return m_Objects.back();
	}

	return nullptr;
}

HRESULT CLayer::Add_ObjectPool(CGameObject * pGameObject)
{
	m_ObjectPool.push_back(pGameObject);

	return S_OK;
}

void CLayer::FrontChange()
{
	CGameObject* pGameObject = m_Objects.front();
	m_Objects.pop_front();
	m_Objects.push_back(pGameObject);
}

HRESULT CLayer::Clear_Layer()
{
	for (auto& pGameObject : m_Objects)
		Safe_Release(pGameObject);

	m_Objects.clear();

	return S_OK;
}

HRESULT CLayer::Save_Layer(HANDLE hFile)
{
	if (hFile == INVALID_HANDLE_VALUE)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	for (auto& pGameObejct : m_Objects)
		pGameObejct->Save_ObjectData(hFile);

	return S_OK;
}

_bool CLayer::Find_Object(CGameObject * pGameObject)
{
	if (0 >= m_Objects.size())
		return false;

	auto& iter = m_Objects.begin();
	for (auto& object : m_Objects)
	{
		if (object == pGameObject)
			return true;
		else
			iter++;
	}

	return false;
}

_bool CLayer::Delete_Object(CGameObject * pGameObject)
{
	if (0 >= m_Objects.size())
		return false;

	auto& iter = m_Objects.begin();
	for (auto& object : m_Objects)
	{
		if (object == pGameObject)
		{
			Safe_Release(*iter);
			m_Objects.erase(iter);
			return true;
		}
		else
			iter++;
	}

	return false;
}

_int CLayer::Tick(_double dTimeDelta)
{
	for (auto iter = m_Objects.begin(); iter != m_Objects.end();)
	{
		if (m_Objects.empty())
			break;

		_int iState = (*iter)->Tick(dTimeDelta * m_TimeSpeed);

		switch (iState)
		{
		case RESULT_ERROR:
			__debugbreak();
			return RESULT_ERROR;
		case RESULT_NOPROBLEM:
			iter++;
			break;
		case RESULT_DEAD:
			Safe_Release(*iter);
			iter = m_Objects.erase(iter);
			break;
		case RESULT_POOL:
			(*iter)->Set_Dead(false);
			Add_ObjectPool(*iter);
			iter = m_Objects.erase(iter);
			break;
		default:
			iter++;
		}
	}
	return RESULT_NOPROBLEM;
}

_int CLayer::LateTick(_double dTimeDelta)
{
	for (auto iter = m_Objects.begin(); iter != m_Objects.end();)
	{
		if (m_Objects.empty())
			break;

		_int iState = (*iter)->LateTick(dTimeDelta * m_TimeSpeed);

		switch (iState)
		{
		case RESULT_ERROR:
			__debugbreak();
			return RESULT_ERROR;
		case RESULT_NOPROBLEM:
			iter++;
			break;
		case RESULT_DEAD:
			Safe_Release(*iter);
			iter = m_Objects.erase(iter);
			break;
		case RESULT_POOL:
			(*iter)->Set_Dead(false);
			Add_ObjectPool(*iter);
			iter = m_Objects.erase(iter);
			break;
		default:
			iter++;
		}
	}
	return RESULT_NOPROBLEM;
}

void CLayer::PostRender(_double TimeDelta)
{
	for (auto& pGameObject : m_Objects)
	{
		if (pGameObject->Get_ImGuiController())
			pGameObject->PostRender(TimeDelta * m_TimeSpeed);
	}
}

CLayer * CLayer::Create()
{
	CLayer*	pInstance = new CLayer();

	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_Objects)
		Safe_Release(pGameObject);

	for (auto& pGameObject : m_ObjectPool)
		Safe_Release(pGameObject);

	m_Objects.clear();
	m_ObjectPool.clear();
}
