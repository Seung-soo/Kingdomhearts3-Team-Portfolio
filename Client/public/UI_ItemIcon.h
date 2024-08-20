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

class CUI_ItemIcon final : public CUI_Parents
{
private:
	CUI_ItemIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_ItemIcon(const CUI_ItemIcon& rhs);
	virtual ~CUI_ItemIcon() = default;

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

private:
	UI_SETTING			m_UI_Setting;

public:
	static CUI_ItemIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END