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

class CUI_Chat final : public CUI_Parents
{
public:
	enum EVENT { EVENT_QUEST, EVENT_QUESTCLEAR, EVENT_END };

	typedef struct tagUIChatDesc
	{
		EVENT	eEvent = EVENT_END;
	}CHATDESC;


private:
	CUI_Chat(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_Chat(const CUI_Chat& rhs);
	virtual ~CUI_Chat() = default;

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
	EVENT					m_eEvent;
	_uint					m_iTexIndex = 0;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CUI_Chat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END