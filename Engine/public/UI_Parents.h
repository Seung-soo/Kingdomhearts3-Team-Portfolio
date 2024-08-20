#pragma once
#include "GameObject.h"


BEGIN(Engine)

class ENGINE_DLL CUI_Parents : public CGameObject
{
protected:
	_float4x4			m_ProjMatrix;
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;

	_float SettingX = 0.f;
	_float SettingY = 0.f;
	_float SizeX = 0.f;
	_float SizeY = 0.f;
	_float SetX = 0.f;
	_float SetY = 0.f;

	/*
	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));
	*/

	typedef struct UI_Setting
	{
		_float*				pSizeX = nullptr;
		_float*				pSizeY = nullptr;
		_float*				pSettingX = nullptr;
		_float*				pSettingY = nullptr;
		_float*				pSetX = nullptr;
		_float*				pSetY = nullptr;

		_int*				p_iProperties = nullptr;

	}UI_SETTING;

	typedef	struct MINIGAME_Setting
	{
		_float4*				pPosition = nullptr;
		_int*				pDamage_Texture = nullptr;

	}MINIGAME_SETTING;


public:
	enum UI_NAME {
		/*Loading_scene*/ UI_LOADING, UI_BLACK, UI_NOW_LOADING,
		/*Play_scene*/ UI_NUMFONT,
		/*UI_tt*/UI_TT_FRAME1, UI_TT_FRAME2, UI_TT_FRAME3,
		UI_TT_SLOT1, UI_TT_SLOT2, UI_TT_SLOT3, UI_TT_SLOT4,
		UI_TT_SLOT2_1, UI_TT_SLOT2_2, UI_TT_SLOT2_3, UI_TT_SLOT2_4,
		UI_TT_SLOT3_1, UI_TT_SLOT3_2, UI_TT_SLOT3_3,
		UI_TT_TEX, UI_TT_TEXT, UI_TT_SHORTCUT, UI_TT_SELECT,
		UI_TT_TRAIN1, UI_TT_TRAIN2,
		/*UI_bt*/UI_BT_FRAME0, UI_BT_FRAME1, UI_BT_FRAME2, UI_BT_FRAME_FAN1, UI_BT_FRAME_FAN2, UI_BT_FRAME4, UI_BT_FRAME5, UI_BT_FRAME6, UI_BT_FRAME7,
		UI_BT_SLOT1, UI_BT_SLOT2, UI_BT_SLOT3, UI_BT_SLOT4,
		UI_BT_SLOT2_1, UI_BT_SLOT2_2, UI_BT_SLOT2_3, UI_BT_SLOT2_4,
		UI_BT_SLOT3_1, UI_BT_SLOT3_2, UI_BT_SLOT3_3, UI_BT_SELECT,
		UI_END
	};

	enum UI_CommandTEXT
	{
		UI_TEXT_ATTACK, UI_TEXT_SKILL, UI_TEXT_ITEM,
		UI_TEXT_DONALD1, UI_TEXT_DONALD2, UI_TEXT_GOOFY1, UI_TEXT_GOOFY2,
		UI_CommandTEXT_END
	};

	enum UI_LOGO {
		LOGO_MAIN, LOGO_BUTTON0, LOGO_BUTTON1, 
		LOGO_TITLE, LOGO_HEART, LOGO_TRD, LOGO_CROWN, LOGO_SORA,
		LOGO_POINT,
		LOGO_END
	};

	enum UI_CIRCLE {
		UI_CIRCLE_MAIN, UI_FACE,
		UI_HP, UI_MP, UI_FOCUS,
		UI_HP_BACK, UI_MP_BACK, UI_FOCUS_BACK,
		UI_CIRCLE_BACK, UI_CIRCLE_BACK2,
		CIRCLE_END
	};

	enum UI_MONSTER {
		MONSTER_TEXT,
		MONSTER_HP, MONSTER_HP_BACK, MONSTER_HP_BAR,
		MONSTER_END
	};

	enum UI_LOCKON {
		UI_LOCKON1, UI_LOCKON2_1, UI_LOCKON2_2,
		UI_LOCKEND
	};

	enum UI_FRAMESECOND {
		DONALD_01, DONALD_02, GOOFY_01, GOOFY_02,
		UI_FRAMESECOND_END
	};

	enum UI_QUICKSLOT {
		SLOT_1, SLOT_2, SLOT_3, 
		SLOT_END
	};

	enum UI_MENU {
		MENU_BACKGROUND, MENU_FRAME, MENU_FRAME_MONEY,
		MENU_ICON_ITEM, MENU_ICON_SHOP, MENU_PLAYER, MENU_PLAYER_BACK,
		MENU_END
	};

	enum MENU_WINDOW {
		ITEMWINDOW0, ITEMWINDOW1, /* INVEN */
		ITEMWINDOWQUICK_SLOT1, ITEMWINDOWQUICK_SLOT2,
		ITEMWINDOWQUICK_SLOT3, ITEMWINDOWQUICK_SLOT4, /* INVEN QUICKSLOT */
		ITEMWINDOW_SLOT1, ITEMWINDOW_SLOT2, ITEMWINDOW_SLOT3,
		ITEMWINDOW_SLOT4, ITEMWINDOW_SLOT5, ITEMWINDOW_SLOT6, /* INVEN SLOT */
		SHOPWINDOW0, SHOPWINDOW1, SHOPWINDOW2, /* SHOP */
		SHOPWINDOW3, SHOPWINDOW4, SHOPWINDOW5, SHOPWINDOW6, SHOPWINDOW7, SHOPWINDOW8,
		WINDOW_SELECT,
		WINDOW_END
	};

	typedef	struct Font_Set
	{
		_int	Number = 0;
		_float	Size = 0.f;
		_float	Gap = 0.f;
		_float	Set_X = 0.f;
		_float	Set_Y = 0.f;

	}FONT_SET;

	enum MINI_GAME {
		MINI_PLAYER,MINI_BULLET,
		MINI_BOSS1, MINI_BOSS2, MINI_BOSS3, MINI_BOSS4, MINI_BOSS5, MINI_GAMEEND,
		MINI_DAMAGE1_0, MINI_DAMAGE1_1, MINI_DAMAGE1_2, MINI_DAMAGE1_3, MINI_DAMAGE1_4, MINI_DAMAGE1_5,
		MINI_DAMAGE2_0, MINI_DAMAGE2_1, MINI_DAMAGE2_2, MINI_DAMAGE2_3, MINI_DAMAGE3_0, MINI_DAMAGE3_1, MINI_DAMAGE3_2,
		MINI_DAMAGE4_0, MINI_DAMAGE4_1, MINI_DAMAGE4_2, MINI_DAMAGE5_0, MINI_DAMAGE5_1, MINI_DAMAGE5_2,
		MINIGAME_END
	};

	enum BOSS_BULLET
	{
		BOSS1_BULLET1, BOSS2_BULLET1, BOSS2_BULLET2,
		BOSS3_BULLET1, BOSS3_BULLET2, BOSS3_BULLET3, BOSS4_BULLET1, BOSS4_BULLET2,
		BOSS5_BULLET1,
		MINIBULLET_END
	};

	_int	Get_ItemCnt() { return iItem_GetCnt; }

protected:
	explicit	CUI_Parents(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit	CUI_Parents(const CUI_Parents& rhs);
	virtual ~CUI_Parents() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

protected:
	UI_SETTING			m_UI; // struct 加己
	_float				m_fFrame = 0.f; // 橇饭烙
	_int				iCom_Texture = 0; // Com_Texture 加己

	_int				iItem_GetCnt = 0;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
