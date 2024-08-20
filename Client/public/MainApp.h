#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CRenderer;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;
public:
	HRESULT NativeConstruct();
	_int	Tick(_double dTimeDelta);
	HRESULT Render(_double dTimeDelta);
public:
	HRESULT Open_Level(LEVEL eLevelIndex);
private:
	CGameInstance*			m_pGameInstance = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

private:
	HRESULT Ready_Prototype_Components();
	HRESULT Ready_Prototype_GameObjects();

public:
	static CMainApp*		Create();
	virtual void			Free() override;
};

END