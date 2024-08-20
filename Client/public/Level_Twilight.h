#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Engine)
class CRenderer;
class CGameObject;
END

BEGIN(Client)

class CLevel_Twilight final : public CLevel
{
public:
	explicit CLevel_Twilight(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Twilight() = default;
public:
	virtual HRESULT NativeConstruct();
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_GameObjects();
	HRESULT Ready_Layer_UI();

	HRESULT Ready_Tutorial_UI();
private:
	_int				iCom_Texture_Set = 0;
	_double				dItem_Time       = 0.0;

	_double				dGuideTime     = 0.0;
	_int				iGuideCreatCnt = 0;

	_vector				vGuide_Position;
	_int				iGuide_Pattern = 0;

	_uint				m_iCameraSceneIndex = 0;

	class CRenderer*	m_pRenderer = nullptr;

	_bool				m_bCreateSecondTrigger = false;

	CGameObject*	m_pFade = nullptr;

	_float				m_fBGMTime = 2.f;
public:
	static CLevel_Twilight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END