#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLEVEL_TWILIGHT final : public CLevel
{
public:
	explicit CLEVEL_TWILIGHT(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLEVEL_TWILIGHT() = default;
public:
	virtual HRESULT NativeConstruct();
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_GameObjects();
	HRESULT Ready_Layer_UI();
	HRESULT Ready_Layer_Effect();

private:
	_int iCom_Texture_Set = 0;

public:
	static CLEVEL_TWILIGHT* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END