#include "StateMachine.h"
#include "Model.h"

CStateMachine::CStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CStateMachine::CStateMachine(const CStateMachine& rhs)
	: CComponent(rhs)
{
}

HRESULT CStateMachine::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CStateMachine::NativeConstruct(void* pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

_bool CStateMachine::BindState(STATETYPE eType, string StateTag, CState* pState, _bool bDeletePrevState)
{
	// 인자로 들어온 pState 가 nullptr 이라면 BREAKPOINT
	if (pState == nullptr)
	{
		BREAKPOINT;
		return false;
	}

	// bDeletePrevState 가 true 라면 기존 상태 무조건 없애고 상태 바인딩
	if (bDeletePrevState == true)
	{
		if (m_States[eType].second != nullptr)
			m_States[eType].second->ExitState();

		ClearState(eType);
		m_States[eType].first  = StateTag;
		m_States[eType].second = pState;
		Set_Components(pState);
		pState->Set_StateIndex(eType);
		pState->EnterState();

		return true;
	}

	// 기존에 이미 상태가 있다면 인자로 들어온 pState 를 Release 하고 false 반환
	if (m_States[eType].second != nullptr)
	{
		Safe_Release(pState);
		return false;
	}

	if (m_States[eType].second != nullptr)
		m_States[eType].second->ExitState();
	m_States[eType].first  = StateTag;
	m_States[eType].second = pState;
	Set_Components(pState);
	pState->Set_StateIndex(eType);
	pState->EnterState();

	return true;
}

_bool CStateMachine::FindStateTag(STATETYPE eType, string StateTag)
{
	if (m_States[eType].second == nullptr)
		return false;

	if (m_States[eType].first == StateTag)
		return true;

	return false;
}

_bool CStateMachine::FindStateTag(string StateTag)
{
	for (auto pair : m_States)
	{
		if (pair.first == StateTag)
			return true;
	}

	return false;
}

_bool CStateMachine::FindState(STATETYPE eType)
{
	// m_States 에 상태가 있다면 true 리턴
	if (m_States[eType].second != nullptr)
		return true;

	return false;
}

_bool CStateMachine::FindMonsterState()
{
	if (m_States[STATETYPE_MONSTERSTATE].second != nullptr)
		return true;

	return false;
}

HRESULT CStateMachine::ClearState(STATETYPE eType)
{
	// eType 에 해당하는 pair<string, CState*> 의 second 가 nullptr 이 아니라면
	if (m_States[eType].second != nullptr)
	{
		// 클리어 될 때 호출하는 함수 호출
		m_States[eType].second->ClearState();

		// 지워주고 first 에 있는 string 도 clear
		Safe_Release(m_States[eType].second);
		m_States[eType].first.clear();
	}

	return S_OK;
}

CState * CStateMachine::Get_State(STATETYPE eType)
{
	if (nullptr == m_States[eType].second)
		return nullptr;

	return m_States[eType].second;
}

string CStateMachine::Get_StateName(STATETYPE eType)
{
	if (nullptr != m_States[eType].second)
		return m_States[eType].first;

	return string("");
}

_uint CStateMachine::Get_NumberOfStates()
{
	_uint number = 0;
	for(_uint i = 0; i < STATETYPE_END; ++i)
	{
		if (true == FindState((STATETYPE)i))
			number++;
	}

	return number;
}

_bool CStateMachine::Get_ExistNextState(STATETYPE eType)
{
	for (_int i = eType + 1; i < STATETYPE_END; ++i)
	{
		if (nullptr == m_States[i].second)
			continue;
		else
			return true;
	}

	return false;
}

HRESULT CStateMachine::Action(_double TimeDelta)
{
	_bool bIsStateEnd = false;

	// STATETYPE 순서대로 돌면서
	for (_uint i = 0; i < STATETYPE_END; ++i)
	{
		// State* 가 nullptr 이 아니라면 Action 실행
		if (m_States[i].second != nullptr)
			bIsStateEnd = m_States[i].second->Action(TimeDelta);

		// Action 이 끝났으면 true 를 반환하니까 true 이면 State* 삭제
		if (bIsStateEnd == true)
			ClearState(STATETYPE(i));
	}

	return S_OK;
}

_bool CStateMachine::Add_AnimIndex(_uint iType, _uint iAnimationIndex)
{
	for (auto anim_index : m_StateAnimations[iType])
	{
		if (anim_index == iAnimationIndex)
			return false;
	}

	m_StateAnimations[iType].push_back(iAnimationIndex);
	return true;
}

_bool CStateMachine::Rmv_AnimIndex(_uint iType, _uint iAnimationIndex)
{
	auto iter = m_StateAnimations[iType].begin();

	for (auto anim_index : m_StateAnimations[iType])
	{
		if (anim_index == iAnimationIndex)
		{
			m_StateAnimations[iType].erase(iter);
			return true;
		}
		else
			iter++;
	}

	return false;
}

_bool CStateMachine::Find_AnimIndex(_uint iType, _uint iAnimationIndex)
{
	for (auto anim_index : m_StateAnimations[iType])
	{
		if (anim_index == iAnimationIndex)
			return true;
	}

	return false;
}

_bool CStateMachine::Add_SingleType(STATETYPE eStateType)
{
	for (auto single_type : m_SingleStateTypes)
	{
		if (single_type == eStateType)
			return false;
	}

	m_SingleStateTypes.push_back(eStateType);
	return true;
}

_bool CStateMachine::Rmv_SingleType(STATETYPE eStateType)
{
	auto iter = m_SingleStateTypes.begin();

	for (auto single_type : m_SingleStateTypes)
	{
		if (single_type == eStateType)
		{
			m_SingleStateTypes.erase(iter);
			return true;
		}
		else
			iter++;
	}

	return false;
}

_bool CStateMachine::Find_SingleType(STATETYPE eStateType)
{
	for (auto single_type : m_SingleStateTypes)
	{
		if (single_type == eStateType)
			return true;
	}

	return false;
}

HRESULT CStateMachine::Clear_CrashTypes(STATETYPE eStateType, _uint* pOut_NumberOfDeleted)
{
	// 싱글 타입 목록에 없는 스테이트를 입력했음
	if (false == Find_SingleType(eStateType))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 싱글 타입 목록을 순회하면서 검사
	for (auto single_type : m_SingleStateTypes)
	{
		// 입력받은 타입일 경우 continue
		if (single_type == eStateType)
			continue;

		// 목록에는 있지만 실제 할당되어있지 않을 경우 continue
		if (false == FindState(single_type))
			continue;

		// 존재하는 스테이트 삭제
		ClearState(single_type);

		if (nullptr != pOut_NumberOfDeleted)
			(*pOut_NumberOfDeleted)++;
	}

	return S_OK;
}

HRESULT CStateMachine::Set_Components(CState * pState)
{
	CTransform* transform = m_pParent->Get_Transform();
	CModel* model = static_cast<CModel*>(m_pParent->Get_Component(TEXT("Com_Model")));

	if (nullptr == transform ||
		nullptr == model)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	pState->Set_Components(m_pParent, transform, model, this);

	return S_OK;
}

CStateMachine* CStateMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CStateMachine* pInstance = new CStateMachine(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CStateMachine::Clone(void* pArg)
{
	CStateMachine* pInstance = new CStateMachine(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateMachine::Free()
{
	__super::Free();

	for (_uint i = 0; i < STATETYPE_END; ++i)
	{
		Safe_Release(m_States[i].second);
		m_States[i].first.clear();
	}
}