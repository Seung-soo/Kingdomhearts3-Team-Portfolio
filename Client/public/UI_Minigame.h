#pragma once

#include "Client_Defines.h"
#include "UI_Parents.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CVIBuffer;
END

BEGIN(Client)

class CUI_Minigame final : public CUI_Parents
{
public:
	CUI_Minigame(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_Minigame(const CUI_Minigame& rhs);
	virtual ~CUI_Minigame() = default;

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
	CVIBuffer*					m_pVIBufferCom = nullptr;

	_int iCom_Texture = 0;
	_int Score = 0;

	_double		dTime = 0;
	_double		dSpeedTime = 0;
	_float		fSpeed = 0.f;
	_bool		bAni = false;
	_bool		bShake = false;


	_bool		m_bEnd = false;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CUI_Minigame* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END