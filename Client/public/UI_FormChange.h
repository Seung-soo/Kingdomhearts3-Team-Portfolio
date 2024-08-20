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
class CUI_FormChange final : public CUI_Parents
{
private:
	CUI_FormChange(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_FormChange(const CUI_FormChange& rhs);
	virtual ~CUI_FormChange() = default;

public:
	void	Set_Case0(_bool _bCase0) { 
		bCase0 = _bCase0;
		fSpeedX = 10.f;
	}

	void Set_CounterAttackUI(_double _lifeTime, _double _timeSpeed);


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

	_double m_dCounterAttackUI_LifeTime = 0.0;
	_double m_dCounterAttackUI_TimeSpeed = 0.0;

	_double		dTime = 0.0;
	_float		fSpeedX = 10.f;
	_bool		bCase0 = false;

	_float		fSize = 0.f;

	_float		fFade = 0.f;

	CGameObject*	m_pLightUI = nullptr;

public:
	static CUI_FormChange* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END