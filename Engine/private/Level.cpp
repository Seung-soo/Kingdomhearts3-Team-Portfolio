#include "..\public\Level.h"
#include "GameInstance.h"

CLevel::CLevel(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice), m_pDeviceContext(pDeviceContext), m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CLevel::NativeConstruct()
{
	return S_OK;
}

_int CLevel::Tick(_double dTimeDelta)
{
	return _int();
}

_int CLevel::LateTick(_double dTimeDelta)
{
	return _int();
}

HRESULT CLevel::Render()
{
	return S_OK;
}

void CLevel::Free()
{
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}