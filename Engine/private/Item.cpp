#include "..\public\Item.h"

CItem::CItem(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CItem::CItem(const CItem & rhs)
	: CGameObject(rhs)
{
}

HRESULT CItem::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CItem::NativeConstruct(void * pArg)
{
	CHECK_FAILED(__super::NativeConstruct(pArg));

	return S_OK;
}

_int CItem::Tick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	return _int();
}

_int CItem::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	return _int();
}

HRESULT CItem::Render()
{
	return S_OK;
}

void CItem::Free()
{
	__super::Free();
}
