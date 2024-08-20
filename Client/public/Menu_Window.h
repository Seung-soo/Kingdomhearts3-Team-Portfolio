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

class CMenu_Window final : public CUI_Parents
{
private:
	CMenu_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMenu_Window(const CMenu_Window& rhs);
	virtual ~CMenu_Window() = default;

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
	void	Shop_Buy(_int _Cnt);
	void	Shop_Sell(_int _Cnt);

private :
	_int	iCom_Texture_Set = 0;
	
	/* Random Position */
	_double	dRandomTime = 0.0;
	_bool	bRandomX = false;
	_bool	bRandomY = false;
	_float	fRandomX = 0.f;
	_float	fRandomY = 0.f;
	_int	iXCnt = 0;
	_int	iYCnt = 0;

	/* ITEM WINDOW ANIMATION */
	_bool	bCreate = true;
	
	_double	dSetTime = 0.001;
	_int	iItemPatternCnt = 0;

	_double	dItemTime_First = 0.0;

	_float	fItemWindowSpeedX = 1.f;
	_float	fItemWindowSpeedY = 1.f;
	_float	fSpeedSet_1 = 2.5f;
	_float	fSpeedSet_2 = 2.f;

	_float	fItemWindowX = 0.f;
	_float	fItemWindowY = 0.f;

	/* ITEM WINDOW SELECT */
	/*Inven*/
	_bool	bInvenFirst = true;
	_bool	bInvenSecond = false;
	_bool	bInvenThird = false;

	_int	iInvenSlotCntFirst = 1;
	_int	iInvenSlotCntSecond = 1;
	_int	iInvenSlotCntThird = 1;

	_float	fSelect_PosX = 10.5f;
	_float	fSelect_PosY = 45.f;

	_int	iInvenTemp = 0;


	/*Shop*/
	_bool	bSale = true;
	
	_bool	bShopFirst = true;
	_bool	bShopSecond = false;
	_bool	bShopThird = false;

	_int	iShopSlotCntFirst = 1;
	_int	iShopSlotCntSecond = 1;
	_int	iShopSlotCntThird = 1;

	_double ShopWindow6Time = 0.0;

	_int	iCreateCnt = 0;

	_int	iShopObject = 0;
	_int	iShopObjectCnt = 1;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CMenu_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END