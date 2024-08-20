#pragma once

#include "Client_Defines.h"
#include "UI_Parents.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CVIBuffer_Rect;
END


/* Prototype_GameObject_LoadingBlack 4 */

BEGIN(Client)
class UI_FadeOut final : public CUI_Parents
{
private:
	UI_FadeOut(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	UI_FadeOut(const UI_FadeOut& rhs);
	virtual ~UI_FadeOut() = default;
public:
	_float			Get_FadeOut() { return fFadeOut; }
	_bool			Get_FadeOutMax() { return m_bFadeOutMax; }
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CVIBuffer_Rect*				m_pVIBufferCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

	_double		dFadeTime = 0.0;
	_float		fFadeOut = 1.f;
	_float		fFadeIn = 0.f;
	_bool		m_bFadeOutMax = false;


	_int		iMinigamePartten = 0;
	_double		dMiniGameTIme = 0;
	_bool		bMinigameStart = false;
	_bool		bFlash = false;
	_int		iFlashCnt = 0;


public:
	static UI_FadeOut* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END
