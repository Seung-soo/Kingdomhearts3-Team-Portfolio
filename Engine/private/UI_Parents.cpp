#include "..\public\UI_Parents.h"

CUI_Parents::CUI_Parents(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CUI_Parents::CUI_Parents(const CUI_Parents & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Parents::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_Parents::NativeConstruct(void * pArg)
{
	CHECK_FAILED(__super::NativeConstruct(pArg));

	return S_OK;
}

_int CUI_Parents::Tick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	return _int();
}

_int CUI_Parents::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	return _int();
}

HRESULT CUI_Parents::Render()
{
	return S_OK;
}

void CUI_Parents::Free()
{
	__super::Free();
}
