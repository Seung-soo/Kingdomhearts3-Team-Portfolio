#include "MonsterCooler.h"

CMonsterCooler::CMonsterCooler(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CMonsterCooler::CMonsterCooler(const CMonsterCooler& rhs)
	: CComponent(rhs)
{
}

HRESULT CMonsterCooler::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonsterCooler::NativeConstruct(void* pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	memcpy(&m_MonsterCoolerDesc, pArg, sizeof(MONSTERCOOLERDESC));

	return S_OK;
}

void CMonsterCooler::Update(_double TimeDelta)
{
	if (m_MonsterCoolerDesc.AttackCool > 0.0)
	{
		m_MonsterCoolerDesc.AttackCool -= TimeDelta;

		if (m_MonsterCoolerDesc.AttackCool <= 0.0)
			m_MonsterCoolerDesc.AttackCool = 0.0;
	}

	if (m_MonsterCoolerDesc.ChaseCool > 0.0)
	{
		m_MonsterCoolerDesc.ChaseCool -= TimeDelta;

		if (m_MonsterCoolerDesc.ChaseCool <= 0.0)
			m_MonsterCoolerDesc.ChaseCool = 0.0;
	}

	if (m_MonsterCoolerDesc.DynamicMoveCool > 0.0)
	{
		m_MonsterCoolerDesc.DynamicMoveCool -= TimeDelta;

		if (m_MonsterCoolerDesc.DynamicMoveCool <= 0.0)
			m_MonsterCoolerDesc.DynamicMoveCool = 0.0;
	}

	if (m_MonsterCoolerDesc.RunAwayCool > 0.0)
	{
		m_MonsterCoolerDesc.RunAwayCool -= TimeDelta;

		if (m_MonsterCoolerDesc.RunAwayCool <= 0.0)
			m_MonsterCoolerDesc.RunAwayCool = 0.0;
	}

	if (m_MonsterCoolerDesc.WaitCool > 0.0)
	{
		m_MonsterCoolerDesc.WaitCool -= TimeDelta;

		if (m_MonsterCoolerDesc.WaitCool <= 0.0)
			m_MonsterCoolerDesc.WaitCool = 0.0;
	}
}

_bool CMonsterCooler::Get_IsStatePossible(COOLTYPE eCoolType)
{
	_bool bResult = false;

	switch (eCoolType)
	{
	case COOLTYPE_ATK:
		bResult = m_MonsterCoolerDesc.AttackCool <= 0.0 ? true : false;
		break;
	case COOLTYPE_CHASE:
		bResult = m_MonsterCoolerDesc.ChaseCool <= 0.0 ? true : false;
		break;
	case COOLTYPE_DYNAMICMOVE:
		bResult = m_MonsterCoolerDesc.DynamicMoveCool <= 0.0 ? true : false;
		break;
	case COOLTYPE_RUNAWAY:
		bResult = m_MonsterCoolerDesc.RunAwayCool <= 0.0 ? true : false;
		break;
	case COOLTYPE_WAIT:
		bResult = m_MonsterCoolerDesc.WaitCool <= 0.0 ? true : false;
		break;
	case COOLTYPE_END:
		break;
	default:
		break;
	}

	return bResult;
}

void CMonsterCooler::Cool_State(COOLTYPE eCoolType)
{
	switch (eCoolType)
	{
	case COOLTYPE_ATK:
		m_MonsterCoolerDesc.AttackCool = m_MonsterCoolerDesc.MaxAttackCool;
		break;
	case COOLTYPE_CHASE:
		m_MonsterCoolerDesc.ChaseCool = m_MonsterCoolerDesc.MaxChaseCool;
		break;
	case COOLTYPE_DYNAMICMOVE:
		m_MonsterCoolerDesc.DynamicMoveCool = m_MonsterCoolerDesc.MaxDynamicMoveCool;
		break;
	case COOLTYPE_RUNAWAY:
		m_MonsterCoolerDesc.RunAwayCool = m_MonsterCoolerDesc.MaxRunAwayCool;
		break;
	case COOLTYPE_WAIT:
		m_MonsterCoolerDesc.WaitCool = m_MonsterCoolerDesc.MaxWaitCool;
		break;
	case COOLTYPE_END:
		break;
	default:
		break;
	}
}

void CMonsterCooler::Reset_Cool(COOLTYPE eCoolType)
{
	switch (eCoolType)
	{
	case COOLTYPE_ATK:
		m_MonsterCoolerDesc.AttackCool = 0.0;
		break;
	case COOLTYPE_CHASE:
		m_MonsterCoolerDesc.ChaseCool = 0.0;
		break;
	case COOLTYPE_DYNAMICMOVE:
		m_MonsterCoolerDesc.DynamicMoveCool = 0.0;
		break;
	case COOLTYPE_RUNAWAY:
		m_MonsterCoolerDesc.RunAwayCool = 0.0;
		break;
	case COOLTYPE_WAIT:
		m_MonsterCoolerDesc.WaitCool = 0.0;
		break;
	case COOLTYPE_END:
		break;
	default:
		break;
	}
}

void CMonsterCooler::Set_Cool(COOLTYPE eCoolType, _double CoolTime)
{
	switch (eCoolType)
	{
	case COOLTYPE_ATK:
		m_MonsterCoolerDesc.AttackCool = CoolTime;
		break;
	case COOLTYPE_CHASE:
		m_MonsterCoolerDesc.ChaseCool = CoolTime;
		break;
	case COOLTYPE_DYNAMICMOVE:
		m_MonsterCoolerDesc.DynamicMoveCool = CoolTime;
		break;
	case COOLTYPE_RUNAWAY:
		m_MonsterCoolerDesc.RunAwayCool = CoolTime;
		break;
	case COOLTYPE_WAIT:
		m_MonsterCoolerDesc.WaitCool = CoolTime;
		break;
	case COOLTYPE_END:
		break;
	default:
		break;
	}
}

CMonsterCooler* CMonsterCooler::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMonsterCooler* pInstance = new CMonsterCooler(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CMonsterCooler::Clone(void* pArg)
{
	CMonsterCooler* pInstance = new CMonsterCooler(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonsterCooler::Free()
{
	__super::Free();
}
