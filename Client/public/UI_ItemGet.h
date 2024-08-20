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

class CUI_ItemGet final : public CUI_Parents
{
private:
	CUI_ItemGet(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_ItemGet(const CUI_ItemGet& rhs);
	virtual ~CUI_ItemGet() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CShader*			m_pShaderCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

	enum		ITEM_GET_PATTERN { PATTERN_1, PATTERN_2, PATTERN_3, PATTERN_END };
	_int		iPattern_Cnt = 0;

	_double		dGetTime = 0.0;
	_float		fSpeedX = 3.f;
	_float		fSpeedY = 1.f;

	//===
	_int		iPosition_Cnt = 0;
	_int		iResent_Cnt = 0;
	_bool		bGet_Item = false;

	/* UI_Position === */
	_bool		bCreate_ItemIcon = false;

	_int		iItemCnt = 0;

	/* =============== */

	void		Creat_Item();


public:
	static CUI_ItemGet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END