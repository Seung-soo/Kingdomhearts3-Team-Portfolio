#pragma once

#include"Client_Defines.h"
#include"UI_Parents.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer_Rect;
class CShader;
class CTexture;
END

BEGIN(Client)

enum SLOT_STATE
{
	SLOT_STATE_FIRST, SLOT_STATE_SECOND_2, SLOT_STATE_SECOND_3, SLOT_STATE_END
};

class CUI_bt_Command final : public CUI_Parents
{
private:
	CUI_bt_Command(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_bt_Command(const CUI_bt_Command& rhs);
	virtual ~CUI_bt_Command() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	static  _int st_iSlot_State;

private:
	_int			iCom_Texture_Set = 0;

	// UI_BT_SLOT ===
private:
	_int		Fnc_On_Slot1(_double TimeDelta);
	_int		Fnc_On_Slot2(_double TimeDelta);
	_int		Fnc_On_Slot3(_double TimeDelta);
	_int		Fnc_On_Slot4(_double TimeDelta);

	_int		Fnc_On_Slot2_1(_double TimeDelta);
	_int		Fnc_On_Slot2_2(_double TimeDelta);
	_int		Fnc_On_Slot2_3(_double TimeDelta);
	_int		Fnc_On_Slot2_4(_double TimeDelta);

	_int		Fnc_On_Slot3_1(_double TimeDelta);
	_int		Fnc_On_Slot3_2(_double TimeDelta);
	_int		Fnc_On_Slot3_3(_double TimeDelta);

	void		Fnc_Slot3_Qucik(_int _Name);
	void		QuickSlot_InvenSort(_int _Name);

	/* Slot2 */
	_bool	bMagicSlot = false;

	_double	dSlot_Time = 0.0;
	_float		fOn_Slot_Position = 0.f;
	_float		fOff_Slot_Position = 0.f;
	_bool		bOn_Slot = false;
	_bool		bOff_Slot = false;

	CGameObject*	m_pMagicSlot_1 = nullptr;
	CGameObject*	m_pMagicSlot_2 = nullptr;

	// UI_BT_FAN
	_double	dFanTime = 0.0;
	_bool	bFan = false;


	// UI_BT_FRAME7
	_double	dLightTime_1 = 0.0;
	_bool	bLight = false;
	_int	iLightCnt = 0;

	// UI_TT_SELECT ===============
	_double		dSelect_Time = 0.0;
	_float		fSelect_SetX = 0.f;
	_float		fSelect_MoveSetX = 0.f;
	_float		fSelect_SetY = 0.f;

	_bool		bSelect_Move_Left = false;

	_int		iSelect_First_Cnt = 0;
	_int		iSelect_Second_Cnt = 0;


	// Player
	class CPlayer* m_pPlayer = nullptr;
	class CPlayer_Weapon* m_pWeapon = nullptr;

public:
	static CUI_bt_Command* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

/* DEFINE */ //================

//switch
#define SLOT_FIRST		91
#define SLOT_SECOND		92

#define ON_SLOT1			41
#define ON_SLOT2			42
#define ON_SLOT3			43
#define ON_SLOT4			44

#define ON_SLOT2_1			51
#define ON_SLOT2_2			52
#define ON_SLOT2_3			53
#define ON_SLOT2_4			54

#define ON_SLOT3_1			61
#define ON_SLOT3_2			62
#define ON_SLOT3_3			63

//==========================