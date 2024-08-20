#include "stdafx.h"
#include "..\public\InvenManager.h"
#include "GameInstance.h"

READY_SINGLETON(CInvenManager)

CInvenManager::CInvenManager()
{
}

void CInvenManager::Item_QuickSlot1_Set(_int _Name) // 실시간으로?
{
	if (SLOT_POTION == _Name
		|| SLOT_HIGH_POTION == _Name
		|| SLOT_ELIXIR == _Name
		|| SLOT_ATKUP == _Name
		|| SLOT_ATKSPEEDUP == _Name
		|| SLOT_SPEEDUP == _Name
		|| SLOT_EMPTY == _Name)
	{
		iQuickSlot1 = QickSlot_Get(_Name);/* 여기에 _Name의 아이템 갯수를 담기*/
		iQuickSlot1_Name = _Name;
	}
	else
	{
		/* 빈 슬롯 */

	}
}
 
void CInvenManager::Item_QuickSlot2_Set(_int _Name)
{
	if (SLOT_POTION == _Name
		|| SLOT_HIGH_POTION == _Name
		|| SLOT_ELIXIR == _Name
		|| SLOT_ATKUP == _Name
		|| SLOT_ATKSPEEDUP == _Name
		|| SLOT_SPEEDUP == _Name
		|| SLOT_EMPTY == _Name)
	{
		iQuickSlot2 = QickSlot_Get(_Name);/* 여기에 _Name의 아이템 갯수를 담기*/
		iQuickSlot2_Name = _Name;
	}
	else
	{
		/* 빈 슬롯 */

	}
}

void CInvenManager::Item_QuickSlot3_Set(_int _Name)
{
	if (SLOT_POTION == _Name
		|| SLOT_HIGH_POTION == _Name
		|| SLOT_ELIXIR == _Name
		|| SLOT_ATKUP == _Name
		|| SLOT_ATKSPEEDUP == _Name
		|| SLOT_SPEEDUP == _Name
		|| SLOT_EMPTY == _Name)
	{
		iQuickSlot3 = QickSlot_Get(_Name);/* 여기에 _Name의 아이템 갯수를 담기*/
		iQuickSlot3_Name = _Name;
	}
	else
	{
		/* 빈 슬롯 */

	}
}

void CInvenManager::Item_InvenSlot1_Set(_int _Name)
{
	if (SLOT_POTION == _Name
		|| SLOT_HIGH_POTION == _Name
		|| SLOT_ELIXIR == _Name
		|| SLOT_ATKUP == _Name
		|| SLOT_ATKSPEEDUP == _Name
		|| SLOT_SPEEDUP == _Name
		|| SLOT_EMPTY == _Name)
	{
		iInvenSlot1 = InvenSlot_Get(_Name);/* 여기에 _Name의 아이템 갯수를 담기*/
		iInvenSlot1_Name = _Name;
	}
	else
	{
		/* 빈 슬롯 */

	}
}

void CInvenManager::Item_InvenSlot2_Set(_int _Name)
{
	if (SLOT_POTION == _Name
		|| SLOT_HIGH_POTION == _Name
		|| SLOT_ELIXIR == _Name
		|| SLOT_ATKUP == _Name
		|| SLOT_ATKSPEEDUP == _Name
		|| SLOT_SPEEDUP == _Name
		|| SLOT_EMPTY == _Name)
	{
		iInvenSlot2 = InvenSlot_Get(_Name);/* 여기에 _Name의 아이템 갯수를 담기*/
		iInvenSlot2_Name = _Name;
	}
	else
	{
		/* 빈 슬롯 */

	}
}

void CInvenManager::Item_InvenSlot3_Set(_int _Name)
{
	if (SLOT_POTION == _Name
		|| SLOT_HIGH_POTION == _Name
		|| SLOT_ELIXIR == _Name
		|| SLOT_ATKUP == _Name
		|| SLOT_ATKSPEEDUP == _Name
		|| SLOT_SPEEDUP == _Name
		|| SLOT_EMPTY == _Name)
	{
		iInvenSlot3 = InvenSlot_Get(_Name);/* 여기에 _Name의 아이템 갯수를 담기*/
		iInvenSlot3_Name = _Name;
	}
	else
	{
		/* 빈 슬롯 */

	}
}

void CInvenManager::Item_InvenSlot4_Set(_int _Name)
{
	if (SLOT_POTION == _Name
		|| SLOT_HIGH_POTION == _Name
		|| SLOT_ELIXIR == _Name
		|| SLOT_ATKUP == _Name
		|| SLOT_ATKSPEEDUP == _Name
		|| SLOT_SPEEDUP == _Name
		|| SLOT_EMPTY == _Name)
	{
		iInvenSlot4 = InvenSlot_Get(_Name);/* 여기에 _Name의 아이템 갯수를 담기*/
		iInvenSlot4_Name = _Name;
	}
	else
	{
		/* 빈 슬롯 */

	}
}

void CInvenManager::Item_InvenSlot5_Set(_int _Name)
{
	if (SLOT_POTION == _Name
		|| SLOT_HIGH_POTION == _Name
		|| SLOT_ELIXIR == _Name
		|| SLOT_ATKUP == _Name
		|| SLOT_ATKSPEEDUP == _Name
		|| SLOT_SPEEDUP == _Name
		|| SLOT_EMPTY == _Name)
	{
		iInvenSlot5 = InvenSlot_Get(_Name);/* 여기에 _Name의 아이템 갯수를 담기*/
		iInvenSlot5_Name = _Name;
	}
	else
	{
		/* 빈 슬롯 */

	}
}

void CInvenManager::Item_InvenSlot6_Set(_int _Name)
{
	if (SLOT_POTION == _Name
		|| SLOT_HIGH_POTION == _Name
		|| SLOT_ELIXIR == _Name
		|| SLOT_ATKUP == _Name
		|| SLOT_ATKSPEEDUP == _Name
		|| SLOT_SPEEDUP == _Name
		|| SLOT_EMPTY == _Name)
	{
		iInvenSlot6 = InvenSlot_Get(_Name);/* 여기에 _Name의 아이템 갯수를 담기*/
		iInvenSlot6_Name = _Name;
	}
	else
	{
		/* 빈 슬롯 */

	}
}



_int CInvenManager::InvenSlot_Get(_int _Name)
{
	_int	Get_Item = 0;

	switch (_Name)
	{
	case SLOT_POTION:
		Get_Item = iPotionCnt;
		break;
	case SLOT_HIGH_POTION:
		Get_Item = iHigh_PotionCnt;
		break;
	case SLOT_ELIXIR:
		Get_Item = iElixirCnt;
		break;
	case SLOT_ATKUP:
		Get_Item = iAtkUpCnt;
		break;
	case SLOT_ATKSPEEDUP:
		Get_Item = iAtkSpeedUpCnt;
		break;
	case SLOT_SPEEDUP:
		Get_Item = iSpeedUpCnt;
		break;
	case SLOT_EMPTY:
		Get_Item = iEmpty;
		break;
	default:
		break;
	}

	return Get_Item;
}

_int CInvenManager::QickSlot_Get(_int _Name)
{
	_int	Get_Item = 0;
	switch (_Name)
	{
	case SLOT_POTION:
		Get_Item = iPotionCnt;
		break;
	case SLOT_HIGH_POTION:
		Get_Item = iHigh_PotionCnt;
		break;
	case SLOT_ELIXIR:
		Get_Item = iElixirCnt;
		break;
	case SLOT_ATKUP:
		Get_Item = iAtkUpCnt;
		break;
	case SLOT_ATKSPEEDUP:
		Get_Item = iAtkSpeedUpCnt;
		break;
	case SLOT_SPEEDUP:
		Get_Item = iSpeedUpCnt;
		break;
	case SLOT_EMPTY:
		Get_Item = iEmpty;
		break;
	default:
		break;
	}

	return Get_Item;
}

void CInvenManager::Inven_sort(_int _Name) /* 인벤토리 정렬 1 (획득순서) */
{
	

	/* ============ */

	if (SLOT_EMPTY == iInvenSlot1_Name
		|| iInvenSlot1_Name == _Name)
	{
		iInvenSlot1_Name = _Name;
		iInvenSlot1 = InvenSlot_Get(_Name);/* 여기에 _Name의 아이템 갯수를 담기*/
	}
	else if (SLOT_EMPTY == iInvenSlot2_Name
		|| iInvenSlot2_Name == _Name)
	{
		iInvenSlot2_Name = _Name;
		iInvenSlot2 = InvenSlot_Get(_Name);/* 여기에 _Name의 아이템 갯수를 담기*/
	}
	else if (SLOT_EMPTY == iInvenSlot3_Name
		|| iInvenSlot3_Name == _Name)
	{
		iInvenSlot3_Name = _Name;
		iInvenSlot3 = InvenSlot_Get(_Name);/* 여기에 _Name의 아이템 갯수를 담기*/
	}
	else if (SLOT_EMPTY == iInvenSlot4_Name
		|| iInvenSlot4_Name == _Name)
	{
		iInvenSlot4_Name = _Name;
		iInvenSlot4 = InvenSlot_Get(_Name);/* 여기에 _Name의 아이템 갯수를 담기*/
	}
	else if (SLOT_EMPTY == iInvenSlot5_Name
		|| iInvenSlot5_Name == _Name)
	{
		iInvenSlot5_Name = _Name;
		iInvenSlot5 = InvenSlot_Get(_Name);/* 여기에 _Name의 아이템 갯수를 담기*/
	}
	else if (SLOT_EMPTY == iInvenSlot6_Name
		|| iInvenSlot6_Name == _Name)
	{
		iInvenSlot6_Name = _Name;
		iInvenSlot6 = InvenSlot_Get(_Name);/* 여기에 _Name의 아이템 갯수를 담기*/
	}
	else
	{
		
	}

	if (0 >= iInvenSlot1)
	{
		iInvenSlot1_Name = SLOT_EMPTY;
		iInvenSlot1 = 0;
	}
	else if (0 >= iInvenSlot2)
	{
		iInvenSlot2_Name = SLOT_EMPTY;
		iInvenSlot2 = 0;
	}
	else if (0 >= iInvenSlot3)
	{
		iInvenSlot3_Name = SLOT_EMPTY;
		iInvenSlot3 = 0;
	}
	else if (0 >= iInvenSlot4)
	{
		iInvenSlot4_Name = SLOT_EMPTY;
		iInvenSlot4 = 0;
	}
	else if (0 >= iInvenSlot5)
	{
		iInvenSlot5_Name = SLOT_EMPTY;
		iInvenSlot5 = 0;
	}
	else if (0 >= iInvenSlot6)
	{
		iInvenSlot6_Name = SLOT_EMPTY;
		iInvenSlot6 = 0;
	}

	if (SLOT_POTION == iInvenSlot1_Name)
	{
		iInvenSlot1 = InvenSlot_Get(SLOT_POTION);
	}
	else if (SLOT_HIGH_POTION == iInvenSlot1_Name)
	{
		iInvenSlot1 = InvenSlot_Get(SLOT_HIGH_POTION);
	}
	else if (SLOT_ELIXIR == iInvenSlot1_Name)
	{
		iInvenSlot1 = InvenSlot_Get(SLOT_ELIXIR);
	}
	else if (SLOT_ATKUP == iInvenSlot1_Name)
	{
		iInvenSlot1 = InvenSlot_Get(SLOT_ATKUP);
	}
	else if (SLOT_ATKSPEEDUP == iInvenSlot1_Name)
	{
		iInvenSlot1 = InvenSlot_Get(SLOT_ATKSPEEDUP);
	}
	else if (SLOT_SPEEDUP == iInvenSlot1_Name)
	{
		iInvenSlot1 = InvenSlot_Get(SLOT_SPEEDUP);
	}

	/* === */

	if (SLOT_POTION == iInvenSlot2_Name)
	{
		iInvenSlot2 = InvenSlot_Get(SLOT_POTION);
	}
	else if (SLOT_HIGH_POTION == iInvenSlot2_Name)
	{
		iInvenSlot2 = InvenSlot_Get(SLOT_HIGH_POTION);
	}
	else if (SLOT_ELIXIR == iInvenSlot2_Name)
	{
		iInvenSlot2 = InvenSlot_Get(SLOT_ELIXIR);
	}
	else if (SLOT_ATKUP == iInvenSlot2_Name)
	{
		iInvenSlot2 = InvenSlot_Get(SLOT_ATKUP);
	}
	else if (SLOT_ATKSPEEDUP == iInvenSlot2_Name)
	{
		iInvenSlot2 = InvenSlot_Get(SLOT_ATKSPEEDUP);
	}
	else if (SLOT_SPEEDUP == iInvenSlot2_Name)
	{
		iInvenSlot2 = InvenSlot_Get(SLOT_SPEEDUP);
	}

	/* === */

	if (SLOT_POTION == iInvenSlot3_Name)
	{
		iInvenSlot3 = InvenSlot_Get(SLOT_POTION);
	}
	else if (SLOT_HIGH_POTION == iInvenSlot3_Name)
	{
		iInvenSlot3 = InvenSlot_Get(SLOT_HIGH_POTION);
	}
	else if (SLOT_ELIXIR == iInvenSlot3_Name)
	{
		iInvenSlot3 = InvenSlot_Get(SLOT_ELIXIR);
	}
	else if (SLOT_ATKUP == iInvenSlot3_Name)
	{
		iInvenSlot3 = InvenSlot_Get(SLOT_ATKUP);
	}
	else if (SLOT_ATKSPEEDUP == iInvenSlot3_Name)
	{
		iInvenSlot3 = InvenSlot_Get(SLOT_ATKSPEEDUP);
	}
	else if (SLOT_SPEEDUP == iInvenSlot3_Name)
	{
		iInvenSlot3 = InvenSlot_Get(SLOT_SPEEDUP);
	}

	/* === */

	if (SLOT_POTION == iInvenSlot4_Name)
	{
		iInvenSlot4 = InvenSlot_Get(SLOT_POTION);
	}
	else if (SLOT_HIGH_POTION == iInvenSlot4_Name)
	{
		iInvenSlot4 = InvenSlot_Get(SLOT_HIGH_POTION);
	}
	else if (SLOT_ELIXIR == iInvenSlot4_Name)
	{
		iInvenSlot4 = InvenSlot_Get(SLOT_ELIXIR);
	}
	else if (SLOT_ATKUP == iInvenSlot4_Name)
	{
		iInvenSlot4 = InvenSlot_Get(SLOT_ATKUP);
	}
	else if (SLOT_ATKSPEEDUP == iInvenSlot4_Name)
	{
		iInvenSlot4 = InvenSlot_Get(SLOT_ATKSPEEDUP);
	}
	else if (SLOT_SPEEDUP == iInvenSlot4_Name)
	{
		iInvenSlot4 = InvenSlot_Get(SLOT_SPEEDUP);
	}

	/* === */

	if (SLOT_POTION == iInvenSlot5_Name)
	{
		iInvenSlot5 = InvenSlot_Get(SLOT_POTION);
	}
	else if (SLOT_HIGH_POTION == iInvenSlot5_Name)
	{
		iInvenSlot5 = InvenSlot_Get(SLOT_HIGH_POTION);
	}
	else if (SLOT_ELIXIR == iInvenSlot5_Name)
	{
		iInvenSlot5 = InvenSlot_Get(SLOT_ELIXIR);
	}
	else if (SLOT_ATKUP == iInvenSlot5_Name)
	{
		iInvenSlot5 = InvenSlot_Get(SLOT_ATKUP);
	}
	else if (SLOT_ATKSPEEDUP == iInvenSlot5_Name)
	{
		iInvenSlot5 = InvenSlot_Get(SLOT_ATKSPEEDUP);
	}
	else if (SLOT_SPEEDUP == iInvenSlot5_Name)
	{
		iInvenSlot5 = InvenSlot_Get(SLOT_SPEEDUP);
	}

	/* === */

	if (SLOT_POTION == iInvenSlot6_Name)
	{
		iInvenSlot6 = InvenSlot_Get(SLOT_POTION);
	}
	else if (SLOT_HIGH_POTION == iInvenSlot6_Name)
	{
		iInvenSlot6 = InvenSlot_Get(SLOT_HIGH_POTION);
	}
	else if (SLOT_ELIXIR == iInvenSlot6_Name)
	{
		iInvenSlot6 = InvenSlot_Get(SLOT_ELIXIR);
	}
	else if (SLOT_ATKUP == iInvenSlot6_Name)
	{
		iInvenSlot6 = InvenSlot_Get(SLOT_ATKUP);
	}
	else if (SLOT_ATKSPEEDUP == iInvenSlot6_Name)
	{
		iInvenSlot6 = InvenSlot_Get(SLOT_ATKSPEEDUP);
	}
	else if (SLOT_SPEEDUP == iInvenSlot6_Name)
	{
		iInvenSlot6 = InvenSlot_Get(SLOT_SPEEDUP);
	}


	Inven_sortSub();
}

/* Inven 빈공간 자동정렬 (Inven_Sort 돌리기 전 or 인벤토리 킬때) */
void CInvenManager::Inven_sortSub() 
{
	if (0 >= iInvenSlot1 && SLOT_EMPTY != iInvenSlot2_Name)
	{
		iInvenSlot1 = iInvenSlot2;
		iInvenSlot1_Name = iInvenSlot2_Name;

		iInvenSlot2 = 0;
		iInvenSlot2_Name = SLOT_EMPTY;
	}

	if (0 >= iInvenSlot2 && SLOT_EMPTY != iInvenSlot3_Name)
	{
		iInvenSlot2 = iInvenSlot3;
		iInvenSlot2_Name = iInvenSlot3_Name;

		iInvenSlot3 = 0;
		iInvenSlot3_Name = SLOT_EMPTY;
	}

	if (0 >= iInvenSlot3 && SLOT_EMPTY != iInvenSlot4_Name)
	{
		iInvenSlot3 = iInvenSlot4;
		iInvenSlot3_Name = iInvenSlot4_Name;

		iInvenSlot4 = 0;
		iInvenSlot4_Name = SLOT_EMPTY;
	}

	if (0 >= iInvenSlot4 && SLOT_EMPTY != iInvenSlot5_Name)
	{
		iInvenSlot4 = iInvenSlot5;
		iInvenSlot4_Name = iInvenSlot5_Name;

		iInvenSlot5 = 0;
		iInvenSlot5_Name = SLOT_EMPTY;
	}

	if (0 >= iInvenSlot5 && SLOT_EMPTY != iInvenSlot6_Name)
	{
		iInvenSlot5 = iInvenSlot6;
		iInvenSlot5_Name = iInvenSlot6_Name;

		iInvenSlot6 = 0;
		iInvenSlot6_Name = SLOT_EMPTY;
	}

	if (0 >= iInvenSlot6)
	{
		iInvenSlot6 = 0;
		iInvenSlot6_Name = SLOT_EMPTY;
	}

}

void CInvenManager::Inven_Object_Zero()
{
	if (iQuickSlot1_Name == SLOT_POTION
		|| iQuickSlot1_Name == SLOT_HIGH_POTION
		|| iQuickSlot1_Name == SLOT_ELIXIR
		|| iQuickSlot1_Name == SLOT_ATKUP
		|| iQuickSlot1_Name == SLOT_ATKSPEEDUP
		|| iQuickSlot1_Name == SLOT_SPEEDUP)
	{
		if (0 >= InvenSlot_Get(iQuickSlot1_Name))
		{
			Item_QuickSlot1_Set(SLOT_EMPTY);
		}
	}

	if (iQuickSlot2_Name == SLOT_POTION
		|| iQuickSlot2_Name == SLOT_HIGH_POTION
		|| iQuickSlot2_Name == SLOT_ELIXIR
		|| iQuickSlot2_Name == SLOT_ATKUP
		|| iQuickSlot2_Name == SLOT_ATKSPEEDUP
		|| iQuickSlot2_Name == SLOT_SPEEDUP)
	{
		if (0 >= InvenSlot_Get(iQuickSlot2_Name))
		{
			Item_QuickSlot2_Set(SLOT_EMPTY);
		}
	}

	if (iQuickSlot3_Name == SLOT_POTION
		|| iQuickSlot3_Name == SLOT_HIGH_POTION
		|| iQuickSlot3_Name == SLOT_ELIXIR
		|| iQuickSlot3_Name == SLOT_ATKUP
		|| iQuickSlot3_Name == SLOT_ATKSPEEDUP
		|| iQuickSlot3_Name == SLOT_SPEEDUP)
	{
		if (0 >= InvenSlot_Get(iQuickSlot3_Name))
		{
			Item_QuickSlot3_Set(SLOT_EMPTY);
		}
	}
}

void CInvenManager::Name_Math(_int _Name) /* Inven_sort 기존 Item을 더하는 함수 */
{
	switch (_Name)
	{
	case SLOT_POTION:
		iPotionCnt += 1;
		break;
	case SLOT_HIGH_POTION:
		iHigh_PotionCnt += 1;
		break;
	case SLOT_ELIXIR:
		iElixirCnt += 1;
		break;
	case SLOT_ATKUP:
		iAtkUpCnt += 1;
		break;
	case SLOT_ATKSPEEDUP:
		iAtkSpeedUpCnt += 1;
		break;
	case SLOT_SPEEDUP:
		iSpeedUpCnt += 1;
		break;
	default:
		break;
	}
}

void CInvenManager::Free()
{
}
