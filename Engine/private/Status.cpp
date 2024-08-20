#include "Status.h"
#include "GameObject.h"

CStatus::CStatus(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CStatus::CStatus(const CStatus& rhs)
	: CComponent(rhs)
{
}

HRESULT CStatus::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CStatus::NativeConstruct(void* pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	memcpy(&m_StatusDesc, pArg, sizeof(STATUSDESC));

	return S_OK;
}

void CStatus::Update(_double TimeDelta)
{
	// 스턴 초기화
	if (m_StatusDesc.bStun == true)
	{
		m_StatusDesc.StunTime += TimeDelta;

		if (m_StatusDesc.StunTime >= m_StatusDesc.MaxStunTime)
		{
			m_StatusDesc.bStun    = false;
			m_StatusDesc.StunTime = 0.0;
		}
	}

	// 슬로우 초기화
	if (m_StatusDesc.SlowSpeed < 1.0)
	{
		m_StatusDesc.SlowSpeed += TimeDelta;

		if (m_StatusDesc.SlowSpeed > 1.0)
			m_StatusDesc.SlowSpeed = 1.0;
	}
}

void CStatus::Set_Stun(_bool bOption, _double StunTime)
{
	if (bOption == true)
	{
		m_StatusDesc.bStun       = true;
		m_StatusDesc.MaxStunTime = StunTime;
		m_pParent->Set_IsAttacking(false);
	}
	else
		m_StatusDesc.bStun = false;
}

_bool CStatus::Heal(_float fHealPercent)
{
	if (fHealPercent <= 0)
		return false;

	_float fHealAmount = m_StatusDesc.fMaxHP * fHealPercent;

	m_StatusDesc.fHP += fHealAmount;

	if (m_StatusDesc.fHP > m_StatusDesc.fMaxHP)
		m_StatusDesc.fHP = m_StatusDesc.fMaxHP;

	return true;
}

_bool CStatus::HealMP(_float fMP)
{
	if (fMP <= 0)
		return false;

	_float fHealAmount = m_StatusDesc.fMaxMP * fMP;

	m_StatusDesc.fMP += fHealAmount;

	if (m_StatusDesc.fMP > m_StatusDesc.fMaxMP)
		m_StatusDesc.fMP = m_StatusDesc.fMaxMP;

	return true;
}

_bool CStatus::Revive()
{
	if (m_pParent->Get_Dead() == false)
		return false;

	m_pParent->Set_Dead(false);
	m_pParent->Set_ReadyToDie(false);
	m_StatusDesc.fHP = m_StatusDesc.fMaxHP;

	return true;
}

_bool CStatus::Damaged(_float fDamage)
{
	if (m_StatusDesc.fHP <= 0.f)
		return false;

	m_StatusDesc.fHP -= fDamage;

	if (m_StatusDesc.fHP <= 0.f)
		m_pParent->Set_ReadyToDie(true);

	return true;
}

_bool CStatus::DamagedMP(_float fDamage)
{
	if (m_StatusDesc.fMP <= 0.f)
		return false;

	m_StatusDesc.fMP -= fDamage;

	if (m_StatusDesc.fMP <= 0.f)
		m_StatusDesc.fMP = 0.f;

	return true;
}

_bool CStatus::ExpUp(_float fEXP)
{
	m_StatusDesc.fEXP += fEXP;

	if (m_StatusDesc.fEXP >= 100.f)
	{
		m_StatusDesc.fEXP = 0.f;
		LevelUp();
	}

	return true;
}

_bool CStatus::LevelUp()
{
	// 레벨업 시 변경할 스테이터스 작성
	m_StatusDesc.fHP  = m_StatusDesc.fMaxHP;

	return true;
}

CStatus* CStatus::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CStatus* pInstance = new CStatus(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CStatus::Clone(void* pArg)
{
	CStatus* pInstance = new CStatus(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatus::Free()
{
	__super::Free();
}