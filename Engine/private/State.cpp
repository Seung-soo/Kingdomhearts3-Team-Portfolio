#include "State.h"

HRESULT CState::Set_Components(CGameObject* pGameObject, CTransform * pTransform, CModel * pModel, CStateMachine * pStateMachine)
{
	if (nullptr == pModel || 
		nullptr == pGameObject||
		nullptr == pTransform ||
		nullptr == pStateMachine)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_pGameObject   = pGameObject;
	m_pModelCom     = pModel;
	m_pTransformCom = pTransform;
	m_pStateMachine = pStateMachine;

	return S_OK;
}