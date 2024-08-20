#include "stdafx.h"
#include "..\public\UI_Num_Manager.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "InvenManager.h"
#include "Easing_Function.h"

#include "Player.h"

READY_SINGLETON(CUI_Num_Manager)

CUI_Num_Manager::CUI_Num_Manager()
{
}

void CUI_Num_Manager::Update(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	LEVEL current_level = GM->Get_CurrentLevel();

	// �������� ������ ��
	if (LEVEL_ENDING != current_level &&
		LEVEL_TWILIGHT <= current_level &&
		LEVEL_MAPTOOL > current_level)
	{
		if (nullptr == m_pPlayer)
		{
			m_pPlayer = static_cast<CPlayer*>(pGameInstance->Get_Front(GM->Get_CurrentLevel(), TEXT("Layer_Player")));

			if (nullptr == m_pPlayer)
				_breakpoint;
		}

		_uint combo_number = m_pPlayer->Get_Combo();

		_double combo_timer = m_pPlayer->Get_ComboTimer();
		_double combo_timer_limit = m_pPlayer->Get_ComboTimer_Limit();

		// �޺���Ʈ ũ�� �ִϸ��̼� ����
		_float	fLeastComboSize = 60.f;
		if (fLeastComboSize < fComboSize)
		{
			Math_ComboSize(-2.f);
			Math_ComboBarPos(+2.f);
		}
		
		/* =================================== */

		Setting_Combo((_int)combo_number, fComboSize); // COMBO_Font Create

		/* �޺���Ʈ �̵� �ִϸ��̼� ���� */
		if (0.5 >= combo_timer)
		{
			d_NMPosTime += TimeDelta;
			f_NMPosSpeed = 1.f + EaseInBack(30.f, (_float)(0.5 - combo_timer), 0.5f);
			f_NMBarSpeed = 1.f + EaseInBack(60.f, (_float)(0.5 - combo_timer), 0.5f); //Bar
			fComboBarPos = f_NMBarSpeed; //Bar

			f_NMPosition += f_NMPosSpeed;
			fComboPos = f_NMPosition;
		}

		/* ======================================================================================== */

		/* MoneyBar Render ===== */
		if (true == bMoneyBar)
		{
			dMoneyBarTime += TimeDelta;
			dMoneySizeTime += TimeDelta;
			int iMoney = Inven->Get_Money();

			if (true == bMoneyBar_Cnt) // Money_Object ȹ�渶�� , Render �ʱ�ȭ
			{
				fMoneySize = 50.f;

				dMoneyBarTime = 0.0;
				bMoneyBar_Cnt = false;
			}

			_float	fLeastComboSize = 32.f; /* �޺���Ʈ �ּ�ũ��*/
			if (0.02 < dMoneySizeTime
				&& fLeastComboSize < fMoneySize)
				Math_MoneySize(-1.f);

			Setting_Money(iMoney, fMoneySize);

			/* ========= */
			/* ========= */

			if (3.0 < dMoneyBarTime)
			{
				Inven->Math_TotalMoney(iMoney);
				Inven->Set_Money(0);

				/*CUI_Money Delete */
				if (FAILED(pGameInstance->Release_LayerObject(current_level, TEXT("Layer_Money"))))
				{
					BREAKPOINT;
					return;
				}
				bMoneyBar = false;
				dMoneyBarTime = 0.0;
			}
		}

		// �÷��̾��� �޺� ���� 0���� �̰ų� �޺� �ٰ� �����Ǿ����� �ʴٸ� return
		if (0 >= m_pPlayer->Get_Combo() || nullptr == m_pComboBar)
			return;

		/* ========================================================================================== */
	}
	else if (LEVEL_LOADING == current_level)
	{
		// �ε� �߿��� nullptr ó��
		if (nullptr != m_pPlayer)
			m_pPlayer = nullptr;
	}
	

	/* ========================= */
}

void CUI_Num_Manager::Setting_Number(_int _Number, _float _FontSize)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// ���۽� ���� ������ ���� =====
	pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_Number"));

	//======================
	bComboRender = false;

	CUI_Parents::FONT_SET	m_Font;
	m_Font.Size = _FontSize;

	iNumber = _Number;
	fGapSet = _FontSize*0.65f;
	fGap = fGapSet;

	if (9 >= iNumber)
	{
		_int	ivalue = _Number;

		fGap -= fGapSet;

		iNumber = ivalue;
		vNumber.push_back(ivalue);

		m_Font.Number = vNumber.back();
		m_Font.Gap = fGap;
		pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Number"), TEXT("Prototype_GameObject_UI_Normal"), &m_Font);
	}

	while (10 <= iNumber)
	{
		_int	ivalue = 0;
		_int	iremainder = 0;

		fGap -= fGapSet;

		if (10 <= iNumber)
		{
			ivalue = iNumber / 10;
			iremainder = iNumber % 10;
		}

		iNumber = ivalue;

		// Set.1===
		vNumber.push_back(iremainder);

		m_Font.Number = vNumber.back();
		m_Font.Gap = fGap;
		pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Number"), TEXT("Prototype_GameObject_UI_Normal"), &m_Font);

		// Set.2===
		if (9 >= iNumber)
		{
			fGap -= fGapSet;

			vNumber.push_back(ivalue);

			m_Font.Number = vNumber.back();
			m_Font.Gap = fGap;
			pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Number"), TEXT("Prototype_GameObject_UI_Normal"), &m_Font);
		}
	}

	vNumber.clear();

}

void CUI_Num_Manager::Setting_Combo(_int _Number, _float _FontSize)
{
	if (0.f > _Number)
		_int a = 0;
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// ���۽� ���� ������ ���� =====
	pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_Combo"));

	//======================

	CUI_Parents::FONT_SET	m_Font;
	m_Font.Size = _FontSize;

	iNumber = _Number;
	fGapSet = _FontSize*0.65f;
	fGap = fGapSet;

	if (9 >= iNumber)
	{
		_int	ivalue = _Number;

		fGap -= fGapSet;

		iNumber = ivalue;
		vNumber.push_back(ivalue);

		m_Font.Number = vNumber.back();
		m_Font.Gap = fGap;
		pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Combo"), TEXT("Prototype_GameObject_UI_Combo"), &m_Font);
	}

	while (10 <= iNumber)
	{
		_int	ivalue = 0;
		_int	iremainder = 0;

		fGap -= fGapSet;

		if (10 <= iNumber)
		{
			ivalue = iNumber / 10;
			iremainder = iNumber % 10;
		}

		iNumber = ivalue;

		// Set.1===
		vNumber.push_back(iremainder);

		m_Font.Number = vNumber.back();
		m_Font.Gap = fGap;
		pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Combo"), TEXT("Prototype_GameObject_UI_Combo"), &m_Font);

		// Set.2===
		if (9 >= iNumber)
		{
			fGap -= fGapSet;

			vNumber.push_back(ivalue);

			m_Font.Number = vNumber.back();
			m_Font.Gap = fGap;
			pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Combo"), TEXT("Prototype_GameObject_UI_Combo"), &m_Font);
		}
	}

	vNumber.clear();

}

void CUI_Num_Manager::Setting_Money(_int _Number, _float _FontSize)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// ���۽� ���� ������ ���� =====
	pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_Money"));

	//======================

	CUI_Parents::FONT_SET	m_Font;
	m_Font.Size = _FontSize;

	iNumber = _Number;
	fGapSet = _FontSize*0.85f;
	fGap = fGapSet;

	if (9 >= iNumber)
	{
		_int	ivalue = _Number;

		fGap -= fGapSet;

		iNumber = ivalue;
		vNumber.push_back(ivalue);

		m_Font.Number = vNumber.back();
		m_Font.Gap = fGap;
		pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Money"), TEXT("Prototype_GameObject_UI_Money"), &m_Font);
	}

	while (10 <= iNumber)
	{
		_int	ivalue = 0;
		_int	iremainder = 0;

		fGap -= fGapSet;

		if (10 <= iNumber)
		{
			ivalue = iNumber / 10;
			iremainder = iNumber % 10;
		}

		iNumber = ivalue;

		// Set.1===
		vNumber.push_back(iremainder);

		m_Font.Number = vNumber.back();
		m_Font.Gap = fGap;
		pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Money"), TEXT("Prototype_GameObject_UI_Money"), &m_Font);

		// Set.2===
		if (9 >= iNumber)
		{
			fGap -= fGapSet;

			vNumber.push_back(ivalue);

			m_Font.Number = vNumber.back();
			m_Font.Gap = fGap;
			pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Money"), TEXT("Prototype_GameObject_UI_Money"), &m_Font);
		}
	}

	vNumber.clear();
}

void CUI_Num_Manager::Setting_Menu(_int _Number, _float _FontSize)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// ���۽� ���� ������ ���� =====
	pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_Money"));

	//======================

	CUI_Parents::FONT_SET	m_Font;
	m_Font.Size = _FontSize;

	iNumber = _Number;
	fGapSet = _FontSize*0.85f;
	fGap = fGapSet;

	if (9 >= iNumber)
	{
		_int	ivalue = _Number;

		fGap -= fGapSet;

		iNumber = ivalue;
		vNumber.push_back(ivalue);

		m_Font.Number = vNumber.back();
		m_Font.Gap = fGap;
		pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Money"), TEXT("Prototype_GameObject_UI_MenuNumber"), &m_Font);
	}

	while (10 <= iNumber)
	{
		_int	ivalue = 0;
		_int	iremainder = 0;

		fGap -= fGapSet;

		if (10 <= iNumber)
		{
			ivalue = iNumber / 10;
			iremainder = iNumber % 10;
		}

		iNumber = ivalue;

		// Set.1===
		vNumber.push_back(iremainder);

		m_Font.Number = vNumber.back();
		m_Font.Gap = fGap;
		pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Money"), TEXT("Prototype_GameObject_UI_MenuNumber"), &m_Font);

		// Set.2===
		if (9 >= iNumber)
		{
			fGap -= fGapSet;

			vNumber.push_back(ivalue);

			m_Font.Number = vNumber.back();
			m_Font.Gap = fGap;
			pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Money"), TEXT("Prototype_GameObject_UI_MenuNumber"), &m_Font);
		}
	}

	vNumber.clear();
}

void CUI_Num_Manager::Setting_Menu_Shop(_int _Number, _float _FontSize)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// ���۽� ���� ������ ���� =====
	pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_Money_Shop"));

	//======================

	CUI_Parents::FONT_SET	m_Font;
	m_Font.Size = _FontSize;

	iNumber = _Number;
	fGapSet = _FontSize*0.85f;
	fGap = fGapSet;

	if (9 >= iNumber)
	{
		_int	ivalue = _Number;

		fGap -= fGapSet;

		iNumber = ivalue;
		vNumber.push_back(ivalue);

		m_Font.Number = vNumber.back();
		m_Font.Gap = fGap;
		pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Money_Shop"), TEXT("Prototype_GameObject_UI_MenuNumber_Shop"), &m_Font);
	}

	while (10 <= iNumber)
	{
		_int	ivalue = 0;
		_int	iremainder = 0;

		fGap -= fGapSet;

		if (10 <= iNumber)
		{
			ivalue = iNumber / 10;
			iremainder = iNumber % 10;
		}

		iNumber = ivalue;

		// Set.1===
		vNumber.push_back(iremainder);

		m_Font.Number = vNumber.back();
		m_Font.Gap = fGap;
		pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Money_Shop"), TEXT("Prototype_GameObject_UI_MenuNumber_Shop"), &m_Font);

		// Set.2===
		if (9 >= iNumber)
		{
			fGap -= fGapSet;

			vNumber.push_back(ivalue);

			m_Font.Number = vNumber.back();
			m_Font.Gap = fGap;
			pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Money_Shop"), TEXT("Prototype_GameObject_UI_MenuNumber_Shop"), &m_Font);
		}
	}

	vNumber.clear();
}

void CUI_Num_Manager::Setting_Minigame(_int _Number, _float _FontSize)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// ���۽� ���� ������ ���� =====
	pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_Minigame"));

	//======================

	CUI_Parents::FONT_SET	m_Font;
	m_Font.Size = _FontSize;

	iNumber = _Number;
	fGapSet = _FontSize*0.85f;
	fGap = fGapSet;

	if (9 >= iNumber)
	{
		_int	ivalue = _Number;

		fGap -= fGapSet;

		iNumber = ivalue;
		vNumber.push_back(ivalue);

		m_Font.Number = vNumber.back();
		m_Font.Gap = fGap;
		pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Minigame"), TEXT("Prototype_GameObject_UI_Minigame_Num"), &m_Font);
	}

	while (10 <= iNumber)
	{
		_int	ivalue = 0;
		_int	iremainder = 0;

		fGap -= fGapSet;

		if (10 <= iNumber)
		{
			ivalue = iNumber / 10;
			iremainder = iNumber % 10;
		}

		iNumber = ivalue;

		// Set.1===
		vNumber.push_back(iremainder);

		m_Font.Number = vNumber.back();
		m_Font.Gap = fGap;
		pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Minigame"), TEXT("Prototype_GameObject_UI_Minigame_Num"), &m_Font);

		// Set.2===
		if (9 >= iNumber)
		{
			fGap -= fGapSet;

			vNumber.push_back(ivalue);

			m_Font.Number = vNumber.back();
			m_Font.Gap = fGap;
			pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Minigame"), TEXT("Prototype_GameObject_UI_Minigame_Num"), &m_Font);
		}
	}

	vNumber.clear();
}

void CUI_Num_Manager::Setting_InvenNumber1(_int _Number, _float _FontSize, _float _SetX, _float _SetY)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// ���۽� ���� ������ ���� =====
	pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_InvenNum1"));

	//======================

	CUI_Parents::FONT_SET	m_Font;
	m_Font.Size = _FontSize;

	iNumber = _Number;
	fGapSet = _FontSize*0.85f;
	fGap = fGapSet;

	if (9 >= iNumber)
	{
		_int	ivalue = _Number;

		fGap -= fGapSet;

		iNumber = ivalue;
		vNumber.push_back(ivalue);

		m_Font.Number = vNumber.back();
		m_Font.Gap = fGap;
		m_Font.Set_X = _SetX;
		m_Font.Set_Y = _SetY;
		pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_InvenNum1"), TEXT("Prototype_GameObject_UI_InvenNum"), &m_Font);
	}

	while (10 <= iNumber)
	{
		_int	ivalue = 0;
		_int	iremainder = 0;

		fGap -= fGapSet;

		if (10 <= iNumber)
		{
			ivalue = iNumber / 10;
			iremainder = iNumber % 10;
		}

		iNumber = ivalue;

		// Set.1===
		vNumber.push_back(iremainder);

		m_Font.Number = vNumber.back();
		m_Font.Gap = fGap;
		pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_InvenNum1"), TEXT("Prototype_GameObject_UI_InvenNum"), &m_Font);

		// Set.2===
		if (9 >= iNumber)
		{
			fGap -= fGapSet;

			vNumber.push_back(ivalue);

			m_Font.Number = vNumber.back();
			m_Font.Gap = fGap;
			pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_InvenNum1"), TEXT("Prototype_GameObject_UI_InvenNum"), &m_Font);
		}
	}

	vNumber.clear();
}

void CUI_Num_Manager::Setting_InvenNumber2(_int _Number, _float _FontSize, _float _SetX, _float _SetY)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// ���۽� ���� ������ ���� =====
	pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_InvenNum2"));

	//======================

	CUI_Parents::FONT_SET	m_Font;
	m_Font.Size = _FontSize;

	iNumber = _Number;
	fGapSet = _FontSize*0.85f;
	fGap = fGapSet;

	if (9 >= iNumber)
	{
		_int	ivalue = _Number;

		fGap -= fGapSet;

		iNumber = ivalue;
		vNumber.push_back(ivalue);

		m_Font.Number = vNumber.back();
		m_Font.Gap = fGap;
		m_Font.Set_X = _SetX;
		m_Font.Set_Y = _SetY;
		pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_InvenNum2"), TEXT("Prototype_GameObject_UI_InvenNum"), &m_Font);
	}

	while (10 <= iNumber)
	{
		_int	ivalue = 0;
		_int	iremainder = 0;

		fGap -= fGapSet;

		if (10 <= iNumber)
		{
			ivalue = iNumber / 10;
			iremainder = iNumber % 10;
		}

		iNumber = ivalue;

		// Set.1===
		vNumber.push_back(iremainder);

		m_Font.Number = vNumber.back();
		m_Font.Gap = fGap;
		pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_InvenNum2"), TEXT("Prototype_GameObject_UI_InvenNum"), &m_Font);

		// Set.2===
		if (9 >= iNumber)
		{
			fGap -= fGapSet;

			vNumber.push_back(ivalue);

			m_Font.Number = vNumber.back();
			m_Font.Gap = fGap;
			pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_InvenNum2"), TEXT("Prototype_GameObject_UI_InvenNum"), &m_Font);
		}
	}

	vNumber.clear();
}

void CUI_Num_Manager::Setting_InvenNumber3(_int _Number, _float _FontSize, _float _SetX, _float _SetY)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// ���۽� ���� ������ ���� =====
	pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_InvenNum3"));

	//======================

	CUI_Parents::FONT_SET	m_Font;
	m_Font.Size = _FontSize;

	iNumber = _Number;
	fGapSet = _FontSize*0.85f;
	fGap = fGapSet;

	if (9 >= iNumber)
	{
		_int	ivalue = _Number;

		fGap -= fGapSet;

		iNumber = ivalue;
		vNumber.push_back(ivalue);

		m_Font.Number = vNumber.back();
		m_Font.Gap = fGap;
		m_Font.Set_X = _SetX;
		m_Font.Set_Y = _SetY;
		pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_InvenNum3"), TEXT("Prototype_GameObject_UI_InvenNum"), &m_Font);
	}

	while (10 <= iNumber)
	{
		_int	ivalue = 0;
		_int	iremainder = 0;

		fGap -= fGapSet;

		if (10 <= iNumber)
		{
			ivalue = iNumber / 10;
			iremainder = iNumber % 10;
		}

		iNumber = ivalue;

		// Set.1===
		vNumber.push_back(iremainder);

		m_Font.Number = vNumber.back();
		m_Font.Gap = fGap;
		pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_InvenNum3"), TEXT("Prototype_GameObject_UI_InvenNum"), &m_Font);

		// Set.2===
		if (9 >= iNumber)
		{
			fGap -= fGapSet;

			vNumber.push_back(ivalue);

			m_Font.Number = vNumber.back();
			m_Font.Gap = fGap;
			pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_InvenNum3"), TEXT("Prototype_GameObject_UI_InvenNum"), &m_Font);
		}
	}

	vNumber.clear();
}

void CUI_Num_Manager::Setting_InvenNumber4(_int _Number, _float _FontSize, _float _SetX, _float _SetY)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// ���۽� ���� ������ ���� =====
	pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_InvenNum4"));

	//======================

	CUI_Parents::FONT_SET	m_Font;
	m_Font.Size = _FontSize;

	iNumber = _Number;
	fGapSet = _FontSize*0.85f;
	fGap = fGapSet;

	if (9 >= iNumber)
	{
		_int	ivalue = _Number;

		fGap -= fGapSet;

		iNumber = ivalue;
		vNumber.push_back(ivalue);

		m_Font.Number = vNumber.back();
		m_Font.Gap = fGap;
		m_Font.Set_X = _SetX;
		m_Font.Set_Y = _SetY;
		pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_InvenNum4"), TEXT("Prototype_GameObject_UI_InvenNum"), &m_Font);
	}

	while (10 <= iNumber)
	{
		_int	ivalue = 0;
		_int	iremainder = 0;

		fGap -= fGapSet;

		if (10 <= iNumber)
		{
			ivalue = iNumber / 10;
			iremainder = iNumber % 10;
		}

		iNumber = ivalue;

		// Set.1===
		vNumber.push_back(iremainder);

		m_Font.Number = vNumber.back();
		m_Font.Gap = fGap;
		pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_InvenNum4"), TEXT("Prototype_GameObject_UI_InvenNum"), &m_Font);

		// Set.2===
		if (9 >= iNumber)
		{
			fGap -= fGapSet;

			vNumber.push_back(ivalue);

			m_Font.Number = vNumber.back();
			m_Font.Gap = fGap;
			pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_InvenNum4"), TEXT("Prototype_GameObject_UI_InvenNum"), &m_Font);
		}
	}

	vNumber.clear();
}

void CUI_Num_Manager::Setting_InvenNumber5(_int _Number, _float _FontSize, _float _SetX, _float _SetY)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// ���۽� ���� ������ ���� =====
	pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_InvenNum5"));

	//======================

	CUI_Parents::FONT_SET	m_Font;
	m_Font.Size = _FontSize;

	iNumber = _Number;
	fGapSet = _FontSize*0.85f;
	fGap = fGapSet;

	if (9 >= iNumber)
	{
		_int	ivalue = _Number;

		fGap -= fGapSet;

		iNumber = ivalue;
		vNumber.push_back(ivalue);

		m_Font.Number = vNumber.back();
		m_Font.Gap = fGap;
		m_Font.Set_X = _SetX;
		m_Font.Set_Y = _SetY;
		pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_InvenNum5"), TEXT("Prototype_GameObject_UI_InvenNum"), &m_Font);
	}

	while (10 <= iNumber)
	{
		_int	ivalue = 0;
		_int	iremainder = 0;

		fGap -= fGapSet;

		if (10 <= iNumber)
		{
			ivalue = iNumber / 10;
			iremainder = iNumber % 10;
		}

		iNumber = ivalue;

		// Set.1===
		vNumber.push_back(iremainder);

		m_Font.Number = vNumber.back();
		m_Font.Gap = fGap;
		pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_InvenNum5"), TEXT("Prototype_GameObject_UI_InvenNum"), &m_Font);

		// Set.2===
		if (9 >= iNumber)
		{
			fGap -= fGapSet;

			vNumber.push_back(ivalue);

			m_Font.Number = vNumber.back();
			m_Font.Gap = fGap;
			pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_InvenNum5"), TEXT("Prototype_GameObject_UI_InvenNum"), &m_Font);
		}
	}

	vNumber.clear();
}

void CUI_Num_Manager::Setting_InvenNumber6(_int _Number, _float _FontSize, _float _SetX, _float _SetY)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// ���۽� ���� ������ ���� =====
	pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_InvenNum6"));

	//======================

	CUI_Parents::FONT_SET	m_Font;
	m_Font.Size = _FontSize;

	iNumber = _Number;
	fGapSet = _FontSize*0.85f;
	fGap = fGapSet;

	if (9 >= iNumber)
	{
		_int	ivalue = _Number;

		fGap -= fGapSet;

		iNumber = ivalue;
		vNumber.push_back(ivalue);

		m_Font.Number = vNumber.back();
		m_Font.Gap = fGap;
		m_Font.Set_X = _SetX;
		m_Font.Set_Y = _SetY;
		pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_InvenNum6"), TEXT("Prototype_GameObject_UI_InvenNum"), &m_Font);
	}

	while (10 <= iNumber)
	{
		_int	ivalue = 0;
		_int	iremainder = 0;

		fGap -= fGapSet;

		if (10 <= iNumber)
		{
			ivalue = iNumber / 10;
			iremainder = iNumber % 10;
		}

		iNumber = ivalue;

		// Set.1===
		vNumber.push_back(iremainder);

		m_Font.Number = vNumber.back();
		m_Font.Gap = fGap;
		pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_InvenNum6"), TEXT("Prototype_GameObject_UI_InvenNum"), &m_Font);

		// Set.2===
		if (9 >= iNumber)
		{
			fGap -= fGapSet;

			vNumber.push_back(ivalue);

			m_Font.Number = vNumber.back();
			m_Font.Gap = fGap;
			pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_InvenNum6"), TEXT("Prototype_GameObject_UI_InvenNum"), &m_Font);
		}
	}

	vNumber.clear();
}

HRESULT CUI_Num_Manager::Create_ComboBar()
{
	// �̹� �޺� �ٰ� �����Ǿ��ִٸ� return
	if (nullptr != m_pComboBar)
		return S_OK;

	// ���� ���� �˻�
	LEVEL current_level = GM->Get_CurrentLevel();
	if (LEVEL_TWILIGHT > current_level || LEVEL_FINALBOSS < current_level)
		return S_OK;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// ��ü ����
	if (FAILED(pGameInstance->Add_GameObject(current_level, TEXT("Layer_ComboBar"), TEXT("Prototype_GameObject_UI_ComboBar"))))
		_breakpoint;

	// ������ ��ü ����
	m_pComboBar = pGameInstance->Get_Back(current_level, TEXT("Layer_ComboBar"));
	if (nullptr == m_pComboBar)
		_breakpoint;

	return S_OK;
}

HRESULT CUI_Num_Manager::Delete_ComboBar()
{
	// ���� �ʱ�ȭ
	f_NMPosSpeed = 0.f;
	f_NMBarSpeed = 0.f;
	b_NMPosition = false;
	fComboBarPos = 0.f;

	// ��ü �ʱ�ȭ
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	LEVEL current_level = GM->Get_CurrentLevel();

	if (FAILED(pGameInstance->Release_LayerObject(current_level, TEXT("Layer_Combo"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Release_LayerObject(current_level, TEXT("Layer_ComboBar"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// ���� ���� �ʱ�ȭ
	m_pComboBar = nullptr;

	return S_OK;
}

void CUI_Num_Manager::Update_ComboUI_Effect()
{
	fComboSize = 80.f;
	f_NMPosition = 0.f;
	fComboPos = f_NMPosition;

	Set_ComboBarPos(-20.f);
}

void CUI_Num_Manager::Free()
{
}
