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
	// ���ڷ� ���� pState �� nullptr �̶�� BREAKPOINT
	if (pState == nullptr)
	{
		BREAKPOINT;
		return false;
	}

	// bDeletePrevState �� true ��� ���� ���� ������ ���ְ� ���� ���ε�
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

	// ������ �̹� ���°� �ִٸ� ���ڷ� ���� pState �� Release �ϰ� false ��ȯ
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
	// m_States �� ���°� �ִٸ� true ����
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
	// eType �� �ش��ϴ� pair<string, CState*> �� second �� nullptr �� �ƴ϶��
	if (m_States[eType].second != nullptr)
	{
		// Ŭ���� �� �� ȣ���ϴ� �Լ� ȣ��
		m_States[eType].second->ClearState();

		// �����ְ� first �� �ִ� string �� clear
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

	// STATETYPE ������� ���鼭
	for (_uint i = 0; i < STATETYPE_END; ++i)
	{
		// State* �� nullptr �� �ƴ϶�� Action ����
		if (m_States[i].second != nullptr)
			bIsStateEnd = m_States[i].second->Action(TimeDelta);

		// Action �� �������� true �� ��ȯ�ϴϱ� true �̸� State* ����
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
	// �̱� Ÿ�� ��Ͽ� ���� ������Ʈ�� �Է�����
	if (false == Find_SingleType(eStateType))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// �̱� Ÿ�� ����� ��ȸ�ϸ鼭 �˻�
	for (auto single_type : m_SingleStateTypes)
	{
		// �Է¹��� Ÿ���� ��� continue
		if (single_type == eStateType)
			continue;

		// ��Ͽ��� ������ ���� �Ҵ�Ǿ����� ���� ��� continue
		if (false == FindState(single_type))
			continue;

		// �����ϴ� ������Ʈ ����
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