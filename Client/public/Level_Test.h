#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CLevel_Test final : public CLevel
{
public:
	explicit CLevel_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Test() = default;
public:
	virtual HRESULT NativeConstruct();
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	_int iCom_Texture_Set = 0;
	CGameObject* m_pPlayer = nullptr;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_GameObjects();

public:
	static CLevel_Test* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END