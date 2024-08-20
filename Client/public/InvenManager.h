#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

#define	SLOT_POTION			100
#define	SLOT_HIGH_POTION	101
#define SLOT_ELIXIR			102
#define SLOT_ATKUP			103
#define	SLOT_ATKSPEEDUP		104
#define SLOT_SPEEDUP		105

#define SLOT_EMPTY			111

class CInvenManager final : public CBase
{
	MAKE_SINGLETON(CInvenManager)
public:
	CInvenManager();
	virtual ~CInvenManager() = default;

public:
	_int	Get_Slot1() { return iQuickSlot1; }
	_int	Get_Slot2() { return iQuickSlot2; }
	_int	Get_Slot3() { return iQuickSlot3; }

	void	Set_Slot1(_int _Cnt) { iQuickSlot1 = _Cnt; }
	void	Set_Slot2(_int _Cnt) { iQuickSlot2 = _Cnt; }
	void	Set_Slot3(_int _Cnt) { iQuickSlot3 = _Cnt; }

	_int	Get_Slot1_Name() { return iQuickSlot1_Name; }
	_int	Get_Slot2_Name() { return iQuickSlot2_Name; }
	_int	Get_Slot3_Name() { return iQuickSlot3_Name; }

	void	Set_Slot1_Name(_int _Name) { iQuickSlot1_Name = _Name; }
	void	Set_Slot2_Name(_int _Name) { iQuickSlot2_Name = _Name; }
	void	Set_Slot3_Name(_int _Name) { iQuickSlot3_Name = _Name; }
	
	/* ========= */

	_int	Get_InvenSlot1() { return iInvenSlot1; }
	_int	Get_InvenSlot2() { return iInvenSlot2; }
	_int	Get_InvenSlot3() { return iInvenSlot3; }
	_int	Get_InvenSlot4() { return iInvenSlot4; }
	_int	Get_InvenSlot5() { return iInvenSlot5; }
	_int	Get_InvenSlot6() { return iInvenSlot6; }

	void	Set_InvenSlot1(_int _Cnt) { iInvenSlot1 = _Cnt; }
	void	Set_InvenSlot2(_int _Cnt) { iInvenSlot2 = _Cnt; }
	void	Set_InvenSlot3(_int _Cnt) { iInvenSlot3 = _Cnt; }
	void	Set_InvenSlot4(_int _Cnt) { iInvenSlot4 = _Cnt; }
	void	Set_InvenSlot5(_int _Cnt) { iInvenSlot5 = _Cnt; }
	void	Set_InvenSlot6(_int _Cnt) { iInvenSlot6 = _Cnt; }

	_int	Get_InvenSlot1_Name() { return iInvenSlot1_Name; }
	_int	Get_InvenSlot2_Name() { return iInvenSlot2_Name; }
	_int	Get_InvenSlot3_Name() { return iInvenSlot3_Name; }
	_int	Get_InvenSlot4_Name() { return iInvenSlot4_Name; }
	_int	Get_InvenSlot5_Name() { return iInvenSlot5_Name; }
	_int	Get_InvenSlot6_Name() { return iInvenSlot6_Name; }

	void	Set_InvenSlot1_Name(_int _Cnt) { iInvenSlot1_Name = _Cnt; }
	void	Set_InvenSlot2_Name(_int _Cnt) { iInvenSlot2_Name = _Cnt; }
	void	Set_InvenSlot3_Name(_int _Cnt) { iInvenSlot3_Name = _Cnt; }
	void	Set_InvenSlot4_Name(_int _Cnt) { iInvenSlot4_Name = _Cnt; }
	void	Set_InvenSlot5_Name(_int _Cnt) { iInvenSlot5_Name = _Cnt; }
	void	Set_InvenSlot6_Name(_int _Cnt) { iInvenSlot6_Name = _Cnt; }

	/* 슬롯 등록 함수 : _Name에 아래 define 담기 , 매 Tick 돌려야함 */
	void	Item_QuickSlot1_Set(_int _Name); 
	void	Item_QuickSlot2_Set(_int _Name);
	void	Item_QuickSlot3_Set(_int _Name);

	void	Item_InvenSlot1_Set(_int _Name);
	void	Item_InvenSlot2_Set(_int _Name);
	void	Item_InvenSlot3_Set(_int _Name);
	void	Item_InvenSlot4_Set(_int _Name);
	void	Item_InvenSlot5_Set(_int _Name);
	void	Item_InvenSlot6_Set(_int _Name);

	/* UI_QiuckItemText */
	_bool	Get_QuickItem_Text() { return m_bQuickItemText; }
	void	Set_QuickItem_Text(_bool _Render) { m_bQuickItemText = _Render; }

	_int	Get_QuickItem_Cnt() { return m_iQuickItem_Num; }
	void	Set_QuickItem_Cnt(_int _Cnt) { m_iQuickItem_Num = _Cnt; }


	/*===*/
	/*===*/

	_int	Get_Potion() { return iPotionCnt; }
	void	Set_Potion(_int _Cnt) { iPotionCnt = _Cnt; }
	void	Math_Potion(_int _Cnt) { iPotionCnt += _Cnt; };

	_int	Get_High_Potion() { return iHigh_PotionCnt; }
	void	Set_High_Potion(_int _Cnt) { iHigh_PotionCnt = _Cnt; }
	void	Math_High_Potion(_int _Cnt) { iHigh_PotionCnt += _Cnt; };

	_int	Get_Elixir() { return iElixirCnt; }
	void	Set_Elixir(_int _Cnt) { iElixirCnt = _Cnt; }
	void	Math_Elixir(_int _Cnt) { iElixirCnt += _Cnt; };

	_int	Get_AtkUp() { return iAtkUpCnt; }
	void	Set_AtkUp(_int _Cnt) { iAtkUpCnt = _Cnt; }
	void	Math_AtkUp(_int _Cnt) { iAtkUpCnt += _Cnt; }

	_int	Get_AtkSpeedUp() { return iAtkSpeedUpCnt; }
	void	Set_AtkSpeedUp(_int _Cnt) { iAtkSpeedUpCnt = _Cnt; }
	void	Math_AtkSpeedUp(_int _Cnt) { iAtkSpeedUpCnt += _Cnt; }

	_int	Get_SpeedUp() { return iSpeedUpCnt; }
	void	Set_SpeedUp(_int _Cnt) { iSpeedUpCnt = _Cnt; }
	void	Math_SpeedUp(_int _Cnt) { iSpeedUpCnt += _Cnt; }

	_int	Get_iEmpty() { return iEmpty; } 

	/*===*/
	/*===*/

	_int	Get_Money() { return iMoney; }
	void	Set_Money(_int _Cnt) { iMoney = _Cnt; }
	void	Math_Money(_int _Cnt) { iMoney += _Cnt; }

	_int	Get_TotalMoney() { return iTotalMoney; }
	void	Math_TotalMoney(_int _Cnt) { iTotalMoney += _Cnt; }

	/*===*/
	/*===*/

	_bool	Get_ItemWindow() { return bItemWindow; }
	void	Set_ItemWindow(_bool _Cnt) { bItemWindow = _Cnt; }

	_bool	Get_ShopWindow() { return bShopWindow; }
	void	Set_ShopWindow(_bool _Cnt) { bShopWindow = _Cnt; }

	/*===*/
	/*===*/

	_float	Get_MovePositionX() { return fMovePositionX; }
	void	Set_MovePositionX(_float _Pos) { fMovePositionX = _Pos; }

	_float	Get_MovePositionY() { return fMovePositionY; }
	void	Set_MovePositionY(_float _Pos) { fMovePositionY = _Pos; }

	_bool	Get_InvenDelete() { return bInvenDelete; }
	void	Set_InvenDelete(_bool _Cnt) { bInvenDelete = _Cnt; }

	_bool	Get_InvenDelete_List() { return bInvenDelete_List; }
	void	Set_InvenDelete_List(_bool _Cnt) { bInvenDelete_List = _Cnt; }

	_bool	Get_InvenList_Render() { return bInvenList_Render; }
	void	Set_InvenList_Render(_bool _Cnt) { bInvenList_Render = _Cnt; }

	/* class UI_ItemGet,UI_ItemIcon*/
	_int	Get_Frame() { return iFrame; }
	void	Set_Frame(_int _Cnt) { iFrame = _Cnt; }

	_bool	Get_ShopItemText() { return bShopItemText; }
	void	Set_ShopItemText(_bool _Cnt) { bShopItemText = _Cnt; }

	/* InvenNum_Render */
	_bool	Get_InvenNumRender() { return bInvenNumRender; }
	void	Set_InvenNumRender(_bool _Cnt) { bInvenNumRender = _Cnt; }

private:
	/* =========== */
	_int	iPotionCnt = 0;
	_int	iHigh_PotionCnt = 0;
	_int	iElixirCnt = 0;
	_int	iAtkUpCnt = 0;
	_int	iAtkSpeedUpCnt = 0;
	_int	iSpeedUpCnt = 0;
	_int	iEmpty = 0;

	/* Money */
	_int	iMoney = 0;
	_int	iTotalMoney = 1000;

	/* Slot */
	_int	iQuickSlot1 = 0;
	_int	iQuickSlot2 = 0;
	_int	iQuickSlot3 = 0;
	_int	iQuickSlot1_Name = SLOT_EMPTY;
	_int	iQuickSlot2_Name = SLOT_EMPTY;
	_int	iQuickSlot3_Name = SLOT_EMPTY;

	_int	iInvenSlot1 = 0;
	_int	iInvenSlot2 = 0;
	_int	iInvenSlot3 = 0;
	_int	iInvenSlot4 = 0;
	_int	iInvenSlot5 = 0;
	_int	iInvenSlot6 = 0;

	_int	iInvenSlot1_Name = SLOT_EMPTY;
	_int	iInvenSlot2_Name = SLOT_EMPTY;
	_int	iInvenSlot3_Name = SLOT_EMPTY;
	_int	iInvenSlot4_Name = SLOT_EMPTY;
	_int	iInvenSlot5_Name = SLOT_EMPTY;
	_int	iInvenSlot6_Name = SLOT_EMPTY;

	/* UI_QiuckItemText */
	_bool	m_bQuickItemText = false;
	_int	m_iQuickItem_Num = 0;

	/* ON ITEM , SHOP */
	_bool	bItemWindow = false;
	_bool	bShopWindow = false;

	/* Inven Move */
	_float	fMovePositionX = 0.f;
	_float	fMovePositionY = 0.f;

	/* Inven Delete */
	_bool	bInvenDelete = false;
	_bool	bInvenDelete_List = false;
	_bool	bInvenList_Render = false;

	/* class UI_ItemGet,UI_ItemIcon*/
	_int	iFrame = 0;
	_bool	bShopItemText = false;

	/* InvenNum_Render */
	_bool	bInvenNumRender = false;

public:
	void Inven_sort(_int _Name);
	_int iSortcase = 0;
	void Inven_sortSub();
	void Inven_Object_Zero();

private:
	_int InvenSlot_Get(_int _Name);
	_int QickSlot_Get(_int _Name);

	void Name_Math(_int _Name);

public:
	virtual void	Free() override;
};

END

/*

if(0 != Get_Potion())
{
	플레이어 회복 
}








*/