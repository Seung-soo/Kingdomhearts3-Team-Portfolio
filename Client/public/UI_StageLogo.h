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

class CUI_StageLogo final : public CUI_Parents
{
private:
	CUI_StageLogo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_StageLogo(const CUI_StageLogo& rhs);
	virtual ~CUI_StageLogo() = default;

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
	_double			dTime = 0.0;
	_float			SizeSpeed = 1.f;

	_bool			bFrame = false;
	_bool			bLogoPatten = false;

	_float			fFade = 0.f;

	//	대기시간
	_float			m_fWaitingTime = 3.f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CUI_StageLogo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END