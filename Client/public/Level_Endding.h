#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Ending final : public CLevel
{
public:
	explicit CLevel_Ending(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Ending() = default;
public:
	virtual HRESULT NativeConstruct();
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Prototype_GameObject();

private:
	
	_int	iCom_Texture = 0;
	_double	dTime = 0;
	_bool	GameStart = false;

	void	Boss1(_double TimeDelta);
	void	Boss2(_double TimeDelta);
	void	Boss3(_double TimeDelta);
	void	Boss4(_double TimeDelta);
	void	Boss5(_double TimeDelta);
	_bool	Boss1_Creat = false;
	_bool	Boss2_Creat = false;
	_bool	Boss3_Creat = false;
	_bool	Boss4_Creat = false;
	_bool	Boss5_Creat = false;


	_double	EndCreateTime = 0;


public:
	static CLevel_Ending* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END