#pragma once

#include "Client_Defines.h"
#include "Level.h"

/* 로더를 개체화하여 보관하낟. */

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
public:
	explicit CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Loading() = default;
public:
	virtual HRESULT NativeConstruct(LEVEL eNextLevel);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	class CLoader*		m_pLoader = nullptr;
	LEVEL				m_eNextLevel = LEVEL_END;

private:
	_int				iCom_Texture_Set = 0;
private:
	HRESULT Ready_Layer_LoadingUI(const _tchar * pLayerTag, LEVEL eNextLevel);

public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVEL eNextLevel);
	virtual void Free() override;
};

END