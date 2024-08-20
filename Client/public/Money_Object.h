#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CLevel;
class CNavigation;
END

BEGIN(Client)

class CMoney_Object final : public CGameObject
{
private:
	CMoney_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMoney_Object(const CMoney_Object& rhs);
	virtual ~CMoney_Object() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CNavigation*		m_pNavigationCom = nullptr;

	/* === */
	_double		dTime = 0.0;
	_bool		bPattern = false;

	_double		dSpeedTime = 0.0;
	_float		fSpeed = 1.f;

	_double		dSpeedTime2 = 0.0;
	_float		fSpeed2 = 0.5f;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	_bool	bChaseOn = false;

public:
	static CMoney_Object* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};


END
