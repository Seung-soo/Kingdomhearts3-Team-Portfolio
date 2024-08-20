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
class CUI_MoneyBar final : public CUI_Parents
{
private:
	CUI_MoneyBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_MoneyBar(const CUI_MoneyBar& rhs);
	virtual ~CUI_MoneyBar() = default;

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

	_float		fSpeed_A = 10.f;
	_float		fSpeed_B = 10.f;

	_float		fMoneyFadeOut = 0.f;

public:
	static CUI_MoneyBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
