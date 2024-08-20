#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	explicit CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel() = default;
public:
	virtual HRESULT		NativeConstruct();
	virtual _int		Tick(_double dTimeDelta);
	virtual _int		LateTick(_double dTimeDelta);
	virtual HRESULT		Render();
protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
protected:
	class CGameInstance*	m_pGameInstance = nullptr;
public:
	virtual void	Free() override;
};

END