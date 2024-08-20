#pragma once

#include"Client_Defines.h"
#include"UI_Parents.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer_Rect;
class CShader;
class CTexture;

// 플레이어가 가지고있는 컴포넌트 가져오기 위한 CStatus 컴포넌트 전방선언
class CStatus;
END

BEGIN(Client)

class CUI_RageGauge final : public CUI_Parents
{
private:
	CUI_RageGauge(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_RageGauge(const CUI_RageGauge& rhs);
	virtual ~CUI_RageGauge() = default;

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

	// Player=
	class CPlayer* m_pPlayer = nullptr;
	class CPlayer_Weapon* m_pPlayerWeapon = nullptr;
	CStatus* m_pPlayerStatus = nullptr;

	_float		fMAX_Gauge = 0.f;
	_float		fGauge = 0.f;
	_float		fSetGauge_First = 0.f;
	_float		fSetGauge_Second = 0.f;

	_bool		bRageGauge = false;


public:
	static CUI_RageGauge* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END