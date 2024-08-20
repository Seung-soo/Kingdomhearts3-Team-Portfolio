#pragma once

#include "Client_Defines.h"
#include "UI_Parents.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_BackSimbol final : public CUI_Parents
{
private:
	CUI_BackSimbol(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_BackSimbol(const CUI_BackSimbol& rhs);
	virtual ~CUI_BackSimbol() = default;

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

private:
	_int	RandomMoveX = 0;
	_int	RandomMoveY = 0;

	_float	fRotate = 0.f;

	_int	iFrame = 0;



public:
	static CUI_BackSimbol* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END
