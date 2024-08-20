#include "stdafx.h"
#include "..\public\Potion.h"
#include "GameInstance.h"

// �ٽ�¥�� ���� �⺻�Լ� �Ȱ����͵���

CPotion::CPotion(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CItem(pDevice,pDeviceContext)
{
}

CPotion::CPotion(const CPotion & rhs)
	:CItem(rhs)
{
}

HRESULT CPotion::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CPotion::ITEM_Potion(void * pArg)
{
	CHECK_FAILED(__super::NativeConstruct(pArg));
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/*
		iPotion = CItem::Item_List::ITEM_POTION
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Potion"), &iPotion )))
		return E_FAIL;

		��𼭵� �̷��� �ҷ����� �۵��ϴ� �������� ����
	*/

	Potion_Cnt = (*(_int*)pArg); // <-iPotion

	switch (Potion_Cnt)
	{
	case Engine::CItem::ITEM_POTION:
		
		/*
		pGameInstance->Get_Back(LEVEL_GAMEPLAY,TEXT("Layer_Player"))
		�ٷ� �÷��̾� ������ �����ؼ� ȸ��
		*/
		
		break;
	case Engine::CItem::ITEM_HIGH_POTION:

		break;
	case Engine::CItem::ITEM_ELIXIR:

		break;
	default:
		break;
	}

	return S_OK;
}

_int CPotion::Tick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	m_bDead = true; // ITEM_Potion���� �ѹ� �� ȣ���ϱ� ������ �ٷ� ����

	return _int();
}

CPotion * CPotion::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPotion* pInstance = new CPotion(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CPotion");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPotion::Clone(void * pArg)
{
	CPotion* pInstance = new CPotion(*this);

	if (FAILED(pInstance->ITEM_Potion(pArg)))
	{
		MSGBOX("Failed to Created CPotion");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPotion::Free()
{
}
