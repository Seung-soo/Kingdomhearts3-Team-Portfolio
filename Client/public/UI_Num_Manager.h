#pragma once

#include"Client_Defines.h"
#include"UI_Parents.h"

BEGIN(Client)

class CUI_Num_Manager	final : public CBase
{
	MAKE_SINGLETON(CUI_Num_Manager)
public:
	CUI_Num_Manager();
	virtual ~CUI_Num_Manager() = default;

	void	Update(_double TImeDelta);

public:
	void	Setting_Number(_int _Number, _float _FontSize);
	void	Setting_Combo(_int _Number, _float _FontSize);
	void	Setting_Money(_int _Number, _float _FontSize);
	void	Setting_Menu(_int _Number, _float _FontSize);
	void	Setting_Menu_Shop(_int _Number, _float _FontSize);
	void	Setting_Minigame(_int _Number, _float _FontSize);

	void	Setting_InvenNumber1(_int _Number, _float _FontSize, _float _SetX, _float _SetY);
	void	Setting_InvenNumber2(_int _Number, _float _FontSize, _float _SetX, _float _SetY);
	void	Setting_InvenNumber3(_int _Number, _float _FontSize, _float _SetX, _float _SetY);
	void	Setting_InvenNumber4(_int _Number, _float _FontSize, _float _SetX, _float _SetY);
	void	Setting_InvenNumber5(_int _Number, _float _FontSize, _float _SetX, _float _SetY);
	void	Setting_InvenNumber6(_int _Number, _float _FontSize, _float _SetX, _float _SetY);
	
public:
	_bool	Get_ComboRender() { return bComboRender; }
	void	Set_ComboRender(_bool _Render) { bComboRender = _Render; }

	_float	Get_ComboSize() { return fComboSize; }
	void	Math_ComboSize(_float _Size) { fComboSize += _Size; }
	void	Set_ComboSize(_float _Size) { fComboSize = _Size; }

	_float	Get_ComboPos() { return fComboPos; }
	void	Set_ComboPos(_float _Pos) { fComboPos = _Pos;}

	//COMBOBAR
	_float	Get_ComboBarPos() { return fComboBarPos; }
	void	Math_ComboBarPos(_float _Pos) { fComboBarPos += _Pos; }
	void	Set_ComboBarPos(_float _Pos) { fComboBarPos = _Pos; }

	_bool	Get_ComboBar_Render() { return bComboBar; }
	void	Set_ComboBar_Render(_bool _Render) { bComboBar = _Render; }

	//Money_UI
	_bool	Get_MoneyRender() { return bMoneyRender; }
	void	Set_MoneyRender(_bool _Render) { bMoneyRender = _Render; }

	_float	Get_MoneySize() { return fMoneySize; }
	void	Math_MoneySize(_float _Size) { fMoneySize += _Size; }
	void	Set_MoneySize(_float _Size) { fMoneySize = _Size; }

	_float	Get_MoneyPos() { return fMoneyPos; }
	void	Set_MoneyPos(_float _Pos) { fMoneyPos = _Pos; }

	_float	Get_MoneyBarPos() { return fMoneyBarPos; }
	void	Math_MoneyBarPos(_float _Pos) { fMoneyBarPos += _Pos; }
	void	Set_MoneyBarPos(_float _Pos) { fMoneyBarPos = _Pos; }

	_bool	Get_MoneyBar_Render() { return bMoneyBar; }
	void	Set_MoneyBar_Render(_bool _Render) { bMoneyBar = _Render; }

	_bool	Get_MoneyBar_Render_Cnt() { return bMoneyBar_Cnt; }
	void	Set_MoneyBar_Render_Cnt(_bool _Render) { bMoneyBar_Cnt = _Render; }

	_bool	Get_MeunNum_Render() { return bMenuNum_Render; }
	void	Set_MenuNum_Render(_bool _Cnt) { bMenuNum_Render = _Cnt; }

public:
	HRESULT Create_ComboBar();
	HRESULT Delete_ComboBar();
	void Update_ComboUI_Effect();

private:
	//Setting_Number
	_int				iNumber = 0;
	_float				fGap = 0.f;
	_float				fGapSet = 0.f;

	vector<_int>		vNumber;

	//Combo
	_bool				bComboRender = false;
	_float				fComboSize = 0.f;
	_float				fComboPos = 0.f;

	_float				fComboBarPos = 0.f;
	_bool				bComboBar = false;

	//Money_UI
	_bool				bMoneyRender = false;
	_float				fMoneySize = 0.f;
	_float				fMoneyPos = 0.f;

	_float				fMoneyBarPos = 0.f;
	_bool				bMoneyBar = false;
	_bool				bMoneyBar_Cnt = false;

	_double				dMoneyBarTime = 0.0;
	_double				dMoneySizeTime = 0.0;

	//Menu_Num
	_bool				bMenuNum_Render = false;


	/* .cpp Combo */
	_double		d_NMPosTime = 0.0;
	_int		i_NMPosCnt = 0;
	_float		f_NMPosSpeed = 0.f;
	_bool		b_NMPosition = false;
	_float		f_NMPosition = 0.f;

	_float		f_NMBarSpeed = 0.f;

	// Player
	class CPlayer* m_pPlayer = nullptr;
	class CGameObject* m_pComboBar = nullptr;

public:
	virtual void Free() override;

};

END