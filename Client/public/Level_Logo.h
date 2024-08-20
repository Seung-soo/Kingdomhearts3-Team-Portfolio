#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Logo final : public CLevel
{
public:
	explicit CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Logo() = default;
public:
	virtual HRESULT NativeConstruct();
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Prototype_GameObject();

private:
	_int iCom_Texture_Set = 0;
	_int iSelect_Point = 0;

	_double	dLoadingTime = 0.0;
	_bool	bLoading = false;

	_bool	bBackSimbol = false;
	_int	iBackSimbolCnt = 0;

	_bool	bBlackCreate = false;


public:
	static CLevel_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END