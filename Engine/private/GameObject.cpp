#include "..\public\GameObject.h"
#include "Component.h"
#include "GameInstance.h"

const _tchar* CGameObject::m_pTransformTag = TEXT("Com_Transform");

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice), m_pDeviceContext(pDeviceContext), m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice), m_pDeviceContext(rhs.m_pDeviceContext)
	, m_isCloned(true), m_pGameInstance(rhs.m_pGameInstance)
{
	Safe_AddRef(m_pDevice); 
	Safe_AddRef(m_pDeviceContext); 
}

CComponent * CGameObject::Get_Component(const _tchar * pComponentTag)
{
	return Find_Component(pComponentTag);
}

CTransform * CGameObject::Get_Transform()
{
	return m_pTransformCom;
}

_float CGameObject::Get_AlphaDepth()
{
	_vector vWorldPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_matrix ViewMatrix = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	_vector vLocalPos = XMVector3TransformCoord(vWorldPos, ViewMatrix);

	return vecz(vLocalPos);
}

void CGameObject::StartDissolve(_float fDissolveSpeed)
{
	m_bDissolve      = true;
	m_iPass          = 4;
	m_fDissolveSpeed = fDissolveSpeed;
}

HRESULT CGameObject::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::NativeConstruct(void * pArg)
{
	m_pTransformCom = CTransform::Create(m_pDevice, m_pDeviceContext);
	m_pTransformCom->Set_Parent(this);
	if (nullptr == m_pTransformCom)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (nullptr != pArg)
		m_pTransformCom->Set_TransformDesc(*(CTransform::TRANSFORMDESC*)pArg);

	if (nullptr != Find_Component(m_pTransformTag))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_Components.emplace(m_pTransformTag, m_pTransformCom);
	Safe_AddRef(m_pTransformCom);
	return S_OK;
}

HRESULT CGameObject::NativeConstruct_Pool(void * pArg)
{
	return S_OK;
}

_int CGameObject::Tick(_double dTimeDelta)
{
	if (true == m_bFirstTick && 0 < m_iTickCount)
		m_bFirstTick = false;

	m_iTickCount++;

	if (true == m_bDissolve)
	{
		m_fDissolveAmount += (_float)dTimeDelta * m_fDissolveSpeed; 
	}

	return RESULT_NOPROBLEM;
}

_int CGameObject::LateTick(_double dTimeDelta)
{
	return RESULT_NOPROBLEM;
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

void CGameObject::PostRender(_double TimeDelta)
{
}

HRESULT CGameObject::Render_Shadow()
{
	return S_OK;
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pComponentTag, CComponent ** pOut, void * pArg)
{
	if (nullptr != Find_Component(pComponentTag))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CComponent* pComponent = pGameInstance->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	pComponent->Set_Parent(this);

	m_Components.emplace(pComponentTag, pComponent);

	*pOut = pComponent;

	Safe_AddRef(pComponent);

	return S_OK;
}

// 이 게임 오브젝트가 해당 컴포넌트를 들고 있는지 검사
CComponent * CGameObject::Find_Component(const _tchar * pComponentTag)
{
	auto iter = find_if(m_Components.begin(), m_Components.end(), CTagFinder(pComponentTag));
	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Free()
{
	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

	if (m_pDissolveTexture != nullptr)
		Safe_Release(m_pDissolveTexture);
}
