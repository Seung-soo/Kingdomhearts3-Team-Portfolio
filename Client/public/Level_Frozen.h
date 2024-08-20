#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)

class CLevel_Frozen final : public CLevel
{
public:
	explicit CLevel_Frozen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Frozen() = default;
public:
	virtual HRESULT NativeConstruct();
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_GameObjects();
	HRESULT Ready_Layer_UI();

private:
	CRenderer* m_pRenderer = nullptr;
	_int		m_iLUTIndexSecond = 0;
public:
	static CLevel_Frozen* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END