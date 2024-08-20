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
class CUI_Menu final : public CUI_Parents
{
private:
	CUI_Menu(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_Menu(const CUI_Menu& rhs);
	virtual ~CUI_Menu() = default;

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
	_int iCom_Texture_Set = 0;

	_double		dBurbleTime = 0.0;
	_double		dTime = 0.0;

	_double		dBackGrondTime = 0.0;
	_float		fBackGround_Size = 0.f;
	_float		fBackG_SizeSpeed = 0.f;
	_float		fBackGround_X = 0.f;
	_float		fBackGround_Y = 0.f;

	_double		dFrameTime = 0.0;
	_float		fFrameX_Speed = 0.f;

	_double		dMoneyBarTime = 0.0;
	_float		fMoneyBarY_Speed = 0.f;

	_double		dIconSizeTime = 0.0;
	_float		fIconSize = 0.f;
	_float		fIconSizeSpeed = 0.f;

	_double		dPlayerTime = 0.0;
	_float		fPlayerSetX = 0.f;
	_float		fPlayerSetXSpeed = 1.f;

	_bool		bMoneyRender = false;

public:
	static CUI_Menu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
