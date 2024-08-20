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

class CUI_Number final : public CUI_Parents
{
public:
	CUI_Number(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_Number(const CUI_Number& rhs);
	virtual ~CUI_Number() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	// push ===
	_int					iPush_Num = 0;
	_float					fPush_Size = 0.f;
	_float					fGap = 0.f;

	// Num_Font ===
	_int					iNumber = 0;
	vector<_int>			vFont;

	_float					fSize_Position = 0.f;

	_double					dTime = 0.0;

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CUI_Number* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END