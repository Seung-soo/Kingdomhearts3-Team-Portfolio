#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)

class CLevel_RapunzelBoss final : public CLevel
{
public:
	explicit CLevel_RapunzelBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_RapunzelBoss() = default;
public:
	virtual HRESULT NativeConstruct();
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_GameObjects();
	HRESULT Ready_Layer_UI();
	
	void	MiniGame(_double TimeDelta);

private:
	_int iCom_Texture_Set = 0;
	_int m_iLUTIndexSecond = 96;
	CRenderer* m_pRenderer = nullptr;

	/* Minigame */
	_double dTime = 0;
	_int	iRandom = 0;

public:
	static CLevel_RapunzelBoss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END