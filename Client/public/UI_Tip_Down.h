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

class CUI_Tip_Down final : public CUI_Parents
{
private:
	CUI_Tip_Down(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_Tip_Down(const CUI_Tip_Down& rhs);
	virtual ~CUI_Tip_Down() = default;

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

	_double		dTime = 0.0;
	_float		fSpeedY = 0.f;
	_float		m_fWaitingTime = 0.f;
	_float		m_fWaitingTime2 = 0.f;
	_bool		m_bWaiting = false;

	_float		m_fTutoWait = 0.f;
	_bool		m_bTuto = false;
private:
	void		Tutorial(_double TimeDelta);
public:
	static CUI_Tip_Down* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END